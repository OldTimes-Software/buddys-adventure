/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#include <PL/platform.h>
#include <PL/platform_console.h>
#include <PL/platform_filesystem.h>
#include <PL/platform_package.h>
#include <PL/platform_graphics.h>
#include <PL/platform_graphics_camera.h>

#define YIN_VERSION 20200308

#define YIN_DISPLAY_WIDTH  320
#define YIN_DISPLAY_HEIGHT 200

#define YIN_WINDOW_TITLE "Buddy's Adventure"

enum {
	YIN_KEY_PAUSE = 19,
	YIN_KEY_CAPS = 20,
	YIN_KEY_ESCAPE = 27,
	YIN_KEY_SPACE = 32,
};

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
