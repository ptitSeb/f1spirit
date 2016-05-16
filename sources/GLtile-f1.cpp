#ifdef _WIN32
#include "windows.h"
#endif

#include "GL/gl.h"
#include "GL/glu.h"
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

				glBegin(GL_QUADS);
				glVertex3f( -part_x / 2.0F, -part_y / 2.0F, 0);
				glVertex3f( -part_x / 2.0F, part_y / 2.0F, 0);
				glVertex3f(part_x / 2.0F, part_y / 2.0F, 0);
				glVertex3f(part_x / 2.0F, -part_y / 2.0F, 0);
				glEnd();

				glDisable(GL_TEXTURE_2D);

			} else {
				glEnable(GL_COLOR_MATERIAL);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, tex[0]);

				glColor4f(1, 1, 1, 1);
				glNormal3f(0.0, 0.0, 1.0);

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

				glDisable(GL_TEXTURE_2D);
			} 

			glPopMatrix();

		} 
	} 

	glPopMatrix();

} 
