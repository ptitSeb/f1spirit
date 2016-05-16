#ifdef _WIN32
#include "windows.h"
#include "glut.h"
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
#include "F1SpiritTrackViewer.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"
#include "randomc.h"
#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif



extern TRanrotBGenerator *rg;


void F1SpiritTrackViewer::initialize_track(int ntrack)
{
	race_minitrack = 0;

	road_tile[0] = 0;
	road_tile[1] = 0;
	road_tile[2] = 0;
	road_ltile[0] = 0;
	road_ltile[1] = 0;
	road_rtile[0] = 0;
	road_rtile[1] = 0;
	road_lines = 0;

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

		tiles.Add(new GLTile(sfc, 0, 64, 48, 48));

		tiles.Add(new GLTile(sfc, 0, 112, 48, 48));

		tiles.Add(new GLTile(sfc, 0, 160, 64, 48));

		SDL_FreeSurface(sfc);

		sfc = IMG_Load("graphics/stock/arrows.png");

		tiles.Add(new GLTile(sfc, 0, 0, 32, 32));

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

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Generating Minimap...\n");

#endif
	{
		SDL_Surface *sfc;

		delete race_minitrack;
		race_minitrack = 0;
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Previous minimap freed\n");
#endif

		sfc = track->draw_minimap(192, 106, &race_minimap_zoom);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Minimap drawn into a surface\n");
#endif

		race_minitrack = new GLTile(sfc);
	}

	{
		SDL_Surface *race_minimap_enemycar_sfc = IMG_Load("graphics/minimap-enemycar.png");
		SDL_Rect r2;
		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, race_minimap_enemycar_sfc->w, race_minimap_enemycar_sfc->h, 32, RMASK, GMASK, BMASK, AMASK);
		r2.x = 0;
		r2.y = 0;
		r2.w = race_minimap_enemycar_sfc->w;
		r2.h = race_minimap_enemycar_sfc->h;
		SDL_SetAlpha(race_minimap_enemycar_sfc, 0, 0);
		SDL_BlitSurface(race_minimap_enemycar_sfc, &r2, sfc, &r2);

		mini_tile = new GLTile(sfc);
		mini_tile->set_hotspot(sfc->w / 2, sfc->h / 2);
		SDL_FreeSurface(race_minimap_enemycar_sfc);
	}


#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Minimap Generated\n");

#endif

	track->get_road()->Rewind();


} 


F1SpiritTrackViewer::F1SpiritTrackViewer(int ntrack)
{
	rg->RandomInit(0);

	initialize_track(ntrack);

	{
		float start_x, start_y, start_a;
		track->get_start_position(10, &start_x, &start_y, &start_a);
		c_z = 0.8F;
		c_x = start_x;
		c_y = start_y;
	}

	vdx = 640;
	vdy = 480;
	c_a = 0;
	road_position.Instance(*track->get_road());
	road_position.Rewind();
	nlaps = 0;
	speed = 0;

	rg->RandomInit(0);

} 



F1SpiritTrackViewer::~F1SpiritTrackViewer()
{
	delete track;
	track = 0;

	delete race_minitrack;
	race_minitrack = 0;

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

	delete mini_tile;
	mini_tile = 0;

} 



bool F1SpiritTrackViewer::cycle(void)
{

	track->cycle();

	{
		LLink<CRoadPiece> *old_pos, *pos;
		CRoadPiece *rp;
		float t1, t2, t3, t4, t5;
		float d, min_d;
		int i;

		old_pos = pos = road_position.GetPos();
		rp = road_position.GetObj();
		rp->get_path_position(c_x, c_y, &t1, &t2, &t3, &t4, &t5, &min_d);

		for (i = 0;i < 4;i++) {
			if (road_position.LastP()) {
				road_position.Rewind();
			} else {
				road_position.Next();
			} 

			rp = road_position.GetObj();

			rp->get_path_position(c_x, c_y, &t1, &t2, &t3, &t4, &t5, &d);

			if (d < min_d) {
				min_d = d;
				pos = road_position.GetPos();
			} 
		} 

		if (min_d < 160) {
			int p1 = 0, p2 = 0;
			road_position.SetPos(old_pos);
			p1 = road_position.PositionRef(road_position.GetObj());
			road_position.SetPos(pos);
			p2 = road_position.PositionRef(road_position.GetObj());
			/* Passed by the start line: */

			if (p2 < p1) {
				nlaps++;
			} 
		} else {
			road_position.SetPos(old_pos);
		} 

		rp = road_position.GetObj();

		rp->get_path_position(c_x, c_y, &t1, &t2, &t3, &t4, &t5, &min_d);

		c_x = t1;

		c_y = t2;

		c_x += speed * cos((t4 * M_PI) / 180.0F);

		c_y += speed * sin((t4 * M_PI) / 180.0F);

		if (speed < 16)
			speed += 0.5F;

	}

	return true;
} 


void F1SpiritTrackViewer::draw(void)
{
	GLfloat val[4];
	float fx = 1.0F, fy = 1.0F;

	glGetFloatv(GL_VIEWPORT, val);
	fx = val[2] / 640.0F;
	fy = val[3] / 480.0F;

	// glEnable( GL_SCISSOR_TEST );
	// glScissor(int(val[0]),int(val[1]),int(vdx*fx),int(vdy*fy));
	/*
	 {
	  float mat[]={0.3F,0.4F,0.3F,0,
	      0.3F,0.4F,0.3F,0,
	      0.3F,0.4F,0.3F,0,
	      0,0,0,1};
	  glMatrixMode(GL_COLOR);
	  glPushMatrix();
	  glLoadMatrixf(mat);
	  glMatrixMode(GL_MODELVIEW);
	 }
	*/
	/* Track: */
	{
		SDL_Rect vp;
		List<CPlacedGLTile> extras;

		glPushMatrix();

		// camera type = 2
		glTranslatef(float(vdx / 2), float(3*(vdy / 4)), 0);

		vp.y = int( -vdy / 2);
		vp.h = int(vdy);
		vp.x = int( -vdx / 2);
		vp.w = int(vdx);

		glRotatef(c_a, 0, 0, 1);

		track->draw(&vp, c_x, c_y, 1.0F, 0, road_tile, road_ltile, road_rtile, road_lines);

		glPopMatrix();

	}

	// glDisable( GL_SCISSOR_TEST );

	/* Draw the minimap: */
	{
		int mini_x = 320 - (214 / 2), mini_y = 8;

		race_minitrack->draw(1, 1, 1, 0.5F, float(mini_x), float(mini_y), 0, 0, 1);

		mini_tile->draw(1, 1, 1, 1.0F, mini_x + c_x*race_minimap_zoom, mini_y + c_y*race_minimap_zoom, 0, 0, 4);

	}

	/*
	 {
	  glMatrixMode(GL_COLOR);
	  glPopMatrix();
	  glMatrixMode(GL_MODELVIEW);
	 }
	*/

} 



int F1SpiritTrackViewer::get_nlaps(void)
{
	return nlaps;
} 

