#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"
#include "math.h"
#include "string.h"

#include "SDL.h"
#include "SDL_image.h"

#include "auxiliar.h"
#include "List.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifndef _WIN32
#ifndef HAVE_STRLWR

char *strlwr (char *str)
{
	char *ptr;

	ptr = str;

	while (*ptr) {
		*ptr = tolower (*ptr);
		ptr++;
	}

	return str;
}


#endif
#endif


bool substr(char *str, char *substr)
{
	char *ptr, *ptr2;

	while ((*str) != 0) {
		ptr = substr;
		ptr2 = str;

		while ((*ptr) != 0 &&
		        (*ptr2) != 0 &&
		        *ptr == *ptr2) {
			ptr++;
			ptr2++;
		} 

		if ((*ptr) == 0)
			return true;

		str++;
	} 

	return false;
} /* substr */


void pause(unsigned int time)
{
	unsigned int initt = SDL_GetTicks();

	while ((SDL_GetTicks() - initt) < time)
		;
} /* pause */



SDL_Surface *load_maskedimage(char *imagefile, char *maskfile, char *path)
{
	char name[256];

	SDL_Surface *res;
	SDL_Surface *tmp;
	SDL_Surface *mask;

	sprintf(name, "%s%s", path, imagefile);
	tmp = IMG_Load(name);
	sprintf(name, "%s%s", path, imagefile);
	mask = IMG_Load(name);

	if (tmp == 0 ||
	        mask == 0)
		return NULL;

	res = SDL_DisplayFormatAlpha(tmp);

	/* Aplicar la máscara: */
	{
		int x, y;
		Uint8 r, g, b, a;
		Uint32 v;

		for (y = 0;y < mask->h;y++) {
			for (x = 0;x < mask->w;x++) {
				SDL_LockSurface(res);
				v = getpixel(res, x, y);
				SDL_UnlockSurface(res);
				SDL_GetRGBA(v, res->format, &r, &g, &b, &a);
				SDL_LockSurface(mask);
				v = getpixel(mask, x, y);
				SDL_UnlockSurface(mask);

				if (v != 0)
					a = 255;
				else
					a = 0;

				v = SDL_MapRGBA(res->format, r, g, b, a);

				SDL_LockSurface(res);

				putpixel(res, x, y, v);

				SDL_UnlockSurface(res);
			} 
		} 
	}

	SDL_FreeSurface(tmp);

	SDL_FreeSurface(mask);

	return res;
} /* load_maskedimage */


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	SDL_Rect clip;
	int bpp = surface->format->BytesPerPixel;

	SDL_GetClipRect(surface, &clip);

	if (x < clip.x || x >= clip.x + clip.w ||
	        y < clip.y || y >= clip.y + clip.h)
		return ;

	if (x < 0 || x >= surface->w ||
	        y < 0 || y >= surface->h)
		return ;

	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {

		case 1:
			*p = (Uint8)pixel;
			break;

		case 2:
			*(Uint16 *)p = (Uint16)pixel;
			break;

		case 3:

			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (Uint8) ((pixel >> 16) & 0xff);
				p[1] = (Uint8) ((pixel >> 8) & 0xff);
				p[2] = (Uint8) (pixel & 0xff);
			} else {
				p[0] = (Uint8) (pixel & 0xff);
				p[1] = (Uint8) ((pixel >> 8) & 0xff);
				p[2] = (Uint8) ((pixel >> 16) & 0xff);
			}

			break;

		case 4:
			*(Uint32 *)p = pixel;
			break;
	}

}


void maximumpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	SDL_Rect clip;
	int bpp = surface->format->BytesPerPixel;
	Uint32 r, g, b, r2, g2, b2;
	Uint8 *p;

	SDL_GetClipRect(surface, &clip);

	if (x < clip.x || x >= clip.x + clip.w ||
	        y < clip.y || y >= clip.y + clip.h)
		return ;

	if (x < 0 || x >= surface->w ||
	        y < 0 || y >= surface->h || bpp != 4)
		return ;

	p = (Uint8 *) & pixel;

	r2 = p[ROFFSET];

	g2 = p[GOFFSET];

	b2 = p[BOFFSET];

	p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	r = p[ROFFSET];

	g = p[GOFFSET];

	b = p[BOFFSET];

	*((Uint32 *)p) = SDL_MapRGB(surface->format, (Uint8) (max(r, r2)), (Uint8) (max(g, g2)), (Uint8) (max(b, b2)));
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;

	if (x < 0 || x >= surface->w ||
	        y < 0 || y >= surface->h)
		return 0;

	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {

		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:

			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(Uint32 *)p;

		default:
			return 0;
	}
}


void surface_shifter(SDL_Surface *surface, int r_inc, int g_inc, int b_inc, int a_inc, SDL_Rect *r)
{
	SDL_Rect r2;
	int i, x, y, offs;
	Uint8 rtable[256], gtable[256], btable[256], atable[256];
	Uint8 *pixels;
	SDL_Surface *tmp;

	if (r == 0) {
		r2.x = 0;
		r2.y = 0;
		r2.w = surface->w;
		r2.h = surface->h;
		r = &r2;
	} 

	if (surface->format->BytesPerPixel != 4 ||
	        (r_inc == 0 &&
	         g_inc == 0 &&
	         b_inc == 0 &&
	         a_inc == 0))
		return ;

	for (i = 0;i < 256;i++) {
		if (i + r_inc < 0)
			rtable[i] = 0;
		else if (i + r_inc > 255)
			rtable[i] = 255;
		else
			rtable[i] = i + r_inc;

		if (i + g_inc < 0)
			gtable[i] = 0;
		else if (i + g_inc > 255)
			gtable[i] = 255;
		else
			gtable[i] = i + g_inc;

		if (i + b_inc < 0)
			btable[i] = 0;
		else if (i + b_inc > 255)
			btable[i] = 255;
		else
			btable[i] = i + b_inc;

		if (i + a_inc < 0)
			atable[i] = 0;
		else if (i + a_inc > 255)
			atable[i] = 255;
		else
			atable[i] = i + a_inc;
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(surface, 0, tmp, 0);
		SDL_LockSurface(tmp);
		pixels = (Uint8 *)(tmp->pixels);
	} else {
		SDL_LockSurface(surface);
		pixels = (Uint8 *)(surface->pixels);
	} 

	for (y = r->y;y < r->y + r->h && y < surface->h;y++) {
		for (x = r->x, offs = y * surface->pitch + r->x * 4;x < r->x + r->w && x < surface->w;x++, offs += 4) {
			pixels[offs + ROFFSET] = rtable[pixels[offs + ROFFSET]];
			pixels[offs + GOFFSET] = gtable[pixels[offs + GOFFSET]];
			pixels[offs + BOFFSET] = btable[pixels[offs + BOFFSET]];
			pixels[offs + AOFFSET] = atable[pixels[offs + AOFFSET]];
		} 
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_UnlockSurface(tmp);
		SDL_BlitSurface(tmp, 0, surface, 0);
		SDL_FreeSurface(tmp);
	} else {
		SDL_UnlockSurface(surface);
	} 


} /* surface_shifter */


