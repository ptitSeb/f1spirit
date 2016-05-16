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

#include "F1Spirit.h"
#include "stdlib.h"
#include "auxiliar.h"
#include "List.h"
#include "2DCMC.h"
#include "GLTile.h"
#include "PlacedGLTile.h"
#include "RotatedGLTile.h"
#include "RotatedGLTile.h"
#include "SDL_glutaux.h"
#include "2DCMC.h"
#include "debug.h"

#include "RoadPiece.h"
#include "track.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#define PART_H 256
#define PART_V 256


#define TYRE_MARK_DURATION 512


extern int reload_textures;

extern bool show_console_msg;
extern char console_msg[80];


CTrack::CTrack(void)
{
	dx = 0;
	dy = 0;

	n_parts_h = 0;
	n_parts_v = 0;
	part_h = PART_H;
	part_v = PART_V;

	background_type = 0;
	background_sfc = 0;
	background_tex = 0;
	textures_loaded = -1;

	background = 0;

	pit_x[0] = 0;
	pit_x[1] = 32;
	pit_y[0] = 0;
	pit_y[1] = 32;

	water_tiles_found = false;
	rock_tiles_found = false;

	sign_list_geenrated_p = false;
} 


CTrack::CTrack(int tn, FILE *fp, List<GLTile> *tiles)
{
	int i, j, k, l, m, t;
	CRotatedGLTile *pt;
	CRoadPiece *rp;
	int init_time;
	int end_time;
	float px, py, pz, pa1, pa2;

	background_type = 0;
	background_sfc = 0;
	background_tex = 0;
	textures_loaded = -1;
	track_number = tn;

	init_time = SDL_GetTicks();

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Get dimensions...\n");
#endif

	if (2 != fscanf(fp, "%i %i", &dx, &dy))
		return ;

	if (1 != fscanf(fp, "%i", &background_type))
		return ;

	set_background(background_type, tiles);

	part_h = PART_H;

	part_v = PART_V;

	n_parts_h = (dx * 8) / part_h;

	if (n_parts_h*part_h < dx*8)
		n_parts_h++;

	n_parts_v = (dy * 8) / part_v;

	if (n_parts_v*part_v < dy*8)
		n_parts_v++;

	if (n_parts_h > 0 && n_parts_v > 0) {

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Initializing parts...\n");
#endif

		background = new List<CRotatedGLTile> **[n_parts_v];

		for (i = 0;i < n_parts_v;i++) {
			background[i] = new List<CRotatedGLTile> *[n_parts_h];

			for (j = 0;j < n_parts_h;j++) {
				background[i][j] = new List<CRotatedGLTile> [9];
			} 
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Loading Tiles...\n");

#endif

		if (1 != fscanf(fp, "%i", &l))
			return ;

		for (m = 0;m < l;m++) {
			if (5 != fscanf(fp, "%f %f %f %f %f", &px, &py, &pz, &pa1, &pa2))
				return ;

			if (1 != fscanf(fp, "%i", &t))
				return ;

			pt = new CRotatedGLTile(px, py, pz, pa1, pa2, tiles->operator [](t));

			i=int(pt->x / part_h);

			j = int(pt->y / part_v);

			if (i >= 0 && i < n_parts_h &&
			        j >= 0 && j < n_parts_v) {
				k = int(( -pt->z) / 8);

				if (k < 0)
					k = 0;

				if (k > 8)
					k = 8;

				background[j][i][k].Add(pt);
			} else {
				delete pt;
			} 
		} 
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Loading Road...\n");

#endif

	if (1 != fscanf(fp, "%i", &l))
		return ;

	for (i = 0;i < l;i++) {
		rp = new CRoadPiece(fp);
		rp->force_internal_draw();
		road.Add(rp);
		tyre_marks.Add(new List<CTyreMark> ());
	} // for

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Getting pit area...\n");

#endif

	if (5 != fscanf(fp, "%f %f %f %f %f", &pit_x[0], &pit_x[1], &pit_y[0], &pit_y[1], &pit_angle))
		return ;

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Track loaded.\n");

#endif

	end_time = SDL_GetTicks();

	load_time = end_time - init_time;

	water_tiles_found = false;

	rock_tiles_found = false;

	sign_list_geenrated_p = false;
} 


CTrack::~CTrack()
{
	int i, j, k;

	while (!water_tiles.EmptyP())
		water_tiles.ExtractIni();

	while (!rock_tiles.EmptyP())
		rock_tiles.ExtractIni();

	for (i = 0;i < n_parts_v;i++) {
		for (j = 0;j < n_parts_h;j++) {
			for (k = 0;k < 9;k++) {
				background[i][j][k].Delete();
			} 

			delete []background[i][j];

			background[i][j] = 0;
		} 

		delete []background[i];

		background[i] = 0;
	} 

	delete []background;

	background = 0;

	background_type = 0;

	if (background_sfc != 0) {
		SDL_FreeSurface(background_sfc);
		glDeleteTextures(1, &background_tex);
	} 

	n_parts_h = 0;

	n_parts_v = 0;
} 


void CTrack::cycle(void)
{
	List<int> todelete;
	List<List<CTyreMark> > l;
	List<CTyreMark> *l2;
	CTyreMark *t;
	int i, *ip;


	l.Instance(tyre_marks);
	l.Rewind();

	while (l.Iterate(l2)) {
		i = 0;
		l2->Rewind();

		while (l2->Iterate(t)) {
			if (t->t == 0) {
				float vx, vy;
				float n;
				float w = 1.0F;

				vx = t->x2 - t->x;
				vy = t->y2 - t->y;

				n = float(sqrt(vx * vx + vy * vy));

				if (n != 0) {
					vx /= n;
					vy /= n;
				} 

				t->qx[0] = t->x - w * vy;

				t->qy[0] = t->y + w * vx;

				t->qx[1] = t->x + w * vy;

				t->qy[1] = t->y - w * vx;

				t->qx[2] = t->x2 - w * vy;

				t->qy[2] = t->y2 + w * vx;

				t->qx[3] = t->x2 + w * vy;

				t->qy[3] = t->y2 - w * vx;
			} 

			t->t++;

			if (t->t >= TYRE_MARK_DURATION) {
				todelete.Insert(new int(i));
			} 

			i++;
		} 

		while (!todelete.EmptyP()) {
			ip = todelete.ExtractIni();
			l2->DeletePosition(*ip);
			delete ip;
		} 

	} 

} 


void CTrack::draw(SDL_Rect *vp, float x, float y, float zoom, List<CPlacedGLTile> *extras, GLTile **road_textures, GLTile **lroad_textures, GLTile **rroad_textures, GLTile *road_line)
{
	int i, j, k;
	float start_x, start_y;
	List<CPlacedGLTile> l;
	CPlacedGLTile *pt;
	List<CRotatedGLTile> l3;
	CRotatedGLTile *rpt;
	List<CRoadPiece> l2;
	CRoadPiece *rp;
	float d;
	float part_rad = float(sqrt(part_h * part_h + part_v * part_v) / 2);
	float screen_rad;
	int part_x, part_y, part_neighbors;
	float xr, yr, wr, hr;
	float road_position = 0;
	float /*tmp1,*/tmp2;

	List<List<CTyreMark> > tm_l;
	List<CTyreMark> *tm_l2;
	CTyreMark *tm;

	/* Road Textures: */
	GLuint *rt;
	GLuint *rt2;
	int nrt2 = 0;

	if (background_type == 1) {
		rt = new GLuint[1];
		rt[0] = road_textures[0]->get_texture(0);
		rt2 = new GLuint[15];
		nrt2 = 15;
		rt2[0] = road_textures[2]->get_texture(0);
		rt2[1] = road_textures[1]->get_texture(0);
		rt2[2] = road_textures[1]->get_texture(0);
		rt2[3] = road_textures[1]->get_texture(0);
		rt2[4] = road_textures[1]->get_texture(0);
		rt2[5] = road_textures[1]->get_texture(0);
		rt2[6] = road_textures[1]->get_texture(0);
		rt2[7] = road_textures[1]->get_texture(0);
		rt2[8] = road_textures[1]->get_texture(0);
		rt2[9] = road_textures[1]->get_texture(0);
		rt2[10] = road_textures[1]->get_texture(0);
		rt2[11] = road_textures[1]->get_texture(0);
		rt2[12] = road_textures[1]->get_texture(0);
		rt2[13] = road_textures[1]->get_texture(0);
		rt2[14] = road_textures[0]->get_texture(0);
	} else {
		rt = new GLuint[1];
		rt[0] = road_textures[0]->get_texture(0);
		rt2 = new GLuint[2];
		nrt2 = 2;
		rt2[0] = road_textures[2]->get_texture(0);
		rt2[1] = road_textures[0]->get_texture(0);
	} 

	/* Side Road Textures: */
	GLuint *lrt;

	GLuint *lrt2;

	GLuint *rrt;

	GLuint *rrt2;

	lrt = new GLuint[1];

	lrt[0] = lroad_textures[0]->get_texture(0);

	lrt2 = new GLuint[1];

	lrt2[0] = lroad_textures[1]->get_texture(0);

	rrt = new GLuint[1];

	rrt[0] = lroad_textures[0]->get_texture(0);

	rrt2 = new GLuint[1];

	rrt2[0] = rroad_textures[1]->get_texture(0);

	GLuint *d_lrt, *d_rrt;

	if (textures_loaded != reload_textures) {
		float t;
		background_tex = createTexture(background_sfc, &t, &t);
		textures_loaded = reload_textures;
	} 

	part_rad += 64; /* margin */

	glPushMatrix();

	glScalef(zoom, zoom, zoom);

	if (vp == 0) {
		start_x = 320 - x;
		start_y = 240 - y;
		screen_rad = float(sqrt(640 * 640 + 480 * 480) / 2);
	} else {
		start_x = vp->x + vp->w / 2 - x;
		start_y = vp->y + vp->h / 2 - y;
		screen_rad = float(sqrt(vp->w * vp->w + vp->h * vp->h) / 2);
	} 

	screen_rad /= zoom;

	// screen_rad*=2;

	part_x = int(x / part_h);

	part_y = int(y / part_v);

	part_neighbors = int(((part_rad + screen_rad) / max(part_h, part_v)) + 1);


	/* Draw the background: */
	if (background_type != 0) {
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, background_tex);

		glColor4f(1, 1, 1, 1);
		glNormal3f(0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(float(start_x), float(start_y), 0);

		glTexCoord2f(0, float(dy) / 8);
		glVertex3f(float(start_x), float(start_y + dy*8), 0);

		glTexCoord2f(float(dx) / 8, float(dy) / 8);
		glVertex3f(float(start_x + dx*8), float(start_y + dy*8), 0);

		glTexCoord2f(float(dx) / 8, 0);
		glVertex3f(float(start_x + dx*8), float(start_y), 0);

		glEnd();
	}

	tmp2 = (screen_rad + part_rad) * (screen_rad + part_rad);

	for (k = 0;k < 9;k++) {

		tm_l.Instance(tyre_marks);
		tm_l.Rewind();

		road_position = -get_length();
		l2.Instance(road);
		l2.Rewind();

		while (l2.Iterate(rp) &&
		        tm_l.Iterate(tm_l2)) {
			if ((rp->z1 <= k*( -8) && rp->z1 > (k + 1)*( -8) && rp->z1 >= rp->z2) ||
			        (rp->z2 <= k*( -8) && rp->z2 > (k + 1)*( -8) && rp->z1 < rp->z2)) {
				rp->get_bbox(&xr, &yr, &wr, &hr);
				d = float(sqrt((x - (xr + wr / 2)) * (x - (xr + wr / 2)) + (y - (yr + hr / 2)) * (y - (yr + hr / 2))));

				if ((d - (wr + hr)) < screen_rad) {

					d_lrt = 0;

					if (rp->ltype == 1)
						d_lrt = lrt;

					if (rp->ltype == 2)
						d_lrt = lrt2;

					d_rrt = 0;

					if (rp->rtype == 1)
						d_rrt = rrt;

					if (rp->rtype == 2)
						d_rrt = rrt2;

					if (l2.EndP()) {
						if (rp->type == 0) {
							rp->draw(start_x, start_y, road_position, 0, 0, 1, d_lrt, 1, d_rrt, road_line->get_texture(0));
						} else {
							rp->draw(start_x, start_y, road_position, nrt2, rt2, 1, d_lrt, 1, d_rrt, road_line->get_texture(0));
						} // if
					} else {
						if (rp->type == 0) {
							rp->draw(start_x, start_y, road_position, 0, 0, 1, d_lrt, 1, d_rrt, road_line->get_texture(0));
						} else {
							rp->draw(start_x, start_y, road_position, 1, rt, 1, d_lrt, 1, d_rrt, road_line->get_texture(0));
						} // if
					} // if

				} 

				/* tyre marks: */
				tm_l2->Rewind();

				while (tm_l2->Iterate(tm)) {
					/* Draw tyre mark: */
					float a = float(tm->t) / float(TYRE_MARK_DURATION);

					if (a < 0)
						a = 0;

					if (a > 1)
						a = 1;

					a = 1 - a;

					glNormal3f(0.0, 0.0, 1.0);

					glColor4f(tm->r, tm->g, tm->b, a*0.75F);

					if (tm->t > 0) {
						//      glBegin(GL_LINES);
						//      glVertex3f(start_x+tm->x,start_y+tm->y,0);
						//      glVertex3f(start_x+tm->x2,start_y+tm->y2,0);
						//      glEnd();
						glBegin(GL_QUADS);
						glVertex3f(start_x + tm->qx[0], start_y + tm->qy[0], tm->z);
						glVertex3f(start_x + tm->qx[1], start_y + tm->qy[1], tm->z);
						glVertex3f(start_x + tm->qx[3], start_y + tm->qy[3], tm->z2);
						glVertex3f(start_x + tm->qx[2], start_y + tm->qy[2], tm->z2);
						glEnd();
					} 
				} 

			} 

			road_position += rp->get_length();
		} 

		for (i = max(part_y - part_neighbors, 0);i < n_parts_v && i < part_y + part_neighbors;i++) {
			for (j = max(part_x - part_neighbors, 0);j < n_parts_h && j < part_x + part_neighbors;j++) {
				if (!background[i][j][k].EmptyP()) {
					//     d=float(sqrt((x-(j*part_h+part_h/2))*(x-(j*part_h+part_h/2))+(y-(i*part_v+part_v/2))*(y-(i*part_v+part_v/2))));
					//     if ((d-part_rad)<screen_rad) {
					d = (x - (j * part_h + part_h / 2)) * (x - (j * part_h + part_h / 2)) + (y - (i * part_v + part_v / 2)) * (y - (i * part_v + part_v / 2));

					if (d < tmp2) {
						l3.Instance(background[i][j][k]);
						l3.Rewind();

						while (l3.Iterate(rpt)) {
							rpt->draw(float(start_x), float(start_y), 0, 0, 1);
						} 
					} 
				} 
			} 
		} 

		if (extras != 0) {
			l.Instance(*extras);
			l.Rewind();

			while (l.Iterate(pt)) {
				if ((pt->z <= k*( -8) && pt->z > (k + 1)*( -8)) ||
				        (pt->z <= k*( -8) && k == 8)) {
					pt->draw(float(start_x), float(start_y), 0, 0, 1);
					//     pt->tile->draw_cmc(pt->x+start_x,pt->y+start_y,-pt->z,pt->angle,1);
				} 
			} 
		} 
	} 

	glPopMatrix();

	delete []rt;

	delete []rt2;

	delete []lrt;

	delete []lrt2;

	delete []rrt;

	delete []rrt2;

} 


SDL_Surface *CTrack::draw_minimap(int w, int h, float *zoom)
{
	List<CRoadPiece> l2;
	CRoadPiece *rp;
	SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);
	int i, j, k;
	float x1, y1, x2, y2;
	bool first;
	Uint32 clearcolor = SDL_MapRGBA(sfc->format, 0, 0, 0, 0);
	Uint32 color = SDL_MapRGBA(sfc->format, 255, 255, 255, 255);
	Uint32 color2 = SDL_MapRGBA(sfc->format, 0, 0, 0, 255);
	Uint32 color_tmp = SDL_MapRGBA(sfc->format, 255, 0, 0, 255); /* temporal color */

	SDL_FillRect(sfc, 0, clearcolor);

	*zoom = min(float(w) / (dx * 8), float(h) / (dy * 8));

	/* Draw the track: */
	first = true;
	l2.Instance(road);
	l2.Rewind();

	while (l2.Iterate(rp)) {
		rp->force_internal_draw();

		for (i = 0;i < rp->path_points - 1;i++) {
			float w = (rp->w1 + rp->w2) / 2;
			int thickness = 3;

			if (w > 128)
				thickness = 4;

			if (w > 196)
				thickness = 5;

			if (w > 256)
				thickness = 6;

			x1 = (rp->path_x[i]) * (*zoom);

			y1 = (rp->path_y[i]) * (*zoom);

			x2 = (rp->path_x[i + 1]) * (*zoom);

			y2 = (rp->path_y[i + 1]) * (*zoom);

			draw_thick_line(sfc, int(x1), int(y1), int(x2), int(y2), color, thickness);

			if (first) {
				float dx, dy, n;
				dx = -(rp->path_y[i + 1] - rp->path_y[i]);
				dy = rp->path_x[i + 1] - rp->path_x[i];

				n = float(sqrt(dx * dx + dy * dy));

				if (n != 0) {
					dx /= n;
					dy /= n;

					draw_thick_line(sfc, int(x1 - dx*3), int(y1 - dy*3), int(x1 + dx*3), int(y1 + dy*3), color, thickness);

					first = false;
				} 

			} 
		} 
	} 

	/* blackoutline: */
	for (i = 0;i < 2;i++) {
		for (j = 1;j < w - 1;j++) {
			for (k = 1;k < h - 1;k++) {
				if (getpixel(sfc, j, k) != color && getpixel(sfc, j, k) != color2) {
					if (getpixel(sfc, j - 1, k) == color || getpixel(sfc, j - 1, k) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j + 1, k) == color || getpixel(sfc, j + 1, k) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j, k - 1) == color || getpixel(sfc, j, k - 1) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j, k + 1) == color || getpixel(sfc, j, k + 1) == color2)
						putpixel(sfc, j, k, color_tmp);
				} 
			} 
		} 

		for (j = 1;j < w - 1;j++) {
			for (k = 1;k < h - 1;k++) {
				if (getpixel(sfc, j, k) == color_tmp)
					putpixel(sfc, j, k, color2);
			} 
		} 
	} 

	return sfc;
} 



