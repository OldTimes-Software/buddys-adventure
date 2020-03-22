/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "act.h"
#include "gfx.h"

#define SARG_NUM_WALK_FRAMES 32

typedef struct ASarg {
	GfxAnimationFrame *walkFrames[ SARG_NUM_WALK_FRAMES ];
} ASarg;

void Sarg_Spawn( Actor *self ) {
	ASarg *sargData = Sys_AllocateMemory( 1, sizeof( ASarg ) );
	Act_SetUserData( self, sargData );

	/* now to load in our sprite data... */
	static const char *walkFrameNames[] = {
			"SARGA1", "SARGA2", "SARGA3", "SARGA4", "SARGA5", "SARGA6", "SARGA7", "SARGA8",
			"SARGB1", "SARGB2", "SARGB3", "SARGB4", "SARGB5", "SARGB6", "SARGB7", "SARGB8",
			"SARGC1", "SARGC2", "SARGC3", "SARGC4", "SARGC5", "SARGC6", "SARGC7", "SARGC8",
			"SARGD1", "SARGD2", "SARGD3", "SARGD4", "SARGD5", "SARGD6", "SARGD7", "SARGD8",
	};
	Gfx_LoadAnimationFrames( walkFrameNames, sargData->walkFrames, SARG_NUM_WALK_FRAMES );
}
