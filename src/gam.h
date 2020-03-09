/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum MenuState {
	MENU_STATE_START, /* draw start screen */
	MENU_STATE_HUD,   /* hud/overlay mode */
} MenuState;

#define PLAYER_VIEW_OFFSET  64.0f
#define PLAYER_MAX_VELOCITY 10.0f

MenuState Gam_GetMenuState( void );

typedef struct Actor Actor;
Actor *Gam_GetPlayer( void );

void Gam_Initialize( void );
void Gam_Shutdown( void );
void Gam_DisplayMap( void );
void Gam_Keyboard( unsigned char key, int x, int y );
