#ifdef _WIN32
#include "windows.h"
#include "glut.h"
#else
#include <sys/time.h>
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

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
#include "randomc.h"
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#define SEMAPHORE_TIMMER 64


extern int N_SFX_CHANNELS;
extern bool sound;
extern int g_stencil_bits;

extern TRanrotBGenerator *rg;


char *f1spirit_replay_version = "F1SAC9";
/* commands: */
/* 0 : keyboard event */
/* 1 : master frame */
/* 2 : end of file */
/* 3 : player passed by the start line */


bool cars_order(RacingCCar *c1, RacingCCar *c2)
{
	return c1->position >= c2->position;
} /* cars_order */


int race_laps[N_TRACKS] = {4, 3, 4, 3, 3,
                           4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                           6, 6, 6, 6
                          };


void F1SpiritGame::initialize_track(int ntrack)
{
	int i;

	switch (ntrack) {

		case 21:

		case 23:

		case 0:
			race_car_sfc = IMG_Load("graphics/cars-stock.png");
			break;

		case 1:
			race_car_sfc = IMG_Load("graphics/cars-rally.png");
			break;

		case 2:
			race_car_sfc = IMG_Load("graphics/cars-f3.png");
			break;

		case 3:
			race_car_sfc = IMG_Load("graphics/cars-f3000.png");
			break;

		case 4:
			race_car_sfc = IMG_Load("graphics/cars-endurance.png");
			break;

		default:
			race_car_sfc = IMG_Load("graphics/cars-f1.png");
			break;
	} 

	race_minimap_car_sfc = IMG_Load("graphics/minimap-car.png");

	race_minimap_enemycar_sfc = IMG_Load("graphics/minimap-enemycar.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/cars-[car-type].png: %p\n", race_car_sfc);

	output_debug_message("graphics/minimap-car.png: %p\n", race_minimap_car_sfc);

	output_debug_message("graphics/minimap-enemycar.png: %p\n", race_minimap_enemycar_sfc);

#endif

	race_minitrack = 0;

	/* 107 x 183 */
	race_semaphore[0] = new GLTile("graphics/semaphore.png", 0, 0, 107, 183);

	race_semaphore[1] = new GLTile("graphics/semaphore.png", 107, 0, 107, 183);

	race_semaphore[2] = new GLTile("graphics/semaphore.png", 214, 0, 107, 183);

	race_semaphore[3] = new GLTile("graphics/semaphore.png", 321, 0, 107, 183);

	race_semaphore[4] = new GLTile("graphics/semaphore.png", 428, 0, 107, 183);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/semaphore: %p,%p,%p,%p\n", race_semaphore[0], race_semaphore[1], race_semaphore[2], race_semaphore[3]);

#endif

	race_lap_tile[0] = new GLTile("graphics/laps.png", 0, 0, 32, 36);

	race_lap_tile[1] = new GLTile("graphics/laps.png", 32, 0, 32, 36);

	race_lap_tile[2] = new GLTile("graphics/laps.png", 64, 0, 32, 36);

	race_lap_tile[3] = new GLTile("graphics/laps.png", 96, 0, 32, 36);

	race_lap_tile[4] = new GLTile("graphics/laps.png", 128, 0, 32, 36);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/laps: %p,%p,%p,%p\n", race_lap_tile[0], race_lap_tile[1], race_lap_tile[2], race_lap_tile[3], race_lap_tile[4]);

#endif

	for (i = 0;i < 9;i++) {
		race_rpm_tile[i] = new GLTile("graphics/rpm.png", 0, 48 * i, 160, 48);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("graphics/rpm %i: %p\n", i, race_rpm_tile[i]);
#endif

	} 

	race_extra_tiles[0] = new GLTile("graphics/extras.png", 0, 0, 8, 8);

	race_extra_tiles[0]->set_hotspot(4, 4);

	race_extra_tiles[1] = new GLTile("graphics/extras.png", 8, 0, 8, 8);

	race_extra_tiles[1]->set_hotspot(4, 4);

	race_extra_tiles[2] = new GLTile("graphics/extras.png", 16, 0, 8, 8);

	race_extra_tiles[2]->set_hotspot(4, 4);

	race_extra_tiles[3] = new GLTile("graphics/extras.png", 24, 0, 8, 8);

	race_extra_tiles[3]->set_hotspot(4, 4);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/extras: %p,%p,%p,%p\n", race_extra_tiles[0], race_extra_tiles[1], race_extra_tiles[2], race_lap_tile[3], race_lap_tile[3]);

#endif

	road_tile[0] = 0;

	road_tile[1] = 0;

	road_tile[2] = 0;

	road_ltile[0] = 0;

	road_ltile[1] = 0;

	road_rtile[0] = 0;

	road_rtile[1] = 0;

	road_lines = 0;

	race_damage = new GLTile("graphics/damage.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/damage.png: %p\n", race_damage);

#endif

	race_fuel = new GLTile("graphics/hud/fuel.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/fuel.png: %p\n", race_fuel);

#endif

	race_cloud[0] = new GLTile("graphics/cloud1.png");

	race_cloud[1] = new GLTile("graphics/cloud2.png");

	race_cloud[2] = new GLTile("graphics/cloud3.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/cloud1.png: %p\n", race_cloud[0]);

	output_debug_message("graphics/cloud2.png: %p\n", race_cloud[1]);

	output_debug_message("graphics/cloud3.png: %p\n", race_cloud[2]);

#endif

	race_sfx_folder = 0;

	race_default_sfx_folder = 0;


	/* New scoreboard graphics: */
	hud_fixed_1 = new GLTile("graphics/hud/hud_fixed_1.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_fixed_1.png: %p\n", hud_fixed_1);

#endif

	hud_fixed_2 = new GLTile("graphics/hud/hud_fixed_2.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_fixed_2.png: %p\n", hud_fixed_2);

#endif

	hud_fixed_3 = new GLTile("graphics/hud/hud_fixed_3.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_fixed_3.png: %p\n", hud_fixed_3);

#endif

	hud_fixed_3_sidefuel = new GLTile("graphics/hud/hud_fixed_3_sidefuel.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_fixed_3_sidefuel.png: %p\n", hud_fixed_3_sidefuel);

#endif

	hud_fixed_4 = new GLTile("graphics/hud/hud_fixed_4.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_fixed_4.png: %p\n", hud_fixed_4);

#endif

	hud_damage = new GLTile("graphics/hud/hud_damage.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_damage.png: %p\n", hud_damage);

#endif

	hud_rpm[0] = new GLTile("graphics/hud/hud_rpm_1.png");

	hud_rpm[1] = new GLTile("graphics/hud/hud_rpm_2.png");

	hud_rpm[2] = new GLTile("graphics/hud/hud_rpm_3.png");

	hud_rpm[3] = new GLTile("graphics/hud/hud_rpm_4.png");

	hud_rpm[4] = new GLTile("graphics/hud/hud_rpm_5.png");

	hud_rpm[5] = new GLTile("graphics/hud/hud_rpm_6.png");

	hud_rpm[6] = new GLTile("graphics/hud/hud_rpm_7.png");

	hud_rpm[7] = new GLTile("graphics/hud/hud_rpm_8.png");

	hud_rpm[8] = new GLTile("graphics/hud/hud_rpm_9.png");

	hud_rpm[9] = new GLTile("graphics/hud/hud_rpm_10.png");

	hud_rpm[10] = new GLTile("graphics/hud/hud_rpm_11.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/hud_rpm_1.png: %p\n", hud_rpm[0]);

	output_debug_message("graphics/hud/hud_rpm_2.png: %p\n", hud_rpm[1]);

	output_debug_message("graphics/hud/hud_rpm_3.png: %p\n", hud_rpm[2]);

	output_debug_message("graphics/hud/hud_rpm_4.png: %p\n", hud_rpm[3]);

	output_debug_message("graphics/hud/hud_rpm_5.png: %p\n", hud_rpm[4]);

	output_debug_message("graphics/hud/hud_rpm_6.png: %p\n", hud_rpm[5]);

	output_debug_message("graphics/hud/hud_rpm_7.png: %p\n", hud_rpm[6]);

	output_debug_message("graphics/hud/hud_rpm_8.png: %p\n", hud_rpm[7]);

	output_debug_message("graphics/hud/hud_rpm_9.png: %p\n", hud_rpm[8]);

	output_debug_message("graphics/hud/hud_rpm_10.png: %p\n", hud_rpm[9]);

	output_debug_message("graphics/hud/hud_rpm_11.png: %p\n", hud_rpm[10]);

#endif

	for (i = 0;i < 6;i++) {
		hud_gear[i] = new GLTile("graphics/hud/hud_gears.png", 0, 16 * i, 18, 16);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("graphics/hud/hud_gears.png %i: %p\n", i, hud_gear[i]);
#endif

	} 

	for (i = 0;i < 10;i++) {
		hud_speed_font[i] = new GLTile("graphics/hud/hud_speed_font.png", 0, 40 * i, 32, 40);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("graphics/hud/hud_speed_font.png %i: %p\n", i, hud_speed_font[i]);
#endif

	} 

	for (i = 0;i < 12;i++) {
		hud_time_font[i] = new GLTile("graphics/hud/hud_time_font.png", 0, 24 * i, 20, 24);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("graphics/hud/hud_time_font.png %i: %p\n", i, hud_time_font[i]);
#endif

	} 

	minihud_fuel = new GLTile("graphics/hud/minihud_fuel.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/hud/minihud_fuel.png: %p\n", minihud_fuel);

#endif

	/* Load signs: */
	for (i = 0;i < 10;i++) {
		signs[i] = new GLTile("graphics/signs.png", 64 * i, 0, 64, 64);
		signs[i]->set_hotspot(32, 32);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("graphics/signs.png %i: %p\n", i, signs[i]);
#endif

	} 


	/* Create the track: */
	switch (ntrack) {

		case 0:  /* STOCK: */
			road_tile[0] = new GLTile("graphics/stock/road1.png");
			road_tile[1] = new GLTile("graphics/stock/road2.png");
			road_tile[2] = new GLTile("graphics/stock/road3.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/road1.png: %p\n", road_tile[0]);
			output_debug_message("graphics/stock/road2.png: %p\n", road_tile[1]);
			output_debug_message("graphics/stock/road3.png: %p\n", road_tile[2]);
#endif

			road_ltile[0] = new GLTile("graphics/stock/lroad.png");
			road_ltile[1] = new GLTile("graphics/stock/lroad-chicane.png");
			road_rtile[0] = new GLTile("graphics/stock/rroad.png");
			road_rtile[1] = new GLTile("graphics/stock/rroad-chicane.png");
			road_lines = new GLTile("graphics/stock/road-lines.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/lroad.png: %p\n", road_ltile[0]);
			output_debug_message("graphics/stock/lroad-chicane.png: %p\n", road_ltile[1]);
			output_debug_message("graphics/stock/rroad.png: %p\n", road_rtile[0]);
			output_debug_message("graphics/stock/rroad-chicane.png: %p\n", road_rtile[1]);
			output_debug_message("graphics/stock/road-lines.png: %p\n", road_lines);
#endif

			break;

		case 1:  /* RALLY: */
			road_tile[0] = new GLTile("graphics/rally/road1.png");
			road_tile[1] = new GLTile("graphics/stock/road2.png");
			road_tile[2] = new GLTile("graphics/rally/road3.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/rally/road1.png: %p\n", road_tile[0]);
			output_debug_message("graphics/stock/road2.png: %p\n", road_tile[1]);
			output_debug_message("graphics/rally/road3.png: %p\n", road_tile[2]);
#endif

			road_ltile[0] = new GLTile("graphics/stock/lroad.png");
			road_ltile[1] = new GLTile("graphics/rally/lroad-chicane.png");
			road_rtile[0] = new GLTile("graphics/stock/rroad.png");
			road_rtile[1] = new GLTile("graphics/rally/rroad-chicane.png");
			road_lines = new GLTile("graphics/stock/road-lines.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/lroad.png: %p\n", road_ltile[0]);
			output_debug_message("graphics/rally/lroad-chicane.png: %p\n", road_ltile[1]);
			output_debug_message("graphics/stock/rroad.png: %p\n", road_rtile[0]);
			output_debug_message("graphics/rally/rroad-chicane.png: %p\n", road_rtile[1]);
			output_debug_message("graphics/stock/road-lines.png: %p\n", road_lines);
#endif

			break;

		case 10:  /* F1-FRANCE */
			road_tile[0] = new GLTile("graphics/stock/road1.png");
			road_tile[1] = new GLTile("graphics/stock/road2.png");
			road_tile[2] = new GLTile("graphics/f1-france/road3.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/road1.png: %p\n", road_tile[0]);
			output_debug_message("graphics/stock/road2.png: %p\n", road_tile[1]);
			output_debug_message("graphics/f1-france/road3.png: %p\n", road_tile[2]);
#endif

			road_ltile[0] = new GLTile("graphics/stock/lroad.png");
			road_ltile[1] = new GLTile("graphics/f1-france/lroad-chicane.png");
			road_rtile[0] = new GLTile("graphics/stock/rroad.png");
			road_rtile[1] = new GLTile("graphics/f1-france/rroad-chicane.png");
			road_lines = new GLTile("graphics/stock/road-lines.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/lroad.png: %p\n", road_ltile[0]);
			output_debug_message("graphics/f1-france/lroad-chicane.png: %p\n", road_ltile[1]);
			output_debug_message("graphics/stock/rroad.png: %p\n", road_rtile[0]);
			output_debug_message("graphics/f1-france/rroad-chicane.png: %p\n", road_rtile[1]);
			output_debug_message("graphics/stock/road-lines.png: %p\n", road_lines);
#endif

			break;

		case 12:  /* F1-WESTGERMANY */
			road_tile[0] = new GLTile("graphics/stock/road1.png");
			road_tile[1] = new GLTile("graphics/stock/road2.png");
			road_tile[2] = new GLTile("graphics/f1-france/road3.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/road1.png: %p\n", road_tile[0]);
			output_debug_message("graphics/stock/road2.png: %p\n", road_tile[1]);
			output_debug_message("graphics/f1-france/road3.png: %p\n", road_tile[2]);
#endif

			road_ltile[0] = new GLTile("graphics/stock/lroad.png");
			road_ltile[1] = new GLTile("graphics/f1-westgermany/lroad-chicane.png");
			road_rtile[0] = new GLTile("graphics/stock/rroad.png");
			road_rtile[1] = new GLTile("graphics/f1-westgermany/rroad-chicane.png");
			road_lines = new GLTile("graphics/stock/road-lines.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/lroad.png: %p\n", road_ltile[0]);
			output_debug_message("graphics/f1-france/lroad-chicane.png: %p\n", road_ltile[1]);
			output_debug_message("graphics/stock/rroad.png: %p\n", road_rtile[0]);
			output_debug_message("graphics/f1-france/rroad-chicane.png: %p\n", road_rtile[1]);
			output_debug_message("graphics/stock/road-lines.png: %p\n", road_lines);
#endif

			break;

		default:  /* F3,F3000, ENDURANCE, F1: */
			road_tile[0] = new GLTile("graphics/stock/road1.png");
			road_tile[1] = new GLTile("graphics/stock/road2.png");
			road_tile[2] = new GLTile("graphics/stock/road3.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/road1.png: %p\n", road_tile[0]);
			output_debug_message("graphics/stock/road2.png: %p\n", road_tile[1]);
			output_debug_message("graphics/stock/road3.png: %p\n", road_tile[2]);
#endif

			road_ltile[0] = new GLTile("graphics/stock/lroad.png");
			road_ltile[1] = new GLTile("graphics/stock/lroad-chicane.png");
			road_rtile[0] = new GLTile("graphics/stock/rroad.png");
			road_rtile[1] = new GLTile("graphics/stock/rroad-chicane.png");
			road_lines = new GLTile("graphics/stock/road-lines.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("graphics/stock/lroad.png: %p\n", road_ltile[0]);
			output_debug_message("graphics/stock/lroad-chicane.png: %p\n", road_ltile[1]);
			output_debug_message("graphics/stock/rroad.png: %p\n", road_rtile[0]);
			output_debug_message("graphics/stock/rroad-chicane.png: %p\n", road_rtile[1]);
			output_debug_message("graphics/stock/road-lines.png: %p\n", road_lines);
#endif

	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Loading SFX...\n");

#endif

	/* SFX: */
	{
		char *folders[6] = {"sound/stock/", "sound/rally/", "sound/f3/",
			"sound/f3000/", "sound/endurance/", "sound/f1/"};

		i = ntrack;

		if (i < 0)
			i = 0;

		if (i > 5)
			i = 5;

		delete []race_sfx_folder;

		delete []race_default_sfx_folder;

		race_sfx_folder = new char[strlen(folders[i]) + 1];

		strcpy(race_sfx_folder, folders[i]);

		race_default_sfx_folder = new char[strlen(folders[0]) + 1];

		strcpy(race_default_sfx_folder, folders[0]);


		S_outof_fuel = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_fuel");

		S_water_splash = load_sfx(race_sfx_folder, race_default_sfx_folder, "water_splash");

		S_squeal = load_sfx(race_sfx_folder, race_default_sfx_folder, "tire_squeal");

		S_car_pass = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_pass");

		S_car_hit1 = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_hit1");

		S_car_hit2 = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_hit2");

		S_car_hit3 = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_hit3");

		S_car_hit4 = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_hit4");

		S_car_brake = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_brake");

		S_car_engine = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_running");

		S_semaphore_high = load_sfx(race_sfx_folder, race_default_sfx_folder, "semaphore_high");

		S_semaphore_low = load_sfx(race_sfx_folder, race_default_sfx_folder, "semaphore_low");

		S_race_finished = load_sfx(race_sfx_folder, race_default_sfx_folder, "car_finish");

		S_chicane = load_sfx(race_sfx_folder, race_default_sfx_folder, "curbs");

		S_rain = load_sfx(race_sfx_folder, race_default_sfx_folder, "rain");
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("SFX loaded\n");

#endif


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Loading Tiles...\n");

#endif

	/* Load tiles: */
	{
		SDL_Surface *sfc;

		switch (ntrack) {

			case 6:
				sfc = IMG_Load("graphics/f1-brazil/grass.png");
				tiles.Add(new GLTile(sfc, 0, 0, 64, 64));
				SDL_FreeSurface(sfc);
				break;

			case 7:
				sfc = IMG_Load("graphics/f1-sanmarino/grass.png");
				tiles.Add(new GLTile(sfc, 0, 0, 64, 64));
				SDL_FreeSurface(sfc);
				break;

			default:
				sfc = IMG_Load("graphics/stock/tiles.png");
				tiles.Add(new GLTile(sfc, 0, 0, 64, 64));
				SDL_FreeSurface(sfc);
		} 

		sfc = IMG_Load("graphics/stock/tiles.png");

		tiles.Add(new GLTile(sfc, 0, 64, 48, 48)); /* 1 */

		tiles.Add(new GLTile(sfc, 0, 112, 48, 48));

		tiles.Add(new GLTile(sfc, 0, 160, 64, 48));

		SDL_FreeSurface(sfc);

		n_wood_tiles = 3;

		wood_tiles[0] = tiles[1];

		wood_tiles[1] = tiles[2];

		wood_tiles[2] = tiles[3];

		sfc = IMG_Load("graphics/stock/arrows.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 32)); /* 4 */

		tiles.Add(new GLTile(sfc, 0, 32, 32, 32));

		tiles.Add(new GLTile(sfc, 0, 64, 32, 32));

		tiles.Add(new GLTile(sfc, 0, 96, 32, 32));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/stock/pit.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 48));

		tiles.Add(new GLTile(sfc, 0, 48, 32, 48));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/stock/fences.png");

		tiles.Add(new GLTile(sfc, 0, 0, 64, 64)); /* 10 */

		tiles.Add(new GLTile(sfc, 0, 64, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 128, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 192, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 256, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 320, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 388, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 64, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 128, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 192, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 256, 64, 128));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/stock/crowd.png");

		tiles.Add(new GLTile(sfc, 0, 0, 80, 64)); /* 21 */

		tiles.Add(new GLTile(sfc, 0, 64, 80, 64));

		tiles.Add(new GLTile(sfc, 0, 384, 80, 64));

		tiles.Add(new GLTile(sfc, 80, 0, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 64, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 128, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 192, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 256, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 320, 48, 64));

		tiles.Add(new GLTile(sfc, 80, 384, 48, 64));

		tiles.Add(new GLTile(sfc, 128, 0, 32, 64)); /* 31 */

		tiles.Add(new GLTile(sfc, 128, 64, 32, 64));

		tiles.Add(new GLTile(sfc, 128, 128, 32, 64));

		tiles.Add(new GLTile(sfc, 160, 0, 48, 64));

		tiles.Add(new GLTile(sfc, 160, 64, 48, 64));

		tiles.Add(new GLTile(sfc, 160, 128, 48, 64));

		tiles.Add(new GLTile(sfc, 128, 192, 96, 64)); /* 37 */

		tiles.Add(new GLTile(sfc, 224, 192, 32, 64));

		tiles.Add(new GLTile(sfc, 224, 256, 32, 64));

		tiles.Add(new GLTile(sfc, 224, 320, 32, 64));

		tiles.Add(new GLTile(sfc, 256, 0, 128, 64));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/rally/tiles.png");

		tiles.Add(new GLTile(sfc, 0, 0, 64, 64)); /* 42 */

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/rally/walls.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 32)); /* 43 */

		tiles.Add(new GLTile(sfc, 0, 32, 32, 32));

		tiles.Add(new GLTile(sfc, 32, 32, 32, 32));

		tiles.Add(new GLTile(sfc, 64, 32, 32, 32));

		tiles.Add(new GLTile(sfc, 96, 32, 32, 32));

		tiles.Add(new GLTile(sfc, 0, 64, 32, 32));

		tiles.Add(new GLTile(sfc, 32, 64, 32, 32));

		tiles.Add(new GLTile(sfc, 64, 64, 32, 32));

		tiles.Add(new GLTile(sfc, 96, 64, 32, 32));

		tiles.Add(new GLTile(sfc, 0, 96, 32, 32));

		tiles.Add(new GLTile(sfc, 32, 96, 32, 32));

		tiles.Add(new GLTile(sfc, 64, 96, 32, 32));

		tiles.Add(new GLTile(sfc, 96, 96, 32, 32));

		tiles.Add(new GLTile(sfc, 0, 128, 32, 32));

		tiles.Add(new GLTile(sfc, 32, 128, 32, 32));

		tiles.Add(new GLTile(sfc, 64, 128, 32, 32));

		tiles.Add(new GLTile(sfc, 96, 128, 32, 32));

		tiles.Add(new GLTile(sfc, 32, 0, 32, 32)); /* 60 */

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/rally/extras.png");

		tiles.Add(new GLTile(sfc, 0, 0, 96, 24)); /* 61 */

		tiles.Add(new GLTile(sfc, 0, 24, 96, 24));

		tiles.Add(new GLTile(sfc, 0, 48, 64, 160));

		tiles.Add(new GLTile(sfc, 0, 208, 48, 32));

		tiles.Add(new GLTile(sfc, 96, 0, 32, 64));

		tiles.Add(new GLTile(sfc, 96, 64, 32, 64));

		tiles.Add(new GLTile(sfc, 96, 128, 32, 64));

		SDL_FreeSurface(sfc);

		water_tile = tiles[63];

		sfc = IMG_Load("graphics/rally/crowd.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 64)); /* 68 */

		tiles.Add(new GLTile(sfc, 0, 64, 32, 64));

		tiles.Add(new GLTile(sfc, 0, 128, 32, 64));

		tiles.Add(new GLTile(sfc, 32, 0, 32, 64));

		tiles.Add(new GLTile(sfc, 32, 64, 32, 64));

		tiles.Add(new GLTile(sfc, 32, 128, 32, 64));

		tiles.Add(new GLTile(sfc, 64, 0, 64, 64));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/f3/lfences.png");

		tiles.Add(new GLTile(sfc, 0, 0, 64, 64)); /* 75 */

		tiles.Add(new GLTile(sfc, 0, 64, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 128, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 192, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 256, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 320, 64, 64));

		tiles.Add(new GLTile(sfc, 0, 388, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 64, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 128, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 192, 64, 64));

		tiles.Add(new GLTile(sfc, 64, 256, 64, 128));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/f3/rfences.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 64)); /* 86 */

		tiles.Add(new GLTile(sfc, 0, 64, 32, 64));

		tiles.Add(new GLTile(sfc, 0, 128, 32, 64));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/rally/extras.png");

		tiles.Add(new GLTile(sfc, 0, 240, 80, 224)); /* 89 */

		SDL_FreeSurface(sfc);

		if (ntrack == 16) {
			sfc = IMG_Load("graphics/f1-portugal/extras.png");
			tiles.Add(new GLTile(sfc, 0, 0, 80, 224)); /* 90 */
			SDL_FreeSurface(sfc);
		} else {
			sfc = IMG_Load("graphics/f3/extras.png");
			tiles.Add(new GLTile(sfc, 0, 0, 80, 224)); /* 90 */
			SDL_FreeSurface(sfc);
		} 

		if (ntrack == 10) {
			/* F1-FRANCE: */
			sfc = IMG_Load("graphics/f1-france/isle.png");
			tiles.Add(new GLTile(sfc, 0, 0, 96, 96)); /* 91 */
			tiles.Add(new GLTile(sfc, 0, 96, 96, 96));
			tiles.Add(new GLTile(sfc, 0, 192, 96, 96));
			SDL_FreeSurface(sfc);
		} else {
			if (ntrack == 12) {
				/* F1-WESTGERMANY: */
				sfc = IMG_Load("graphics/f1-westgermany/isle.png");
				tiles.Add(new GLTile(sfc, 0, 0, 96, 96)); /* 91 */
				tiles.Add(new GLTile(sfc, 0, 96, 96, 96));
				tiles.Add(new GLTile(sfc, 0, 192, 96, 96));
				SDL_FreeSurface(sfc);
			} else {
				sfc = IMG_Load("graphics/f3/isle.png");
				tiles.Add(new GLTile(sfc, 0, 0, 96, 96)); /* 91 */
				tiles.Add(new GLTile(sfc, 0, 96, 96, 96));
				tiles.Add(new GLTile(sfc, 0, 192, 96, 96));
				SDL_FreeSurface(sfc);
			} 
		} 

		sfc = IMG_Load("graphics/endurance/extras.png");

		tiles.Add(new GLTile(sfc, 0, 0, 64, 48)); /* 94 */

		tiles.Add(new GLTile(sfc, 0, 48, 16, 64));

		tiles.Add(new GLTile(sfc, 0, 112, 80, 16));

		tiles.Add(new GLTile(sfc, 0, 128, 56, 40));

		tiles.Add(new GLTile(sfc, 0, 168, 40, 56));

		tiles.Add(new GLTile(sfc, 0, 224, 40, 20));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/rally/rock.png");

		tiles.Add(new GLTile(sfc, 0, 0, 24, 16)); /* 100 */

		SDL_FreeSurface(sfc);

		rock_tile = tiles[100];

	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Tiles loaded\n");

#endif


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Loading Track...\n");

#endif


	/* Load the track: */
	{
		FILE *fp;
		char *tracks[N_TRACKS] = {"tracks/stock.f1t",
		                          "tracks/rally.f1t",
		                          "tracks/f3.f1t",
		                          "tracks/f3000.f1t",
		                          "tracks/endurance.f1t",
		                          "tracks/f1-brazil.f1t",
		                          "tracks/f1-sanmarino.f1t",
		                          "tracks/f1-belgium.f1t",
		                          "tracks/f1-monaco.f1t",
		                          "tracks/f1-usa.f1t",
		                          "tracks/f1-france.f1t",
		                          "tracks/f1-greatbritain.f1t",
		                          "tracks/f1-westgermany.f1t",
		                          "tracks/f1-hungary.f1t",
		                          "tracks/f1-austria.f1t",
		                          "tracks/f1-italy.f1t",
		                          "tracks/f1-portugal.f1t",
		                          "tracks/f1-spain.f1t",
		                          "tracks/f1-mexico.f1t",
		                          "tracks/f1-japan.f1t",
		                          "tracks/f1-australia.f1t",
		                          "tracks/extras/oval.f1t",
		                          "tracks/extras/oval.f1t",
		                          "tracks/extras/micro.f1t",
		                          "tracks/extras/micro.f1t"
		                         };

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("File nr %i\n", ntrack);
		output_debug_message("File: %s\n", tracks[ntrack]);
#endif

		fp = f1open(tracks[ntrack], "r", GAMEDATA);

		/* If the track does not exist, load the Stock track: */

		if (fp == 0) {
			fp = f1open(tracks[0], "r", GAMEDATA);
		} 

		if (fp != 0) {
			track = new CTrack(ntrack, fp, &tiles);
			fclose(fp);
		} else {
			track = 0;
		} 
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Track loaded: %p\n", track);

#endif

	track->get_road()->Rewind();

	while (!race_positions.EmptyP())
		race_positions.ExtractIni();


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Generating clouds...");

#endif

	{
		int nclouds = rg->IRandom(0, 2);
		int i;
		CPlacedGLTile *pt;

		nclouds = 2;

		race_clouds.Delete();

		if (nclouds == 1)
			nclouds = 4 * 32;

		if (nclouds == 2)
			nclouds = 16 * 32;

		for (i = 0;i < nclouds;i++) {
			pt = new CPlacedGLTile(float((rg->IRandom(0, track->get_dx() - 1)) * 8),
			                       float((rg->IRandom(0, track->get_dy() - 1)) * 8), -512, 0, race_cloud[rg->IRandom(0, 2)]);

			race_clouds.Add(pt);
		} 

	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Initializing the car grid...");

#endif

	{
		int i;
		car_grid = new List<RacingCCar> *[CAR_GRID_SIZE];

		for (i = 0;i < CAR_GRID_SIZE;i++)
			car_grid[i] = new List<RacingCCar> [CAR_GRID_SIZE];

		car_grid_sx = (track->get_dx() * 8) / CAR_GRID_SIZE;

		car_grid_sy = (track->get_dy() * 8) / CAR_GRID_SIZE;
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Initializing the weather...");

#endif
	{

		if (parameters.rain_probability[ntrack] == 0) {
			// No rain
			m_rain_strength = 0.0f;
			m_current_rain_strength = m_rain_strength;
			m_rain_period1 = 0;
			m_rain_period2 = 0;
			m_rain_times = 0;
		} else {
			// Randomly select an amount of rain:
			float f = float(rand()) / float(RAND_MAX);	// This one uses "rand" instead of the RanRot
			// Because it has to be really random, and not being reinitialized
			// Everytime a new game is started

			f /= parameters.rain_probability[ntrack];

			if (f > 1.0f) {
				// No rain:
				m_rain_strength = 0.0f;
				m_current_rain_strength = m_rain_strength;
				m_rain_period1 = 0;
				m_rain_period2 = 0;
				m_rain_times = 0;
			} else if (f > 0.5f) {
				// Intermitent rain:
				m_rain_strength = 1.0f;
				m_current_rain_strength = m_rain_strength;
				m_rain_period1 = int(500 + (3000 * (2 * (f - 0.5f))));
				m_rain_period2 = int(1000 + (3000 * (1.0 - 2 * (f - 0.5f))));
				m_rain_times = 1000;
			} else {
				// Full rain:
				m_rain_strength = 1.0f;
				m_current_rain_strength = m_rain_strength;
				m_rain_period1 = 0;
				m_rain_period2 = -1;
				m_rain_times = 0;
			} // if
		} // if
	}

} 


F1SpiritGame::F1SpiritGame(CPlayerInfo *player, int ntrack, int nplayers, int max_cars, int enemy_speed, int *selected_car, int **selected_part, SDL_Surface *f, KEYBOARDSTATE *k)
{
	// int max_cars=2;
	int n_enemy_cars = 1;
	int engine_channels = 0;
	int i;

	font = f;
	current_player = player;

	play_music = true;

	rg->RandomInit(0);

	parameters.load_ascii("f1spirit2.cfg");
	initialize_track(ntrack);

	// max_cars=parameters.race_cars[ntrack];
	n_enemy_cars = max_cars - nplayers;

	if (n_enemy_cars < 0)
		n_enemy_cars = 0;

	{
		SDL_Surface *sfc;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Generating Minimap...\n");
#endif

		delete race_minitrack;
		race_minitrack = 0;
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Previous minimap freed\n");
#endif

		if (nplayers > 1) {
			sfc = track->draw_vertical_minimap(16, 464, race_laps[ntrack], &race_minimap_zoom);
		} else {
			sfc = track->draw_minimap(192, 106, &race_minimap_zoom);
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Minimap drawn into a surface\n");

#endif

		race_minitrack = new GLTile(sfc);

		//   SDL_FreeSurface(sfc);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Minimap Generated\n");

#endif

	}


#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Creating Enemy Cars...\n");

#endif


	race_first_sfx_channel = 0;

	race_sfx_channel = 0;

	rain_channel = 0;

	/* Create the enemy cars: */
	{
		EnemyCCar *ec;
		float effective_handycap_decrement = parameters.handycap_decrement[ntrack];
		float effective_base_handycap = parameters.base_handycap[ntrack];

		while (effective_handycap_decrement*n_enemy_cars > 0.4F)
			effective_handycap_decrement *= 0.8F;

		if (enemy_speed == 0)
			effective_base_handycap *= 0.95;

		if (enemy_speed == 2)
			effective_base_handycap *= 1.05;

		enemy_cars.Delete();

		for (i = 0;i < n_enemy_cars;i++) {
			ec = new EnemyCCar();

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("Enemy Car %i...\n", i);
#endif

			{
				int p1, p2, p3, p4;
				int type;

				type = min(ntrack, 5);

				if (ntrack == 21)
					type = 0;

				if (ntrack == 22)
					type = 5;

				if (ntrack == 23)
					type = 0;

				/*
				    float cr[4]={1,0.25F, 1   ,0.5F};
				    float cg[4]={1,0.25F, 0.5F,0};
				    float cb[4]={1,0.25F, 0.5F,0};
				*/
				p1 = rg->IRandom(0, 2);

				p2 = rg->IRandom(0, 2);

				p3 = rg->IRandom(0, 2);

				p4 = rg->IRandom(0, 2);

				if (i > n_enemy_cars - 5) {
					if (p1 < 2)
						p1++;

					if (p2 < 2)
						p2++;

					if (i > n_enemy_cars - 2) {
						if (p1 < 2)
							p1++;

						if (p2 < 2)
							p2++;
					} 
				} 

				ec->AI_type = i % 4;

				ec->car = create_car(type, p1, p2, p3, 3, p4, 1, 1, 1, false, S_car_engine,
				                     -1, effective_base_handycap - i * effective_handycap_decrement);
			}

#ifdef F1SPIRIT_DEBUG_MESSAGES
			output_debug_message("placing car in the track.");
#endif

			{
				float start_x, start_y, start_a;
				CRoadPiece *start_p;
				start_p = track->get_start_position(i, &start_x, &start_y, &start_a);
				ec->car->set_x(start_x);
				ec->car->set_y(start_y);
				ec->car->set_a(start_a);
				ec->car->set_z( -10);
				ec->road_position.Instance(*track->get_road());

				if (start_p != 0) {
					ec->road_position.Rewind();

					while (ec->road_position.GetObj() != start_p && !ec->road_position.EndP())
						ec->road_position.Next();

					if (ec->road_position.EndP())
						ec->road_position.Rewind();
				} 
			}

#ifdef F1SPIRIT_DEBUG_MESSAGES
			output_debug_message("(%f)", ec->car->get_a());
#endif

			ec->car->set_state(0);
			ec->laps = 0;
			ec->track = track;
			ec->road_offset_change = -1;
			ec->road_offset = 0;

			ec->position = track->get_position(ec->car->get_x(), ec->car->get_y()) - track->get_length();

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message(".");
#endif

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message(".");
#endif

			enemy_cars.Add(ec);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message(".");
#endif

			race_positions.Add((RacingCCar *)ec);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("ok\n");
#endif

		} 
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Enemy cars generated\n");

#endif


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Creating Players Cars..\n");

#endif

	player_cars.Delete();

	for (i = 0;i < nplayers;i++) {
		CCar *player_car;
		int type;

		type = min(ntrack, 5);

		if (ntrack == 21)
			type = 0;

		if (ntrack == 22)
			type = 5;

		if (ntrack == 23)
			type = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Player car %i...", i + 1);

#endif

		/* Create the players' cars: */
		player_car = 0;

		switch (selected_car[i]) {

			case 0:
				/* READY MADE 1: */
				player_car = create_car(type, 0, 0, 0, 0, 0, -1, -1, -1, true, S_car_engine, race_first_sfx_channel++, 1);
				break;

			case 1:
				/* READY MADE 2: */
				player_car = create_car(type, 1, 1, 1, 1, 1, -1, -1, -1, true, S_car_engine, race_first_sfx_channel++, 1);
				break;

			case 2:
				/* READY MADE 3: */
				player_car = create_car(type, 2, 2, 2, 2, 2, -1, -1, -1, true, S_car_engine, race_first_sfx_channel++, 1);
				break;

			case 3:
				/* ORIGINAL DESIGN: */
				player_car = create_car(type, selected_part[i][0], selected_part[i][1], selected_part[i][2], selected_part[i][3], selected_part[i][4], -1, -1, -1, true, S_car_engine, race_first_sfx_channel++, 1);
				break;
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("placing car in the track...");

#endif


		/* Create the viewports: */
		if (player_car != 0) {
			PlayerCCar *v;

			v = new PlayerCCar();

			switch (nplayers) {

				case 1:
					v->vx = 0;
					v->vy = 0;
					v->vdx = 640;
					v->vdy = 480;
					break;

				case 2:
					v->vx = 322 * i;
					v->vdx = 318;
					v->vy = 0;
					v->vdy = 480;
					break;

				case 3:

					if (i == 0) {
						v->vx = 0;
						v->vdx = 318;
						v->vy = 0;
						v->vdy = 480;
					} else {
						v->vx = 322;
						v->vdx = 318;
						v->vy = 240 * (i - 1);
						v->vdy = 238;
					} 

					break;

				default:
					v->vx = 322 * int(i / 2);

					v->vdx = 318;

					v->vy = 240 * (i % 2);

					v->vdy = 238;

					break;
			} 

			if (current_player->get_joystick(i) == -1) {
				v->up = current_player->get_key(i, 0);
				v->down = current_player->get_key(i, 1);
				v->left = current_player->get_key(i, 2);
				v->right = current_player->get_key(i, 3);
				v->accelerate = current_player->get_key(i, 4);
				v->brake = current_player->get_key(i, 5);
			} else {
				int j = current_player->get_joystick(i);
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

			v->track = track;

			v->car = player_car;

			player_car = 0;

			{
				float start_x, start_y, start_a;
				CRoadPiece *start_p;
				start_p = track->get_start_position(enemy_cars.Length() + i, &start_x, &start_y, &start_a);
				v->car->set_x(start_x);
				v->car->set_y(start_y);
				v->car->set_a(start_a);
				//    v->car->set_a(track->get_road()->GetObj()->get_a1()+90);
				v->car->set_z( -10);

				v->road_position.Instance(*track->get_road());

				if (start_p != 0) {
					v->road_position.Rewind();

					while (v->road_position.GetObj() != start_p && !v->road_position.EndP())
						v->road_position.Next();

					if (v->road_position.EndP())
						v->road_position.Rewind();
				} 

			}

			v->car->set_state(0);

			v->laps = 0;
			v->show_lap_time = 0;
			v->car->set_carengine_channel(engine_channels);
			engine_channels++;

			// camera type 2
			v->c_x = v->car->get_x();
			v->c_y = v->car->get_y();

			v->c_z = 1;
			v->c_a = 90 - v->car->get_a();
			v->c_a_speed = 0;

			v->c_old_x = v->c_x;
			v->c_old_y = v->c_y;
			v->c_old_a = v->c_a;
			v->c_old_z = v->c_z;

			v->position = track->get_position(v->car->get_x(), v->car->get_y()) - track->get_length();

			player_cars.Add(v);

			race_positions.Add((RacingCCar *)v);
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("OK\n");

#endif

	} 

	race_sfx_channel = race_first_sfx_channel;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("All players created.\n");

#endif

	race_time = 0;

	race_nlaps = race_laps[ntrack];

	race_state = 0;

	race_state_timmer = 0;

	race_semaphore_timmer = 0;

	/* Prepare to save a replay: */
	replay_filename = new char[strlen("replays/tmp.rpl") + 1];

	strcpy(replay_filename, "replays/tmp.rpl");

	replay_fp = 0;

	replay_fp = f1open(replay_filename, "wb+", USERDATA);

	replay_cycle = 0;


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("OK.\n");

#endif

	rg->RandomInit(0);

} 


F1SpiritGame::F1SpiritGame(CReplayInfo *ri, SDL_Surface *f, KEYBOARDSTATE *k)
{
	int engine_channels = 0;
	int i;

	font = f;
	current_player = ri->player;

	rg->RandomInit(0);

	play_music = true;

	parameters.load_ascii("f1spirit2.cfg");
	parameters.instance(&(ri->parameters));
	initialize_track(ri->track_num);

	race_first_sfx_channel = 0;
	race_sfx_channel = 0;
	rain_channel = 0;

	{
		SDL_Surface *sfc;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Generating Minimap...\n");
#endif

		delete race_minitrack;
		race_minitrack = 0;
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Previous minimap freed\n");
#endif

		if (ri->n_player_cars > 1) {
			sfc = track->draw_vertical_minimap(16, 464, race_laps[ri->track_num], &race_minimap_zoom);
		} else {
			sfc = track->draw_minimap(192, 106, &race_minimap_zoom);
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Minimap drawn into a surface\n");

#endif

		race_minitrack = new GLTile(sfc);

		//   SDL_FreeSurface(sfc);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Minimap Generated\n");

#endif

	}


#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Creating Enemy Cars...\n");
#endif

	/* Create the enemy cars: */
	{
		EnemyCCar *ec;

		enemy_cars.Delete();

		for (i = 0;i < ri->n_enemy_cars;i++) {
			ec = new EnemyCCar();

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("Enemy Car %i...\n", i);
#endif

			ec->AI_type = ri->ec_AI_type[i];
			ec->car = create_car(min(ri->track_num, 5), ri->ec_body[i], ri->ec_engine[i], ri->ec_brake[i], ri->ec_suspension[i], ri->ec_gear[i], ri->ec_r[i], ri->ec_g[i], ri->ec_b[i], false, S_car_engine, -1, ri->ec_handycap[i]);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("placing car in the track.");
#endif

			{
				float start_x, start_y, start_a;
				CRoadPiece *start_p;
				start_p = track->get_start_position(i, &start_x, &start_y, &start_a);
				ec->car->set_x(start_x);
				ec->car->set_y(start_y);
				ec->car->set_a(start_a);
				ec->car->set_z( -10);
				ec->road_position.Instance(*track->get_road());

				if (start_p != 0) {
					ec->road_position.Rewind();

					while (ec->road_position.GetObj() != start_p && !ec->road_position.EndP())
						ec->road_position.Next();

					if (ec->road_position.EndP())
						ec->road_position.Rewind();
				} 
			}

#ifdef F1SPIRIT_DEBUG_MESSAGES
			output_debug_message("(%f)", ec->car->get_a());
#endif

			ec->car->set_state(0);
			ec->laps = 0;
			ec->track = track;
			ec->road_offset_change = -1;
			ec->road_offset = 0;


#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message(".");
#endif

			enemy_cars.Add(ec);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message(".");
#endif

			race_positions.Add((RacingCCar *)ec);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("ok\n");
#endif

		} 
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Enemy cars generated\n");
#endif


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Creating Players Cars..\n");
#endif

	player_cars.Delete();

	for (i = 0;i < ri->n_player_cars;i++) {
		CCar *player_car;

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Player car %i...", i + 1);
#endif

		/* Create the players' cars: */
		/* ... */
		player_car = create_car(min(ri->track_num, 5), ri->pc_body[i], ri->pc_engine[i], ri->pc_brake[i], ri->pc_suspension[i], ri->pc_gear[i], ri->pc_r[i], ri->pc_g[i], ri->pc_b[i], true, S_car_engine, race_first_sfx_channel++, ri->pc_handycap[i]);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("placing car in the track...");
#endif

		/* Create the viewports: */

		if (player_car != 0) {
			PlayerCCar *v;

			v = new PlayerCCar();

			switch (ri->n_player_cars) {

				case 1:
					v->vx = 0;
					v->vy = 0;
					v->vdx = 640;
					v->vdy = 480;
					break;

				case 2:
					v->vx = 320 * i;
					v->vdx = 318;
					v->vy = 0;
					v->vdy = 480;
					break;

				case 3:

					if (i == 0) {
						v->vx = 0;
						v->vdx = 318;
						v->vy = 0;
						v->vdy = 480;
					} else {
						v->vx = 320;
						v->vdx = 318;
						v->vy = 240 * (i - 1);
						v->vdy = 238;
					} 

					break;

				default:
					v->vx = 320 * int(i / 2);

					v->vdx = 318;

					v->vy = 240 * (i % 2);

					v->vdy = 238;

					break;
			} 

			if (current_player->get_joystick(i) == -1) {
				v->up = current_player->get_key(i, 0);
				v->down = current_player->get_key(i, 1);
				v->left = current_player->get_key(i, 2);
				v->right = current_player->get_key(i, 3);
				v->accelerate = current_player->get_key(i, 4);
				v->brake = current_player->get_key(i, 5);
			} else {
				int j = current_player->get_joystick(i);
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

			v->track = track;

			v->car = player_car;

			player_car = 0;

			{
				float start_x, start_y, start_a;
				CRoadPiece *start_p;
				start_p = track->get_start_position(enemy_cars.Length() + i, &start_x, &start_y, &start_a);
				v->car->set_x(start_x);
				v->car->set_y(start_y);
				v->car->set_a(start_a);
				//    v->car->set_a(track->get_road()->GetObj()->get_a1()+90);
				v->car->set_z( -10);

				v->road_position.Instance(*track->get_road());

				if (start_p != 0) {
					v->road_position.Rewind();

					while (v->road_position.GetObj() != start_p && !v->road_position.EndP())
						v->road_position.Next();

					if (v->road_position.EndP())
						v->road_position.Rewind();
				} 
			}

			v->car->set_state(0);

			v->laps = 0;
			v->show_lap_time = 0;
			v->car->set_carengine_channel(engine_channels);
			engine_channels++;

			v->c_x = v->car->get_x();
			v->c_y = v->car->get_y();
			v->c_z = 1;
			v->c_a = 90 - v->car->get_a();
			v->c_a_speed = 0;

			v->c_old_x = v->c_x;
			v->c_old_y = v->c_y;
			v->c_old_a = v->c_a;
			v->c_old_z = v->c_z;

			player_cars.Add(v);

			race_positions.Add((RacingCCar *)v);
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("OK\n");

#endif

	} 

	race_sfx_channel = race_first_sfx_channel;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("All players created.\n");

#endif

	race_time = 0;

	race_nlaps = race_laps[ri->track_num];

	race_state = 0;

	race_state_timmer = 0;

	race_semaphore_timmer = 0;

	/* Do not prepare to save a replay, since this is a game created to SEE a replay: */
	replay_filename = 0;

	replay_fp = 0;

	replay_cycle = 0;


#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("OK.\n");

#endif

	rg->RandomInit(0);

} 


F1SpiritGame::~F1SpiritGame()
{
	int i, j;

	SDL_FreeSurface(race_car_sfc);
	SDL_FreeSurface(race_minimap_car_sfc);
	SDL_FreeSurface(race_minimap_enemycar_sfc);
	race_car_sfc = 0;
	race_minimap_car_sfc = 0;
	race_minimap_enemycar_sfc = 0;

	while (!race_positions.EmptyP())
		race_positions.ExtractIni();

	for (i = 0;i < CAR_GRID_SIZE;i++) {
		for (j = 0;j < CAR_GRID_SIZE;j++) {
			while (!car_grid[i][j].EmptyP())
				car_grid[i][j].ExtractIni();
		} 

		delete []car_grid[i];

		car_grid[i] = 0;
	} 

	delete []car_grid;

	car_grid = 0;

	delete track;

	track = 0;

	delete race_minitrack;

	race_minitrack = 0;

	delete race_semaphore[0];

	delete race_semaphore[1];

	delete race_semaphore[2];

	delete race_semaphore[3];

	delete race_semaphore[4];

	race_semaphore[0] = 0;

	race_semaphore[1] = 0;

	race_semaphore[2] = 0;

	race_semaphore[3] = 0;

	race_semaphore[4] = 0;

	delete race_lap_tile[0];

	delete race_lap_tile[1];

	delete race_lap_tile[2];

	delete race_lap_tile[3];

	delete race_lap_tile[4];

	race_lap_tile[0] = 0;

	race_lap_tile[1] = 0;

	race_lap_tile[2] = 0;

	race_lap_tile[3] = 0;

	race_lap_tile[4] = 0;

	for (i = 0;i < 9;i++) {
		delete race_rpm_tile[i];
		race_rpm_tile[i] = 0;
	} 

	delete race_extra_tiles[0];

	delete race_extra_tiles[1];

	delete race_extra_tiles[2];

	delete race_extra_tiles[3];

	race_extra_tiles[0] = 0;

	race_extra_tiles[1] = 0;

	race_extra_tiles[2] = 0;

	race_extra_tiles[3] = 0;

	delete race_damage;

	race_damage = 0;

	delete race_fuel;

	race_fuel = 0;

	/* new hud: */
	delete hud_fixed_1;

	delete hud_fixed_2;

	delete hud_fixed_3;

	delete hud_fixed_4;

	delete hud_damage;

	for (i = 0;i < 11;i++) {
		delete hud_rpm[i];
		hud_rpm[i] = 0;
	} 

	for (i = 0;i < 6;i++) {
		delete hud_gear[i];
		hud_gear[i] = 0;
	} 

	for (i = 0;i < 10;i++) {
		delete hud_speed_font[i];
		hud_speed_font[i] = 0;
	} 

	for (i = 0;i < 12;i++) {
		delete hud_time_font[i];
		hud_time_font[i] = 0;
	} 

	delete minihud_fuel;

	for (i = 0;i < 10;i++) {
		delete signs[i];
		signs[i] = 0;
	} 

	delete []race_sfx_folder;

	race_sfx_folder = 0;

	delete []race_default_sfx_folder;

	race_default_sfx_folder = 0;

	delete road_tile[0];

	delete road_tile[1];

	delete road_tile[2];

	road_tile[0] = 0;

	road_tile[1] = 0;

	road_tile[2] = 0;

	delete road_ltile[0];

	delete road_ltile[1];

	road_ltile[0] = 0;

	road_ltile[1] = 0;

	delete road_rtile[0];

	delete road_rtile[1];

	road_rtile[0] = 0;

	road_rtile[1] = 0;

	delete road_lines;

	road_lines = 0;

	delete race_cloud[0];

	delete race_cloud[1];

	delete race_cloud[2];

	race_cloud[0] = 0;

	race_cloud[1] = 0;

	race_cloud[2] = 0;

	race_clouds.Delete();

	Sound_delete_sound(S_outof_fuel);

	Sound_delete_sound(S_water_splash);

	Sound_delete_sound(S_squeal);

	Sound_delete_sound(S_car_pass);

	Sound_delete_sound(S_car_hit1);

	Sound_delete_sound(S_car_hit2);

	Sound_delete_sound(S_car_hit3);

	Sound_delete_sound(S_car_hit4);

	Sound_delete_sound(S_car_brake);

	Sound_delete_sound(S_car_engine);

	Sound_delete_sound(S_semaphore_high);

	Sound_delete_sound(S_semaphore_low);

	Sound_delete_sound(S_race_finished);

	Sound_delete_sound(S_chicane);

	Sound_delete_sound(S_rain);

	delete replay_filename;

	replay_filename = 0;

	if (replay_fp != 0)
		fclose(replay_fp);

	replay_fp = 0;
} 



bool F1SpiritGame::cycle(KEYBOARDSTATE *k)
{
	List<PlayerCCar> l;
	PlayerCCar *v;
	List<EnemyCCar> l2, todelete;
	EnemyCCar *ec;
	bool cars_with_fuel = false;
	int terrain;

	/*
	#ifdef F1SPIRIT_DEBUG_MESSAGES
	  output_debug_message("Random test: %i\n",rg->IRandom(0,10000));
	#endif
	*/
	rg->RandomInit(replay_cycle);

	if (replay_fp != 0) {
		if (replay_cycle == 0) {
			int i;
			char *tracks[N_TRACKS] = {"stock.f1t",
			                          "rally.f1t",
			                          "f3.f1t",
			                          "f3000.f1t",
			                          "endurance.f1t",
			                          "f1-brazil.f1t",
			                          "f1-sanmarino.f1t",
			                          "f1-belgium.f1t",
			                          "f1-monaco.f1t",
			                          "f1-usa.f1t",
			                          "f1-france.f1t",
			                          "f1-greatbritain.f1t",
			                          "f1-westgermany.f1t",
			                          "f1-hungary.f1t",
			                          "f1-austria.f1t",
			                          "f1-italy.f1t",
			                          "f1-portugal.f1t",
			                          "f1-spain.f1t",
			                          "f1-mexico.f1t",
			                          "f1-japan.f1t",
			                          "f1-australia.f1t",
			                          "extras/oval.f1t",
			                          "extras/oval.f1t",
			                          "extras/micro.f1t"
			                          "extras/micro.f1t"
			                         };

			/* version: */

			for (i = 0;i < 6;i++)
				fputc(f1spirit_replay_version[i], replay_fp);

			/* date: */
			{
#ifdef _WIN32

				struct tm today;

				_tzset();
				_getsystime(&today);

				fputc(today.tm_year, replay_fp);
				fputc(today.tm_mon, replay_fp);
				fputc(today.tm_mday, replay_fp);
				fputc(today.tm_hour, replay_fp);
				fputc(today.tm_min, replay_fp);
				fputc(today.tm_sec, replay_fp);

#else

				struct timeval ttime;

				struct tm *today;

				gettimeofday(&ttime, NULL);
				today = localtime(&(ttime.tv_sec));

				fputc(today->tm_year, replay_fp);
				fputc(today->tm_mon, replay_fp);
				fputc(today->tm_mday, replay_fp);
				fputc(today->tm_hour, replay_fp);
				fputc(today->tm_min, replay_fp);
				fputc(today->tm_sec, replay_fp);
#endif

			}

			/* game parameters: */
			parameters.save_bin(replay_fp);

			/* player configuration: */
			current_player->save(replay_fp);

			/* track: */
			fputc(track->get_track_number(), replay_fp);

			fputc(strlen(tracks[track->get_track_number()]), replay_fp);

			for (i = 0;unsigned(i) < strlen(tracks[track->get_track_number()]);i++)
				fputc(tracks[track->get_track_number()][i], replay_fp);

			/* initial enemy cars: */
			fputc(enemy_cars.Length(), replay_fp);

			{
				List<EnemyCCar> l;
				EnemyCCar *ec;

				l.Instance(enemy_cars);
				l.Rewind();

				while (l.Iterate(ec)) {
					fputc(ec->AI_type, replay_fp);
					ec->save(replay_fp);
				} 
			}

			/* initial player cars: */
			fputc(player_cars.Length(), replay_fp);

			{
				List<PlayerCCar> l;
				PlayerCCar *pc;

				l.Instance(player_cars);
				l.Rewind();

				while (l.Iterate(pc))
					pc->save(replay_fp);
			}
		} else {
			int i;

			if ((replay_cycle % 50) == 0) {
				fputc(replay_cycle&0x0ff, replay_fp);
				fputc(replay_cycle >> 8, replay_fp);
				fputc(1, replay_fp);

				/* initial enemy cars: */
				fputc(enemy_cars.Length(), replay_fp);
				{
					List<EnemyCCar> l;
					EnemyCCar *ec;

					l.Instance(enemy_cars);
					l.Rewind();

					while (l.Iterate(ec))
						ec->save_status(replay_fp);
				}

				/* initial player cars: */
				fputc(player_cars.Length(), replay_fp);
				{
					List<PlayerCCar> l;
					PlayerCCar *pc;

					l.Instance(player_cars);
					l.Rewind();

					while (l.Iterate(pc))
						pc->save_status(replay_fp);
				}

			} 

			/* keyboard: */
			for (i = 0;i < SDLK_LAST;i++) {
				if (k->keyboard[i] != k->old_keyboard[i]) {
					fputc(replay_cycle&0x0ff, replay_fp);
					fputc(replay_cycle >> 8, replay_fp);
					fputc(0, replay_fp);
					fputc(i&0xff, replay_fp);
					fputc(i >> 8, replay_fp);
					fputc(k->keyboard[i], replay_fp);
				} 
			} 
		} 
	} 


	switch (race_state) {

		case 0:
			/* SEMAPHORE: */
			{
				int i, max, len, stopped_cars = 12, intervals = 8;

				race_semaphore_timmer++;
				race_state_timmer = race_semaphore_timmer;

				if (race_semaphore_timmer == 50 + SEMAPHORE_TIMMER)
					Sound_play(S_semaphore_low, current_player->get_sfx_volume());

				if (race_semaphore_timmer == 50 + SEMAPHORE_TIMMER*2)
					Sound_play(S_semaphore_low, current_player->get_sfx_volume());

				if (race_semaphore_timmer == 50 + SEMAPHORE_TIMMER*3)
					Sound_play(S_semaphore_low, current_player->get_sfx_volume());

				if (race_semaphore_timmer == 50 + SEMAPHORE_TIMMER*4)
					Sound_play(S_semaphore_high, current_player->get_sfx_volume());

				if (race_semaphore_timmer > 50 + SEMAPHORE_TIMMER*4) {
					char tmp[256];
					race_state = 1;
					l.Instance(player_cars);
					l.Rewind();

					while (l.Iterate(v)) {
						v->car->set_state(1);
					} 

					for (i = 0;i < enemy_cars.Length();i++) {
						if (enemy_cars[i]->car->get_state() != 1)
							enemy_cars[i]->car->set_state(1);
					} 

					if (play_music) {
						sprintf(tmp, "%s%s", race_sfx_folder, "music");

						if (Sound_file_test(tmp)) {
							Sound_create_music(tmp, -1);
						} else {
							sprintf(tmp, "%s%s", race_default_sfx_folder, "music");
							Sound_create_music(tmp, -1);
						} 

						Sound_music_volume(current_player->get_music_volume());
					} 
				} 

				if (track->get_track_number() == 1) {
					stopped_cars = 8;
					intervals = 16;
				} 

				if (track->get_track_number() == 4) {
					stopped_cars = 8;
					intervals = 16;
				} 

				if (track->get_track_number() == 21 ||
				        track->get_track_number() == 22) {
					stopped_cars = 32;
					intervals = 8;
				} 

				len = enemy_cars.Length();

				max = (len - stopped_cars) * intervals;

				for (i = 0;i < len - stopped_cars;i++) {
					if (race_semaphore_timmer >= 50 + SEMAPHORE_TIMMER*4 - max + i*intervals)
						if (enemy_cars[i]->car->get_state() != 1)
							enemy_cars[i]->car->set_state(1);
				} 
			}

			break;

		case 1:
			/* RACING: */
			{
				Sound_music_volume(current_player->get_music_volume());

				if (race_semaphore_timmer > 0) {
					race_semaphore_timmer++;

					if (race_semaphore_timmer > 100 + SEMAPHORE_TIMMER*5)
						race_semaphore_timmer = 0;
				} 

				race_time++;

				race_state_timmer++;
			}

			break;

		case 2:
			/* FINISHED: */
			Sound_music_volume(current_player->get_music_volume());
			race_time++;
			race_state_timmer++;

			if (race_state_timmer > 128) {
				race_state = 4;
				race_state_timmer = 0;
			} 

			break;

		case 3:
			/* QUIT/RESTART: */
			race_time++;

			race_state_timmer++;

			if (race_state_timmer > 50) {
				if (replay_fp != 0) {
					fputc(replay_cycle&0x0ff, replay_fp);
					fputc(replay_cycle >> 8, replay_fp);
					fputc(2, replay_fp);
				} 

				return false;
			} 

			break;

		case 4:
			/* RACE FINISHED: */
			race_time++;

			race_state_timmer++;

			{
				float f;
				f = float(50 - race_state_timmer) / 50.0F;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;

				if (play_music)
					Sound_music_volume(int(current_player->get_music_volume()*f));
			}

			if (race_state_timmer > 50) {
				if (replay_fp != 0) {
					fputc(replay_cycle&0x0ff, replay_fp);
					fputc(replay_cycle >> 8, replay_fp);
					fputc(4, replay_fp);
				} 

				return false;
			} 

			break;
	} 

	l2.Instance(enemy_cars);

	l2.Rewind();

	while (l2.Iterate(ec)) {
		if (!ec->cycle(ec->road_position.GetObj(), ec->track, &race_positions, &sfxs, current_player->get_sfx_volume(), car_grid, car_grid_sx, car_grid_sy)) {
			/* Car must be destroyed: */
			todelete.Add(ec);
		} 
	} 

	{
		List<CPlacedGLTile> lc;
		CPlacedGLTile *pt;
		/* Move clouds: */
		lc.Instance(race_clouds);
		lc.Rewind();

		while (lc.Iterate(pt)) {
			// cloud speed
			pt->x -= 0.1;

			// cloud size
			pt->zoom = 0.8f;

			if (pt->x < 0)
				pt->x += float(track->get_dx() * 8);
		} 
	}

	track->cycle();

	l.Instance(player_cars);
	l.Rewind();

	while (l.Iterate(v)) {
		v->c_old_x = v->c_x;
		v->c_old_y = v->c_y;
		v->c_old_a = v->c_a;
		v->c_old_z = v->c_z;

		if (v->car->get_fuel() > 0 || v->car->get_speed() > 1) {
			if (v->car->get_fuel() > 0 && race_outoffuel_timmer != 0)
				race_outoffuel_timmer = 0;

			cars_with_fuel = true;
		} 

		/* zooms: */
		float base_zoom = 1.0F;

		if (v->vdy < 400) {
			base_zoom = 0.75F;
		} else {
			base_zoom = 1.0F;
		} 

		switch (race_state) {

			case 0:
				/* SEMAPHORE: */

				if (race_state_timmer < SEMAPHORE_TIMMER*2) {
					v->c_z = base_zoom * 0.5F;
				} else {
					v->c_z = base_zoom * (0.5F + (race_state_timmer - (SEMAPHORE_TIMMER * 2)) * 0.01F );

					if (v->c_z > 1.0)
						v->c_z = base_zoom * 1.0F;
				} 

				break;

			case 1:
				/* RACING: */
				{
					v->inside_pit = false;
					{
						float cx = v->car->get_x(), cy = v->car->get_y();

						if (v->track->pit_angle != 0) {
							float tx = v->car->get_x() - v->track->pit_x[0];
							float ty = v->car->get_y() - v->track->pit_y[0];
							float a = v->track->pit_angle * M_PI / 180.0F;
							cx = float(tx * cos(a) + ty * sin(a)) + v->track->pit_x[0];
							cy = float( -tx * sin(a) + ty * cos(a)) + v->track->pit_y[0];
						} 

						if (cx >= v->track->pit_x[0] &&
						        cx < v->track->pit_x[1] &&
						        cy >= v->track->pit_y[0] &&
						        cy < v->track->pit_y[1]) {
							v->inside_pit = true;
						} 
					}

					if (v->inside_pit) {
						if (v->car->get_speed() == 0) {
							if (v->c_z < base_zoom*4.0F)
								v->c_z += 0.1F;
							else
								v->c_z = base_zoom * 4.0F;
						} else {
							if (v->c_z > base_zoom)
								v->c_z -= 0.1F;
							else
								v->c_z = base_zoom;
						} 
					} else {
						// default zoom speed = 4
						/*
						float dz = base_zoom * 1.25F;

						if (v->c_z > dz + 0.005F)
							v->c_z -= 0.005F;
						else if (v->c_z < dz - 0.005F)
							v->c_z += 0.005F;
						else
							v->c_z = v->c_z = dz;
						*/
						float dz=v->c_z;
						if (v->car->get_speed()<100) dz=base_zoom*1.2F;
						if (v->car->get_speed()>125 && v->car->get_speed()<225) dz=base_zoom*1.0F;
						if (v->car->get_speed()>250) dz=base_zoom*0.8F;
						if (v->c_z>dz+0.005F) v->c_z-=0.005F;
						else if (v->c_z<dz-0.005F) v->c_z+=0.005F;
						else v->c_z=v->c_z=dz;
					} 
				}

				break;

			case 2:
				/* FINISHED: */

				if (race_state_timmer < 50) {
					v->c_z = base_zoom * (1.0F - race_state_timmer * 0.01F);
				} else {
					v->c_z = base_zoom * 0.5F;
				} 

				break;

			case 3:
				break;

			case 4:
				break;
		} 


		/* Follow the players position && compute terrain: */
		terrain = -1;

		if (v->show_lap_time > 0)
			v->show_lap_time--;

		if (!v->road_position.EmptyP()) {
			LLink<CRoadPiece> *old_pos, *pos;
			CRoadPiece *rp;
			float t1, t2, t3, t4, t5;
			float d, min_d;
			int i;

			old_pos = pos = v->road_position.GetPos();
			rp = v->road_position.GetObj();
			rp->get_path_position(v->car->get_x(), v->car->get_y(), &t1, &t2, &t3, &t4, &t5, &min_d);

			if (rp->type != 0 && rp->inside_test(v->car->get_x(), v->car->get_y())) {
				terrain = 0;
			} 

			for (i = 0;i < 4;i++) {
				if (v->road_position.LastP()) {
					v->road_position.Rewind();
				} else {
					v->road_position.Next();
				} 

				rp = v->road_position.GetObj();

				rp->get_path_position(v->car->get_x(), v->car->get_y(), &t1, &t2, &t3, &t4, &t5, &d);

				if (d < min_d) {
					min_d = d;
					pos = v->road_position.GetPos();

					if (terrain == -1 && rp->type != 0 && rp->inside_test(v->car->get_x(), v->car->get_y())) {
						terrain = 0;
					} 
				} 
			} 

			if (min_d < 160) {
				int p1 = 0, p2 = 0;
				v->road_position.SetPos(old_pos);
				p1 = v->road_position.PositionRef(v->road_position.GetObj());
				v->road_position.SetPos(pos);
				p2 = v->road_position.PositionRef(v->road_position.GetObj());
				/* Passed by the start line: */

				if (p2 < p1) {
					if (v->first_lap) {
						int prev_time = 0, i;

						if (replay_fp != 0) {
							fputc(replay_cycle&0x0ff, replay_fp);
							fputc(replay_cycle >> 8, replay_fp);
							fputc(3, replay_fp);
							fputc(player_cars.PositionRef(v), replay_fp);
						} 

						if (v->laps < race_nlaps) {
							for (i = 0;i < v->laps;i++)
								prev_time += v->lap_time[i];

							v->lap_time[v->laps] = race_time - prev_time;

							v->show_lap_time = 200;
						} 

						v->laps++;

						if (v->laps >= race_nlaps) {
							race_state = 2;
							race_state_timmer = 0;

							if (race_sfx_channel >= (N_SFX_CHANNELS - 1))
								race_sfx_channel = race_first_sfx_channel;

							Sound_play_ch(S_race_finished, race_sfx_channel++, current_player->get_sfx_volume());
						} 
					} else {
						v->first_lap = true;
					} 
				} 
			} else {
				v->road_position.SetPos(old_pos);
			} 

			if (terrain == -1) {
				CRoadPiece *rp;
				int i = v->road_position.PositionRef(v->road_position.GetObj());

				if (i == 0) {
					rp = v->road_position[v->road_position.Length() - 1];
				} else {
					rp = v->road_position[i + 1];

					if (rp == 0)
						v->road_position[0];
				} 

				if (rp != 0 &&
				        rp->type != 0 &&
				        rp->inside_test(v->car->get_x(), v->car->get_y())) {
					terrain = 0;
				} 
			} 

			if (terrain == -1) {
				if (track->get_background_type() == 1)
					terrain = 1;

				if (track->get_background_type() == 2)
					terrain = 2;
			} 

		} 

		/* Compute position: */
		{
			List<CRoadPiece> lr;
			CRoadPiece *rp;
			int effective_laps;

			if (!v->first_lap) {
				effective_laps = -1;
			} else {
				effective_laps = v->laps;
			} 

			/* laps: */
			v->position = effective_laps * v->track->get_length();

			/* pieces: */
			lr.Instance(v->track->road);

			lr.Rewind();

			while (lr.Iterate(rp) && rp != v->road_position.GetObj())
				v->position += rp->get_length();

			/* offset inside current piece: */
			/* simplification: inverse of the distance to end of piece = distance */
			{
				v->position += rp->get_length();
				float d = float(sqrt((v->car->get_x() - rp->get_x2()) * (v->car->get_x() - rp->get_x2()) +
				                     (v->car->get_y() - rp->get_y2()) * (v->car->get_y() - rp->get_y2())));
				v->position -= d;
			}

			//#ifdef F1SPIRIT_DEBUG_MESSAGES
			//   output_debug_message("Player pos: (%i - %i) %g\n",effective_laps,v->laps,v->position);
			//#endif

		}

		{
			int turn = 0, gear = 0;

			if (k->keyboard[v->left] && !k->keyboard[v->right])
				turn = -1;

			if (k->keyboard[v->right] && !k->keyboard[v->left])
				turn = 1;

			if (k->keyboard[v->up] && !k->old_keyboard[v->up] && !k->keyboard[v->down])
				gear = 1;

			if (k->keyboard[v->down] && !k->old_keyboard[v->down] && !k->keyboard[v->up])
				gear = -1;

			v->car->cycle(k->keyboard[v->accelerate] != 0,
			              k->keyboard[v->brake] != 0,
			              gear,
			              turn,
			              v->road_position.GetObj(),
			              v->track,
			              &race_positions,
			              &sfxs,
			              current_player->get_sfx_volume(),
			              true,
			              //        false,
			              terrain,
			              v->position,
			              car_grid, car_grid_sx, car_grid_sy);

			{
				float offset, rangle;

				if (v->road_position.GetObj()->offset_from_road_center(v->car->get_x(), v->car->get_y(), &offset, &rangle)) {
					int wide = int(v->road_position.GetObj()->get_w1() / 2);

					if ((offset >= wide && offset <= wide + 16 && v->road_position.GetObj()->ltype == 2) ||
					        (offset <= -wide && offset >= -(wide + 16) && v->road_position.GetObj()->rtype == 2)) {
						/* Chicane sound: */
						if ((v->car->chicane_timmer % 5) == 0) {
							F1S_SFX *s = new F1S_SFX();
							s->x = int(v->car->get_x());
							s->y = int(v->car->get_y());
							s->SFX = SFX_CHICANE;
							sfxs.Add(s);
						} 

						v->car->chicane_timmer++;
					} else {
						v->car->chicane_timmer = 0;
					} 
				} 
			}
		}

		{
			bool change_a = true;
			float desired_a;
			float dif;

			// camera type 2
			v->c_x = v->car->get_x();
			v->c_y = v->car->get_y();

			// camera type 2
			/* Follow track: */

			if (!v->road_position.EmptyP()) {
				CRoadPiece *rp;
				float px, py, pz, pa, pw, pd;

				rp = v->road_position.GetObj();

				if (rp->get_w1() != rp->get_w2())
					change_a = false;

				if (rp->get_path_position(v->car->get_x(), v->car->get_y(), &px, &py, &pz, &pa, &pw, &pd)) {
					desired_a = -pa;
				} else {
					desired_a = 90 - v->car->get_a();
				} 
			} else {
				desired_a = 90 - v->car->get_a();
			} 

			if (change_a) {
				dif = desired_a - v->c_a;

				while (dif > 180)
					dif -= 360;

				while (dif < -180)
					dif += 360;

				// Make the rotation of the camera not to stop abruptly!
				{
					float d = fabs(dif);

					if (d > 20) {
						v->c_a_speed = 2;
					} else {
						v->c_a_speed = d / 10;
					} 

					if (dif < 0)
						v->c_a_speed = -v->c_a_speed;
				}


				if (dif > 0) {
					if (dif > v->c_a_speed)
						v->c_a += v->c_a_speed;
					else
						v->c_a = desired_a;
				} else {
					if (dif < v->c_a_speed)
						v->c_a += v->c_a_speed;
					else
						v->c_a = desired_a;
				} 
			} 
		}

		/* mini dispersed view damage control: */
		{
			if (v->car->get_new_damage()) {
				switch (v->show_damage_cycle) {

					case 0:
						v->show_damage_cycle = 1;
						v->show_damage_timmer = 0;
						break;

					case 1:
						break;

					case 2:
						v->show_damage_timmer = 0;
						break;

					case 3:
						v->show_damage_cycle = 1;
						v->show_damage_timmer = 50 - v->show_damage_timmer;
						break;
				} 

				v->car->set_new_damage(false);
			} 

			switch (v->show_damage_cycle) {

				case 0:
					break;

				case 1:
					v->show_damage_timmer++;

					if (v->show_damage_timmer >= 50) {
						v->show_damage_cycle = 2;
						v->show_damage_timmer = 0;
					} 

					break;

				case 2:
					v->show_damage_timmer++;

					if (v->show_damage_timmer >= 50) {
						v->show_damage_cycle = 3;
						v->show_damage_timmer = 0;
					} 

					break;

				case 3:
					v->show_damage_timmer++;

					if (v->show_damage_timmer >= 50) {
						v->show_damage_cycle = 0;
						v->show_damage_timmer = 0;
					} 

					break;
			} 
		}


		/* pit stops: */
		{
			v->inside_pit = false;
			{
				float cx = v->car->get_x(), cy = v->car->get_y();

				if (v->track->pit_angle != 0) {
					float tx = v->car->get_x() - v->track->pit_x[0];
					float ty = v->car->get_y() - v->track->pit_y[0];
					float a = (v->track->pit_angle) * M_PI / 180.0F;
					cx = float(tx * cos(a) + ty * sin(a)) + v->track->pit_x[0];
					cy = float( -tx * sin(a) + ty * cos(a)) + v->track->pit_y[0];
				} 

				if (cx >= v->track->pit_x[0] &&
				        cx < v->track->pit_x[1] &&
				        cy >= v->track->pit_y[0] &&
				        cy < v->track->pit_y[1]) {
					v->inside_pit = true;
				} 
			}

			if (v->inside_pit) {
				if (v->car->get_speed() == 0) {
					/* car stopped in boxes: */
					if (v->show_pit_stop == -1) {
						v->pit_time = 0;
						v->show_pit_stop = 0;
					} else {
						v->pit_time++;

						/* Add fuel and repair: */

						if (k->keyboard[v->down]) {
							v->car->set_fuel(v->car->get_fuel() + (parameters.fuel_recharge_speed / 2));
							v->car->set_brake_damage(v->car->get_brake_damage() - (parameters.repair_speed*2));
							v->car->set_rtyre_damage(v->car->get_rtyre_damage() - (parameters.repair_speed*2));
							v->car->set_ftyre_damage(v->car->get_ftyre_damage() - (parameters.repair_speed*2));
							v->car->set_engine_damage(v->car->get_engine_damage() - (parameters.repair_speed*2));
						} else {
							v->car->set_fuel(v->car->get_fuel() + parameters.fuel_recharge_speed);
							v->car->set_brake_damage(v->car->get_brake_damage() - parameters.repair_speed);
							v->car->set_rtyre_damage(v->car->get_rtyre_damage() - parameters.repair_speed);
							v->car->set_ftyre_damage(v->car->get_ftyre_damage() - parameters.repair_speed);
							v->car->set_engine_damage(v->car->get_engine_damage() - parameters.repair_speed);
						} 
					} 
				} else {
					if (v->show_pit_stop >= 0) {
						v->show_pit_stop++;

						if (v->show_pit_stop >= 100)
							v->show_pit_stop = -1;
					} 
				} 
			} else {
				if (v->show_pit_stop >= 0) {
					v->show_pit_stop++;

					if (v->show_pit_stop >= 100)
						v->show_pit_stop = -1;
				} 
			} 
		}

		// weather:
		if (m_rain_period1 != 0 || m_rain_period2 != 0) {
			int rain_setup_time = 250;
			// compute the rain strength:

			m_current_rain_strength = 0.0f;

			if (m_rain_period1 < 0) {
				m_current_rain_strength = 0;
			} else if (m_rain_period2 < 0) {
				if (m_rain_period1 == 0) {
					m_current_rain_strength = 1.0f;
				} else {
					if (replay_cycle < m_rain_period1) {
						m_current_rain_strength = 0;
					} else if (replay_cycle < m_rain_period1 + rain_setup_time) {
						m_current_rain_strength = float(replay_cycle - m_rain_period1) / float(rain_setup_time);
					} else {
						m_current_rain_strength = 1.0f;
					} // if
				} // if
			} else {
				int time = replay_cycle / (m_rain_period1 + m_rain_period2);
				int cycle = replay_cycle % (m_rain_period1 + m_rain_period2);

				if (time < m_rain_times) {
					if (cycle < m_rain_period1) {
						m_current_rain_strength = 0;
					} else if (cycle < m_rain_period1 + rain_setup_time) {
						m_current_rain_strength = float(cycle - m_rain_period1) / float(rain_setup_time);
					} else if (cycle > m_rain_period1 + m_rain_period2 - rain_setup_time) {
						m_current_rain_strength = float(m_rain_period1 + m_rain_period2 - cycle) / float(rain_setup_time);
					} else {
						m_current_rain_strength = 1.0f;
					} // if
				} else {
					m_current_rain_strength = 0.0f;
				} // if
			} // if

			if (m_current_rain_strength < 0) {
				m_current_rain_strength = 0;
			}

			if (m_current_rain_strength > 1.0) {
				m_current_rain_strength = 1.0f;
			}

			cycle_rain(v, m_current_rain_strength * m_rain_strength);

			// play rain sfx

			if (rain_channel == 0) {
				rain_channel = Mix_FadeInChannel(SFX_RAIN, S_rain, -1, 2000);
			}

		} else {
			m_current_rain_strength = 0.0f;
			// stop rain sfx

			if (rain_channel != 0) {
				Mix_FadeOutChannel(rain_channel, 1000);
				rain_channel = 0;
			}
		}


	} 

	if (cars_with_fuel) {
		race_outoffuel_timmer = 0;
	} else {
		race_outoffuel_timmer++;

		if (race_outoffuel_timmer >= 50 && race_state < 3) {
			if (v->laps >= race_nlaps) {
				if (race_state != 2) {
					race_state = 2;
					race_state_timmer = 0;
				} 
			} else {
				race_state = 3;
				race_state_timmer = 0;
			} 
		} 
	} 

	/* Update orders: */
	/* Test for CAR_PASS SFX to be played: */
	{
		int i;
		int n = player_cars.Length();
		int np;
		int *positions;

		positions = new int[n];

		for (i = 0;i < n;i++)
			positions[i] = race_positions.PositionRef((RacingCCar *)player_cars[i]);

		if (race_state < 2)
			race_positions.Sort(&cars_order);

		for (i = 0;i < n;i++) {
			np = race_positions.PositionRef((RacingCCar *)player_cars[i]);

			if (np >= 0 && positions[i] >= 0) {
				if (np > positions[i]) {
					if (fabs(race_positions[np - 1]->car->get_speed() -
					         race_positions[np]->car->get_speed()) > 50) {
						F1S_SFX *s = new F1S_SFX();
						s->x = int(race_positions[np]->car->get_x());
						s->y = int(race_positions[np]->car->get_y());
						s->SFX = SFX_PASS;
						sfxs.Add(s);
					} 
				} 

				if (np < positions[i]) {
					if (fabs(race_positions[np + 1]->car->get_speed() -
					         race_positions[np]->car->get_speed()) > 50) {
						F1S_SFX *s = new F1S_SFX();
						s->x = int(race_positions[np]->car->get_x());
						s->y = int(race_positions[np]->car->get_y());
						s->SFX = SFX_PASS;
						sfxs.Add(s);
					} 
				} 
			} 
		} 

		delete positions;
	}

	/* Delete crashed cars: */
	{
		EnemyCCar *ec;

		while (!todelete.EmptyP()) {
			ec = todelete.ExtractIni();

			race_positions.DeleteElement(ec);
			enemy_cars.DeleteElement(ec);
			delete ec;
		} 
	}


	/* RECOMPUTE CAR GRID: */
	{
		int x, y;
		int i, j;

		for (i = 0;i < CAR_GRID_SIZE;i++) {
			for (j = 0;j < CAR_GRID_SIZE;j++) {
				while (!car_grid[i][j].EmptyP())
					car_grid[i][j].ExtractIni();
			} 
		} 

		l2.Instance(enemy_cars);

		l2.Rewind();

		while (l2.Iterate(ec)) {
			x = int(ec->car->get_x() / car_grid_sx);
			y = int(ec->car->get_y() / car_grid_sy);
			car_grid[x][y].Add(ec);
		} 

		l.Instance(player_cars);

		l.Rewind();

		while (l.Iterate(v)) {
			x = int(v->car->get_x() / car_grid_sx);
			y = int(v->car->get_y() / car_grid_sy);
			car_grid[x][y].Add(v);
		} 
	}


	/* Play SFX: */
	{
		F1S_SFX *s;
		float distance, d;
		bool first;
		int volume;

		while (!sfxs.EmptyP()) {
			s = sfxs.ExtractIni();
			/* find the closest player: */

			first = true;
			l.Instance(player_cars);
			l.Rewind();

			while (l.Iterate(v)) {
				d = float(sqrt((v->car->get_x() - s->x) * (v->car->get_x() - s->x) +
				               (v->car->get_y() - s->y) * (v->car->get_y() - s->y)));

				if (first || d < distance) {
					distance = d;
					first = false;
				} 
			} 

			volume = current_player->get_sfx_volume();

			if (distance > 256) {
				float f = (512 - distance) / 256.0F;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;

				volume = int(volume * f);
			} 

			// FIXME: reserve a channel for the rain sfx properly
			if (race_sfx_channel >= (N_SFX_CHANNELS - 1))
				race_sfx_channel = race_first_sfx_channel;

			if (volume > 0) {
				switch (s->SFX) {

					case SFX_BRAKE_STOCK:
						Sound_play_ch(S_car_brake, race_sfx_channel++, volume);
						break;

					case SFX_HIT_WOOD:
						Sound_play_ch(S_car_hit1, race_sfx_channel++, volume);
						break;

					case SFX_HIT_METAL:
						Sound_play_ch(S_car_hit2, race_sfx_channel++, volume);
						break;

					case SFX_HIT_CRASH:
						Sound_play_ch(S_car_hit3, race_sfx_channel++, volume);
						break;

					case SFX_HIT_CAR:
						Sound_play_ch(S_car_hit4, race_sfx_channel++, volume);
						break;

					case SFX_FUEL:
						Sound_play_ch(S_outof_fuel, race_sfx_channel++, volume);
						break;

					case SFX_WATER:
						Sound_play_ch(S_water_splash, race_sfx_channel++, volume);
						break;

					case SFX_SQUEAL:
						Sound_play_ch(S_squeal, race_sfx_channel++, volume);
						break;

					case SFX_CHICANE:
						Sound_play_ch(S_chicane, race_sfx_channel++, volume);
						break;

					case SFX_PASS:
						Sound_play_ch(S_car_pass, race_sfx_channel++, volume);
						break;
				} 
			} 

			delete s;
		} 

	}

	replay_cycle++;

	//#ifdef F1SPIRIT_DEBUG_MESSAGES
	// if ((replay_cycle%10)==0) output_debug_message("Cycle %i\n",replay_cycle);
	//#endif


	return true;
} 


void F1SpiritGame::draw(bool draw_scoreboard)
{

	PlayerCCar *v;
	List<PlayerCCar> l;
	EnemyCCar *ec;
	List<EnemyCCar> l2;

	l.Instance(player_cars);
	l.Rewind();

	while (l.Iterate(v)) {

		//  v->c_z=0.25;

		{
			GLfloat val[4];
			float fx = 1.0F, fy = 1.0F;

			glGetFloatv(GL_VIEWPORT, val);
			fx = val[2] / 640.0F;
			fy = val[3] / 480.0F;

			glEnable( GL_SCISSOR_TEST );
			glScissor(int(val[0] + v->vx*fx), int(val[1] + (480 - (v->vy + v->vdy))*fy), int(v->vdx*fx), int(v->vdy*fy));
		}

		glPushMatrix();
		/* Track: */
		{
			SDL_Rect vp;
			List<CPlacedGLTile> extras;

			glPushMatrix();
			{
				// camera type 2
				glTranslatef(float(v->vx + v->vdx / 2), float(v->vy + 3*(v->vdy / 4)), 0);

				vp.y = -v->vdy / 2;
				vp.h = v->vdy;
				vp.x = -v->vdx / 2;
				vp.w = v->vdx;
			}

			glRotatef(v->c_a - 90, 0, 0, 1);

			/* Add the car + the shadow of the car: */
			{
				PlayerCCar *v2;
				List<PlayerCCar> l;
				EnemyCCar *e;
				List<EnemyCCar> l2;
				List<CPlacedGLTile> todelete, l3;
				CPlacedGLTile *pt;
				List<GLTile> todelete2;
				int i = 0;

				l.Instance(player_cars);
				l.Rewind();

				while (l.Iterate(v2)) {
					i++;

					if (v2->car->get_state() == 3) {
						v2->car->get_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.2)));
						v2->car->get_shadow_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.2)));
					} 


					if (v2 == v) {
						if (race_state == 0) {
							v2->car->get_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.1)));
							v2->car->get_shadow_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.1)));
						} 
					} else {
						char tmp[80];
						SDL_Surface *sfc;
						GLTile *t;
						CPlacedGLTile *pt;

						sprintf(tmp, "P%i", i);
						sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
						print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
						t = new GLTile(sfc);
						t->set_hotspot(sfc->w / 2, sfc->h / 2);
						pt = new CPlacedGLTile(v2->car->get_x() + 32, v2->car->get_y(), v2->car->get_z() - 8, -(v->c_a - 90), t);
						extras.Add(pt);
						todelete.Add(pt);
						todelete2.Add(t);
					} 

					v2->car->add_drawing_extras(&extras);
				} 

				i = 0;

				l2.Instance(enemy_cars);

				l2.Rewind();

				while (l2.Iterate(e)) {
					if (e->car->get_state() == 3) {
						e->car->get_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.2)));
						e->car->get_shadow_placedtile()->set_color(1, 1, 1, 0.6F + 0.4F*float(sin(race_state_timmer*0.2)));
					} 

					e->car->add_drawing_extras(&extras);
				} 

				/* Clouds: */
				l3.Instance(race_clouds);

				l3.Rewind();

				while (l3.Iterate(pt)) {
					extras.Add(pt);
				} 


				v->track->draw(&vp, v->c_x, v->c_y, v->c_z, &extras, road_tile, road_ltile, road_rtile, road_lines);

				l2.Instance(enemy_cars);

				l2.Rewind();

				while (l2.Iterate(e)) {
					if (e->car->get_state() == 3) {
						e->car->get_placedtile()->set_color(1, 1, 1, 1);
						e->car->get_shadow_placedtile()->set_color(1, 1, 1, 1);
					} 
				} 

				l.Instance(player_cars);

				l.Rewind();

				while (l.Iterate(v2)) {
					if (v2->car->get_state() == 3) {
						v2->car->get_placedtile()->set_color(1, 1, 1, 1);
						v2->car->get_shadow_placedtile()->set_color(1, 1, 1, 1);
					} 

					if (v2 == v && race_state == 0) {
						v2->car->get_placedtile()->set_color(1, 1, 1, 1);
						v2->car->get_shadow_placedtile()->set_color(1, 1, 1, 1);
					} 
				} 

				todelete.Delete();

				todelete2.Delete();

				while (!extras.EmptyP())
					extras.ExtractIni();

			}

			/* Weather: */
			draw_rain(v, m_current_rain_strength*m_rain_strength);

			if (track->get_track_number() == 4) {
				if (g_stencil_bits == 0) draw_nightmode_nostencil(v);
				else draw_nightmode_stencil(v);
			} 

			glPopMatrix();
		}

		glPopMatrix();

		// Draw the signs:
		{
			float sign_cycle = (replay_cycle % 24) / 24.0F;
			float f = 0.5F + (float)(sin(2 * sign_cycle * M_PI - (M_PI / 2)) * 0.5F);

			if (sign_cycle == 0) {
				int tmp;
				//    tmp=track->track_sign_to_show(v->c_x,v->c_y,&v->road_position);
				{
					GLTile **arrow_tiles;
					arrow_tiles = new GLTile * [10];
					arrow_tiles[0] = tiles[4];
					arrow_tiles[1] = tiles[5];
					arrow_tiles[2] = 0;
					arrow_tiles[3] = 0;
					arrow_tiles[4] = tiles[7];
					arrow_tiles[5] = tiles[6];
					arrow_tiles[6] = 0;
					arrow_tiles[7] = 0;
					arrow_tiles[8] = 0;
					arrow_tiles[9] = 0;
					tmp = track->track_sign_to_show2(v->c_x, v->c_y, v->position, arrow_tiles);
					delete []arrow_tiles;
				}

				if (tmp != -1 && tmp != v->last_sign_showing) {
					v->last_sign_showing = tmp;
					v->times_sign_showed = 0;
				} else {
					v->times_sign_showed++;

					if (v->times_sign_showed >= 4) {
						v->last_sign_showing = -1;
						v->times_sign_showed = 0;
					} 
				} 
			} 


			if (v->last_sign_showing >= 0)
				signs[v->last_sign_showing]->draw(1, 1, 1, f, float(v->vx + v->vdx / 2), float(v->vy + 128), 0, 0, 1);
		}

		if (race_semaphore_timmer < 100 + (SEMAPHORE_TIMMER*5)) {
			int k = (race_semaphore_timmer - 50) / SEMAPHORE_TIMMER;
			int k2 = k + 1;
			float f1 = 1.0;
			float f2 = 0.0;

			if (race_semaphore_timmer < 50)
				f1 = race_semaphore_timmer * 0.02F;

			if (race_semaphore_timmer > 50 + (SEMAPHORE_TIMMER*5))
				f1 = (100 + (SEMAPHORE_TIMMER * 5) - race_semaphore_timmer) * 0.02F;

			if (f1 < 0)
				f1 = 0;

			if (f1 > 1)
				f1 = 1;

			{
				int t = SEMAPHORE_TIMMER - ((race_semaphore_timmer - 50) - k * SEMAPHORE_TIMMER);

				if (t < 10)
					f2 = (10 - t) * 0.1F;
			}


			if (k < 0)
				k = 0;

			if (k > 4)
				k = 4;

			if (k >= 0 && k <= 4)
				race_semaphore[k]->draw(1, 1, 1, f1, 300, 128, 0, 0, 1);

			if (k2 >= 0 && k2 <= 4)
				race_semaphore[k2]->draw(1, 1, 1, f2, 300, 128, 0, 0, 1);
		} 

		/* Out of fuel message: */
		{
			if (v->car->get_fuel() <= 0 && v->car->get_speed() < 1 && !v->inside_pit) {
				char tmp[80];
				SDL_Surface *sfc;
				GLTile *t;

				sprintf(tmp, "OUT OF FUEL");
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(float(v->vx + v->vdx / 2), float(v->vy + v->vdy / 2 + 32), 0, 0, 1);

				delete t;
			} 

		}

		/* Wrong way message: */
		{
			float t1, t2, t3, t4, t5, pa, ac;
			v->road_position.GetObj()->get_path_position(v->car->get_x(), v->car->get_y(), &t1, &t2, &t3, &pa, &t4, &t5);
			ac = v->car->get_a() - 90;

			{
				float dif = ac - pa;

				if (dif >= 360)
					dif -= 360;

				if (dif <= -360)
					dif += 360;

				if (fabs(dif) < 90 || fabs(dif) > 270) {
					v->wrong_way = false;
				} else {
					v->wrong_way = true;
				} 

				if (v->car->spinning_timmer > 0)
					v->wrong_way = false;
			}

			if (v->wrong_way && v->car->get_state() == 1) {
				char tmp[80];
				SDL_Surface *sfc;
				GLTile *t;

				sprintf(tmp, "WRONG WAY!");
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(float(v->vx + v->vdx / 2), float(v->vy + v->vdy / 2 + 16), 0, 0, 1);

				delete t;
			} 

			//   glEnable( GL_DEPTH_TEST );

		}

		/* Pit time: */
		if (v->show_pit_stop >= 0) {
			char tmp[80];
			SDL_Surface *sfc;
			GLTile *t;
			float f;
			{
				int m, s, c;

				c = v->pit_time * 2;
				s = c / 100;
				m = s / 60;
				c = c % 100;
				s = s % 60;
				sprintf(tmp, "PIT TIME: %i:%.2i:%.2i", m, s, c);
			}

			if (v->show_pit_stop < 50) {
				f = 1;
			} else {
				f = (100 - v->show_pit_stop) * 0.2F;
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->set_hotspot(sfc->w / 2, 0);

			t->draw(1, 1, 1, f, float(v->vx + v->vdx / 2), float(v->vy + v->vdy / 2), 0, 0, 1);

			delete t;
		} 


		/* Lap time: */
		if (v->show_lap_time > 0) {
			char tmp[80];
			SDL_Surface *sfc;
			GLTile *t;
			float f;
			{
				int m, s, c;

				c = v->lap_time[v->laps - 1] * 2;
				s = c / 100;
				m = s / 60;
				c = c % 100;
				s = s % 60;
				sprintf(tmp, "LAP TIME: %i:%.2i:%.2i", m, s, c);
			}

			if (v->show_lap_time > 150) {
				f = (200 - v->show_lap_time) * 0.02F;
			} 

			if (v->show_lap_time <= 150 &&
			        v->show_lap_time > 50) {
				f = 1;
			} 

			if (v->show_lap_time <= 50) {
				f = v->show_lap_time * 0.02F;
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->set_hotspot(sfc->w / 2, 0);

			t->draw(1, 1, 1, f, float(v->vx + v->vdx / 2), float(v->vy + v->vdy / 2), 0, 0, 1);

			delete t;

			if (v->laps == race_nlaps - 1) {
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FINAL LAP", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)"FINAL LAP", font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(1, 1, 1, f*(0.5F + 0.5F*(cos(v->show_lap_time))), float(v->vx + v->vdx / 2), float(v->vy + v->vdy / 2) + 16, 0, 0, 1);
				delete t;
			} 
		} 


		/* Scoreboard: */
		if (draw_scoreboard) {
			if (player_cars.Length() > 1) {
				draw_mini_dispersed_hud(v);
			} else {
				draw_dispersed_hud(v);
			} 
		} 

	} // while

	glDisable( GL_SCISSOR_TEST );

	/* Draw the minimap: */
	if (draw_scoreboard) {
		int mini_x = 640 - 214, mini_y = 18 + 48;
		float alpha = 0.5F;

		if (player_cars.Length() > 1) {
			mini_x = 320 - (race_minitrack->get_dx()) / 2;
			mini_y = 240 - (race_minitrack->get_dy()) / 2;
			alpha = 1.0F;

			/* Draw vertical minimap: */
			race_minitrack->draw(1, 1, 1, alpha, float(mini_x), float(mini_y), 0, 0, 1);

			if (race_state == 1) {
				if (race_state_timmer > 350) {
					float f = 1.0;

					if (race_state_timmer < 400)
						f = (race_state_timmer - 350) * 0.02F;

					l2.Instance(enemy_cars);

					l2.Rewind();

					while (l2.Iterate(ec)) {
						float y = 0, max_y = race_minitrack->get_dy() - 16;
						y = (ec->position / (track->get_length() * race_nlaps)) * max_y;

						if (y > max_y)
							y = max_y;

						if (y < 0)
							y = 0;

						ec->car->draw_mini(1, 320, 240 + (race_minitrack->get_dy() - 16) / 2 - y, 0, 0, 1);
					} 
				} 
			} 

			l.Instance(player_cars);

			l.Rewind();

			while (l.Iterate(v)) {
				float y = 0, max_y = race_minitrack->get_dy() - 16;
				y = (v->position / (track->get_length() * race_nlaps)) * max_y;

				if (y > max_y)
					y = max_y;

				if (y < 0)
					y = 0;

				v->car->draw_mini(1, 320, 240 + (race_minitrack->get_dy() - 16) / 2 - y, 0, 0, 1);
			} 


		} else {
			bool orientation = true;

			mini_x = 0;
			mini_y = 64 + race_minitrack->get_dx();
			alpha = 1.0F;
			orientation = false;

			if (orientation) {
				race_minitrack->draw(1, 1, 1, alpha, float(mini_x), float(mini_y), 0, 0, 1);

				if (race_state == 1) {
					if (race_state_timmer > 350) {
						float f = 1.0;

						if (race_state_timmer < 400)
							f = (race_state_timmer - 350) * 0.02F;

						l2.Instance(enemy_cars);

						l2.Rewind();

						while (l2.Iterate(ec)) {
							ec->car->draw_mini(f, mini_x + ec->car->get_x()*race_minimap_zoom, mini_y + ec->car->get_y()*race_minimap_zoom, 0, 0, 1);
						} 
					} 
				} 

				l.Instance(player_cars);

				l.Rewind();

				while (l.Iterate(v)) {
					v->car->draw_mini(1, mini_x + v->car->get_x()*race_minimap_zoom, mini_y + v->car->get_y()*race_minimap_zoom, 0, 0, 1);
				} 
			} else {
				race_minitrack->draw(1, 1, 1, alpha, float(mini_x), float(mini_y), 0, -90, 1);

				if (race_state == 1) {
					if (race_state_timmer > 350) {
						float f = 1.0;

						if (race_state_timmer < 400)
							f = (race_state_timmer - 350) * 0.02F;

						l2.Instance(enemy_cars);

						l2.Rewind();

						while (l2.Iterate(ec)) {
							ec->car->draw_mini(f, mini_x + ec->car->get_y()*race_minimap_zoom, mini_y - ec->car->get_x()*race_minimap_zoom, 0, 0, 1);
						} 
					} 
				} 

				l.Instance(player_cars);

				l.Rewind();

				while (l.Iterate(v)) {
					v->car->draw_mini(1, mini_x + v->car->get_y()*race_minimap_zoom, mini_y - v->car->get_x()*race_minimap_zoom, 0, 0, 1);
				} 
			} 
		} 
	} 

	if (race_state == 0) {
		{
			float f = 0;
			f = abs(race_state_timmer) / 50.0F;

			if (f < 0)
				f = 0;

			if (f > 1)
				f = 1;

			f = 1 - f;

			glColor4f(0, 0, 0, f);
		}

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -4);
		glVertex3f(0, 480, -4);
		glVertex3f(640, 480, -4);
		glVertex3f(640, 0, -4);
		glEnd();

	} 

	if (race_state == 4 || race_state == 3) {
		{
			float f = 0;
			f = abs(race_state_timmer) / 50.0F;

			if (f < 0)
				f = 0;

			if (f > 1)
				f = 1;

			glColor4f(0, 0, 0, f);
		}

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -4);
		glVertex3f(0, 480, -4);
		glVertex3f(640, 480, -4);
		glVertex3f(640, 0, -4);
		glEnd();
	} 

} 


