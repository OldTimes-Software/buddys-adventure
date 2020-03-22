/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum GfxShaderType {
	SHADER_GENERIC,
	SHADER_TEXTURE,
	SHADER_ALPHA_TEST,
	SHADER_LIT,

	MAX_SHADER_TYPES
} GfxShaderType;

typedef struct GfxAnimationFrame {
	unsigned int leftOffset;
	unsigned int topOffset;
	PLTexture    *texture;
} GfxAnimationFrame;

void Gfx_Initialize( void );
void Gfx_Shutdown( void );
void Gfx_Display( void );
void Gfx_EnableShaderProgram( GfxShaderType type );

void Gfx_DrawAxesPivot( PLVector3 position, PLVector3 rotation );
void Gfx_DrawAnimationFrame( GfxAnimationFrame *frame, PLVector3 position );

void Gfx_LoadAnimationFrames( const char **frameList, GfxAnimationFrame **destination, unsigned int numFrames );

PLTexture *Gfx_GetWallTexture( unsigned int index );
PLTexture *Gfx_GetFloorTexture( unsigned int index );
