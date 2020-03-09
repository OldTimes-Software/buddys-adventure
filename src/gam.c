
#include "yin.h"
#include "gam.h"
#include "act.h"

/* game specific implementation goes here! */

typedef enum InputTarget {
	INPUT_TARGET_MENU, /* menu mode */
	INPUT_TARGET_GAME, /* game mode */
} InputTarget;
static InputTarget inputTarget = INPUT_TARGET_MENU;

typedef enum MenuState {
	MENU_STATE_START, /* draw start screen */
	MENU_STATE_HUD,   /* hud/overlay mode */
} MenuState;
static MenuState menuState = MENU_STATE_START;

Actor *playerActor = NULL;

void Gam_Start( void ) {
	menuState = MENU_STATE_HUD;
	inputTarget = INPUT_TARGET_GAME;

	Act_SpawnActors();
}

void Gam_GameKeyboard( unsigned char key ) {

}

void Gam_MenuKeyboard( unsigned char key ) {
	switch( menuState ) {
		case MENU_STATE_START:
			/* if any key was hit here, just switch to the game */
			Gam_Start();
			break;
		default:
			PrintError( "Unhandled menu state, %d!\n", menuState );
	}
}

void Gam_Keyboard( unsigned char key, int x, int y ) {
	if ( inputTarget == INPUT_TARGET_GAME ) {
		Gam_GameKeyboard( key );
		return;
	}

	Gam_MenuKeyboard( key );
}

void Gam_Initialize( void ) {}

void Gam_Shutdown( void ) {}
