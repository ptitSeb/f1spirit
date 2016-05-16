#ifdef _WIN32
#include "windows.h"
#else
#include <dirent.h>
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

#include "F1Spirit.h"
#include "sound.h"
#include "auxiliar.h"
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
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern int N_SFX_CHANNELS;
extern bool sound;

#define MENU_CONSTANT 20


int F1SpiritApp::gameoptions_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {

		menu_fading = -1;
		menu_fading_ctnt = MENU_CONSTANT;
		menu_current_menu = 5;

		/* ... */

	} 

	if (menu_fading == 0) {} else {
		if (menu_fading > 0)
			menu_fading_ctnt++;
		else
			menu_fading_ctnt--;

		if (menu_fading_ctnt <= 0 && menu_fading == -1) {
			menu_fading = 0;
		} 

		if (menu_fading_ctnt >= MENU_CONSTANT && menu_fading == 1) {
			menu_fading = 0;
			race_game->race_state = 3;
			race_game->race_state_timmer = 0;
			race_desired_action = 0;
			Sound_unpause_music();
			race_game->resumeSFX();
			race_game->rain_channel = 0;
			return APP_STATE_RACE;
		} 

		if (menu_fading_ctnt >= MENU_CONSTANT && menu_fading == 2) {
			menu_fading = 0;
			Sound_unpause_music();
			race_game->resumeSFX();
			race_game->rain_channel = 0;
			return APP_STATE_RACE;
		} 

		if (menu_fading_ctnt >= MENU_CONSTANT && menu_fading == 3) {
			menu_fading = 0;
			race_game->race_state = 3;
			race_game->race_state_timmer = 0;
			race_desired_action = 1;
			Sound_unpause_music();
			race_game->resumeSFX();
			race_game->rain_channel = 0;
			return APP_STATE_RACE;
		} 
	} 

	if (state_cycle == 0 || menu_state == MENU_CONSTANT*2 || menu_redefining_key || menu_force_rebuild_menu) {
		int browsing = 0;

		if (menu_options[1] != 0)
			browsing = 1;

		if (state_cycle != 0) {

#ifdef F1SPIRIT_DEBUG_MESSAGES
			output_debug_message("Executing gameoptions action: %i(%i)\n", menu_option_type[browsing][menu_selected[browsing]], menu_option_parameter[browsing][menu_selected[browsing]]);
#endif

			switch (menu_option_type[browsing][menu_selected[browsing]]) {

				case 0:  /* QUIT RACE: */
					menu_fading = 1;
					menu_fading_ctnt = 0;
					break;

				case 1:  /* MENU CHANGE: */
					menu_current_menu = menu_option_parameter[browsing][menu_selected[browsing]];

					if (menu_current_menu == 23) {
						menu_readme_start_y = 0;
						menu_readme_move_y = 0;
					} 

					break;

				case 7:  /* SET A KEY: */
					break;

				case 8:  /* SET A JOYSTICK: */
					menu_current_menu = menu_option_parameter[browsing][menu_selected[browsing]];

					{
						int i;
						FILE *fp;
						PlayerCCar *v;
						current_player->set_joystick(menu_selected[0] - 7, menu_selected[1] - 1);
						i = menu_selected[0] - 7;
						v = race_game->player_cars[i];

						if (menu_selected[1] - 1 == -1) {
							v->up = current_player->get_key(i, 0);
							v->down = current_player->get_key(i, 1);
							v->left = current_player->get_key(i, 2);
							v->right = current_player->get_key(i, 3);
							v->accelerate = current_player->get_key(i, 4);
							v->brake = current_player->get_key(i, 5);
						} else {
							int j = menu_selected[1] - 1;
							v->up = k->joystick_0_pos + j * k->joystick_size + 2;
							v->down = k->joystick_0_pos + j * k->joystick_size + 3;
							v->left = k->joystick_0_pos + j * k->joystick_size;
							v->right = k->joystick_0_pos + j * k->joystick_size + 1;
							v->accelerate = k->joystick_0_pos + j * k->joystick_size + 4;
							v->brake = k->joystick_0_pos + j * k->joystick_size + 5;
						} 

						if (v->up < 0)
							v->up = 0;

						if (v->up >= k->k_size)
							v->up = 0;

						if (v->down < 0)
							v->down = 0;

						if (v->down >= k->k_size)
							v->down = 0;

						if (v->left < 0)
							v->left = 0;

						if (v->left >= k->k_size)
							v->left = 0;

						if (v->up < 0)
							v->up = 0;

						if (v->right >= k->k_size)
							v->right = 0;

						if (v->up < 0)
							v->up = 0;

						if (v->accelerate >= k->k_size)
							v->accelerate = 0;

						if (v->up < 0)
							v->up = 0;

						if (v->brake >= k->k_size)
							v->brake = 0;

						v = 0;

						fp = f1open(player_filename, "wb", USERDATA);

						if (fp != 0) {
							current_player->save(fp);
							fclose(fp);
						} 
					}

					break;

				case 11:  /* MENU CHANGE TO SUBMENU: */
					menu_current_menu = menu_option_parameter[browsing][menu_selected[browsing]];
					break;

				case 13: { /* CONTINUE RACE: */
						menu_selecting_player++;
						menu_fading = 2;
						menu_fading_ctnt = 0;
					}

					break;

				case 15: { /* CHANGE MUSIC VOLUME: */
					}

					break;

				case 16: { /* CHANGE SFX VOLUME: */
					}

					break;

				case 24: { /* RESTART RACE: */
						menu_selecting_player++;
						menu_fading = 3;
						menu_fading_ctnt = 0;

						// also stop music/sfx on restart
						Mix_HaltChannel(SFX_RAIN);
						Sound_pause_music();
					}

					break;

			} 
		} 

		delete []menu_title[0];

		menu_title[0] = 0;

		delete []menu_options[0];

		menu_options[0] = 0;

		delete []menu_title[1];

		menu_title[1] = 0;

		delete []menu_options[1];

		menu_options[1] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Generating a new gameoptions menu: %i\n", menu_current_menu);

#endif

		switch (menu_current_menu) {

			case 5:

			case 6:

			case 7:

			case 8:

			case 19:

			case 20:

			case 22:

			case 24: {
					int i, l, pos;
					char *volumes[5] = {"NONE", "LOW ", "MED.", "HIGH", "MAX."};

					l = strlen("PLAYER 000\n") * current_player->get_nplayers() + strlen("CONTINUE\nQUIT\nRESTART\n\nMUSIC VOL: MAX.\nSFX VOL: MAX.\nPLAYER 1\nPLAYER 2\nPLAYER 3\nPLAYER 4\n") + 1;
					menu_options[0] = new char[l];
					pos = 0;
					{
						int v1 = 0, v2 = 0;
						v1 = current_player->get_music_volume() / 32;
						v2 = current_player->get_sfx_volume() / 32;
						sprintf(menu_options[0], "CONTINUE\nQUIT\nRESTART\n\nMUSIC VOL: %s\nSFX VOL: %s\n", volumes[v1], volumes[v2]);
					}

					pos = strlen(menu_options[0]);

					for (i = 0;i < current_player->get_nplayers();i++) {
						sprintf(menu_options[0] + pos, "PLAYER %i\n", i + 1);
						pos = strlen(menu_options[0]);
					} 

					menu_title[0] = new char[strlen("PAUSE") + 1];

					strcpy(menu_title[0], "PAUSE");

					menu_noptions[0] = current_player->get_nplayers() + 7;

					menu_option_type[0][0] = 13;

					menu_option_type[0][1] = 0;

					menu_option_type[0][2] = 24;

					menu_option_type[0][3] = -1;

					menu_option_type[0][4] = 15;

					menu_option_type[0][5] = 16;

					menu_option_type[0][6] = 11;

					menu_option_parameter[0][6] = 22;

					for (i = 0;i < current_player->get_nplayers();i++) {
						menu_option_type[0][i + 7] = 11;
						menu_option_parameter[0][i + 7] = 22;
					} 

					menu_first_option[0] = 0;
				}

				if (menu_current_menu == 22) {
					menu_title[1] = 0;
					menu_options[1] = new char[strlen("KEYBOARD CFG.\nJOYSTICK/KEYS\nBACK\n") + 1];
					strcpy(menu_options[1], "KEYBOARD CFG.\nJOYSTICK/KEYS\nBACK\n");
					menu_noptions[1] = 3;
					menu_option_type[1][0] = 1;
					menu_option_type[1][1] = 1;
					menu_option_type[1][2] = 1;
					menu_option_parameter[1][0] = 7;
					menu_option_parameter[1][1] = 8;
					menu_option_parameter[1][2] = 5;
					menu_first_option[1] = 0;
				} 

				if (menu_current_menu == 7) {
					char tmp[256];
					menu_title[1] = 0;
					sprintf(tmp, "GEAR UP: %s\nGEAR DOWN: %s\nLEFT: %s\nRIGHT: %s\nACCEL.: %s\nBRAKE: %s\nBACK\n",
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 0))),
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 1))),
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 2))),
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 3))),
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 4))),
					        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 6, 5))));
					//    strupr(tmp);
					menu_options[1] = new char[strlen(tmp) + 1];
					strcpy(menu_options[1], tmp);
					menu_noptions[1] = 7;
					menu_option_type[1][0] = 7;
					menu_option_type[1][1] = 7;
					menu_option_type[1][2] = 7;
					menu_option_type[1][3] = 7;
					menu_option_type[1][4] = 7;
					menu_option_type[1][5] = 7;
					menu_option_type[1][6] = 1;
					menu_option_parameter[1][0] = 7;
					menu_option_parameter[1][1] = 7;
					menu_option_parameter[1][2] = 7;
					menu_option_parameter[1][3] = 7;
					menu_option_parameter[1][4] = 7;
					menu_option_parameter[1][5] = 7;
					menu_option_parameter[1][6] = 6;
					menu_first_option[1] = 0;

					if (current_player->get_joystick(menu_selected[0]) != -1) {
						current_player->set_joystick(menu_selected[0], -1);
						{
							FILE *fp;

							fp = f1open(player_filename, "wb", USERDATA);

							if (fp != 0) {
								current_player->save(fp);
								fclose(fp);
							} 
						}
					} 
				} 

				if (menu_current_menu == 8) {
					int nj = SDL_NumJoysticks();
					int i, pos;

					menu_options[1] = new char[nj * strlen("JOYSTICK 000\n") + strlen("KEYBOARD\nBACK\n") + 1];
					pos = 0;
					sprintf(menu_options[1] + pos, "KEYBOARD\n");
					pos = strlen(menu_options[1]);

					for (i = 0;i < nj;i++) {
						sprintf(menu_options[1] + pos, "JOYSTICK %i\n", i);
						pos = strlen(menu_options[1]);
					} 

					sprintf(menu_options[1] + pos, "BACK\n");

					menu_noptions[1] = nj + 2;

					for (i = 0;i < nj + 1;i++) {
						menu_option_type[1][i] = 8;
						menu_option_parameter[1][i] = 6;
					} 

					menu_option_type[1][nj + 1] = 1;

					menu_option_parameter[1][nj + 1] = 6;

					menu_first_option[1] = 0;
				} 
		} 


		if ((menu_options[1] == 0 && menu_prev_nmenus != 2 && !menu_force_rebuild_menu)) {
			menu_selected[0] = 0;
			menu_selected_timmer[0] = 0;
		} 

		if (!menu_redefining_key && !menu_force_rebuild_menu)
			menu_selected[1] = 0;

		if (!menu_redefining_key && !menu_force_rebuild_menu)
			menu_selected_timmer[1] = 0;

		if (!menu_redefining_key && !menu_force_rebuild_menu)
			menu_state = 0;

		menu_force_rebuild_menu = false;
	} else {
		if (menu_fading == 0) {
			if (menu_state < MENU_CONSTANT)
				menu_state++;

			if (menu_state == MENU_CONSTANT) {
				menu_prev_nmenus = 0;

				if (menu_options[0] != 0)
					menu_prev_nmenus++;

				if (menu_options[1] != 0)
					menu_prev_nmenus++;
			} 

			if (menu_state > MENU_CONSTANT)
				menu_state++;
		} 
	} 


	if (menu_readme_move_y != 0) {
		menu_readme_start_y += menu_readme_move_y;

		if (menu_readme_move_y > 0)
			menu_readme_move_y--;

		if (menu_readme_move_y < 0)
			menu_readme_move_y++;

		if (menu_readme_start_y < 0)
			menu_readme_start_y = 0;
	} 

	if (menu_state == MENU_CONSTANT) {
		int browsing = 0;

		if (menu_options[1] != 0)
			browsing = 1;

		if (!menu_redefining_key) {
			if ((k->keyboard[SDLK_DOWN] && !k->old_keyboard[SDLK_DOWN]) ||
			        (k->keyboard[SDLK_RIGHT] && !k->old_keyboard[SDLK_RIGHT])) {
				if ((k->keyboard[SDLK_RIGHT] && !k->old_keyboard[SDLK_RIGHT]) &&
				        (menu_option_type[browsing][menu_selected[browsing]] == 15 ||
				         menu_option_type[browsing][menu_selected[browsing]] == 16)) {
					if (menu_option_type[browsing][menu_selected[browsing]] == 15) {
						int v = current_player->get_music_volume();

						if (v == 128)
							current_player->set_music_volume(0);

						if (v == 96)
							current_player->set_music_volume(128);

						if (v == 64)
							current_player->set_music_volume(96);

						if (v == 32)
							current_player->set_music_volume(64);

						if (v == 0)
							current_player->set_music_volume(32);
					} else {
						int v = current_player->get_sfx_volume();

						if (v == 128)
							current_player->set_sfx_volume(0);

						if (v == 96)
							current_player->set_sfx_volume(128);

						if (v == 64)
							current_player->set_sfx_volume(96);

						if (v == 32)
							current_player->set_sfx_volume(64);

						if (v == 0)
							current_player->set_sfx_volume(32);
					} 

					menu_force_rebuild_menu = true;

					Sound_play(S_menu_move, 128);

					{
						FILE *fp;

						fp = f1open(player_filename, "wb", USERDATA);

						if (fp != 0) {
							current_player->save(fp);
							fclose(fp);
						} 
					}
				} else {
					menu_selected[browsing]++;

					if (menu_option_type[browsing][menu_selected[browsing]] == -1)
						menu_selected[browsing]++;

					Sound_play(S_menu_move, 128);

					if (menu_selected[browsing] >= menu_noptions[browsing])
						menu_selected[browsing] -= menu_noptions[browsing];

					menu_selected_timmer[browsing] = 0;
				} 
			} 

			if ((k->keyboard[SDLK_UP] && !k->old_keyboard[SDLK_UP]) ||
			        (k->keyboard[SDLK_LEFT] && !k->old_keyboard[SDLK_LEFT])) {

				if ((k->keyboard[SDLK_LEFT] && !k->old_keyboard[SDLK_LEFT]) &&
				        (menu_option_type[browsing][menu_selected[browsing]] == 15 ||
				         menu_option_type[browsing][menu_selected[browsing]] == 16)) {
					if (menu_option_type[browsing][menu_selected[browsing]] == 15) {
						int v = current_player->get_music_volume();

						if (v == 128)
							current_player->set_music_volume(96);

						if (v == 96)
							current_player->set_music_volume(64);

						if (v == 64)
							current_player->set_music_volume(32);

						if (v == 32)
							current_player->set_music_volume(0);

						if (v == 0)
							current_player->set_music_volume(128);
					} else {
						int v = current_player->get_sfx_volume();

						if (v == 128)
							current_player->set_sfx_volume(96);

						if (v == 96)
							current_player->set_sfx_volume(64);

						if (v == 64)
							current_player->set_sfx_volume(32);

						if (v == 32)
							current_player->set_sfx_volume(0);

						if (v == 0)
							current_player->set_sfx_volume(128);
					} 

					menu_force_rebuild_menu = true;

					Sound_play(S_menu_move, 128);

					{
						FILE *fp;

						fp = f1open(player_filename, "wb", USERDATA);

						if (fp != 0) {
							current_player->save(fp);
							fclose(fp);
						} 
					}
				} else {
					menu_selected[browsing]--;

					if (menu_option_type[browsing][menu_selected[browsing]] == -1)
						menu_selected[browsing]--;

					Sound_play(S_menu_move, 128);

					if (menu_selected[browsing] < 0)
						menu_selected[browsing] += menu_noptions[browsing];

					menu_selected_timmer[browsing] = 0;
				} 
			} 

			if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
				if (menu_current_menu == 5)
					menu_selected[browsing] = 0;
				else
					menu_selected[browsing] = menu_noptions[browsing] - 1;

				Sound_play(S_menu_select, 128);

				menu_state++;
			} 

			if (menu_option_type[browsing][menu_selected[browsing]] != 2 &&
			        k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE]) {
				if (menu_option_type[browsing][menu_selected[browsing]] != 19 &&
				        menu_option_type[browsing][menu_selected[browsing]] != 20)
					Sound_play(S_menu_select, 128);

				if (menu_option_type[browsing][menu_selected[browsing]] == 7) {
					/* redefining a key: */
					menu_redefining_key = true;
				} else {
					if (menu_option_type[browsing][menu_selected[browsing]] == 15 ||
					        menu_option_type[browsing][menu_selected[browsing]] == 16 ||
					        menu_option_type[browsing][menu_selected[browsing]] == 19 ||
					        menu_option_type[browsing][menu_selected[browsing]] == 20) {
						if (menu_option_type[browsing][menu_selected[browsing]] == 15) {
							int v = current_player->get_music_volume();

							if (v == 128)
								current_player->set_music_volume(0);

							if (v == 96)
								current_player->set_music_volume(128);

							if (v == 64)
								current_player->set_music_volume(96);

							if (v == 32)
								current_player->set_music_volume(64);

							if (v == 0)
								current_player->set_music_volume(32);
						} 

						if (menu_option_type[browsing][menu_selected[browsing]] == 16) {
							int v = current_player->get_sfx_volume();

							if (v == 128)
								current_player->set_sfx_volume(0);

							if (v == 96)
								current_player->set_sfx_volume(128);

							if (v == 64)
								current_player->set_sfx_volume(96);

							if (v == 32)
								current_player->set_sfx_volume(64);

							if (v == 0)
								current_player->set_sfx_volume(32);
						} 

						menu_force_rebuild_menu = true;
					} else {
						menu_state++;
					} 
				} 
			} 


		} else {
			int i, np;
			np = menu_selected[0] - 6;
			PlayerCCar *v = race_game->player_cars[np];

			for (i = 0;i < SDLK_LAST;i++) {
				if (k->keyboard[i] && !k->old_keyboard[i]) {
					current_player->set_key(np, menu_selected[1], i);

					if (current_player->get_joystick(np) == -1) {
						v->up = current_player->get_key(np, 0);
						v->down = current_player->get_key(np, 1);
						v->left = current_player->get_key(np, 2);
						v->right = current_player->get_key(np, 3);
						v->accelerate = current_player->get_key(np, 4);
						v->brake = current_player->get_key(np, 5);
					} 

					if (v->up < 0)
						v->up = 0;

					if (v->up >= k->k_size)
						v->up = 0;

					if (v->down < 0)
						v->down = 0;

					if (v->down >= k->k_size)
						v->down = 0;

					if (v->left < 0)
						v->left = 0;

					if (v->left >= k->k_size)
						v->left = 0;

					if (v->up < 0)
						v->up = 0;

					if (v->right >= k->k_size)
						v->right = 0;

					if (v->up < 0)
						v->up = 0;

					if (v->accelerate >= k->k_size)
						v->accelerate = 0;

					if (v->up < 0)
						v->up = 0;

					if (v->brake >= k->k_size)
						v->brake = 0;

					v = 0;

					menu_redefining_key = false;

					menu_force_rebuild_menu = true;

					Sound_play(S_menu_select, 128);

					{
						FILE *fp;

						fp = f1open(player_filename, "wb", USERDATA);

						if (fp != 0) {
							current_player->save(fp);
							fclose(fp);
						} 
					}
				} 
			} 
		} 
	} 

	menu_selected_timmer[0]++;

	menu_selected_timmer[1]++;

	return APP_STATE_GAMEOPTIONS;
} 


