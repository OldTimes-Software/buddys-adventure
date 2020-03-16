/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum ShaderType {
	SHADER_GENERIC,
	SHADER_TEXTURE,
	SHADER_ALPHA_TEST,
	SHADER_LIT,

	MAX_SHADER_TYPES
} ShaderType;

void Gfx_Initialize( void );
void Gfx_Shutdown( void );
void Gfx_Display( void );
void Gfx_EnableShaderProgram( ShaderType type );
void Gfx_DrawAxesPivot( PLVector3 position, PLVector3 rotation );

PLTexture *Gfx_GetWallTexture( unsigned int index );
PLTexture *Gfx_GetFloorTexture( unsigned int index );
