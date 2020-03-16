/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "map.h"
#include "gfx.h"

static struct {
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

static void Map_LoadPoints( PLPackage *wad ) {
	PLFile *filePtr = plLoadPackageFile( wad, "M_POINTS" );
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

static void Map_LoadLines( PLPackage *wad ) {
	PLFile *filePtr = plLoadPackageFile( wad, "M_LINES" );
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

void Map_LoadAreas( PLPackage *wad ) {
	PLFile *filePtr = plLoadPackageFile( wad, "M_AREAS" );
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

		area->max.x = area->max.y = -9999.0f;
		area->min.x = area->min.y = 9999.0f;

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

void Map_Load( PLPackage *wad ) {
	Map_LoadPoints( wad );
	Map_LoadLines( wad );
	Map_LoadAreas( wad );
}

void Map_Draw( void ) {
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
