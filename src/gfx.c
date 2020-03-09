/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "gfx.h"
#include "gam.h"

static PLShaderProgram *shaderPrograms[MAX_SHADER_TYPES];

static PLCamera *auxCamera = NULL;
static PLCamera *playerCamera = NULL;

typedef struct RGBMap {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGBMap;
static RGBMap playPal[256], titlePal[256];

//#define DEBUG_PAL
#ifdef DEBUG_PAL
PLTexture *playPalTexture = NULL;
PLTexture *titlePalTexture = NULL;
#endif

PLTexture *fallbackTexture = NULL;
PLTexture *titlePicTexture = NULL;

PLTexture *Gfx_GenerateTextureFromData( uint8_t *data, unsigned int w, unsigned int h, unsigned int numChannels,
										bool generateMipMap ) {
	PLColourFormat cFormat;
	PLImageFormat iFormat;

	switch ( numChannels ) {
		default:
			PrintWarn( "Invalid number of colour channels specified!\n" );
			return NULL;
		case 3:
			cFormat = PL_COLOURFORMAT_RGB;
			iFormat = PL_IMAGEFORMAT_RGB8;
			break;
		case 4:
			cFormat = PL_COLOURFORMAT_RGBA;
			iFormat = PL_IMAGEFORMAT_RGBA8;
			break;
	}

	PLImage *imageData = plCreateImage( data, w, h, cFormat, iFormat );
	if ( imageData == NULL) {
		PrintWarn( "Failed to generate image data!\nPL: %s\n", plGetError());
	}

	plWriteImage( imageData, "test.png" );

	PLTexture *texture = plCreateTexture();
	if ( texture == NULL) {
		PrintError( "Failed to create texture!\nPL: %s\n", plGetError());
	}

	if ( !generateMipMap ) {
		texture->flags &= PL_TEXTURE_FLAG_NOMIPS;
	}

	texture->filter = PL_TEXTURE_FILTER_NEAREST;

	if ( !plUploadTextureImage( texture, imageData )) {
		PrintError( "Failed to generate texture from image!\nPL: %s\n", plGetError());
	}

	plDestroyImage( imageData );

	return texture;
}

void Gfx_LoadPalette( RGBMap *palette, const char *indexName ) {
	/* clear out the palette before we continue */
	memset( palette, 0, 256 );

	PLFile *filePtr = plLoadPackageFile( globalWad, indexName );
	if ( filePtr == NULL) {
		PrintWarn( "Failed to find \"%s\"!\nPL: %s\n", indexName, plGetError());
		return;
	}

	if ( plReadFile( filePtr, palette, sizeof( RGBMap ), 256 ) != 256 ) {
		PrintWarn( "Failed to read in palette data for \"%s\"!\nPL: %s\n", indexName, plGetError());
	}

	plCloseFile( filePtr );
}

PLTexture *Gfx_LoadLumpTexture( const RGBMap *palette, const char *indexName ) {
	PLFile *filePtr = plLoadPackageFile( globalWad, indexName );
	if ( filePtr == NULL) {
		PrintWarn( "Failed to find \"%s\"!\nPL: %s\n", indexName, plGetError());
		return fallbackTexture;
	}

	bool status;
	uint16_t width = plReadInt16( filePtr, false, &status );
	if ( width == 0 ) {
		PrintError( "Invalid image width for \"%s\"!\n", indexName );
	}

	uint16_t height = plReadInt16( filePtr, false, &status );
	if ( height == 0 ) {
		PrintError( "Invalid image height \"%s\"!\n", indexName );
	}

	if ( !status ) {
		PrintError( "Failed to read in lump width and height for \"%s\"!\nPL: %s\n", indexName, plGetError());
	}

	unsigned int lumpDataSize = width * height;

	/* seems to be totally unused... */
	plFileSeek( filePtr, 4, PL_SEEK_CUR );

	uint8_t *imageBuffer = malloc( lumpDataSize );
	if ( plReadFile( filePtr, imageBuffer, 1, lumpDataSize ) != lumpDataSize ) {
		PrintError( "Failed to read in lump data for \"%s\"!\nPL: %s\n", indexName, plGetError());
	}

	plCloseFile( filePtr );

	/* now convert using the palette (I'm lazy, so we'll just convert to rgba) */
	PLColour *colourBuffer = malloc( sizeof( PLColour ) * lumpDataSize );
	for ( unsigned int i = 0; i < lumpDataSize; ++i ) {
		colourBuffer[ i ].r = palette[ imageBuffer[ i ]].r;
		colourBuffer[ i ].g = palette[ imageBuffer[ i ]].g;
		colourBuffer[ i ].b = palette[ imageBuffer[ i ]].b;
		colourBuffer[ i ].a = ( imageBuffer[ i ] == 255 ) ? 0 : 255;
	}
	free( imageBuffer );

	PLTexture *texture = Gfx_GenerateTextureFromData(( uint8_t * ) colourBuffer, width, height, 4, false );

	free( colourBuffer );

	return texture;
}

void Gfx_RegisterShader( ShaderType type, const char *vertPath, const char *fragPath ) {
	shaderPrograms[ type ] = plCreateShaderProgram();
	if ( shaderPrograms[ type ] == NULL) {
		PrintError( "Failed to create shader program!\nPL: %s\n", plGetError());
	}

	if ( !plRegisterShaderStageFromDisk( shaderPrograms[ type ], vertPath, PL_SHADER_TYPE_VERTEX )) {
		PrintError( "Failed to register vertex stage!\nPL: %s\n", plGetError());
	}

	if ( !plRegisterShaderStageFromDisk( shaderPrograms[ type ], fragPath, PL_SHADER_TYPE_FRAGMENT )) {
		PrintError( "Failed to register fragment stage!\nPL: %s\n", plGetError());
	}

	if ( !plLinkShaderProgram( shaderPrograms[ type ] )) {
		PrintError( "Failed to link shader stages!\nPL: %s\n", plGetError());
	}
}

void Gfx_EnableShaderProgram( ShaderType type ) {
	plSetShaderProgram( shaderPrograms[ type ] );
}

void Gfx_Initialize( void ) {
	PrintMsg( "Initializing Gfx...\n" );

	plSetGraphicsMode( PL_GFX_MODE_OPENGL_CORE );

	/* create both the interface camera and player camera */

	auxCamera = plCreateCamera();
	if ( auxCamera == NULL) {
		PrintError( "Failed to create auxiliary camera!\nPL: %s\n", plGetError());
	}
	auxCamera->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
	auxCamera->near = 0.0f;
	auxCamera->far = 1000.0f;
	auxCamera->viewport.w = YIN_DISPLAY_WIDTH;
	auxCamera->viewport.h = YIN_DISPLAY_HEIGHT;

	playerCamera = plCreateCamera();
	if ( playerCamera == NULL) {
		PrintError( "Failed to create player camera!\nPL: %s\n", plGetError());
	}
	playerCamera->viewport.w = YIN_DISPLAY_WIDTH;
	playerCamera->viewport.h = YIN_DISPLAY_HEIGHT;

	/* create the default shader programs */
	Gfx_RegisterShader( SHADER_GENERIC, "shaders/generic.vert", "shaders/vertex_colour.frag" );
	Gfx_RegisterShader( SHADER_TEXTURE, "shaders/generic.vert", "shaders/texture.frag" );
	Gfx_RegisterShader( SHADER_ALPHA_TEST, "shaders/generic.vert", "shaders/alpha_test.frag" );

	Gfx_EnableShaderProgram( SHADER_GENERIC );

	plSetClearColour(PLColour( 255, 0, 255, 255 ) );

	Gfx_LoadPalette( titlePal, "TITLEPAL" );
	Gfx_LoadPalette( playPal, "PLAYPAL" );

	/* generate fallback texture */
	PLColour fallbackData[] = {
			{ 255, 0, 220, 255 },
			{ 0,   0, 0,   255 },
			{ 0,   0, 0,   255 },
			{ 255, 0, 220, 255 },
	};
	fallbackTexture = Gfx_GenerateTextureFromData(( uint8_t * ) fallbackData, 2, 2, 4, false );
	if ( fallbackTexture == NULL) {
		PrintError( "Failed to create fallback texture!\n" );
	}

	/* and now, finally, load in the splash screen! */
	titlePicTexture = Gfx_LoadLumpTexture( titlePal, "TITLEPIC" );

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

void Gfx_Shutdown( void ) {
	plDestroyCamera( auxCamera );
	plDestroyCamera( playerCamera );
}

void Gfx_DisplayMenu( void ) {

}

void Gfx_Display( void ) {
	plClearBuffers( PL_BUFFER_DEPTH | PL_BUFFER_COLOUR );

	plSetupCamera( auxCamera );

	PLMatrix4 transform = plMatrix4Identity();
	plDrawGrid( &transform, 0, 0, 320, 200, 8 );

	Gfx_EnableShaderProgram( SHADER_ALPHA_TEST );

	plDrawTexturedRectangle( 0, 0, 320, 200, titlePicTexture );

#ifdef DEBUG_PAL
	plDrawTexturedRectangle( 16, 16, 32, 32, fallbackTexture );
	plDrawTexturedRectangle( 16, 64, 32, 32, playPalTexture );
	plDrawTexturedRectangle( 48, 64, 32, 32, titlePalTexture );
#endif
}
