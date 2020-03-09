/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum ActorType {
	ACTOR_NONE,

	/* monsters */
	ACTOR_BOSS, /* baron of hell */
	ACTOR_TROO, /* imp */
	ACTOR_SARG, /* demon */

	ACTOR_PLAYER,

	MAX_ACTOR_TYPES,
} ActorType;

typedef struct Actor Actor;

void      Act_Initialize( void );
void      Act_Shutdown( void );
void      Act_SpawnActors( void );
void      Act_DisplayActors( void );
Actor     *Act_SpawnActor( ActorType type, PLVector3 position, float angle, void *userData );
Actor     *Act_DestroyActor( Actor *self );
void      Act_SetPosition( Actor *self, const PLVector3 *position );
PLVector3 Act_GetPosition( const Actor *self );
void      Act_SetVelocity( Actor *self, const PLVector3 *velocity );
PLVector3 Act_GetVelocity( const Actor *self );
void      Act_SetAngle( Actor *self, float angle );
float     Act_GetAngle( const Actor *self );
PLVector3 Act_GetForward( const Actor *self );
