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

#include "List.h"
#include "Vector.h"
#include "GLTile.h"
#include "SDL_glutaux.h"
#include "geometrics.h"
#include "2DCMC.h"

#include "auxiliar.h"
#include "RoadPiece.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#define OPTIMIZE_DX  64
#define OPTIMIZE_DY  64


CRoadPiece::CRoadPiece(void)
{
	type = 1;
	ltype = 2;
	rtype = 2;

	x1 = y1 = z1 = 0;
	w1 = 96;
	a1 = 90;
	lchicane1 = 16;
	rchicane1 = 16;
	nlines1 = 0;

	x2 = z2 = 0;
	y2 = 128;
	w2 = 96;
	a2 = 90;
	lchicane2 = 16;
	rchicane2 = 16;
	nlines2 = 0;

	path_points = 0;
	path_x = 0;
	path_y = 0;
	path_z = 0;
	path_a = 0;
	path_w = 0;

	r_x1 = r_y1 = 0;
	r_x2 = r_y2 = 0;
	r_z = 0;
	r_cx1 = r_cy1 = 0;
	r_cx2 = r_cy2 = 0;

	length_computed = false;

} 


CRoadPiece::CRoadPiece(float x, float y, float z)
{
	type = 1;
	ltype = 2;
	rtype = 2;

	x1 = x;
	y1 = y;
	z1 = z;
	w1 = 96;
	a1 = 90;
	lchicane1 = 16;
	rchicane1 = 16;
	nlines1 = 0;

	x2 = x;
	y2 = y + 128;
	z2 = z;
	w2 = 96;
	a2 = 90;
	lchicane2 = 16;
	rchicane2 = 16;
	nlines2 = 0;

	path_points = 0;
	path_x = 0;
	path_y = 0;
	path_z = 0;
	path_a = 0;
	path_w = 0;

	r_x1 = r_y1 = 0;
	r_x2 = r_y2 = 0;
	r_z = 0;
	r_cx1 = r_cy1 = 0;
	r_cx2 = r_cy2 = 0;

	length_computed = false;

} 



CRoadPiece::CRoadPiece(FILE *fp)
{
	if (3 != fscanf(fp, "%i %i %i", &type, &ltype, &rtype))
		return ;

	if (8 != fscanf(fp, "%f %f %f %f %f %f %f %i", &x1, &y1, &z1, &w1, &a1, &lchicane1, &rchicane1, &nlines1))
		return ;

	if (8 != fscanf(fp, "%f %f %f %f %f %f %f %i", &x2, &y2, &z2, &w2, &a2, &lchicane2, &rchicane2, &nlines2))
		return ;

	// w1=w2=96;
	lchicane1 = lchicane2 = rchicane1 = rchicane2 = 16;

	// x1*=6;
	// y1*=6;
	// x2*=6;
	// y2*=6;

	path_points = 0;

	path_x = 0;

	path_y = 0;

	path_z = 0;

	path_a = 0;

	path_w = 0;

	r_x1 = r_y1 = 0;

	r_x2 = r_y2 = 0;

	r_z = 0;

	r_cx1 = r_cy1 = 0;

	r_cx2 = r_cy2 = 0;

	length_computed = false;

} 


CRoadPiece::~CRoadPiece()
{
	delete []r_x1;
	delete []r_y1;
	delete []r_x2;
	delete []r_y2;
	delete []r_z;
	delete []r_cx1;
	delete []r_cy1;
	delete []r_cx2;
	delete []r_cy2;

	delete []path_x;
	delete []path_y;
	delete []path_z;
	delete []path_a;
	delete []path_w;
	path_points = 0;
	path_x = 0;
	path_y = 0;
	path_z = 0;
	path_a = 0;
	path_w = 0;

} 


void CRoadPiece::save(FILE *fp)
{
	fprintf(fp, "%i %i %i\n", type, ltype, rtype);
	fprintf(fp, "%g %g %g %g %g %g %g %i\n", x1, y1, z1, w1, a1, lchicane1, rchicane1, nlines1);
	fprintf(fp, "%g %g %g %g %g %g %g %i\n", x2, y2, z2, w2, a2, lchicane2, rchicane2, nlines2);
} 


void CRoadPiece::get_bbox(float *x, float *y, float *w, float *h)
{
	if (r_x1 == 0)
		draw();

	if (r_x1 != 0) {
		*x = bbox[0];
		*y = bbox[1];
		*w = bbox[2] - bbox[0];
		*h = bbox[3] - bbox[1];
	} else {
		*x = 0;
		*y = 0;
		*w = 0;
		*h = 0;
	} 
} 


void CRoadPiece::force_internal_draw(void)
{
	delete []r_x1;
	delete []r_y1;
	delete []r_x2;
	delete []r_y2;
	delete []r_z;
	delete []r_cx1;
	delete []r_cy1;
	delete []r_cx2;
	delete []r_cy2;

	delete []path_x;
	delete []path_y;
	delete []path_z;
	delete []path_a;
	delete []path_w;
	r_x1 = r_y1 = 0;
	r_x2 = r_y2 = 0;
	r_z = 0;
	r_cx1 = r_cy1 = 0;
	r_cx2 = r_cy2 = 0;
	internal_draw();
} 


