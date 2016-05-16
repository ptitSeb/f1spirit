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

int F1SpiritApp::disclaimer_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle >= 250) {
		delete disclaimer_image;
		disclaimer_image = 0;

		if (m_skip_intro_screens) {
			// FIXME: sneakily skip part of the title animation
			state_cycle = 0;
			return APP_STATE_TITLE;
		}

		return APP_STATE_KONAMI;
	} 

	if ((k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) ||
	        (k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE])) {
		m_skip_intro_screens = true;

		if (state_cycle < 200) {
			state_cycle = 200;
		}
	}

	return APP_STATE_DISCLAIMER;
}

void F1SpiritApp::disclaimer_draw(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (disclaimer_image == 0)
		disclaimer_image = new GLTile("graphics/disclaimer.jpg");

	if (konami_image1 == 0)
		konami_image1 = new GLTile("graphics/konami1.jpg");

	if (konami_image2 == 0)
		konami_image2 = new GLTile("graphics/konami2.png");

	if (disclaimer_image != 0) {
		if (state_cycle >= 0 && state_cycle < 50)
			disclaimer_image->draw((state_cycle)*0.02F, (state_cycle)*0.02F, (state_cycle)*0.02F, 1.0F);

		if (state_cycle >= 50 && state_cycle < 200)
			disclaimer_image->draw();

		if (state_cycle >= 200 && state_cycle < 250)
			disclaimer_image->draw((250 - state_cycle)*0.02F, (250 - state_cycle)*0.02F, (250 - state_cycle)*0.02F, 1.0F);
	} 
} 

