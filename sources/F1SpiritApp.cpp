#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#include "glut.h"
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_net.h>

#include <curl/curl.h>

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

/* External variables: */
extern int SCREEN_X;
extern int SCREEN_Y;
extern int COLOUR_DEPTH;
extern int MAX_CONNECTIONS;
extern bool sound;
extern int frames_per_sec;
extern bool show_fps;
extern bool fullscreen;
extern bool network;
extern int network_tcp_port;
extern int network_udp_port;

extern bool show_console_msg;
extern char console_msg[80];

extern char *f1spirit_replay_version;


F1SpiritApp::F1SpiritApp()
{
	int i;

	load_configuration("f1spirit.cfg");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Initializing F1-Spirit\n");
#endif

	state = APP_STATE_DISCLAIMER;

	state_cycle = 0;
	font = load_bmp_font("graphics/font-unicode-alpha.png", 0, 256);
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("font loaded: %p\n", font);
#endif

	small_font = load_bmp_font("graphics/smallfont-unicode-alpha.png", 0, 256);
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("small font loaded: %p\n", font);
#endif

	friendly_player = new CPlayerInfo();
	current_player = 0;
	player_filename[0] = 0;

	S_wrong = Sound_create_sound("sound/wrong");
	S_pause = Sound_create_sound("sound/pause");

	cheat_max_point = 0;

	m_skip_intro_screens = false;

	/* DISCLAIMER: */
	disclaimer_image = 0;

	/* KONAMI: */
	konami_image1 = 0;
	konami_image2 = 0;

	/* TITLE: */
	title_state = 0;
	title_parta = new GLTile("graphics/title1-a-alpha.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/title1-a-alpha.png loaded: %p\n", title_parta);
#endif

	title_parta->set_hotspot(153, 79);
	title_partb = new GLTile("graphics/title1-b-alpha.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/title1-b-alpha.png loaded: %p\n", title_partb);
#endif

	title_partb->set_hotspot(168, 29);
	title_partc = new GLTile("graphics/title1-c-alpha.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/title1-c-alpha.png loaded: %p\n", title_partc);
#endif

	title_partc->set_hotspot(170, 25);
	title_partd = new GLTile("graphics/title1-d-alpha.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/title1-d-alpha.png loaded: %p\n", title_partd);
#endif

	title_partd->set_hotspot(22, 6);
	title_complete = new GLTile("graphics/title2-alpha.png");
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/title2-alpha.png loaded: %p\n", title_complete);
#endif

	title_konami = 0;
	title_braingames = 0;
	title_space = 0;
	title_start = 0;
	title_game = 0;
	title_replay = 0;

	/* GAMESTART: */
	gamestart_1 = new GLTile("graphics/gamestart1.png");
	gamestart_1->set_smooth();
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/gamestart1.png: %p\n", gamestart_1);
#endif

	gamestart_2 = new GLTile("graphics/gamestart2.png");
	gamestart_2->set_smooth();
#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/gamestart2.png: %p\n", gamestart_2);
#endif

	/* DEMO: */
	/* ... */

	/* HISCORE: */
	blank_hiscores();
	load_hiscores();

	for (i = 0;i < 26;i++) {
		hiscore_table[i] = 0;
	}
	
	hiscore_table_regen = true;

	/* MENU: */
	menu_background = new GLTile("graphics/menu_background.jpg");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/carmodelw.jpg: %p\n", menu_background);

#endif

	menu_flag = new GLTile("graphics/flag0.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/flag0.png: %p\n", menu_flag);

#endif

	menu_title[0] = 0;
	menu_options[0] = 0;
	menu_title[1] = 0;
	
	menu_options[1] = 0;

	menu_current_menu = 0;
	menu_prev_nmenus = 0;

	menu_x[0] = 0;
	menu_x[1] = 0;

	menu_redefining_key = false;
	menu_force_rebuild_menu = false;
	menu_fading = 0;
	menu_fading_ctnt = 0;
	menu_editing_valid = true;
	menu_selected_track = -1;
	menu_selected_nplayers = 1;
	menu_selected_part = new int * [4];

	for (i = 0;i < 4;i++) {
		menu_selected_part[i] = new int[5];
	}
		
	menu_showing_points = 0;
	menu_showing_track = -1;
	menu_showing_track_timmer = 0;
	menu_showing_track_timmer2 = 0;
	menu_track_viewer = 0;
	menu_track = 0;
	menu_track_frame = 0;
	menu_points_frame = 0;
	menu_car_top[0] = 0;

	menu_car_side[0] = 0;
	menu_car_top[1] = 0;
	menu_car_side[1] = 0;
	menu_car_top[2] = 0;
	menu_car_side[2] = 0;
	menu_car_frame[0] = 0;
	menu_car_frame[1] = 0;
	menu_car_frame[2] = 0;
	menu_showing_car_type = -1;
	menu_showing_car = -1;
	menu_showing_car_timmer = 0;
	menu_info_frame = 0;
	menu_piece[0] = 0;
	menu_piece[1] = 0;
	menu_piece[2] = 0;
	menu_piece[3] = 0;
	menu_piece[4] = 0;
	menu_piece[5] = 0;
	
	menu_piece_frame[0] = 0;
	menu_piece_frame[1] = 0;
	menu_piece_frame[2] = 0;
	menu_piece_frame[3] = 0;
	menu_piece_frame[4] = 0;
	menu_piece_frame[5] = 0;
	
	menu_showing_piece_type = -1;
	menu_showing_piece = -1;
	menu_showing_piece_timmer = 0;
	menu_trackviewing_background = 0;
	menu_track_locked = false;
	menu_locked = new GLTile("graphics/locked.png");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("graphics/locked.png: %p\n", menu_locked);

#endif

	menu_design = 0;

	S_menu_move = Sound_create_sound("sound/menu_cursor");
	S_menu_select = Sound_create_sound("sound/menu_select");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("sound/menu_cursor: %p\n", S_menu_move);
	output_debug_message("sound/menu_select: %p\n", S_menu_select);

#endif

	menu_register_http = 0;
	menu_server_registered = false;
	menu_server_register_timmer = 0;
	menu_client_register_timmer = 0;
	menu_playerlist_frame = 0;
	menu_chat_frame = 0;
	menu_playerlist_timmer = 0;
	menu_chat_timmer = 0;
	menu_local_is_server = false;
	menu_multiplayer_n_enemycars = 0;
	menu_multiplayer_enemy_speed = 0;

	/* RACE: */
	race_game = 0;

	/* RACE RESULT: */
	/* ... */

	/* END SEQUENCE: */
	endsequence_returnstate = APP_STATE_MENU;

	/* REPLAY MANAGER: */
	replaymanager_frame1 = 0;
	replaymanager_frame2 = 0;
	replaymanager_game = 0;
	replaymanager_replay_playing = false;
	replaymanager_replay_viewport = 0;

	if (network) {
		menu_socket_set = SDLNet_AllocSocketSet(MAX_CONNECTIONS + 1);
		menu_local_computer.tcp_socket = 0;
		menu_local_computer.udp_socket = 0;
	} 

	menu_selected_server = 0;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("F1-Spirit initialization finished\n");

#endif

}