SDL_Surface *CTrack::draw_vertical_minimap(int w, int h, int nlaps, float *zoom)
{
	List<CRoadPiece> l2;
	SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);
	int i, j, k;
	Uint32 clearcolor = SDL_MapRGBA(sfc->format, 0, 0, 0, 0);
	Uint32 color = SDL_MapRGBA(sfc->format, 255, 255, 255, 255);
	Uint32 color2 = SDL_MapRGBA(sfc->format, 0, 0, 0, 255);
	Uint32 color_tmp = SDL_MapRGBA(sfc->format, 255, 0, 0, 255); /* temporal color */

	SDL_FillRect(sfc, 0, clearcolor);

	*zoom = min(float(w) / (dx * 8), float(h) / (dy * 8));

	/* Draw the track: */
	draw_thick_line(sfc, w / 2, 8, w / 2, h - 8, color, 4);

	/* Draw the start lines: */

	for (i = 0;i < nlaps;i++) {
		draw_thick_line(sfc, w / 2 - 3, 8 + ((h - 16) / nlaps)*i, w / 2 + 3, 8 + ((h - 16) / nlaps)*i, color, 4);
	} 

	draw_thick_line(sfc, w / 2 - 3, h - 8, w / 2 + 3, h - 8, color, 4);

	/* blackoutline: */
	for (i = 0;i < 3;i++) {
		for (j = 1;j < w - 1;j++) {
			for (k = 1;k < h - 1;k++) {
				if (getpixel(sfc, j, k) != color && getpixel(sfc, j, k) != color2) {
					if (getpixel(sfc, j - 1, k) == color || getpixel(sfc, j - 1, k) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j + 1, k) == color || getpixel(sfc, j + 1, k) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j, k - 1) == color || getpixel(sfc, j, k - 1) == color2)
						putpixel(sfc, j, k, color_tmp);

					if (getpixel(sfc, j, k + 1) == color || getpixel(sfc, j, k + 1) == color2)
						putpixel(sfc, j, k, color_tmp);
				} 
			} 
		} 

		for (j = 1;j < w - 1;j++) {
			for (k = 1;k < h - 1;k++) {
				if (getpixel(sfc, j, k) == color_tmp)
					putpixel(sfc, j, k, color2);
			} 
		} 
	} 

	return sfc;
} 


