#ifdef _WIN32
#include "windows.h"
#endif

#ifdef HAVE_GLES
#include <GLES/gl.h>
#include <GLES/glu.h>
#else
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"
#include "SDL_image.h"

#include "auxiliar.h"
#include "GLTile.h"
#include "SDL_glutaux.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

void GLTile::f1_draw_effect1(int ax, int ay, int az, int h_tiles, int v_tiles, int timmer1, int timmer2, int time, float r, float g, float b)
{
	int i, j;
	int local_time = 0;
	float t_part_x, t_part_y;
	float part_x, part_y;

	if (nparts != 1)
		return ;

	if (textures_loaded != reload_textures)
		load_textures();

	glPushMatrix();

	glTranslatef(float(ax), float(ay), float(az));

	t_part_x = tex_coord_x[0] / h_tiles;

	t_part_y = tex_coord_y[0] / v_tiles;

	part_x = float(g_dx / h_tiles);

	part_y = float(g_dy / v_tiles);

	#ifdef HAVE_GLES
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	GLfloat tex1[2*4];
	GLfloat vtx[2*4];
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glTexCoordPointer(2, GL_FLOAT, 0, tex1);
	#endif

	for (i = 0;i < v_tiles;i++) {
		for (j = 0;j < h_tiles;j++) {
			local_time = time - (i + j) * timmer1;

			glPushMatrix();
			glTranslatef(part_x*(j + 0.5F) - hot_x, part_y*(i + 0.5F) - hot_y, 0);

			if (local_time > -timmer2 &&
			        local_time < timmer2) {
				glRotatef( 180 - 180*float(local_time + timmer2) / float(timmer2*2) , 0, 1, 0);
			} else {
				if (local_time < 0) {
					glRotatef(180, 0, 1, 0);
				} 
			} 

			if (local_time < 0) {
				glEnable(GL_COLOR_MATERIAL);

				glColor4f(r, g, b, 1);
				glNormal3f(0.0, 0.0, -1.0);

				#ifdef HAVE_GLES
				vtx[0*2+0] = -part_x / 2.0F; vtx[0*2+1] = -part_y / 2.0F;
				vtx[1*2+0] = -part_x / 2.0F; vtx[1*2+1] = part_y / 2.0F;
				vtx[2*2+0] = part_x / 2.0F; vtx[2*2+1] = part_y / 2.0F;
				vtx[3*2+0] = part_x / 2.0F; vtx[3*2+1] = -part_y / 2.0F;
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				#else
				glBegin(GL_QUADS);
				glVertex3f( -part_x / 2.0F, -part_y / 2.0F, 0);
				glVertex3f( -part_x / 2.0F, part_y / 2.0F, 0);
				glVertex3f(part_x / 2.0F, part_y / 2.0F, 0);
				glVertex3f(part_x / 2.0F, -part_y / 2.0F, 0);
				glEnd();
				glDisable(GL_TEXTURE_2D);
				#endif


			} else {
				glEnable(GL_COLOR_MATERIAL);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, tex[0]);

				glColor4f(1, 1, 1, 1);
				glNormal3f(0.0, 0.0, 1.0);

				#ifdef HAVE_GLES
				tex1[0*2+0] = t_part_x*j; 		tex1[0*2+1] = t_part_y*i;
				vtx[0*2+0] = -part_x / 2.0F; 	vtx[0*2+1] = -part_y / 2.0F;
				
				tex1[1*2+0] = t_part_x*j; 		tex1[1*2+1] = t_part_y*(i + 1);
				vtx[1*2+0] = -part_x / 2.0F; 	vtx[1*2+1] = part_y / 2.0F;
				
				tex1[2*2+0] = t_part_x*(j + 1); tex1[2*2+1] = t_part_y*(i + 1);
				vtx[2*2+0] = part_x / 2.0F; 	vtx[2*2+1] = part_y / 2.0F;
				
				tex1[3*2+0] = t_part_x*(j + 1); tex1[3*2+1] = t_part_y*i;
				vtx[3*2+0] = part_x / 2.0F; 	vtx[3*2+1] = -part_y / 2.0F;
				
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				#else
				glBegin(GL_QUADS);
				glTexCoord2f(t_part_x*j, t_part_y*i);
				glVertex3f( -part_x / 2.0F, -part_y / 2.0F, 0);

				glTexCoord2f(t_part_x*j, t_part_y*(i + 1));
				glVertex3f( -part_x / 2.0F, part_y / 2.0F, 0);

				glTexCoord2f(t_part_x*(j + 1), t_part_y*(i + 1));
				glVertex3f(part_x / 2.0F, part_y / 2.0F, 0);

				glTexCoord2f(t_part_x*(j + 1), t_part_y*i);
				glVertex3f(part_x / 2.0F, -part_y / 2.0F, 0);

				glEnd();
				#endif

				glDisable(GL_TEXTURE_2D);
			} 

			glPopMatrix();

		} 
	} 
	#ifdef HAVE_GLES
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	#endif

	glPopMatrix();

} 
