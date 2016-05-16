#ifdef _WIN32
#include "windows.h"
#include "glut.h"
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
#include "ReplayInfo.h"
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif



CReplayInfo::CReplayInfo(char *fn, char *folder)
{
	FILE *fp;
	bool found;
	int i;
	int nversions = 5;
	char *versions[] = {"F1S000", "F1SAC6", "F1SAC7", "F1SAC8", "F1SAC9"};
	// F1SAC8 is the same as F1SAC7 but without the camera and scoreboard information
	//
	char tmp[256];

	version_tag[0] = 0;
	decoded = false;
	filename = 0;
	track_name = 0;
	player = 0;
	times = 0;
	length = 0;

	sprintf(tmp, "%s/%s", folder, fn);

	if (strcmp(folder, "demos") == 0)
		fp = f1open(tmp, "rb", GAMEDATA);
	else
		fp = f1open(tmp, "rb", USERDATA);

	if (fp != 0) {
		filename = new char[strlen(fn) + 1];
		strcpy(filename, fn);

		version_tag[0] = fgetc(fp);
		version_tag[1] = fgetc(fp);
		version_tag[2] = fgetc(fp);
		version_tag[3] = fgetc(fp);
		version_tag[4] = fgetc(fp);
		version_tag[5] = fgetc(fp);
		version_tag[6] = 0;

		for (found = false, i = 0;!found && i < nversions;i++) {
			if (strcmp(versions[i], version_tag) == 0) {
				found = true;

				switch (i) {

					case 0:
						version000_decoder(fp);
						break;

					case 1:
						version_ac6_decoder(fp);
						break;

					case 2:
						version_ac7_decoder(fp);
						break;

					case 3:
						version_ac8_decoder(fp);
						break;

					case 4:
						version_ac9_decoder(fp);
						break;

					default:
						break;
				} 
			} 
		} 

		fclose(fp);
	} 

} 


CReplayInfo::~CReplayInfo()
{} 


bool CReplayInfo::replay_cycle(int cycle, KEYBOARDSTATE *k, F1SpiritGame *game)
{
	CReplayEvent *e = 0;
	int i;

	for (i = 0;i < k->k_size;i++) {
		k->old_keyboard[i] = k->keyboard[i];
	} 

	do {
		e = events.GetObj();

		if (e != 0) {
			if (e->cycle <= cycle) {
				events.Next();
#ifdef F1SPIRIT_DEBUG_MESSAGES

				output_debug_message("Replay cycle %i: event %i\n", e->cycle, e->type);
#endif

				switch (e->type) {

					case 0:
#ifdef F1SPIRIT_DEBUG_MESSAGES

						output_debug_message("Replay cycle %i: keyboard event %i [%i,%i]\n", e->cycle, e->key, e->state, k->old_keyboard[e->key]);
#endif

						k->keyboard[e->key] = e->state;
						break;

					case 1:
#ifdef F1SPIRIT_DEBUG_MESSAGES

						output_debug_message("Replay cycle %i: control frame\n", e->cycle);
#endif
						/* Test that all the cars are in their proper positions: */
						{
							CReplayCarStatus *control;
							PlayerCCar *pcar;
							EnemyCCar *ecar;

							int i;

							for (i = 0;i < e->n_playercars;i++) {
								control = e->playercar_status[i];
								pcar = game->player_cars[i];

								if (pcar->car->tcar->x != control->x ||
								        pcar->car->tcar->y != control->y ||
								        pcar->car->tcar->z != control->z ||
								        pcar->car->tcar->angle != control->a ||

								        pcar->car->speed != control->speed ||
								        pcar->car->speed_x != control->speed_x ||
								        pcar->car->speed_y != control->speed_y ||
								        pcar->car->speed_a != control->speed_a ||

								        pcar->car->fuel != control->fuel ||
								        pcar->car->current_gear != control->current_gear ||
								        pcar->car->rpm != control->rpm ||
								        pcar->car->brake_timmer != control->brake_timmer ||
								        pcar->car->turn_timmer != control->turn_timmer ||
								        pcar->car->old_speed_x != control->old_speed_x ||
								        pcar->car->old_speed_y != control->old_speed_y ||
								        pcar->car->state_timmer != control->state_timmer ||
								        pcar->car->state != control->state ||

								        pcar->car->damage_ftyre != control->damage_ftyre ||
								        pcar->car->damage_rtyre != control->damage_rtyre ||
								        pcar->car->damage_brake != control->damage_brake ||
								        pcar->car->damage_engine != control->damage_engine) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
									output_debug_message("Player car %i has an error in its parameters, corrected\n", i);
#endif

									pcar->car->tcar->x = control->x;
									pcar->car->tcar->y = control->y;
									pcar->car->tcar->z = control->z;
									pcar->car->tcar->angle = control->a;

									pcar->car->speed = control->speed;
									pcar->car->speed_x = control->speed_x;
									pcar->car->speed_y = control->speed_y;
									pcar->car->speed_a = control->speed_a;

									pcar->car->fuel = control->fuel;
									pcar->car->current_gear = control->current_gear;
									pcar->car->rpm = control->rpm;
									pcar->car->brake_timmer = control->brake_timmer;
									pcar->car->turn_timmer = control->turn_timmer;
									pcar->car->old_speed_x = control->old_speed_x;
									pcar->car->old_speed_y = control->old_speed_y;
									pcar->car->state_timmer = control->state_timmer;
									pcar->car->state = control->state;

									pcar->car->damage_ftyre = control->damage_ftyre;
									pcar->car->damage_rtyre = control->damage_rtyre;
									pcar->car->damage_brake = control->damage_brake;
									pcar->car->damage_engine = control->damage_engine;
								} 
							} 

							for (i = 0;i < e->n_enemycars;i++) {
								control = e->enemycar_status[i];
								ecar = game->enemy_cars[i];

								if (ecar != 0 &&
								        (ecar->road_offset_change != control->road_offset_change ||
								         ecar->road_offset != control->road_offset ||

								         ecar->car->tcar->x != control->x ||
								         ecar->car->tcar->y != control->y ||
								         ecar->car->tcar->z != control->z ||
								         ecar->car->tcar->angle != control->a ||

								         ecar->car->speed != control->speed ||
								         ecar->car->speed_x != control->speed_x ||
								         ecar->car->speed_y != control->speed_y ||
								         ecar->car->speed_a != control->speed_a ||

								         ecar->car->fuel != control->fuel ||
								         ecar->car->current_gear != control->current_gear ||
								         ecar->car->rpm != control->rpm ||
								         ecar->car->brake_timmer != control->brake_timmer ||
								         ecar->car->turn_timmer != control->turn_timmer ||
								         ecar->car->old_speed_x != control->old_speed_x ||
								         ecar->car->old_speed_y != control->old_speed_y ||
								         ecar->car->state_timmer != control->state_timmer ||
								         ecar->car->state != control->state ||

								         ecar->car->damage_ftyre != control->damage_ftyre ||
								         ecar->car->damage_rtyre != control->damage_rtyre ||
								         ecar->car->damage_brake != control->damage_brake ||
								         ecar->car->damage_engine != control->damage_engine)) {

#ifdef F1SPIRIT_DEBUG_MESSAGES
									output_debug_message("Enemy car %i has an error in its parameters, corrected\n", i);
#endif

									ecar->road_offset_change = control->road_offset_change;
									ecar->road_offset = control->road_offset;

									ecar->car->tcar->x = control->x;
									ecar->car->tcar->y = control->y;
									ecar->car->tcar->z = control->z;
									ecar->car->tcar->angle = control->a;

									ecar->car->speed = control->speed;
									ecar->car->speed_x = control->speed_x;
									ecar->car->speed_y = control->speed_y;
									ecar->car->speed_a = control->speed_a;

									ecar->car->fuel = control->fuel;
									ecar->car->current_gear = control->current_gear;
									ecar->car->rpm = control->rpm;
									ecar->car->brake_timmer = control->brake_timmer;
									ecar->car->turn_timmer = control->turn_timmer;
									ecar->car->old_speed_x = control->old_speed_x;
									ecar->car->old_speed_y = control->old_speed_y;
									ecar->car->state_timmer = control->state_timmer;
									ecar->car->state = control->state;

									ecar->car->damage_ftyre = control->damage_ftyre;
									ecar->car->damage_rtyre = control->damage_rtyre;
									ecar->car->damage_brake = control->damage_brake;
									ecar->car->damage_engine = control->damage_engine;
								} 

							} 
						}
						break;

					case 2:
						return false;
						break;

					case 4:
						return false;
						break;
				} 
			} 
		} 
	} while (events.GetObj() != 0 &&
	         events.GetObj()->cycle <= cycle);

	return true;
} 