CRotatedGLTile *CTrack::collision(float x, float y, float z, float a, C2DCMC *cmc)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	float d;
	float part_rad = float(sqrt(part_h * part_h + part_v * part_v) / 2);
	float cmc_rad = cmc->get_radius();
	int part_x, part_y, part_neighbors;

	part_rad += 64; /* margin */

	part_x = int(x / part_h);
	part_y = int(y / part_v);
	part_neighbors = int(((part_rad + cmc_rad) / max(part_h, part_v)) + 1);

	for (k = 0;k < 9;k++) {
		//  if (abs(z-(-k*8))<16) {
		for (i = max(part_y - part_neighbors, 0);i < n_parts_v && i < part_y + part_neighbors;i++) {
			for (j = max(part_x - part_neighbors, 0);j < n_parts_h && j < part_x + part_neighbors;j++) {
				if (!background[i][j][k].EmptyP()) {
					d = float(sqrt((x - (j * part_h + part_h / 2)) * (x - (j * part_h + part_h / 2)) + (y - (i * part_v + part_v / 2)) * (y - (i * part_v + part_v / 2))));

					if ((d - part_rad) < cmc_rad) {
						l.Instance(background[i][j][k]);
						l.Rewind();

						while (l.Iterate(pt)) {
							if (cmc->collision(x, y, a, pt->get_cmc(), pt->x, pt->y, pt->angle1))
								return pt;
						} 
					} 
				} 
			} 
		} 

		//  } 
	} 

	return 0;
} 



