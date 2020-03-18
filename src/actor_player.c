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

typedef struct APlayer {
	PLVector3 ulViewPos;
	PLVector3 urViewPos;
	PLVector3 llViewPos;
	PLVector3 lrViewPos;

	PLVector3 cViewPos; /* center */
} APlayer;

static void Player_CalculateViewFrustum( Actor *self ) {
	APlayer *playerData = Act_GetUserData( self );

	PLVector3 forward, left;
	plAnglesAxes( PLVector3( 0, Act_GetAngle( self ), 0 ), &left, NULL, &forward );

	PLVector3 curPos = Act_GetPosition( self );
	playerData->cViewPos = plAddVector3( curPos, plScaleVector3f( forward, 2048.0f ) );
}

/**
 * Ensure the 2D point provided is within the player's field of view 
 */
bool Player_IsPointVisible( Actor *self, const PLVector2 *point ) {
	if ( Act_GetType( self ) != ACTOR_PLAYER ) {
		return false;
	}

	APlayer *playerData = Act_GetUserData( self );
	if ( playerData == NULL ) {
		return false;
	}

	PLVector3 testPos = PLVector3( point->x, 0.0f, point->y );

	PLVector3 forward = Act_GetForward( self );
	PLVector3 curPos = Act_GetPosition( self );


	
	return true;
}

void Player_Spawn( Actor *self ) {
	Act_SetViewOffset( self, PLAYER_VIEW_OFFSET );

	APlayer* playerData = Sys_AllocateMemory( 1, sizeof( APlayer ) );
	Act_SetUserData( self, playerData );

	Player_CalculateViewFrustum( self );
}

void Player_Tick( Actor *self, void *userData ) {
	float nAngle = Act_GetAngle( self );
	if ( Sys_GetInputState( YIN_INPUT_LEFT ) ) {
		nAngle += PLAYER_TURN_SPEED;
	} else if ( Sys_GetInputState( YIN_INPUT_RIGHT ) ) {
		nAngle -= PLAYER_TURN_SPEED;
	}
	Act_SetAngle( self, nAngle );

	float forwardVelocity = 0.0f;
	if ( Sys_GetInputState( YIN_INPUT_UP ) ) {
		forwardVelocity = 1.0f;
	} else if ( Sys_GetInputState( YIN_INPUT_DOWN ) ) {
		forwardVelocity = -1.0f;
	}

	PLVector3 curVelocity = Act_GetVelocity( self );
	//plSubtractVector3( curVelocity, PLVector3( 0.1f, 0.0f, 0.1f ) );

	PLVector3 forward = Act_GetForward( self );
	curVelocity = plAddVector3( curVelocity, plScaleVector3f( forward, forwardVelocity ) );

	/* clamp the velocity as necessary */
	//curVelocity.x = plClamp( -PLAYER_MAX_VELOCITY, curVelocity.x, PLAYER_MAX_VELOCITY );
	//curVelocity.z = plClamp( -PLAYER_MAX_VELOCITY, curVelocity.z, PLAYER_MAX_VELOCITY );
	//if ( curVelocity.x < PLAYER_MIN_VELOCITY && curVelocity.x > -PLAYER_MIN_VELOCITY ) {
	//	curVelocity.x = 0.0f;
	//}
	//if ( curVelocity.z < PLAYER_MIN_VELOCITY && curVelocity.z > -PLAYER_MIN_VELOCITY ) {
	//	curVelocity.z = 0.0f;
	//}

	PrintMsg( "V: %s\n", plPrintVector3( &curVelocity, pl_int_var ) );

	PLVector3 curPosition = Act_GetPosition( self );
	curPosition = plAddVector3( curPosition, curVelocity );

	Act_SetPosition( self, &curPosition );
	Act_SetVelocity( self, &curVelocity );

	Player_CalculateViewFrustum( self );
}
