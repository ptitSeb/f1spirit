#ifdef _WIN32
#include "windows.h"
#else
#include "dirent.h"
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
#include "sound.h"
#include "List.h"
#include "Vector.h"
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
#include "F1Spirit-auxiliar.h"

#include "filehandling.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

/* External variables: */
extern int N_SFX_CHANNELS;
extern bool sound;
extern int SCREEN_X;
extern int SCREEN_Y;

int F1SpiritApp::replaymanager_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {
		CReplayInfo *ri;
		replaymanager_state = 0;
		replaymanager_timmer = 25;
		replaymanager_first = 0;
		if (replaymanager_game!=0) delete replaymanager_game;
		replaymanager_game = 0;
		replaymanager_replay_playing = false;
		replaymanager_replay_viewport = 0;
		replaymanager_replay_desiredviewport = 0;

		replaymanager_replays.Delete();
		replaymanager_replaytiles.Delete();

		/* find replay files: */
#ifdef _WIN32
		/* Find files: */
		WIN32_FIND_DATA finfo;
		HANDLE h;

		h = FindFirstFile("replays/*.rpl", &finfo);

		if (h != INVALID_HANDLE_VALUE) {
			ri = new CReplayInfo(finfo.cFileName, "replays");
			replaymanager_replays.Add(ri);

			while (FindNextFile(h, &finfo) == TRUE) {
				ri = new CReplayInfo(finfo.cFileName, "replays");
				replaymanager_replays.Add(ri);
			} 
		} 

#else
		DIR *dp;

		struct dirent *ep;

		char dir[STRLEN];

		snprintf(dir, STRLEN - 1, "%s/.%s/replays", getenv("HOME"), GAMENAME);

		dp = opendir(dir);

		if (dp != NULL) {
			while (ep = readdir (dp)) {
				if (strlen(ep->d_name) > 4 &&
				        ep->d_name[strlen(ep->d_name) - 4] == '.' &&
				        ep->d_name[strlen(ep->d_name) - 3] == 'r' &&
				        ep->d_name[strlen(ep->d_name) - 2] == 'p' &&
				        ep->d_name[strlen(ep->d_name) - 1] == 'l') {

					ri = new CReplayInfo(ep->d_name, "replays");
					replaymanager_replays.Add(ri);
				} 

			} 

			(void) closedir (dp);
		} 

#endif

	} 

	switch (replaymanager_state) {

		case 0:
			replaymanager_timmer--;

			if (replaymanager_timmer <= 0) {
				replaymanager_state = 1;
				replaymanager_timmer = 0;
			} 

			break;

		case 1: {
				bool delete_replay = false;
				replaymanager_timmer++;

				if (replaymanager_replays.Length() != 0) {
					if (k->keyboard[SDLK_DOWN] && !k->old_keyboard[SDLK_DOWN]) {
						Sound_play(S_menu_move, 128);
						replaymanager_timmer = 10;
						replaymanager_replays.Next();

						if (replaymanager_replays.EndP())
							replaymanager_replays.Rewind();

						delete_replay = true;
					} 

					if (k->keyboard[SDLK_UP] && !k->old_keyboard[SDLK_UP]) {
						Sound_play(S_menu_move, 128);
						replaymanager_timmer = 10;

						if (replaymanager_replays.BeginP()) {
							replaymanager_replays.Forward();
						} else {
							replaymanager_replays.Prev();
						} 

						delete_replay = true;
					} 

					if (k->keyboard[SDLK_PAGEDOWN] && !k->old_keyboard[SDLK_PAGEDOWN]) {
						int i;
						Sound_play(S_menu_move, 128);
						replaymanager_timmer = 10;

						for (i = 0;i < 25;i++) {
							if (!replaymanager_replays.LastP())
								replaymanager_replays.Next();
						} 

						delete_replay = true;
					} 

					if (k->keyboard[SDLK_PAGEUP] && !k->old_keyboard[SDLK_PAGEUP]) {
						int i;
						Sound_play(S_menu_move, 128);
						replaymanager_timmer = 10;

						for (i = 0;i < 25;i++) {
							if (!replaymanager_replays.BeginP())
								replaymanager_replays.Prev();
						} 

						delete_replay = true;
					} 

					if (k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE]) {
						if (replaymanager_replay_playing) {
							if (replaymanager_game!=0) replaymanager_game->pauseSFX();
							replaymanager_replay_playing = false;
						} else {
							if (replaymanager_game!=0) replaymanager_game->resumeSFX();
							replaymanager_replay_playing = true;
						} // if 

						if (replaymanager_game == 0) {
							int i;
							/*
							#ifdef _WIN32

							                            if (sound) {
							                                Stop_playback();
							                                N_SFX_CHANNELS = Resume_playback(N_SFX_CHANNELS, 0);
							                            } // if
							#endif
							*/
							Sound_release_music();
							replaymanager_game = replaymanager_replays.GetObj()->create_game(font, k);
							replaymanager_replays.GetObj()->events.Rewind();

							for (i = 0;i < replaymanager_replay_keyboard.k_size;i++) {
								replaymanager_replay_keyboard.keyboard[i] = 0;
								replaymanager_replay_keyboard.old_keyboard[i] = 0;
							} 

							replaymanager_replay_playing = true;
						} /* of */


						Sound_play(S_menu_select, 128);
					} 

					if (k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN]) {
						if (replaymanager_replay_desiredviewport == 0) {
							replaymanager_replay_desiredviewport = 25;
						} else {
							replaymanager_replay_desiredviewport = 0;
						} 
					} 


				} 

				if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
					replaymanager_state = 2;
					replaymanager_timmer = 0;
				} 

				if (replaymanager_replay_playing && replaymanager_game != 0) {
					/* update replay keyboard: */

					if (!replaymanager_replays.GetObj()->replay_cycle(replaymanager_game->replay_cycle, &replaymanager_replay_keyboard, replaymanager_game)) {
						delete_replay = true;
					} 

					if (!delete_replay &&
					        !replaymanager_game->cycle(&replaymanager_replay_keyboard)) {
						replaymanager_replay_playing = false;
						delete_replay = true;
					} 
				} 

				if (delete_replay) {
					if (replaymanager_game != 0) {
						/*
						#ifdef _WIN32
						                        if (sound) {
						                            Stop_playback();
						                            N_SFX_CHANNELS = Resume_playback(N_SFX_CHANNELS, 0);
						                        } // if
						#endif
						*/
						Sound_release_music();
						delete replaymanager_game;
						replaymanager_game = 0;
						replaymanager_replay_desiredviewport = 0;
						Sound_create_music("sound/game_menu", -1);
					} 
				} 

				if (replaymanager_replay_desiredviewport > replaymanager_replay_viewport) {
					replaymanager_replay_viewport++;
				} 

				if (replaymanager_replay_desiredviewport < replaymanager_replay_viewport) {
					replaymanager_replay_viewport--;
				} 

			}

			break;

		case 2:
			replaymanager_timmer++;

			if (replaymanager_timmer >= 25) {
				menu_fading = -1;
				menu_fading_ctnt = 25;
				menu_current_menu = 4;
				if (replaymanager_game!=0) delete replaymanager_game;
				replaymanager_game = 0;

				return APP_STATE_MENU;
			} 

			break;
	} 

	return APP_STATE_REPLAYMANAGER;
} 


