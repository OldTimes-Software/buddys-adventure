/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"
#include "gfx.h"
#include "act.h"
#include "game.h"

#include <GL/freeglut.h>

PLPackage *globalWad = NULL;

static void Sys_Close( void ) {
	Act_Shutdown();
	Gam_Shutdown();
	Gfx_Shutdown();
}

static void Sys_Display( void ) {
	Gfx_Display();

	glutSwapBuffers();
}

static unsigned char Sys_TranslateKeyboardInput( unsigned char key ) {
	switch( key ) {
		default: return YIN_INPUT_INVALID;
		case 'w': return YIN_INPUT_UP;
		case 's': return YIN_INPUT_DOWN;
		case 'a': return YIN_INPUT_LEFT;
		case 'd': return YIN_INPUT_RIGHT;
		case GLUT_KEY_SHIFT_L: return YIN_INPUT_LEFT_STICK;
		case 27: return YIN_INPUT_START; /* escape */
		case ' ': return YIN_INPUT_A;
	}
}

bool keyStates[ MAX_BUTTON_INPUTS ];
bool Sys_GetInputState( InputButton inputIndex ) {
	return keyStates[ inputIndex ];
}

static void Sys_Keyboard( unsigned char key, int x, int y ) {
	u_unused( x );
	u_unused( y );

	key = Sys_TranslateKeyboardInput( key );
	if ( key == YIN_INPUT_INVALID ) {
		return;
	}

	keyStates[ key ] = true;
}

static void Sys_KeyboardUp( unsigned char key, int x, int y ) {
	u_unused( x );
	u_unused( y );

	key = Sys_TranslateKeyboardInput( key );
	if ( key == YIN_INPUT_INVALID ) {
		return;
	}

	keyStates[ key ] = false;
}

static void Sys_Reshape( int width, int height ) {
	//glutReshapeWindow( YIN_DISPLAY_WIDTH, YIN_DISPLAY_HEIGHT );
}

static void Sys_Idle( void ) {
	Gam_Tick();

	Sys_Display();
}

int main( int argc, char **argv ) {
	/* initialize the platform library */
	plInitialize( argc, argv );
	plInitializeSubSystems( PL_SUBSYSTEM_GRAPHICS | PL_SUBSYSTEM_IO | PL_SUBSYSTEM_IMAGE );

	/* mount all the dirs we need */
	plMountLocation( "./" );

	plSetupLogOutput( "log.txt" );
	plSetupLogLevel( LOG_LEVEL_ERROR, "error", PL_COLOUR_RED, true );
	plSetupLogLevel( LOG_LEVEL_WARN, "warning", PL_COLOUR_ORANGE, true );
	plSetupLogLevel( LOG_LEVEL_INFO, NULL, PL_COLOUR_WHITE, true );

	plRegisterStandardPackageLoaders();

	PrintMsg( "Initializing...\n" );

	/* ensure our wad is available */
	globalWad = plLoadPackage( YIN_GLOBAL_WAD );
	if ( globalWad == NULL ) {
		PrintError( "Failed to load \"" YIN_GLOBAL_WAD "\"!\nPL: %s\n", plGetError() );
	}

	glutInitWindowSize( YIN_WINDOW_WIDTH, YIN_WINDOW_HEIGHT );
	glutInitWindowPosition( 256, 256 );
	glutInit( &argc, argv );
	glutInitContextVersion( 3, 2 );
	glutInitContextFlags( GLUT_CORE_PROFILE | GLUT_DEBUG );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

	glutCreateWindow( YIN_WINDOW_TITLE );

	Gfx_Initialize();

	glutReshapeFunc( Sys_Reshape );
	glutDisplayFunc( Sys_Display );
	glutKeyboardFunc( Sys_Keyboard );
	glutKeyboardUpFunc( Sys_KeyboardUp );
	glutCloseFunc( Sys_Close );
	glutIdleFunc( Sys_Idle );

	Act_Initialize();

	glutMainLoop();

	return EXIT_SUCCESS;
}