F1SpiritGame *CReplayInfo::create_game(SDL_Surface *font, KEYBOARDSTATE *k)
{
	return new F1SpiritGame(this, font, k);
} 


/* Decoders for different versions of F1-Spirit: */
void CReplayInfo::version000_decoder(FILE *fp)
{
	int i, j, l;
	int last_cycle = 0;
	int cycle;
	int *laps;
	bool end;
	int nlaps[21] = {4, 3, 4, 3, 3,
	                 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
	                };
	CReplayEvent *event;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Decoding replay file...\n");
#endif

	year = fgetc(fp);
	month = fgetc(fp);
	day = fgetc(fp);
	hour = fgetc(fp);
	min = fgetc(fp);
	sec = fgetc(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Replay date: %i-%i-%i, %i:%i:%i\n", day, month + 1, year + 1900, hour, min, sec);
#endif

	parameters.load_bin_AC5(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Game parameters retrieved\n");
#endif

	player = new CPlayerInfo();
	player->load_ac5(fp);

	track_num = fgetc(fp);
	l = fgetc(fp);
	track_name = new char[l + 1];

	for (i = 0;i < l;i++)
		track_name[i] = fgetc(fp);

	track_name[i] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track name: %s\n", track_name);

#endif

	n_enemy_cars = fgetc(fp);

	if (n_enemy_cars > 0) {
		ec_type = new int[n_enemy_cars];
		ec_r = new float[n_enemy_cars];
		ec_g = new float[n_enemy_cars];
		ec_b = new float[n_enemy_cars];
		ec_body = new int[n_enemy_cars];
		ec_engine = new int[n_enemy_cars];
		ec_brake = new int[n_enemy_cars];
		ec_suspension = new int[n_enemy_cars];
		ec_gear = new int[n_enemy_cars];
		ec_handycap = new float[n_enemy_cars];
		ec_AI_type = new int[n_enemy_cars];
	} 

	for (i = 0;i < n_enemy_cars;i++) {
		ec_AI_type[i] = fgetc(fp);
		ec_type[i] = fgetc(fp);
		load_float(fp, &ec_r[i]);
		load_float(fp, &ec_g[i]);
		load_float(fp, &ec_b[i]);
		ec_body[i] = fgetc(fp);
		ec_engine[i] = fgetc(fp);
		ec_brake[i] = fgetc(fp);
		ec_suspension[i] = fgetc(fp);
		ec_gear[i] = fgetc(fp);
		load_float(fp, &ec_handycap[i]);

		for (j = 0;j < 79;j++)
			fgetc(fp);
	} 

	n_player_cars = fgetc(fp);

	if (n_player_cars > 0) {
		pc_type = new int[n_player_cars];
		pc_r = new float[n_player_cars];
		pc_g = new float[n_player_cars];
		pc_b = new float[n_player_cars];
		pc_body = new int[n_player_cars];
		pc_engine = new int[n_player_cars];
		pc_brake = new int[n_player_cars];
		pc_suspension = new int[n_player_cars];
		pc_gear = new int[n_player_cars];
		pc_handycap = new float[n_player_cars];
	} 

	for (i = 0;i < n_player_cars;i++) {
		pc_type[i] = fgetc(fp);
		load_float(fp, &pc_r[i]);
		load_float(fp, &pc_g[i]);
		load_float(fp, &pc_b[i]);
		pc_body[i] = fgetc(fp);
		pc_engine[i] = fgetc(fp);
		pc_brake[i] = fgetc(fp);
		pc_suspension[i] = fgetc(fp);
		pc_gear[i] = fgetc(fp);
		load_float(fp, &pc_handycap[i]);

		for (j = 0;j < 73;j++)
			fgetc(fp);
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Cars: %i enemy + %i player\n", n_enemy_cars, n_player_cars);

#endif

	/* compute player times + replay length: */
	end = false;

	laps = new int[n_player_cars];

	times = new int[n_player_cars];

	for (j = 0;j < n_player_cars;j++) {
		laps[j] = 0;
		times[j] = 0;
	} 

	do {
		cycle = fgetc(fp);
		cycle += fgetc(fp) * 256;
		last_cycle = cycle;
		event = new CReplayEvent();
		event->cycle = cycle;
		event->type = fgetc(fp);

		switch (event->type) {

			case 0:
				event->key = fgetc(fp);
				event->key += fgetc(fp) * 256;
				event->state = fgetc(fp);
				events.Add(event);
				break;

			case 1:
				event->n_enemycars = fgetc(fp);
				event->enemycar_status = new CReplayCarStatus * [event->n_enemycars];

				for (j = 0;j < event->n_enemycars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->enemycar_status[j] = cs;

					cs->road_offset_change = fgetc(fp);
					cs->road_offset_change += fgetc(fp) * 256;

					if (cs->road_offset_change == 65535)
						cs->road_offset_change = -1;

					load_float(fp, &(cs->road_offset));

					load_float(fp, &(cs->x));

					load_float(fp, &(cs->y));

					load_float(fp, &(cs->z));

					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));

					load_float(fp, &(cs->speed_x));

					load_float(fp, &(cs->speed_y));

					load_float(fp, &(cs->speed_a));

					load_float(fp, &(cs->fuel));

					cs->current_gear = fgetc(fp);

					load_float(fp, &(cs->rpm));

					cs->brake_timmer = fgetc(fp);

					cs->brake_timmer += fgetc(fp) * 256;

					cs->turn_timmer = fgetc(fp);

					cs->turn_timmer += fgetc(fp) * 256;

					load_float(fp, &(cs->old_speed_x));

					load_float(fp, &(cs->old_speed_y));

					cs->state_timmer = fgetc(fp);

					cs->state_timmer += fgetc(fp) * 256;

					cs->state = fgetc(fp);

					cs->state += fgetc(fp) * 256;

					load_float(fp, &(cs->damage_ftyre));

					load_float(fp, &(cs->damage_rtyre));

					load_float(fp, &(cs->damage_brake));

					load_float(fp, &(cs->damage_engine));
				} 

				event->n_playercars = fgetc(fp);

				event->playercar_status = new CReplayCarStatus * [event->n_playercars];

				for (j = 0;j < event->n_playercars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->playercar_status[j] = cs;

					load_float(fp, &(cs->x));
					load_float(fp, &(cs->y));
					load_float(fp, &(cs->z));
					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));
					load_float(fp, &(cs->speed_x));
					load_float(fp, &(cs->speed_y));
					load_float(fp, &(cs->speed_a));
					load_float(fp, &(cs->fuel));
					cs->current_gear = fgetc(fp);
					load_float(fp, &(cs->rpm));
					cs->brake_timmer = fgetc(fp);
					cs->brake_timmer += fgetc(fp) * 256;
					cs->turn_timmer = fgetc(fp);
					cs->turn_timmer += fgetc(fp) * 256;
					load_float(fp, &(cs->old_speed_x));
					load_float(fp, &(cs->old_speed_y));
					cs->state_timmer = fgetc(fp);
					cs->state_timmer += fgetc(fp) * 256;
					cs->state = fgetc(fp);
					cs->state += fgetc(fp) * 256;
					load_float(fp, &(cs->damage_ftyre));
					load_float(fp, &(cs->damage_rtyre));
					load_float(fp, &(cs->damage_brake));
					load_float(fp, &(cs->damage_engine));
				} 

				events.Add(event);

				break;

			case 2:
				events.Add(event);

				end = true;

				break;

			case 3:
				j = fgetc(fp);

				laps[j]++;

				times[j] = cycle;

				delete event;

				break;

			case 4:
				events.Add(event);

				end = true;

				break;

			default:
				delete event;

				break;
		} 
	} while (!end && !feof(fp));

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Events: %i\n", events.Length());