F1SpiritApp::~F1SpiritApp()
{
	int i;

	save_configuration("f1spirit.cfg");

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Releasing F1-Spirit\n");
#endif

	delete disclaimer_image;
	disclaimer_image = 0;

	delete konami_image1;
	konami_image1 = 0;
	delete konami_image2;
	konami_image2 = 0;


	SDL_FreeSurface(font);
	font = 0;
	SDL_FreeSurface(small_font);
	small_font = 0;

	delete title_parta;
	delete title_partb;
	delete title_partc;
	delete title_partd;
	delete title_complete;
	delete title_konami;
	delete title_braingames;
	delete title_space;
	delete title_start;

	delete gamestart_1;
	delete gamestart_2;

	if (current_player != 0 && current_player != friendly_player) {
		delete current_player;
	}

	current_player = 0;

	delete friendly_player;

	friendly_player = 0;


	for (i = 0;i < N_TRACKS;i++) {
		delete []hiscore_time_names[i];
		delete []hiscore_bestlap_names[i];
	}

	for (i = 0;i < 4;i++) {
		delete menu_selected_part[i];
		menu_selected_part[i] = 0;
	}

	delete []menu_selected_part;

	menu_selected_part = 0;

	delete menu_track;

	menu_track = 0;

	delete menu_track_frame;

	menu_track_frame = 0;

	delete menu_points_frame;

	menu_points_frame = 0;

	delete menu_track_viewer;

	menu_track_viewer = 0;

	delete menu_background;

	menu_background = 0;

	delete menu_flag;

	menu_flag = 0;

	for (i = 0;i < 3;i++) {
		delete menu_car_top[i];
		menu_car_top[i] = 0;
		delete menu_car_side[i];
		menu_car_side[i] = 0;
		delete menu_car_frame[i];
		menu_car_frame[i] = 0;
	}

	for (i = 0;i < 6;i++) {
		delete menu_piece[i];
		menu_piece[i] = 0;
		delete menu_piece_frame[i];
		menu_piece_frame[i] = 0;
	}

	delete menu_info_frame;

	menu_info_frame = 0;

	delete menu_design;

	menu_design = 0;

	delete []menu_title[0];

	menu_title[0] = 0;

	delete []menu_options[0];

	menu_options[0] = 0;

	delete []menu_title[1];

	menu_title[1] = 0;

	delete []menu_options[1];

	menu_options[1] = 0;

	if (menu_trackviewing_background != 0)
		delete menu_trackviewing_background;

	menu_trackviewing_background = 0;

	delete menu_locked;

	menu_locked = 0;

	Sound_delete_sound(S_menu_move);

	Sound_delete_sound(S_menu_select);

	if (menu_register_http != 0) {
		delete menu_register_http;
	}

	menu_register_http = 0;

	if (menu_playerlist_frame != 0) {
		delete menu_playerlist_frame;
	}

	menu_playerlist_frame = 0;

	if (menu_chat_frame != 0) {
		delete menu_chat_frame;
	}

	menu_chat_frame = 0;

	delete race_game;

	race_game = 0;

	delete replaymanager_frame1;

	replaymanager_frame1 = 0;

	delete replaymanager_frame2;

	replaymanager_frame2 = 0;

	delete replaymanager_game;

	replaymanager_game = 0;

	Sound_delete_sound(S_wrong);
	Sound_delete_sound(S_pause);

	if (network) {
		SDLNet_FreeSocketSet(menu_socket_set);
	}

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("F1-Spirit release finished\n");
#endif

	if (menu_available_servers.MemberRefP(menu_selected_server)) {
		menu_selected_server = 0;
	} else {
		delete menu_selected_server;
		menu_selected_server = 0;
	}
}


