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

#define MENU_CONSTANT 20
#define TRACKPREVIEW_CONSTANT 25


void F1SpiritApp::menu_draw(void)
{
	GLTile *menu;
	SDL_Surface *sfc1 = 0, *sfc2 = 0;
	int w1, w2, wt;
	float sel_factor, enter_factor = 1.0F;
	int menu_y = 300;

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (menu_state < MENU_CONSTANT)
		enter_factor = menu_state / float(MENU_CONSTANT);

	if (menu_state == MENU_CONSTANT)
		enter_factor = 1.0F;

	if (menu_state > MENU_CONSTANT)
		enter_factor = ((MENU_CONSTANT * 2) - menu_state) / float(MENU_CONSTANT);

	menu_background->draw();

	if (menu_current_menu == 23) {
		float f = 0;
		{

			if (menu_state < MENU_CONSTANT)
				f = menu_state / float(MENU_CONSTANT);

			if (menu_state == MENU_CONSTANT)
				f = 1.0F;

			if (menu_state > MENU_CONSTANT)
				f = ((MENU_CONSTANT * 2) - menu_state) / float(MENU_CONSTANT);

			glColor4f(0, 0, 0, f*0.5F);
		}

		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -4);
		glVertex3f(0, 480, -4);
		glVertex3f(640, 480, -4);
		glVertex3f(640, 0, -4);
		glEnd();

		/* Show readme.txt file: */
		{

			if (menu_readme_line.Length() == 0) {
				FILE *fp;
				char tmp[512];
				int i, c;
				GLTile *t;
				SDL_Surface *sfc;

				fp = f1open("readme.txt", "r", GAMEDATA);

				if (fp != 0) {

					i = 0;

					while (!feof(fp)) {
						c = fgetc(fp);

						if (c != 13 && c != 10) {
							tmp[i++] = c;
						} else {
							if (c == 10) {
								if (i == 0) {
									menu_readme_line.Add(0);
								} else {
									tmp[i++] = 0;

									if (tmp[0] == '-' && tmp[1] == '-') {
										sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
										print_left_bmp((unsigned char *)tmp, font, sfc, 0, 0, 0);
										surface_fader(sfc, 0.5, 0.5, 1, 1, 0);
									} else {
										sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, -1), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
										print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, -1);
									} 

									t = new GLTile(sfc);

									//         t->set_hotspot(sfc->w/2,0);
									menu_readme_line.Add(t);

									i = 0;
								} 
							} 
						} 

					} 

					fclose(fp);
				} 

			} 

			{
				GLTile *t;
				List<GLTile> l;
				int y = 8 - menu_readme_start_y;

				l.Instance(menu_readme_line);
				l.Rewind();

				while (l.Iterate(t) && y < 480) {
					if (t != 0) {
						y += t->get_dy() + 2;
					} else {
						y += 10;
					} 
				} 

				if (l.EndP() && y < 400)
					menu_readme_start_y -= (400 - y);

				y = 8 - menu_readme_start_y;

				l.Rewind();

				while (l.Iterate(t) && y < 480) {
					if (t != 0) {
						t->draw(1, 1, 1, f, 16, float(y), 0, 0, 1);
						y += t->get_dy() + 2;
					} else {
						y += 10;
					} 
				} 
			}

		}

	} 

	if (menu_options[0] != 0) {
		sel_factor = float(0.6F - 0.4F * sin(menu_selected_timmer[0] / 10.0F));

		if (menu_prev_nmenus == 2 &&
		        (menu_current_menu != 3 || menu_selected[1] != 0)
		        && menu_current_menu != 4) {
			sfc1 = draw_menu(7, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, 1, font, &(menu_first_option[0]));
		} else {
			if ((menu_option_type[0][menu_selected[0]] == 11 && menu_state > MENU_CONSTANT) ||
			        (menu_state < MENU_CONSTANT && menu_options[1] != 0)) {
				sfc1 = draw_menu(7, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, 1, font, &(menu_first_option[0]));
			} else {
				sfc1 = draw_menu(7, menu_title[0], menu_options[0], menu_option_type[0], menu_selected[0], sel_factor, enter_factor, font, &(menu_first_option[0]));
			} 
		} 
	} 

	if (menu_options[1] != 0) {
		sel_factor = float(0.6F - 0.4F * sin(menu_selected_timmer[1] / 10.0F));

		if (menu_redefining_key)
			sel_factor = 0;

		sfc2 = draw_menu(7, menu_title[1], menu_options[1], menu_option_type[1], menu_selected[1], sel_factor, enter_factor, font, &(menu_first_option[1]));
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

	/* Track: */
	if (menu_track != 0) {
		if (menu_track_frame == 0) {
			int dx, dy;
			SDL_Surface *sfc;

			dx = menu_track->get_dx();
			dy = menu_track->get_dy();
			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, dx + 32, dy + 32, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			draw_menu_frame(sfc, 0, 0, dx + 32, dy + 32);

			menu_track_frame = new GLTile(sfc);
			//   SDL_FreeSurface(sfc);
		} 

		if (menu_points_frame == 0) {
			SDL_Surface *sfc;
			unsigned char tmp[80];
			int requirements[N_TRACKS] = {0, 0, 0, 12, 12,
			                              25, 33, 33, 48, 48, 65, 65, 84, 84, 105, 105, 128, 128, 153, 153, 180,
			                              180, 180, 180, 180
			                             };

			sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, 256 + 32, 256 + 32, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);

			print_left_bmp((unsigned char *)"TRACK INFO:", font, sfc, 32, 32, 0);

			/* puntuación, mejor tiempo total, mejor tiempo del jugador actual */
			sprintf((char *)tmp, "SCORE:");
			print_left_bmp(tmp, font, sfc, 32, 64, 0);
			sprintf((char *)tmp, "  %i", current_player->get_points(menu_showing_track));
			print_left_bmp(tmp, font, sfc, 32, 80, 0);

			sprintf((char *)tmp, "(COLLECTED %i/189)", current_player->get_points());
			print_left_bmp(tmp, small_font, sfc, 96, 84, 0);

			sprintf((char *)tmp, "TIME:");
			print_left_bmp(tmp, font, sfc, 32, 112, 0);

			{
				int t;
				int h, m, s, c;
				t = current_player->get_time(menu_showing_track);

				if (t == 0) {
					sprintf((char *)tmp, "  -:--:-- --");
				} else {
					c = t % 100;
					t /= 100;
					s = t % 60;
					t /= 60;
					m = t % 60;
					t /= 60;
					h = t;

					if (h == 0) {
						sprintf((char *)tmp, "  %.2i:%.2i %.2i", m, s, c);
					} else {
						sprintf((char *)tmp, "  %i:%.2i:%.2i %.2i", h, m, s, c);
					} 
				} 

				print_left_bmp(tmp, font, sfc, 32, 128, 0);
			}

			print_left_bmp((unsigned char *)"BEST TIME:", font, sfc, 32, 160, 0);
			{
				int t;
				int h, m, s, c;
				t = hiscore_time[menu_showing_track];

				if (t == 0) {
					sprintf((char *)tmp, "  -:--:-- --");
				} else {
					c = t % 100;
					t /= 100;
					s = t % 60;
					t /= 60;
					m = t % 60;
					t /= 60;
					h = t;

					if (h == 0) {
						sprintf((char *)tmp, "  %.2i:%.2i %.2i", m, s, c);
					} else {
						sprintf((char *)tmp, "  %i:%.2i:%.2i %.2i", h, m, s, c);
					} 
				} 

				print_left_bmp(tmp, font, sfc, 32, 176, 0);

				if (hiscore_time_names[menu_showing_track] != 0) {
					sprintf((char *)tmp, "  %s", hiscore_time_names[menu_showing_track]);
					print_left_bmp(tmp, font, sfc, 32, 192, 0);
				} 
			}

			if (current_player->get_points() < requirements[menu_showing_track]) {
				sprintf((char *)tmp, "REQUIREMENTS %i", requirements[menu_showing_track]);

				if (requirements[menu_showing_track] < 100) {
					print_left_bmp(tmp, font, sfc, 24, 234, 0);
				} else {
					print_left_bmp(tmp, font, sfc, 16, 234, 0);
				} 
			} 

			draw_menu_frame(sfc, 0, 0, 256 + 32, 256 + 32);

			menu_points_frame = new GLTile(sfc);
		} 

		if (menu_showing_track_timmer != 0) {
			float f = 1;

			if (menu_showing_track_timmer < 0) {
				f = 1 + menu_showing_track_timmer * 0.04F;
			} else {
				f = menu_showing_track_timmer * 0.04F;
			} 

			menu_track_frame->draw(1, 1, 1, f, 16, 8, 0, 0, 1);

			menu_track->draw(1, 1, 1, f, 32, 24, 0, 0, 1);

			if (menu_track_locked)
				menu_locked->draw(1, 1, 1, f, 32, 24, 0, 0, 1);

			menu_points_frame->draw(1, 1, 1, f, 336, 8, 0, 0, 1);
		} else {
			menu_track_frame->draw(16, 8, 0, 0, 1);

			if (menu_showing_track_timmer2 > 200) {
				if (menu_track_viewer == 0)
					menu_track_viewer = new F1SpiritTrackViewer(menu_showing_track);

				if (menu_trackviewing_background == 0)
					menu_trackviewing_background = new GLTile(32, 480 - (24 + 256), 256, 256);

				glEnable( GL_SCISSOR_TEST );

				glScissor(32, 480 - (24 + 256), 256, 256);

				glViewport(32 - 42, 480 - (24 + 256), 341, 256);

				menu_track_viewer->draw();

				glViewport(0, 0, SCREEN_X, SCREEN_Y);

				glScissor(0, 0, 640, 480);

				glDisable( GL_SCISSOR_TEST );

				if (menu_showing_track_timmer2 < 200 + TRACKPREVIEW_CONSTANT) {
					float f = float(menu_showing_track_timmer2 - 200) / TRACKPREVIEW_CONSTANT;
					menu_trackviewing_background->draw(1, 1, 1, 1 - f, 32, 24, 0, 0, 1);
					menu_track->draw(1, 1, 1, 1 - f, 32, 24, 0, 0, 1);
				} 

				if (menu_track_viewer->get_nlaps() > 0 && menu_showing_track_timmer2 < 10000)
					menu_showing_track_timmer2 = 10000;

				if (menu_showing_track_timmer2 > 10000) {
					float f = float(menu_showing_track_timmer2 - 10000) / TRACKPREVIEW_CONSTANT;

					if (f > 1)
						f = 1;

					menu_trackviewing_background->draw(1, 1, 1, f, 32, 24, 0, 0, 1);

					menu_track->draw(1, 1, 1, f, 32, 24, 0, 0, 1);
				} 

			} else {
				if (menu_trackviewing_background != 0) {
					delete menu_trackviewing_background;
					menu_trackviewing_background = 0;
				} 

				menu_track->draw(32, 24, 0, 0, 1);
			} 

			if (menu_track_locked)
				menu_locked->draw(32, 24, 0, 0, 1);

			menu_points_frame->draw(336, 8, 0, 0, 1);


		} 
	} 



	/* Cars: */
	{
		int i;
		int w = 32;
		int x;

		for (i = 0;i < 3;i++) {
			if (menu_car_top[i] != 0 &&
			        menu_car_side[i] != 0) {
				if (menu_car_frame[i] == 0) {
					int dx, dy;
					SDL_Surface *sfc;

					dx = max(menu_car_top[i]->get_dx(), menu_car_side[i]->get_dx());
					dy = menu_car_top[i]->get_dy() + 16 + menu_car_side[i]->get_dy();
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, dx + 32, dy + 32, 32, RMASK, GMASK, BMASK, AMASK);
					SDL_FillRect(sfc, 0, 0);
					draw_menu_frame(sfc, 0, 0, dx + 32, dy + 32, 220);
					//surface_shifter(sfc, 64, 64, 64, 0, 0);
					menu_car_frame[i] = new GLTile(sfc);
					//     SDL_FreeSurface(sfc);
				} 
			} 
		} 

		if (menu_car_frame[0] != 0)
			w += menu_car_frame[0]->get_dx();

		if (menu_car_frame[1] != 0)
			w += menu_car_frame[1]->get_dx();

		if (menu_car_frame[2] != 0)
			w += menu_car_frame[2]->get_dx();

		if (menu_showing_car != -1) {
			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 96, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			draw_menu_frame(sfc, 0, 0, 512, 96);

			switch (menu_showing_car_type) {

				case 0:
					/* STOCK */

					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"V8 2900", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC-TURBO, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"595PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"1500KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"V12 2800", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"560PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"1200KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"V8 3000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE TURBO", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"680PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"1015KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;

				case 1:
					/* RALLY */
					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"V6 2000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC-TURBO, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"560PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"1020KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"V6 2996", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"480PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"985KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"V8 5000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"580PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"1000KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;

				case 2:
					/* F3 */
					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"4R 1998", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"180PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"460KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"4R 1998", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"165PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"450KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"4R 2000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"170PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"355KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;

				case 3:
					/* F3000 */
					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"V8 3000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"480PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"V8 3000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 16VALVE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"465PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"V8 2000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"480PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;

				case 4:
					/* ENDURANCE */
					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"V8 3000", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"865PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"850KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"V12 5993", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC 24VALVE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"780PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"700KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"V6 2960", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC TWIN TURBO", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"900PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"756KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;

				case 5:
					/* F1 */
					switch (menu_showing_car) {

						case 0:
							print_left_bmp((unsigned char *)"V6 1500", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC-TURBO, AUTOMATIC", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"1300PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"V8 1400", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"1060PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"V6 1500", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"1200PS", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"540KG", font, sfc, 16, 64, 0);
							break;
					} 

					break;
			} 

			delete menu_info_frame;

			menu_info_frame = 0;

			menu_info_frame = new GLTile(sfc);

			//   SDL_FreeSurface(sfc);
		} else {
			delete menu_info_frame;
			menu_info_frame = 0;
		} 

		x = 320 - w / 2;

		{
			float f = 1;

			if (menu_showing_car_timmer < 0) {
				f = 1 + menu_showing_car_timmer * 0.04F;
			} else {
				if (menu_showing_car_timmer != 0)
					f = menu_showing_car_timmer * 0.04F;
				else
					f = 1;
			} 

			for (i = 0;i < 3;i++) {
				if (menu_car_top[i] != 0 &&
				        menu_car_side[i] != 0) {
					float f1 = 1;

					if (menu_showing_car != i)
						f1 = 0.25F;

					if (menu_showing_car_timmer != 0) {
						int y = 8;
						f1 *= f;
						menu_car_frame[i]->draw(1, 1, 1, f1, float(x), float(y), 0, 0, 1);
						menu_car_top[i]->draw(1, 1, 1, f1, float(x + menu_car_frame[i]->get_dx() / 2 - menu_car_top[i]->get_dx() / 2), float(y + 16), 0, 0, 1);
						menu_car_side[i]->draw(1, 1, 1, f1, float(x + menu_car_frame[i]->get_dx() / 2 - menu_car_side[i]->get_dx() / 2), float(y + menu_car_top[i]->get_dy() + 16 + 16), 0, 0, 1);

					} else {
						int y = 8;
						menu_car_frame[i]->draw(1, 1, 1, f1, float(x), float(y), 0, 0, 1);
						menu_car_top[i]->draw(1, 1, 1, f1, float(x + menu_car_frame[i]->get_dx() / 2 - menu_car_top[i]->get_dx() / 2), float(y + 16), 0, 0, 1);
						menu_car_side[i]->draw(1, 1, 1, f1, float(x + menu_car_frame[i]->get_dx() / 2 - menu_car_side[i]->get_dx() / 2), float(y + menu_car_top[i]->get_dy() + 16 + 16), 0, 0, 1);
					} 

					x += menu_car_frame[i]->get_dx() + 16;
				} 
			} 

			if (menu_car_frame[0] != 0 && menu_info_frame != 0)
				menu_info_frame->draw(1, 1, 1, f, float(64), float(8 + menu_car_frame[0]->get_dy()), 0, 0, 1);
		}
	}


	/* Personal Design: */
	if (menu_design != 0) {

		if (menu_showing_design_timmer != 0) {
			float f = 1;

			if (menu_showing_design_timmer < 0) {
				f = 1 + menu_showing_design_timmer * 0.04F;
			} else {
				f = menu_showing_design_timmer * 0.04F;
			} 

			menu_design->draw(1, 1, 1, f, 320, 8, 0, 0, 0.8F);

		} else {
			menu_design->draw(320, 8, 0, 0, 0.8F);
		} 
	} 


	/* Car pieces: */
	{

		int i;
		int w = 16;
		int x;
		bool reset_piece_x = false;
		int piece_x[7];

		for (i = 0;i < 6;i++) {
			if (menu_piece[i] != 0) {
				if (menu_piece_frame[i] == 0) {
					int dx, dy;
					SDL_Surface *sfc;

					reset_piece_x = true;
					dx = menu_piece[i]->get_dx();
					dy = menu_piece[i]->get_dy();
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, dx + 32, dy + 32, 32, RMASK, GMASK, BMASK, AMASK);
					SDL_FillRect(sfc, 0, 0);
					draw_menu_frame(sfc, 0, 0, dx + 32, dy + 32, 220);
					//surface_shifter(sfc, 64, 64, 64, 0, 0);
					menu_piece_frame[i] = new GLTile(sfc);
					//     SDL_FreeSurface(sfc);
				} 
			} 
		} 

		piece_x[0] = w;

		if (menu_piece_frame[0] != 0)
			w += menu_piece_frame[0]->get_dx() + 16;

		piece_x[1] = w;

		if (menu_piece_frame[1] != 0)
			w += menu_piece_frame[1]->get_dx() + 16;

		piece_x[2] = w;

		if (menu_piece_frame[2] != 0)
			w += menu_piece_frame[2]->get_dx() + 16;

		piece_x[3] = w;

		if (menu_piece_frame[3] != 0)
			w += menu_piece_frame[3]->get_dx() + 16;

		piece_x[4] = w;

		if (menu_piece_frame[4] != 0)
			w += menu_piece_frame[4]->get_dx() + 16;

		piece_x[5] = w;

		if (menu_piece_frame[5] != 0)
			w += menu_piece_frame[5]->get_dx() + 16;

		piece_x[6] = w;

		if (menu_showing_piece != -1) {
			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, 576, 96, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_FillRect(sfc, 0, 0);
			draw_menu_frame(sfc, 0, 0, 576, 96);

			switch (menu_showing_piece_type) {

				case 0:
					/* BODY */

					switch (menu_showing_piece) {

						case 0:
							print_left_bmp((unsigned char *)"STRONG BODY", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"CARBON FIBER FRAME", font, sfc, 16, 32, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"BALANCED SETTING", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"CARBON FIBER FRAME", font, sfc, 16, 32, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"LIGHT WEIGHT", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"CARBON FIBER FRAME", font, sfc, 16, 32, 0);
							break;
					} 

					break;

				case 1:
					/* ENGINE */
					switch (menu_selected_track) {

						case 0:

						case 1:

							switch (menu_showing_piece) {

								case 0:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"LOW-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"580PS", font, sfc, 16, 64, 0);
									break;

								case 1:
									print_left_bmp((unsigned char *)"V6 1490cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"WIDE-TORUQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"600PS", font, sfc, 16, 64, 0);
									break;

								case 2:
									print_left_bmp((unsigned char *)"V8 1200cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"EASY-CONTROL", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"700PS", font, sfc, 16, 64, 0);
									break;

								case 3:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"2BAND-TORQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"680PS", font, sfc, 16, 64, 0);
									break;

								case 4:
									print_left_bmp((unsigned char *)"V8 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"HIGH-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"800PS", font, sfc, 16, 64, 0);
									break;

								case 5:
									print_left_bmp((unsigned char *)"V6 1400cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"PEEKY POWER", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"720PS", font, sfc, 16, 64, 0);
									break;
							} 

							break;

						case 2:
							switch (menu_showing_piece) {

								case 0:
									print_left_bmp((unsigned char *)"V6 2000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"LOW-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"175PS", font, sfc, 16, 64, 0);
									break;

								case 1:
									print_left_bmp((unsigned char *)"V6 1800cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"WIDE-TORUQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"165PS", font, sfc, 16, 64, 0);
									break;

								case 2:
									print_left_bmp((unsigned char *)"V8 2000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"EASY-CONTROL", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"175PS", font, sfc, 16, 64, 0);
									break;

								case 3:
									print_left_bmp((unsigned char *)"V6 2000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"2BAND-TORQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"180PS", font, sfc, 16, 64, 0);
									break;

								case 4:
									print_left_bmp((unsigned char *)"V6 2000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"HIGH-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"190PS", font, sfc, 16, 64, 0);
									break;

								case 5:
									print_left_bmp((unsigned char *)"V6 2000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"PEEKY POWER", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"185PS", font, sfc, 16, 64, 0);
									break;
							} 

							break;

						case 3:
							switch (menu_showing_piece) {

								case 0:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"LOW-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"400PS", font, sfc, 16, 64, 0);
									break;

								case 1:
									print_left_bmp((unsigned char *)"V6 1490cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"WIDE-TORUQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"450PS", font, sfc, 16, 64, 0);
									break;

								case 2:
									print_left_bmp((unsigned char *)"V8 1200cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"EASY-CONTROL", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"470PS", font, sfc, 16, 64, 0);
									break;

								case 3:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"2BAND-TORQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"480PS", font, sfc, 16, 64, 0);
									break;

								case 4:
									print_left_bmp((unsigned char *)"V8 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"HIGH-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"497PS", font, sfc, 16, 64, 0);
									break;

								case 5:
									print_left_bmp((unsigned char *)"V6 1400cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"PEEKY POWER", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"487PS", font, sfc, 16, 64, 0);
									break;

							} 

							break;

						case 4:
							switch (menu_showing_piece) {

								case 0:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"LOW-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"780PS", font, sfc, 16, 64, 0);
									break;

								case 1:
									print_left_bmp((unsigned char *)"V6 1490cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"WIDE-TORUQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"870PS", font, sfc, 16, 64, 0);
									break;

								case 2:
									print_left_bmp((unsigned char *)"V8 1200cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"EASY-CONTROL", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"900PS", font, sfc, 16, 64, 0);
									break;

								case 3:
									print_left_bmp((unsigned char *)"V6 3000cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"2BAND-TORQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"880PS", font, sfc, 16, 64, 0);
									break;

								case 4:
									print_left_bmp((unsigned char *)"V8 3200cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"HIGH-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"990PS", font, sfc, 16, 64, 0);
									break;

								case 5:
									print_left_bmp((unsigned char *)"V6 2400cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"PEEKY POWER", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"920PS", font, sfc, 16, 64, 0);
									break;
							} 

							break;

						default:
							switch (menu_showing_piece) {

								case 0:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"LOW-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1180PS", font, sfc, 16, 64, 0);
									break;

								case 1:
									print_left_bmp((unsigned char *)"V6 1490cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"WIDE-TORUQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1100PS", font, sfc, 16, 64, 0);
									break;

								case 2:
									print_left_bmp((unsigned char *)"V8 1200cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"EASY-CONTROL", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1200PS", font, sfc, 16, 64, 0);
									break;

								case 3:
									print_left_bmp((unsigned char *)"V6 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC-TURBO", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"2BAND-TORQUE", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1500PS", font, sfc, 16, 64, 0);
									break;

								case 4:
									print_left_bmp((unsigned char *)"V8 1500cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"TURBO SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"HIGH-COMPRESS", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1300PS", font, sfc, 16, 64, 0);
									break;

								case 5:
									print_left_bmp((unsigned char *)"V6 1400cc", font, sfc, 16, 16, 0);
									print_left_bmp((unsigned char *)"DOHC SUPER-CHARGER", font, sfc, 16, 32, 0);
									print_left_bmp((unsigned char *)"PEEKY POWER", font, sfc, 16, 48, 0);
									print_left_bmp((unsigned char *)"1030PS", font, sfc, 16, 64, 0);
									break;
							} 
					} 

					break;

				case 2:
					/* BRAKE */
					switch (menu_showing_piece) {

						case 0:
							print_left_bmp((unsigned char *)"FRONT: DISK TYPE-A", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"REAR:  DISK TYPE-A", font, sfc, 16, 32, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"FRONT: DISK TYPE-B", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"REAR:  DRUM", font, sfc, 16, 32, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"FRONT: DISK TYPE-B", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"REAR:  DISK TYPE-B", font, sfc, 16, 32, 0);
							break;
					} 

					break;

				case 3:
					/* SUSPENSION */
					switch (menu_showing_piece) {

						case 0:
							print_left_bmp((unsigned char *)"SOFT-SETTING", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"TWINTUBE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"GASSHOCK TYPE", font, sfc, 16, 48, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"MEDIUM-SETTING", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"TWINTUBE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"ADJUSTABLE TYPE", font, sfc, 16, 48, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"HARD-SETTING", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"MONOTUBE", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"OILSHOCK TYPE", font, sfc, 16, 48, 0);
							break;
					} 

					break;

				case 4:
					/* GEAR */
					switch (menu_showing_piece) {

						case 0:
							print_left_bmp((unsigned char *)"4SPEED AUTOMATIC", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"NEW TYPE", font, sfc, 16, 32, 0);
							break;

						case 1:
							print_left_bmp((unsigned char *)"4SPEED MANUAL          1 - 13.26", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"                       2 - 18.26", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"                       3 - 24.25", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"                       4 - 27.21", font, sfc, 16, 64, 0);
							break;

						case 2:
							print_left_bmp((unsigned char *)"4SPEED MANUAL HI-GEAR  1 - 14.29", font, sfc, 16, 16, 0);
							print_left_bmp((unsigned char *)"                       2 - 22.29", font, sfc, 16, 32, 0);
							print_left_bmp((unsigned char *)"                       3 - 23.24", font, sfc, 16, 48, 0);
							print_left_bmp((unsigned char *)"                       4 - 26.21", font, sfc, 16, 64, 0);
							break;
					} 

					break;
			} 

			delete menu_info_frame;

			menu_info_frame = 0;

			menu_info_frame = new GLTile(sfc);

			//   SDL_FreeSurface(sfc);
		} else {
			delete menu_info_frame;
			menu_info_frame = 0;
		} 

		x = 320 - w / 2;

		{
			int tmpx = x + piece_x[menu_showing_piece];
			int tmpx2 = x + piece_x[menu_showing_piece + 1];

			menu_desired_showing_piece_x = x;

			if (tmpx < 16)
				menu_desired_showing_piece_x = x - (tmpx - 16);

			if (tmpx2 > 640 - 16)
				menu_desired_showing_piece_x = x - (tmpx2 - (640 - 16));
		}

		if (reset_piece_x)
			menu_showing_piece_x = menu_desired_showing_piece_x;

		x = menu_showing_piece_x;


		{
			float f = 1;

			if (menu_showing_piece_timmer < 0) {
				f = 1 + menu_showing_piece_timmer * 0.04F;
			} else {
				if (menu_showing_piece_timmer != 0)
					f = menu_showing_piece_timmer * 0.04F;
				else
					f = 1;
			} 

			for (i = 0;i < 6;i++) {
				if (menu_piece[i] != 0) {
					float f1 = 1;

					if (menu_showing_piece != i)
						f1 = 0.25F;

					if (menu_showing_piece_timmer != 0) {
						int y = 8;
						f1 *= f;
						menu_piece_frame[i]->draw(1, 1, 1, f1, float(x), float(y), 0, 0, 1);
						menu_piece[i]->draw(1, 1, 1, f1, float(x + menu_piece_frame[i]->get_dx() / 2 - menu_piece[i]->get_dx() / 2), float(y + 16), 0, 0, 1);

					} else {
						int y = 8;
						menu_piece_frame[i]->draw(1, 1, 1, f1, float(x), float(y), 0, 0, 1);
						menu_piece[i]->draw(1, 1, 1, f1, float(x + menu_piece_frame[i]->get_dx() / 2 - menu_piece[i]->get_dx() / 2), float(y + 16), 0, 0, 1);
					} 

					x += menu_piece_frame[i]->get_dx() + 16;
				} 
			} 

			if (menu_piece_frame[0] != 0 && menu_info_frame != 0)
				menu_info_frame->draw(1, 1, 1, f, float(32), float(8 + menu_piece_frame[0]->get_dy()), 0, 0, 1);
		}

	}


	/* Network menus: player list & chat window */
	{
		if (menu_playerlist_timmer > 0) {
			float f = menu_playerlist_timmer / 50.0F;

			if (menu_playerlist_frame == 0) {
				int dx, dy;
				SDL_Surface *sfc;

				dx = 168;
				dy = 256;
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, dx + 32, dy + 32, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_FillRect(sfc, 0, 0);
				draw_menu_frame(sfc, 0, 0, dx + 32, dy + 32);

				menu_playerlist_frame = new GLTile(sfc);
			} 

			menu_playerlist_frame->draw(1, 1, 1, f, float(8), float(8), 0, 0, 1);

			/* Draw player list: */
			{
				int y = 24;
				GLTile *t;
				SDL_Surface *sfc;
				List<F1SComputer> l;
				F1SComputer *c;

				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)menu_local_computer.name, font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_FillRect(sfc, 0, 0);
				print_left_bmp((unsigned char *)menu_local_computer.name, small_font, sfc, 0, 0, 0);

				t = new GLTile(sfc);
				t->set_hotspot(0, 0);

				t->draw(0.5F, 0.5F, 1, f, float(24), float(y), 0, 0, 1);
				delete t;
				y += 10;

				if (!menu_local_is_server &&
				        menu_selected_server != 0 &&
				        menu_selected_server->name[0] != 0) {
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)menu_selected_server->name, small_font, 0), font->h, 32, RMASK, GMASK, BMASK, AMASK);
					SDL_FillRect(sfc, 0, 0);
					print_left_bmp((unsigned char *)menu_selected_server->name, small_font, sfc, 0, 0, 0);

					t = new GLTile(sfc);
					t->set_hotspot(0, 0);

					t->draw(1, 1, 1, f, float(24), float(y), 0, 0, 1);
					delete t;
					y += 10;
				} 

				l.Instance(menu_registered_clients);

				l.Rewind();

				while (l.Iterate(c)) {
					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)c->name, font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
					SDL_FillRect(sfc, 0, 0);
					print_left_bmp((unsigned char *)c->name, small_font, sfc, 0, 0, 0);

					t = new GLTile(sfc);
					t->set_hotspot(0, 0);

					t->draw(1, 1, 1, f, float(24), float(y), 0, 0, 1);
					delete t;
					y += 10;
				} 
			}
		} 

		if (menu_chat_timmer > 0) {
			float f = menu_playerlist_timmer / 50.0F;

			if (menu_chat_frame == 0) {
				int dx, dy;
				SDL_Surface *sfc;

				dx = 400;
				dy = 256;
				sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, dx + 32, dy + 32, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_FillRect(sfc, 0, 0);
				draw_menu_frame(sfc, 0, 0, dx + 32, dy + 32);

				menu_chat_frame = new GLTile(sfc);
			} 

			menu_chat_frame->draw(1, 1, 1, f, float(208), float(8), 0, 0, 1);

			/* Draw chat messages: */
			{
				int y = 24;
				GLTile *t;
				SDL_Surface *sfc;
				List<ChatMessage> l;
				ChatMessage *m;
				char tmp[320];

				while (menu_chat_messages.Length() > 26) {
					m = menu_chat_messages.ExtractIni();
					delete m;
				} 

				l.Instance(menu_chat_messages);

				l.Rewind();

				while (l.Iterate(m)) {
					sprintf(tmp, "%s: %s", m->name, m->message);

					sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, font, 0), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
					SDL_FillRect(sfc, 0, 0);
					print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, 0);

					t = new GLTile(sfc);
					t->set_hotspot(0, 0);

					if (strcmp(m->name, menu_local_computer.name) == 0) {
						t->draw(0.5, 0.5, 1, f, float(224), float(y), 0, 0, 1);
					} else {
						t->draw(1, 1, 1, f, float(224), float(y), 0, 0, 1);
					} 

					delete t;

					y += 10;
				} 
			}
		} 
	}


	if (menu_fading != 0) {
		{
			float f = 0;
			f = abs(menu_fading_ctnt) / float(MENU_CONSTANT);
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