CRotatedGLTile *CTrack::collision(float x, float y, float z)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	float d;
	float part_rad = float(sqrt(part_h * part_h + part_v * part_v) / 2);
	int part_x, part_y, part_neighbors;

	part_rad += 64; /* margin */

	part_x = int(x / part_h);
	part_y = int(y / part_v);
	part_neighbors = int((part_rad / max(part_h, part_v)) + 1);

	for (k = 0;k < 9;k++) {
		//  if (abs(z-(-k*8))<16) {
		for (i = max(part_y - part_neighbors, 0);i < n_parts_v && i < part_y + part_neighbors;i++) {
			for (j = max(part_x - part_neighbors, 0);j < n_parts_h && j < part_x + part_neighbors;j++) {
				if (!background[i][j][k].EmptyP()) {
					d = float(sqrt((x - (j * part_h + part_h / 2)) * (x - (j * part_h + part_h / 2)) + (y - (i * part_v + part_v / 2)) * (y - (i * part_v + part_v / 2))));

					if ((d - part_rad) < 1) {
						l.Instance(background[i][j][k]);
						l.Rewind();

						while (l.Iterate(pt)) {
							if (pt->get_cmc()->collision(pt->x, pt->y, pt->angle1, x, y))
								return pt;
						} 
					} 
				} 
			} 
		} 

		//  } 
	} 

	return 0;
} 


