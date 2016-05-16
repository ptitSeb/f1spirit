#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
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
#include "2DCMC.h"
#include "RoadPiece.h"
#include "track.h"
#include "CPlayer.h"
#include "CCar.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"
#include "randomc.h"

#include "debug.h"


#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

/*      GLOBAL VARIABLES INITIALIZATION:       */

char *application_name = "F-1 Spirit";
int application_version = 0;
int SCREEN_X = 640;
int SCREEN_Y = 480;
int g_stencil_bits = 0;
int N_SFX_CHANNELS = 16;
int COLOUR_DEPTH = 32;
int MAX_CONNECTIONS = 16;
bool sound = true;
bool fullscreen = false;
bool network = true;
int network_tcp_port = 32124;
int network_udp_port = 32125;
int LISTENING_TIME = 1;

TRanrotBGenerator *rg = 0;

/* Redrawing constant: */
const int REDRAWING_PERIOD = 20;

/* Frames per second counter: */
int frames_per_sec = 0;
int frames_per_sec_tmp = 0;
int init_time = 0;
bool show_fps = false;

/* F1Spirit console messages: */
bool show_console_msg = false;
char console_msg[80] = "";


/*      AUXILIAR FUNCTION DEFINITION:       */


SDL_Surface *initialization(int flags)
{
	const SDL_VideoInfo* info = 0;
	int bpp = 0;
	SDL_Surface *screen;

	rg = new TRanrotBGenerator(0);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Initializing SDL\n");
#endif

	if (SDL_Init(SDL_INIT_VIDEO | (sound ? SDL_INIT_AUDIO : 0) | SDL_INIT_JOYSTICK) < 0) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Video initialization failed: %s\n", SDL_GetError());
#endif

		return 0;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("SDL initialized\n");

#endif

	info = SDL_GetVideoInfo();

	if (!info) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Video query failed: %s\n", SDL_GetError());
#endif

		return 0;
	} 

	if (fullscreen) {
		bpp = COLOUR_DEPTH;
	} else {
		bpp = info->vfmt->BitsPerPixel;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Setting OpenGL attributes\n");

#endif

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("OpenGL attributes set\n");

#endif

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Initializing video mode\n");

#endif

	flags = SDL_OPENGL | flags;

	screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, bpp, flags);

	if (screen == 0) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Video mode set failed: %s\n", SDL_GetError());
#endif

		return 0;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Video mode initialized\n");

#endif

	SDL_WM_SetCaption(application_name, 0);

	SDL_WM_SetIcon(SDL_LoadBMP("graphics/f1sicon.bmp"), NULL);

	SDL_ShowCursor(SDL_DISABLE);

	if (sound) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Initializing Audio\n");
#endif

		N_SFX_CHANNELS = Sound_initialization(N_SFX_CHANNELS, 0);

#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("Audio initialized\n");
#endif

	} 

	// Network:
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Initializing SDL_net...\n");

#endif

	if (SDLNet_Init() == -1) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Error initializing SDL_net: %s.\n", SDLNet_GetError());
#endif

		network = false;
	} else {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("SDL_net initialized.\n");
#endif

		network = true;
	} 

	SDL_EnableUNICODE(1);

	glGetIntegerv(GL_STENCIL_BITS, &g_stencil_bits);

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("OpenGL stencil buffer bits: %i\n", g_stencil_bits);

#endif


	return screen;
} /* initialization */


void finalization()
{
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Finalizing SDL\n");
#endif

	if (network) {
		SDLNet_Quit();
	} 

	delete rg;

	rg = 0;

	free_auxiliar_menu_surfaces();

	if (sound)
		Sound_release();

	SDL_Quit();

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("SDL finalized\n");

#endif

} /* finalization */



#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char** argv) {
#endif

	SDL_Surface *screen_sfc;
	F1SpiritApp *game;
	KEYBOARDSTATE *k;

	int time, act_time;
	SDL_Event event;
	bool quit = false;
	bool need_to_redraw = true;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Application started\n");