bool F1SpiritApp::cycle(KEYBOARDSTATE *k)
{
	int old_state = state;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	if (state_cycle == 0) {
		output_debug_message("First Cycle started for state %i...\n", state);
	}
	
#endif

	switch (state) {

		case APP_STATE_DISCLAIMER:
			state = disclaimer_cycle(k);
			break;

		case APP_STATE_KONAMI:
			state = konami_cycle(k);
			break;

		case APP_STATE_TITLE:
			state = title_cycle(k);
			break;

		case APP_STATE_GAMESTART:
			state = gamestart_cycle(k);
			break;

		case APP_STATE_HISCORE:
			state = hiscore_cycle(k);
			break;

		case APP_STATE_MENU:
			state = menu_cycle(k);
			break;

		case APP_STATE_RACE:
			state = race_cycle(k);
			break;

		case APP_STATE_RACE_RESULT:
			state = race_result_cycle(k);
			break;

		case APP_STATE_TRACKLOAD:
			state = trackload_cycle(k);
			break;

		case APP_STATE_ENDSEQUENCE:
			state = endsequence_cycle(k);
			break;

		case APP_STATE_REPLAYMANAGER:
			state = replaymanager_cycle(k);
			break;

		case APP_STATE_GAMEOPTIONS:
			state = gameoptions_cycle(k);
			break;

		default:
			return false;
	}

	if (old_state == state) {
		state_cycle++;
	} else {
		if (!((old_state == APP_STATE_GAMEOPTIONS && state == APP_STATE_RACE) || m_skip_intro_screens)) {
			state_cycle = 0;
		}

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("F1-Spirit state change: %i -> %i\n", old_state, state);

#endif

	}

	return true;
}

