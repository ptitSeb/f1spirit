#ifdef _WIN32
#include "windows.h"
#include "glut.h"
#else
#include <sys/time.h>
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
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
#include "randomc.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif


#define SEMAPHORE_TIMMER 64


extern int N_SFX_CHANNELS, SCREEN_X, SCREEN_Y;
extern bool sound;

extern TRanrotBGenerator *rg;


void F1SpiritGame::draw_nightmode_nostencil(PlayerCCar *v)
{
	glEnable( GL_DEPTH_TEST );
	glClear(GL_DEPTH_BUFFER_BIT);

	/* Draw the car lights: */
	{
		PlayerCCar *pc;
		List<PlayerCCar> pc_l;
		EnemyCCar *ec;
		List<EnemyCCar> ec_l;

		glPushMatrix();
		glScalef(v->c_z, v->c_z, v->c_z);

		glColor4f(0, 0, 0, 0.0);

		pc_l.Instance(player_cars);
		pc_l.Rewind();

		while (pc_l.Iterate(pc)) {
			if (pc->car->state != 2) {
				glPushMatrix();
				glTranslatef(pc->car->get_x() - v->c_x, pc->car->get_y() - v->c_y, 0);
				glRotatef(pc->car->get_a() - 90, 0, 0, 1);

				glBegin(GL_TRIANGLE_FAN);
				glVertex3f(8, 8, -10);
				glVertex3f(8, -8, -10);

				glVertex3f(80, -52, -10);
				glVertex3f(95, -50, -10);
				glVertex3f(108, -40, -10);
				glVertex3f(110, -25, -10);
				glVertex3f(108, -10, -10);
				glVertex3f(100, 0, -10);
				glVertex3f(108, 10, -10);
				glVertex3f(110, 25, -10);
				glVertex3f(108, 40, -10);
				glVertex3f(95, 50, -10);
				glVertex3f(80, 52, -10);
				glEnd();

				glPopMatrix();
			} // if
		} 

		ec_l.Instance(enemy_cars);

		ec_l.Rewind();

		while (ec_l.Iterate(ec)) {
			if (pc->car->state != 2) {
				glPushMatrix();
				glTranslatef(ec->car->get_x() - v->c_x, ec->car->get_y() - v->c_y, 0);
				glRotatef(ec->car->get_a() - 90, 0, 0, 1);

				glBegin(GL_TRIANGLE_FAN);
				glVertex3f(8, 8, -10);
				glVertex3f(8, -8, -10);

				glVertex3f(40, -26, -10);
				glVertex3f(50, -25, -10);
				glVertex3f(54, -20, -10);
				glVertex3f(55, -12.5f, -10);
				glVertex3f(54, -5, -10);
				glVertex3f(50, 0, -10);
				glVertex3f(54, 5, -10);
				glVertex3f(55, 12.5F, -10);
				glVertex3f(54, 20, -10);
				glVertex3f(50, 25, -10);
				glVertex3f(40, 26, -10);
				glEnd();

				glPopMatrix();
			} // if
		} 

		glPopMatrix();

	}

	/* Darken the non-illuminated region: */
	{
		glColor4f(0, 0, 0, 0.75F);
		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glVertex3f( -1000, -1000, -5);
		glVertex3f( -1000, 1000, -5);
		glVertex3f(1000, 1000, -5);
		glVertex3f(1000, -1000, -5);
		glEnd();
	}

	glDisable( GL_DEPTH_TEST );
} 


