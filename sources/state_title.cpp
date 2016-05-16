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


#define TITLE_COLOR1 0.7F,0.14F,0.125F
#define TITLE_COLOR2 0.125F,0.14F,1
#define TITLE_COLOR3 0.7F,0.7F,0.7F

#define TITLE_YOFFS  64

#define TITLE_TIMMER1 50.0F
#define TITLE_TIMMER2 50.0F
#define TITLE_TIMMER3 75.0F
#define TITLE_TIMMER4 (TITLE_TIMMER1*3+75+TITLE_TIMMER3)
#define TITLE_TIMMER5 50.0F
#define TITLE_TIMMER6 (TITLE_TIMMER4+700.0F)
#define TITLE_TIMMER7 (TITLE_TIMMER6+2000.0F)

extern TRanrotBGenerator *rg;

int F1SpiritApp::title_cycle(KEYBOARDSTATE *k)
{
	m_skip_intro_screens = false;

	if (state_cycle == 0) {
		title_state = 0;

		if (title_game != 0)
			delete title_game;

		if (title_replay != 0) {
			delete title_replay;
			title_replay = 0;
		}

		if (title_replay==0) {
			int i;
			char *demos[] = {"demo1.rpl", "demo2.rpl", "demo3.rpl", "demo4.rpl", "demo5.rpl", "demo6.rpl"};
			title_replay = new CReplayInfo(demos[rg->IRandom(0, 5)], "demos");
			title_game = title_replay->create_game(font, k);
			title_game->set_without_music();
			for (i = 0;i < title_replay_keyboard.k_size;i++) {
				title_replay_keyboard.keyboard[i] = 0;
				title_replay_keyboard.old_keyboard[i] = 0;
			} 

		}


		Sound_create_music("sound/game_title", 0);

		Sound_music_volume(127);
	} 


	if (state_cycle < TITLE_TIMMER4) {
		if (k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE])
			state_cycle = int(TITLE_TIMMER4);

		if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE])
			state_cycle = int(TITLE_TIMMER4);
	} 

	if (state_cycle > TITLE_TIMMER4) {
		if (title_state == 0 &&
		        k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE]) {
			title_state = 1;
			Sound_release_music();
			Sound_create_music("sound/game_start", 0);
		} 

		if (title_state == 0 &&
		        k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
			title_state = -1;
		} 

		if (title_state > 0) {
			title_state++;
			//   Sound_music_volume(127-title_state*5);
		} else {
			if (title_state < 0) {
				title_state--;
				Sound_music_volume(127 + title_state*5);
			} else {
				Sound_music_volume(127);
			} 
		} // if

		if (title_state >= TITLE_TIMMER5) {
			menu_fading = -1;
			menu_fading_ctnt = 25;
			menu_current_menu = 0;

			if (current_player != 0 && current_player != friendly_player)
				delete current_player;

			current_player = 0;

			//   Sound_release_music();
			if (title_replay!=0) delete title_replay;
			if (title_game!=0) delete title_game;
			title_replay = 0;
			title_game = 0;

			return APP_STATE_GAMESTART;
		} 

		if (title_state <= -TITLE_TIMMER5) {
			Sound_release_music();
			if (title_replay!=0) delete title_replay;
			if (title_game!=0) delete title_game;
			title_replay = 0;
			title_game = 0;
			return APP_STATE_END;
		} 
	} 

	if (state_cycle >= TITLE_TIMMER6 &&
	        title_replay != 0 && title_game != 0) {
		if (!title_replay->replay_cycle(title_game->replay_cycle, &title_replay_keyboard, title_game)) {
			if (title_replay!=0) delete title_replay;
			if (title_game!=0) delete title_game;
			title_replay = 0;
			title_game = 0;
		} 

//		if ((state_cycle%4)==0) 
			if (title_game != 0 &&
				!title_game->cycle(&title_replay_keyboard)) {
				if (title_replay!=0) delete title_replay;
				if (title_game!=0) delete title_game;
				title_replay = 0;
				title_game = 0;
			} 
	} 

	if (state_cycle >= TITLE_TIMMER7) {
		if (title_replay!=0) delete title_replay;
		if (title_game!=0) delete title_game;
		title_replay = 0;
		title_game = 0;
		return APP_STATE_KONAMI;
	} 

	return APP_STATE_TITLE;
} 


