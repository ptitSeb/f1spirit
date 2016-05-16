#ifdef _WIN32
#include "windows.h"
#endif

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"

#include "auxiliar.h"
#include "2DCMC.h"
#include "GLTile.h"
#include "SDL_glutaux.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

int reload_textures = 0;


int GLTile::memory_used = 0;


GLTile::GLTile()
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;
} 


GLTile::GLTile(char *fname)
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;
	set
	(fname);
} 


GLTile::GLTile(SDL_Surface *sfc)
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;
	set
	(sfc);
} 


GLTile::GLTile(char *fname, int ax, int ay, int adx, int ady)
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;

	{
		SDL_Surface *sfc = IMG_Load(fname);

		if (sfc != 0) {
			SDL_Surface *sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, adx, ady, 32, RMASK, GMASK, BMASK, AMASK);
			SDL_Rect r;

			r.x = ax;
			r.y = ay;
			r.w = adx;
			r.h = ady;
			SDL_SetAlpha(sfc, 0, 0);
			SDL_BlitSurface(sfc, &r, sfc2, 0);
			set
			(sfc2);
			SDL_FreeSurface(sfc);
		} 
	}
} 


GLTile::GLTile(SDL_Surface *sfc, int ax, int ay, int adx, int ady)
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;

	if (sfc != 0) {
		SDL_Surface *sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, adx, ady, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_Rect r;

		r.x = ax;
		r.y = ay;
		r.w = adx;
		r.h = ady;
		SDL_SetAlpha(sfc, 0, 0);
		SDL_BlitSurface(sfc, &r, sfc2, 0);
		set
		(sfc2);
		//  SDL_FreeSurface(sfc2);
	} 
} 


