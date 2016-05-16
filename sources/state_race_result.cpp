#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"

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

extern int N_SFX_CHANNELS;
extern bool sound;



int F1SpiritApp::race_result_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {
		bool valid_replay_name = true;
		int replay_num = 0;
		FILE *fp;
		char tmp[128];

		if (race_game != 0) race_game->pauseSFX();

		raceresult_trackrecord = false;
		raceresult_laprecord = false;
		raceresult_replaysaved = false;

		raceresult_selected = 0;
		raceresult_state = 0;
		raceresult_timmer = 0;

		if (race_game != 0 && race_game->replay_fp != 0) {
			race_game->finish_replay();

			do {
				if (replay_num == 0) {
					sprintf(raceresult_editing, "%s-%s", current_player->get_name(), race_game->track->get_track_name());
				} else {
					sprintf(raceresult_editing, "%s-%s-%i", current_player->get_name(), race_game->track->get_track_name(), replay_num);
				} 

				replay_num++;

				sprintf(tmp, "replays/%s.rpl", raceresult_editing);

				fp = f1open(tmp, "b", USERDATA);

				if (fp != 0) {
					valid_replay_name = false;
					fclose(fp);
				} else {
					valid_replay_name = true;
				} 

			} while (!valid_replay_name);
		} else {
			raceresult_editing[0] = 0;
		} 


		/* Set the High scores: */
		if (current_player != 0) {
			Uint32 points[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};

			if (race_game->get_racefinished() && current_player != friendly_player &&
			        race_game->get_nplayers() == 1) {
				Uint32 time = race_game->get_player_time(0);
				Uint32 totaltime = 0;

				if (race_game->get_player_position(0) < current_player->get_position(menu_selected_track) ||
				        current_player->get_position(menu_selected_track) == -1) {
					current_player->set_position(menu_selected_track, race_game->get_player_position(0));
				} 

				if (race_game->get_player_position(0) < 9) {
					int p = points[race_game->get_player_position(0)];

					if (current_player->get_points(menu_selected_track) < p)
						current_player->set_points(menu_selected_track, p);
				} 

				if (current_player->get_time(menu_selected_track) > time ||
				        current_player->get_time(menu_selected_track) <= 0 ||
				        current_player->get_bestlap(menu_selected_track) > race_game->get_player_bestlap(0) ||
				        current_player->get_bestlap(menu_selected_track) == 0) {
					/* Completed a track in lower time than the record: */
					char replay_filename[256];
					FILE *fp_in, *fp_out;

					if (current_player->get_time(menu_selected_track) > time ||
					        current_player->get_time(menu_selected_track) <= 0) {
						current_player->set_time(menu_selected_track, time);
						sprintf(replay_filename, "highscores/%s-%s.rpl", current_player->get_name(), race_game->track->get_track_name());

						fp_in = f1open(race_game->replay_filename, "rb", USERDATA);
						fp_out = f1open(replay_filename, "wb", USERDATA);

						while (!feof(fp_in))
							fputc(fgetc(fp_in), fp_out);

						fclose(fp_in);

						fclose(fp_out);
					} 

					if (current_player->get_bestlap(menu_selected_track) > race_game->get_player_bestlap(0) ||
					        current_player->get_bestlap(menu_selected_track) == 0)
						current_player->set_bestlap(menu_selected_track, race_game->get_player_bestlap(0));
				} 

				{
					int i;

					for (i = 0;i < N_TRACKS;i++) {
						if (current_player->get_time(i) > 0) {
							totaltime += current_player->get_time(i);
						} 
					} 
				}

				/* Check if it is an overall record: */
				{
					if (hiscore_time[menu_selected_track] <= 0 ||
					        (Uint32)hiscore_time[menu_selected_track] > current_player->get_time(menu_selected_track))
						raceresult_trackrecord = true;

					if (hiscore_bestlap[menu_selected_track] <= 0 ||
					        (Uint32)hiscore_bestlap[menu_selected_track] > current_player->get_bestlap(menu_selected_track))
						raceresult_laprecord = true;
				}

				add_hiscore_points(current_player->get_name(), current_player->get_points(), totaltime);

				add_hiscore_time(current_player->get_name(), current_player->get_time(menu_selected_track), menu_selected_track);
				add_hiscore_bestlap(current_player->get_name(), current_player->get_bestlap(menu_selected_track), menu_selected_track);

			} 

			save_hiscores();

			{
				FILE *fp;

				fp = f1open(player_filename, "wb", USERDATA);

				if (fp != 0) {
					current_player->save(fp);
					fclose(fp);
				} 
			}

		} 

		Sound_create_music("sound/game_next", -1);

		Sound_music_volume(128);
	} 

	switch (raceresult_state) {

		case 0:
			raceresult_timmer++;

			if (raceresult_timmer >= 50) {
				raceresult_state = 1;
				raceresult_timmer = 0;
			} 

			if ((k->keyboard[SDLK_UP] && !k->old_keyboard[SDLK_UP]) ||
			        (k->keyboard[SDLK_LEFT] && !k->old_keyboard[SDLK_LEFT])) {
				raceresult_selected--;

				if (raceresult_selected < 0)
					raceresult_selected = 1;

				Sound_play(S_menu_move, 128);
			} 

			if ((k->keyboard[SDLK_DOWN] && !k->old_keyboard[SDLK_DOWN]) ||
			        (k->keyboard[SDLK_RIGHT] && !k->old_keyboard[SDLK_RIGHT])) {
				raceresult_selected++;

				if (!raceresult_replaysaved && raceresult_selected > 2)
					raceresult_selected = 0;

				if (raceresult_replaysaved && raceresult_selected > 1)
					raceresult_selected = 0;

				Sound_play(S_menu_move, 128);
			} 

			break;

		case 1:
			raceresult_timmer++;

			if ((k->keyboard[SDLK_UP] && !k->old_keyboard[SDLK_UP]) ||
			        (k->keyboard[SDLK_LEFT] && !k->old_keyboard[SDLK_LEFT])) {
				raceresult_selected--;

				if (raceresult_selected < 0)
					raceresult_selected = 1;

				raceresult_timmer = 0;

				Sound_play(S_menu_move, 128);
			} 

			if ((k->keyboard[SDLK_DOWN] && !k->old_keyboard[SDLK_DOWN]) ||
			        (k->keyboard[SDLK_RIGHT] && !k->old_keyboard[SDLK_RIGHT])) {
				raceresult_selected++;

				if (!raceresult_replaysaved && raceresult_selected > 2)
					raceresult_selected = 0;

				if (raceresult_replaysaved && raceresult_selected > 1)
					raceresult_selected = 0;

				raceresult_timmer = 0;

				Sound_play(S_menu_move, 128);
			} 

			if ((k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE]) ||
			        (k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN])) {
				Sound_play(S_menu_select, 128);
				raceresult_timmer = 0;

				if (raceresult_selected == 0) {
					raceresult_state = 2;
					raceresult_timmer = 0;
					remove (race_game->replay_filename);
				} 

				if (raceresult_selected == 1) {
					raceresult_state = 2;
					raceresult_timmer = 0;
					remove (race_game->replay_filename);
				} 

				if (raceresult_selected == 2) {
					raceresult_state = 3;
					raceresult_timmer = 0;
				} 
			} 

			if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
				Sound_play(S_menu_select, 128);
				raceresult_state = 2;
				raceresult_timmer = 0;
				remove (race_game->replay_filename);
			} 

			break;

		case 2: {
				float f;
				f = float(raceresult_timmer) / 50.0F;

				if (f < 0)
					f = 0;

				if (f > 1)
					f = 1;

				Sound_music_volume(int(128*f));
			}

			raceresult_timmer++;

			if (raceresult_timmer >= 25) {

				menu_fading = -1;
				menu_fading_ctnt = 25;
				menu_current_menu = 4;

				if (raceresult_selected == 0) {
					return APP_STATE_MENU;
				} else {
					return APP_STATE_TRACKLOAD;
				} 
			} 

			break;

		case 3:
			/* disappearing menu 1 (save/continute/race again): */
			raceresult_timmer++;

			if (raceresult_timmer >= 50) {
				raceresult_state = 4;
				raceresult_timmer = 0;
			} 

			break;

		case 4:
			/* appearing menu 2 (replay name/back): */
			raceresult_timmer++;

			if (raceresult_timmer >= 25) {
				raceresult_state = 5;
				raceresult_timmer = 0;
			} 

			break;

		case 5: {
				raceresult_timmer++;

				List<SDL_keysym> l;
				SDL_keysym *ks;

				l.Instance(k->keyevents);
				l.Rewind();

				while (l.Iterate(ks)) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
					output_debug_message("User has pressed a key in the EDIT window: %i\n", ks->unicode);
#endif

					if ((ks->unicode & 0xFF00) == 0) {
						int c;
						int l = strlen((char *)raceresult_editing);

						if (l > 28)
							l = 28;

						c = ks->unicode & 0xFF;

#ifdef F1SPIRIT_DEBUG_MESSAGES

						output_debug_message("The Unicode value is: %i\n", c);

#endif

						if (c >= 32 && c < 256 && c != '.') {
							raceresult_editing[l + 1] = 0;
							raceresult_editing[l] = c;
							Sound_play(S_menu_move, 128);

#ifdef F1SPIRIT_DEBUG_MESSAGES

							output_debug_message("The string being edited is now: %s\n", raceresult_editing);
#endif

						} 
					} 
				} 

				if (k->keyboard[SDLK_BACKSPACE] && !k->old_keyboard[SDLK_BACKSPACE]) {
					if (raceresult_editing[0] != 0)
						raceresult_editing[strlen((char *)raceresult_editing) - 1] = 0;

					Sound_play(S_menu_move, 128);
				} 



				if ((k->keyboard[SDLK_RETURN] && !k->old_keyboard[SDLK_RETURN])) {
					bool replay_name_valid = true;
					FILE *fp;
					char tmp2[128];

					sprintf(tmp2, "replays/%s.rpl", raceresult_editing);

					fp = f1open(tmp2, "b", USERDATA);

					if (fp != 0) {
						replay_name_valid = false;
						fclose(fp);
					} 

					if (replay_name_valid) {
						if (rename(race_game->replay_filename, tmp2) != 0)
							replay_name_valid = false;
					} 

					if (replay_name_valid) {
						Sound_play(S_menu_select, 128);
						raceresult_state = 8;
						raceresult_timmer = 0;
					} else {
						Sound_play(S_wrong, 128);
					} 
				} 

				if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) {
					Sound_play(S_menu_select, 128);
					raceresult_state = 6;
					raceresult_timmer = 0;
				} 
			}

			break;

		case 6:
			/* disappearing menu 2: (rejected) */
			raceresult_timmer++;

			if (raceresult_timmer >= 25) {
				raceresult_state = 7;
				raceresult_timmer = 0;
			} 

			break;

		case 7:
			/* reappearing menu 1: */
			raceresult_timmer++;

			if (raceresult_timmer >= 25) {
				raceresult_state = 1;
				raceresult_timmer = 0;
			} 

			break;

		case 8:
			/* disappearing menu 2: (accepted) */
			raceresult_timmer++;

			if (raceresult_timmer >= 25) {
				raceresult_state = 9;
				raceresult_timmer = 0;
				raceresult_selected = 0;
				raceresult_replaysaved = true;
			} 

			break;

		case 9:
			/* reappearing menu 1: */
			raceresult_timmer++;

			if (raceresult_timmer >= 25) {
				raceresult_state = 1;
				raceresult_timmer = 0;
			} 

			break;

	} 

	return APP_STATE_RACE_RESULT;
} 