void CRoadPiece::draw(void)
{
	if (r_x1 == 0)
		internal_draw();

	if (r_x1 != 0) {
		int i;

		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.5, 0.5, 0.5, 1.0);
		glBegin(GL_QUADS);

		for (i = 0;i < steps;i++) {
			glVertex3f(r_x1[i], r_y1[i], r_z[i]);
			glVertex3f(r_x2[i], r_y2[i], r_z[i]);
			glVertex3f(r_x2[i + 1], r_y2[i + 1], r_z[i + 1]);
			glVertex3f(r_x1[i + 1], r_y1[i + 1], r_z[i + 1]);
		} 

		glEnd();

		if (r_cx1 != 0) {
			glColor4f(1, 0, 0, 1.0);
			glBegin(GL_QUADS);

			for (i = 0;i < steps;i++) {
				glVertex3f(r_cx1[i], r_cy1[i], r_z[i]);
				glVertex3f(r_x1[i], r_y1[i], r_z[i]);
				glVertex3f(r_x1[i + 1], r_y1[i + 1], r_z[i + 1]);
				glVertex3f(r_cx1[i + 1], r_cy1[i + 1], r_z[i + 1]);
			} 

			for (i = 0;i < steps;i++) {
				glVertex3f(r_cx2[i], r_cy2[i], r_z[i]);
				glVertex3f(r_x2[i], r_y2[i], r_z[i]);
				glVertex3f(r_x2[i + 1], r_y2[i + 1], r_z[i + 1]);
				glVertex3f(r_cx2[i + 1], r_cy2[i + 1], r_z[i + 1]);
			} 

			glEnd();
		} 

	} 
} 


void CRoadPiece::draw(float orig_x, float orig_y)
{
	glPushMatrix();
	glTranslatef(orig_x, orig_y, 0);

	draw();

	glPopMatrix();
} 