GLTile::GLTile(SDL_Surface **sfcs, int *start_x, int *start_y, int nsfcs, int adx, int ady)
{

	int i, k, l;
	bool *empty;
	bool some_empty;
	int n_non_empty = 0;

	clamp = false;
	smooth = false;

	cmc = 0;

	empty = new bool[nsfcs];
	some_empty = false;

	/* Determine which parts are empty: */

	for (i = 0;i < nsfcs;i++) {
		empty[i] = true;

		for (k = 0;k < sfcs[i]->h && empty[i];k++) {
			for (l = 0;l < sfcs[i]->w && empty[i];l++) {
				if (empty[i] && getpixel(sfcs[i], l, k) != 0) {
					empty[i] = false;
					n_non_empty++;
				} 
			} 
		} 

		if (empty[i])
			some_empty = true;
	} 


	if (some_empty) {

		/* Create textures: */
		nparts = n_non_empty;
		tile = new SDL_Surface * [nparts];
		x = new int[nparts];
		y = new int[nparts];
		dx = new int[nparts];
		dy = new int[nparts];
		tex_coord_x = new float[nparts];
		tex_coord_y = new float[nparts];
		tex = new GLuint[nparts];

		g_dx = adx;
		g_dy = ady;
		hot_x = 0;
		hot_y = 0;

		k = 0;

		for (i = 0;i < nsfcs;i++) {
			if (!empty[i]) {

				tile[k] = SDL_CreateRGBSurface(SDL_SWSURFACE, sfcs[i]->w, sfcs[i]->h, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_SetAlpha(sfcs[i], 0, 0);
				SDL_BlitSurface(sfcs[i], 0, tile[k], 0);

				x[k] = start_x[i];
				y[k] = start_y[i];
				dx[k] = sfcs[i]->w;
				dy[k] = sfcs[i]->h;
				tex[k] = createTexture(tile[k], &(tex_coord_x[k]), &(tex_coord_y[k]));

				if (tex_coord_x[k] != 0 && tex_coord_y[k] != 0)
					memory_used += int((dx[k] * dy[k] * 4) / (tex_coord_x[k] * tex_coord_y[k]));

				k++;
			} 
		} 
	} else {

		nparts = 0;

		g_dx = 0;
		g_dy = 0;
		hot_x = 0;
		hot_y = 0;

		tile = 0;
		x = 0;
		y = 0;
		dx = 0;
		dy = 0;
		tex_coord_x = 0;
		tex_coord_y = 0;
		tex = 0;

	} 

	delete []empty;

} 




GLTile::GLTile(int ax, int ay, int adx, int ady)
{
	nparts = 0;

	clamp = false;
	smooth = false;

	g_dx = 0;
	g_dy = 0;
	hot_x = 0;
	hot_y = 0;

	tile = 0;
	x = 0;
	y = 0;
	dx = 0;
	dy = 0;
	tex_coord_x = 0;
	tex_coord_y = 0;
	tex = 0;

	cmc = 0;

	{
		int i;
		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, adx, ady, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_Surface *sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, adx, ady, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_Rect r1, r2;

		glReadPixels(ax, ay, adx, ady, GL_RGBA, GL_UNSIGNED_BYTE, sfc->pixels);
		surface_shifter(sfc, 0, 0, 0, 255, 0);
		SDL_SetAlpha(sfc, 0, 0);

		r1.x = 0;
		r1.w = adx;
		r1.h = 1;
		r2.x = 0;
		r2.w = adx;
		r2.h = 1;

		for (i = 0;i < ady;i++) {
			r1.y = i;
			r2.y = (ady - 1) - i;
			SDL_BlitSurface(sfc, &r1, sfc2, &r2);
		} 

		set
		(sfc2);

		SDL_FreeSurface(sfc);
	}
} 




GLTile::~GLTile()
{
	free();
}  


void GLTile::free(void)
{
	if (nparts != 0) {
		int i;

		for (i = 0;i < nparts;i++) {
			if (tile[i] != 0) {
				SDL_FreeSurface(tile[i]);
				tile[i] = 0;
				glDeleteTextures(1, &(tex[i]));

				if (tex_coord_x[i] != 0 && tex_coord_y[i] != 0)
					memory_used -= int((dx[i] * dy[i] * 4) / (tex_coord_x[i] * tex_coord_y[i]));
			} 
		} 

		delete []tile;

		delete []x;

		delete []y;

		delete []dx;

		delete []dy;

		delete []tex_coord_x;

		delete []tex_coord_y;

		delete []tex;
	} 

	nparts = 0;

	g_dx = 0;

	g_dy = 0;

	hot_x = 0;

	hot_y = 0;

	tile = 0;

	x = 0;

	y = 0;

	dx = 0;

	dy = 0;

	tex_coord_x = 0;

	tex_coord_y = 0;

	tex = 0;

	delete cmc;

	cmc = 0;
} // GLTile::free


void GLTile::set
(char *fname)
{
	free();

	clamp = false;
	smooth = false;

	nparts = 1;
	tile = new SDL_Surface * [1];
	tile[0] = IMG_Load(fname);

	x = new int[1];
	y = new int[1];
	dx = new int[1];
	dy = new int[1];
	tex_coord_x = new float[1];
	tex_coord_y = new float[1];
	tex = new GLuint[1];

	if (tile[0] != 0) {
		g_dx = tile[0]->w;
		g_dy = tile[0]->h;
		x[0] = 0;
		y[0] = 0;
		dx[0] = tile[0]->w;
		dy[0] = tile[0]->h;
		textures_loaded = reload_textures;
		tex[0] = createTexture(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

		if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
			memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
	} else {
		x[0] = 0;
		y[0] = 0;
		dx[0] = 0;
		dy[0] = 0;
		tex_coord_x[0] = 0;
		tex_coord_y[0] = 0;
		tex[0] = (GLuint) - 1;
	} 

	compute_cmc();

} 


void GLTile::set
(SDL_Surface *sfc)
{
	free();

	clamp = false;
	smooth = false;

	if (sfc != 0) {
		nparts = 1;
		tile = new SDL_Surface * [1];
		tile[0] = sfc;

		x = new int[1];
		y = new int[1];
		dx = new int[1];
		dy = new int[1];
		tex_coord_x = new float[1];
		tex_coord_y = new float[1];
		tex = new GLuint[1];

		if (tile[0] != 0) {
			g_dx = tile[0]->w;
			g_dy = tile[0]->h;
			x[0] = 0;
			y[0] = 0;
			dx[0] = tile[0]->w;
			dy[0] = tile[0]->h;
			textures_loaded = reload_textures;
			tex[0] = createTexture(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

			if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
				memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
		} else {
			x[0] = 0;
			y[0] = 0;
			dx[0] = 0;
			dy[0] = 0;
			tex_coord_x[0] = 0;
			tex_coord_y[0] = 0;
			tex[0] = (GLuint) - 1;
		} 
	} 

	compute_cmc();
} 


void GLTile::set_repeat(void)
{
	int i;

	clamp = false;
	smooth = false;

	textures_loaded = reload_textures;

	for (i = 0;i < nparts;i++) {
		tex[i] = createTexture(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
	} 

} 


void GLTile::set_clamp(void)
{
	int i;

	clamp = true;
	smooth = false;

	textures_loaded = reload_textures;

	for (i = 0;i < nparts;i++) {
		tex[i] = createTextureClamp(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
	} 

} 


void GLTile::set_smooth(void)
{
	int i;

	clamp = false;
	smooth = true;

	textures_loaded = reload_textures;

	for (i = 0;i < nparts;i++) {
		tex[i] = createTextureSmooth(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
	} 

} 


void GLTile::set_smooth(char *fname)
{
	free();

	clamp = false;
	smooth = true;

	nparts = 1;
	tile = new SDL_Surface * [1];
	tile[0] = IMG_Load(fname);

	x = new int[1];
	y = new int[1];
	dx = new int[1];
	dy = new int[1];
	tex_coord_x = new float[1];
	tex_coord_y = new float[1];
	tex = new GLuint[1];

	if (tile[0] != 0) {
		g_dx = tile[0]->w;
		g_dy = tile[0]->h;
		x[0] = 0;
		y[0] = 0;
		dx[0] = tile[0]->w;
		dy[0] = tile[0]->h;
		textures_loaded = reload_textures;
		tex[0] = createTextureSmooth(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

		if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
			memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
	} else {
		x[0] = 0;
		y[0] = 0;
		dx[0] = 0;
		dy[0] = 0;
		tex_coord_x[0] = 0;
		tex_coord_y[0] = 0;
		tex[0] = (GLuint) - 1;
	} 

	compute_cmc();

} 


void GLTile::set_smooth(SDL_Surface *sfc)
{
	free();

	clamp = false;
	smooth = true;

	if (sfc != 0) {
		nparts = 1;
		tile = new SDL_Surface * [1];
		tile[0] = sfc;

		x = new int[1];
		y = new int[1];
		dx = new int[1];
		dy = new int[1];
		tex_coord_x = new float[1];
		tex_coord_y = new float[1];
		tex = new GLuint[1];

		if (tile[0] != 0) {
			g_dx = tile[0]->w;
			g_dy = tile[0]->h;
			x[0] = 0;
			y[0] = 0;
			dx[0] = tile[0]->w;
			dy[0] = tile[0]->h;
			textures_loaded = reload_textures;
			tex[0] = createTextureSmooth(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

			if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
				memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
		} else {
			x[0] = 0;
			y[0] = 0;
			dx[0] = 0;
			dy[0] = 0;
			tex_coord_x[0] = 0;
			tex_coord_y[0] = 0;
			tex[0] = (GLuint) - 1;
		} 
	} 

	compute_cmc();
} 


void GLTile::set_clamp(char *fname)
{
	free();

	clamp = true;
	smooth = false;

	nparts = 1;
	tile = new SDL_Surface * [1];
	tile[0] = IMG_Load(fname);

	x = new int[1];
	y = new int[1];
	dx = new int[1];
	dy = new int[1];
	tex_coord_x = new float[1];
	tex_coord_y = new float[1];
	tex = new GLuint[1];

	if (tile[0] != 0) {
		g_dx = tile[0]->w;
		g_dy = tile[0]->h;
		x[0] = 0;
		y[0] = 0;
		dx[0] = tile[0]->w;
		dy[0] = tile[0]->h;
		textures_loaded = reload_textures;
		tex[0] = createTextureClamp(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

		if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
			memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
	} else {
		x[0] = 0;
		y[0] = 0;
		dx[0] = 0;
		dy[0] = 0;
		tex_coord_x[0] = 0;
		tex_coord_y[0] = 0;
		tex[0] = (GLuint) - 1;
	} 

	compute_cmc();

} 


void GLTile::set_clamp(SDL_Surface *sfc)
{
	free();

	clamp = true;
	smooth = false;

	if (sfc != 0) {
		nparts = 1;
		tile = new SDL_Surface * [1];
		tile[0] = sfc;

		x = new int[1];
		y = new int[1];
		dx = new int[1];
		dy = new int[1];
		tex_coord_x = new float[1];
		tex_coord_y = new float[1];
		tex = new GLuint[1];

		if (tile[0] != 0) {
			g_dx = tile[0]->w;
			g_dy = tile[0]->h;
			x[0] = 0;
			y[0] = 0;
			dx[0] = tile[0]->w;
			dy[0] = tile[0]->h;
			textures_loaded = reload_textures;
			tex[0] = createTextureClamp(tile[0], &(tex_coord_x[0]), &(tex_coord_y[0]));

			if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
				memory_used += int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));
		} else {
			x[0] = 0;
			y[0] = 0;
			dx[0] = 0;
			dy[0] = 0;
			tex_coord_x[0] = 0;
			tex_coord_y[0] = 0;
			tex[0] = (GLuint) - 1;
		} 
	} 

	compute_cmc();
} 



void GLTile::set_hotspot(int hx, int hy)
{
	hot_x = hx;
	hot_y = hy;

	compute_cmc();
} 


void GLTile::compute_cmc(void)
{
	float x[2] = {0, 0}, y[2] = {0, 0};
	bool first = true;
	int i, j;
	Uint32 c;

	delete cmc;

	for (i = 0;i < get_dy();i++) {
		for (j = 0;j < get_dx();j++) {
			c = get_pixel(j, i);

			if ((c&AMASK) == AMASK) {
				if (first) {
					first = false;
					x[0] = x[1] = float(j);
					y[0] = y[1] = float(i);
				} else {
					if (j < x[0])
						x[0] = float(j);

					if (j > x[1])
						x[1] = float(j);

					if (i < y[0])
						y[0] = float(i);

					if (i > y[1])
						y[1] = float(i);
				} 
			} 

		} 
	} 

	x[0] -= get_hot_x();

	x[1] -= get_hot_x();

	y[0] -= get_hot_y();

	y[1] -= get_hot_y();

	cmc = new C2DCMC(x, y, 2);
} 


void GLTile::load_textures(void)
{
	int i;

	textures_loaded = reload_textures;

	for (i = 0;i < nparts;i++) {
		if (clamp) {
			if (smooth)
				tex[i] = createTextureClampSmooth(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
			else
				tex[i] = createTextureClamp(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
		} else {
			if (smooth)
				tex[i] = createTextureSmooth(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
			else
				tex[i] = createTexture(tile[i], &(tex_coord_x[i]), &(tex_coord_y[i]));
		} 
	} 
} // GLTile::reload_textures


void GLTile::draw(void)
{
	draw(1, 1, 1, 1);
} 


void GLTile::draw(float r, float g, float b, float a)
{
	int i;

	if (textures_loaded != reload_textures)
		load_textures();

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_TEXTURE_2D);

	for (i = 0;i < nparts;i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);

		glColor4f(r, g, b, a);
		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(float(x[i] - hot_x), float(y[i] - hot_y), 0);

		glTexCoord2f(0, tex_coord_y[i]);
		glVertex3f(float(x[i] - hot_x), float(y[i] + dy[i] - hot_y), 0);

		glTexCoord2f(tex_coord_x[i], tex_coord_y[i]);
		glVertex3f(float(x[i] + dx[i] - hot_x), float(y[i] + dy[i] - hot_y), 0);

		glTexCoord2f(tex_coord_x[i], 0);
		glVertex3f(float(x[i] + dx[i] - hot_x), float(y[i] - hot_y), 0);

		glEnd();
	} 

	glDisable(GL_TEXTURE_2D);
} 



void GLTile::draw(float x, float y, float z, float angle, float scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	if (scale != 1)
		glScalef(scale, scale, scale);

	if (angle != 0)
		glRotatef(angle, 0, 0, 1);

	draw();

	glPopMatrix();
} 


void GLTile::draw(float r, float g, float b, float a, float x, float y, float z, float angle, float scale)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	if (scale != 1)
		glScalef(scale, scale, scale);

	if (angle != 0)
		glRotatef(angle, 0, 0, 1);

	draw(r, g, b, a);

	glPopMatrix();
} // GLTile::draw


void GLTile::draw_cmc(void)
{
	cmc->draw(1, 1, 1, 1);
} 


void GLTile::draw_cmc(float dx, float dy, float dz, float angle, float zoom)
{
	glPushMatrix();
	glTranslatef(dx, dy, dz);

	if (zoom != 1)
		glScalef(zoom, zoom, zoom);

	if (angle != 1)
		glRotatef(angle, 0, 0, 1);

	cmc->draw(1, 1, 1, 1);

	glPopMatrix();
} 



void GLTile::draw_cmc(float r, float g, float b, float a)
{
	cmc->draw(r, g, b, a);
} 


void GLTile::draw_cmc(float r, float g, float b, float a, float dx, float dy, float dz, float angle, float zoom)
{
	glPushMatrix();
	glTranslatef(dx, dy, dz);

	if (zoom != 1)
		glScalef(zoom, zoom, zoom);

	if (angle != 1)
		glRotatef(angle, 0, 0, 1);

	cmc->draw(r, g, b, a);

	glPopMatrix();
} 


void GLTile::optimize(int o_dx, int o_dy)
{
	if (nparts == 1 && tile != 0 && tile[0] != 0) {
		int i, j, k;
		int old_hsx, old_hsy;
		bool *empty;
		bool some_empty;
		int grid_dx = 0, grid_dy = 0;
		int n_non_empty = 0;
		SDL_Surface *sfc;
		//  sfc=SDL_CreateRGBSurface(SDL_SWSURFACE,tile[0]->w,tile[0]->h,32,RMASK,GMASK,BMASK,AMASK);;
		//  SDL_SetAlpha(tile[0], 0, 0);
		//  SDL_BlitSurface(tile[0],0,sfc,0);

		sfc = tile[0];
		tile[0] = 0;

		old_hsx = hot_x;
		old_hsy = hot_y;

		grid_dx = sfc->w / o_dx;

		if (grid_dx*o_dx < sfc->w)
			grid_dx++;

		grid_dy = sfc->h / o_dy;

		if (grid_dy*o_dy < sfc->h)
			grid_dy++;

		empty = new bool[grid_dx * grid_dy];

		some_empty = false;

		/* Determine which parts are empty: */
		for (i = 0;i < grid_dy;i++) {
			for (j = 0;j < grid_dx;j++) {
				empty[j + i*grid_dx] = true;

				if (empty[j + i*grid_dx] && getpixel(sfc, j*o_dx, i*o_dy) != 0) {
					empty[j + i*grid_dx] = false;
					n_non_empty++;
				} 

				if (empty[j + i*grid_dx] && getpixel(sfc, j*o_dx + o_dx - 1, i*o_dy) != 0) {
					empty[j + i*grid_dx] = false;
					n_non_empty++;
				} 

				if (empty[j + i*grid_dx] && getpixel(sfc, j*o_dx, i*o_dy + o_dy - 1) != 0) {
					empty[j + i*grid_dx] = false;
					n_non_empty++;
				} 

				if (empty[j + i*grid_dx] && getpixel(sfc, j*o_dx + o_dx - 1, i*o_dy + o_dy - 1) != 0) {
					empty[j + i*grid_dx] = false;
					n_non_empty++;
				} 

				/*

				    for(k=0;k<o_dy && empty[j+i*grid_dx];k+=8) {
				     for(l=0;l<o_dx && empty[j+i*grid_dx];l+=8) {
				      if (empty[j+i*grid_dx] && getpixel(sfc,j*o_dx+l,i*o_dy+k)!=0) {
				       empty[j+i*grid_dx]=false;
				       n_non_empty++;
				      } // if
				     } // for
				    } // for
				*/

				if (empty[j + i*grid_dx])
					some_empty = true;
			} 
		} 


		if (some_empty) {

			glDeleteTextures(1, &(tex[0]));

			if (tex_coord_x[0] != 0 && tex_coord_y[0] != 0)
				memory_used -= int((dx[0] * dy[0] * 4) / (tex_coord_x[0] * tex_coord_y[0]));

			delete []tile;

			delete []x;

			delete []y;

			delete []dx;

			delete []dy;

			delete []tex_coord_x;

			delete []tex_coord_y;

			delete []tex;

			nparts = 0;

			g_dx = 0;

			g_dy = 0;

			hot_x = 0;

			hot_y = 0;

			tile = 0;

			x = 0;

			y = 0;

			dx = 0;

			dy = 0;

			tex_coord_x = 0;

			tex_coord_y = 0;

			tex = 0;


			/* Create textures: */
			nparts = n_non_empty;

			tile = new SDL_Surface * [nparts];

			x = new int[nparts];

			y = new int[nparts];

			dx = new int[nparts];

			dy = new int[nparts];

			tex_coord_x = new float[nparts];

			tex_coord_y = new float[nparts];

			tex = new GLuint[nparts];

			g_dx = sfc->w;

			g_dy = sfc->h;

			hot_x = old_hsx;

			hot_y = old_hsy;

			k = 0;

			SDL_SetAlpha(sfc, 0, 0);

			for (i = 0;i < grid_dy;i++) {
				for (j = 0;j < grid_dx;j++) {
					if (!empty[j + i*grid_dx]) {
						SDL_Rect r;
						int t_dx, t_dy;

						t_dx = o_dx;
						t_dy = o_dy;

						if (j*o_dx + t_dx > g_dx)
							t_dx = g_dx - j * o_dx;

						if (i*o_dy + t_dy > g_dy)
							t_dy = g_dy - i * o_dy;

						tile[k] = SDL_CreateRGBSurface(SDL_SWSURFACE, t_dx, t_dy, 32, RMASK, GMASK, BMASK, AMASK);

						r.x = j * o_dx;

						r.y = i * o_dy;

						r.w = t_dx;

						r.h = t_dy;

						SDL_BlitSurface(sfc, &r, tile[k], 0);

						x[k] = j * o_dx;

						y[k] = i * o_dy;

						dx[k] = t_dx;

						dy[k] = t_dy;

						tex[k] = createTexture(tile[k], &(tex_coord_x[k]), &(tex_coord_y[k]));

						if (tex_coord_x[k] != 0 && tex_coord_y[k] != 0)
							memory_used += int((dx[k] * dy[k] * 4) / (tex_coord_x[k] * tex_coord_y[k]));

						k++;
					} 
				} 
			} 

			SDL_FreeSurface(sfc);
		} else {
			tile[0] = sfc;
			sfc = 0;
		} 


		delete []empty;
	} 

} // GLTile::optimize


Uint32 GLTile::get_pixel(int ax, int ay)
{
	int i;

	for (i = 0;i < nparts;i++) {
		if (ax >= x[i] && ax < x[i] + dx[i] &&
		        ay >= y[i] && ay < y[i] + dy[i]) {
			return getpixel(tile[i], ax - x[i], ay - y[i]);
		} 
	} 

	return 0;
} // GLTile::get_pixel