void F1SpiritApp::replaymanager_draw(void)
{
	glClearColor(0.2F, 0.2F, 0.2F, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (replaymanager_replay_viewport < 25) {
		int i, j;
		int dx = 16, dy = 16;
		float x1, y1, x2, y2;
		Vector prev_v1, prev_v2, v;
		float prev_z1, prev_z2, z;
		bool prev;
		glEnable(GL_COLOR_MATERIAL);
		//  glColor3f(0.8F,0.8F,0.8F);
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

				/*
				    if (((((j+6)/4)+((i+6)/4))%2)==0) {
				     glBegin(GL_QUADS);

				     if (prev) {
				      glNormal3f(float(prev_v2.x),float(prev_v2.y),float(prev_v2.z));
				      glVertex3f(float(j*dx-prev_z2/4),float(i*dy),prev_z2);

				      glNormal3f(float(prev_v1.x),float(prev_v1.y),float(prev_v1.z));
				      glVertex3f(float(j*dx-prev_z1/4),float(i*dy+dy),prev_z1);
				     } else {
				      v.x=v.y=cos((x1+y1)*0.01)*2;
				      v.z=1;
				      v.normalize();
				      glNormal3f(float(v.x),float(v.y),float(v.z));
				      z=float(30+30*sin((x1+y1)*0.02));
				      glVertex3f(float(j*dx-z/4),float(i*dy),z);

				      v.x=v.y=cos((x1+y2)*0.01)*2;
				      v.z=1;
				      v.normalize();
				      glNormal3f(float(v.x),float(v.y),float(v.z));
				      z=float(30+30*sin((x1+y2)*0.02));
				      glVertex3f(float(j*dx-z/4),float(i*dy+dy),z);
				     } // if

				     prev_v1.x=prev_v1.y=cos((x2+y2)*0.01)*2;
				     prev_v1.z=1;
				     prev_v1.normalize();
				     glNormal3f(float(prev_v1.x),float(prev_v1.y),float(prev_v1.z));
				     prev_z1=float(30+30*sin((x2+y2)*0.02));
				     glVertex3f(float(j*dx+dx-prev_z1/4),float(i*dy+dy),prev_z1);

				     prev_v2.x=prev_v2.y=cos((x2+y1)*0.01)*2;
				     prev_v2.z=1;
				     prev_v2.normalize();
				     glNormal3f(float(prev_v2.x),float(prev_v2.y),float(prev_v2.z));
				     prev_z2=float(30+30*sin((x2+y1)*0.02));
				     glVertex3f(float(j*dx+dx-prev_z2/4),float(i*dy),prev_z2);
				     glEnd();
				     prev=true;
				    } else {
				     prev=false;
				    } // if
				*/
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
	} 

	/* Draw the replay-names list: */
	if (replaymanager_replay_viewport < 25) {
		if (replaymanager_frame1 == 0) {
			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, 376, 432, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			draw_menu_frame(sfc, 0, 0, 376, 432);
			replaymanager_frame1 = new GLTile(sfc);
		} 

		if (replaymanager_frame2 == 0) {
			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, 240, 432, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			draw_menu_frame(sfc, 0, 0, 240, 432);
			replaymanager_frame2 = new GLTile(sfc);
		} 

		if (replaymanager_replays.Length() != replaymanager_replaytiles.Length()) {
			char tmp[256];
			SDL_Surface *sfc;
			List<CReplayInfo> l;
			CReplayInfo *ri;

			l.Instance(replaymanager_replays);
			l.Rewind();

			while (l.Iterate(ri)) {
				sprintf(tmp, "%s", ri->filename);
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				replaymanager_replaytiles.Add(new GLTile(sfc));
			} 

		} 

		replaymanager_frame1->draw(8, 16, 0, 0, 1);

		replaymanager_frame2->draw(392, 16, 0, 0, 1);

		{
			char tmp[80];
			SDL_Surface *sfc;
			GLTile *t;
			sprintf(tmp, "SPACE-PLAY/PAUSE    ENTER-SMALL/LARGE");
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->draw(float(320 - t->get_dx() / 2), 456, 0, 0, 1);
			delete t;
		}

		/* file list: */
		{
			List<CReplayInfo> l;
			CReplayInfo *ri;
			List<GLTile> l2;
			GLTile *t;
			int y = 32 - replaymanager_first * 16;

			glEnable( GL_SCISSOR_TEST );
			glScissor(24, 48, 328, 400);

			l.Instance(replaymanager_replays);
			l.Rewind();
			l2.Instance(replaymanager_replaytiles);
			l2.Rewind();

			while (l.Iterate(ri) &&
			        l2.Iterate(t)) {
				if (y >= 32 && y <= 416) {
					if (!replaymanager_replays.EmptyP() &&
					        replaymanager_replays.GetObj() == ri) {
						t->draw(1, 1, 1, float(0.6F + 0.4*sin(replaymanager_timmer / 4.0F)), 24, float(y), 0, 0, 1);

						if (y < 32)
							replaymanager_first--;

						if (y > 416)
							replaymanager_first++;
					} else {
						t->draw(24, float(y), 0, 0, 1);
					} 
				} 

				y += 16;
			} 

			glDisable( GL_SCISSOR_TEST );

		}

		/* Current replay info: */
		if (!replaymanager_replays.EmptyP() &&
		        replaymanager_replays.GetObj() != 0) {
			GLTile *t;
			SDL_Surface *sfc;
			CReplayInfo *ri;
			char tmp[128];
			int i;
			char *months[] = {"gen", "feb", "mar", "apr", "may", "jun",
			                  "jul", "aug", "sep", "oct", "nov", "dec"
			                 };
			int y = 32 + 160 + 16;

			ri = replaymanager_replays.GetObj();

			/* Version: */
			sprintf(tmp, "Version: %s", ri->version_tag);
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->draw(392 + 16, float(y), 0, 0, 1);
			delete t;
			y += 12;

			/* Date: */
			sprintf(tmp, "Recorded:");
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->draw(392 + 16, float(y), 0, 0, 1);
			delete t;
			y += 12;

			sprintf(tmp, " %i - %s - %i", ri->day, months[ri->month], ri->year + 1900);
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->draw(392 + 16, float(y), 0, 0, 1);
			delete t;
			y += 12;

			sprintf(tmp, " %i:%.2i:%.2i", ri->hour, ri->min, ri->sec);
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->draw(392 + 16, float(y), 0, 0, 1);
			delete t;
			y += 12;

			/* track: */

			if (ri->track_name != 0) {
				sprintf(tmp, "Track: %s", ri->track_name);
			} else {
				sprintf(tmp, "Track: ???");
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->draw(392 + 16, float(y), 0, 0, 1);

			delete t;

			y += 12;

			/* player: */
			if (ri->player != 0) {
				sprintf(tmp, "Player: %s", ri->player->get_name());
			} else {
				sprintf(tmp, "Player: ???");
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->draw(392 + 16, float(y), 0, 0, 1);

			delete t;

			y += 12;

			/* lenght: */
			if (ri->length != 0) {
				int time = ri->length;
				int h, m, s, c;
				c = time % 100;
				time /= 100;
				s = time % 60;
				time /= 60;
				m = time % 60;
				time /= 60;
				h = time;
				sprintf((char *)tmp, "Length: %i:%.2i:%.2i'%.2i\"", h, m, s, c);
			} else {
				sprintf((char *)tmp, "Length: -:--:--'--\"");
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->draw(392 + 16, float(y), 0, 0, 1);

			delete t;

			y += 12;

			/* nº of players: */
			sprintf(tmp, "Number of Players: %i", ri->n_player_cars);

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->draw(392 + 16, float(y), 0, 0, 1);

			delete t;

			y += 12;

			/* player time: */
			if (ri->times != 0) {
				for (i = 0;i < ri->n_player_cars;i++) {
					if (ri->times[i] != 0) {
						int time = ri->times[i];
						int h, m, s, c;
						c = time % 100;
						time /= 100;
						s = time % 60;
						time /= 60;
						m = time % 60;
						time /= 60;
						h = time;
						sprintf((char *)tmp, "- Player %i: %i:%.2i:%.2i'%.2i\"", i + 1, h, m, s, c);
					} else {
						sprintf((char *)tmp, "- Player %i: -:--:--'--\"", i + 1);
					} 

					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);

					print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

					t = new GLTile(sfc);

					t->draw(392 + 16, float(y), 0, 0, 1);

					delete t;

					y += 12;
				} 
			} 

		} 
	} 

	if (replaymanager_replay_viewport > 0 && replaymanager_replay_viewport < 25) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = abs(replaymanager_replay_viewport) / 25.0F;
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

	/* Replay mini-view: */
	if (replaymanager_game != 0) {
		float f;
		float dx, dy;
		float x, y;
		f = replaymanager_replay_viewport / 25.0F;

		dx = 200 + (640 - 200) * f;
		dy = 150 + (480 - 150) * f;
		x = 412 + (0 - 412) * f;
		y = 292 + (0 - 292) * f;

		glViewport(int(x), int(y), int(dx), int(dy));
		replaymanager_game->draw(true);
		glViewport(0, 0, SCREEN_X, SCREEN_Y);
	} 

	if (replaymanager_timmer >= 0 && replaymanager_state != 1) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = abs(replaymanager_timmer) / 25.0F;
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

