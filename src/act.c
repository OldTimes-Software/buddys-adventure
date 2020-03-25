/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include <PL/pl_llist.h>

#include "yin.h"
#include "act.h"
#include "gfx.h"

typedef struct ActorSetup {
	void (*Spawn)( struct Actor *self );
	void (*Tick)( struct Actor *self, void *userData );
	void (*Draw)( struct Actor *self, void *userData );
	void (*Destroy)( struct Actor *self, void *userData );
} ActorSetup;

void Act_DrawBasic( Actor *self, void *userData ) {
	Gfx_EnableShaderProgram( SHADER_GENERIC );
	Gfx_DrawAxesPivot( Act_GetPosition( self ), PLVector3( 0, 0, 0 ) );
}

void Boss_Spawn( Actor *self );
void Boss_Draw( Actor *self, void *userData );
void Troo_Spawn( Actor *self );
void Troo_Draw( Actor *self, void *userData );
void Sarg_Spawn( Actor *self );

void Player_Spawn( Actor *self );
void Player_Tick( Actor *self, void *userData );

ActorSetup actorSpawnSetup[ MAX_ACTOR_TYPES ] = {
		[ ACTOR_NONE   ] = { NULL, NULL, NULL, NULL },
		[ ACTOR_PLAYER ] = { Player_Spawn, Player_Tick, Act_DrawBasic, NULL },
		[ ACTOR_BOSS   ] = { Boss_Spawn, NULL, Boss_Draw, NULL },
		[ ACTOR_SARG   ] = { Sarg_Spawn, NULL, Act_DrawBasic, NULL },
		[ ACTOR_TROO   ] = { Troo_Spawn, NULL, Troo_Draw, NULL },
};

typedef struct Actor {
	PLVector3 position;
	PLVector3 velocity;
	PLVector3 forward;
	float     angle;
	float     viewOffset;
	PLAABB    bounds;

	unsigned int currentFrame;

	ActorType  type;
	ActorSetup setup;

	PLLinkedListNode *node;
	void             *userData;
} Actor;

PLLinkedList *actorList;

Actor *Act_SpawnActor( ActorType type, PLVector3 position, float angle ) {
	Actor *actor = Sys_AllocateMemory( 1, sizeof( Actor ) );
	actor->node     = plInsertLinkedListNode( actorList, actor );
	actor->setup    = actorSpawnSetup[ type ];
	actor->type     = type;
	actor->position = position;
	actor->angle    = angle;

	/* give everything a set of basic bounds */
	actor->bounds.maxs = PLVector3( 8.0f, 16.0f, 8.0f );
	actor->bounds.mins = PLVector3( -8.0f, 0.0f, -8.0f );

	if ( actor->setup.Spawn != NULL ) {
		actor->setup.Spawn( actor );
	}

	return actor;
}

Actor *Act_DestroyActor( Actor *self ) {
	if ( self->setup.Destroy != NULL) {
		self->setup.Destroy( self, self->userData );
	}

	plDestroyLinkedListNode( actorList, self->node );
	free( self->userData );
	free( self );
	return NULL;
}

ActorType Act_GetType( const Actor *self ) { return self->type; }
void      Act_SetPosition( Actor *self, const PLVector3 *position ) { self->position = *position; }
PLVector3 Act_GetPosition( const Actor *self ) { return self->position; }
void      Act_SetVelocity( Actor *self, const PLVector3 *velocity ) { self->velocity = *velocity; }
PLVector3 Act_GetVelocity( const Actor *self ) { return self->velocity; }
void      Act_SetAngle( Actor *self, float angle ) { self->angle = angle; }
float     Act_GetAngle( const Actor *self ) { return self->angle; }
void      Act_SetViewOffset( Actor *self, float viewOffset ) { self->viewOffset = viewOffset; }
float     Act_GetViewOffset( Actor *self ) { return self->viewOffset; }
void      Act_SetUserData( Actor *self, void *userData ) { self->userData = userData; }
void      *Act_GetUserData( Actor *self ) { return self->userData; }

void Act_SetBounds( Actor *self, PLVector3 mins, PLVector3 maxs ) {
	if( mins.x > maxs.x || mins.y > maxs.y || mins.z > maxs.z ) {
		PrintError( "Invalid bounds for actor (mins %s, maxs %s)!\n", plPrintVector3( &mins, pl_int_var ), plPrintVector3( &maxs, pl_int_var ) );
	}

	self->bounds.maxs = maxs;
	self->bounds.mins = mins;
}

const PLAABB *Act_GetBounds( Actor *self ) {
	return &self->bounds;
}

PLVector3 Act_GetForward( const Actor *self ) {
	return self->forward;
}

void Act_SpawnActors( void ) {
	PrintMsg( "Spawning actors...\n" );

	PLFile *filePtr = plLoadPackageFile( globalWad, "M_THINGS" );
	if ( filePtr == NULL) {
		PrintError( "Failed to find \"M_THINGS\" block!\nPL: %s\n", plGetError());
	}

	bool status;
	uint32_t numThings = plReadInt32( filePtr, false, &status );
	if ( !status ) {
		PrintError( "Failed to get number of things!\nPL: %s\n", plGetError());
	}

	for ( unsigned int i = 0; i < numThings; ++i ) {
		struct {
			int16_t xPos;
			int16_t yPos;
			uint16_t type;
			uint16_t flags;
		} thing;

		PrintMsg( "Spawning actor %d/%d...\n", i + 1, numThings );

		/* these are intentionally flipped... */
		thing.yPos = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 ) * 2;
		thing.xPos = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 ) * 2;
		thing.type = ( uint16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
		thing.flags = ( uint16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );

		if ( !status ) {
			PrintError( "Failed to get thing data!\nPL: %s\n", plGetError());
		}

		Act_SpawnActor( thing.type, PLVector3( thing.xPos, 0, thing.yPos ), 0.0f );
	}
}

void Act_DisplayActors( void ) {
	PLLinkedListNode *curNode = plGetRootNode( actorList );
	while ( curNode != NULL ) {
		Actor *actor = plGetLinkedListNodeUserData( curNode );
		if ( actor == NULL ) {
			PrintError( "Invalid actor data in node!\n" );
		}

		if ( actor->setup.Draw ) {
			actor->setup.Draw( actor, actor->userData );
		}

		curNode = plGetNextLinkedListNode( curNode );
	}
}

void Act_TickActors( void ) {
	PLLinkedListNode *curNode = plGetRootNode( actorList );
	while ( curNode != NULL ) {
		Actor *actor = plGetLinkedListNodeUserData( curNode );
		if ( actor == NULL ) {
			PrintError( "Invalid actor data in node!\n" );
		}

		if ( actor->setup.Tick ) {
			actor->setup.Tick( actor, actor->userData );
		}

		plAnglesAxes( PLVector3( 0, actor->angle, 0 ), NULL, NULL, &actor->forward );

		curNode = plGetNextLinkedListNode( curNode );
	}
}

void Act_Initialize( void ) {
	PrintMsg( "Initializing Act...\n" );

	actorList = plCreateLinkedList();
	if ( actorList == NULL) {
		PrintError( "Failed to create actor list!\nPL: %s\n", plGetError());
	}
}

void Act_Shutdown( void ) {
	plDestroyLinkedList( actorList );
}
