/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include "yin.h"

#include <GL/freeglut.h>

static void System_Idle( void ) {

}

int main( int argc, char **argv ) {
	plInitialize( argc, argv );

	glutInitWindowSize( YIN_DISPLAY_WIDTH, YIN_DISPLAY_HEIGHT );
	glutInitWindowPosition( 256, 256 );
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE );

	glutCreateWindow( YIN_WINDOW_TITLE );

	glutIdleFunc( System_Idle );

	glutMainLoop();

	return EXIT_SUCCESS;
}