void F1SpiritGame::draw_dispersed_hud(PlayerCCar *v)
{
	char tmp[80];

	/* Fixed part: */
	hud_fixed_1->draw(float(v->vx), float(v->vy), 0, 0, 1);
	hud_fixed_2->draw(float(v->vx + 440), float(v->vy), 0, 0, 1);
	hud_fixed_3->draw(float(v->vx), float(v->vy + 340), 0, 0, 1);
	hud_fixed_4->draw(float(v->vx + 498), float(v->vy + 432), 0, 0, 1);

	/* Variable part: */
	/* TIMES: */
	{
		int i, j, x = 537; /* y = 27 */

		if (v->laps > 0) {
			unsigned int best_time = v->lap_time[0];
			int m, s, c;

			for (i = 1;i < v->laps;i++)
				if (v->lap_time[i] < best_time)
					best_time = v->lap_time[i];

			c = best_time * 2;

			s = c / 100;

			m = s / 60;

			c = c % 100;

			s = s % 60;

			sprintf(tmp, "%i:%.2i:%.2i", m, s, c);
		} else {
			sprintf(tmp, "0:00:00");
		} 

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 9, 0, 0, 1);

			x += 14;
		} 

		x = 537;

		if (v->laps > 0) {
			int m, s, c;

			c = v->lap_time[v->laps - 1] * 2;
			s = c / 100;
			m = s / 60;
			c = c % 100;
			s = s % 60;
			sprintf(tmp, "%i:%.2i:%.2i", m, s, c);
		} else {
			sprintf(tmp, "0:00:00");
		} 

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 33, 0, 0, 1);

			x += 14;
		} 

		x = 537;

		{
			int prev_time = 0;
			int m, s, c;

			for (i = 0;i < v->laps;i++)
				prev_time += v->lap_time[i];

			c = (race_time - prev_time) * 2;

			s = c / 100;

			m = s / 60;

			c = c % 100;

			s = s % 60;

			sprintf(tmp, "%i:%.2i:%.2i", m, s, c);
		}

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 57, 0, 0, 1);

			x += 14;
		} 
	}

	/* POSITION: */
	{
		int i, j, x = 16; /* y = 27 */
		sprintf(tmp, "%.2i/%.2i", race_positions.PositionRef((RacingCCar *)v) + 1, race_positions.Length());

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 27, 0, 0, 1);

			x += 14;
		} 
	}

	/* LAP: */
	{
		int i, j, x = 112; /* y = 27 */
		sprintf(tmp, "%i/%i", v->laps + 1, race_nlaps);

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 27, 0, 0, 1);

			x += 14;
		} 
	}

	/* SPEED: */
	{
		int i, j;
		int w;
		int x = 496; /* y = 438 */
		sprintf(tmp, "%i", int(v->car->get_speed()));

		for (i = strlen(tmp) - 1;i >= 0;i--) {
			j = tmp[i] - '0';

			if (j < 0)
				j = 0;

			if (j > 9)
				j = 9;

			w = 20;

			//   if (j==1) w=10;
			hud_speed_font[j]->draw(v->vx + x - w, v->vy + 438, 0, 0, 1);

			x -= w;
		} 
	}

	/* GEAR: */
	hud_gear[v->car->get_currentgear()]->draw(float(v->vx + 613), float(v->vy + 447), 0, 0, 1);

	/* RPM: */
	{
		float f = float(v->car->get_relative_rpm() * v->car->get_relative_rpm());
		int n = int(f * 12);

		if (n < 0)
			n = 0;

		if (n > 11)
			n = 11;

		if (n > 0) {
			hud_rpm[n - 1]->draw(float(v->vx + 507), float(v->vy + 443), 0, 0, 1);
		} 
	}

	/* fuel: */
	{
		float f = v->car->get_fuel();
		int n = int(((f * 8) + (parameters.MAX_FUEL - 1)) / parameters.MAX_FUEL);
		int i;

		for (i = 0;i < n;i++) {
			race_fuel->draw(float(v->vx + 53) + i*16, float(v->vy + 360), 0, 0, 1);
		} 
	}

	/* damage: */
	{
		float scale_r[3] = {0, 1, 1};
		float scale_g[3] = {1, 1, 0};
		float scale_b[3] = {0, 0, 0};
		float d[4] = {0, 0, 0, 0}, dr = 0, dg = 1, db = 0, da = 1;
		int i;

		d[0] = v->car->get_ftyre_damage();
		d[1] = v->car->get_brake_damage();
		d[2] = v->car->get_engine_damage();
		d[3] = v->car->get_rtyre_damage();

		for (i = 0;i < 4;i++) {
			if (d[i] < 0.5F) {
				dr = scale_r[0] * (1 - d[i] * 2) + scale_r[1] * (d[i] * 2);
				dg = scale_g[0] * (1 - d[i] * 2) + scale_g[1] * (d[i] * 2);
				db = scale_b[0] * (1 - d[i] * 2) + scale_b[1] * (d[i] * 2);
			} else {
				dr = scale_r[1] * (1 - (d[i] - 0.5F) * 2) + scale_r[2] * ((d[i] - 0.5F) * 2);
				dg = scale_g[1] * (1 - (d[i] - 0.5F) * 2) + scale_g[2] * ((d[i] - 0.5F) * 2);
				db = scale_b[1] * (1 - (d[i] - 0.5F) * 2) + scale_b[2] * ((d[i] - 0.5F) * 2);
			} 

			if (d[i] > 0) {
				da = float((1 - d[i] / 2) + (d[i] / 2) * sin(race_state_timmer * d[i] / 4.0F));
			} else {
				da = 1.0F;
			} 

			hud_damage->draw(dr, dg, db, da, float(v->vx + 171), float(v->vx + 400 + i*16), 0, 0, 1);
		} 
	}

} 