void F1SpiritGame::draw_nightmode_stencil(PlayerCCar *v)
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	/* Draw the car lights in the stencil buffer: */
	{
		int i;
		PlayerCCar *pc;
		List<PlayerCCar> pc_l;
		EnemyCCar *ec;
		List<EnemyCCar> ec_l;

		glClearStencil(0);
		glClear( GL_STENCIL_BUFFER_BIT );
		glEnable( GL_STENCIL_TEST );
		glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
		glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

		glPushMatrix();
		glScalef(v->c_z, v->c_z, v->c_z);

		glColor4f(0, 0, 0, 0.0);

		pc_l.Instance(player_cars);
		pc_l.Rewind();

		while (pc_l.Iterate(pc)) {
			if (pc->car->state != 2) {
				glPushMatrix();
				glTranslatef(pc->car->get_x() - v->c_x, pc->car->get_y() - v->c_y, 0);
				glRotatef(pc->car->get_a() - 90, 0, 0, 1);

				for (i = 0;i < 16;i++) {
					float f = i * 0.0625f;
					float MAGNIFYING = 48.0f;
					glBegin(GL_TRIANGLE_FAN);
					glVertex3f(10, 8 + f*4, -4);
					glVertex3f(10, -8 + f*4, -4);

					glVertex3f(80 + f*MAGNIFYING*0.75, -52 - f*MAGNIFYING*0.5f, -5);
					glVertex3f(95 + f*MAGNIFYING, -50 - f*MAGNIFYING*0.5f, -5);
					glVertex3f(105 + f*MAGNIFYING, -40 - f*MAGNIFYING*0.375f, -5);
					glVertex3f(110 + f*MAGNIFYING, -25 - f*MAGNIFYING*0.25f, -5);
					glVertex3f(108 + f*MAGNIFYING, -10 - f*MAGNIFYING*0.125f, -5);
					glVertex3f(100 + f*MAGNIFYING, 0, -5);
					glVertex3f(108 + f*MAGNIFYING, 10 + f*MAGNIFYING*0.125f, -5);
					glVertex3f(110 + f*MAGNIFYING, 25 + f*MAGNIFYING*0.25f, -5);
					glVertex3f(105 + f*MAGNIFYING, 40 + f*MAGNIFYING*0.375f, -5);
					glVertex3f(95 + f*MAGNIFYING, 50 + f*MAGNIFYING*0.5f, -5);
					glVertex3f(80 + f*MAGNIFYING*0.75, 52 + f*MAGNIFYING*0.5, -5);
					glEnd();
				} // for

				glPopMatrix();
			} // if

		} 

		ec_l.Instance(enemy_cars);

		ec_l.Rewind();

		while (ec_l.Iterate(ec)) {
			if (pc->car->state != 2) {
				glPushMatrix();
				glTranslatef(ec->car->get_x() - v->c_x, ec->car->get_y() - v->c_y, 0);
				glRotatef(ec->car->get_a() - 90, 0, 0, 1);

				for (i = 0;i < 16;i++) {
					float f = i * 0.0625f;
					float MAGNIFYING = 24.0f;
					glBegin(GL_TRIANGLE_FAN);
					glVertex3f(10, 8 + f*4, -4);
					glVertex3f(10, -8 + f*4, -4);

					glVertex3f(40 + f*MAGNIFYING*0.75, -26 - f*MAGNIFYING*0.5f, -5);
					glVertex3f(48 + f*MAGNIFYING, -25 - f*MAGNIFYING*0.5f, -5);
					glVertex3f(53 + f*MAGNIFYING, -20 - f*MAGNIFYING*0.375f, -5);
					glVertex3f(55 + f*MAGNIFYING, -13 - f*MAGNIFYING*0.25f, -5);
					glVertex3f(54 + f*MAGNIFYING, -5 - f*MAGNIFYING*0.125f, -5);
					glVertex3f(50 + f*MAGNIFYING, 0, -5);
					glVertex3f(54 + f*MAGNIFYING, 5 + f*MAGNIFYING*0.125f, -5);
					glVertex3f(55 + f*MAGNIFYING, 13 + f*MAGNIFYING*0.25f, -5);
					glVertex3f(53 + f*MAGNIFYING, 20 + f*MAGNIFYING*0.375f, -5);
					glVertex3f(48 + f*MAGNIFYING, 25 + f*MAGNIFYING*0.5f, -5);
					glVertex3f(40 + f*MAGNIFYING*0.75, 26 + f*MAGNIFYING*0.5, -5);
					glEnd();
				} // if

				glPopMatrix();
			} // if
		} 

		glPopMatrix();

	}

	/* Darken the non-illuminated region: */
	{
//		float darkness[]={0.5F,0.5F,0.25F};
//		float darkness[]={0.75f, 0.7f,0.6f,0.5f,0.2f, 0.15f,0.1f,0.05f,0.0f};
//		float darkness[]={0.4f, 0.35f,0.3f,0.25f,0.2f, 0.15f,0.1f,0.05f,0.0f};
//		float darkness[]={0.8f, 0.7f,0.6f,0.5f,0.4f, 0.3f,0.2f,0.1f,0.0f};
		int i;
		glNormal3f(0.0, 0.0, 1.0);

		for (i = 32;i >= 0;i--) {
			glColor4f(0, 0, 0, (32 - i)*0.025f);
			glStencilFunc( GL_EQUAL, i, 0xFF );
			glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
			glBegin(GL_QUADS);
			glVertex3f(-1000, -1000, -4);
			glVertex3f(-1000, 1000, -4);
			glVertex3f(1000, 1000, -4);
			glVertex3f(1000, -1000, -4);
			glEnd();
		} // if
	}

	glPopAttrib();
} 


