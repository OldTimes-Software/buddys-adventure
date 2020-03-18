/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "act.h"

#define PLAYER_VIEW_OFFSET  75.0f

#define PLAYER_TURN_SPEED    4.5f
#define PLAYER_FORWARD_SPEED 10.0f
#define PLAYER_MAX_VELOCITY  PLAYER_FORWARD_SPEED
#define PLAYER_MIN_VELOCITY  0.5f

void Player_Spawn( Actor *self ) {
	Act_SetViewOffset( self, PLAYER_VIEW_OFFSET );
}

void Player_Tick( Actor *self, void *userData ) {
	float nAngle = Act_GetAngle( self );
	if ( Sys_GetInputState( YIN_INPUT_LEFT ) ) {
		nAngle -= PLAYER_TURN_SPEED;
	} else if ( Sys_GetInputState( YIN_INPUT_RIGHT ) ) {
		nAngle += PLAYER_TURN_SPEED;
	}
	Act_SetAngle( self, nAngle );

	float forwardVelocity = 0.0f;
	if ( Sys_GetInputState( YIN_INPUT_UP ) ) {
		forwardVelocity = 1.0f;
	} else if ( Sys_GetInputState( YIN_INPUT_DOWN ) ) {
		forwardVelocity = -1.0f;
	}

	PLVector3 curVelocity = Act_GetVelocity( self );
	if ( curVelocity.x > 0.0f ) { curVelocity.x -= 0.5f; }
	else if ( curVelocity.x < 0.0f ) { curVelocity.x += 0.5f; }
	if ( curVelocity.z > 0.0f ) { curVelocity.z -= 0.5f; }
	else if ( curVelocity.z < 0.0f ) { curVelocity.z += 0.5f; }

	PLVector3 forward = Act_GetForward( self );
	if( forwardVelocity > 0 ) {
		curVelocity = plAddVector3( curVelocity, plScaleVector3f( forward, forwardVelocity ) );
	} else if ( forwardVelocity < 0 ) {
		curVelocity = plSubtractVector3( curVelocity, plScaleVector3f( forward, forwardVelocity ) );
	}

	curVelocity.x = plClamp( -PLAYER_MAX_VELOCITY, curVelocity.x, PLAYER_MAX_VELOCITY );
	curVelocity.z = plClamp( -PLAYER_MAX_VELOCITY, curVelocity.z, PLAYER_MAX_VELOCITY );

	/* clamp the velocity as necessary */
	if ( curVelocity.x < PLAYER_MIN_VELOCITY && curVelocity.x > -PLAYER_MIN_VELOCITY ) {
		curVelocity.x = 0.0f;
	}
	if ( curVelocity.z < PLAYER_MIN_VELOCITY && curVelocity.z > -PLAYER_MIN_VELOCITY ) {
		curVelocity.z = 0.0f;
	}

	PrintMsg( "V: %s\n", plPrintVector3( &curVelocity, pl_int_var ) );

	PLVector3 curPosition = Act_GetPosition( self );
	curPosition = plAddVector3( curPosition, curVelocity );

	Act_SetPosition( self, &curPosition );
	Act_SetVelocity( self, &curVelocity );
}