void surface_fader(SDL_Surface *surface, float r_factor, float g_factor, float b_factor, float a_factor, SDL_Rect *r)
{
	SDL_Rect r2;
	int i, x, y, offs;
	Uint8 rtable[256], gtable[256], btable[256], atable[256];
	Uint8 *pixels;
	SDL_Surface *tmp;

	if (r == 0) {
		r2.x = 0;
		r2.y = 0;
		r2.w = surface->w;
		r2.h = surface->h;
		r = &r2;
	} 

	if (surface->format->BytesPerPixel != 4 ||
	        (r_factor == 1.0 &&
	         g_factor == 1.0 &&
	         b_factor == 1.0 &&
	         a_factor == 1.0))
		return ;

	for (i = 0;i < 256;i++) {
		rtable[i] = (Uint8)(i * r_factor);
		gtable[i] = (Uint8)(i * g_factor);
		btable[i] = (Uint8)(i * b_factor);
		atable[i] = (Uint8)(i * a_factor);
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(surface, 0, tmp, 0);
		SDL_LockSurface(tmp);
		pixels = (Uint8 *)(tmp->pixels);
	} else {
		SDL_LockSurface(surface);
		pixels = (Uint8 *)(surface->pixels);
	} 

	for (y = r->y;y < r->y + r->h && y < surface->h;y++) {
		for (x = r->x, offs = y * surface->pitch + r->x * 4;x < r->x + r->w && x < surface->w;x++, offs += 4) {
			pixels[offs + ROFFSET] = rtable[pixels[offs + ROFFSET]];
			pixels[offs + GOFFSET] = gtable[pixels[offs + GOFFSET]];
			pixels[offs + BOFFSET] = btable[pixels[offs + BOFFSET]];
			pixels[offs + AOFFSET] = atable[pixels[offs + AOFFSET]];
		} 
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_UnlockSurface(tmp);
		SDL_BlitSurface(tmp, 0, surface, 0);
		SDL_FreeSurface(tmp);
	} else {
		SDL_UnlockSurface(surface);
	} 


} /* surface_fader */