void F1SpiritApp::draw()
{
	float lightpos[4] = {0, 0, -1000, 0};
	float tmpls[4] = {1.0F, 1.0F, 1.0F, 1.0};
	float tmpld[4] = {1.0F, 1.0F, 1.0F, 1.0};
	float tmpla[4] = {1.0F, 1.0F, 1.0F, 1.0};
	float lightpos2[4] = {0, 0, 1000, 0};
	float tmpls2[4] = {1.0F, 1.0F, 1.0F, 1.0};
	float tmpld2[4] = {0.6F, 0.6F, 0.6F, 1.0};
	float tmpla2[4] = {0.2F, 0.2F, 0.2F, 1.0};
	float ratio;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	if (state_cycle == 0) {
		output_debug_message("First Drawing cycle started for state %i...\n", state);
	}

#endif

	/* Enable Lights, etc.: */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations

	glEnable(GL_LIGHT0);

	if (state != APP_STATE_TITLE &&
	        state != APP_STATE_HISCORE &&
	        state != APP_STATE_RACE_RESULT &&
	        state != APP_STATE_REPLAYMANAGER) {
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, tmpla);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, tmpld);
		glLightfv(GL_LIGHT0, GL_SPECULAR, tmpls);
	} else {
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos2);
		glLightfv(GL_LIGHT0, GL_AMBIENT, tmpla2);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, tmpld2);
		glLightfv(GL_LIGHT0, GL_SPECULAR, tmpls2);
	}

	glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);

	glShadeModel( GL_SMOOTH );

	glDisable( GL_CULL_FACE );

	glDisable( GL_SCISSOR_TEST );

	glDisable( GL_DEPTH_TEST );

	glPolygonMode(GL_FRONT, GL_FILL);

	glClearColor(0, 0, 0, 0.0);

	glViewport(0, 0, SCREEN_X, SCREEN_Y);

	ratio = (float)SCREEN_X / float(SCREEN_Y);

	glMatrixMode( GL_PROJECTION );

	glLoadIdentity( );

	gluPerspective( 30.0, ratio, 1.0, 1024.0 );

	gluLookAt(320, 240, -895.8F , 320, 240, 0, 0, -1, 0); /* for 640x480 better */

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);

	switch (state) {

		case APP_STATE_DISCLAIMER:
			disclaimer_draw();
			break;

		case APP_STATE_KONAMI:
			konami_draw();
			break;

		case APP_STATE_TITLE:
			title_draw();
			break;

		case APP_STATE_GAMESTART:
			gamestart_draw();
			break;

		case APP_STATE_HISCORE:
			hiscore_draw();
			break;

		case APP_STATE_MENU:
			menu_draw();
			break;

		case APP_STATE_RACE:
			race_draw();
			break;

		case APP_STATE_RACE_RESULT:
			race_result_draw();
			break;

		case APP_STATE_TRACKLOAD:
			trackload_draw();
			break;

		case APP_STATE_ENDSEQUENCE:
			endsequence_draw();
			break;

		case APP_STATE_REPLAYMANAGER:
			replaymanager_draw();
			break;

		case APP_STATE_GAMEOPTIONS:
			gameoptions_draw();
			break;
	}

	if (show_fps) {
		char tmp[80];

		sprintf(tmp, "VIDEO MEM: %.4gMB - FPS: %i", float(GLTile::get_memory_used()) / float(1024*1024), frames_per_sec);
		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)tmp, small_font, -1), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
		GLTile *t;
		print_left_bmp((unsigned char *)tmp, small_font, sfc, 0, 0, -1);

		t = new GLTile(sfc);
		t->set_hotspot(sfc->w, sfc->h);
		t->draw(626, 472, 0, 0, 1);
		delete t;
	}

	if (show_console_msg) {

		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, get_text_width_bmp((unsigned char *)console_msg, small_font, -1), small_font->h, 32, RMASK, GMASK, BMASK, AMASK);
		GLTile *t;
		print_left_bmp((unsigned char *)console_msg, small_font, sfc, 0, 0, -1);

		t = new GLTile(sfc);
		t->set_hotspot(0, sfc->h);
		t->draw(8, 472, 0, 0, 1);
		delete t;
	}
	
	glDisable(GL_BLEND);

	SDL_GL_SwapBuffers();
}