#endif

	length = last_cycle * 2;

	for (j = 0;j < n_player_cars;j++) {
		if (laps[j] < nlaps[track_num]) {
			times[j] = 0;
		} else {
			times[j] = (times[j] - (50 + 256)) * 2;
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Replay decoding finished.\n");

#endif

	decoded = true;
} 



/* Decoders for different versions of F1-Spirit: */
void CReplayInfo::version_ac6_decoder(FILE *fp)
{
	int i, j, l;
	int last_cycle = 0;
	int cycle;
	int *laps;
	bool end;
	int nlaps[N_TRACKS] = {4, 3, 4, 3, 3,
	                       4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                       6, 6, 6, 6
	                      };
	CReplayEvent *event;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Decoding replay file...\n");
#endif

	year = fgetc(fp);
	month = fgetc(fp);
	day = fgetc(fp);
	hour = fgetc(fp);
	min = fgetc(fp);
	sec = fgetc(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Replay date: %i-%i-%i, %i:%i:%i\n", day, month + 1, year + 1900, hour, min, sec);
#endif

	parameters.load_bin_AC8(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Game parameters retrieved\n");
#endif

	player = new CPlayerInfo();
	player->load(fp, 21, "F1SAC6");

	track_num = fgetc(fp);
	l = fgetc(fp);
	track_name = new char[l + 1];

	for (i = 0;i < l;i++)
		track_name[i] = fgetc(fp);

	track_name[i] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track name: %s\n", track_name);

#endif

	n_enemy_cars = fgetc(fp);

	if (n_enemy_cars > 0) {
		ec_type = new int[n_enemy_cars];
		ec_r = new float[n_enemy_cars];
		ec_g = new float[n_enemy_cars];
		ec_b = new float[n_enemy_cars];
		ec_body = new int[n_enemy_cars];
		ec_engine = new int[n_enemy_cars];
		ec_brake = new int[n_enemy_cars];
		ec_suspension = new int[n_enemy_cars];
		ec_gear = new int[n_enemy_cars];
		ec_handycap = new float[n_enemy_cars];
		ec_AI_type = new int[n_enemy_cars];
	} 

	for (i = 0;i < n_enemy_cars;i++) {
		ec_AI_type[i] = fgetc(fp);
		ec_type[i] = fgetc(fp);
		load_float(fp, &ec_r[i]);
		load_float(fp, &ec_g[i]);
		load_float(fp, &ec_b[i]);
		ec_body[i] = fgetc(fp);
		ec_engine[i] = fgetc(fp);
		ec_brake[i] = fgetc(fp);
		ec_suspension[i] = fgetc(fp);
		ec_gear[i] = fgetc(fp);
		load_float(fp, &ec_handycap[i]);

		for (j = 0;j < 79;j++)
			fgetc(fp);
	} 

	n_player_cars = fgetc(fp);

	if (n_player_cars > 0) {
		pc_type = new int[n_player_cars];
		pc_r = new float[n_player_cars];
		pc_g = new float[n_player_cars];
		pc_b = new float[n_player_cars];
		pc_body = new int[n_player_cars];
		pc_engine = new int[n_player_cars];
		pc_brake = new int[n_player_cars];
		pc_suspension = new int[n_player_cars];
		pc_gear = new int[n_player_cars];
		pc_handycap = new float[n_player_cars];
	} 

	for (i = 0;i < n_player_cars;i++) {
		pc_type[i] = fgetc(fp);
		load_float(fp, &pc_r[i]);
		load_float(fp, &pc_g[i]);
		load_float(fp, &pc_b[i]);
		pc_body[i] = fgetc(fp);
		pc_engine[i] = fgetc(fp);
		pc_brake[i] = fgetc(fp);
		pc_suspension[i] = fgetc(fp);
		pc_gear[i] = fgetc(fp);
		load_float(fp, &pc_handycap[i]);

		for (j = 0;j < 73;j++)
			fgetc(fp);
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Cars: %i enemy + %i player\n", n_enemy_cars, n_player_cars);

#endif

	/* compute player times + replay length: */
	end = false;

	laps = new int[n_player_cars];

	times = new int[n_player_cars];

	for (j = 0;j < n_player_cars;j++) {
		laps[j] = 0;
		times[j] = 0;
	} 

	do {
		cycle = fgetc(fp);
		cycle += fgetc(fp) * 256;
		last_cycle = cycle;
		event = new CReplayEvent();
		event->cycle = cycle;
		event->type = fgetc(fp);

		switch (event->type) {

			case 0:
				event->key = fgetc(fp);
				event->key += fgetc(fp) * 256;
				event->state = fgetc(fp);
				events.Add(event);
				break;

			case 1:
				event->n_enemycars = fgetc(fp);
				event->enemycar_status = new CReplayCarStatus * [event->n_enemycars];

				for (j = 0;j < event->n_enemycars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->enemycar_status[j] = cs;

					cs->road_offset_change = fgetc(fp);
					cs->road_offset_change += fgetc(fp) * 256;

					if (cs->road_offset_change == 65535)
						cs->road_offset_change = -1;

					load_float(fp, &(cs->road_offset));

					load_float(fp, &(cs->x));

					load_float(fp, &(cs->y));

					load_float(fp, &(cs->z));

					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));

					load_float(fp, &(cs->speed_x));

					load_float(fp, &(cs->speed_y));

					load_float(fp, &(cs->speed_a));

					load_float(fp, &(cs->fuel));

					cs->current_gear = fgetc(fp);

					load_float(fp, &(cs->rpm));

					cs->brake_timmer = fgetc(fp);

					cs->brake_timmer += fgetc(fp) * 256;

					cs->turn_timmer = fgetc(fp);

					cs->turn_timmer += fgetc(fp) * 256;

					load_float(fp, &(cs->old_speed_x));

					load_float(fp, &(cs->old_speed_y));

					cs->state_timmer = fgetc(fp);

					cs->state_timmer += fgetc(fp) * 256;

					cs->state = fgetc(fp);

					cs->state += fgetc(fp) * 256;

					load_float(fp, &(cs->damage_ftyre));

					load_float(fp, &(cs->damage_rtyre));

					load_float(fp, &(cs->damage_brake));

					load_float(fp, &(cs->damage_engine));
				} 

				event->n_playercars = fgetc(fp);

				event->playercar_status = new CReplayCarStatus * [event->n_playercars];

				for (j = 0;j < event->n_playercars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->playercar_status[j] = cs;

					load_float(fp, &(cs->x));
					load_float(fp, &(cs->y));
					load_float(fp, &(cs->z));
					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));
					load_float(fp, &(cs->speed_x));
					load_float(fp, &(cs->speed_y));
					load_float(fp, &(cs->speed_a));
					load_float(fp, &(cs->fuel));
					cs->current_gear = fgetc(fp);
					load_float(fp, &(cs->rpm));
					cs->brake_timmer = fgetc(fp);
					cs->brake_timmer += fgetc(fp) * 256;
					cs->turn_timmer = fgetc(fp);
					cs->turn_timmer += fgetc(fp) * 256;
					load_float(fp, &(cs->old_speed_x));
					load_float(fp, &(cs->old_speed_y));
					cs->state_timmer = fgetc(fp);
					cs->state_timmer += fgetc(fp) * 256;
					cs->state = fgetc(fp);
					cs->state += fgetc(fp) * 256;
					load_float(fp, &(cs->damage_ftyre));
					load_float(fp, &(cs->damage_rtyre));
					load_float(fp, &(cs->damage_brake));
					load_float(fp, &(cs->damage_engine));
				} 

				events.Add(event);

				break;

			case 2:
				events.Add(event);

				end = true;

				break;

			case 3:
				j = fgetc(fp);

				laps[j]++;

				times[j] = cycle;

				delete event;

				break;

			case 4:
				events.Add(event);

				end = true;

				break;

			default:
				delete event;

				break;
		} 
	} while (!end && !feof(fp));

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Events: %i\n", events.Length());

#endif

	length = last_cycle * 2;

	for (j = 0;j < n_player_cars;j++) {
		if (laps[j] < nlaps[track_num]) {
			times[j] = 0;
		} else {
			times[j] = (times[j] - (50 + 256)) * 2;
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Replay decoding finished.\n");

#endif

	decoded = true;
} 




/* Decoders for different versions of F1-Spirit: */
void CReplayInfo::version_ac7_decoder(FILE *fp)
{
	int i, j, l;
	int last_cycle = 0;
	int cycle;
	int *laps;
	bool end;
	int nlaps[N_TRACKS] = {4, 3, 4, 3, 3,
	                       4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                       6, 6, 6, 6
	                      };
	CReplayEvent *event;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Decoding replay file...\n");
#endif

	year = fgetc(fp);
	month = fgetc(fp);
	day = fgetc(fp);
	hour = fgetc(fp);
	min = fgetc(fp);
	sec = fgetc(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Replay date: %i-%i-%i, %i:%i:%i\n", day, month + 1, year + 1900, hour, min, sec);
#endif

	parameters.load_bin_AC8(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Game parameters retrieved\n");
#endif

	player = new CPlayerInfo();
	player->load(fp, N_TRACKS, "F1SAC7");

	track_num = fgetc(fp);
	l = fgetc(fp);
	track_name = new char[l + 1];

	for (i = 0;i < l;i++)
		track_name[i] = fgetc(fp);

	track_name[i] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track name: %s\n", track_name);

#endif

	n_enemy_cars = fgetc(fp);

	if (n_enemy_cars > 0) {
		ec_type = new int[n_enemy_cars];
		ec_r = new float[n_enemy_cars];
		ec_g = new float[n_enemy_cars];
		ec_b = new float[n_enemy_cars];
		ec_body = new int[n_enemy_cars];
		ec_engine = new int[n_enemy_cars];
		ec_brake = new int[n_enemy_cars];
		ec_suspension = new int[n_enemy_cars];
		ec_gear = new int[n_enemy_cars];
		ec_handycap = new float[n_enemy_cars];
		ec_AI_type = new int[n_enemy_cars];
	} 

	for (i = 0;i < n_enemy_cars;i++) {
		ec_AI_type[i] = fgetc(fp);
		ec_type[i] = fgetc(fp);
		load_float(fp, &ec_r[i]);
		load_float(fp, &ec_g[i]);
		load_float(fp, &ec_b[i]);
		ec_body[i] = fgetc(fp);
		ec_engine[i] = fgetc(fp);
		ec_brake[i] = fgetc(fp);
		ec_suspension[i] = fgetc(fp);
		ec_gear[i] = fgetc(fp);
		load_float(fp, &ec_handycap[i]);

		for (j = 0;j < 79;j++)
			fgetc(fp);
	} 

	n_player_cars = fgetc(fp);

	if (n_player_cars > 0) {
		pc_type = new int[n_player_cars];
		pc_r = new float[n_player_cars];
		pc_g = new float[n_player_cars];
		pc_b = new float[n_player_cars];
		pc_body = new int[n_player_cars];
		pc_engine = new int[n_player_cars];
		pc_brake = new int[n_player_cars];
		pc_suspension = new int[n_player_cars];
		pc_gear = new int[n_player_cars];
		pc_handycap = new float[n_player_cars];
	} 

	for (i = 0;i < n_player_cars;i++) {
		pc_type[i] = fgetc(fp);
		load_float(fp, &pc_r[i]);
		load_float(fp, &pc_g[i]);
		load_float(fp, &pc_b[i]);
		pc_body[i] = fgetc(fp);
		pc_engine[i] = fgetc(fp);
		pc_brake[i] = fgetc(fp);
		pc_suspension[i] = fgetc(fp);
		pc_gear[i] = fgetc(fp);
		load_float(fp, &pc_handycap[i]);

		for (j = 0;j < 73;j++)
			fgetc(fp);
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Cars: %i enemy + %i player\n", n_enemy_cars, n_player_cars);

#endif

	/* compute player times + replay length: */
	end = false;

	laps = new int[n_player_cars];

	times = new int[n_player_cars];

	for (j = 0;j < n_player_cars;j++) {
		laps[j] = 0;
		times[j] = 0;
	} 

	do {
		cycle = fgetc(fp);
		cycle += fgetc(fp) * 256;
		last_cycle = cycle;
		event = new CReplayEvent();
		event->cycle = cycle;
		event->type = fgetc(fp);

		switch (event->type) {

			case 0:
				event->key = fgetc(fp);
				event->key += fgetc(fp) * 256;
				event->state = fgetc(fp);
				events.Add(event);
				break;

			case 1:
				event->n_enemycars = fgetc(fp);
				event->enemycar_status = new CReplayCarStatus * [event->n_enemycars];

				for (j = 0;j < event->n_enemycars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->enemycar_status[j] = cs;

					cs->road_offset_change = fgetc(fp);
					cs->road_offset_change += fgetc(fp) * 256;

					if (cs->road_offset_change == 65535)
						cs->road_offset_change = -1;

					load_float(fp, &(cs->road_offset));

					load_float(fp, &(cs->x));

					load_float(fp, &(cs->y));

					load_float(fp, &(cs->z));

					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));

					load_float(fp, &(cs->speed_x));

					load_float(fp, &(cs->speed_y));

					load_float(fp, &(cs->speed_a));

					load_float(fp, &(cs->fuel));

					cs->current_gear = fgetc(fp);

					load_float(fp, &(cs->rpm));

					cs->brake_timmer = fgetc(fp);

					cs->brake_timmer += fgetc(fp) * 256;

					cs->turn_timmer = fgetc(fp);

					cs->turn_timmer += fgetc(fp) * 256;

					load_float(fp, &(cs->old_speed_x));

					load_float(fp, &(cs->old_speed_y));

					cs->state_timmer = fgetc(fp);

					cs->state_timmer += fgetc(fp) * 256;

					cs->state = fgetc(fp);

					cs->state += fgetc(fp) * 256;

					load_float(fp, &(cs->damage_ftyre));

					load_float(fp, &(cs->damage_rtyre));

					load_float(fp, &(cs->damage_brake));

					load_float(fp, &(cs->damage_engine));
				} 

				event->n_playercars = fgetc(fp);

				event->playercar_status = new CReplayCarStatus * [event->n_playercars];

				for (j = 0;j < event->n_playercars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->playercar_status[j] = cs;

					load_float(fp, &(cs->x));
					load_float(fp, &(cs->y));
					load_float(fp, &(cs->z));
					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));
					load_float(fp, &(cs->speed_x));
					load_float(fp, &(cs->speed_y));
					load_float(fp, &(cs->speed_a));
					load_float(fp, &(cs->fuel));
					cs->current_gear = fgetc(fp);
					load_float(fp, &(cs->rpm));
					cs->brake_timmer = fgetc(fp);
					cs->brake_timmer += fgetc(fp) * 256;
					cs->turn_timmer = fgetc(fp);
					cs->turn_timmer += fgetc(fp) * 256;
					load_float(fp, &(cs->old_speed_x));
					load_float(fp, &(cs->old_speed_y));
					cs->state_timmer = fgetc(fp);
					cs->state_timmer += fgetc(fp) * 256;
					cs->state = fgetc(fp);
					cs->state += fgetc(fp) * 256;
					load_float(fp, &(cs->damage_ftyre));
					load_float(fp, &(cs->damage_rtyre));
					load_float(fp, &(cs->damage_brake));
					load_float(fp, &(cs->damage_engine));
				} 

				events.Add(event);

				break;

			case 2:
				events.Add(event);

				end = true;

				break;

			case 3:
				j = fgetc(fp);

				laps[j]++;

				times[j] = cycle;

				delete event;

				break;

			case 4:
				events.Add(event);

				end = true;

				break;

			default:
				delete event;

				break;
		} 
	} while (!end && !feof(fp));

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Events: %i\n", events.Length());

#endif

	length = last_cycle * 2;

	for (j = 0;j < n_player_cars;j++) {
		if (laps[j] < nlaps[track_num]) {
			times[j] = 0;
		} else {
			times[j] = (times[j] - (50 + 256)) * 2;
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Replay decoding finished.\n");

#endif

	decoded = true;
} 



void CReplayInfo::version_ac8_decoder(FILE *fp)
{
	int i, j, l;
	int last_cycle = 0;
	int cycle;
	int *laps;
	bool end;
	int nlaps[N_TRACKS] = {4, 3, 4, 3, 3,
	                       4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                       6, 6, 6, 6
	                      };
	CReplayEvent *event;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Decoding replay file...\n");
#endif

	year = fgetc(fp);
	month = fgetc(fp);
	day = fgetc(fp);
	hour = fgetc(fp);
	min = fgetc(fp);
	sec = fgetc(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Replay date: %i-%i-%i, %i:%i:%i\n", day, month + 1, year + 1900, hour, min, sec);
#endif

	parameters.load_bin_AC8(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Game parameters retrieved\n");
#endif

	player = new CPlayerInfo();
	player->load(fp, N_TRACKS, "F1SAC8" );

	track_num = fgetc(fp);
	l = fgetc(fp);
	track_name = new char[l + 1];

	for (i = 0;i < l;i++)
		track_name[i] = fgetc(fp);

	track_name[i] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track name: %s\n", track_name);

#endif

	n_enemy_cars = fgetc(fp);

	if (n_enemy_cars > 0) {
		ec_type = new int[n_enemy_cars];
		ec_r = new float[n_enemy_cars];
		ec_g = new float[n_enemy_cars];
		ec_b = new float[n_enemy_cars];
		ec_body = new int[n_enemy_cars];
		ec_engine = new int[n_enemy_cars];
		ec_brake = new int[n_enemy_cars];
		ec_suspension = new int[n_enemy_cars];
		ec_gear = new int[n_enemy_cars];
		ec_handycap = new float[n_enemy_cars];
		ec_AI_type = new int[n_enemy_cars];
	} 

	for (i = 0;i < n_enemy_cars;i++) {
		ec_AI_type[i] = fgetc(fp);
		ec_type[i] = fgetc(fp);
		load_float(fp, &ec_r[i]);
		load_float(fp, &ec_g[i]);
		load_float(fp, &ec_b[i]);
		ec_body[i] = fgetc(fp);
		ec_engine[i] = fgetc(fp);
		ec_brake[i] = fgetc(fp);
		ec_suspension[i] = fgetc(fp);
		ec_gear[i] = fgetc(fp);
		load_float(fp, &ec_handycap[i]);

		for (j = 0;j < 79;j++)
			fgetc(fp);
	} 

	n_player_cars = fgetc(fp);

	if (n_player_cars > 0) {
		pc_type = new int[n_player_cars];
		pc_r = new float[n_player_cars];
		pc_g = new float[n_player_cars];
		pc_b = new float[n_player_cars];
		pc_body = new int[n_player_cars];
		pc_engine = new int[n_player_cars];
		pc_brake = new int[n_player_cars];
		pc_suspension = new int[n_player_cars];
		pc_gear = new int[n_player_cars];
		pc_handycap = new float[n_player_cars];
	} 

	for (i = 0;i < n_player_cars;i++) {
		pc_type[i] = fgetc(fp);
		load_float(fp, &pc_r[i]);
		load_float(fp, &pc_g[i]);
		load_float(fp, &pc_b[i]);
		pc_body[i] = fgetc(fp);
		pc_engine[i] = fgetc(fp);
		pc_brake[i] = fgetc(fp);
		pc_suspension[i] = fgetc(fp);
		pc_gear[i] = fgetc(fp);
		load_float(fp, &pc_handycap[i]);

		for (j = 0;j < 73;j++)
			fgetc(fp);
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Cars: %i enemy + %i player\n", n_enemy_cars, n_player_cars);

#endif

	/* compute player times + replay length: */
	end = false;

	laps = new int[n_player_cars];

	times = new int[n_player_cars];

	for (j = 0;j < n_player_cars;j++) {
		laps[j] = 0;
		times[j] = 0;
	} 

	do {
		cycle = fgetc(fp);
		cycle += fgetc(fp) * 256;
		last_cycle = cycle;
		event = new CReplayEvent();
		event->cycle = cycle;
		event->type = fgetc(fp);

		switch (event->type) {

			case 0:
				event->key = fgetc(fp);
				event->key += fgetc(fp) * 256;
				event->state = fgetc(fp);
				events.Add(event);
				break;

			case 1:
				event->n_enemycars = fgetc(fp);
				event->enemycar_status = new CReplayCarStatus * [event->n_enemycars];

				for (j = 0;j < event->n_enemycars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->enemycar_status[j] = cs;

					cs->road_offset_change = fgetc(fp);
					cs->road_offset_change += fgetc(fp) * 256;

					if (cs->road_offset_change == 65535)
						cs->road_offset_change = -1;

					load_float(fp, &(cs->road_offset));

					load_float(fp, &(cs->x));

					load_float(fp, &(cs->y));

					load_float(fp, &(cs->z));

					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));

					load_float(fp, &(cs->speed_x));

					load_float(fp, &(cs->speed_y));

					load_float(fp, &(cs->speed_a));

					load_float(fp, &(cs->fuel));

					cs->current_gear = fgetc(fp);

					load_float(fp, &(cs->rpm));

					cs->brake_timmer = fgetc(fp);

					cs->brake_timmer += fgetc(fp) * 256;

					cs->turn_timmer = fgetc(fp);

					cs->turn_timmer += fgetc(fp) * 256;

					load_float(fp, &(cs->old_speed_x));

					load_float(fp, &(cs->old_speed_y));

					cs->state_timmer = fgetc(fp);

					cs->state_timmer += fgetc(fp) * 256;

					cs->state = fgetc(fp);

					cs->state += fgetc(fp) * 256;

					load_float(fp, &(cs->damage_ftyre));

					load_float(fp, &(cs->damage_rtyre));

					load_float(fp, &(cs->damage_brake));

					load_float(fp, &(cs->damage_engine));
				} 

				event->n_playercars = fgetc(fp);

				event->playercar_status = new CReplayCarStatus * [event->n_playercars];

				for (j = 0;j < event->n_playercars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->playercar_status[j] = cs;

					load_float(fp, &(cs->x));
					load_float(fp, &(cs->y));
					load_float(fp, &(cs->z));
					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));
					load_float(fp, &(cs->speed_x));
					load_float(fp, &(cs->speed_y));
					load_float(fp, &(cs->speed_a));
					load_float(fp, &(cs->fuel));
					cs->current_gear = fgetc(fp);
					load_float(fp, &(cs->rpm));
					cs->brake_timmer = fgetc(fp);
					cs->brake_timmer += fgetc(fp) * 256;
					cs->turn_timmer = fgetc(fp);
					cs->turn_timmer += fgetc(fp) * 256;
					load_float(fp, &(cs->old_speed_x));
					load_float(fp, &(cs->old_speed_y));
					cs->state_timmer = fgetc(fp);
					cs->state_timmer += fgetc(fp) * 256;
					cs->state = fgetc(fp);
					cs->state += fgetc(fp) * 256;
					load_float(fp, &(cs->damage_ftyre));
					load_float(fp, &(cs->damage_rtyre));
					load_float(fp, &(cs->damage_brake));
					load_float(fp, &(cs->damage_engine));
				} 

				events.Add(event);

				break;

			case 2:
				events.Add(event);

				end = true;

				break;

			case 3:
				j = fgetc(fp);

				laps[j]++;

				times[j] = cycle;

				delete event;

				break;

			case 4:
				events.Add(event);

				end = true;

				break;

			default:
				delete event;

				break;
		} 
	} while (!end && !feof(fp));

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Events: %i\n", events.Length());

#endif

	length = last_cycle * 2;

	for (j = 0;j < n_player_cars;j++) {
		if (laps[j] < nlaps[track_num]) {
			times[j] = 0;
		} else {
			times[j] = (times[j] - (50 + 256)) * 2;
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Replay decoding finished.\n");

#endif

	decoded = true;
} 




void CReplayInfo::version_ac9_decoder(FILE *fp)
{
	int i, j, l;
	int last_cycle = 0;
	int cycle;
	int *laps;
	bool end;
	int nlaps[N_TRACKS] = {4, 3, 4, 3, 3,
	                       4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                       6, 6, 6, 6
	                      };
	CReplayEvent *event;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Decoding replay file...\n");
#endif

	year = fgetc(fp);
	month = fgetc(fp);
	day = fgetc(fp);
	hour = fgetc(fp);
	min = fgetc(fp);
	sec = fgetc(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Replay date: %i-%i-%i, %i:%i:%i\n", day, month + 1, year + 1900, hour, min, sec);
#endif

	parameters.load_bin(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Game parameters retrieved\n");
#endif

	player = new CPlayerInfo();
	player->load(fp, N_TRACKS, "F1SAC9" );

	track_num = fgetc(fp);
	l = fgetc(fp);
	track_name = new char[l + 1];

	for (i = 0;i < l;i++)
		track_name[i] = fgetc(fp);

	track_name[i] = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track name: %s\n", track_name);

#endif

	n_enemy_cars = fgetc(fp);

	if (n_enemy_cars > 0) {
		ec_type = new int[n_enemy_cars];
		ec_r = new float[n_enemy_cars];
		ec_g = new float[n_enemy_cars];
		ec_b = new float[n_enemy_cars];
		ec_body = new int[n_enemy_cars];
		ec_engine = new int[n_enemy_cars];
		ec_brake = new int[n_enemy_cars];
		ec_suspension = new int[n_enemy_cars];
		ec_gear = new int[n_enemy_cars];
		ec_handycap = new float[n_enemy_cars];
		ec_AI_type = new int[n_enemy_cars];
	} 

	for (i = 0;i < n_enemy_cars;i++) {
		ec_AI_type[i] = fgetc(fp);
		ec_type[i] = fgetc(fp);
		load_float(fp, &ec_r[i]);
		load_float(fp, &ec_g[i]);
		load_float(fp, &ec_b[i]);
		ec_body[i] = fgetc(fp);
		ec_engine[i] = fgetc(fp);
		ec_brake[i] = fgetc(fp);
		ec_suspension[i] = fgetc(fp);
		ec_gear[i] = fgetc(fp);
		load_float(fp, &ec_handycap[i]);

		for (j = 0;j < 79;j++)
			fgetc(fp);
	} 

	n_player_cars = fgetc(fp);

	if (n_player_cars > 0) {
		pc_type = new int[n_player_cars];
		pc_r = new float[n_player_cars];
		pc_g = new float[n_player_cars];
		pc_b = new float[n_player_cars];
		pc_body = new int[n_player_cars];
		pc_engine = new int[n_player_cars];
		pc_brake = new int[n_player_cars];
		pc_suspension = new int[n_player_cars];
		pc_gear = new int[n_player_cars];
		pc_handycap = new float[n_player_cars];
	} 

	for (i = 0;i < n_player_cars;i++) {
		pc_type[i] = fgetc(fp);
		load_float(fp, &pc_r[i]);
		load_float(fp, &pc_g[i]);
		load_float(fp, &pc_b[i]);
		pc_body[i] = fgetc(fp);
		pc_engine[i] = fgetc(fp);
		pc_brake[i] = fgetc(fp);
		pc_suspension[i] = fgetc(fp);
		pc_gear[i] = fgetc(fp);
		load_float(fp, &pc_handycap[i]);

		for (j = 0;j < 73;j++)
			fgetc(fp);
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Cars: %i enemy + %i player\n", n_enemy_cars, n_player_cars);

#endif

	/* compute player times + replay length: */
	end = false;

	laps = new int[n_player_cars];

	times = new int[n_player_cars];

	for (j = 0;j < n_player_cars;j++) {
		laps[j] = 0;
		times[j] = 0;
	} 

	do {
		cycle = fgetc(fp);
		cycle += fgetc(fp) * 256;
		last_cycle = cycle;
		event = new CReplayEvent();
		event->cycle = cycle;
		event->type = fgetc(fp);

		switch (event->type) {

			case 0:
				event->key = fgetc(fp);
				event->key += fgetc(fp) * 256;
				event->state = fgetc(fp);
				events.Add(event);
				break;

			case 1:
				event->n_enemycars = fgetc(fp);
				event->enemycar_status = new CReplayCarStatus * [event->n_enemycars];

				for (j = 0;j < event->n_enemycars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->enemycar_status[j] = cs;

					cs->road_offset_change = fgetc(fp);
					cs->road_offset_change += fgetc(fp) * 256;

					if (cs->road_offset_change == 65535)
						cs->road_offset_change = -1;

					load_float(fp, &(cs->road_offset));

					load_float(fp, &(cs->x));

					load_float(fp, &(cs->y));

					load_float(fp, &(cs->z));

					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));

					load_float(fp, &(cs->speed_x));

					load_float(fp, &(cs->speed_y));

					load_float(fp, &(cs->speed_a));

					load_float(fp, &(cs->fuel));

					cs->current_gear = fgetc(fp);

					load_float(fp, &(cs->rpm));

					cs->brake_timmer = fgetc(fp);

					cs->brake_timmer += fgetc(fp) * 256;

					cs->turn_timmer = fgetc(fp);

					cs->turn_timmer += fgetc(fp) * 256;

					load_float(fp, &(cs->old_speed_x));

					load_float(fp, &(cs->old_speed_y));

					cs->state_timmer = fgetc(fp);

					cs->state_timmer += fgetc(fp) * 256;

					cs->state = fgetc(fp);

					cs->state += fgetc(fp) * 256;

					load_float(fp, &(cs->damage_ftyre));

					load_float(fp, &(cs->damage_rtyre));

					load_float(fp, &(cs->damage_brake));

					load_float(fp, &(cs->damage_engine));
				} 

				event->n_playercars = fgetc(fp);

				event->playercar_status = new CReplayCarStatus * [event->n_playercars];

				for (j = 0;j < event->n_playercars;j++) {
					CReplayCarStatus *cs;
					cs = new CReplayCarStatus();
					event->playercar_status[j] = cs;

					load_float(fp, &(cs->x));
					load_float(fp, &(cs->y));
					load_float(fp, &(cs->z));
					load_float(fp, &(cs->a));

					load_float(fp, &(cs->speed));
					load_float(fp, &(cs->speed_x));
					load_float(fp, &(cs->speed_y));
					load_float(fp, &(cs->speed_a));
					load_float(fp, &(cs->fuel));
					cs->current_gear = fgetc(fp);
					load_float(fp, &(cs->rpm));
					cs->brake_timmer = fgetc(fp);
					cs->brake_timmer += fgetc(fp) * 256;
					cs->turn_timmer = fgetc(fp);
					cs->turn_timmer += fgetc(fp) * 256;
					load_float(fp, &(cs->old_speed_x));
					load_float(fp, &(cs->old_speed_y));
					cs->state_timmer = fgetc(fp);
					cs->state_timmer += fgetc(fp) * 256;
					cs->state = fgetc(fp);
					cs->state += fgetc(fp) * 256;
					load_float(fp, &(cs->damage_ftyre));
					load_float(fp, &(cs->damage_rtyre));
					load_float(fp, &(cs->damage_brake));
					load_float(fp, &(cs->damage_engine));
				} 

				events.Add(event);

				break;

			case 2:
				events.Add(event);

				end = true;

				break;

			case 3:
				j = fgetc(fp);

				laps[j]++;

				times[j] = cycle;

				delete event;

				break;

			case 4:
				events.Add(event);

				end = true;

				break;

			default:
				delete event;

				break;
		} 
	} while (!end && !feof(fp));

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Events: %i\n", events.Length());

#endif

	length = last_cycle * 2;

	for (j = 0;j < n_player_cars;j++) {
		if (laps[j] < nlaps[track_num]) {
			times[j] = 0;
		} else {
			times[j] = (times[j] - (50 + 256)) * 2;
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Replay decoding finished.\n");

#endif

	decoded = true;
} 





CReplayEvent::CReplayEvent()
{
	cycle = 0;
	type = 0;
	key = 0;
	state = 0;
	n_playercars = 0;
	playercar_status = 0;
	n_enemycars = 0;
	enemycar_status = 0;
} 


CReplayEvent::~CReplayEvent()
{
	if (n_playercars > 0) {
		int i;

		for (i = 0;i < n_playercars;i++) {
			delete playercar_status[i];
			playercar_status[i] = 0;
		} 

		delete playercar_status;
	} 

	if (n_enemycars > 0) {
		int i;

		for (i = 0;i < n_enemycars;i++) {
			delete enemycar_status[i];
			enemycar_status[i] = 0;
		} 

		delete enemycar_status;
	} 
} 