void F1SpiritApp::title_draw(void)
{
	int s;

	if (state_cycle < TITLE_TIMMER4 + TITLE_TIMMER2) {
		if (state_cycle < TITLE_TIMMER2) {
			glClearColor(0.8F*state_cycle / TITLE_TIMMER2, 0.8F*state_cycle / TITLE_TIMMER2, 0.8F*state_cycle / TITLE_TIMMER2, 1);
		} else {
			glClearColor(0.8F, 0.8F, 0.8F, 1);
		} 

		glClear(GL_COLOR_BUFFER_BIT);
	} else {

		/* draw the flag: */
		if (state_cycle < TITLE_TIMMER6) {
			int i, j;
			int dx = 16, dy = 16;
			float x1, y1, x2, y2;
			Vector prev_v1, prev_v2, v;
			float prev_z1, prev_z2, z;
			bool prev;
			glClearColor(0.2F, 0.2F, 0.2F, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			glEnable(GL_COLOR_MATERIAL);
			//   glColor3f(0.8F,0.8F,0.8F);
			glColor3f(1.0F, 1.0F, 1.0F);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, menu_flag->get_texture(0));

			for (i = -6;i < 480 / dy + 12;i++) {
				x1 = float((state_cycle) * 2 + ( -6) * dx);
				y1 = float((state_cycle) * 2 + i * dy);
				x2 = float((state_cycle) * 2 + ( -6) * dx + dx);
				y2 = float((state_cycle) * 2 + i * dy + dy);

				prev = false;

				for (j = -6;j < 640 / dx + 12;j++) {
					x1 += dx;
					x2 += dx;

					glBegin(GL_QUADS);

					if (prev) {
						glTexCoord2f(0.25F + (j % 8)*0.0625F, 0.25F + (i % 8)*0.0625F);
						glNormal3f(float(prev_v2.x), float(prev_v2.y), float(prev_v2.z));
						glVertex3f(float(j*dx - prev_z2 / 4), float(i*dy), prev_z2);

						glTexCoord2f(0.25F + (j % 8)*0.0625F, 0.25F + ((i % 8) + 1)*0.0625F);
						glNormal3f(float(prev_v1.x), float(prev_v1.y), float(prev_v1.z));
						glVertex3f(float(j*dx - prev_z1 / 4), float(i*dy + dy), prev_z1);
					} else {
						v.x = v.y = cos((x1 + y1) * 0.01) * 2;
						v.z = 1;
						v.normalize();
						glTexCoord2f(0.25F + (j % 8)*0.0625F, 0.25F + (i % 8)*0.0625F);
						glNormal3f(float(v.x), float(v.y), float(v.z));
						z = float(30 + 30 * sin((x1 + y1) * 0.02));
						glVertex3f(float(j*dx - z / 4), float(i*dy), z);

						v.x = v.y = cos((x1 + y2) * 0.01) * 2;
						v.z = 1;
						v.normalize();
						glTexCoord2f(0.25F + (j % 8)*0.0625F, 0.25F + ((i % 8) + 1)*0.0625F);
						glNormal3f(float(v.x), float(v.y), float(v.z));
						z = float(30 + 30 * sin((x1 + y2) * 0.02));
						glVertex3f(float(j*dx - z / 4), float(i*dy + dy), z);
					} // if

					prev_v1.x = prev_v1.y = cos((x2 + y2) * 0.01) * 2;

					prev_v1.z = 1;

					prev_v1.normalize();

					glTexCoord2f(0.25F + ((j % 8) + 1)*0.0625F, 0.25F + ((i % 8) + 1)*0.0625F);

					glNormal3f(float(prev_v1.x), float(prev_v1.y), float(prev_v1.z));

					prev_z1 = float(30 + 30 * sin((x2 + y2) * 0.02));

					glVertex3f(float(j*dx + dx - prev_z1 / 4), float(i*dy + dy), prev_z1);

					prev_v2.x = prev_v2.y = cos((x2 + y1) * 0.01) * 2;

					prev_v2.z = 1;

					prev_v2.normalize();

					glTexCoord2f(0.25F + ((j % 8) + 1)*0.0625F, 0.25F + (i % 8)*0.0625F);

					glNormal3f(float(prev_v2.x), float(prev_v2.y), float(prev_v2.z));

					prev_z2 = float(30 + 30 * sin((x2 + y1) * 0.02));

					glVertex3f(float(j*dx + dx - prev_z2 / 4), float(i*dy), prev_z2);

					glEnd();

					prev = true;

				} 
			} 

			if (state_cycle > TITLE_TIMMER6 - 50) {
				glEnable(GL_COLOR_MATERIAL);

				{
					float f = 0;
					f = abs(int(state_cycle - TITLE_TIMMER6) + 50) / 50.0F;
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
		} else {
			/* Draw the demo replay: */
			glClearColor(0.0F, 0.0F, 0.0F, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if (title_game != 0)
				title_game->draw(false);
		} 


	} 

	if (state_cycle < TITLE_TIMMER4 + TITLE_TIMMER2) {
		// F1
		{
			s = state_cycle;

			if (s < TITLE_TIMMER3)
				title_parta->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 153, 79 + TITLE_YOFFS, 1, 0, (s / 35.0F));
		}

		// SPIRIT

		if (state_cycle >= TITLE_TIMMER1) {
			s = state_cycle - int(TITLE_TIMMER1);

			if (s < TITLE_TIMMER3)
				title_partb->draw(TITLE_COLOR2, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 286 + 168, 53 + 29 + TITLE_YOFFS, 1, 0, (s / 35.0F));
		} // if

		// THE WAY TO FORMULA 1
		if (state_cycle >= TITLE_TIMMER1*2) {
			s = state_cycle - int(TITLE_TIMMER1 * 2);

			if (s < TITLE_TIMMER3)
				title_partc->draw(TITLE_COLOR3, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 300 + 170, 108 + 25 + TITLE_YOFFS, 1, 0, (s / 35.0F));
		} // if

		// RAYAS:
		{
			int i;

			for (i = 0;i < 9;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER3)
						title_partd->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, float(237 + 22 + i*43), 18 + 6 + TITLE_YOFFS, 1, 0, (s / 35.0F));
				} // if
			} // for

			for (i = 0;i < 3;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER3)
						title_partd->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, float(156 + 22 + i*43), 129 + 6 + TITLE_YOFFS, 1, 0, (s / 35.0F));
				} // if
			} // for
		}


		/* F1 */
		{
			s = state_cycle;

			if (s < TITLE_TIMMER2)
				title_parta->draw(TITLE_COLOR1, s*(1 / TITLE_TIMMER2), 153, 79 + TITLE_YOFFS, 0, 0, 1);

			if (s >= TITLE_TIMMER2)
				title_parta->draw(TITLE_COLOR1, 1, 153, 79 + TITLE_YOFFS, 0, 0, 1);
		}

		// SPIRIT
		if (state_cycle >= TITLE_TIMMER1) {
			s = state_cycle - int(TITLE_TIMMER1);

			if (s < TITLE_TIMMER2)
				title_partb->draw(TITLE_COLOR2, s*(1 / TITLE_TIMMER2), 286 + 168, 53 + 29 + TITLE_YOFFS, 0, 0, 1);

			if (s >= TITLE_TIMMER2)
				title_partb->draw(TITLE_COLOR2, 1, 286 + 168, 53 + 29 + TITLE_YOFFS, 0, 0, 1);
		} 

		// THE WAY TO FORMULA 1
		if (state_cycle >= TITLE_TIMMER1*2) {
			s = state_cycle - int(TITLE_TIMMER1 * 2);

			if (s < TITLE_TIMMER2)
				title_partc->draw(TITLE_COLOR3, s*(1 / TITLE_TIMMER2), 300 + 170, 108 + 25 + TITLE_YOFFS, 0, 0, 1);

			if (s >= TITLE_TIMMER2)
				title_partc->draw(TITLE_COLOR3, 1, 300 + 170, 108 + 25 + TITLE_YOFFS, 0, 0, 1);
		} 

		// RAYAS:
		{
			int i;

			for (i = 0;i < 9;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER2)
						title_partd->draw(TITLE_COLOR1, s*(1 / TITLE_TIMMER2), float(237 + 22 + i*43), 18 + 6 + TITLE_YOFFS, 0, 0, 1);

					if (s >= TITLE_TIMMER2)
						title_partd->draw(TITLE_COLOR1, 1, float(237 + 22 + i*43), 18 + 6 + TITLE_YOFFS, 0, 0, 1);
				} 
			} 

			for (i = 0;i < 3;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER2)
						title_partd->draw(TITLE_COLOR1, s*(1 / TITLE_TIMMER2), float(156 + 22 + i*43), 129 + 6 + TITLE_YOFFS, 0, 0, 1);

					if (s >= TITLE_TIMMER2)
						title_partd->draw(TITLE_COLOR1, 1, float(156 + 22 + i*43), 129 + 6 + TITLE_YOFFS, 0, 0, 1);
				} 
			} 
		}


		// F1
		{
			s = state_cycle;

			if (s < TITLE_TIMMER3)
				title_parta->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 153, 79 + TITLE_YOFFS, -2, 0, (s / 25.0F));
		}

		// SPIRIT
		if (state_cycle >= TITLE_TIMMER1) {
			s = state_cycle - int(TITLE_TIMMER1);

			if (s < TITLE_TIMMER3)
				title_partb->draw(TITLE_COLOR2, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 286 + 168, 53 + 29 + TITLE_YOFFS, -2, 0, (s / 25.0F));
		} // if

		// THE WAY TO FORMULA 1
		if (state_cycle >= TITLE_TIMMER1*2) {
			s = state_cycle - int(TITLE_TIMMER1 * 2);

			if (s < TITLE_TIMMER3)
				title_partc->draw(TITLE_COLOR3, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, 300 + 170, 108 + 25 + TITLE_YOFFS, -2, 0, (s / 25.0F));
		} // if

		// RAYAS:
		{
			int i;

			for (i = 0;i < 9;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER3)
						title_partd->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, float(237 + 22 + i*43), 18 + 6 + TITLE_YOFFS, -2, 0, (s / 25.0F));
				} // if
			} // for

			for (i = 0;i < 3;i++) {
				if (state_cycle >= TITLE_TIMMER1*3 + i*10) {
					s = state_cycle - int(TITLE_TIMMER1 * 3 + i * 10);

					if (s < TITLE_TIMMER3)
						title_partd->draw(TITLE_COLOR1, (TITLE_TIMMER3 - s) / TITLE_TIMMER3, float(156 + 22 + i*43), 129 + 6 + TITLE_YOFFS, -2, 0, (s / 25.0F));
				} // if
			} // for
		}


	} else {
		title_complete->draw( 0, TITLE_YOFFS, 0, 0, 1);

		{
			if (title_konami == 0) {
				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"KONAMI 1987", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)"KONAMI 1987", font, sfc, 0, 0, 0);
				surface_fader(sfc, 0.9F, 0.9F, 0.9F, 1, 0);

				title_konami = new GLTile(sfc);
				title_konami->set_hotspot(sfc->w / 2, 0);
				//    SDL_FreeSurface(sfc);
			} 

			if (title_braingames == 0) {
				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"BRAIN GAMES 2008", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)"BRAIN GAMES 2008", font, sfc, 0, 0, 0);
				surface_fader(sfc, 0.9F, 0.9F, 0.9F, 1, 0);

				title_braingames = new GLTile(sfc);
				title_braingames->set_hotspot(sfc->w / 2, 0);
				//    SDL_FreeSurface(sfc);
			} 

			if (title_space == 0) {
				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"PRESS SPACE TO PLAY", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)"PRESS SPACE TO PLAY", font, sfc, 0, 0, 0);

				title_space = new GLTile(sfc);
				title_space->set_hotspot(sfc->w / 2, 0);
				//    SDL_FreeSurface(sfc);
			} 

			if (title_start == 0) {
				SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"GAME START", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)"GAME START", font, sfc, 0, 0, 0);

				title_start = new GLTile(sfc);
				title_start->set_hotspot(sfc->w / 2, 0);
				//    SDL_FreeSurface(sfc);
			} 

			title_konami->draw(320, 280, 0, 0, 1);

			title_braingames->draw(320, 296, 0, 0, 1);

			if (title_state == 0) {
				float a = float(0.6F + 0.4 * sin(state_cycle / 4.0F));
				title_space->draw(0.9F, 0.9F, 0.9F, a, 320, 380, 0, 0, 1);
			} else {
				if (title_state > 0) {

					float a = float(0.6F + 0.4 * sin(state_cycle));
					title_start->draw(0.9F, 0.9F, 0.9F, a, 320, 380, 0, 0, 1);

				} 
			} 
		}
	} 

	if (state_cycle > TITLE_TIMMER4 && state_cycle < TITLE_TIMMER4 + TITLE_TIMMER2*5) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;

			if (state_cycle < TITLE_TIMMER4 + TITLE_TIMMER2) {
				f = (state_cycle - TITLE_TIMMER4) / TITLE_TIMMER2;
			} else {
				f = (TITLE_TIMMER4 + TITLE_TIMMER2 * 5 - state_cycle) / (TITLE_TIMMER2 * 4);
			} 

			glColor4f(1, 1, 1, f);
		}

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -3);
		glVertex3f(0, 480, -3);
		glVertex3f(640, 480, -3);
		glVertex3f(640, 0, -3);
		glEnd();
	} 

	if (title_state != 0) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = abs(title_state) / TITLE_TIMMER5;
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

	if (state_cycle > TITLE_TIMMER7 - 50) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = abs(int(state_cycle - TITLE_TIMMER7) + 50) / 50.0F;
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