void F1SpiritGame::draw_mini_dispersed_hud(PlayerCCar *v)
{
	char tmp[80];

	/* Fixed part: */
	hud_fixed_1->draw(float(v->vx), float(v->vy), 0, 0, 1);
	minihud_fuel->draw(float(v->vx + v->vdx - 40), float(v->vy), 0, 0, 1);
	hud_fixed_4->draw(float(v->vx + v->vdx - 136), float(v->vy + v->vdy - 48), 0, 0, 1);

	/* Variable Part: */
	/* POSITION: */
	{
		int i, j, x = 16; /* y = 27 */
		sprintf(tmp, "%.2i/%.2i", race_positions.PositionRef((RacingCCar *)v) + 1, race_positions.Length());

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 27, 0, 0, 1);

			x += 14;
		} 
	}

	/* LAP: */
	{
		int i, j, x = 112; /* y = 27 */
		sprintf(tmp, "%i/%i", v->laps + 1, race_nlaps);

		for (i = 0;i < int(strlen(tmp));i++) {
			if (tmp[i] == '/') {
				j = 10;
			} else if (tmp[i] == ':') {
				j = 11;
			} else {
				j = tmp[i] - '0';

				if (j < 0)
					j = 0;

				if (j > 9)
					j = 9;
			} 

			hud_time_font[j]->draw(v->vx + x, v->vy + 27, 0, 0, 1);

			x += 14;
		} 
	}

	/* SPEED: */
	{
		int i, j;
		int w;
		int x = v->vdx - 136;
		sprintf(tmp, "%i", int(v->car->get_speed()));

		for (i = strlen(tmp) - 1;i >= 0;i--) {
			j = tmp[i] - '0';

			if (j < 0)
				j = 0;

			if (j > 9)
				j = 9;

			w = 20;

			//   if (j==1) w=10;
			hud_speed_font[j]->draw(v->vx + x - w, v->vy + v->vdy - 42, 0, 0, 1);

			x -= w;
		} 
	}

	/* GEAR: */
	hud_gear[v->car->get_currentgear()]->draw(float(v->vx + v->vdx - 23), float(v->vy + v->vdy - 30), 0, 0, 1);

	/* RPM: */
	{
		float f = float(v->car->get_relative_rpm() * v->car->get_relative_rpm());
		int n = int(f * 12);

		if (n < 0)
			n = 0;

		if (n > 11)
			n = 11;

		if (n > 0) {
			hud_rpm[n - 1]->draw(float(v->vx + v->vdx - 128), float(v->vy + v->vdy - 38), 0, 0, 1);
		} 
	}


	/* fuel: */
	{
		float f = v->car->get_fuel();
		int n = int(((f * 8) + (parameters.MAX_FUEL - 1)) / parameters.MAX_FUEL);
		int i;

		for (i = 0;i < n;i++) {
			race_fuel->draw(float(v->vx + v->vdx - 28), float(v->vy + 20 + (7 - i)*8), 0, 0, 1);
		} 
	}

	/* damage: */
	{
		float f = 0.0F;
		int x;

		switch (v->show_damage_cycle) {

			case 0:
				f = 0.0F;
				break;

			case 1:
				f = v->show_damage_timmer * 0.02F;
				break;

			case 2:
				f = 1.0F;
				break;

			case 3:
				f = (50 - v->show_damage_timmer) * 0.02F;
				break;
		} 

		x = int( -220 + 96 * f);

		hud_fixed_3_sidefuel->draw(float(v->vx + x), float(v->vy + v->vdy - hud_fixed_3_sidefuel->get_dy()), 0, 0, 1);

		{
			float scale_r[3] = {0, 1, 1};
			float scale_g[3] = {1, 1, 0};
			float scale_b[3] = {0, 0, 0};
			float d[4] = {0, 0, 0, 0}, dr = 0, dg = 1, db = 0, da = 1;
			int i;

			d[0] = v->car->get_ftyre_damage();
			d[1] = v->car->get_brake_damage();
			d[2] = v->car->get_engine_damage();
			d[3] = v->car->get_rtyre_damage();

			for (i = 0;i < 4;i++) {
				if (d[i] < 0.5F) {
					dr = scale_r[0] * (1 - d[i] * 2) + scale_r[1] * (d[i] * 2);
					dg = scale_g[0] * (1 - d[i] * 2) + scale_g[1] * (d[i] * 2);
					db = scale_b[0] * (1 - d[i] * 2) + scale_b[1] * (d[i] * 2);
				} else {
					dr = scale_r[1] * (1 - (d[i] - 0.5F) * 2) + scale_r[2] * ((d[i] - 0.5F) * 2);
					dg = scale_g[1] * (1 - (d[i] - 0.5F) * 2) + scale_g[2] * ((d[i] - 0.5F) * 2);
					db = scale_b[1] * (1 - (d[i] - 0.5F) * 2) + scale_b[2] * ((d[i] - 0.5F) * 2);
				} 

				if (d[i] > 0) {
					da = float((1 - d[i] / 2) + (d[i] / 2) * sin(race_state_timmer * d[i] / 4.0F));
				} else {
					da = 1.0F;
				} 

				hud_damage->draw(dr, dg, db, da, float(v->vx + 223 + x), float(v->vy + v->vdy - hud_fixed_3_sidefuel->get_dy() + 40 + 16*i), 0, 0, 1);
			} 
		}
	}

} 




