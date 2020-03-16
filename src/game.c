/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "game.h"
#include "act.h"
#include "gfx.h"

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

typedef struct MapLine {
	uint16_t startVertex;
	uint16_t endVertex;
	uint16_t flags;
	uint16_t unknown0;
	uint16_t colSomething;
	uint8_t  unknown1;
	uint8_t  hScale;
	uint16_t unknown2;
	uint32_t unknown3;
	uint16_t unknown4;
} MapLine;

typedef struct MapArea {
	uint32_t unknown0;
	uint16_t unused0;
	uint16_t unused1;
	unsigned int numLines;
	unsigned int *lineIndices;
	PLVector2    max; /* boundary maximum */
	PLVector2    min; /* boundary minimum */
} MapArea;

struct {
	MapArea      *areas;
	unsigned int numAreas;
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

void Gam_LoadMapPoints( void ) {
	PLFile *filePtr = plLoadPackageFile( globalWad, "M_POINTS" );
	if ( filePtr == NULL ) {
		PrintError( "Failed to find point data!\nPL: %s\n", plGetError() );
	}

	bool status;
	mapData.numPoints = plReadInt32( filePtr, false, &status );
	mapData.points = malloc( sizeof( MapPoint ) * mapData.numPoints );
	for ( unsigned int i = 0; i < mapData.numPoints; ++i ) {
		/* flipped so they match up with what we need */
		mapData.points[ i ].y = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 ) * 2;
		mapData.points[ i ].x = ( int16_t ) ( plReadInt32( filePtr, false, &status ) >> 16 ) * 2;
	}

	plCloseFile( filePtr );
}

void Gam_LoadMapLines( void ) {
	PLFile *filePtr = plLoadPackageFile( globalWad, "M_LINES" );
	if ( filePtr == NULL ) {
		PrintError( "Failed to find line data!\nPL: %s\n", plGetError() );
	}

	bool status;
	mapData.numLines = plReadInt32( filePtr, false, &status );
	mapData.lines = malloc( sizeof( MapLine ) * mapData.numLines );
	for ( unsigned int i = 0; i < mapData.numLines; ++i ) {
		mapData.lines[ i ].startVertex  = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].endVertex    = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].flags        = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown0     = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].colSomething = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown1     = plReadInt8( filePtr, &status );
		mapData.lines[ i ].hScale     = plReadInt8( filePtr, &status );
		mapData.lines[ i ].unknown2     = plReadInt16( filePtr, false, &status );
		mapData.lines[ i ].unknown3     = plReadInt32( filePtr, false, &status );
		mapData.lines[ i ].unknown4     = plReadInt16( filePtr, false, &status );

		printf( "%d %d\n", mapData.lines[ i ].startVertex, mapData.lines[ i ].endVertex );
	}

	plCloseFile( filePtr );
}

void Gam_LoadMapAreas( void ) {
	PLFile *filePtr = plLoadPackageFile( globalWad, "M_AREAS" );
	if ( filePtr == NULL ) {
		PrintError( "Failed to load area data!\nPL: %s\n", plGetError() );
	}

	bool status;
	mapData.numAreas = plReadInt32( filePtr, false, &status );
	mapData.areas = calloc( mapData.numAreas, sizeof( MapArea ) );
	for ( unsigned int i = 0; i < mapData.numAreas; ++i ) {
		MapArea *area = &mapData.areas[ i ];
		area->unknown0 = plReadInt32( filePtr, false, &status );
		area->unused0 = plReadInt16( filePtr, false, &status );
		area->unused1 = plReadInt16( filePtr, false, &status );
		area->numLines = plReadInt16( filePtr, false, &status );

		/* generate a list of all our line indices */
		area->lineIndices = malloc( sizeof( unsigned int ) * mapData.areas[ i ].numLines );
		for ( unsigned int j = 0; j < mapData.areas[ i ].numLines; ++j ) {
			mapData.areas[ i ].lineIndices[ j ] = plReadInt16( filePtr, false, &status );

			/* calculate the area bounds */
			const MapPoint *points[ 2 ];
			points[ 0 ] = &mapData.points[ mapData.lines[ area->lineIndices[ j ] ].startVertex ];
			points[ 1 ] = &mapData.points[ mapData.lines[ area->lineIndices[ j ] ].endVertex ];
			for ( unsigned int k = 0; k < 2; ++k ) {
				if ( points[ k ]->x > area->max.x ) {
					area->max.x = points[ k ]->x;
				}

				if ( points[ k ]->y > area->max.y ) {
					area->max.y = points[ k ]->y;
				}

				if ( points[ k ]->x < area->min.x ) {
					area->min.x = points[ k ]->x;
				}

				if ( points[ k ]->y < area->min.y ) {
					area->min.y = points[ k ]->y;
				}
			}
		}
	}

	plCloseFile( filePtr );
}

void Gam_LoadMap( const char *indexName ) {
	Gam_LoadMapPoints();
	Gam_LoadMapLines();
	Gam_LoadMapAreas();
}

void Gam_DisplayMap( void ) {
	Gfx_EnableShaderProgram( SHADER_LIT );

	for ( unsigned int i = 0; i < mapData.numAreas; ++i ) {
		const MapArea *area = &mapData.areas[ i ];
		for ( unsigned int j = 0; j < area->numLines; ++j ) {
			MapPoint *startPoint = &mapData.points[ mapData.lines[ area->lineIndices[ j ] ].startVertex ];
			MapPoint *endPoint = &mapData.points[ mapData.lines[ area->lineIndices[ j ] ].endVertex ];

			PLTexture *texture = Gfx_GetWallTexture( 20 );
			unsigned int wallHeight = texture->h * 2;

			plDrawTexturedQuad(
					&PLVector3( startPoint->x, wallHeight, startPoint->y ),
					&PLVector3( endPoint->x, wallHeight, endPoint->y ),
					&PLVector3( startPoint->x, 0, startPoint->y ),
					&PLVector3( endPoint->x, 0, endPoint->y ),
					2, 2,
					texture
			);
		}

		/* draw the ceiling and floor */

		plDrawTexturedQuad(
				&PLVector3( area->max.x, 0.0f, area->max.y ),
				&PLVector3( area->min.x, 0.0f, area->max.y ),
				&PLVector3( area->max.x, 0.0f, area->min.y ),
				&PLVector3( area->min.x, 0.0f, area->min.y ),
				2, 2,
				Gfx_GetFloorTexture( 0 )
		);
		plDrawTexturedQuad(
				&PLVector3( area->max.x, 128.0f, area->max.y ),
				&PLVector3( area->min.x, 128.0f, area->max.y ),
				&PLVector3( area->max.x, 128.0f, area->min.y ),
				&PLVector3( area->min.x, 128.0f, area->min.y ),
				2, 2,
				Gfx_GetFloorTexture( 1 )
		);
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