void F1SpiritApp::race_result_draw(void)
{
	float y = 0;

	glClearColor(0.2F, 0.2F, 0.2F, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	{

		int i, j;
		int dx = 16, dy = 16;
		float x1, y1, x2, y2;
		Vector prev_v1, prev_v2, v;
		float prev_z1, prev_z2, z;
		bool prev;
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

	/* draw race results: */

	if (menu_selected_nplayers == 1) {
		{
			GLTile *t;
			char tmp[80];
			Uint32 points[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
			Uint32 time = race_game->get_player_time(0);
			SDL_Surface *sfc;

			y = 64;

			sprintf((char *)tmp, "RANKING");
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->set_hotspot(sfc->w / 2, 0);
			t->draw(320, y, 0, 0, 1);
			delete t;
			y += 16;

			if (current_player == friendly_player) {
				sprintf((char *)tmp, "PRACTICE RACE");
			} else {
				sprintf((char *)tmp, "%s", current_player->get_name());
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->set_hotspot(sfc->w / 2, 0);

			t->draw(320, y, 0, 0, 1);

			delete t;

			y += 32;

			if (race_game->get_racefinished()) {
				sprintf((char *)tmp, "POSITION     %.2i", race_game->get_player_position(0) + 1);
			} else {
				sprintf((char *)tmp, "POSITION     --");
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->set_hotspot(sfc->w / 2, 0);

			t->draw(320, y, 0, 0, 1);

			delete t;

			y += 16;

			if (race_game->get_player_position(0) < 9) {
				sprintf((char *)tmp, "POINTS        %i", points[race_game->get_player_position(0)]);
			} else {
				sprintf((char *)tmp, "POINTS        0");
			} 

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);

			t->set_hotspot(sfc->w / 2, 0);

			t->draw(320, y, 0, 0, 1);

			delete t;

			y += 32;

			{
				int h, m, s, c;
				c = time % 100;
				time /= 100;
				s = time % 60;
				time /= 60;
				m = time % 60;
				time /= 60;
				h = time;

				if (raceresult_trackrecord)
					sprintf((char *)tmp, "TIME   %i:%.2i:%.2i %.2i  NEW RECORD!", h, m, s, c);
				else
					sprintf((char *)tmp, "TIME   %i:%.2i:%.2i %.2i", h, m, s, c);
			}

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->set_hotspot(sfc->w / 2, 0);
			t->draw(320, y, 0, 0, 1);
			delete t;
			y += 32;
			{
				int h, m, s, c;
				time = race_game->get_player_bestlap(0);

				if (time != 0) {
					c = time % 100;
					time /= 100;
					s = time % 60;
					time /= 60;
					m = time % 60;
					time /= 60;
					h = time;

					if (raceresult_trackrecord)
						sprintf((char *)tmp, "BEST LAP   %i:%.2i:%.2i %.2i  NEW RECORD!", h, m, s, c);
					else
						sprintf((char *)tmp, "BEST LAP   %i:%.2i:%.2i %.2i", h, m, s, c);
				} else {
					sprintf((char *)tmp, "BEST LAP   -:--:-- --");
				} 
			}

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->set_hotspot(sfc->w / 2, 0);
			t->draw(320, y, 0, 0, 1);
			delete t;
			y += 32;

			if (current_player != friendly_player) {
				sprintf((char *)tmp, "BEST POINT");
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;

				sprintf((char *)tmp, "STOCK       %i", current_player->get_points(0));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;
				sprintf((char *)tmp, "RALLY       %i", current_player->get_points(1));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;
				sprintf((char *)tmp, "F3          %i", current_player->get_points(2));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;
				sprintf((char *)tmp, "F3000       %i", current_player->get_points(3));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;
				sprintf((char *)tmp, "ENDURANCE   %i", current_player->get_points(4));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;
				sprintf((char *)tmp, "F1 TOTAL    %i", current_player->get_points(5) + current_player->get_points(6) +
				        current_player->get_points(7) + current_player->get_points(8) +
				        current_player->get_points(9) + current_player->get_points(10) +
				        current_player->get_points(11) + current_player->get_points(12) +
				        current_player->get_points(13) + current_player->get_points(14) +
				        current_player->get_points(15) + current_player->get_points(16) +
				        current_player->get_points(17) + current_player->get_points(18) +
				        current_player->get_points(19) + current_player->get_points(20));
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 32;
			} 
		}

	} else {

		{
			GLTile *t;
			char tmp[80];

			Uint32 time = 0;
			SDL_Surface *sfc;
			int i;

			y = 64;

			sprintf((char *)tmp, "RANKING");
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
			print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
			t = new GLTile(sfc);
			t->set_hotspot(sfc->w / 2, 0);
			t->draw(320, y, 0, 0, 1);
			delete t;
			y += 32;

			for (i = 0;i < menu_selected_nplayers;i++) {

				time = race_game->get_player_time(i);

				sprintf((char *)tmp, "PLAYER %i", i + 1);
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;

				sprintf((char *)tmp, "POSITION     %.2i", race_game->get_player_position(i) + 1);
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 16;

				if (time == 0) {
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
					print_left_bmp((unsigned char *)"TIME   -:--:-- --", font, sfc, 0, 0, 0);
					t = new GLTile(sfc);
					t->set_hotspot(sfc->w / 2, 0);
					t->draw(320, y, 0, 0, 1);
					delete t;
					y += 16;
				} else {
					int h, m, s, c;
					c = time % 100;
					time /= 100;
					s = time % 60;
					time /= 60;
					m = time % 60;
					time /= 60;
					h = time;

					sprintf((char *)tmp, "TIME   %i:%.2i:%.2i %.2i", h, m, s, c);
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
					print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
					t = new GLTile(sfc);
					t->set_hotspot(sfc->w / 2, 0);
					t->draw(320, y, 0, 0, 1);
					delete t;
					y += 16;
				} 

				{
					int h, m, s, c;
					time = race_game->get_player_bestlap(i);

					if (time != 0) {
						c = time % 100;
						time /= 100;
						s = time % 60;
						time /= 60;
						m = time % 60;
						time /= 60;
						h = time;
						sprintf((char *)tmp, "BEST LAP   %i:%.2i:%.2i %.2i", h, m, s, c);
					} else {
						sprintf((char *)tmp, "BEST LAP   -:--:-- --");
					} 

				}

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

				print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
				t = new GLTile(sfc);
				t->set_hotspot(sfc->w / 2, 0);
				t->draw(320, y, 0, 0, 1);
				delete t;
				y += 32;
			}
		} 

	} 

	/* Print menu options: */
	y += 16;

	if (y > 400)
		y = 400;

	{
		int first = 0;
		float f = 1.0;
		SDL_Surface *sfc;

		if (raceresult_state == 0 ||
		        raceresult_state == 7 ||
		        raceresult_state == 9)
			f = (raceresult_timmer) / 25.0F;

		if (raceresult_state == 2 ||
		        raceresult_state == 3)
			f = (50 - raceresult_timmer) / 50.0F;

		if (raceresult_state == 4)
			f = (raceresult_timmer) / 25.0F;

		if (raceresult_state == 6 ||
		        raceresult_state == 8)
			f = (50 - raceresult_timmer) / 50.0F;

		if (f < 0)
			f = 0;

		if (f > 1)
			f = 1;

		if (raceresult_state == 0 ||
		        raceresult_state == 1 ||
		        raceresult_state == 2 ||
		        raceresult_state == 3 ||
		        raceresult_state == 7 ||
		        raceresult_state == 9) {
			if (raceresult_replaysaved) {
				sfc = draw_menu(7, "", "CONTINUE\nRACE AGAIN\n", 0, raceresult_selected,
				                float(0.6F - 0.4F * sin(raceresult_timmer / 10.0F)),
				                f, font, &first);
			} else {
				sfc = draw_menu(7, "", "CONTINUE\nRACE AGAIN\nSAVE REPLAY\n", 0, raceresult_selected,
				                float(0.6F - 0.4F * sin(raceresult_timmer / 10.0F)),
				                f, font, &first);
			} 
		} else {
			char tmp[128];
			sprintf(tmp, "%s_ [.RPL]\n", raceresult_editing);
			sfc = draw_menu(7, "ENTER FILE NAME:", tmp, 0, 0,
			                float(0.6F - 0.4F * sin(raceresult_timmer / 10.0F)),
			                f, font, &first);
		} 

		GLTile *t;

		t = new GLTile(sfc);

		t->set_hotspot(sfc->w / 2, 0);

		t->draw(320, y, 0, 0, 1);

		delete t;

		if (raceresult_state != 0 && raceresult_state != 2)
			f = 1;

		glColor4f(0, 0, 0, 1 - f);

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);

		glVertex3f(0, 0, -4);

		glVertex3f(0, 480, -4);

		glVertex3f(640, 480, -4);

		glVertex3f(640, 0, -4);

		glEnd();
	} 

} 

