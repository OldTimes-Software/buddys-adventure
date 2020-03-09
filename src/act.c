/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include <PL/pl_llist.h>

#include "yin.h"
#include "act.h"

typedef struct Actor {
	PLVector3        position;
	float            angle;
	PLLinkedListNode *node;
	void             *userData;
	void             (*Spawn)( struct Actor *self );
	void             (*Destroy)( struct Actor *self );
} Actor;

PLLinkedList *actorList;

Actor *Act_SpawnActor( PLVector3 position, float angle, void *userData ) {
	Actor *actor = malloc( sizeof( Actor ));
	actor->userData = userData;
	actor->node = plInsertLinkedListNode( actorList, actor );
	return actor;
}

Actor *Act_DestroyActor( Actor *self ) {
	if ( self->Destroy != NULL) {
		self->Destroy( self );
	}

	plDestroyLinkedListNode( actorList, self->node );
	free( self->userData );
	free( self );
	return NULL;
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

		/* for the sake of padding, they're stored as 32bit integers
		 * in the file (which changed in later doom versions...) */
		thing.xPos = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
		thing.yPos = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
		thing.type = ( uint16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
		thing.flags = ( uint16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );

		if ( !status ) {
			PrintError( "Failed to get thing data!\nPL: %s\n", plGetError());
		}

		Act_SpawnActor(PLVector3( thing.xPos, thing.yPos, 0 ), 0.0f, NULL);

		PrintMsg( "Actor %d has spawned!\n"
				  "xPos: %d\nyPos: %d\ntype: %d\nflags: %d\n",
				  i + 1,
				  thing.xPos, thing.yPos, thing.type, thing.flags );
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