void F1SpiritApp::blank_hiscores(void)
{
	/* generate blank hiscores: */
	int j;
	hiscore_list.Delete();

	for (j = 0;j < N_TRACKS;j++) {
		hiscore_time[j] = 59999; /* 9 minues, 59 seconds, 99 hundreths */
		hiscore_time_names[j] = new char[5];
		strcpy(hiscore_time_names[j], "NONE");
		hiscore_bestlap[j] = 59999; /* 9 minues, 59 seconds, 99 hundreths */
		hiscore_bestlap_names[j] = new char[5];
		strcpy(hiscore_bestlap_names[j], "NONE");
	}
}

bool F1SpiritApp::load_hiscores(void)
{
	FILE *fp;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Loading HIGH SCORES\n");
#endif

	fp = f1open("highscores/highscores.dat", "rb", USERDATA);

	if (fp != 0) {
		/* load hiscores: */
		int i, k, l, n;
		HiScoreEntry *entry;

		load_int(fp, &n);

		for (i = 0;i < n;i++) {
			entry = new HiScoreEntry;
			load_int(fp, &(entry->points));
			load_int(fp, &(entry->totaltime));
			l = fgetc(fp);

			for (k = 0;k < l;k++)
				entry->name[k] = fgetc(fp);

			entry->name[k] = 0;

			hiscore_list.Add(entry);
		}

		for (i = 0;i < N_TRACKS;i++) {
			load_int(fp, &(hiscore_time[i]));
			l = fgetc(fp);
			delete []hiscore_time_names[i];
			hiscore_time_names[i] = new char[l + 1];

			for (k = 0;k < l;k++)
				hiscore_time_names[i][k] = fgetc(fp);

			hiscore_time_names[i][l] = 0;

			load_int(fp, &(hiscore_bestlap[i]));

			l = fgetc(fp);

			delete []hiscore_bestlap_names[i];

			hiscore_bestlap_names[i] = new char[l + 1];

			for (k = 0;k < l;k++)
				hiscore_bestlap_names[i][k] = fgetc(fp);

			hiscore_bestlap_names[i][l] = 0;
		}

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("HIGH SCORES loaded\n");

#endif

	} else {

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("HIGH SCORES do not exist, creating a blank table.\n");
#endif

		blank_hiscores();
		hiscores_from_players();
		save_hiscores();
	}

	return true;
}