#endif

	screen_sfc = initialization((fullscreen ? SDL_FULLSCREEN : 0));

	if (screen_sfc == 0)
		return 0;

	k = new KEYBOARDSTATE();

	game = new F1SpiritApp();

	if (fullscreen) {
		screen_sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));
		SDL_WM_SetCaption(application_name, 0);
		SDL_ShowCursor(SDL_DISABLE);
		reload_textures++;

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	} 

	time = init_time = SDL_GetTicks();

	while (!quit) {
		while ( SDL_PollEvent( &event ) ) {
			switch ( event.type ) {
					/* Keyboard event */

				case SDL_KEYDOWN:
#ifdef __APPLE__

					if (event.key.keysym.sym == SDLK_q) {
						SDLMod modifiers;
						modifiers = SDL_GetModState();

						if ((modifiers &KMOD_META) != 0) {
							quit = true;
						}
					}

#else
					if (event.key.keysym.sym == SDLK_F12) {
						quit = true;
					} 

#endif
					if (event.key.keysym.sym == SDLK_F10) {
						game->save_configuration("f1spirit.cfg");
						game->load_configuration("f1spirit.cfg");
					} 

#ifdef _WIN32
					if (event.key.keysym.sym == SDLK_F4) {
						SDLMod modifiers;

						modifiers = SDL_GetModState();

						if ((modifiers&KMOD_ALT) != 0)
							quit = true;
					} 

#endif
#ifdef __APPLE__
					if (event.key.keysym.sym == SDLK_f) {
						SDLMod modifiers;

						modifiers = SDL_GetModState();

						if ((modifiers&KMOD_META) != 0) {
							/* Toggle FULLSCREEN mode: */
							if (fullscreen)
								fullscreen = false;
							else
								fullscreen = true;

							screen_sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));

							SDL_WM_SetCaption(application_name, 0);

							SDL_ShowCursor(SDL_DISABLE);

							reload_textures++;

							SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
						}
					}

#else
					if (event.key.keysym.sym == SDLK_RETURN) {
						SDLMod modifiers;

						modifiers = SDL_GetModState();

						if ((modifiers&KMOD_ALT) != 0) {
							/* Toggle FULLSCREEN mode: */
							if (fullscreen)
								fullscreen = false;
							else
								fullscreen = true;

							screen_sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));

							SDL_WM_SetCaption(application_name, 0);

							SDL_ShowCursor(SDL_DISABLE);

							reload_textures++;

							SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

							SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
						}
					}

#endif

					if (event.key.keysym.sym == SDLK_f) {
						SDLMod modifiers;

						modifiers = SDL_GetModState();

						if ((modifiers&KMOD_ALT) != 0) {
							/* toggle FPS mode: */
							if (show_fps)
								show_fps = false;
							else
								show_fps = true;
						} 
					} 

					/* Keyboard event */
					SDL_keysym *ks;

					ks = new SDL_keysym();

					*ks = event.key.keysym;

					k->keyevents.Add(ks);

					break;

					/* SDL_QUIT event (window close) */

				case SDL_QUIT:
					quit = true;

					break;
			} 
		} 

		act_time = SDL_GetTicks();

		if (act_time - time >= REDRAWING_PERIOD) {
			int max_frame_step = 10;
			/*
			   frames_per_sec_tmp+=1;
			   if ((act_time-init_time)>=1000) {
			    frames_per_sec=frames_per_sec_tmp;
			    frames_per_sec_tmp=0;
			    init_time=act_time;
			   } // if
			*/

			do {
				time += REDRAWING_PERIOD;

				if ((act_time - time) > 10*REDRAWING_PERIOD)
					time = act_time;

				/* cycle */
				k->cycle();

				if (!game->cycle(k))
					quit = true;

				need_to_redraw = true;

				k->keyevents.Delete();

				act_time = SDL_GetTicks();

				max_frame_step--;
			} while (act_time - time >= REDRAWING_PERIOD && max_frame_step > 0);

		} 

		/* Redraw */
		if (need_to_redraw) {
			game->draw();
			need_to_redraw = false;
			frames_per_sec_tmp += 1;
		} 

		if ((act_time - init_time) >= 1000) {
			frames_per_sec = frames_per_sec_tmp;
			frames_per_sec_tmp = 0;
			init_time = act_time;
		} 

		SDL_Delay(1);

	} 


	delete k;

	k = 0;

	delete game;

	game = 0;

	Stop_playback();

	finalization();

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Application finished\n");

	close_debug_messages();

#endif

	return 0;
} /* main */
