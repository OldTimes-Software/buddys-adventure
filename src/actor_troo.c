/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "act.h"
#include "gfx.h"

#define TROO_NUM_WALK_FRAMES 32

typedef struct ATroo {
	GfxAnimationFrame *walkFrames[ TROO_NUM_WALK_FRAMES ];
} ATroo;

void Troo_Draw( Actor *self, void *userData ) {
	if ( Act_GetType( self ) != ACTOR_TROO ) {
		return;
	}

	ATroo *trooData = ( ATroo* ) userData;
	Gfx_DrawAnimationFrame( trooData->walkFrames[ 4 ], Act_GetPosition( self ) );
}

void Troo_Spawn( Actor *self ) {
	ATroo *trooData = Sys_AllocateMemory( 1, sizeof( ATroo ) );
	Act_SetUserData( self, trooData );

	/* now to load in our sprite data... */
	static const char *walkFrameNames[] = {
			"TROOA1", "TROOA2", "TROOA3", "TROOA4", "TROOA5", "TROOA6", "TROOA7", "TROOA8",
			"TROOB1", "TROOB2", "TROOB3", "TROOB4", "TROOB5", "TROOB6", "TROOB7", "TROOB8",
			"TROOC1", "TROOC2", "TROOC3", "TROOC4", "TROOC5", "TROOC6", "TROOC7", "TROOC8",
			"TROOD1", "TROOD2", "TROOD3", "TROOD4", "TROOD5", "TROOD6", "TROOD7", "TROOD8",
	};
	Gfx_LoadAnimationFrames( walkFrameNames, trooData->walkFrames, TROO_NUM_WALK_FRAMES );
}
