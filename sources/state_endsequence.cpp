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
#include "Vector.h"
#include "sound.h"
#include "List.h"
#include "auxiliar.h"
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

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern int N_SFX_CHANNELS;
extern bool sound;
extern TRanrotBGenerator *rg;


#define ENDSEQUENCE_CTNT 100
#define ENDSEQUENCE_CTNT2   7


int F1SpiritApp::endsequence_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {

		/*
		#ifdef _WIN32
		        if (sound) {
		            Stop_playback();
		            N_SFX_CHANNELS = Resume_playback(N_SFX_CHANNELS, 0);
		        } // if
		#endif
		*/
		Sound_create_music("sound/game_ending", -1);
		Sound_music_volume(127);

		endsequence_images[0] = new GLTile("graphics/end.png", 0, 0, 256, 128);
		endsequence_images[1] = new GLTile("graphics/end.png", 0, 128, 256, 128);
		endsequence_images[2] = new GLTile("graphics/end.png", 0, 256, 256, 128);
		endsequence_images[3] = new GLTile("graphics/end.png", 0, 384, 256, 128);
		endsequence_images[4] = new GLTile("graphics/end.png", 0, 512, 256, 128);
	} 

	switch (endsequence_state) {

		case 0:  /* FADING IN: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT) {
				endsequence_state = 1;
				endsequence_timmer = 0;
			} 

			break;

		case 1:  /* SHOWING HELMET: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 2;
				endsequence_timmer = 0;
			} 

			break;

		case 2:  /* SHOWING CUP: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 3;
				endsequence_timmer = 0;
			} 

			break;

		case 3:  /* SHOWING ROAD: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 4;
				endsequence_timmer = 0;
			} 

			break;

		case 4:  /* SHOWING GIRLS: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 5;
				endsequence_timmer = 0;
			} 

			break;

		case 5:  /* SHOWING HELMET: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 6;
				endsequence_timmer = 0;
			} 

			break;

		case 6:  /* SHOWING CREDITS: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT2*ENDSEQUENCE_CTNT) {
				endsequence_state = 7;
				endsequence_timmer = 0;
			} 

			break;

		case 7:  /* FADING OUT: */
			endsequence_timmer++;

			if (endsequence_timmer >= ENDSEQUENCE_CTNT)
				endsequence_state = 8;

			Sound_music_volume(int(127.0F*float(ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT));

			break;

		case 8:  /* finish */
			break;
	} 

	if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
		if (endsequence_state != 7) {
			endsequence_state = 7;
			endsequence_timmer = 0;
		} 
	} 

	if (endsequence_state == 8) {
		delete endsequence_images[0];
		delete endsequence_images[1];
		delete endsequence_images[2];
		delete endsequence_images[3];
		delete endsequence_images[4];
		endsequence_images[0] = 0;
		endsequence_images[1] = 0;
		endsequence_images[2] = 0;
		endsequence_images[3] = 0;
		endsequence_images[4] = 0;

		menu_fading = -1;
		menu_fading_ctnt = 25;
		menu_current_menu = 4;

		return endsequence_returnstate;
	} 

	return APP_STATE_ENDSEQUENCE;
} 