bool F1SpiritApp::save_hiscores(void)
{
	FILE *fp;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Saving HIGH SCORES\n");
#endif

	fp = f1open("highscores/highscores.dat", "wb", USERDATA);

	if (fp != 0) {
		int i;
		unsigned int k;
		HiScoreEntry *entry;

		save_int(fp, hiscore_list.Length());

		for (i = 0;i < hiscore_list.Length();i++) {
			entry = hiscore_list[i];
			save_int(fp, entry->points);
			save_int(fp, entry->totaltime);
			fputc(strlen(entry->name), fp);

			for (k = 0;k < strlen(entry->name);k++)
				fputc(entry->name[k], fp);
		}

		for (i = 0;i < N_TRACKS;i++) {
			save_int(fp, hiscore_time[i]);
			fputc(strlen(hiscore_time_names[i]), fp);

			for (k = 0;k < strlen(hiscore_time_names[i]);k++)
				fputc(hiscore_time_names[i][k], fp);

			save_int(fp, hiscore_bestlap[i]);

			fputc(strlen(hiscore_bestlap_names[i]), fp);

			for (k = 0;k < strlen(hiscore_bestlap_names[i]);k++)
				fputc(hiscore_bestlap_names[i][k], fp);
		}

		fclose(fp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("HIGH SCORES saved\n");

#endif

		return true;
	}

	return false;
}

bool hiscores_order(HiScoreEntry *e1, HiScoreEntry *e2)
{
	if (e2->points > e1->points)
		return false;

	if (e2->points == e1->points &&
	        e2->totaltime < e1->totaltime)
		return false;

	return true;
}

void F1SpiritApp::add_hiscore_points(char *pname, int points, int time)
{
	List<HiScoreEntry> l;
	HiScoreEntry *entry;
	HiScoreEntry *found = 0;


	/* find entry: */
	l.Instance(hiscore_list);
	l.Rewind();

	while (l.Iterate(entry) && found == 0) {
		if (strcmp(entry->name, pname) == 0)
			found = entry;
	} 

	/* if found, delete it: */
	if (found != 0)
		hiscore_list.DeleteElement(found);

	/* add entry: */
	if (found) {
		entry = found;
		entry->points = points;
		entry->totaltime = time;
	} else {
		entry = new HiScoreEntry;
		strcpy(entry->name, pname);
		entry->points = points;
		entry->totaltime = time;
	} 

	hiscore_list.Add(entry);

	/* sort list: */
	hiscore_list.Sort(hiscores_order);

} 


void F1SpiritApp::add_hiscore_time(char *pname, int time, int race)
{

	if (hiscore_time[race] > time || hiscore_time[race] == 0) {
		hiscore_time_names[race] = new char[strlen(pname) + 1];
		strcpy(hiscore_time_names[race], pname);
		hiscore_time[race] = time;
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("New BEST TIME, track: %i, time: %i\n", race, time);
#endif

	} 

} 


void F1SpiritApp::add_hiscore_bestlap(char *pname, int time, int race)
{

	if (hiscore_bestlap[race] > time || hiscore_bestlap[race] == 0) {
		hiscore_bestlap_names[race] = new char[strlen(pname) + 1];
		strcpy(hiscore_bestlap_names[race], pname);
		hiscore_bestlap[race] = time;
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("New BEST LAP, track: %i, time: %i\n", race, time);
#endif

	} 

} 


void F1SpiritApp::hiscores_from_players(void)
{
	/* Create the player: */
	CPlayerInfo *pi;
	FILE *fp = 0;
	int i, j, totaltime;
	char filename[80];
	bool found = false;

	i = 0;

	do {
		pi = new CPlayerInfo();
		found = false;

		sprintf(filename, "players/player%i.dat", i);
		fp = f1open(filename, "rb", USERDATA);

		if (fp != 0) {
			found = true;
			pi->load(fp, N_TRACKS, f1spirit_replay_version);

			totaltime = 0;

			for (j = 0;j < N_TRACKS;j++) {
				totaltime += pi->get_time(j);

				if (pi->get_time(j))
					add_hiscore_time(pi->get_name(), pi->get_time(j), j);

				if (pi->get_bestlap(j))
					add_hiscore_bestlap(pi->get_name(), pi->get_bestlap(j), j);
			} 

			add_hiscore_points(pi->get_name(), pi->get_points(), totaltime);

			delete pi;

			fclose(fp);
		} 

		i++;
	} while (found);

} 




bool F1SpiritApp::load_configuration(char *file)
{
	FILE *fp;
	bool error = false;

	fp = f1open(file, "r", USERDATA);

	if (fp != 0) {
		int tmp;

		if (1 == fscanf(fp, "%i", &tmp)) {
			if (tmp == 0)
				fullscreen = false;
			else
				fullscreen = true;
		} else {
			error = true;
		} 

		fclose(fp);
	} else {
		error = true;
	} 

	if (error)
		save_configuration(file);

	return error;
} 


bool F1SpiritApp::save_configuration(char *file)
{
	FILE *fp;

	fp = f1open(file, "w", USERDATA);

	if (fp != 0) {
		fprintf(fp, "%i\n", (fullscreen ? 1 : 0));
		fclose(fp);
		return true;
	} 

	return false;
} 


bool F1SpiritApp::read_configuration_token(FILE *fp, char *token)
{
	int i = 0;
	int c;

	c = fgetc(fp);

	do {
		while (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			c = fgetc(fp);

		if (c == ';') {
			c = fgetc(fp);

			while (c != '\n' && !feof(fp))
				c = fgetc(fp);

			if (c == '\r')
				c = fgetc(fp);
		} 

		if (feof(fp))
			return false;
	} while (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ';');

	while (c != 0 && c != ' ' && c != '\t' && c != '\r' && c != '\n' && !feof(fp)) {
		token[i++] = c;
		c = fgetc(fp);
	} 

	token[i] = 0;

	if (i > 0)
		return true;

	return false;
} 



