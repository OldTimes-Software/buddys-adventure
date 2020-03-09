/* Copyright (C) 2020 Mark Sowden <markelswo@gmail.com>
 * Project Yin
 * */

#pragma once

typedef enum ActorType {
	ACTOR_TYPE_NONE,

	/* monsters */
	ACTOR_TYPE_BOSS, /* baron of hell */
	ACTOR_TYPE_TROO, /* imp */
	ACTOR_TYPE_SARG, /* demon */

	ACTOR_TYPE_PLAYER,
} ActorType;

typedef struct Actor Actor;

void Act_Initialize( void );
void Act_Shutdown( void );

void Act_SpawnActors( void );

Actor *Act_SpawnActor( PLVector3 position, float angle, void *userData );
Actor *Act_DestroyActor( Actor *self );
