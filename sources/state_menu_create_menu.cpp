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

#include <curl/curl.h>

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
#include "F1Shttp.h"
#include "RoadPiece.h"
#include "track.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritTrackViewer.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern int application_version;
extern int LISTENING_TIME;
extern int N_SFX_CHANNELS;
extern bool sound;
extern int SCREEN_X;
extern int SCREEN_Y;
extern bool network;
extern int network_tcp_port;
extern int network_udp_port;

extern char *f1spirit_replay_version;

void F1SpiritApp::menu_create_menu(void)
{


#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Generating a new menu: %i\n", menu_current_menu);
#endif

	switch (menu_current_menu) {

		case 0: {
				List<char> names;
				int nplayers = get_player_list(&names);

				if (nplayers > 0) {
					menu_title[0] = new char[strlen("CHOOSE AN OPTION") + 1];
					strcpy(menu_title[0], "CHOOSE AN OPTION");
					menu_options[0] = new char[strlen("CONTINUE GAME\nNEW GAME\nPRACTICE MODE\nTITLE SCREEN\n") + 1];
					strcpy(menu_options[0], "CONTINUE GAME\nNEW GAME\nPRACTICE MODE\nTITLE SCREEN\n");
					menu_noptions[0] = 4;
					menu_option_type[0][0] = 1;
					menu_option_type[0][1] = 1;
					menu_option_type[0][2] = 1;
					menu_option_type[0][3] = 0;
					menu_option_parameter[0][0] = 2;
					menu_option_parameter[0][1] = 1;
					menu_option_parameter[0][2] = 4;
					menu_first_option[0] = 0;
				} else {
					menu_title[0] = new char[strlen("CHOOSE AN OPTION") + 1];
					strcpy(menu_title[0], "CHOOSE AN OPTION");
					menu_options[0] = new char[strlen("NEW GAME\nPRACTICE MODE\nTITLE SCREEN\n") + 1];
					strcpy(menu_options[0], "NEW GAME\nPRACTICE MODE\nTITLE SCREEN\n");
					menu_noptions[0] = 3;
					menu_option_type[0][0] = 1;
					menu_option_type[0][1] = 1;
					menu_option_type[0][2] = 0;
					menu_option_parameter[0][0] = 1;
					menu_option_parameter[0][1] = 4;
					menu_first_option[0] = 0;
				} 
			}

			break;

		case 1:
			menu_title[0] = new char[strlen("ENTER YOUR NAME") + 1];
			strcpy(menu_title[0], "ENTER YOUR NAME");
			menu_options[0] = new char[strlen("_\nBACK\n") + 1];
			strcpy(menu_options[0], "_\nBACK\n");
			menu_editing[0] = 0;
			menu_noptions[0] = 2;
			menu_option_type[0][0] = 2;
			menu_option_type[0][1] = 1;
			menu_option_parameter[0][0] = 37;
			menu_option_parameter[0][1] = 0;
			menu_first_option[0] = 0;
			menu_editing_valid = false;
			break;

		case 37: {
				char tmp[80];

				sprintf(tmp, "PASSWORD FOR %s", menu_stored_player_name);
				menu_title[0] = new char[strlen(tmp) + 1];
				strcpy(menu_title[0], tmp);
				menu_options[0] = new char[strlen("_\nBACK\n") + 1];
				strcpy(menu_options[0], "_\nBACK\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 2;
				menu_option_type[0][0] = 24;
				menu_option_type[0][1] = 1;
				menu_option_parameter[0][0] = 4;
				menu_option_parameter[0][1] = 0;
				menu_first_option[0] = 0;
				menu_editing_valid = false;
			}

			break;

		case 2:
			menu_title[0] = new char[strlen("PREVIOUS GAMES") + 1];
			strcpy(menu_title[0], "PREVIOUS GAMES");

			{
				/* Create the player list: */
				List<char> names;
				char *name;
				int l = 0;
				int nplayers;

				nplayers = get_player_list(&names);

				names.Rewind();

				while (names.Iterate(name)) {
					l += strlen(name);
					l++;
				} 

				l += strlen("BACK\n") + 1;

				menu_options[0] = new char[l];

				l = 0;

				names.Rewind();

				while (names.Iterate(name)) {
					sprintf(menu_options[0] + l, "%s\n", name);
					l += strlen(name);
					l++;
				} 

				sprintf(menu_options[0] + l, "BACK\n");

				menu_noptions[0] = nplayers + 1;

				for (l = 0;l < nplayers;l++) {
					menu_option_type[0][l] = 11;
					menu_option_parameter[0][l] = 3;
				} 

				menu_option_type[0][nplayers] = 1;

				menu_option_parameter[0][nplayers] = 0;
			}

			menu_first_option[0] = 0;
			break;

		case 3:
			menu_title[0] = new char[strlen("PREVIOUS GAMES") + 1];
			strcpy(menu_title[0], "PREVIOUS GAMES");

			{
				/* Create the player list: */
				List<char> names;
				char *name;
				int l = 0;
				int nplayers;

				nplayers = get_player_list(&names);

				names.Rewind();

				while (names.Iterate(name)) {
					l += strlen(name);
					l++;
				} 

				l += strlen("BACK\n") + 1;

				menu_options[0] = new char[l];

				l = 0;

				names.Rewind();

				while (names.Iterate(name)) {
					sprintf(menu_options[0] + l, "%s\n", name);
					l += strlen(name);
					l++;
				} 

				sprintf(menu_options[0] + l, "BACK\n");

				menu_noptions[0] = nplayers + 1;

				for (l = 0;l < nplayers;l++) {
					menu_option_type[0][l] = 1;
					menu_option_parameter[0][l] = 3;
				} 

				menu_option_type[0][nplayers] = 1;

				menu_option_parameter[0][nplayers] = 0;

				menu_title[1] = 0;

				menu_options[1] = new char[strlen("CONTINUE GAME\nDELETE GAME\nBACK\n") + 1];

				strcpy(menu_options[1], "CONTINUE GAME\nDELETE GAME\nBACK\n");

				menu_noptions[1] = 3;

				menu_option_type[1][0] = 3;

				menu_option_type[1][1] = 4;

				menu_option_type[1][2] = 1;

				menu_option_parameter[1][0] = 4;

				menu_option_parameter[1][1] = 2;

				menu_option_parameter[1][2] = 2;

				menu_first_option[1] = 0;
			}

			break;

		case 4:

			if (current_player == 0) {
				FILE *fp;
				current_player = friendly_player;

				fp = f1open("players/friendly.dat", "rb", USERDATA);

				if (fp != 0) {
					current_player->load(fp, N_TRACKS, f1spirit_replay_version);
					strcpy(player_filename, "players/friendly.dat");
					fclose(fp);
				} else {
					fp = f1open("players/friendly.dat", "wb", USERDATA);

					if (fp != 0) {
						current_player->save(fp);
						strcpy(player_filename, "players/friendly.dat");
						fclose(fp);
					} 
				} 

			} 

			menu_title[0] = new char[strlen(current_player->get_name()) + 1];

			strcpy(menu_title[0], current_player->get_name());

			menu_options[0] = new char[strlen("PLAY\nCONFIGURE\nHELP\nHIGH SCORES\nREPLAYS\nWEB\nTITLE SCREEN\n") + 1];

			strcpy(menu_options[0], "PLAY\nCONFIGURE\nHELP\nHIGH SCORES\nREPLAYS\nWEB\nTITLE SCREEN\n");

			menu_noptions[0] = 7;

			menu_option_type[0][0] = 1;

			menu_option_type[0][1] = 1;

			menu_option_type[0][2] = 1;

			menu_option_type[0][3] = 9;

			menu_option_type[0][4] = 9;

			menu_option_type[0][5] = 1;

			menu_option_type[0][6] = 0;

			menu_option_parameter[0][0] = 25;

			menu_option_parameter[0][1] = 5;

			menu_option_parameter[0][2] = 23;

			menu_option_parameter[0][3] = 0;

			menu_option_parameter[0][4] = 1;

			menu_option_parameter[0][5] = 29;

			menu_first_option[0] = 0;

			break;

		case 25:
			menu_title[0] = new char[strlen("GAME MODE") + 1];

			strcpy(menu_title[0], "GAME MODE");

			menu_options[0] = new char[strlen("1 PLAYER\nMULTIPLAYER\nNETWORK\nBACK\n") + 1];

			strcpy(menu_options[0], "1 PLAYER\nMULTIPLAYER\nNETWORK\nBACK\n");

			menu_noptions[0] = 4;

			menu_option_type[0][0] = 1;

			menu_option_type[0][1] = 1;

			if (network)
				menu_option_type[0][2] = 1;
			else
				menu_option_type[0][2] = -1;

			menu_option_type[0][3] = 1;

			menu_option_parameter[0][0] = 9;

			menu_option_parameter[0][1] = 21;

			menu_option_parameter[0][2] = 30;

			menu_option_parameter[0][3] = 4;

			menu_first_option[0] = 0;

			break;

		case 5:

		case 6:

		case 7:

		case 8:

		case 19:

		case 20:

		case 22:

		case 24: {
				int i, l, pos;
				char *volumes[5] = {"NONE", "LOW ", "MED ", "HIGH", "MAX "};

				l = strlen("PLAYER 000\n") * current_player->get_nplayers() + strlen("MUSIC VOL: MAX \nSFX VOL: MAX \nBACK\n") + 1;
				menu_options[0] = new char[l];
				pos = 0;
				{
					int v1 = 0, v2 = 0;
					v1 = current_player->get_music_volume() / 32;
					v2 = current_player->get_sfx_volume() / 32;
					sprintf(menu_options[0], "MUSIC VOL: %s\nSFX VOL: %s\n", volumes[v1], volumes[v2]);
				}

				pos = strlen(menu_options[0]);

				for (i = 0;i < current_player->get_nplayers();i++) {
					sprintf(menu_options[0] + pos, "PLAYER %i\n", i + 1);
					pos = strlen(menu_options[0]);
				} 

				sprintf(menu_options[0] + pos, "BACK\n");

				menu_title[0] = new char[strlen(current_player->get_name()) + 1];

				strcpy(menu_title[0], current_player->get_name());

				menu_noptions[0] = current_player->get_nplayers() + 3;

				menu_option_type[0][0] = 15;

				menu_option_type[0][1] = 16;

				for (i = 0;i < current_player->get_nplayers();i++) {
					menu_option_type[0][i + 2] = 11;
					menu_option_parameter[0][i + 2] = 22;
				} 

				menu_option_type[0][current_player->get_nplayers() + 2] = 1;

				menu_option_parameter[0][current_player->get_nplayers() + 2] = 4;

				if (menu_current_menu == 5)
					menu_first_option[0] = 0;
			}

			if (menu_current_menu == 22) {
				menu_title[1] = 0;
				menu_options[1] = new char[strlen("KEYBOARD CFG\nJOYSTICK/KEYS\nBACK\n") + 1];
				strcpy(menu_options[1], "KEYBOARD CFG\nJOYSTICK/KEYS\nBACK\n");
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
				sprintf(tmp, "GEAR UP: %s\nGEAR DOWN: %s\nLEFT: %s\nRIGHT: %s\nACCEL: %s\nBRAKE: %s\nBACK\n",
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 0))),
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 1))),
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 2))),
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 3))),
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 4))),
				        SDL_GetKeyName(SDLKey(current_player->get_key(menu_selected[0] - 2, 5))));
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

			break;

		case 9:

		case 10:  /* F1 */

		case 44: { /* EXTRAS */
				int i;
				int n_tracks = 3;
				int points = 0;
				int length = 0, pos = 0;
				char *tracknames[7] = {"STOCK", "RALLY", "F3", "F3000", "ENDURANCE", "F1", "EXTRAS"};

				points = current_player->get_points();

				if (points >= 12)
					n_tracks = 5;

				if (points >= 25)
					n_tracks = 6;

				if (points >= 33)
					n_tracks = 8;

				if (points >= 48)
					n_tracks = 10;

				if (points >= 65)
					n_tracks = 12;

				if (points >= 84)
					n_tracks = 14;

				if (points >= 105)
					n_tracks = 16;

				if (points >= 128)
					n_tracks = 18;

				if (points >= 153)
					n_tracks = 20;

				if (points >= 180)
					n_tracks = 21;

				if (points >= 189)
					n_tracks = 22;

				if (cheat_max_point && current_player == friendly_player)
					n_tracks = 22;

				//   if (n_tracks>6) n_tracks=6;

				menu_title[0] = new char[strlen("SELECT TRACK") + 1];

				strcpy(menu_title[0], "SELECT TRACK");

				for (i = 0;i < 7;i++)
					length += strlen(tracknames[i]) + 1;

				length += strlen("BACK\n") + 1;

				menu_options[0] = new char[length];

				pos = 0;

				for (i = 0;i < 7;i++) {
					sprintf(menu_options[0] + pos, "%s\n", tracknames[i]);
					pos = strlen(menu_options[0]);
				} 

				sprintf(menu_options[0] + pos, "BACK\n");

				menu_editing[0] = 0;

				menu_noptions[0] = 8;

				for (i = 0;i < 6;i++) {
					if (i == 5) {
						if (i < n_tracks) {
							menu_option_type[0][i] = 11;
							menu_option_parameter[0][i] = 10;
						} else {
							menu_option_type[0][i] = -11;
							menu_option_parameter[0][i] = 10;
						} 
					} else {
						if (i < n_tracks) {
							menu_option_type[0][i] = 10;
							menu_option_parameter[0][i] = i;
						} else {
							menu_option_type[0][i] = -10;
							menu_option_parameter[0][i] = i;
						} 
					} 
				} 

				if (n_tracks < 22) {
					menu_option_type[0][6] = -11;
					menu_option_parameter[0][6] = 44;
					menu_first_option[0] = 0;
				} else {
					menu_option_type[0][6] = 11;
					menu_option_parameter[0][6] = 44;
					menu_first_option[0] = 0;
				} 

				menu_option_type[0][7] = 1;

				menu_option_parameter[0][7] = 4;

				menu_first_option[0] = 0;

				if (menu_current_menu == 10) {
					int i;
					int n_tracks = 3;
					int points = 0;
					int length = 0, pos = 0;
					char *f1names[16] = {"BRAZIL",
					                     "SAN MARINO",
					                     "BELGIUM",
					                     "MONACO",
					                     "USA",
					                     "FRANCE",
					                     "GREAT BRITAIN",
					                     "WEST GERMANY",
					                     "HUNGARY",
					                     "AUSTRIA",
					                     "ITALY",
					                     "PORTUGAL",
					                     "SPAIN",
					                     "MEXICO",
					                     "JAPAN",
					                     "AUSTRALIA"
					                    };

					points = current_player->get_points();

					if (points >= 12)
						n_tracks = 5;

					if (points >= 25)
						n_tracks = 6;

					if (points >= 33)
						n_tracks = 8;

					if (points >= 48)
						n_tracks = 10;

					if (points >= 65)
						n_tracks = 12;

					if (points >= 84)
						n_tracks = 14;

					if (points >= 105)
						n_tracks = 16;

					if (points >= 128)
						n_tracks = 18;

					if (points >= 153)
						n_tracks = 20;

					if (points >= 180)
						n_tracks = 21;

					if (points >= 189)
						n_tracks = 22;

					if (cheat_max_point && current_player == friendly_player)
						n_tracks = 22;

					n_tracks -= 5;

					menu_title[1] = 0;

					for (i = 0;i < 16;i++) {
						length += strlen(f1names[i]) + 1;
					} 

					length += strlen("BACK\n") + 1;

					menu_options[1] = new char[length];

					pos = 0;

					for (i = 0;i < 16;i++) {
						sprintf(menu_options[1] + pos, "%s\n", f1names[i]);
						pos = strlen(menu_options[1]);
					} 

					sprintf(menu_options[1] + pos, "BACK\n");

					menu_noptions[1] = 17;

					for (i = 0;i < 16;i++) {
						if (i < n_tracks) {
							menu_option_type[1][i] = 10;
							menu_option_parameter[1][i] = i + 5;
						} else {
							menu_option_type[1][i] = -10;
							menu_option_parameter[1][i] = i + 5;
						} 
					} 

					menu_option_type[1][16] = 1;

					menu_option_parameter[1][16] = 9;

					menu_first_option[1] = 0;
				} 

				if (menu_current_menu == 44) {
					int i;
					int length = 0, pos = 0;
					char *f1names[4] = {"OVAL STOCK",
					                    "OVAL F1",
					                    "MICRO STOCK",
					                    "MICRO F1"
					                   };

					menu_title[1] = 0;

					for (i = 0;i < 4;i++) {
						length += strlen(f1names[i]) + 1;
					} 

					length += strlen("BACK\n") + 1;

					menu_options[1] = new char[length];

					pos = 0;

					for (i = 0;i < 4;i++) {
						sprintf(menu_options[1] + pos, "%s\n", f1names[i]);
						pos = strlen(menu_options[1]);
					} 

					sprintf(menu_options[1] + pos, "BACK\n");

					menu_noptions[1] = 5;

					for (i = 0;i < 4;i++) {
						if (i < n_tracks) {
							menu_option_type[1][i] = 10;
							menu_option_parameter[1][i] = i + 21;
						} else {
							menu_option_type[1][i] = -10;
							menu_option_parameter[1][i] = i + 21;
						} 
					} 

					menu_option_type[1][4] = 1;

					menu_option_parameter[1][4] = 9;

					menu_first_option[1] = 0;
				} 
			}

			break;

		case 11: {
				char tmp[80];
				sprintf(tmp, "CAR SELECTION PLAYER %i", menu_selecting_player + 1);
				menu_title[0] = new char[strlen(tmp) + 1];
				strcpy(menu_title[0], tmp);
			}

			menu_options[0] = new char[strlen("READY MADE\nORIGINAL DESIGN\nBACK\n") + 1];
			strcpy(menu_options[0], "READY MADE\nORIGINAL DESIGN\nBACK\n");
			menu_noptions[0] = 3;
			menu_option_type[0][0] = 1;
			menu_option_type[0][1] = 1;
			menu_option_type[0][2] = 1;
			menu_option_parameter[0][0] = 12;
			menu_option_parameter[0][1] = 13;

			if (menu_selected_track >= 5) {
				if (menu_selected_track >= 21) {
					menu_option_parameter[0][2] = 44;
				} else {
					menu_option_parameter[0][2] = 10;
				} 
			} else {
				menu_option_parameter[0][2] = 9;
			} 

			menu_first_option[0] = 0;

			menu_selected_part[menu_selecting_player][0] = 0;

			menu_selected_part[menu_selecting_player][1] = 0;

			menu_selected_part[menu_selecting_player][2] = 0;

			menu_selected_part[menu_selecting_player][3] = 0;

			menu_selected_part[menu_selecting_player][4] = 0;

			break;

		case 12: {
				char tmp[80];
				sprintf(tmp, "CAR SELECTION PLAYER %i", menu_selecting_player + 1);
				menu_title[0] = new char[strlen(tmp) + 1];
				strcpy(menu_title[0], tmp);
			}

			switch (menu_selected_track) {

				case 0:

				case 21:
					menu_options[0] = new char[strlen("V8 2900\nV12 2800\nV8 3000\nBACK\n") + 1];
					strcpy(menu_options[0], "V8 2900\nV12 2800\nV8 3000\nBACK\n");
					break;

				case 1:
					menu_options[0] = new char[strlen("V6 2000\nV6 2996\nV8 5000\nBACK\n") + 1];
					strcpy(menu_options[0], "V6 2000\nV6 2996\nV8 5000\nBACK\n");
					break;

				case 2:
					menu_options[0] = new char[strlen("4R 1998\n4R 1998\n4R 2000\nBACK\n") + 1];
					strcpy(menu_options[0], "4R 1998\n4R 1998\n4R 2000\nBACK\n");
					break;

				case 3:
					menu_options[0] = new char[strlen("V8 3000\nV12 3000\nV8 2000\nBACK\n") + 1];
					strcpy(menu_options[0], "V8 3000\nV12 3000\nV8 2000\nBACK\n");
					break;

				case 4:
					menu_options[0] = new char[strlen("V8 3000\nV12 5993\nV8 2960\nBACK\n") + 1];
					strcpy(menu_options[0], "V8 3000\nV12 5993\nV8 2960\nBACK\n");
					break;

				default:
					menu_options[0] = new char[strlen("V6 1500\nV8 1400\nV6 1500\nBACK\n") + 1];
					strcpy(menu_options[0], "V6 1500\nV8 1400\nV6 1500\nBACK\n");
					break;
			} 

			menu_noptions[0] = 4;

			menu_option_type[0][0] = 13;

			menu_option_type[0][1] = 13;

			menu_option_type[0][2] = 13;

			menu_option_type[0][3] = 1;

			menu_option_parameter[0][0] = 0;

			menu_option_parameter[0][1] = 1;

			menu_option_parameter[0][2] = 2;

			menu_option_parameter[0][3] = 11;

			menu_first_option[0] = 0;

			break;

		case 13:

		case 14:

		case 15:

		case 16:

		case 17:

		case 18:

		case 28: {
				char tmp[80];
				sprintf(tmp, "CAR SELECTION PLAYER %i", menu_selecting_player + 1);
				menu_title[0] = new char[strlen(tmp) + 1];
				strcpy(menu_title[0], tmp);
			}

			menu_options[0] = new char[strlen("CHANGE CAR\nLOAD\nSAVE\nACCEPT\nBACK\n") + 1];

			strcpy(menu_options[0], "CHANGE CAR\nLOAD\nSAVE\nACCEPT\nBACK\n");
			menu_noptions[0] = 5;
			menu_option_type[0][0] = 11;
			menu_option_type[0][1] = 1;
			menu_option_type[0][2] = 1;
			menu_option_type[0][3] = 13;
			menu_option_type[0][4] = 1;
			menu_option_parameter[0][0] = 28;
			menu_option_parameter[0][1] = 27;
			menu_option_parameter[0][2] = 26;
			menu_option_parameter[0][3] = 3;
			menu_option_parameter[0][4] = 11;
			menu_first_option[0] = 0;

			if (menu_current_menu == 28) {
				menu_options[1] = new char[strlen("BODY\nENGINE\nBRAKE\nSUSPENSION\nGEAR\nBACK\n") + 1];
				strcpy(menu_options[1], "BODY\nENGINE\nBRAKE\nSUSPENSION\nGEAR\nBACK\n");
				menu_noptions[1] = 6;
				menu_option_type[1][0] = 11;
				menu_option_type[1][1] = 11;
				menu_option_type[1][2] = 11;
				menu_option_type[1][3] = 11;
				menu_option_type[1][4] = 11;
				menu_option_type[1][5] = 1;
				menu_option_parameter[1][0] = 14;
				menu_option_parameter[1][1] = 15;
				menu_option_parameter[1][2] = 16;
				menu_option_parameter[1][3] = 17;
				menu_option_parameter[1][4] = 18;
				menu_option_parameter[1][5] = 13;
				menu_first_option[1] = 0;
			} 

			if (menu_current_menu == 14) {
				menu_options[1] = new char[strlen("STRONG BODY\nBALANCED SETTING\nLIGHT WEIGHT\nBACK\n") + 1];
				strcpy(menu_options[1], "STRONG BODY\nBALANCED SETTING\nLIGHT WEIGHT\nBACK\n");
				menu_noptions[1] = 4;
				menu_option_type[1][0] = 12;
				menu_option_type[1][1] = 12;
				menu_option_type[1][2] = 12;
				menu_option_type[1][3] = 1;
				menu_option_parameter[1][0] = 0;
				menu_option_parameter[1][1] = 0;
				menu_option_parameter[1][2] = 0;
				menu_option_parameter[1][3] = 28;
				menu_first_option[1] = 0;
			} 

			if (menu_current_menu == 15) {
				switch (menu_selected_track) {

					case 0:

					case 21:
						menu_options[1] = new char[strlen("V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n");
						break;

					case 1:
						menu_options[1] = new char[strlen("V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n");
						break;

					case 2:
						menu_options[1] = new char[strlen("V6 2000\nV6 1800\nV8 2000\nV6 2000\nV6 2000\nV6 2000\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 2000\nV6 1800\nV8 2000\nV6 2000\nV6 2000\nV6 2000\nBACK\n");
						break;

					case 3:
						menu_options[1] = new char[strlen("V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n");
						break;

					case 4:
						menu_options[1] = new char[strlen("V6 1500\nV6 1490\nV8 1200\nV6 3000\nV8 3200\nV6 2400\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 1500\nV6 1490\nV8 1200\nV6 3000\nV8 3200\nV6 2400\nBACK\n");
						break;

					default:
						menu_options[1] = new char[strlen("V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n") + 1];
						strcpy(menu_options[1], "V6 1500\nV6 1490\nV8 1200\nV6 1500\nV8 1500\nV6 1400\nBACK\n");
						break;
				} 

				menu_noptions[1] = 7;

				menu_option_type[1][0] = 12;

				menu_option_type[1][1] = 12;

				menu_option_type[1][2] = 12;

				menu_option_type[1][3] = 12;

				menu_option_type[1][4] = 12;

				menu_option_type[1][5] = 12;

				menu_option_type[1][6] = 1;

				menu_option_parameter[1][0] = 1;

				menu_option_parameter[1][1] = 1;

				menu_option_parameter[1][2] = 1;

				menu_option_parameter[1][3] = 1;

				menu_option_parameter[1][4] = 1;

				menu_option_parameter[1][5] = 1;

				menu_option_parameter[1][6] = 28;

				menu_first_option[1] = 0;
			} 

			if (menu_current_menu == 16) {
				menu_options[1] = new char[strlen("WEAK AND STABLE\nBALANCED\nSTRONG BUT UNSTABLE\nBACK\n") + 1];
				strcpy(menu_options[1], "WEAK AND STABLE\nBALANCED\nSTRONG BUT UNSTABLE\nBACK\n");
				menu_noptions[1] = 4;
				menu_option_type[1][0] = 12;
				menu_option_type[1][1] = 12;
				menu_option_type[1][2] = 12;
				menu_option_type[1][3] = 1;
				menu_option_parameter[1][0] = 2;
				menu_option_parameter[1][1] = 2;
				menu_option_parameter[1][2] = 2;
				menu_option_parameter[1][3] = 28;
				menu_first_option[1] = 0;
			} 

			if (menu_current_menu == 17) {
				menu_options[1] = new char[strlen("SOFT-SETTING\nMEDIUM SETTING\nHARD-SETTING\nBACK\n") + 1];
				strcpy(menu_options[1], "SOFT-SETTING\nMEDIUM SETTING\nHARD-SETTING\nBACK\n");
				menu_noptions[1] = 4;
				menu_option_type[1][0] = 12;
				menu_option_type[1][1] = 12;
				menu_option_type[1][2] = 12;
				menu_option_type[1][3] = 1;
				menu_option_parameter[1][0] = 3;
				menu_option_parameter[1][1] = 3;
				menu_option_parameter[1][2] = 3;
				menu_option_parameter[1][3] = 28;
				menu_first_option[1] = 0;
			} 

			if (menu_current_menu == 18) {
				menu_options[1] = new char[strlen("AUTOMATIC\nMANUAL\nMANUAL HI-GEAR\nBACK\n") + 1];
				strcpy(menu_options[1], "AUTOMATIC\nMANUAL\nMANUAL HI-GEAR\nBACK\n");
				menu_noptions[1] = 4;
				menu_option_type[1][0] = 12;
				menu_option_type[1][1] = 12;
				menu_option_type[1][2] = 12;
				menu_option_type[1][3] = 1;
				menu_option_parameter[1][0] = 4;
				menu_option_parameter[1][1] = 4;
				menu_option_parameter[1][2] = 4;
				menu_option_parameter[1][3] = 28;
				menu_first_option[1] = 0;
			} 

			break;

		case 21:
			menu_title[0] = new char[strlen("Nº OF PLAYERS") + 1];

			strcpy(menu_title[0], "MULTIPLAYER");

			menu_options[0] = new char[strlen("2 PLAYERS\n3 PLAYERS\n4 PLAYERS\nBACK\n") + 1];

			strcpy(menu_options[0], "2 PLAYERS\n3 PLAYERS\n4 PLAYERS\nBACK\n");

			menu_noptions[0] = 4;

			menu_option_type[0][0] = 18;

			menu_option_type[0][1] = 18;

			menu_option_type[0][2] = 18;

			menu_option_type[0][3] = 1;

			menu_option_parameter[0][0] = 9;

			menu_option_parameter[0][1] = 9;

			menu_option_parameter[0][2] = 9;

			menu_option_parameter[0][3] = 4;

			menu_first_option[0] = 0;

			break;

		case 23:
			menu_title[0] = 0;

			menu_options[0] = new char[strlen("BACK\n") + 1];

			strcpy(menu_options[0], "BACK\n");

			menu_noptions[0] = 1;

			menu_option_type[0][0] = 1;

			menu_option_parameter[0][0] = 4;

			menu_first_option[0] = 0;

			break;

		case 26:
			/* SAVE CAR DESIGN: */
			menu_title[0] = new char[strlen("ENTER DESIGN NAME") + 1];

			strcpy(menu_title[0], "ENTER DESIGN NAME");

			menu_options[0] = new char[strlen("_\nBACK\n") + 1];

			strcpy(menu_options[0], "_\nBACK\n");

			menu_editing[0] = 0;

			menu_noptions[0] = 2;

			menu_option_type[0][0] = 22;

			menu_option_type[0][1] = 1;

			menu_option_parameter[0][0] = 13;

			menu_option_parameter[0][1] = 13;

			menu_first_option[0] = 0;

			menu_editing_valid = false;

			break;

		case 27:
			/* LOAD CAR DESIGN: */
			{
				int i, len = 0, pos = 0, noptions = 0;
				int type;
				List<char> names;
				char *folders[6] = {"designs/stock",
				                    "designs/rally",
				                    "designs/f3",
				                    "designs/f3000",
				                    "designs/endurance",
				                    "designs/f1"
				                   };
				char *tmp;
				type = min(menu_selected_track, 5);

				if (menu_selected_track == 21)
					type = 0;

				if (menu_selected_track == 22)
					type = 5;

#ifdef _WIN32
				/* Find files: */
				char filename[256];

				WIN32_FIND_DATA finfo;

				HANDLE h;

				sprintf(filename, "%s/*.car", folders[type]);

				h = FindFirstFile(filename, &finfo);

				if (h != INVALID_HANDLE_VALUE) {
					tmp = new char[strlen(finfo.cFileName) + 1];
					strcpy(tmp, finfo.cFileName);
					tmp[strlen(tmp) - 4] = 0;
					names.Add(tmp);
					len += strlen(tmp) + 1;
					noptions++;

					while (FindNextFile(h, &finfo) == TRUE) {
						tmp = new char[strlen(finfo.cFileName) + 1];
						strcpy(tmp, finfo.cFileName);
						tmp[strlen(tmp) - 4] = 0;
						names.Add(tmp);
						len += strlen(tmp) + 1;
						noptions++;
					} 
				} 

#else
				DIR *dp;

				struct dirent *ep;

				dp = opendir (folders[type]);

				if (dp != NULL) {
					while (ep = readdir (dp)) {
						if (strlen(ep->d_name) > 4 &&
						        ep->d_name[strlen(ep->d_name) - 4] == '.' &&
						        ep->d_name[strlen(ep->d_name) - 3] == 'c' &&
						        ep->d_name[strlen(ep->d_name) - 2] == 'a' &&
						        ep->d_name[strlen(ep->d_name) - 1] == 'r') {

							tmp = new char[strlen(ep->d_name) + 1];
							strcpy(tmp, ep->d_name);
							tmp[strlen(tmp) - 4] = 0;
							names.Add(tmp);
							len += strlen(tmp) + 1;
							noptions++;
						} 

					} 

					(void) closedir (dp);
				} 

#endif
				menu_title[0] = new char[strlen("LOAD CAR DESIGN") + 1];

				strcpy(menu_title[0], "LOAD CAR DESIGN");

				menu_options[0] = new char[strlen("BACK\n") + len + 1];

				names.Rewind();

				while (!names.EmptyP()) {
					tmp = names.ExtractIni();
					sprintf(menu_options[0] + pos, "%s\n", tmp);
					pos = strlen(menu_options[0]);
				} 

				sprintf(menu_options[0] + pos, "BACK\n");

				menu_noptions[0] = noptions + 1;

				for (i = 0;i < noptions + 1;i++) {
					menu_option_type[0][i] = 23;
					menu_option_parameter[0][i] = 13;
				} 

				menu_option_type[0][noptions] = 1;

				menu_option_parameter[0][noptions] = 13;

				menu_first_option[0] = 0;
			}

			break;

		case 29:
			/* ON-LINE SERVICES: */
			menu_title[0] = new char[strlen("ON-LINE SERVICES") + 1];
			strcpy(menu_title[0], "ON-LINE SERVICES");

			if (current_player == friendly_player) {
				menu_options[0] = new char[strlen("DOWNLOAD HIGHSCORES\nBACK\n") + 1];
				strcpy(menu_options[0], "DOWNLOAD HIGHSCORES\nBACK\n");
				menu_noptions[0] = 2;
				menu_option_type[0][0] = 26;
				menu_option_type[0][1] = 1;
				menu_option_parameter[0][0] = 36;
				menu_option_parameter[0][1] = 4;
				menu_first_option[0] = 0;
			} else {
				menu_options[0] = new char[strlen("REGISTER\nUPLOAD HIGHSCORES\nDOWNLOAD HIGHSCORES\nBACK\n") + 1];
				strcpy(menu_options[0], "REGISTER\nUPLOAD HIGHSCORES\nDOWNLOAD HIGHSCORES\nBACK\n");
				menu_noptions[0] = 4;
				menu_option_type[0][0] = 27;
				menu_option_type[0][1] = 28;
				menu_option_type[0][2] = 26;
				menu_option_type[0][3] = 1;
				menu_option_parameter[0][0] = 31;
				menu_option_parameter[0][1] = 35;
				menu_option_parameter[0][2] = 36;
				menu_option_parameter[0][3] = 4;
				menu_first_option[0] = 0;
			} 

			break;

		case 31: { /* HTTP REGISTER: */
				menu_title[0] = new char[strlen("REGISTERING...") + 1];
				strcpy(menu_title[0], "REGISTERING...");
				menu_options[0] = new char[strlen("CANCEL\n") + 1];
				strcpy(menu_options[0], "CANCEL\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 1;
				menu_option_type[0][0] = 25;
				menu_option_parameter[0][0] = 29;
				menu_first_option[0] = 0;
			}

			break;

		case 32: {
				menu_title[0] = new char[strlen("OPERATION SUCCEED") + 1];
				strcpy(menu_title[0], "OPERATION SUCCEED");
				menu_options[0] = new char[strlen("BACK\n") + 1];
				strcpy(menu_options[0], "BACK\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 1;
				menu_option_type[0][0] = 1;
				menu_option_parameter[0][0] = 29;
				menu_first_option[0] = 0;
			}

			break;

		case 33: {
				menu_title[0] = new char[strlen("OPERATION FAILED") + 1];
				strcpy(menu_title[0], "OPERATION FAILED");
				menu_options[0] = new char[strlen("BACK\n") + 1];
				strcpy(menu_options[0], "BACK\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 1;
				menu_option_type[0][0] = 1;
				menu_option_parameter[0][0] = 29;
				menu_first_option[0] = 0;
			}

			break;

		case 35: { /* UPLOADING RECORDS: */
				menu_title[0] = new char[strlen("UPLOADING...") + 1];
				strcpy(menu_title[0], "UPLOADING...");
				menu_options[0] = new char[strlen("CANCEL\n") + 1];
				strcpy(menu_options[0], "CANCEL\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 1;
				menu_option_type[0][0] = 1;
				menu_option_parameter[0][0] = 29;
				menu_first_option[0] = 0;
			}

			break;

		case 36: { /* DOWNLOADING RECORDS: */
				menu_title[0] = new char[strlen("DOWNLOADING...") + 1];
				strcpy(menu_title[0], "DOWNLOADING...");
				menu_options[0] = new char[strlen("CANCEL\n") + 1];
				strcpy(menu_options[0], "CANCEL\n");
				menu_editing[0] = 0;
				menu_noptions[0] = 1;
				menu_option_type[0][0] = 1;
				menu_option_parameter[0][0] = 29;
				menu_first_option[0] = 0;
			}

			break;

			/* SELECTION OF Nº OF ENEMY CARS AND OF ENEMY SPEED IN MULTIPLAYER MODES: */

		case 46: {
				char tmp[128];
				char *speed[] = {"SLOW", "NORMAL", "FAST"};
				menu_title[0] = new char[strlen("RACE SETTINGS") + 1];
				strcpy(menu_title[0], "RACE SETTINGS");
				sprintf(tmp, "ACCEPT\nENEMY CARS: %i\nENEMY SPEED: %s\nBACK\n",
				        menu_multiplayer_n_enemycars,
				        speed[menu_multiplayer_enemy_speed]);
				menu_options[0] = new char[strlen(tmp) + 1];
				strcpy(menu_options[0], tmp);
				menu_noptions[0] = 4;
				menu_option_type[0][0] = 1;
				menu_option_type[0][1] = 38;
				menu_option_type[0][2] = 39;
				menu_option_type[0][3] = 1;
				menu_option_parameter[0][0] = 11;
				menu_option_parameter[0][3] = 9;
				menu_first_option[0] = 0;
			}

			break;

			/* NETWORK MENUS: */

		case 30:
			menu_title[0] = new char[strlen("NETWORK GAME") + 1];
			strcpy(menu_title[0], "NETWORK GAME");
			menu_options[0] = new char[strlen("SERVER\nCLIENT\nBACK\n") + 1];
			strcpy(menu_options[0], "SERVER\nCLIENT\nBACK\n");
			menu_noptions[0] = 3;
			menu_option_type[0][0] = 1;
			menu_option_type[0][1] = 1; /* retrieve server list */
			menu_option_type[0][2] = 1;
			menu_option_parameter[0][0] = 34;
			menu_option_parameter[0][1] = 45;
			menu_option_parameter[0][2] = 4;
			menu_first_option[0] = 0;
			break;

		case 45:
			menu_title[0] = new char[strlen("ENTER CLIENT NAME:") + 1];
			strcpy(menu_title[0], "ENTER CLIENT NAME:");
			menu_options[0] = new char[strlen("_\nBACK\n") + 1];
			strcpy(menu_options[0], "_\nBACK\n");
			menu_editing[0] = 0;
			menu_noptions[0] = 2;
			menu_option_type[0][0] = 34;
			menu_option_type[0][1] = 1;
			menu_option_parameter[0][0] = 201;
			menu_option_parameter[0][1] = 30;
			menu_first_option[0] = 0;
			menu_editing_valid = false;
			break;

		case 34:
			menu_title[0] = new char[strlen("SERVER TYPE") + 1];
			strcpy(menu_title[0], "SERVER TYPE");
			menu_options[0] = new char[strlen("DEDICATED\nNORMAL PLAYER\nBACK\n") + 1];
			strcpy(menu_options[0], "DEDICATED\nNORMAL PLAYER\nBACK\n");
			menu_noptions[0] = 3;
			menu_option_type[0][0] = 1;
			menu_option_type[0][1] = 1;
			menu_option_type[0][2] = 1;
			menu_option_parameter[0][0] = 39;
			menu_option_parameter[0][1] = 39;
			menu_option_parameter[0][2] = 30;
			menu_first_option[0] = 0;
			break;

		case 39:
			menu_title[0] = new char[strlen("MAKE THIS SERVER PUBLIC?") + 1];
			strcpy(menu_title[0], "MAKE THIS SERVER PUBLIC?");
			menu_options[0] = new char[strlen("YES\nNO\nBACK\n") + 1];
			strcpy(menu_options[0], "YES\nNO\nBACK\n");
			menu_noptions[0] = 3;
			menu_option_type[0][0] = 1;
			menu_option_type[0][1] = 1;
			menu_option_type[0][2] = 1;
			menu_option_parameter[0][0] = 40;
			menu_option_parameter[0][1] = 43;
			menu_option_parameter[0][2] = 30;
			menu_first_option[0] = 0;
			break;

		case 40: /* SERVER NAME FOR PUBLIC SERVERS */
			menu_title[0] = new char[strlen("ENTER SERVER NAME:") + 1];
			strcpy(menu_title[0], "ENTER SERVER NAME:");
			menu_options[0] = new char[strlen("_\nBACK\n") + 1];
			strcpy(menu_options[0], "_\nBACK\n");
			menu_editing[0] = 0;
			menu_noptions[0] = 2;
			menu_option_type[0][0] = 29; /* REGISTER */
			menu_option_type[0][2] = 1;
			menu_option_parameter[0][0] = 41;
			menu_option_parameter[0][2] = 39;
			menu_first_option[0] = 0;
			menu_editing_valid = false;
			break;

		case 43: /* SERVER NAME FOR PRIVATE SERVERS */
			menu_title[0] = new char[strlen("ENTER SERVER NAME:") + 1];
			strcpy(menu_title[0], "ENTER SERVER NAME:");
			menu_options[0] = new char[strlen("_\nBACK\n") + 1];
			strcpy(menu_options[0], "_\nBACK\n");
			menu_editing[0] = 0;
			menu_noptions[0] = 2;
			menu_option_type[0][0] = 31;
			menu_option_type[0][1] = 1;
			menu_option_parameter[0][0] = 100;
			menu_option_parameter[0][1] = 39;
			menu_first_option[0] = 0;
			menu_editing_valid = false;
			break;

		case 41:
			/* registering server: */
			menu_title[0] = new char[strlen("REGISTERING SERVER...") + 1];
			strcpy(menu_title[0], "REGISTERING SERVER...");
			menu_options[0] = new char[strlen("CANCEL\n") + 1];
			strcpy(menu_options[0], "CANCEL\n");
			menu_noptions[0] = 1;
			menu_option_type[0][0] = 25;
			menu_option_parameter[0][0] = 39;
			menu_first_option[0] = 0;
			break;

		case 42:
			/* unable to register server: */
			menu_title[0] = new char[strlen("UNABLE TO REGISTER SERVER") + 1];
			strcpy(menu_title[0], "UNABLE TO REGISTER SERVER");
			menu_options[0] = new char[strlen("BACK\n") + 1];
			strcpy(menu_options[0], "BACK\n");
			menu_noptions[0] = 1;
			menu_option_type[0][0] = 1; /* IF REGISTERED, UNREGISTER */
			menu_option_parameter[0][0] = 39;
			menu_first_option[0] = 0;
			break;


		case 100:
			/* server chat room: */
			menu_title[0] = new char[strlen("F-1 SPIRIT SERVER") + 1];
			strcpy(menu_title[0], "F-1 SPIRIT SERVER");
			menu_options[0] = new char[strlen("_\nTERMINATE\n") + 1];
			strcpy(menu_options[0], "_\nTERMINATE\n");
			menu_editing[0] = 0;
			menu_noptions[0] = 2;
			menu_option_type[0][0] = 32; /* CHAT MESSAGE */
			menu_option_parameter[0][0] = 0;

			if (menu_server_registered) {
				menu_option_type[0][1] = 30; /* IF REGISTERED, UNREGISTER */
				menu_option_parameter[0][1] = 101;
			} else {
				menu_option_type[0][1] = 1;
				menu_option_parameter[0][1] = 4;
			} 

			menu_first_option[0] = 0;

			break;

		case 101:
			/* unregistering server: */
			menu_title[0] = new char[strlen("UNREGISTERING SERVER...") + 1];

			strcpy(menu_title[0], "UNREGISTERING SERVER...");

			menu_options[0] = new char[strlen("CANCEL\n") + 1];

			strcpy(menu_options[0], "CANCEL\n");

			menu_noptions[0] = 1;

			menu_option_type[0][0] = 25;

			menu_option_parameter[0][0] = 100;

			menu_first_option[0] = 0;

			break;

		case 200:
			/* client chat room: */
			menu_title[0] = new char[strlen("F-1 SPIRIT CLIENT") + 1];

			strcpy(menu_title[0], "F-1 SPIRIT CLIENT");

			menu_options[0] = new char[strlen("_\nTERMINATE\n") + 1];

			strcpy(menu_options[0], "_\nTERMINATE\n");

			menu_noptions[0] = 1;

			menu_editing[0] = 0;

			menu_noptions[0] = 2;

			menu_option_type[0][0] = 32; /* CHAT MESSAGE */

			menu_option_parameter[0][0] = 0;

			menu_option_type[0][1] = 37; /* IF REGISTERED, UNREGISTER */

			menu_option_parameter[0][1] = 4;

			menu_first_option[0] = 0;

			break;


		case 201:
			menu_title[0] = new char[strlen("LOOKING FOR SERVERS...") + 1];

			strcpy(menu_title[0], "LOOKING FOR SERVERS...");

			menu_options[0] = new char[strlen("CANCEL\nBACK\n") + 1];

			strcpy(menu_options[0], "CANCEL\nBACK\n");

			menu_noptions[0] = 2;

			menu_option_type[0][0] = 25;

			menu_option_parameter[0][0] = 202;

			menu_option_type[0][1] = 25;

			menu_option_parameter[0][1] = 30;

			menu_first_option[0] = 0;

			break;

		case 202: {
				char *base1 = "IP: _\n";
				char *base2 = "BACK\n";
				char *tmp;
				int len, i;
				Uint32 ip;
				unsigned char *ipp;
				List<F1SComputer> l;
				F1SComputer *c;

				len = strlen(base1) + strlen(base2) + 1 + (32) * menu_available_servers.Length();
				tmp = new char[len];

				sprintf(tmp, base1);
				l.Instance(menu_available_servers);
				l.Rewind();

				while (l.Iterate(c)) {
					i = strlen(tmp);
					ip = SDLNet_Read32(&(c->ip.host));
					ipp = (unsigned char *) & ip;
					sprintf(tmp + i, "%i.%i.%i.%i: %s\n", int(ipp[3]), int(ipp[2]), int(ipp[1]), int(ipp[0]), c->name);
				} 

				i = strlen(tmp);

				sprintf(tmp + i, base2);

				menu_title[0] = new char[strlen("SERVER LIST") + 1];

				strcpy(menu_title[0], "SERVER LIST");

				menu_options[0] = new char[strlen(tmp) + 1];

				strcpy(menu_options[0], tmp);

				delete tmp;

				menu_noptions[0] = 2 + menu_available_servers.Length();

				menu_option_type[0][0] = 35;

				menu_option_parameter[0][0] = 200;

				for (i = 0;i < menu_available_servers.Length();i++) {
					menu_option_type[0][i + 1] = 36;
					menu_option_parameter[0][i + 1] = 200;
				} 

				menu_option_type[0][i + 1] = 1;

				menu_option_parameter[0][i + 1] = 30;

				menu_first_option[0] = 0;

				menu_editing[0] = 0;

				menu_editing_valid = false;
			}

			break;

		case 203:
			/* Waiting for server answer (after a request to register) */
			menu_title[0] = new char[strlen("WAITING FOR SERVER...") + 1];
			strcpy(menu_title[0], "WAITING FOR SERVER...");
			menu_options[0] = new char[strlen("CANCEL\n") + 1];
			strcpy(menu_options[0], "CANCEL\n");
			menu_noptions[0] = 1;
			menu_option_type[0][0] = 1;
			menu_option_parameter[0][0] = 201;
			menu_first_option[0] = 0;
			break;

	} 


	if ((menu_options[1] == 0 && menu_prev_nmenus != 2 && !menu_force_rebuild_menu) ||
	        menu_current_menu == 4 ||
	        menu_current_menu == 11) {
		menu_selected[0] = 0;
		menu_selected_timmer[0] = 0;
	} 

	if (!menu_redefining_key && !menu_force_rebuild_menu)
		menu_selected[1] = 0;

	if (!menu_redefining_key && !menu_force_rebuild_menu)
		menu_selected_timmer[1] = 0;

	if (!menu_redefining_key && !menu_force_rebuild_menu)
		menu_state = 0;
} 
