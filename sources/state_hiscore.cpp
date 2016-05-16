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

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

int F1SpiritApp::hiscore_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 0) {
		hiscore_showing = 0;
		hiscore_timmer = 25;
		hiscore_action = 0;
		hiscore_table_regen = true;
	} 

	if (hiscore_timmer > 0 && hiscore_action == 0)
		hiscore_timmer--;

	if (hiscore_timmer > 0 && hiscore_action != 0)
		hiscore_timmer++;

	if (hiscore_timmer == 25) {
		if (hiscore_action == 1) {
			hiscore_action = 0;
			menu_fading = -1;
			menu_fading_ctnt = 25;
			menu_current_menu = 4;
			return APP_STATE_MENU;
		} 

		if (hiscore_action == 2) {
			hiscore_showing++;
			hiscore_table_regen = true;

			if (hiscore_showing >= 3)
				hiscore_showing = 0;

			hiscore_action = 0;
		} 

		if (hiscore_action == 3) {
			hiscore_showing--;
			hiscore_table_regen = true;

			if (hiscore_showing < 0)
				hiscore_showing = 2;

			hiscore_action = 0;
		} 
	} 

	if (k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE] && hiscore_timmer == 0) {
		hiscore_action = 1;
		hiscore_timmer = 1;
	} 

	if (k->keyboard[SDLK_RIGHT] && !k->old_keyboard[SDLK_RIGHT] && hiscore_timmer == 0) {
		hiscore_action = 2;
		hiscore_timmer = 1;
	} 

	if (k->keyboard[SDLK_LEFT] && !k->old_keyboard[SDLK_LEFT] && hiscore_timmer == 0) {
		hiscore_action = 3;
		hiscore_timmer = 1;
	} 

	return APP_STATE_HISCORE;
} 


