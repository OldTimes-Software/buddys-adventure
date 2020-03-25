/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "act.h"
#include "gfx.h"

#define BOSS_NUM_WALK_FRAMES 32

typedef struct ABoss {
	GfxAnimationFrame *walkFrames[ BOSS_NUM_WALK_FRAMES ];
} ABoss;

void Boss_Draw( Actor *self, void *userData ) {
	if ( Act_GetType( self ) != ACTOR_BOSS ) {
		return;
	}

	ABoss *bossData = ( ABoss* ) userData;
	Gfx_DrawAnimationFrame( bossData->walkFrames[ 0 ], Act_GetPosition( self ) );
}

void Boss_Spawn( Actor *self ) {
	ABoss *bossData = Sys_AllocateMemory( 1, sizeof( ABoss ) );
	Act_SetUserData( self, bossData );

	/* now to load in our sprite data... */
	static const char *walkFrameNames[] = {
			"BOSSA1", "BOSSA2", "BOSSA3", "BOSSA4", "BOSSA5", "BOSSA6", "BOSSA7", "BOSSA8",
			"BOSSB1", "BOSSB2", "BOSSB3", "BOSSB4", "BOSSB5", "BOSSB6", "BOSSB7", "BOSSB8",
			"BOSSC1", "BOSSC2", "BOSSC3", "BOSSC4", "BOSSC5", "BOSSC6", "BOSSC7", "BOSSC8",
			"BOSSD1", "BOSSD2", "BOSSD3", "BOSSD4", "BOSSD5", "BOSSD6", "BOSSD7", "BOSSD8",
	};
	Gfx_LoadAnimationFrames( walkFrameNames, bossData->walkFrames, BOSS_NUM_WALK_FRAMES );
}