void F1SpiritApp::endsequence_draw(void)
{
	glClearColor(0.8F, 0.8F, 0.8F, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (endsequence_state) {

		case 0:  /* FADING IN: */
			glEnable(GL_COLOR_MATERIAL);

			{
				float f = 0;
				f = float(abs(ENDSEQUENCE_CTNT - endsequence_timmer)) / ENDSEQUENCE_CTNT;
				glColor4f(0, 0, 0, f);
			}

			glNormal3f(0.0, 0.0, 1.0);

			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 480, 0);
			glVertex3f(640, 480, 0);
			glVertex3f(640, 0, 0);
			glEnd();
			break;

		case 1: { /* SHOWING HELMET: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;


				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"THE MEMORIES OF", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"THE MEMORIES OF", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 256, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FORMULA 1", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"FORMULA 1", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 272, 336, 0, 0, 1);

				delete t;

				endsequence_images[0]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
			}

			break;

		case 2: { /* SHOWING CUP: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;


				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"THE MEMORIES OF", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"THE MEMORIES OF", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 256, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FORMULA 1", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"FORMULA 1", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 272, 336, 0, 0, 1);

				delete t;

				if (endsequence_timmer > ENDSEQUENCE_CTNT &&
				        endsequence_timmer < (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT &&
				        (rg->IRandom(0, 23)) == 0) {
					endsequence_images[2]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
				} else {
					endsequence_images[1]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
				} 
			}

			break;

		case 3: { /* SHOWING ROAD: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;


				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"THE MEMORIES OF", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"THE MEMORIES OF", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 256, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FORMULA 1", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"FORMULA 1", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 272, 336, 0, 0, 1);

				delete t;

				endsequence_images[3]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
			}

			break;

		case 4: { /* SHOWING GIRLS: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;


				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"THE MEMORIES OF", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"THE MEMORIES OF", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 256, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FORMULA 1", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"FORMULA 1", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 272, 336, 0, 0, 1);

				delete t;

				endsequence_images[4]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
			}

			break;

		case 5: { /* SHOWING HELMET: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;


				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"THE MEMORIES OF", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"THE MEMORIES OF", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 256, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"FORMULA 1", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"FORMULA 1", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(0.5F, 0.5F, 0.5F, f, 272, 336, 0, 0, 1);

				delete t;

				endsequence_images[0]->draw(1, 1, 1, f, 240, 160, 0, 0, 1);
			}

			break;

		case 6: { /* SHOWING CREDITS: */
				float f = 1.0F;

				if (endsequence_timmer < ENDSEQUENCE_CTNT)
					f = float(endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (endsequence_timmer > (ENDSEQUENCE_CTNT2 - 1)*ENDSEQUENCE_CTNT)
					f = float(ENDSEQUENCE_CTNT2 * ENDSEQUENCE_CTNT - endsequence_timmer) / ENDSEQUENCE_CTNT;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;

				GLTile *t;

				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"ORIGINAL KONAMI CREW", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"ORIGINAL KONAMI CREW", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 96, 64, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"CODE: T.ADACHI, M.OZAWA, R.SAGISAKA", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"CODE: T.ADACHI, M.OZAWA, R.SAGISAKA", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 96, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"GRAPHICS: H.MAKITANI, T.SHIMIZU,", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"GRAPHICS: H.MAKITANI, T.SHIMIZU,", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 128, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"          B.KINOSHITA", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"          B.KINOSHITA", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 144, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"SOUND: K.UEHARA, M.SASAKI, M.FURUKAWA", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"SOUND: K.UEHARA, M.SASAKI, M.FURUKAWA", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 176, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"BRAIN GAMES CREW", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"BRAIN GAMES CREW", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 96, 240, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"CODE: POPOLON", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"CODE: POPOLON", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 272, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"GRAPHICS: RAMONMSX, MP83", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"GRAPHICS: RAMONMSX, MP83", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 304, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"          VALERIAN, PICILI", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"          VALERIAN, PICILI", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 320, 0, 0, 1);

				delete t;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"SOUND: JORITO", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)"SOUND: JORITO", font, sfc, 0, 0, 0);

				t = new GLTile(sfc);

				t->draw(1, 1, 1, f, 16, 352, 0, 0, 1);

				delete t;
			}

			break;

		case 7:  /* FADING OUT: */
			glEnable(GL_COLOR_MATERIAL);

			{
				float f = 0;
				f = float(abs(endsequence_timmer)) / 100.0F;
				glColor4f(0, 0, 0, f);
			}

			glNormal3f(0.0, 0.0, 1.0);

			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 480, 0);
			glVertex3f(640, 480, 0);
			glVertex3f(640, 0, 0);
			glEnd();
			break;

		case 8:  /* finish */
			break;
	} 

	if (title_state != 0) {} 

} 