void F1SpiritGame::draw_rain(PlayerCCar *v, float rain_strength)
{
	float v1 = 1.5f, v2 = 3.0f;
	float f;
	RainDrop *rd;

	glPushMatrix();
	glScalef(v->c_z, v->c_z, v->c_z);

	glNormal3f(0.0, 0.0, 1.0);

	m_rain_drops.Rewind();

	while (m_rain_drops.Iterate(rd)) {
		if (rd->z < 0) {
			// Draws a rain drop:
			glColor4f(0.75f, 0.75f, 1.0f, 0.5f);
			glBegin(GL_LINES);
			glVertex3f(rd->x - v->c_x, rd->y - v->c_y, rd->z);
			glVertex3f(rd->x - rd->inc_x*2 - v->c_old_x, rd->y - rd->inc_y*2 - v->c_old_y, rd->z - rd->inc_z*2);
			glEnd();
		} else {
			// Rain splash
			f = rd->z / 500.0f;

			v1 = 0.5f * (f + 0.5f);
			v2 = 1.5f * (f + 0.5f);

			f = (1.0f - f) * 0.5f;
			glColor4f(0.05f, 0.05f, 0.1f, f);

			glBegin(GL_TRIANGLE_FAN);

			glVertex3f(rd->x - v->c_x, rd->y - v->c_y - v2, 0);
			glVertex3f(rd->x - v->c_x + v2, rd->y - v->c_y - v1, 0);
			glVertex3f(rd->x - v->c_x + v2, rd->y - v->c_y + v1, 0);
			glVertex3f(rd->x - v->c_x, rd->y - v->c_y + v2, 0);
			glVertex3f(rd->x - v->c_x - v2, rd->y - v->c_y + v1, 0);
			glVertex3f(rd->x - v->c_x - v2, rd->y - v->c_y - v1, 0);

			glEnd();
		} // if
	} // while

	glPopMatrix();

	glNormal3f(0.0, 0.0, 1.0);

	glColor4f(0, 0, 0, rain_strength*0.25f);

	glBegin(GL_QUADS);

	glVertex3f(-1000, -1000, -4);

	glVertex3f(-1000, 1000, -4);

	glVertex3f(1000, 1000, -4);

	glVertex3f(1000, -1000, -4);

	glEnd();


} 


// quick random for rain:
#define RAIN_RANDOM_SIZE	99999
#define RAIN_DROPS_PER_CYCLE	800
#define RAIN_DROPS_SPAN	700

bool quick_rain_random_generated = false;
float quick_random_a[RAIN_RANDOM_SIZE];
float quick_random_b[RAIN_RANDOM_SIZE];
int quick_rain_random_index = 0;

RainDrop *to_delete_buffer[RAIN_DROPS_PER_CYCLE*10];
int to_delete_index;



void F1SpiritGame::cycle_rain(PlayerCCar *v, float rain_strength)
{
	int i;
	RainDrop *rd;
	PlayerCCar *pc;
	List<PlayerCCar> pc_l;

	if (!quick_rain_random_generated) {
		for (i = 0;i < RAIN_RANDOM_SIZE;i++) {
			quick_random_a[i] = rg->IRandom(0, RAIN_DROPS_SPAN * 2);
			quick_random_b[i] = rg->IRandom(0, 20);
		} // for

		quick_rain_random_generated = true;
	} // if

	to_delete_index = 0;

	m_rain_drops.Rewind();

	while (m_rain_drops.Iterate(rd)) {
		rd->z += rd->inc_z;

		if (rd->z < 0) {
			rd->x += rd->inc_x;
			rd->y += rd->inc_y;
		} // if

		if (rd->z == 0) {
			rd->z = quick_random_b[quick_rain_random_index++] * 20;

			if (quick_rain_random_index >= RAIN_RANDOM_SIZE) quick_rain_random_index = 0;
		} // if

		if (rd->z >= 400) {
			to_delete_buffer[to_delete_index++] = rd;
		} // if
	} // while

	pc_l.Instance(player_cars);

	pc_l.Rewind();

	while (pc_l.Iterate(pc)) {
		for (i = 0;i < RAIN_DROPS_PER_CYCLE*rain_strength;i++) {
			if (to_delete_index > 0) {
				// Reuse a previous drop if available:
				rd = to_delete_buffer[--to_delete_index];
			} else {
				rd = new RainDrop();
				m_rain_drops.Add(rd);
			} // if

			rd->x = pc->car->get_x() + quick_random_a[quick_rain_random_index++] - RAIN_DROPS_SPAN;

			if (quick_rain_random_index >= RAIN_RANDOM_SIZE) quick_rain_random_index = 0;

			rd->y = pc->car->get_y() + quick_random_a[quick_rain_random_index++] - RAIN_DROPS_SPAN;

			if (quick_rain_random_index >= RAIN_RANDOM_SIZE) quick_rain_random_index = 0;

			rd->z = -200;

			rd->inc_x = 2;

			rd->inc_y = 2;

			rd->inc_z = 20;
		} // for
	} // while


	while (to_delete_index > 0) {
		rd = to_delete_buffer[--to_delete_index];
		m_rain_drops.DeleteElement(rd);
		delete rd;
	} // while
} 