void F1SpiritApp::hiscore_draw(void)
{
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


	if (hiscore_table_regen) {
		unsigned int i, j;
		char tmp[80];
		char title[80] = "HIGH SCORES";
		char table[25][80];

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("In state HISCORE: regenerating table of hiscores...\n");
#endif

		for (i = 0;i < 26;i++) {
			delete hiscore_table[i];
			hiscore_table[i] = 0;
		} 

		hiscore_table_regen = false;

		for (i = 0;i < 25;i++)
			sprintf(table[i], "....................................");

		if (hiscore_showing == 0) {

#ifdef F1SPIRIT_DEBUG_MESSAGES
			output_debug_message("In state HISCORE: table to regenerate is overall\n");
#endif

			sprintf(title, "OVERALL TOP PLAYERS");

			for (i = 0;i < 25 && i < unsigned(hiscore_list.Length());i++) {
				int h = 0, m = 0, s = 0, c = 0;

				if (hiscore_list[i]->totaltime > 0) {
					int t = hiscore_list[i]->totaltime;
					c = t % 100;
					t /= 100;
					s = t % 60;
					t /= 60;
					m = t % 60;
					t /= 60;
					h = t;
				} 

				for (j = 0;j < strlen(hiscore_list[i]->name);j++)
					table[i][j] = hiscore_list[i]->name[j];

				sprintf(tmp, "%i - %i:%.2i:%.2i'%.2i\"", hiscore_list[i]->points, h, m, s, c);

				for (j = 0;j < strlen(tmp);j++)
					table[i][36 - strlen(tmp) + j] = tmp[j];
			} 
		} 

		if (hiscore_showing == 1) {
			char *tracknames[N_TRACKS] = {"STOCK", "RALLY", "F3", "F3000", "ENDURANCE",
			                              "F1-BRAZIL",
			                              "F1-SAN MARINO",
			                              "F1-BELGIUM",
			                              "F1-MONACO",
			                              "F1-USA",
			                              "F1-FRANCE",
			                              "F1-G. BRITAIN",
			                              "F1-W. GERMANY",
			                              "F1-HUNGARY",
			                              "F1-AUSTRIA",
			                              "F1-ITALY",
			                              "F1-PORTUGAL",
			                              "F1-SPAIN",
			                              "F1-MEXICO",
			                              "F1-JAPAN",
			                              "F1-AUSTRALIA",
			                              "STOCK-OVAL",
			                              "F1-OVAL",
			                              "STOCK-MICRO",
			                              "F1-MICRO"
			                             };

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("In state HISCORE: table to regenerate is best track times\n");
#endif

			sprintf(title, "BEST TRACK TIMES");

			for (i = 0;i < N_TRACKS;i++) {
				int h = 0, m = 0, s = 0, c = 0;

				for (j = 0;j < strlen(tracknames[i]);j++)
					table[i][j] = tracknames[i][j];

				for (j = 0;j < strlen(hiscore_time_names[i]);j++)
					table[i][j + 14] = hiscore_time_names[i][j];

				if (hiscore_time[i] > 0) {
					int t = hiscore_time[i];
					c = t % 100;
					t /= 100;
					s = t % 60;
					t /= 60;
					m = t % 60;
					t /= 60;
					h = t;
				} 

				if (h == 0) {
					sprintf(tmp, "%.2i:%.2i'%.2i\"", m, s, c);
				} else {
					sprintf(tmp, "%i:%.2i:%.2i'%.2i\"", h, m, s, c);
				} 


				for (j = 0;j < strlen(tmp);j++)
					table[i][36 - strlen(tmp) + j] = tmp[j];
			} 
		} 


		if (hiscore_showing == 2) {
			char *tracknames[N_TRACKS] = {"STOCK", "RALLY", "F3", "F3000", "ENDURANCE",
			                              "F1-BRAZIL",
			                              "F1-SAN MARINO",
			                              "F1-BELGIUM",
			                              "F1-MONACO",
			                              "F1-USA",
			                              "F1-FRANCE",
			                              "F1-G. BRITAIN",
			                              "F1-W. GERMANY",
			                              "F1-HUNGARY",
			                              "F1-AUSTRIA",
			                              "F1-ITALY",
			                              "F1-PORTUGAL",
			                              "F1-SPAIN",
			                              "F1-MEXICO",
			                              "F1-JAPAN",
			                              "F1-AUSTRALIA",
			                              "STOCK-OVAL",
			                              "F1-OVAL",
			                              "STOCK-MICRO",
			                              "F1-MICRO"
			                             };

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("In state HISCORE: table to regenerate is best lap times\n");
#endif

			sprintf(title, "BEST LAP TIMES");

			for (i = 0;i < N_TRACKS;i++) {
				int h = 0, m = 0, s = 0, c = 0;

				for (j = 0;j < strlen(tracknames[i]);j++)
					table[i][j] = tracknames[i][j];

				for (j = 0;j < strlen(hiscore_bestlap_names[i]);j++)
					table[i][j + 14] = hiscore_bestlap_names[i][j];

				if (hiscore_bestlap[i] > 0) {
					int t = hiscore_bestlap[i];
					c = t % 100;
					t /= 100;
					s = t % 60;
					t /= 60;
					m = t % 60;
					t /= 60;
					h = t;
				} 

				if (h == 0) {
					sprintf(tmp, "%.2i:%.2i'%.2i\"", m, s, c);
				} else {
					sprintf(tmp, "%i:%.2i:%.2i'%.2i\"", h, m, s, c);
				} 


				for (j = 0;j < strlen(tmp);j++)
					table[i][36 - strlen(tmp) + j] = tmp[j];
			} 
		} 

		{
			int i;
			GLTile *t;

#ifdef F1SPIRIT_DEBUG_MESSAGES

			output_debug_message("In state HISCORE: generating text surfaces...\n");
#endif

			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)title, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			print_left_bmp((unsigned char *)title, font, sfc, 0, 0, 0);

			t = new GLTile(sfc);
			t->set_hotspot(sfc->w / 2, 0);
			hiscore_table[0] = t;
			//   SDL_FreeSurface(sfc);

			for (i = 0;i < 25;i++) {
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)table[0], font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_FillRect(sfc, 0, 0);
				print_left_bmp((unsigned char *)table[i], font, sfc, 0, 0, 0);

				if (hiscore_showing == 0) {
					if (i == 0)
						surface_fader(sfc, 0.5F, 0.5F, 1, 1, 0);

					if (i == 1)
						surface_fader(sfc, 0.65F, 0.65F, 1, 1, 0);

					if (i == 2)
						surface_fader(sfc, 0.85F, 0.85F, 1, 1, 0);
				} else {
					if ((i % 2) == 0)
						surface_fader(sfc, 0.85F, 0.85F, 1, 1, 0);
				} 

				t = new GLTile(sfc);

				t->set_hotspot(sfc->w / 2, 0);

				hiscore_table[i + 1] = t;
			} 

			//   SDL_FreeSurface(sfc);
		}

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("In state HISCORE: table regenerated.\n");

#endif

	} 

	{
		int i;

		if (hiscore_table[0] != 0)
			hiscore_table[0]->draw(320, 8, 0, 0, 1);

		for (i = 0;i < 25;i++)
			if (hiscore_table[i + 1] != 0)
				hiscore_table[i + 1]->draw(320, float(40 + i*16), 0, 0, 1);

		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)"LEFT/RIGHT/ESCAPE TO BROWSE", font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);

		SDL_FillRect(sfc, 0, 0);

		print_left_bmp((unsigned char *)"LEFT/RIGHT/ESCAPE TO BROWSE", font, sfc, 0, 0, 0);

		surface_fader(sfc, 0.9F, 0.9F, 0.9F, float(0.6F + 0.4*sin(state_cycle / 4.0F)), 0);

		GLTile *t = new GLTile(sfc);

		t->set_hotspot(sfc->w / 2, 0);

		t->draw(320, 456, 0, 0, 1);

		delete t;

		//  SDL_FreeSurface(sfc);
	}



	if (hiscore_timmer >= 0) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = abs(hiscore_timmer) / 25.0F;
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

