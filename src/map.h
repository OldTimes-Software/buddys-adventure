/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

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
	uint32_t     unknown0;
	uint16_t     unused0;
	uint16_t     unused1;
	unsigned int numLines;
	unsigned int *lineIndices;
	int          max[ 2 ]; /* boundary maximum */
	int          min[ 2 ]; /* boundary minimum */
} MapArea;

void Map_Load( PLPackage *wad );
void Map_Draw( void );
