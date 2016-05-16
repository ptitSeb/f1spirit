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
#include "SDL_mixer.h"
#include "SDL_image.h"
#include "SDL_net.h"

#include "stdlib.h"

#include "F1Spirit.h"
#include "auxiliar.h"
#include "sound.h"
#include "List.h"
#include "2DCMC.h"
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
#include "randomc.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


extern TRanrotBGenerator *rg;




int F1SpiritApp::trackload_cycle(KEYBOARDSTATE *k)
{

	if (state_cycle == 40) {

		rg->RandomInit(0);

		/* init F1SpiritGame object */
		delete race_game;
		race_game = new F1SpiritGame(current_player, menu_selected_track, menu_selected_nplayers, menu_multiplayer_n_enemycars, menu_multiplayer_enemy_speed, (int *)menu_selected_car, (int **)menu_selected_part, font, k);

		race_state = 0;
		race_state_timmer = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Prodeeding to game start!\n");
#endif

	} 

	if (state_cycle >= 80)
		return APP_STATE_RACE;

	return APP_STATE_TRACKLOAD;
} 


void F1SpiritApp::trackload_draw(void)
{

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	{
		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"LOADING TRACK...", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
		GLTile *t;

		print_left_bmp((unsigned char *)"LOADING TRACK...", font, sfc, 0, 0, 0);

		if (state_cycle < 25)
			surface_fader(sfc, 1.0, 1.0, 1.0, state_cycle*0.04F, 0);

		if (state_cycle > 55)
			surface_fader(sfc, 1.0, 1.0, 1.0, (80 - state_cycle)*0.04F, 0);

		t = new GLTile(sfc);

		t->set_hotspot(sfc->w / 2, 0);

		t->draw(320, 200, 0, 0, 1);

		delete t;
	}

} 