void CTrack::save(FILE *fp, List<GLTile> *tiles)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	List<CRoadPiece> l2;
	CRoadPiece *rp;
	int length;

	fprintf(fp, "%i %i\n", dx, dy);

	fprintf(fp, "%i\n", background_type);

	length = 0;

	for (k = 0;k < 9;k++) {
		for (i = 0;i < n_parts_v;i++) {
			for (j = 0;j < n_parts_h;j++) {
				length += background[i][j][k].Length();
			} 
		} 
	} 

	fprintf(fp, "%i\n", length);

	for (k = 0;k < 9;k++) {
		for (i = 0;i < n_parts_v;i++) {
			for (j = 0;j < n_parts_h;j++) {
				l.Instance(background[i][j][k]);
				l.Rewind();

				while (l.Iterate(pt)) {
					fprintf(fp, "%g %g %g %g %g %i\n", pt->x, pt->y, pt->z, pt->angle1, pt->angle2, tiles->PositionRef(pt->tile));
				} 
			} 
		} 
	} 

	fprintf(fp, "%i\n", road.Length());

	l2.Instance(road);

	l2.Rewind();

	while (l2.Iterate(rp)) {
		rp->save(fp);
	} 

	fprintf(fp, "%g %g %g %g %g\n", pit_x[0], pit_x[1], pit_y[0], pit_y[1], pit_angle);
} 


void CTrack::add_tile(CRotatedGLTile *pt)
{
	int i, j, k;

	i = int(pt->x / part_h);
	j = int(pt->y / part_v);

	if (i >= 0 && i < n_parts_h &&
	        j >= 0 && j < n_parts_v) {
		k = int(( -pt->z) / 8);

		if (k < 0)
			k = 0;

		if (k > 8)
			k = 8;

		background[j][i][k].Add(pt);
	} else {
		delete pt;
	} 
} 


void CTrack::del_tile(float x, float y)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	bool found = false;

	for (i = 0;i < n_parts_v && !found;i++) {
		for (j = 0;j < n_parts_h && !found;j++) {
			for (k = 8;k >= 0 && !found;k--) {
				l.Instance(background[i][j][k]);
				l.Rewind();

				while (l.Iterate(pt)) {
					if (pt->x == x && pt->y == y) {
						background[i][j][k].DeleteElement(pt);
						delete pt;
						found = true;
					} 
				} 
			} 
		} 
	} 
} 


void CTrack::rotate_tile(float x, float y, float inc)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	bool found = false;

	for (i = 0;i < n_parts_v && !found;i++) {
		for (j = 0;j < n_parts_h && !found;j++) {
			for (k = 8;k >= 0 && !found;k--) {
				l.Instance(background[i][j][k]);
				l.Rewind();

				while (l.Iterate(pt)) {
					if (pt->x == x && pt->y == y) {
						pt->angle1 += inc;
					} 
				} 
			} 
		} 
	} 
} 


