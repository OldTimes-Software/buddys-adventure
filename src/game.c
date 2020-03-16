/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "game.h"
#include "act.h"
#include "gfx.h"
#include "map.h"

/* game specific implementation goes here! */

typedef enum InputTarget {
	INPUT_TARGET_MENU, /* menu mode */
	INPUT_TARGET_GAME, /* game mode */
} InputTarget;
static InputTarget inputTarget = INPUT_TARGET_MENU;
static MenuState menuState = MENU_STATE_START;

MenuState Gam_GetMenuState( void ) {
	return menuState;
}

Actor *playerActor = NULL;

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

Actor *Gam_GetPlayer( void ) {
	return playerActor;
}

void Gam_Start( void ) {
	menuState = MENU_STATE_HUD;
	inputTarget = INPUT_TARGET_GAME;

	Map_Load( globalWad ); /* load the map from the global wad */

	Act_SpawnActors();

	/* spawn the player in */
	playerActor = Act_SpawnActor( ACTOR_PLAYER, PLVector3( 0, 0, 0 ), 0.0f, NULL );
}

void Gam_End( void ) {}

void Gam_Tick( void ) {
	if ( inputTarget == INPUT_TARGET_MENU ) {
		switch( menuState ) {
			case MENU_STATE_START:
				/* if any key was hit here, just switch to the game */
				Gam_Start();
				break;
			default:
			PrintError( "Unhandled menu state, %d!\n", menuState );
		}
		return;
	}

	float forwardVelocity = 0.0f;
	if ( Sys_GetInputState( YIN_INPUT_UP ) ) {
		forwardVelocity = 2.0f;
	} else if ( Sys_GetInputState( YIN_INPUT_DOWN ) ) {
		forwardVelocity = -2.0f;
	}

	float nAngle = Act_GetAngle( playerActor );
	if ( Sys_GetInputState( YIN_INPUT_LEFT ) ) {
		nAngle -= 5.0f;
	} else if ( Sys_GetInputState( YIN_INPUT_RIGHT ) ) {
		nAngle += 5.0f;
	}

#if 0
	PLVector3 curVelocity = Act_GetVelocity( playerActor );

	curVelocity.x += forwardVelocity;

	curVelocity.x -= 1.0f;
	if ( curVelocity.x < 2.0f && curVelocity.x > -2.0f ) {
		curVelocity.x = 0.0f;
	} else if ( curVelocity.x > PLAYER_MAX_VELOCITY ) {
		curVelocity.x = PLAYER_MAX_VELOCITY;
	} else if ( curVelocity.x < -PLAYER_MAX_VELOCITY ) {
		curVelocity.x = -PLAYER_MAX_VELOCITY;
	}

	curVelocity.z -= 1.0f;
	if ( curVelocity.z < 2.0f && curVelocity.z > -2.0f ) {
		curVelocity.z = 0.0f;
	} else if ( curVelocity.z > PLAYER_MAX_VELOCITY ) {
		curVelocity.z = PLAYER_MAX_VELOCITY;
	} else if ( curVelocity.z < -PLAYER_MAX_VELOCITY ) {
		curVelocity.z = -PLAYER_MAX_VELOCITY;
	}

	PLVector3 forward = Act_GetForward( playerActor );

	PLVector3 nPosition = Act_GetPosition( playerActor );
	nPosition.x += curVelocity.x * 100.0f * forward.x;
	nPosition.y += curVelocity.y * 100.0f * forward.y;
	nPosition.z += curVelocity.z * 100.0f * forward.z;
#else
	PLVector3 forward = Act_GetForward( playerActor );
	PLVector3 nPosition = Act_GetPosition( playerActor );
	nPosition.x -= ( -forwardVelocity * forward.z ) * 5.0f;
	nPosition.z += ( forwardVelocity * forward.x ) * 5.0f;
#endif

	Act_SetAngle( playerActor, nAngle );
	Act_SetPosition( playerActor, &nPosition );

	Act_TickActors();
}

void Gam_Initialize( void ) {}

void Gam_Shutdown( void ) {}