void F1SpiritApp::gameoptions_draw(void)
{
	GLTile *menu;
	SDL_Surface *sfc1 = 0, *sfc2 = 0;
	int w1, w2, wt;
	float sel_factor, enter_factor = 1.0F;
	int menu_y = 160;

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (menu_state < MENU_CONSTANT)
		enter_factor = menu_state / float(MENU_CONSTANT);

	if (menu_state == MENU_CONSTANT)
		enter_factor = 1.0F;

	if (menu_state > MENU_CONSTANT)
		enter_factor = ((MENU_CONSTANT * 2) - menu_state) / float(MENU_CONSTANT);

	race_game->draw(true);

	{
		if (menu_fading != 0) {
			float f = 0;
			f = abs(menu_fading_ctnt) / float(MENU_CONSTANT);
			glColor4f(0, 0, 0, (1 - f) / 2);
		} else {
			glColor4f(0, 0, 0, 0.5F);
		} 

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);

		glVertex3f(0, 0, -4);

		glVertex3f(0, 480, -4);

		glVertex3f(640, 480, -4);

		glVertex3f(640, 0, -4);

		glEnd();
	}


	if (menu_options[0] != 0) {
		sel_factor = float(0.6F - 0.4F * sin(menu_selected_timmer[0] / 10.0F));

		if (menu_prev_nmenus == 2 &&
		        (menu_current_menu != 3 || menu_selected[1] != 0)
		        && menu_current_menu != 4) {
			sfc1 = draw_menu(11, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, 1, font, &(menu_first_option[0]));
		} else {
			if ((menu_option_type[0][menu_selected[0]] == 11 && menu_state > MENU_CONSTANT) ||
			        (menu_state < MENU_CONSTANT && menu_options[1] != 0)) {
				sfc1 = draw_menu(11, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, 1, font, &(menu_first_option[0]));
			} else {
				sfc1 = draw_menu(11, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, enter_factor, font, &(menu_first_option[0]));
			} 
		} 
	} 

	if (menu_options[1] != 0) {
		sel_factor = float(0.6F - 0.4F * sin(menu_selected_timmer[1] / 10.0F));

		if (menu_redefining_key)
			sel_factor = 0;

		sfc2 = draw_menu(11, menu_title[1], menu_options[1], menu_option_type[1], menu_selected[1], sel_factor, enter_factor, font, &(menu_first_option[1]));
	} 

	w1 = w2 = wt = 0;

	if (sfc1 != 0) {
		wt += sfc1->w;
		w1 = sfc1->w;
	} 

	if (sfc2 != 0) {
		wt += sfc2->w;
		w2 = sfc2->w;
	} 

	if (sfc1 != 0 && sfc2 != 0)
		wt += 16;

	if (menu_current_menu == 23)
		menu_y = 400;

	if (sfc1 != 0) {
		menu = new GLTile(sfc1);
		menu->set_hotspot(0, 0);

		if ((menu_prev_nmenus == 2 && (menu_current_menu != 3 || menu_selected[1] != 0) && menu_current_menu != 4) ||
		        (menu_state < MENU_CONSTANT && menu_options[1] != 0)) {
			float f = float(sqrt(enter_factor));
			menu->draw((320 - wt / 2)*(f) + (menu_x[0])*(1 - f), float(menu_y), 0, 0, 1);

			if (enter_factor == 1.0)
				menu_x[0] = 320 - wt / 2;
		} else {
			if (menu_option_type[0][menu_selected[0]] == 11 && menu_state > MENU_CONSTANT) {
				float f = float(sqrt(enter_factor));

				menu->draw((320 - wt / 2)*(f) + (320 - (wt*3) / 4)*(1 - f), float(menu_y), 0, 0, 1);
				menu_x[0] = int((320 - wt / 2) * (f) + (320 - (wt * 3) / 4) * (1 - f));
			} else {
				menu->draw(float(320 - wt / 2), float(menu_y), 0, 0, 1);
				menu_x[0] = 320 - wt / 2;
			} 
		} 

		delete menu;

		//  SDL_FreeSurface(sfc1);
	} 

	if (sfc2 != 0) {
		menu = new GLTile(sfc2);
		menu->set_hotspot(sfc2->w, 0);
		menu->draw(float(320 + wt / 2), float(menu_y), 0, 0, 1);
		delete menu;
		//  SDL_FreeSurface(sfc2);
	} 

} 