void CTrack::rotate_tile2(float x, float y, float inc)
{
	int i, j, k;
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;
	bool found = false;

	for (i = 0;i < n_parts_v && !found;i++) {
		for (j = 0;j < n_parts_h && !found;j++) {
			for (k = 8;k >= 0 && !found;k--) {
				l.Instance(background[i][j][k]);
				l.Rewind();

				while (l.Iterate(pt)) {
					if (pt->x == x && pt->y == y) {
						pt->angle2 += inc;
					} 
				} 
			} 
		} 
	} 
} 


void CTrack::move_tiles(float x, float y, float z)
{
	int i, j, k;
	List<CRoadPiece> lr;
	List<CRotatedGLTile> l;
	CRoadPiece *rp;
	CRotatedGLTile *pt;

	for (i = 0;i < n_parts_v;i++) {
		for (j = 0;j < n_parts_h;j++) {
			for (k = 0;k < 9;k++) {
				l.Instance(background[i][j][k]);
				l.Rewind();

				while (l.Iterate(pt)) {
					pt->x += x;
					pt->y += y;
					pt->z += z;
				} 
			} 
		} 
	} 

	/* road pieces: */
	lr.Instance(road);

	lr.Rewind();

	while (lr.Iterate(rp)) {
		rp->y1 += y;
		rp->y2 += y;
		rp->x1 += x;
		rp->x2 += x;
		rp->z1 += z;
		rp->z2 += z;
		rp->force_internal_draw();
	} 

} 


void CTrack::change_size(int ndx, int ndy)
{
	int i, j, k;
	CRotatedGLTile *pt;
	List<CRotatedGLTile> tmp_bg;

	for (k = 0;k < 9;k++) {
		for (i = 0;i < n_parts_v;i++) {
			for (j = 0;j < n_parts_h;j++) {
				while (!background[i][j][k].EmptyP()) {
					pt = background[i][j][k].ExtractIni();
					tmp_bg.Add(pt);
				} 
			} 
		} 
	} 

	for (i = 0;i < n_parts_v;i++) {
		for (j = 0;j < n_parts_h;j++) {
			for (k = 0;k < 9;k++) {
				background[i][j][k].Delete();
			} 

			delete []background[i][j];

			background[i][j] = 0;
		} 

		delete []background[i];

		background[i] = 0;
	} 

	delete []background;

	background = 0;

	dx = ndx;

	dy = ndy;

	n_parts_h = (dx * 8) / part_h;

	if (n_parts_h*part_h < dx*8)
		n_parts_h++;

	n_parts_v = (dy * 8) / part_v;

	if (n_parts_v*part_v < dy*8)
		n_parts_v++;

	if (n_parts_h > 0 && n_parts_v > 0) {
		background = new List<CRotatedGLTile> **[n_parts_v];

		for (i = 0;i < n_parts_v;i++) {
			background[i] = new List<CRotatedGLTile> *[n_parts_h];

			for (j = 0;j < n_parts_h;j++) {
				background[i][j] = new List<CRotatedGLTile> [9];
			} 
		} 

		while (!tmp_bg.EmptyP()) {
			pt = tmp_bg.ExtractIni();
			i = int(pt->x / part_h);
			j = int(pt->y / part_v);

			if (i >= 0 && i < n_parts_h &&
			        j >= 0 && j < n_parts_v) {
				k = int(( -pt->z) / 8);

				if (k < 0)
					k = 0;

				if (k > 8)
					k = 8;

				background[j][i][k].Add(pt);
			} else {
				delete pt;
			} 
		} 
	} 
} 


float CTrack::get_length(void)
{
	float length = 0;
	List<CRoadPiece> lr;
	CRoadPiece *rp;

	lr.Instance(road);
	lr.Rewind();

	while (lr.Iterate(rp))
		length += rp->get_length();

	return length;
} 


void CTrack::scale_road(float factor)
{
	List<CRoadPiece> lr;
	CRoadPiece *rp;

	change_size(int(dx*factor), int(dy*factor));

	lr.Instance(road);
	lr.Rewind();

	while (lr.Iterate(rp)) {
		rp->x1 *= factor;
		rp->y1 *= factor;
		rp->x2 *= factor;
		rp->y2 *= factor;
		rp->force_internal_draw();
	} 

} 


void CTrack::set_background(int type, List<GLTile> *tiles)
{
	background_type = 0;

	if (background_sfc != 0) {
		SDL_FreeSurface(background_sfc);
		glDeleteTextures(1, &background_tex);
	} 

	switch (type) {

		case 0:
			background_type = 0;
			background_sfc = 0;
			background_tex = 0;
			break;

		case 1: {
				float t;
				background_type = 1;
				SDL_Surface *sfc = tiles->operator [](0)->get_tile(0);
				background_sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, sfc->w, sfc->h, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_SetAlpha(sfc, 0, 0);
				SDL_BlitSurface(sfc, 0, background_sfc, 0);
				background_tex = createTexture(background_sfc, &t, &t);
				textures_loaded = reload_textures;
			}

			break;

		case 2: {
				float t;
				background_type = 2;
				SDL_Surface *sfc = tiles->operator [](42)->get_tile(0);
				background_sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, sfc->w, sfc->h, 32, RMASK, GMASK, BMASK, AMASK);
				SDL_SetAlpha(sfc, 0, 0);
				SDL_BlitSurface(sfc, 0, background_sfc, 0);
				background_tex = createTexture(background_sfc, &t, &t);
				textures_loaded = reload_textures;
			}

			break;
	} 


} 


