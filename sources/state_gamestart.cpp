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

#define GAMESTART_FINISH 145

int F1SpiritApp::gamestart_cycle(KEYBOARDSTATE *k)
{
	/*
	    if ((k->keyboard[SDLK_ESCAPE] && !k->old_keyboard[SDLK_ESCAPE]) ||
	            (k->keyboard[SDLK_SPACE] && !k->old_keyboard[SDLK_SPACE])) {
	        if (state_cycle < GAMESTART_FINISH) {
	            if (state_cycle < 50)
	                state_cycle = GAMESTART_FINISH - state_cycle;
	            else
	                state_cycle = GAMESTART_FINISH;
	        } // if
	    } // if
	*/

	if (state_cycle >= GAMESTART_FINISH + 50)
		return APP_STATE_MENU;

	return APP_STATE_GAMESTART;
} 


void F1SpiritApp::gamestart_draw(void)
{
	// glClearColor(0.7F,0.7F,0.7F,1);
	glClearColor(0.0F, 0.0F, 0.0F, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glScalef(1.25F, 1.25F, 1.25F);

	{
		int i, j;

		for (i = 0;i < 3;i++) {
			for (j = 0;j < 4;j++) {
				if (((j + i) % 2) == 0) {
					gamestart_1->f1_draw_effect1(j*128, i*128, 0, 4, 4, 8, 24, state_cycle - (j + i*4)*8, 0.8F, 0.8F, 0.8F);
				} else {
					gamestart_2->f1_draw_effect1(j*128, i*128, 0, 4, 4, 8, 24, state_cycle - (j + i*4)*8, 0, 0, 0);
				} 
			} 
		} 
	}

	glPopMatrix();

	if (state_cycle < 25) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = (25 - state_cycle) / 25.0F;
			glColor4f(0, 0, 0, f);
		}

		glNormal3f(0.0, 0.0, -1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -8);
		glVertex3f(0, 480, -8);
		glVertex3f(640, 480, -8);
		glVertex3f(640, 0, -8);
		glEnd();
	} 

	if (state_cycle >= GAMESTART_FINISH) {
		glEnable(GL_COLOR_MATERIAL);

		{
			float f = 0;
			f = (state_cycle - GAMESTART_FINISH) / 25.0F;
			glColor4f(0, 0, 0, f);
		}

		glNormal3f(0.0, 0.0, -1.0);

		glBegin(GL_QUADS);
		glVertex3f(0, 0, -8);
		glVertex3f(0, 480, -8);
		glVertex3f(640, 480, -8);
		glVertex3f(640, 0, -8);
		glEnd();
	} 

} 