void CRoadPiece::draw(float offs, int n_textures, GLuint *textures, int n_ltextures, GLuint *ltextures, int n_rtextures, GLuint *rtextures, GLuint line_texture)
{
	if (r_x1 == 0)
		internal_draw();

	if (r_x1 != 0) {
		int i, j, lj, rj;
		float pos = offs;
		float inc;
		float p1x, p1y, p2x, p2y;
		float l = get_length(), tmp;
		float n_lines, n_lines_inc;
		float n_lines_tmp1, n_lines_tmp2;
		float r_width, r_width_inc;

		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glColor4f(1, 1, 1, 1.0);
		n_lines = float(nlines2);
		n_lines_inc = float(nlines1 - nlines2) / steps;
		r_width = w1;
		r_width_inc = (w2 - w1) / steps;

		for (i = 0;i < steps;i++, n_lines += n_lines_inc, r_width += r_width_inc) {

			tmp = l - (pos - offs);
			j = int(tmp / 128.0F);

			if (j < 0)
				j = 0;

			if (j >= n_textures)
				j = n_textures - 1;

			{
				p1x = (r_x1[i] + r_x2[i]) / 2;
				p1y = (r_y1[i] + r_y2[i]) / 2;
				p2x = (r_x1[i + 1] + r_x2[i + 1]) / 2;
				p2y = (r_y1[i + 1] + r_y2[i + 1]) / 2;
				inc = float(sqrt((p1x - p2x) * (p1x - p2x) + (p1y - p2y) * (p1y - p2y)));
			}

			if (textures != 0) {
				glBindTexture(GL_TEXTURE_2D, textures[j]);
				glBegin(GL_QUADS);
				glTexCoord2f(0, -pos / 128.0F);
				glVertex3f(r_x1[i], r_y1[i], r_z[i]);

				glTexCoord2f(0.75, -pos / 128.0F);
				glVertex3f(r_x2[i], r_y2[i], r_z[i]);

				glTexCoord2f(0.75, -(pos + inc) / 128.0F);
				glVertex3f(r_x2[i + 1], r_y2[i + 1], r_z[i + 1]);

				glTexCoord2f(0, -(pos + inc) / 128.0F);
				glVertex3f(r_x1[i + 1], r_y1[i + 1], r_z[i + 1]);
				glEnd();
			} 

			if (r_cx1 != 0) {
				lj = int(tmp / 128.0F);

				if (lj < 0)
					lj = 0;

				if (lj >= n_ltextures)
					lj = n_ltextures - 1;

				if (ltextures != 0) {
					glBindTexture(GL_TEXTURE_2D, ltextures[lj]);

					glBegin(GL_QUADS);
					glTexCoord2f(0, -pos / 128.0F);
					glVertex3f(r_cx1[i], r_cy1[i], r_z[i]);

					glTexCoord2f(1, -pos / 128.0F);
					glVertex3f(r_x1[i], r_y1[i], r_z[i]);

					glTexCoord2f(1, -(pos + inc) / 128.0F);
					glVertex3f(r_x1[i + 1], r_y1[i + 1], r_z[i + 1]);

					glTexCoord2f(0, -(pos + inc) / 128.0F);
					glVertex3f(r_cx1[i + 1], r_cy1[i + 1], r_z[i + 1]);
					glEnd();
				} 
			} 

			if (r_cx2 != 0) {
				rj = int(tmp / 128.0F);

				if (rj < 0)
					rj = 0;

				if (rj >= n_rtextures)
					rj = n_rtextures - 1;

				if (rtextures != 0) {
					glBindTexture(GL_TEXTURE_2D, rtextures[rj]);

					glBegin(GL_QUADS);
					glTexCoord2f(0, -pos / 128.0F);
					glVertex3f(r_cx2[i], r_cy2[i], r_z[i]);

					glTexCoord2f(1, -pos / 128.0F);
					glVertex3f(r_x2[i], r_y2[i], r_z[i]);

					glTexCoord2f(1, -(pos + inc) / 128.0F);
					glVertex3f(r_x2[i + 1], r_y2[i + 1], r_z[i + 1]);

					glTexCoord2f(0, -(pos + inc) / 128.0F);
					glVertex3f(r_cx2[i + 1], r_cy2[i + 1], r_z[i + 1]);
					glEnd();
				} 
			} 


			if (n_lines > 0) {
				/* Road lines: */
				n_lines_tmp1 = n_lines;
				n_lines_tmp2 = n_lines + n_lines_inc;

				if (n_lines_tmp1 < 1)
					n_lines_tmp1 = 1;

				if (n_lines_tmp2 < 1)
					n_lines_tmp2 = 1;

				int line;

				float line_start1, line_inc1, line_width1;

				float line_start2, line_inc2, line_width2;

				float lx1, lx2, ly1, ly2;

				float next_lx1, next_lx2, next_ly1, next_ly2;

				line_start1 = 1.0F / (n_lines_tmp1 + 1);

				line_inc1 = 1.0F / (n_lines_tmp1 + 1);

				line_width1 = (1.0F / r_width) * 2;

				line_start2 = 1.0F / (n_lines_tmp2 + 1);

				line_inc2 = 1.0F / (n_lines_tmp2 + 1);

				line_width2 = (1.0F / (r_width + r_width_inc)) * 2;

				for (line = 0;float(line) < max(n_lines_tmp1, n_lines_tmp2);line++) {
					if ((line_start1 - line_width1 > 0 && line_start1 + line_width1 < 1.0F) ||
					        (line_start2 - line_width2 > 0 && line_start2 + line_width2 < 1.0F)) {
						lx1 = r_x1[i] * (line_start1 - line_width1) + r_x2[i] * (1.0F - (line_start1 - line_width1));
						ly1 = r_y1[i] * (line_start1 - line_width1) + r_y2[i] * (1.0F - (line_start1 - line_width1));
						lx2 = r_x1[i] * (line_start1 + line_width1) + r_x2[i] * (1.0F - (line_start1 + line_width1));
						ly2 = r_y1[i] * (line_start1 + line_width1) + r_y2[i] * (1.0F - (line_start1 + line_width1));
						next_lx1 = r_x1[i + 1] * (line_start2 - line_width2) + r_x2[i + 1] * (1.0F - (line_start2 - line_width2));
						next_ly1 = r_y1[i + 1] * (line_start2 - line_width2) + r_y2[i + 1] * (1.0F - (line_start2 - line_width2));
						next_lx2 = r_x1[i + 1] * (line_start2 + line_width2) + r_x2[i + 1] * (1.0F - (line_start2 + line_width2));
						next_ly2 = r_y1[i + 1] * (line_start2 + line_width2) + r_y2[i + 1] * (1.0F - (line_start2 + line_width2));

						glBindTexture(GL_TEXTURE_2D, line_texture);
						glBegin(GL_QUADS);
						glTexCoord2f(0, -pos / 128.0F);
						glVertex3f(lx1, ly1, r_z[i]);

						glTexCoord2f(1.0F / 32.0F, -pos / 128.0F);
						glVertex3f(lx2, ly2, r_z[i]);

						glTexCoord2f(1.0F / 32.0F, -(pos + inc) / 128.0F);
						glVertex3f(next_lx2, next_ly2, r_z[i + 1]);

						glTexCoord2f(0, -(pos + inc) / 128.0F);
						glVertex3f(next_lx1, next_ly1, r_z[i + 1]);
						glEnd();

					} 

					line_start1 += line_inc1;

					line_start2 += line_inc2;
				} 


			} 


			pos += inc;
		} 


		glDisable(GL_TEXTURE_2D);

	} 
} 


void CRoadPiece::draw(float orig_x, float orig_y, float offs, int n_textures, GLuint *textures, int n_ltextures, GLuint *ltextures, int n_rtextures, GLuint *rtextures, GLuint line_texture)
{
	glPushMatrix();
	glTranslatef(orig_x, orig_y, 0);

	draw(offs, n_textures, textures, n_ltextures, ltextures, n_rtextures, rtextures, line_texture);

	glPopMatrix();
} 


void CRoadPiece::internal_draw(void)
{
	// draw_linear();
	draw_circular_adapted();

} 