CRoadPiece *CTrack::get_start_position(int n, float *x, float *y, float *a)
{
	if (road[0]->get_w1() >= 196) {
		int side, row;
		float first_x[6] = { -40, -44, -48, -52, -56, -60}, first_y[6] = { -100, -60, -20, 20, 60, 100};
		float base_x, base_y, tmp_z, base_angle, tmp_d, tmp_w, initial_w;
		int i, current_road_pos = 0;

		row = n / 6;
		side = n % 6;

		initial_w = road[road.Length() - 1]->get_w2();

		base_angle = -(road[0]->get_a1() * M_PI) / 180.0F;
		base_x = float(first_x[side] * cos(base_angle) + road[current_road_pos]->get_x1());
		base_y = float( -first_x[side] * sin(base_angle) + road[current_road_pos]->get_y1());

		if (!road[current_road_pos]->inside_test(base_x, base_y)) {
			current_road_pos--;

			if (current_road_pos < 0)
				current_road_pos = road.Length() - 1;
		} 

		road[current_road_pos]->get_path_position(base_x, base_y, &base_x, &base_y, &tmp_z, &base_angle, &tmp_w, &tmp_d);

		base_angle = -(base_angle * M_PI) / 180.0F;

		for (i = 0;i < row;i++) {
			base_x += float((( -80) * cos(base_angle)));
			base_y += float(( -( -80) * sin(base_angle)));

			if (!road[current_road_pos]->inside_test(base_x, base_y)) {
				current_road_pos--;

				if (current_road_pos < 0)
					current_road_pos = road.Length() - 1;
			} 

			road[current_road_pos]->get_path_position(base_x, base_y, &base_x, &base_y, &tmp_z, &base_angle, &tmp_w, &tmp_d);

			base_angle = -(base_angle * M_PI) / 180.0F;
		} 

		*x = base_x + float((first_y[side] + (tmp_w - initial_w) / 2) * sin(base_angle));

		*y = base_y + float((first_y[side] + (tmp_w - initial_w) / 2) * cos(base_angle));

		*a = -(base_angle * 180.0F) / M_PI + 90;

		return road[current_road_pos];
	} else {
		int side, row;
		float first_x[2] = { -48, -64}, first_y[2] = { -24, 24};
		float base_x, base_y, tmp_z, base_angle, tmp_d, tmp_w, initial_w;
		int i, current_road_pos = 0;

		if (track_number == 10)
			n += 2;

		row = n / 2;

		side = n % 2;

		initial_w = road[road.Length() - 1]->get_w2();

		base_angle = -(road[0]->get_a1() * M_PI) / 180.0F;

		base_x = float(first_x[side] * cos(base_angle) + road[current_road_pos]->get_x1());

		base_y = float( -first_x[side] * sin(base_angle) + road[current_road_pos]->get_y1());

		if (!road[current_road_pos]->inside_test(base_x, base_y)) {
			current_road_pos--;

			if (current_road_pos < 0)
				current_road_pos = road.Length() - 1;
		} 

		road[current_road_pos]->get_path_position(base_x, base_y, &base_x, &base_y, &tmp_z, &base_angle, &tmp_w, &tmp_d);

		base_angle = -(base_angle * M_PI) / 180.0F;

		for (i = 0;i < row;i++) {
			base_x += float((( -80) * cos(base_angle)));
			base_y += float(( -( -80) * sin(base_angle)));

			if (!road[current_road_pos]->inside_test(base_x, base_y)) {
				current_road_pos--;

				if (current_road_pos < 0)
					current_road_pos = road.Length() - 1;
			} 

			road[current_road_pos]->get_path_position(base_x, base_y, &base_x, &base_y, &tmp_z, &base_angle, &tmp_w, &tmp_d);

			base_angle = -(base_angle * M_PI) / 180.0F;
		} 

		*x = base_x + float((first_y[side] + (tmp_w - initial_w) / 2) * sin(base_angle));

		*y = base_y + float((first_y[side] + (tmp_w - initial_w) / 2) * cos(base_angle));

		*a = -(base_angle * 180.0F) / M_PI + 90;

		return road[current_road_pos];

		return 0;
	} 

} 



bool CTrack::over_water(float x, float y, GLTile *water_tile)
{
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;

	if (!water_tiles_found) {
		int i, j, k;

		/* Look for water tiles: */

		for (i = 0;i < n_parts_v;i++) {
			for (j = 0;j < n_parts_h;j++) {
				for (k = 8;k >= 0;k--) {
					l.Instance(background[i][j][k]);
					l.Rewind();

					while (l.Iterate(pt)) {
						if (pt->tile == water_tile) {
							water_tiles.Add(pt);
						} 
					} 
				} 
			} 
		} 

		water_tiles_found = true;
	} 

	l.Instance(water_tiles);

	l.Rewind();

	while (l.Iterate(pt)) {
		if (x >= pt->x && y >= pt->y &&
		        x < pt->x + pt->tile->get_dx() && y < pt->y + pt->tile->get_dy()) {
			return true;
		} 
	} 

	return false;
} 


