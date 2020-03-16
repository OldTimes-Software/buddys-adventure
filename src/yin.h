/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include <PL/platform.h>
#include <PL/platform_console.h>
#include <PL/platform_filesystem.h>
#include <PL/platform_package.h>
#include <PL/platform_graphics.h>
#include <PL/platform_graphics_camera.h>

#include <assert.h>

#define YIN_VERSION 20200308

#define YIN_DISPLAY_WIDTH  640
#define YIN_DISPLAY_HEIGHT 400

#define YIN_WINDOW_WIDTH  640
#define YIN_WINDOW_HEIGHT 400

#define YIN_WINDOW_TITLE "Buddy's Adventure"

#define YIN_TICK_RATE 60 /* ms */

#define u_unused( a ) ( void )( ( a ) )

/* map everything out to controller-style input
 * even if the user isn't necessarily using a controller
 */
typedef enum InputButton {
	YIN_INPUT_INVALID,

	YIN_INPUT_UP,
	YIN_INPUT_DOWN,
	YIN_INPUT_LEFT,
	YIN_INPUT_RIGHT,

	YIN_INPUT_LEFT_STICK,
	YIN_INPUT_RIGHT_STICK,

	YIN_INPUT_START,

	YIN_INPUT_A,
	YIN_INPUT_B,
	YIN_INPUT_X,
	YIN_INPUT_Y,

	YIN_INPUT_LB,
	YIN_INPUT_LT,
	YIN_INPUT_RB,
	YIN_INPUT_RT,

	MAX_BUTTON_INPUTS
} InputButton;

enum {
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
};

#define PrintError( ... ) plLogMessage( LOG_LEVEL_ERROR, __VA_ARGS__ ); exit( EXIT_FAILURE )
#define PrintWarn( ... )  plLogMessage( LOG_LEVEL_WARN, __VA_ARGS__ )
#define PrintMsg( ... )   plLogMessage( LOG_LEVEL_INFO, __VA_ARGS__ )

extern PLPackage *globalWad;
#define YIN_GLOBAL_WAD "doom.wad"

bool Sys_GetInputState( InputButton inputIndex );
