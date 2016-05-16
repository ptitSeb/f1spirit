#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_net.h"

#include "F1Spirit.h"
#include "sound.h"
#include "List.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "PlacedGLTile.h"
#include "RotatedGLTile.h"
#include "keyboardstate.h"
#include "CPlayer.h"
#include "CCar.h"
#include "2DCMC.h"
#include "RoadPiece.h"
#include "track.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

PlayerCCar::PlayerCCar() : RacingCCar()
{
	int i;

	track = 0;
	car = 0;
	laps = 0;

	show_damage_timmer = 0;
	show_damage_cycle = 0;

	for (i = 0;i < MAX_NLAPS;i++)
		lap_time[i] = 0;

	wrong_way = false;

	show_pit_stop = -1;

	inside_pit = false;

	pit_time = 0;

	last_sign_showing = -1;

	times_sign_showed = 0;

	c_a_speed = 0;
} 


PlayerCCar::~PlayerCCar()
{
	delete car;
	car = 0;
} 