bool CTrack::over_rock(float x, float y, GLTile *rock_tile)
{
	List<CRotatedGLTile> l;
	CRotatedGLTile *pt;

	if (!rock_tiles_found) {
		int i, j, k;

		/* Look for rock tiles: */

		for (i = 0;i < n_parts_v;i++) {
			for (j = 0;j < n_parts_h;j++) {
				for (k = 8;k >= 0;k--) {
					l.Instance(background[i][j][k]);
					l.Rewind();

					while (l.Iterate(pt)) {
						if (pt->tile == rock_tile) {
							rock_tiles.Add(pt);
						} 
					} 
				} 
			} 
		} 

		rock_tiles_found = true;
	} 

	l.Instance(rock_tiles);

	l.Rewind();

	while (l.Iterate(pt)) {
		if (x >= pt->x && y >= pt->y &&
		        x < pt->x + pt->tile->get_dx() && y < pt->y + pt->tile->get_dy()) {
			return true;
		} 
	} 

	return false;
} 


char *track_names[N_TRACKS] = {"stock",
                               "rally",
                               "f3",
                               "f3000",
                               "endurance",
                               "f1-brazil",
                               "f1-sanmarino",
                               "f1-belgium",
                               "f1-monaco",
                               "f1-usa",
                               "f1-france",
                               "f1-greatbritain",
                               "f1-westgermany",
                               "f1-hungary",
                               "f1-austria",
                               "f1-italy",
                               "f1-portugal",
                               "f1-spain",
                               "f1-mexico",
                               "f1-japan",
                               "f1-australia",
                               "stock-oval",
                               "f1-oval",
                               "stock-micro",
                               "f1-micro"
                              };

const char *CTrack::get_track_name(void)
{
	return track_names[track_number];
} 


int CTrack::track_sign_to_show2(float x, float y, float position, GLTile **arrow_tiles)
{
	if (!sign_list_geenrated_p) {
		int i, j, k, m;
		List<CRotatedGLTile> l;
		CRotatedGLTile *t;

		/* Look for arrows: */

		for (k = 0;k < 9;k++) {
			for (i = 0;i < n_parts_v;i++) {
				for (j = 0;j < n_parts_h;j++) {
					l.Instance(background[i][j][k]);
					l.Rewind();

					while (l.Iterate(t)) {
						for (m = 0;m < 10;m++) {
							if (t->tile == arrow_tiles[m] && arrow_tiles[m] != 0) {
								/* Found an arrow: */
								sign_positions.Add(new float(get_position(t->x, t->y)));
								sign_sign.Add(new int(m));
#ifdef F1SPIRIT_DEBUG_MESSAGES

								output_debug_message("Arrow found (%i)\n", m);
#endif

							} 
						} 
					} 
				} 
			} 
		} 

		/* Sort the sign list: */
		// FIXME: Santi, better sort this once, while initializing, in stead of keeping this code here
		{
			float ftmp;
			int itmp;
			bool changes = false;
			int i, l = sign_sign.Length();

			do {
				changes = false;

				for (i = 0;i < l - 1;i++) {
					if ((*(sign_positions[i])) > (*(sign_positions[i + 1]))) {
						ftmp = *(sign_positions[i]);
						*(sign_positions[i]) = *(sign_positions[i + 1]);
						*(sign_positions[i + 1]) = ftmp;
						itmp = *(sign_sign[i]);
						*(sign_sign[i]) = *(sign_sign[i + 1]);
						*(sign_sign[i + 1]) = itmp;
						changes = true;
					} 
				} 
			} while (changes);

#ifdef F1SPIRIT_DEBUG_MESSAGES

			{
				float *pos;
				int *sign;
				output_debug_message("Sorted road arrows found:\n");
				sign_positions.Rewind();
				sign_sign.Rewind();

				while (sign_positions.Iterate(pos) && sign_sign.Iterate(sign)) {
					output_debug_message("Position: %g, Sign: %i\n", *pos, *sign);
				} 
			}
#endif
		}

		sign_list_geenrated_p = true;
	} 

	// look for the closest sign:
	{
		float *pos;
		int *sign;
		float tl = get_length();

		while (position > tl)
			position -= tl;

		sign_positions.Rewind();

		sign_sign.Rewind();

		while (sign_positions.Iterate(pos) && sign_sign.Iterate(sign)) {
			if ((*pos) > position && (*pos) < position + 800) {
				return *sign;
			} 
		} 
	}

	return -1;
} 


float CTrack::get_position(float x, float y)
{
	List<CRoadPiece> l;
	CRoadPiece *rp, *closest = 0;
	float d, min_d = 0;
	float position = 0;

	l.Instance(road);
	l.Rewind();

	while (l.Iterate(rp)) {
		rp->distance_to_road(x, y, &d);

		if (closest == 0 || d < min_d) {
			min_d = d;
			closest = rp;
		} 
	} 

	/* pieces: */
	l.Rewind();

	while (l.Iterate(rp) && rp != closest)
		position += rp->get_length();

	/* offset inside current piece: */
	/* simplification: inverse of the distance to end of piece = distance */
	{
		position += rp->get_length();
		float d = float(sqrt((x - rp->get_x2()) * (x - rp->get_x2()) +
		                     (y - rp->get_y2()) * (y - rp->get_y2())));
		position -= d;
	}

	return position;
} 

