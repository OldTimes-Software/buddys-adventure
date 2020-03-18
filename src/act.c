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
	Gfx_DrawAxesPivot( Act_GetPosition( self ), PLVector3( 0, 0, 0 ) );
}

void Player_Spawn( Actor *self );
void Player_Tick( Actor *self, void *userData );

ActorSetup actorSpawnSetup[ MAX_ACTOR_TYPES ] = {
		[ ACTOR_NONE   ] = { NULL, NULL, NULL, NULL },
		[ ACTOR_PLAYER ] = { Player_Spawn, Player_Tick, NULL, NULL },
		[ ACTOR_BOSS   ] = { NULL, NULL, Act_DrawBasic, NULL },
		[ ACTOR_SARG   ] = { NULL, NULL, Act_DrawBasic, NULL },
		[ ACTOR_TROO   ] = { NULL, NULL, Act_DrawBasic, NULL },
};

typedef struct Actor {
	PLVector3        position;
	PLVector3        velocity;
	float            angle;
	float            viewOffset;
	unsigned int     curArea;
	ActorType        type;
	ActorSetup       setup;
	PLLinkedListNode *node;
	void             *userData;
} Actor;

PLLinkedList *actorList;

Actor *Act_SpawnActor( ActorType type, PLVector3 position, float angle ) {
	Actor *actor = Sys_AllocateMemory( 1, sizeof( Actor ) );
	actor->node = plInsertLinkedListNode( actorList, actor );
	actor->setup = actorSpawnSetup[ type ];
	actor->position = position;
	actor->angle = angle;

	if ( actor->setup.Spawn != NULL ) {
		actor->setup.Spawn( actor );
	}

	PrintMsg( "Actor has spawned!\n"
			  "xPos: %d\nyPos: %d\ntype: %d\nflags: %d\n",
			  (int) actor->position.x, (int) actor->position.z, type );

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

void      Act_SetPosition( Actor *self, const PLVector3 *position ) { self->position = *position; }
PLVector3 Act_GetPosition( const Actor *self ) { return self->position; }
void      Act_SetVelocity( Actor *self, const PLVector3 *velocity ) { self->velocity = *velocity; }
PLVector3 Act_GetVelocity( const Actor *self ) { return self->velocity; }
void      Act_SetAngle( Actor *self, float angle ) { self->angle = angle; }
float     Act_GetAngle( const Actor *self ) { return self->angle; }
void      Act_SetViewOffset( Actor *self, float viewOffset ) { self->viewOffset = viewOffset; }
float     Act_GetViewOffset( Actor *self ) { return self->viewOffset; }

PLVector3 Act_GetForward( const Actor *self ) {
	PLVector3 forward;
	plAnglesAxes( PLVector3( 0, self->angle, 0 ), NULL, NULL, &forward );
	return forward;
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

