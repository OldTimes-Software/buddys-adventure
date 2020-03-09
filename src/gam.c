/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "gam.h"
#include "act.h"

/* game specific implementation goes here! */

typedef enum InputTarget {
	INPUT_TARGET_MENU, /* menu mode */
	INPUT_TARGET_GAME, /* game mode */
} InputTarget;
static InputTarget inputTarget = INPUT_TARGET_MENU;
static MenuState menuState = MENU_STATE_START;

typedef struct MapPoint {
	int16_t x, y;
} MapPoint;
PL_STATIC_ASSERT( sizeof( MapPoint ) == 4, "Invalid MapPoint size!" );

typedef struct MapLine {
	uint16_t startVertex;
	uint16_t endVertex;
	uint16_t flags;
	uint16_t unknown0;
	uint16_t colSomething;
	uint8_t  unknown1;
	uint8_t  horScale;
	uint16_t unknown2;
	uint32_t unknown3;
	uint16_t unknown4;
} MapLine;
//PL_STATIC_ASSERT( sizeof( MapLine ) == 20, "Invalid MapLine size!" );

struct {
	MapPoint     *points;
	unsigned int numPoints;
	MapLine      *lines;
	unsigned int numLines;
} mapData = {
		.points    = NULL,
		.numPoints = 0,
		.lines     = NULL,
		.numLines  = 0,
};

MenuState Gam_GetMenuState( void ) {
	return menuState;
}

Actor *playerActor = NULL;

Actor *Gam_GetPlayer( void ) {
	return playerActor;
}

void Gam_LoadMap( const char *indexName ) {
	/* for now, we'll just ignore the index name... */

	PLFile *filePtr;

	/* load the points in */

	filePtr = plLoadPackageFile( globalWad, "M_POINTS" );
	if ( filePtr == NULL ) {
		PrintError( "Failed to find point data!\nPL: %s\n", plGetError() );
	}

	bool status;
	mapData.numPoints = plReadInt32( filePtr, false, &status );
	mapData.points = malloc( sizeof( MapPoint ) * mapData.numPoints );
	for ( unsigned int i = 0; i < mapData.numPoints; ++i ) {
		mapData.points[ i ].x = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
		mapData.points[ i ].y = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 );
	}

	plCloseFile( filePtr );

	/* and the lines */

	filePtr = plLoadPackageFile( globalWad, "M_LINES" );
	if ( filePtr == NULL ) {
		PrintError( "Failed to find line data!\nPL: %s\n", plGetError() );
	}

	mapData.numLines = plReadInt32( filePtr, false, &status );
	mapData.lines = malloc( sizeof( MapLine ) * mapData.numLines );
	for ( unsigned int i = 0; i < mapData.numLines; ++i ) {
		mapData.lines[ i ].startVertex  = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].endVertex    = plReadInt16( filePtr, false, &status );
		/*
		mapData.lines[ i ].flags        = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown0     = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].colSomething = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown1     = plReadInt8( filePtr, &status );
		mapData.lines[ i ].horScale     = plReadInt8( filePtr, &status );
		mapData.lines[ i ].unknown2     = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown3     = plReadInt32( filePtr, false, &status );
		mapData.lines[ i ].unknown4     = plReadInt16( filePtr, false, &status );
		 */

		plFileSeek( filePtr, 16, SEEK_CUR );

		printf( "%d %d\n", mapData.lines[ i ].startVertex, mapData.lines[ i ].endVertex );
	}

	plCloseFile( filePtr );
}

void Gam_DisplayMap( void ) {
	for ( unsigned int i = 0; i < mapData.numPoints; ++i ) {
		PLMatrix4 transform = plTranslateMatrix4( PLVector3( mapData.points[ i ].x, 0, mapData.points[ i ].y ) );
		plDrawSimpleLine( &transform, &PLVector3( 0, 0, 0 ), &PLVector3( 0, 128, 0 ), &PLColour( 255, 0, 0, 255 ) );
	}

	for ( unsigned int i = 0; i < mapData.numLines; ++i ) {
		MapPoint *startPoint = &mapData.points[ mapData.lines[ i ].startVertex ];
		MapPoint *endPoint = &mapData.points[ mapData.lines[ i ].endVertex ];

		PLMatrix4 transform = plMatrix4Identity();
		plDrawSimpleLine(
				&transform,
				&PLVector3( startPoint->x, 0, startPoint->y ),
				&PLVector3( endPoint->x, 0, endPoint->y ),
				&PLColour( 0, 255, 0, 255 ) );
		plDrawSimpleLine(
				&transform,
				&PLVector3( startPoint->x, 128, startPoint->y ),
				&PLVector3( endPoint->x, 128, endPoint->y ),
				&PLColour( 0, 255, 0, 255 ) );
	}
}

void Gam_Start( void ) {
	menuState = MENU_STATE_HUD;
	inputTarget = INPUT_TARGET_GAME;

	Gam_LoadMap( "whatever..." );

	Act_SpawnActors();

	/* spawn the player in */
	playerActor = Act_SpawnActor( ACTOR_PLAYER, PLVector3( 0, 0, 0 ), 0.0f, NULL );
}

void Gam_End( void ) {}

void Gam_GameKeyboard( unsigned char key ) {
	float forwardVelocity = 0.0f;
	//float strafeVelocity  = 0.0f;
	float nAngle = Act_GetAngle( playerActor );
	switch( key ) {
		case 'w':
			forwardVelocity += 2.0f;
			break;
		case 's':
			forwardVelocity -= 2.0f;
			break;
		case 'a':
		case YIN_KEY_LEFT:
			nAngle -= 2.0f;
			break;
		case 'd':
		case YIN_KEY_RIGHT:
			nAngle += 2.0f;
			break;
	}

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

	Act_SetAngle( playerActor, nAngle );
	Act_SetPosition( playerActor, &nPosition );
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
