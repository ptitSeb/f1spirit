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
#include "SDL_net.h"

#include "F1Spirit.h"
#include "SDL_glutaux.h"
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
#include "F1Spirit-auxiliar.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern int N_SFX_CHANNELS;
extern bool sound;


int F1SpiritApp::race_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Game started!\n");
#endif

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Executing first game cycle... race_state = %i!\n", race_state);
#endif

		race_desired_action = 0;

	} 

	if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE] && race_state < 3) {
		Sound_pause_music();
		race_game->pauseSFX();

		// pause rain sfx
		Mix_HaltChannel(SFX_RAIN);

		Sound_play(S_pause, 128);

		menu_fading = -1;
		menu_fading_ctnt = 20;

		return APP_STATE_GAMEOPTIONS;
	} 

	switch (race_state) {

		case 0:
			/* RACING: */
			break;

		case 1:
			/* RESTART: */
			return APP_STATE_TRACKLOAD;
			break;

		case 2:
			/* RACE FINISHED/QUIT: */
			raceresult_state = 0;
			raceresult_timmer = 0;
			Mix_HaltChannel(SFX_RAIN);

			{
				int i;
				bool all_completed = true;

				for (i = 0;i < 20;i++)
					if (current_player->get_points(i) != 9)
						all_completed = false;

				if (all_completed &&
				        current_player->get_points(20) < 9 &&
				        race_game->get_nplayers() == 1 &&
				        race_game->get_player_position(0) == 0) {

					endsequence_returnstate = APP_STATE_RACE_RESULT;
					return APP_STATE_ENDSEQUENCE;
				} else {
					return APP_STATE_RACE_RESULT;
				} 
			}

			break;
	} 

	if (race_state == 0) {
		bool retval = race_game->cycle(k);

		if (!retval) {
			if (race_desired_action == 0)
				race_state = 2;
			else  {
				// stop race music being played if user desired to restart the race
				Sound_release_music();

				race_state = 1;
			}

			race_desired_action = 0;
		} 
	} 

	return APP_STATE_RACE;
} 


void F1SpiritApp::race_draw(void)
{

#ifdef F1SPIRIT_DEBUG_MESSAGES

	if (state_cycle == 0) {
		output_debug_message("First Game Drawing cycle started...\n");
	} 

#endif

	glClearColor(0, 0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	race_game->draw(true);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	if (state_cycle == 0) {
		output_debug_message("First Game Drawing cycle COMPLETE\n");
	} 

#endif
} 



