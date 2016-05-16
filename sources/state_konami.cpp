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

int F1SpiritApp::konami_cycle(KEYBOARDSTATE *k)
{
	if (state_cycle == 150) {
		Sound_play(Sound_create_sound("sound/braingames"));
	}

	if (state_cycle >= 500) {
		delete konami_image1;
		konami_image1 = 0;
		delete konami_image2;
		konami_image2 = 0;

		if (m_skip_intro_screens) {
			// FIXME: sneakily skip part of the title animation
			state_cycle = 0;
		}

		return APP_STATE_TITLE;
	} 

	if ((k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) ||
	        (k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE])) {
		m_skip_intro_screens = true;

		if (state_cycle < 450) {
			if (state_cycle < 50)
				state_cycle = 500 - state_cycle;
			else
				state_cycle = 450;
		}
	}

	return APP_STATE_KONAMI;
}

void F1SpiritApp::konami_draw(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	if (konami_image1 == 0)
		konami_image1 = new GLTile("graphics/konami1.jpg");

	if (konami_image2 == 0)
		konami_image2 = new GLTile("graphics/konami2.png");

	if (konami_image1 != 0) {
		if (state_cycle >= 0 && state_cycle < 50)
			konami_image1->draw((state_cycle)*0.02F, (state_cycle)*0.02F, (state_cycle)*0.02F, 1.0F);

		if (state_cycle >= 50 && state_cycle < 450)
			konami_image1->draw();

		if (state_cycle >= 450 && state_cycle < 500)
			konami_image1->draw((500 - state_cycle)*0.02F, (500 - state_cycle)*0.02F, (500 - state_cycle)*0.02F, 1.0F);
	} 

	if (konami_image2 != 0) {
		glPushMatrix();
		glTranslatef(0, 0, -1);

		if (state_cycle >= 100 && state_cycle < 200)
			konami_image2->draw(1.0F, 1.0F, 1.0F, (state_cycle - 100)*0.01F);

		if (state_cycle >= 200 && state_cycle < 450)
			konami_image2->draw();

		if (state_cycle >= 450 && state_cycle < 500)
			konami_image2->draw(1.0F, 1.0F, 1.0F, (500 - state_cycle)*0.02F);

		glPopMatrix();
	} 
} 