int F1SpiritGame::get_nplayers(void)
{
	return player_cars.Length();
} 


Uint32 F1SpiritGame::get_player_time(int p)
{
	int i;
	Uint32 t;
	PlayerCCar *c;

	c = player_cars[p];

	for (t = 0, i = 0;i < race_nlaps;i++)
		t += c->lap_time[i];

	return t*2;
} 


Uint32 F1SpiritGame::get_player_bestlap(int p)
{
	int i;
	PlayerCCar *c;
	Uint32 time;

	c = player_cars[p];

	time = c->lap_time[0];

	for (i = 1;i < race_nlaps;i++) {
		if (c->lap_time[i] != 0 && (c->lap_time[i]) < time)
			time = c->lap_time[i];
	} 

	return time*2;
} 


int F1SpiritGame::get_player_position(int p)
{
	PlayerCCar *c;

	c = player_cars[p];

	return race_positions.PositionRef(c);
} 


bool F1SpiritGame::get_racefinished(void)
{
	int i;

	for (i = 0;i < player_cars.Length();i++) {
		if (player_cars[i]->lap_time[race_nlaps - 1] != 0)
			return true;
	} 

	return false;
} 


void F1SpiritGame::finish_replay(void)
{
	if (replay_fp != 0) {
		fputc(replay_cycle&0x0ff, replay_fp);
		fputc(replay_cycle >> 8, replay_fp);
		fputc(2, replay_fp);
		fclose(replay_fp);
		replay_fp = 0;
	} 
} 