void surface_fader_mask(SDL_Surface *surface, SDL_Surface *mask, float r_factor, float g_factor, float b_factor, float a_factor, SDL_Rect *r)
{
	SDL_Rect r2;
	int i, x, y, offs, offs2, sp, mp, sbpp, mbpp;
	Uint8 rtable[256], gtable[256], btable[256], atable[256];
	Uint8 *pixels, *pixels_mask;
	SDL_Surface *tmp, *tmp2;

	if (r == 0) {
		r2.x = 0;
		r2.y = 0;
		r2.w = surface->w;
		r2.h = surface->h;
		r = &r2;
	} 

	if (surface->format->BytesPerPixel != 4 ||
	        (r_factor == 1.0 &&
	         g_factor == 1.0 &&
	         b_factor == 1.0 &&
	         a_factor == 1.0))
		return ;

	for (i = 0;i < 256;i++) {
		rtable[i] = (Uint8)(i * r_factor);
		gtable[i] = (Uint8)(i * g_factor);
		btable[i] = (Uint8)(i * b_factor);
		atable[i] = (Uint8)(i * a_factor);
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(surface, 0, tmp, 0);
		SDL_LockSurface(tmp);
		pixels = (Uint8 *)(tmp->pixels);
		sp = tmp->pitch;
		sbpp = tmp->format->BytesPerPixel;
	} else {
		SDL_LockSurface(surface);
		pixels = (Uint8 *)(surface->pixels);
		sp = surface->pitch;
		sbpp = surface->format->BytesPerPixel;
	} 

	if ((mask->flags&SDL_HWSURFACE) != 0) {
		tmp2 = SDL_CreateRGBSurface(SDL_SWSURFACE, mask->w, mask->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(mask, 0, tmp2, 0);
		SDL_LockSurface(tmp2);
		pixels_mask = (Uint8 *)(tmp2->pixels);
		mp = tmp2->pitch;
		mbpp = tmp2->format->BytesPerPixel;
	} else {
		SDL_LockSurface(mask);
		pixels_mask = (Uint8 *)(mask->pixels);
		mp = mask->pitch;
		mbpp = mask->format->BytesPerPixel;
	} 

	for (y = r->y;y < r->y + r->h && y < surface->h;y++) {
		for (x = r->x, offs = y * sp + r->x * sbpp, offs2 = y * mp + r->x * mbpp;x < r->x + r->w && x < surface->w;x++, offs += sbpp, offs2 += mbpp) {
			if (pixels_mask[offs2 + ROFFSET] != 0 &&
			        pixels_mask[offs2 + GOFFSET] != 0 &&
			        pixels_mask[offs2 + BOFFSET] != 0) {
				pixels[offs + ROFFSET] = rtable[pixels[offs + ROFFSET]];
				pixels[offs + GOFFSET] = gtable[pixels[offs + GOFFSET]];
				pixels[offs + BOFFSET] = btable[pixels[offs + BOFFSET]];
				pixels[offs + AOFFSET] = atable[pixels[offs + AOFFSET]];
			} 
		} 
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_UnlockSurface(tmp);
		SDL_BlitSurface(tmp, 0, surface, 0);
		SDL_FreeSurface(tmp);
	} else {
		SDL_UnlockSurface(surface);
	} 

	if ((mask->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_FreeSurface(tmp2);
	} else {
		SDL_UnlockSurface(mask);
	} 
} /* surface_fader_mask */


void surface_shader(SDL_Surface *surface, float factor, int red, int green, int blue, int alpha, SDL_Rect *r)
{
	int x, y, offs;
	Uint8 *pixels;
	int ifactor = int(factor * 256), inv_ifactor;
	SDL_Surface *tmp;

	if (ifactor < 0)
		ifactor = 0;

	if (ifactor >= 256)
		ifactor = 256;

	inv_ifactor = 256 - ifactor;

	if (surface->format->BytesPerPixel != 4)
		return ;

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(surface, 0, tmp, 0);
		SDL_LockSurface(tmp);
		pixels = (Uint8 *)(tmp->pixels);
	} else {
		SDL_LockSurface(surface);
		pixels = (Uint8 *)(surface->pixels);
	} 

	if (r == 0) {
		for (y = 0;y < surface->h;y++) {
			for (x = 0, offs = y * surface->pitch;x < surface->w;x++, offs += 4) {
				if (red >= 0)
					pixels[offs + ROFFSET] = (red * ifactor + pixels[offs + ROFFSET] * (inv_ifactor)) >> 8;

				if (green >= 0)
					pixels[offs + GOFFSET] = (green * ifactor + pixels[offs + GOFFSET] * (inv_ifactor)) >> 8;

				if (blue >= 0)
					pixels[offs + BOFFSET] = (blue * ifactor + pixels[offs + BOFFSET] * (inv_ifactor)) >> 8;

				if (alpha >= 0)
					pixels[offs + AOFFSET] = (alpha * ifactor + pixels[offs + AOFFSET] * (inv_ifactor)) >> 8;
			} 
		} 
	} else {
		for (y = max(r->y, 0);y < surface->h && y < r->y + r->h;y++) {
			for (x = max(r->x, 0), offs = y * surface->pitch;x < surface->w && x < r->x + r->w;x++, offs += 4) {
				if (red >= 0)
					pixels[offs + ROFFSET] = (red * ifactor + pixels[offs + ROFFSET] * (inv_ifactor)) >> 8;

				if (green >= 0)
					pixels[offs + GOFFSET] = (green * ifactor + pixels[offs + GOFFSET] * (inv_ifactor)) >> 8;

				if (blue >= 0)
					pixels[offs + BOFFSET] = (blue * ifactor + pixels[offs + BOFFSET] * (inv_ifactor)) >> 8;

				if (alpha >= 0)
					pixels[offs + AOFFSET] = (alpha * ifactor + pixels[offs + AOFFSET] * (inv_ifactor)) >> 8;
			} 
		} 
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_UnlockSurface(tmp);
		SDL_BlitSurface(tmp, 0, surface, 0);
		SDL_FreeSurface(tmp);
	} else {
		SDL_UnlockSurface(surface);
	} 

} /* surface_shader */


void surface_bicolor(SDL_Surface *surface, float factor, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2, SDL_Rect *r)
{
	int x, y, offs;
	Uint8 *pixels;
	int ifactor = int(factor * 256), inv_ifactor;
	int bw_color;
	SDL_Surface *tmp;

	if (ifactor < 0)
		ifactor = 0;

	if (ifactor >= 256)
		ifactor = 256;

	inv_ifactor = 256 - ifactor;

	if (surface->format->BytesPerPixel != 4)
		return ;

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(surface, 0, tmp, 0);
		SDL_LockSurface(tmp);
		pixels = (Uint8 *)(tmp->pixels);
	} else {
		SDL_LockSurface(surface);
		pixels = (Uint8 *)(surface->pixels);
	} 

	if (r == 0) {
		for (y = 0;y < surface->h;y++) {
			for (x = 0, offs = y * surface->pitch;x < surface->w;x++, offs += 4) {
				bw_color = (74 * pixels[offs + ROFFSET] + 154 * pixels[offs + GOFFSET] + 28 * pixels[offs + BOFFSET]) >> 8;

				if (r1 >= 0 && r2 >= 0)
					pixels[offs + BOFFSET] = (((b1 * bw_color + b2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + BOFFSET] * (inv_ifactor)) >> 8;

				if (g1 >= 0 && g2 >= 0)
					pixels[offs + GOFFSET] = (((g1 * bw_color + g2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + GOFFSET] * (inv_ifactor)) >> 8;

				if (b1 >= 0 && b2 >= 0)
					pixels[offs + ROFFSET] = (((r1 * bw_color + r2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + ROFFSET] * (inv_ifactor)) >> 8;

				if (a1 >= 0 && a2 >= 0)
					pixels[offs + AOFFSET] = (((a1 * bw_color + a2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + AOFFSET] * (inv_ifactor)) >> 8;
			} 
		} 
	} else {
		for (y = max(r->y, 0);y < surface->h && y < r->y + r->h;y++) {
			for (x = max(r->x, 0), offs = y * surface->pitch;x < surface->w && x < r->x + r->w;x++, offs += 4) {
				bw_color = (74 * pixels[offs + ROFFSET] + 154 * pixels[offs + GOFFSET] + 28 * pixels[offs + BOFFSET]) >> 8;

				if (r1 >= 0 && r2 >= 0)
					pixels[offs + BOFFSET] = (((b1 * bw_color + b2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + BOFFSET] * (inv_ifactor)) >> 8;

				if (g1 >= 0 && g2 >= 0)
					pixels[offs + GOFFSET] = (((g1 * bw_color + g2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + GOFFSET] * (inv_ifactor)) >> 8;

				if (b1 >= 0 && b2 >= 0)
					pixels[offs + ROFFSET] = (((r1 * bw_color + r2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + ROFFSET] * (inv_ifactor)) >> 8;

				if (a1 >= 0 && a2 >= 0)
					pixels[offs + AOFFSET] = (((a1 * bw_color + a2 * (256 - bw_color)) >> 8) * ifactor + pixels[offs + AOFFSET] * (inv_ifactor)) >> 8;
			} 
		} 
	} 

	if ((surface->flags&SDL_HWSURFACE) != 0) {
		/* HARDWARE SURFACE!!!: */
		SDL_UnlockSurface(tmp);
		SDL_BlitSurface(tmp, 0, surface, 0);
		SDL_FreeSurface(tmp);
	} else {
		SDL_UnlockSurface(surface);
	} 

} /* surface_bicolor */


void draw_rectangle(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel)
{
	int i;

	SDL_LockSurface(surface);

	for (i = 0;i < w;i++) {
		if (x + i >= 0 && x + i < surface->w && y >= 0 && y < surface->h)
			putpixel(surface, x + i, y, pixel);

		if (x + i >= 0 && x + i < surface->w && y + h - 1 >= 0 && y + h < surface->h)
			putpixel(surface, x + i, y + h - 1, pixel);
	} 

	for (i = 0;i < h;i++) {
		if (x >= 0 && x < surface->w && y + i >= 0 && y + i < surface->h)
			putpixel(surface, x, y + i, pixel);

		if (x + w - 1 >= 0 && x + w < surface->w && y + i >= 0 && y + i < surface->h)
			putpixel(surface, x + w - 1, y + i, pixel);
	} 

	SDL_UnlockSurface(surface);

} /* rectangle */



void draw_rectangle_vp(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel, SDL_Rect *vp)
{
	int i;
	int x0, y0, x1, y1;

	SDL_LockSurface(surface);

	if (vp->x > 0)
		x0 = vp->x;
	else
		x0 = 0;

	if (vp->x + vp->w < surface->w)
		x1 = vp->x + vp->w;
	else
		x1 = surface->w;

	if (vp->y > 0)
		y0 = vp->y;
	else
		y0 = 0;

	if (vp->y + vp->h < surface->h)
		y1 = vp->y + vp->h;
	else
		y1 = surface->h;

	if (y >= y0 && y < y1)
		for (i = 0;i < w;i++)
			if (x + i >= x0 && x + i < x1)
				putpixel(surface, x + i, y, pixel);

	if (y + h - 1 >= y0 && y + h < y1)
		for (i = 0;i < w;i++)
			if (x + i >= x0 && x + i < x1)
				putpixel(surface, x + i, y + h - 1, pixel);

	if (y + i >= y0 && y + i < y1)
		for (i = 0;i < h;i++)
			if (x >= x0 && x < x1)
				putpixel(surface, x, y + i, pixel);

	if (y + i >= y0 && y + i < y1)
		for (i = 0;i < h;i++)
			if (x + w - 1 >= x0 && x + w < x1)
				putpixel(surface, x + w - 1, y + i, pixel);

	SDL_UnlockSurface(surface);

} /* rectangle */



void draw_line(SDL_Surface *sfc, int x1, int y1, int x2, int y2, Uint32 pixel)
{
	long incy, rincy, incx, errterm, a;
	long d_x, d_y;
	int act_x, act_y;

	act_x = x1;
	act_y = y1;
	errterm = 0;
	d_x = x2 - x1;
	d_y = y2 - y1;

	if (d_y < 0) {
		incy = -1;
		rincy = -1;
		d_y = -d_y;
	} else {
		incy = 1;
		rincy = 1;
	} 

	if (d_x < 0) {
		incx = -1;
		d_x = -d_x;
	} else {
		incx = 1;
	} 

	SDL_LockSurface(sfc);

	if (d_x > d_y) {
		/* xline */
		for (a = 0;a <= d_x;a++) {
			putpixel(sfc, act_x, act_y, pixel);
			errterm += d_y;

			if (errterm >= d_x) {
				errterm -= d_x;
				act_y += incy;
			} 

			act_x += incx;
		} 
	} else {
		/* yline */
		for (a = 0;a <= d_y;a++) {
			putpixel(sfc, act_x, act_y, pixel);
			errterm += d_x;

			if (errterm >= d_y) {
				errterm -= d_y;
				act_x += incx;
			} 

			act_y += incy;
		} 
	} 

	SDL_UnlockSurface(sfc);
} /* draw_line */


void draw_thick_line(SDL_Surface *sfc, int x1, int y1, int x2, int y2, Uint32 pixel, int thickness)
{
	long incy, rincy, incx, errterm, a;
	long d_x, d_y;
	int act_x, act_y;

	int dot_x1, dot_x2, i, j;

	dot_x1 = -(thickness / 2);
	dot_x2 = dot_x1 + thickness;

	act_x = x1;
	act_y = y1;
	errterm = 0;
	d_x = x2 - x1;
	d_y = y2 - y1;

	if (d_y < 0) {
		incy = -1;
		rincy = -1;
		d_y = -d_y;
	} else {
		incy = 1;
		rincy = 1;
	} 

	if (d_x < 0) {
		incx = -1;
		d_x = -d_x;
	} else {
		incx = 1;
	} 

	SDL_LockSurface(sfc);

	if (d_x > d_y) {
		/* xline */
		for (a = 0;a <= d_x;a++) {
			for (i = dot_x1;i < dot_x2;i++)
				for (j = dot_x1;j < dot_x2;j++)
					putpixel(sfc, act_x + i, act_y + j, pixel);

			errterm += d_y;

			if (errterm >= d_x) {
				errterm -= d_x;
				act_y += incy;
			} 

			act_x += incx;
		} 
	} else {
		/* yline */
		for (a = 0;a <= d_y;a++) {
			for (i = dot_x1;i < dot_x2;i++)
				for (j = dot_x1;j < dot_x2;j++)
					putpixel(sfc, act_x + i, act_y + j, pixel);

			errterm += d_x;

			if (errterm >= d_y) {
				errterm -= d_y;
				act_x += incx;
			} 

			act_y += incy;
		} 
	} 

	SDL_UnlockSurface(sfc);
} /* draw_thick_line */


void area_fill(SDL_Surface *sfc, int x, int y, Uint32 pixel)
{
	int *y_p, *x_p;
	List<int> x_l, y_l;
	Uint32 background;
	Uint8 *p = (Uint8 *)sfc->pixels;

	if (sfc->format->BytesPerPixel != 4)
		return ;

	SDL_LockSurface(sfc);

	background = ((Uint32 *)(p + y * sfc->pitch))[x];

	if (pixel == background) {
		SDL_UnlockSurface(sfc);
		return ;
	} 

	x_l.Add(new int(x));

	y_l.Add(new int(y));

	while (!x_l.EmptyP()) {
		x_p = x_l.ExtractIni();
		y_p = y_l.ExtractIni();

		if (((Uint32 *)(p + *y_p*sfc->pitch))[*x_p] == background) {
			((Uint32 *)(p + *y_p*sfc->pitch))[*x_p] = pixel;

			if (*x_p > 0) {
				x_l.Insert(new int((*x_p) - 1));
				y_l.Insert(new int((*y_p)));
			} 

			if (*x_p < sfc->w - 1) {
				x_l.Insert(new int((*x_p) + 1));
				y_l.Insert(new int((*y_p)));
			} 

			if (*y_p > 0) {
				x_l.Insert(new int((*x_p)));
				y_l.Insert(new int((*y_p) - 1));
			} 

			if (*y_p < sfc->h - 1) {
				x_l.Insert(new int((*x_p)));
				y_l.Insert(new int((*y_p) + 1));
			} 
		} 

		delete x_p;

		delete y_p;
	} 

	SDL_UnlockSurface(sfc);
} /* area_fill */


void fast_area_fill(SDL_Surface *sfc, int x, int y, Uint32 pixel)
{
	int *y_p, *x_p, x_tmp;
	List<int> x_l, y_l;
	Uint32 background;
	Uint8 *p = (Uint8 *)sfc->pixels;
	Uint32 *p2, *p3, *p4;

	if (sfc->format->BytesPerPixel != 4)
		return ;

	SDL_LockSurface(sfc);

	background = ((Uint32 *)(p + y * sfc->pitch))[x];

	if (pixel == background) {
		SDL_UnlockSurface(sfc);
		return ;
	} 

	x_l.Add(new int(x));

	y_l.Add(new int(y));

	while (!x_l.EmptyP()) {
		x_p = x_l.ExtractIni();
		y_p = y_l.ExtractIni();
		x = *x_p;
		y = *y_p;
		delete x_p;
		delete y_p;

		p2 = (Uint32 *)(p + y * sfc->pitch);

		if (p2[x] == background) {
			p3 = (Uint32 *)(p + (y - 1) * sfc->pitch);
			p4 = (Uint32 *)(p + (y + 1) * sfc->pitch);
			x_tmp = x;

			do {
				p2[x] = pixel;

				if (y > 0 && x > 0 && p3[x - 1] != background && p3[x] == background) {
					x_l.Insert(new int(x));
					y_l.Insert(new int(y - 1));
				} 

				if (y < sfc->h - 1 && x > 0 && p4[x - 1] != background && p4[x] == background) {
					x_l.Insert(new int(x));
					y_l.Insert(new int(y + 1));
				} 

				x--;
			} while (x >= 0 && p2[x] == background);

			x++;

			if (y > 0 && p3[x] == background) {
				x_l.Insert(new int(x));
				y_l.Insert(new int(y - 1));
			} 

			if (y < sfc->h - 1 && p4[x] == background) {
				x_l.Insert(new int(x));
				y_l.Insert(new int(y + 1));
			} 

			x = x_tmp + 1;

			while (x < sfc->w - 1 && p2[x] == background) {
				p2[x] = pixel;

				if (y > 0 && x > 0 && p3[x - 1] != background && p3[x] == background) {
					x_l.Insert(new int(x));
					y_l.Insert(new int(y - 1));
				} 

				if (y < sfc->h - 1 && x > 0 && p4[x - 1] != background && p4[x] == background) {
					x_l.Insert(new int(x));
					y_l.Insert(new int(y + 1));
				} 

				x++;
			} 
		} 
	} 

	SDL_UnlockSurface(sfc);
} /* fast_area_fill */


void surface_automatic_alpha(SDL_Surface *sfc)
{
	int i, j;
	Uint32 color;
	Uint8 r, g, b, a;

	for (i = 0;i < sfc->w;i++) {
		for (j = 0;j < sfc->h;j++) {
			SDL_LockSurface(sfc);
			color = getpixel(sfc, i, j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color, sfc->format, &r, &g, &b, &a);

			if (r != 0 || g != 0 || b != 0)
				a = 255;
			else
				a = 0;

			color = SDL_MapRGBA(sfc->format, r, g, b, a);

			SDL_LockSurface(sfc);

			putpixel(sfc, i, j, color);

			SDL_UnlockSurface(sfc);
		} 
	} 
} /* surface_automatic_alpha */


void surface_bw(SDL_Surface *sfc, int threshold)
{
	int i, j;
	Uint32 color;
	Uint8 r, g, b, a;

	for (i = 0;i < sfc->w;i++) {
		for (j = 0;j < sfc->h;j++) {
			SDL_LockSurface(sfc);
			color = getpixel(sfc, i, j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color, sfc->format, &r, &g, &b, &a);

			if (r >= threshold || g >= threshold || b >= threshold)
				a = 255;
			else
				a = 0;

			color = SDL_MapRGBA(sfc->format, a, a, a, a);

			SDL_LockSurface(sfc);

			putpixel(sfc, i, j, color);

			SDL_UnlockSurface(sfc);
		} 
	} 
} /* surface_bw */


void surface_mask_from_bitmap(SDL_Surface *sfc, SDL_Surface *mask, int x, int y)
{
	int i, j;
	int mean;
	Uint32 color;
	Uint8 r, g, b, a;

	for (i = 0;i < sfc->w;i++) {
		for (j = 0;j < sfc->h;j++) {
			SDL_LockSurface(mask);
			color = getpixel(mask, x + i, y + j);
			SDL_UnlockSurface(mask);
			SDL_GetRGBA(color, sfc->format, &r, &g, &b, &a);
			mean = (r + g + b) / 3;
			SDL_LockSurface(sfc);
			color = getpixel(sfc, i, j);
			SDL_UnlockSurface(sfc);
			SDL_GetRGBA(color, sfc->format, &r, &g, &b, &a);
			color = SDL_MapRGBA(sfc->format, r, g, b, mean);
			SDL_LockSurface(sfc);
			putpixel(sfc, i, j, color);
			SDL_UnlockSurface(sfc);
		} 
	} 
} /* surface_mask_from_bitmap */


bool segment_collision(int s1x1, int s1y1, int s1x2, int s1y2,
                       int s2x1, int s2y1, int s2x2, int s2y2,
                       int *cx, int *cy)
{
	float v1x, v1y;
	float v2x, v2y;
	float l1, l2;
	float det;

	v1x = float(s1x2 - s1x1);
	v1y = float(s1y2 - s1y1);
	v2x = float(s2x2 - s2x1);
	v2y = float(s2y2 - s2y1);

	det = v1y * v2x - v1x * v2y;

	if (det != 0) {
		l1 = ( v2x * (s2y1 - s1y1) - v2y * (s2x1 - s1x1) ) / det;
		l2 = ( v1x * (s2y1 - s1y1) - v1y * (s2x1 - s1x1) ) / det;

		if (l1 >= 0 && l1 <= 1 &&
		        l2 >= 0 && l2 <= 1) {
			*cx = int(s1x1 + v1x * l1);
			*cy = int(s1y1 + v1y * l1);
			return true;
		} 
	} else {
		if (s1y1 == s2y1) {
			*cx = s1x1;
			*cy = s1y1;
			return true;
		} 
	} 

	return false;
} /* segment_collision */

/*
void print_left(char *text,TTF_Font *font,int red,int g,int b,SDL_Surface *sfc,int x,int y)
{
 SDL_Surface *tmp;
 SDL_Rect r;
 SDL_Color c;
 c.r=red;
 c.g=g;
 c.b=b;

// tmp=TTF_RenderText_Blended(font,text,c);
 tmp=TTF_RenderText_Solid(font,text,c);

 if (tmp!=0) {
  r.x=x;
  r.y=y;
  r.w=tmp->w;
  r.h=tmp->h;
  SDL_BlitSurface(tmp,0,sfc,&r);
  SDL_FreeSurface(tmp);
 } // if
} // print_left


void print_centered(char *text,TTF_Font *font,int red,int g,int b,SDL_Surface *sfc,int x,int y)
{
 SDL_Surface *tmp;
 SDL_Rect r;
 SDL_Color c;
 c.r=red;
 c.g=g;
 c.b=b;

// tmp=TTF_RenderText_Blended(font,text,c);
 tmp=TTF_RenderText_Solid(font,text,c);

 if (tmp!=0) {
  r.x=x-tmp->w/2;
  r.y=y;
  r.w=tmp->w;
  r.h=tmp->h;
  SDL_BlitSurface(tmp,0,sfc,&r);
  SDL_FreeSurface(tmp);
 } // if
} // print_centered


int get_text_width(char *text, TTF_Font *font)
{
 int dx,dy;

 TTF_SizeText(font,text,&dx,&dy);

 return dx;
} // get_text_width




SDL_Surface *multiline_text_surface(char *text,int line_dist,TTF_Font *font,SDL_Color c)
{
 int i,j,y;
 int sizex=0,sizey=0;
 SDL_Surface *tmp,*tmp2;
 List<SDL_Surface> sfc_l;
 char text_tmp[256];

 i=j=0;

 while(text[i]!=0) {
  text_tmp[j]=text[i];
  if (text[i]=='\n') {
   text_tmp[j]=0;
   tmp=TTF_RenderText_Blended(font,text_tmp,c);
   if (tmp->w>sizex) sizex=tmp->w;

   if (sizey!=0) sizey+=line_dist;
   sizey+=tmp->h;

   sfc_l.Add(tmp);
   j=0;
  } else {
   j++;
  } // if
  i++;
  if (text[i]==0) {
   text_tmp[j]=0;
   tmp=TTF_RenderText_Blended(font,text_tmp,c);
   if (tmp->w>sizex) sizex=tmp->w;

   if (sizey!=0) sizey+=line_dist;
   sizey+=tmp->h;

   sfc_l.Add(tmp);
   j=0;
  } // if
 } // while

 tmp=SDL_CreateRGBSurface(0,sizex,sizey,32,0,0,0,AMASK);
 SDL_FillRect(tmp,0,0);
 SDL_SetAlpha(tmp,SDL_SRCALPHA,0);

 y=0;
 while(!sfc_l.EmptyP()) {
  SDL_Rect r;

  tmp2=sfc_l.ExtractIni();

  r.x=(sizex-tmp2->w)/2;
  r.y=y;
  r.w=tmp2->w;
  r.h=tmp2->h;
  SDL_SetAlpha(tmp2,0,0);
  SDL_BlitSurface(tmp2,0,tmp,&r);
  y+=tmp2->h;
  y+=line_dist;
  SDL_FreeSurface(tmp2);
 } // while

 return tmp;
} // multiline_text_surface


SDL_Surface *multiline_text_surface_limited(char *text,int line_dist,TTF_Font *font,SDL_Color c,int max_w)
{
 int i,j,y;
 int sizex=0,sizey=0;
 SDL_Surface *tmp,*tmp2;
 List<SDL_Surface> sfc_l;
 char text_tmp[256];
 int w,h;

 if (text==0) return 0;

 i=j=0;

 while(text[i]!=0) {
  text_tmp[j]=text[i];
  text_tmp[j+1]=0;

  TTF_SizeText(font,text_tmp,&w,&h);
  if (text[i]=='\n' || w>max_w) {
   text_tmp[j]=0;
   if (w>max_w) {
    int k=j,l=i-1;
    while(k>0 && text_tmp[k]!=' ') {
     k--;
     l--;
    } // while
    if (k>0) {
     text_tmp[k]=0;
     i=l;
    } else {
     i--;
    } // if
   } // if
   if (text_tmp[0]==0) {
    text_tmp[0]=' ';
    text_tmp[1]=0;
   } // if
   tmp=TTF_RenderText_Blended(font,text_tmp,c);
   if (tmp!=0) {
    if (tmp->w>sizex) sizex=tmp->w;

    if (sizey!=0) sizey+=line_dist;
    sizey+=tmp->h;

    sfc_l.Add(tmp);
   } // if
   j=0;
  } else {
   j++;
  } // if
  i++;
  if (text[i]==0) {
   text_tmp[j]=0;
   if (text_tmp[0]==0) {
    text_tmp[0]=' ';
    text_tmp[1]=0;
   } // if
   tmp=TTF_RenderText_Blended(font,text_tmp,c);
   if (tmp!=0) {
    if (tmp->w>sizex) sizex=tmp->w;

    if (sizey!=0) sizey+=line_dist;
    sizey+=tmp->h;

    sfc_l.Add(tmp);
   } // if
   j=0;
  } // if
 } // while

 if (text[0]==0) {
  text_tmp[0]=' ';
  text_tmp[1]=0;
  tmp=TTF_RenderText_Blended(font,text_tmp,c);
  if (tmp!=0) {
   if (tmp->w>sizex) sizex=tmp->w;

   if (sizey!=0) sizey+=line_dist;
   sizey+=tmp->h;

   sfc_l.Add(tmp);
  } // if
  j=0;
 } // if

 tmp=SDL_CreateRGBSurface(0,sizex,sizey,32,0,0,0,AMASK);
 SDL_FillRect(tmp,0,0);
 SDL_SetAlpha(tmp,SDL_SRCALPHA,0);

 y=0;
 while(!sfc_l.EmptyP()) {
  SDL_Rect r;

  tmp2=sfc_l.ExtractIni();

  r.x=(sizex-tmp2->w)/2;
  r.y=y;
  r.w=tmp2->w;
  r.h=tmp2->h;
  SDL_SetAlpha(tmp2,0,0);
  SDL_BlitSurface(tmp2,0,tmp,&r);
  y+=tmp2->h;
  y+=line_dist;
  SDL_FreeSurface(tmp2);
 } // while

 return tmp;
} // multiline_text_surface_limited





*/


void print_left_bmp(unsigned char *text, SDL_Surface *font, SDL_Surface *sfc, int x, int y, int spacing)
{
	if (text != 0) {
		int i, s;
		unsigned int c;
		s = font->w / 256;
		SDL_Rect orig, dest;

		orig.y = 0;
		orig.w = s;
		orig.h = font->h;
		dest.x = x;
		dest.y = y;
		dest.w = s;
		dest.h = font->h;

		for (i = 0;text[i] != 0;i++) {
			c = text[i];
			c *= s;
			orig.x = c;
			orig.w = s;
			orig.h = font->h;
			surface_blit_alpha(font, &orig, sfc, &dest);
			//   SDL_BlitSurface(font,&orig,sfc,&dest);
			dest.x += s + spacing;
		} 
	} 
} /* print_left_bmp */


void print_centered_bmp(unsigned char *text, SDL_Surface *font, SDL_Surface *sfc, int x, int y, int spacing)
{
	if (text != 0) {
		int i, s;
		unsigned int c;
		s = font->w / 256;
		SDL_Rect orig, dest;

		orig.y = 0;
		orig.w = s;
		orig.h = font->h;
		dest.x = x - (get_text_width_bmp(text, font, spacing)) / 2;
		dest.y = y;
		dest.w = s;
		dest.h = font->h;

		for (i = 0;text[i] != 0;i++) {
			c = text[i];
			c *= s;
			orig.x = c;
			orig.w = s;
			orig.h = font->h;
			surface_blit_alpha(font, &orig, sfc, &dest);
			//   SDL_BlitSurface(font,&orig,sfc,&dest);
			dest.x += s + spacing;
		} 
	} 
} /* print_centered_bmp */




int get_text_width_bmp(unsigned char *text, SDL_Surface *font, int spacing)
{
	int l1, l2;
	int s = font->w / 256;

	if (text != 0) {
		l1 = strlen((char *)text);
		l2 = l1 - 1;

		if (l2 < 0)
			l2 = 0;

		return l1*s + l2*spacing;
	} else {
		return 0;
	} 
} /* get_text_width_bmp */








void surface_blit_alpha(SDL_Surface *orig, SDL_Rect *o_r, SDL_Surface *dest, SDL_Rect *d_r)
{
	int i, j;
	SDL_Rect r1, r2;
	Uint8 *opixels, *dpixels;
	int a1, a2;
	int ap, app;

	if (orig->format->BytesPerPixel != 4 ||
	        dest->format->BytesPerPixel != 4)
		return ;

	if (o_r != 0) {
		r1 = *o_r;
	} else {
		r1.x = 0;
		r1.y = 0;
		r1.w = orig->w;
		r1.h = orig->h;
	} 

	if (d_r != 0) {
		r2 = *d_r;
	} else {
		r2.x = 0;
		r2.y = 0;
	} 

	if (r1.x < 0) {
		r1.w += r1.x;
		r1.x = 0;
	} 

	if (r1.x >= orig->w)
		return ;

	if (r1.y < 0) {
		r1.h += r1.y;
		r1.y = 0;
	} 

	if (r1.y >= orig->h)
		return ;

	if (r2.x < 0) {
		r1.w += r2.x;
		r2.x = 0;
	} 

	if (r2.x >= dest->w)
		return ;

	if (r2.y < 0) {
		r1.h += r2.y;
		r2.y = 0;
	} 

	if (r2.y >= dest->h)
		return ;

	if (r1.w <= 0)
		return ;

	if (r2.x + r1.w > dest->w)
		r1.w = dest->w - r2.x;

	if (r1.h <= 0)
		return ;

	if (r2.y + r1.h > dest->h)
		r1.h = dest->h - r2.y;

	SDL_LockSurface(orig);

	SDL_LockSurface(dest);

	for (i = 0;i < r1.h;i++) {
		opixels = (Uint8 *)orig->pixels + orig->pitch * (i + r1.y) + r1.x * 4;
		dpixels = (Uint8 *)dest->pixels + dest->pitch * (i + r2.y) + r2.x * 4;

		for (j = 0;j < r1.w;j++, opixels += 4, dpixels += 4) {
			a2 = opixels[AOFFSET]; //*alpha;

			if (a2 != 0) {
				a1 = dpixels[AOFFSET];
				//    a2/=255;
				ap = a1 + a2 - (a1 * a2) / 255;

				if (ap > 255)
					ap = 255;

				if (ap < 0)
					ap = 0;

				if (a2 == 0)
					app = 0;
				else
					app = ( -a2 * 255) / (((a1 * a2) / 255) - a1 - a2);

				if (app > 255)
					app = 255;

				if (app < 0)
					app = 0;

				dpixels[ROFFSET] = (opixels[ROFFSET] * app + dpixels[ROFFSET] * (255 - app)) / 255;

				dpixels[GOFFSET] = (opixels[GOFFSET] * app + dpixels[GOFFSET] * (255 - app)) / 255;

				dpixels[BOFFSET] = (opixels[BOFFSET] * app + dpixels[BOFFSET] * (255 - app)) / 255;

				dpixels[AOFFSET] = ap;
			} 
		} 
	} 

	SDL_UnlockSurface(orig);

	SDL_UnlockSurface(dest);
} /* surface_blit_alpha */


char get_key_ascii(int key)
{
	int i;
	char keyascii[SDLK_LAST];

	for (i = 0;i < SDLK_LAST;i++)
		keyascii[i] = 0;

	/* Fill in the blanks in keyascii */
	keyascii[SDLK_BACKSPACE] = '\b';

	keyascii[SDLK_TAB] = '\t';

	keyascii[SDLK_RETURN] = '\n';

	keyascii[SDLK_SPACE] = ' ';

	keyascii[SDLK_EXCLAIM] = '!';

	keyascii[SDLK_QUOTEDBL] = '\"';

	keyascii[SDLK_HASH] = '#';

	keyascii[SDLK_DOLLAR] = '$';

	keyascii[SDLK_AMPERSAND] = '&';

	keyascii[SDLK_QUOTE] = '\'';

	keyascii[SDLK_LEFTPAREN] = '(';

	keyascii[SDLK_RIGHTPAREN] = ')';

	keyascii[SDLK_ASTERISK] = '*';

	keyascii[SDLK_PLUS] = '+';

	keyascii[SDLK_COMMA] = ',';

	keyascii[SDLK_MINUS] = '-';

	keyascii[SDLK_PERIOD] = '.';

	keyascii[SDLK_SLASH] = '/';

	keyascii[SDLK_0] = '0';

	keyascii[SDLK_1] = '1';

	keyascii[SDLK_2] = '2';

	keyascii[SDLK_3] = '3';

	keyascii[SDLK_4] = '4';

	keyascii[SDLK_5] = '5';

	keyascii[SDLK_6] = '6';

	keyascii[SDLK_7] = '7';

	keyascii[SDLK_8] = '8';

	keyascii[SDLK_9] = '9';

	keyascii[SDLK_COLON] = ':';

	keyascii[SDLK_SEMICOLON] = ';';

	keyascii[SDLK_LESS] = '<';

	keyascii[SDLK_EQUALS] = '=';

	keyascii[SDLK_GREATER] = '>';

	keyascii[SDLK_QUESTION] = '?';

	keyascii[SDLK_AT] = '@';

	keyascii[SDLK_LEFTBRACKET] = '[';

	keyascii[SDLK_BACKSLASH] = '\\';

	keyascii[SDLK_RIGHTBRACKET] = ']';

	keyascii[SDLK_CARET] = '^';

	keyascii[SDLK_UNDERSCORE] = '_';

	keyascii[SDLK_BACKQUOTE] = '`';

	keyascii[SDLK_a] = 'a';

	keyascii[SDLK_b] = 'b';

	keyascii[SDLK_c] = 'c';

	keyascii[SDLK_d] = 'd';

	keyascii[SDLK_e] = 'e';

	keyascii[SDLK_f] = 'f';

	keyascii[SDLK_g] = 'g';

	keyascii[SDLK_h] = 'h';

	keyascii[SDLK_i] = 'i';

	keyascii[SDLK_j] = 'j';

	keyascii[SDLK_k] = 'k';

	keyascii[SDLK_l] = 'l';

	keyascii[SDLK_m] = 'm';

	keyascii[SDLK_n] = 'n';

	keyascii[SDLK_o] = 'o';

	keyascii[SDLK_p] = 'p';

	keyascii[SDLK_q] = 'q';

	keyascii[SDLK_r] = 'r';

	keyascii[SDLK_s] = 's';

	keyascii[SDLK_t] = 't';

	keyascii[SDLK_u] = 'u';

	keyascii[SDLK_v] = 'v';

	keyascii[SDLK_w] = 'w';

	keyascii[SDLK_x] = 'x';

	keyascii[SDLK_y] = 'y';

	keyascii[SDLK_z] = 'z';

	return keyascii[key];
} /* get_key_ascii */


bool save_float(FILE *fp, float f)
{
	unsigned char *p;
	p = (unsigned char *) & f;
	fputc(p[0], fp);
	fputc(p[1], fp);
	fputc(p[2], fp);
	fputc(p[3], fp);

	return true;
} /* save_float */


bool load_float(FILE *fp, float *f)
{
	unsigned char *p;
	p = (unsigned char *)f;
	p[0] = fgetc(fp);
	p[1] = fgetc(fp);
	p[2] = fgetc(fp);
	p[3] = fgetc(fp);

	return true;
} /* load_float */


bool save_int(FILE *fp, int f)
{
	unsigned char *p;
	p = (unsigned char *) & f;
	fputc(p[0], fp);
	fputc(p[1], fp);
	fputc(p[2], fp);
	fputc(p[3], fp);

	return true;
} /* save_int */


bool load_int(FILE *fp, int *f)
{
	unsigned char *p;
	p = (unsigned char *)f;
	p[0] = fgetc(fp);
	p[1] = fgetc(fp);
	p[2] = fgetc(fp);
	p[3] = fgetc(fp);

	return true;
} /* load_int */