void CRoadPiece::draw_linear(void)
{
	int i;
	bool first = true;

	/* Compute the path: linear pieces have simply two path points */
	path_points = 2;
	path_x = new float[2];
	path_y = new float[2];
	path_z = new float[2];
	path_a = new float[2];
	path_w = new float[2];
	path_x[0] = float(x1);
	path_x[1] = float(x2);
	path_y[0] = float(y1);
	path_y[1] = float(y2);
	path_z[0] = float(z1);
	path_z[1] = float(z2);
	// path_w[0]=float(w1)+(lchicane1+rchicane1)/2.0F;
	// path_w[1]=float(w2)+(lchicane2+rchicane2)/2.0F;
	path_w[0] = float(w1);
	path_w[1] = float(w2);
	{
		float s = float(x2 - x1);
		float c = float(y2 - y1);
		float n = float(sqrt(s * s + c * c));

		if (n != 0 && w1 == w2) {
			float a = float((atan2(c / n, s / n) * 180) / M_PI);
			path_a[0] = a;
			path_a[1] = a;
		} else {
			path_a[0] = float(a1);
			path_a[1] = float(a2);
		} 
	}

	/* Draw the road piece: */
	float t_x = float(x1), t_y = float(y1), t_z = float(z1);
	float t_rx, t_ry;
	float t_lcrx, t_lcry;
	float t_rcrx, t_rcry;
	float t_a = float(a1), t_w = float(w1);
	float t_lchicane = lchicane1;
	float t_rchicane = rchicane1;
	float dist = float(sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));

	steps = int((dist / 16) + 1);

	float t_a_inc = (a2 - a1) / float(steps);
	float t_w_inc = (w2 - w1) / float(steps);
	float t_lchicane_inc = (lchicane2 - lchicane1) / float(steps);
	float t_rchicane_inc = (rchicane2 - rchicane1) / float(steps);
	float t_vx = (x2 - x1) / float(steps);
	float t_vy = (y2 - y1) / float(steps);
	float t_vz = (z2 - z1) / float(steps);

	r_x1 = new float[steps + 1];
	r_y1 = new float[steps + 1];
	r_x2 = new float[steps + 1];
	r_y2 = new float[steps + 1];
	r_z = new float[steps + 1];

	if (lchicane1 > 0 || lchicane2 > 0 || rchicane1 > 0 || rchicane2 > 0) {
		r_cx1 = new float[steps + 1];
		r_cy1 = new float[steps + 1];
		r_cx2 = new float[steps + 1];
		r_cy2 = new float[steps + 1];
	} 

	for (i = 0;i <= steps;i++) {

		t_rx = float(t_w * cos((t_a + 90) * M_PI / 180) / 2);
		t_ry = float(t_w * sin((t_a + 90) * M_PI / 180) / 2);

		r_x1[i] = t_x + t_rx;
		r_y1[i] = t_y + t_ry;

		r_x2[i] = t_x - t_rx;
		r_y2[i] = t_y - t_ry;

		r_z[i] = t_z;

		if (first) {
			bbox[0] = min(r_x1[i], r_x2[i]);
			bbox[1] = min(r_y1[i], r_y2[i]);
			bbox[2] = max(r_x1[i], r_x2[i]);
			bbox[3] = max(r_y1[i], r_y2[i]);
			first = false;
		} else {
			if (r_x1[i] < bbox[0])
				bbox[0] = r_x1[i];

			if (r_x2[i] < bbox[0])
				bbox[0] = r_x2[i];

			if (r_y1[i] < bbox[1])
				bbox[1] = r_y1[i];

			if (r_y2[i] < bbox[1])
				bbox[1] = r_y2[i];

			if (r_x1[i] > bbox[2])
				bbox[2] = r_x1[i];

			if (r_x2[i] > bbox[2])
				bbox[2] = r_x2[i];

			if (r_y1[i] > bbox[3])
				bbox[3] = r_y1[i];

			if (r_y2[i] > bbox[3])
				bbox[3] = r_y2[i];
		} 


		if (lchicane1 > 0 || lchicane2 > 0 || rchicane1 > 0 || rchicane2 > 0) {
			t_lcrx = float(t_lchicane * cos((t_a + 90) * M_PI / 180) / 2);
			t_lcry = float(t_lchicane * sin((t_a + 90) * M_PI / 180) / 2);
			t_rcrx = float(t_rchicane * cos((t_a + 90) * M_PI / 180) / 2);
			t_rcry = float(t_rchicane * sin((t_a + 90) * M_PI / 180) / 2);

			r_cx1[i] = r_x1[i] + t_lcrx;
			r_cy1[i] = r_y1[i] + t_lcry;

			r_cx2[i] = r_x2[i] - t_rcrx;
			r_cy2[i] = r_y2[i] - t_rcry;

			if (first) {
				bbox[0] = min(r_cx1[i], r_cx2[i]);
				bbox[1] = min(r_cy1[i], r_cy2[i]);
				bbox[2] = max(r_cx1[i], r_cx2[i]);
				bbox[3] = max(r_cy1[i], r_cy2[i]);
				first = false;
			} else {
				if (r_cx1[i] < bbox[0])
					bbox[0] = r_cx1[i];

				if (r_cx2[i] < bbox[0])
					bbox[0] = r_cx2[i];

				if (r_cy1[i] < bbox[1])
					bbox[1] = r_cy1[i];

				if (r_cy2[i] < bbox[1])
					bbox[1] = r_cy2[i];

				if (r_cx1[i] > bbox[2])
					bbox[2] = r_cx1[i];

				if (r_cx2[i] > bbox[2])
					bbox[2] = r_cx2[i];

				if (r_cy1[i] > bbox[3])
					bbox[3] = r_cy1[i];

				if (r_cy2[i] > bbox[3])
					bbox[3] = r_cy2[i];
			} 
		} 

		t_x += t_vx;

		t_y += t_vy;

		t_z += t_vz;

		t_a += t_a_inc;

		t_w += t_w_inc;

		t_lchicane += t_lchicane_inc;

		t_rchicane += t_rchicane_inc;
	} // for
} 




