/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "act.h"

#define PLAYER_MAX_VELOCITY 10.0f

void Player_Tick( Actor *self, void *userData ) {
	PLVector3 curVelocity = Act_GetVelocity( self );
	/* clamp the velocity as necessary */
	if ( curVelocity.x < 2.0f && curVelocity.x > -2.0f ) {
		curVelocity.x = 0.0f;
	} else if ( curVelocity.x > PLAYER_MAX_VELOCITY ) {
		curVelocity.x = PLAYER_MAX_VELOCITY;
	} else if ( curVelocity.x < -PLAYER_MAX_VELOCITY ) {
		curVelocity.x = -PLAYER_MAX_VELOCITY;
	}

	PLVector3 curPosition = Act_GetPosition( self );
	curPosition = plAddVector3( curPosition, curVelocity );

	Act_SetPosition( self, &curPosition );
	Act_SetVelocity( self, &curVelocity );
}
