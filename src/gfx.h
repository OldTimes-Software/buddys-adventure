/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum ShaderType {
	SHADER_GENERIC,
	SHADER_TEXTURE,
	SHADER_ALPHA_TEST,

	MAX_SHADER_TYPES
} ShaderType;

void Gfx_Initialize( void );
void Gfx_Shutdown( void );
void Gfx_Display( void );