void CRoadPiece::draw_circular_adapted(void)
{
	int i;

	if (fabs(a1 - a2) < 10) {
		draw_linear();
		return ;
	} 

	/* compute the size of the road piece: */
	float t_x = float(x1), t_y = float(y1), t_z = float(z1);

	float t_rx, t_ry;

	float t_a = float(a1), t_w = float(w1);

	float dist = float(sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));

	steps = int((dist / 16) + 1);

	float t_w_inc = (w2 - w1) / float(steps);

	float t_z_inc = (z2 - z1) / float(steps);

	float v1[3] = {0, 0, 0}, v2[3] = {0, 0, 0};

	v1[0] = float(cos(a1 * M_PI / 180));
	v1[1] = float(sin(a1 * M_PI / 180));
	v2[0] = float(cos(a2 * M_PI / 180));
	v2[1] = float(sin(a2 * M_PI / 180));

	/* Bisectriz: */
	float pm[3] = {float((x1 + x2) / 2), float((y1 + y2) / 2), 0};
	float vm[3] = {float( -(y2 - y1)), float(x2 - x1), 0};
	float vmn = float(sqrt(vm[0] * vm[0] + vm[1] * vm[1]));
	vm[0] /= vmn;
	vm[1] /= vmn;

	/* Centro de la circunferencia: */
	float p1[3] = {float(x1), float(y1), 0}, p2[3] = {float(x2), float(y2), 0};

	float pv1[3] = { -v1[1], v1[0], 0};
	float pv2[3] = { -v2[1], v2[0], 0};
	float center[3] = {0, 0, 0};
	float c1, c2;
	int ndata = 0;

	if (LineLineCollision(p1, pv1, pm, vm, &c1, &c2)) {
		center[0] += p1[0] + c1 * pv1[0];
		center[1] += p1[1] + c1 * pv1[1];
		ndata++;
	} 

	if (LineLineCollision(p2, pv2, pm, vm, &c1, &c2)) {
		center[0] += p2[0] + c1 * pv2[0];
		center[1] += p2[1] + c1 * pv2[1];
		ndata++;
	} 

	if (ndata == 0) {
		draw_linear();
		return ;
	} 

	center[0] /= ndata;

	center[1] /= ndata;

	/* calcular el ángulo inicial, el ángulo final y la dirección: */
	float ta1 = atan2f(y1 - center[1], x1 - center[0]);

	float ta2 = atan2f(y2 - center[1], x2 - center[0]);

	float t_a_inc;

	float radius = float(sqrt((x1 - center[0]) * (x1 - center[0]) + (y1 - center[1]) * (y1 - center[1])));

	if (radius > 10*dist) {
		draw_linear();
		return ;
	} 

	float d1 = ta2 - ta1;

	float d2 = ta1 - ta2;

	while (d1 < 0)
		d1 += float(M_PI * 2);

	while (d2 < 0)
		d2 += float(M_PI * 2);

	while (d1 > M_PI*2)
		d1 -= float(M_PI * 2);

	while (d2 > M_PI*2)
		d2 -= float(M_PI * 2);

	bool first = true;

	float t_lchicane = 0, t_lchicane_inc = 0;

	float t_rchicane = 0, t_rchicane_inc = 0;

	float t_crx, t_cry;

	float t_pa, t_pa_inc; /* path angle */

	if (d1 < d2) {
		t_a = ta1;
		t_a_inc = d1 / steps;
		t_pa = float(a1);

		if (fabs(a2 - a1) > 180) {
			if (a2 > a1) {
				t_pa_inc = ((a2 - 360) - a1) / float(steps);
			} else {
				t_pa_inc = (a2 - (a1 - 360)) / float(steps);
			} 
		} else {
			t_pa_inc = (a2 - a1) / float(steps);
		} 

		t_lchicane = lchicane1;

		t_rchicane = rchicane1;

		t_w = float(w1);

		t_z = float(z1);

		t_w_inc = (w2 - w1) / float(steps);

		t_z_inc = (z2 - z1) / float(steps);

		t_lchicane_inc = (lchicane2 - lchicane1) / float(steps);

		t_rchicane_inc = (rchicane2 - rchicane1) / float(steps);
	} else {
		t_a = ta2;
		t_a_inc = d2 / steps;
		t_pa = float(a2);

		if (fabs(a1 - a2) > 180) {
			if (a1 > a2) {
				t_pa_inc = ((a1 - 360) - a2) / float(steps);
			} else {
				t_pa_inc = (a1 - (a2 - 360)) / float(steps);
			} 
		} else {
			t_pa_inc = (a1 - a2) / float(steps);
		} 

		t_lchicane = lchicane2;

		t_rchicane = rchicane2;

		t_w = float(w2);

		t_z = float(z2);

		t_w_inc = (w1 - w2) / float(steps);

		t_z_inc = (z1 - z2) / float(steps);

		t_lchicane_inc = (lchicane1 - lchicane2) / float(steps);

		t_rchicane_inc = (rchicane1 - rchicane2) / float(steps);
	} 

	r_x1 = new float[steps + 1];

	r_y1 = new float[steps + 1];

	r_x2 = new float[steps + 1];

	r_y2 = new float[steps + 1];

	r_z = new float[steps + 1];

	if (lchicane2 > 0 || lchicane1 > 0 || rchicane2 > 0 || rchicane1 > 0) {
		r_cx1 = new float[steps + 1];
		r_cy1 = new float[steps + 1];
		r_cx2 = new float[steps + 1];
		r_cy2 = new float[steps + 1];
	} 

	/* Compute the path: */
	path_points = steps + 1;

	path_x = new float[steps + 1];

	path_y = new float[steps + 1];

	path_z = new float[steps + 1];

	path_a = new float[steps + 1];

	path_w = new float[steps + 1];

	for (i = 0;i <= steps;i++, t_a += t_a_inc, t_w += t_w_inc, t_z += t_z_inc, t_lchicane += t_lchicane_inc, t_rchicane += t_rchicane_inc, t_pa += t_pa_inc) {
		t_x = float(center[0] + radius * cos(t_a));
		t_y = float(center[1] + radius * sin(t_a));

		t_rx = float(t_w * cos((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);
		t_ry = float(t_w * sin((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);


		r_x1[i] = t_x + t_rx;
		r_y1[i] = t_y + t_ry;

		r_x2[i] = t_x - t_rx;
		r_y2[i] = t_y - t_ry;

		r_z[i] = t_z;

		if (first) {
			bbox[0] = min(r_x1[i], r_x2[i]);
			bbox[1] = min(r_y1[i], r_y2[i]);
			bbox[2] = max(r_x1[i], r_x2[i]);
			bbox[3] = max(r_y1[i], r_y2[i]);
			first = false;
		} else {
			if (r_x1[i] < bbox[0])
				bbox[0] = r_x1[i];

			if (r_x2[i] < bbox[0])
				bbox[0] = r_x2[i];

			if (r_y1[i] < bbox[1])
				bbox[1] = r_y1[i];

			if (r_y2[i] < bbox[1])
				bbox[1] = r_y2[i];

			if (r_x1[i] > bbox[2])
				bbox[2] = r_x1[i];

			if (r_x2[i] > bbox[2])
				bbox[2] = r_x2[i];

			if (r_y1[i] > bbox[3])
				bbox[3] = r_y1[i];

			if (r_y2[i] > bbox[3])
				bbox[3] = r_y2[i];
		} 


		if (lchicane2 > 0 || lchicane1 > 0 || rchicane2 > 0 || rchicane1 > 0) {
			t_crx = float(t_lchicane * cos((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);
			t_cry = float(t_lchicane * sin((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);

			r_cx1[i] = r_x1[i] + t_crx;
			r_cy1[i] = r_y1[i] + t_cry;

			t_crx = float(t_rchicane * cos((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);
			t_cry = float(t_rchicane * sin((t_pa * M_PI) / 180.0F + M_PI / 2) / 2);
			r_cx2[i] = r_x2[i] - t_crx;
			r_cy2[i] = r_y2[i] - t_cry;


			if (first) {
				bbox[0] = min(r_cx1[i], r_cx2[i]);
				bbox[1] = min(r_cy1[i], r_cy2[i]);
				bbox[2] = max(r_cx1[i], r_cx2[i]);
				bbox[3] = max(r_cy1[i], r_cy2[i]);
				first = false;
			} else {
				if (r_cx1[i] < bbox[0])
					bbox[0] = r_cx1[i];

				if (r_cx2[i] < bbox[0])
					bbox[0] = r_cx2[i];

				if (r_cy1[i] < bbox[1])
					bbox[1] = r_cy1[i];

				if (r_cy2[i] < bbox[1])
					bbox[1] = r_cy2[i];

				if (r_cx1[i] > bbox[2])
					bbox[2] = r_cx1[i];

				if (r_cx2[i] > bbox[2])
					bbox[2] = r_cx2[i];

				if (r_cy1[i] > bbox[3])
					bbox[3] = r_cy1[i];

				if (r_cy2[i] > bbox[3])
					bbox[3] = r_cy2[i];
			} 
		} 

		path_x[i] = t_x;

		path_y[i] = t_y;

		path_z[i] = t_z;

		path_a[i] = t_pa;

		//  path_w[i]=t_w+(t_lchicane+t_rchicane)/2;
		path_w[i] = t_w;
	} // for

} 



bool CRoadPiece::inside_test(float x, float y)
{
	float d = 0;

	if (r_x1 == 0)
		internal_draw();

	/* bounding box check: */
	if (x < bbox[0] || y < bbox[1] ||
	        x >= bbox[2] ||
	        y >= bbox[3])
		return false;

	/* pixel perfect check: */
	if (!distance_to_road(x, y, &d) ||
	        d > 0)
		return false;

	return true;
} 


bool CRoadPiece::inside_test(C2DCMC *cmc)
{
	/* Note: this function assumes that the "cmc" is "little" compared with the road piece. */

	if (inside_test(cmc->x[0], cmc->y[0]) ||
	        inside_test(cmc->x[0], cmc->y[1]) ||
	        inside_test(cmc->x[1], cmc->y[0]) ||
	        inside_test(cmc->x[1], cmc->y[1]))
		return true;

	return false;
} 


bool CRoadPiece::get_path_position(float x, float y, float *px, float *py, float *pz, float *pa, float *pw, float *distance)
{
	int closest = 0;
	int i;
	float d, min_d = -1;
	bool f1 = false, f2 = false;
	float l1, l2;

	for (i = 0;i < path_points;i++) {
		d = (x - path_x[i]) * (x - path_x[i]) + (y - path_y[i]) * (y - path_y[i]);

		if (min_d == -1 ||
		        d < min_d) {
			closest = i;
			min_d = d;
		} 
	} 

	/* test the position in the interval: closest -> closest+1 */
	if (closest < path_points - 1) {
		float v[2] = {path_x[closest + 1] - path_x[closest], path_y[closest + 1] - path_y[closest]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest]) * w[1] + w[0] * (y - path_y[closest]);

		if (d != 0) {
			f1 = true;
			l1 = n / d;
		} 
	} 

	/* test the position in the interval: closest -> closest-1 */
	if (closest > 0) {
		float v[2] = {path_x[closest] - path_x[closest - 1], path_y[closest] - path_y[closest - 1]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest - 1]) * w[1] + w[0] * (y - path_y[closest - 1]);

		if (d != 0) {
			f2 = true;
			l2 = n / d;
		} 
	} 

	/* choose the better point: */
	if (f1 || f2) {
		if ((f1 && l1 >= 0 && l1 <= 1) ||
		        !f2) {
			*px = path_x[closest] * (1 - l1) + path_x[closest + 1] * l1;
			*py = path_y[closest] * (1 - l1) + path_y[closest + 1] * l1;
			*pz = path_z[closest] * (1 - l1) + path_z[closest + 1] * l1;
			*pa = path_a[closest] * (1 - l1) + path_a[closest + 1] * l1;
			*pw = path_w[closest] * (1 - l1) + path_w[closest + 1] * l1;

			if (l1 < 0)
				*pa = path_a[closest];

			if (l1 > 1)
				*pa = path_a[closest + 1];

			if (l1 >= 0 && l1 <= 1) {
				*distance = float(sqrt((*px - x) * (*px - x) + (*py - y) * (*py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			return true;
		} 

		if ((f2 && l2 >= 0 && l2 <= 1) ||
		        !f1) {
			*px = path_x[closest - 1] * (1 - l2) + path_x[closest] * l2;
			*py = path_y[closest - 1] * (1 - l2) + path_y[closest] * l2;
			*pz = path_z[closest - 1] * (1 - l2) + path_z[closest] * l2;
			*pa = path_a[closest - 1] * (1 - l2) + path_a[closest] * l2;
			*pw = path_w[closest - 1] * (1 - l2) + path_w[closest] * l2;

			if (l2 < 0)
				*pa = path_a[closest - 1];

			if (l2 > 1)
				*pa = path_a[closest];

			if (l2 >= 0 && l2 <= 1) {
				*distance = float(sqrt((*px - x) * (*px - x) + (*py - y) * (*py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			return true;
		} 

		if (f1) {
			*px = path_x[closest] * (1 - l1) + path_x[closest + 1] * l1;
			*py = path_y[closest] * (1 - l1) + path_y[closest + 1] * l1;
			*pz = path_z[closest] * (1 - l1) + path_z[closest + 1] * l1;
			*pa = path_a[closest] * (1 - l1) + path_a[closest + 1] * l1;
			*pw = path_w[closest] * (1 - l1) + path_w[closest + 1] * l1;

			if (l1 < 0)
				*pa = path_a[closest];

			if (l1 > 1)
				*pa = path_a[closest + 1];

			if (l1 >= 0 && l1 <= 1) {
				*distance = float(sqrt((*px - x) * (*px - x) + (*py - y) * (*py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			return true;
		} 

		if (f2) {
			*px = path_x[closest - 1] * (1 - l2) + path_x[closest] * l2;
			*py = path_y[closest - 1] * (1 - l2) + path_y[closest] * l2;
			*py = path_z[closest - 1] * (1 - l2) + path_z[closest] * l2;
			*pa = path_a[closest - 1] * (1 - l2) + path_a[closest] * l2;
			*pw = path_w[closest - 1] * (1 - l2) + path_w[closest] * l2;

			if (l2 < 0)
				*pa = path_a[closest - 1];

			if (l2 > 1)
				*pa = path_a[closest];

			if (l2 >= 0 && l2 <= 1) {
				*distance = float(sqrt((*px - x) * (*px - x) + (*py - y) * (*py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			return true;
		} 
	} 

	return false;
} 


bool CRoadPiece::distance_to_road(float x, float y, float *distance)
{
	int closest = 0;
	int i;
	float d, min_d = -1;
	bool f1 = false, f2 = false;
	float l1, l2;
	float px, py, pw;

	for (i = 0;i < path_points;i++) {
		d = (x - path_x[i]) * (x - path_x[i]) + (y - path_y[i]) * (y - path_y[i]);

		if (min_d == -1 ||
		        d < min_d) {
			closest = i;
			min_d = d;
		} 
	} 

	/* test the position in the interval: closest -> closest+1 */
	if (closest < path_points - 1) {
		float v[2] = {path_x[closest + 1] - path_x[closest], path_y[closest + 1] - path_y[closest]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest]) * w[1] + w[0] * (y - path_y[closest]);

		if (d != 0) {
			f1 = true;
			l1 = n / d;
		} 
	} 

	/* test the position in the interval: closest -> closest-1 */
	if (closest > 0) {
		float v[2] = {path_x[closest] - path_x[closest - 1], path_y[closest] - path_y[closest - 1]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest - 1]) * w[1] + w[0] * (y - path_y[closest - 1]);

		if (d != 0) {
			f2 = true;
			l2 = n / d;
		} 
	} 

	/* choose the better point: */
	if (f1 || f2) {
		if ((f1 && l1 >= 0 && l1 <= 1) ||
		        !f2) {
			px = path_x[closest] * (1 - l1) + path_x[closest + 1] * l1;
			py = path_y[closest] * (1 - l1) + path_y[closest + 1] * l1;
			pw = path_w[closest] * (1 - l1) + path_w[closest + 1] * l1;

			if (l1 < 0)
				pw = path_w[closest];

			if (l1 > 1)
				pw = path_w[closest + 1];

			if (l1 >= 0 && l1 <= 1) {
				*distance = float(sqrt((px - x) * (px - x) + (py - y) * (py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			*distance -= pw;

			if (*distance < 0)
				*distance = 0;

			return true;
		} 

		if ((f2 && l2 >= 0 && l2 <= 1) ||
		        !f1) {
			px = path_x[closest - 1] * (1 - l2) + path_x[closest] * l2;
			py = path_y[closest - 1] * (1 - l2) + path_y[closest] * l2;
			pw = path_w[closest - 1] * (1 - l2) + path_w[closest] * l2;

			if (l2 < 0)
				pw = path_w[closest - 1];

			if (l2 > 1)
				pw = path_w[closest];

			if (l2 >= 0 && l2 <= 1) {
				*distance = float(sqrt((px - x) * (px - x) + (py - y) * (py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			*distance -= pw;

			if (*distance < 0)
				*distance = 0;

			return true;
		} 

		if (f1) {
			px = path_x[closest] * (1 - l1) + path_x[closest + 1] * l1;
			py = path_y[closest] * (1 - l1) + path_y[closest + 1] * l1;
			pw = path_w[closest] * (1 - l1) + path_w[closest + 1] * l1;

			if (l1 < 0)
				pw = path_w[closest];

			if (l1 > 1)
				pw = path_w[closest + 1];

			if (l1 >= 0 && l1 <= 1) {
				*distance = float(sqrt((px - x) * (px - x) + (py - y) * (py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			*distance -= pw;

			if (*distance < 0)
				*distance = 0;

			return true;
		} 

		if (f2) {
			px = path_x[closest - 1] * (1 - l2) + path_x[closest] * l2;
			py = path_y[closest - 1] * (1 - l2) + path_y[closest] * l2;
			pw = path_w[closest - 1] * (1 - l2) + path_w[closest] * l2;

			if (l2 < 0)
				pw = path_w[closest - 1];

			if (l2 > 1)
				pw = path_w[closest];

			if (l2 >= 0 && l2 <= 1) {
				*distance = float(sqrt((px - x) * (px - x) + (py - y) * (py - y)));
			} else {
				*distance = float(sqrt((path_x[closest] - x) * (path_x[closest] - x) + (path_y[closest] - y) * (path_y[closest] - y)));
			} 

			*distance -= pw;

			if (*distance < 0)
				*distance = 0;

			return true;
		} 
	} 

	return false;
} 


bool CRoadPiece::offset_from_road_center(float x, float y, float *offset, float *rangle)
{
	int closest = 0;
	int i;
	float d, min_d = -1;
	bool f1 = false, f2 = false;
	float l1, l2;
	float px, py, pa;

	for (i = 0;i < path_points;i++) {
		d = (x - path_x[i]) * (x - path_x[i]) + (y - path_y[i]) * (y - path_y[i]);

		if (min_d == -1 ||
		        d < min_d) {
			closest = i;
			min_d = d;
		} 
	} 

	/* test the position in the interval: closest -> closest+1 */
	if (closest < path_points - 1) {
		float v[2] = {path_x[closest + 1] - path_x[closest], path_y[closest + 1] - path_y[closest]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest]) * w[1] + w[0] * (y - path_y[closest]);

		if (d != 0) {
			f1 = true;
			l1 = n / d;
		} 
	} 

	/* test the position in the interval: closest -> closest-1 */
	if (closest > 0) {
		float v[2] = {path_x[closest] - path_x[closest - 1], path_y[closest] - path_y[closest - 1]};
		float w[2] = { -v[1], v[0]};
		float d, n;

		d = -v[0] * w[1] + v[1] * w[0];
		n = -(x - path_x[closest - 1]) * w[1] + w[0] * (y - path_y[closest - 1]);

		if (d != 0) {
			f2 = true;
			l2 = n / d;
		} 
	} 

	/* choose the better point: */
	if ((f1 && l1 >= 0 && l1 <= 1) || (f2 && l2 >= 0 && l2 <= 1)) {
		//  float side=1;
		if (f1 && l1 >= 0 && l1 <= 1) {
			px = path_x[closest] * (1 - l1) + path_x[closest + 1] * l1;
			py = path_y[closest] * (1 - l1) + path_y[closest + 1] * l1;
			pa = path_a[closest] * (1 - l1) + path_a[closest + 1] * l1;
		} 

		if (f2 && l2 >= 0 && l2 <= 1) {
			px = path_x[closest - 1] * (1 - l2) + path_x[closest] * l2;
			py = path_y[closest - 1] * (1 - l2) + path_y[closest] * l2;
			pa = path_a[closest - 1] * (1 - l2) + path_a[closest] * l2;
		} { 

			float dx = x - px;
			float dy = y - py;
			float rot_x = float(dx * cos(M_PI / 2 + (pa * M_PI) / 180.0F) + dy * sin(M_PI / 2 + (pa * M_PI) / 180.0F));

			*offset = rot_x;
			*rangle = pa;
			return true;
		}
	} 

	return false;
} 


float CRoadPiece::get_length(void)
{
	float dx, dy;
	int i;

	if (length_computed)
		return length;

	length = 0;

	for (i = 0;i < path_points - 1;i++) {
		dx = path_x[i + 1] - path_x[i];
		dy = path_y[i + 1] - path_y[i];
		length += float(sqrt(dx * dx + dy * dy));
	} 

	length_computed = true;

	return length;
} 
