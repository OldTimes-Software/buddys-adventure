/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"

static PLShaderProgram *shaderPrograms[ MAX_SHADER_TYPES ];
static PLCamera *cameraInterface = NULL;

typedef struct RGBMap {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGBMap;
static RGBMap playPal[ 256 ], titlePal[ 256 ];

//#define DEBUG_PAL
#ifdef DEBUG_PAL
PLTexture *playPalTexture = NULL;
PLTexture *titlePalTexture = NULL;
#endif

void Gfx_LoadPalette( RGBMap *palette, const char *indexName ) {
	/* clear out the palette before we continue */
	memset( palette, 0, 256 );

	PLFile *filePtr = plLoadPackageFile( globalWad, indexName );
	if ( filePtr == NULL ) {
		PrintWarn( "Failed to find \"%s\"!\nPL: %s\n", indexName, plGetError() );
		return;
	}

	if ( plReadFile( filePtr, palette, sizeof( RGBMap ), 256 ) != 256 ) {
		PrintWarn( "Failed to read in palette data for \"%s\"!\nPL: %s\n", indexName, plGetError() );
	}

	plCloseFile( filePtr );
}

void Gfx_RegisterShader( ShaderType type, const char *vertPath, const char *fragPath ) {
	shaderPrograms[ type ] = plCreateShaderProgram();
	if ( shaderPrograms[ type ] == NULL ) {
		PrintError( "Failed to create shader program!\nPL: %s\n", plGetError() );
	}

	if ( !plRegisterShaderStageFromDisk( shaderPrograms[ type ], vertPath, PL_SHADER_TYPE_VERTEX ) ) {
		PrintError( "Failed to register vertex stage!\nPL: %s\n", plGetError() );
	}

	if ( !plRegisterShaderStageFromDisk( shaderPrograms[ type ], fragPath, PL_SHADER_TYPE_FRAGMENT ) ) {
		PrintError( "Failed to register fragment stage!\nPL: %s\n", plGetError() );
	}

	if ( !plLinkShaderProgram( shaderPrograms[ type ] ) ) {
		PrintError( "Failed to link shader stages!\nPL: %s\n", plGetError() );
	}
}

void Gfx_EnableShaderProgram( ShaderType type ) {
	plSetShaderProgram( shaderPrograms[ type ] );
}

void Gfx_Initialize( void ) {
	PrintMsg( "Initializing Gfx...\n" );

	plSetGraphicsMode( PL_GFX_MODE_OPENGL_CORE );

	cameraInterface = plCreateCamera();
	if ( cameraInterface == NULL ) {
		PrintError( "Failed to create camera!\nPL: %s\n", plGetError() );
	}

	cameraInterface->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
	cameraInterface->fov = 90.0f;
	cameraInterface->near = 0.0f;
	cameraInterface->far = 1000.0f;
	cameraInterface->viewport.w = YIN_DISPLAY_WIDTH;
	cameraInterface->viewport.h = YIN_DISPLAY_HEIGHT;

	/* create the default shader programs */
	Gfx_RegisterShader( SHADER_GENERIC, "shaders/generic.vert", "shaders/vertex_colour.frag" );
	Gfx_RegisterShader( SHADER_TEXTURE, "shaders/generic.vert", "shaders/texture.frag" );
	Gfx_RegisterShader( SHADER_ALPHA_TEST, "shaders/generic.vert", "shaders/alpha_test.frag" );

	Gfx_EnableShaderProgram( SHADER_GENERIC );

	plSetClearColour( PLColour( 128, 0, 0, 255 ) );

	Gfx_LoadPalette( titlePal, "TITLEPAL" );
	Gfx_LoadPalette( playPal, "PLAYPAL" );

	/* for debugging */
#ifdef DEBUG_PAL
	PLImage *palImage;
	palImage = plCreateImage( ( uint8_t * ) titlePal, 16, 16, PL_COLOURFORMAT_RGB, PL_IMAGEFORMAT_RGB8 );
	titlePalTexture = plCreateTexture();
	plUploadTextureImage( titlePalTexture, palImage );
	plDestroyImage( palImage );
	palImage = plCreateImage( ( uint8_t * ) playPal, 16, 16, PL_COLOURFORMAT_RGB, PL_IMAGEFORMAT_RGB8 );
	playPalTexture = plCreateTexture();
	plUploadTextureImage( playPalTexture, palImage );
	plDestroyImage( palImage );
#endif
}

void Gfx_Display( void ) {
	plClearBuffers( PL_BUFFER_DEPTH | PL_BUFFER_COLOUR );

	plSetupCamera( cameraInterface );

	PLMatrix4 transform = plMatrix4Identity();
	plDrawGrid( &transform, 0, 0, 320, 200, 8 );

	plDrawTriangle( 128, 0, 64, 64 );

	Gfx_EnableShaderProgram( SHADER_TEXTURE );

#ifdef DEBUG_PAL
	plDrawTexturedRectangle( 16, 64, 32, 32, playPalTexture );
	plDrawTexturedRectangle( 48, 64, 32, 32, titlePalTexture );
#endif
}
