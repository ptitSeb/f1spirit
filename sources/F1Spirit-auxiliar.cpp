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
#include "string.h"
#include "math.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "auxiliar.h"
#include "List.h"

#include "sound.h"

#include "F1Spirit-auxiliar.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

SDL_Surface *frame_corner[4] = {0, 0, 0, 0};
SDL_Surface *frame_h[2] = {0, 0}, *frame_v[2] = {0, 0};

int linear_base [ 16] = {0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2};

int corner_base1[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 2,
                         0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2
                        };
int corner_base2[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
                         0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2
                        };


void free_auxiliar_menu_surfaces(void)
{
	int i;

	for (i = 0;i < 4;i++) {
		if (frame_corner[i] != 0)
			SDL_FreeSurface(frame_corner[i]);

		frame_corner[i] = 0;
	} 

	if (frame_h[0] != 0)
		SDL_FreeSurface(frame_h[0]);

	frame_h[0] = 0;

	if (frame_h[1] != 0)
		SDL_FreeSurface(frame_h[1]);

	frame_h[1] = 0;

	if (frame_v[0] != 0)
		SDL_FreeSurface(frame_v[0]);

	frame_v[0] = 0;

	if (frame_v[1] != 0)
		SDL_FreeSurface(frame_v[1]);

	frame_v[1] = 0;
} /* free_auxiliar_menu_surfaces */


SDL_Surface *load_bmp_font(char *filename, int first, int last)
{
	SDL_Surface *bitmap_font = 0;
	SDL_Surface *sfc = IMG_Load(filename);

	if (sfc != 0) {
		int w = (sfc->w / (last - first));
		bitmap_font = SDL_CreateRGBSurface(SDL_SWSURFACE, w * 256, sfc->h, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(bitmap_font, 0, 0);
		SDL_FillRect(bitmap_font, 0, 0);
	} 

	if (bitmap_font != 0) {
		int i, j, k, s;
		Uint32 color;
		s = sfc->w / (last - first);

		for (k = 0;k < 256;k++) {
			if (k >= first && k < last) {
				for (i = 0;i < s;i++) {
					for (j = 0;j < sfc->h;j++) {
						SDL_LockSurface(sfc);
						color = getpixel(sfc, (k - first) * s + i, j);
						SDL_UnlockSurface(sfc);
						SDL_LockSurface(bitmap_font);
						putpixel(bitmap_font, k*s + i, j, color);
						SDL_UnlockSurface(bitmap_font);
					} 
				} 
			} 
		} 
	} 

	return bitmap_font;
} /* load_bmp_font */


SDL_Surface *draw_menu(int MAX_OPTIONS, char *title, char *options, int *option_type, int selected, float sel_factor, float enter_factor, SDL_Surface *font, int *first_option)
{
	SDL_Surface *sfc;
	int w = 0, h = 0;
	int title_w = 0, title_h = 0;
	int options_w = 0, options_h = 0;
	int n_options = 0;


	/* Compute the number of options: */
	{
		int i;

		i = 0;

		while (options != 0 && options[i] != 0) {
			if (options[i] == '\n')
				n_options++;

			i++;
		} 
	}

	if (selected < *first_option) {
		*first_option = selected;
	} 

	if ((*first_option) + (MAX_OPTIONS - 1) < selected) {
		*first_option = selected - (MAX_OPTIONS - 1);
	} 

	/* Compute the size of the surface: */
	/* title + options + frame */
	if (title != 0) {
		title_w = get_text_width_bmp((unsigned char *)title, font, 0);
		title_h = font->h + 8;
	} 


	{
		int count = 0;
		int i, j, s;
		char text_tmp[256];

		i = j = 0;

		s = font->w / 256;

		while (options != 0 && options[i] != 0) {
			text_tmp[j] = options[i];

			if (options[i] == '\n') {
				text_tmp[j] = 0;

				if (option_type != 0 && (option_type[count] == 11)) {
					if (int(strlen(text_tmp))*s + 7 > options_w)
						options_w = strlen(text_tmp) * s + 7;
				} else {
					if (int(strlen(text_tmp))*s > options_w)
						options_w = strlen(text_tmp) * s;
				} 

				if (count >= *first_option &&
				        count < (*first_option + MAX_OPTIONS)) {
					options_h += font->h - 2;
				} 

				count++;

				j = 0;
			} else {
				j++;
			} 

			i++;
		} 

		options_h += 2;
	}

	if (options != 0) {
		options_w += 32; /* 8*2 : frame + 8*2 : gap between text and frame */
		options_h += 32; /* 8*2 : frame + 8*2 : gap between text and frame */
	} 

	w = max(title_w, options_w);

	h = title_h + options_h;


	if (w > 0 && h > 0) {
		int y = 0;
		int max_y = h;

		if (title != 0)
			max_y -= font->h + 8;

		max_y -= 32;

		max_y = int(max_y * enter_factor);

		max_y += 32;

		if (title != 0)
			max_y += font->h + 8;

		sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);

		SDL_SetAlpha(sfc, 0, 0);

		SDL_FillRect(sfc, 0, 0);

		/* Draw the elements of the menu: */
		/* title: */
		if (title != 0) {
			SDL_Rect r;
			print_left_bmp((unsigned char *)title, font, sfc, 0, 0, 0);
			r.x = 0;
			r.y = 0;
			r.w = get_text_width_bmp((unsigned char *)title, font, 0);
			r.h = font->h;
			surface_fader(sfc, 1, 1, 1, enter_factor, &r);
			y += font->h + 8;
		} else {
			y++;
		} 

		y = y + 16;

		/* options: */
		{
			int i, j, count = 0, s, sel = 0;
			char text_tmp[256];

			i = j = 0;

			s = font->w / 256;

			while (options != 0 && options[i] != 0 && y < max_y) {
				text_tmp[j] = options[i];

				if (options[i] == '\n') {
					if (sel >= *first_option &&
					        sel < (*first_option + MAX_OPTIONS)) {
						text_tmp[j] = 0;
						print_left_bmp((unsigned char *)text_tmp, font, sfc, 16, y, 0);

						if (option_type != 0 && (option_type[count] == 11)) {
							int w = get_text_width_bmp((unsigned char *)text_tmp, font, 0);
							Uint32 color = SDL_MapRGB(sfc->format, 255, 255, 255);
							Uint32 color2 = SDL_MapRGB(sfc->format, 0, 0, 0);

							/* Draw triangle that denotes that the option leads to a submenu: */
							putpixel(sfc, 16 + w + 4, y + 6, color);
							putpixel(sfc, 16 + w + 4, y + 7, color);
							putpixel(sfc, 16 + w + 4, y + 8, color);
							putpixel(sfc, 16 + w + 4, y + 9, color);
							putpixel(sfc, 16 + w + 4, y + 10, color);

							putpixel(sfc, 16 + w + 5, y + 7, color);
							putpixel(sfc, 16 + w + 5, y + 8, color);
							putpixel(sfc, 16 + w + 5, y + 9, color);

							putpixel(sfc, 16 + w + 6, y + 8, color);

							/* bloack outline: */
							putpixel(sfc, 16 + w + 3, y + 6, color2);
							putpixel(sfc, 16 + w + 3, y + 7, color2);
							putpixel(sfc, 16 + w + 3, y + 8, color2);
							putpixel(sfc, 16 + w + 3, y + 9, color2);
							putpixel(sfc, 16 + w + 3, y + 10, color2);

							putpixel(sfc, 16 + w + 4, y + 5, color2);
							putpixel(sfc, 16 + w + 4, y + 11, color2);

							putpixel(sfc, 16 + w + 5, y + 6, color2);
							putpixel(sfc, 16 + w + 5, y + 10, color2);

							putpixel(sfc, 16 + w + 6, y + 7, color2);
							putpixel(sfc, 16 + w + 6, y + 9, color2);

							putpixel(sfc, 16 + w + 7, y + 8, color2);

						} 

						/* Selected option: */
						if (sel == selected) {
							SDL_Rect r;
							r.x = 16;
							r.y = y;
							r.w = get_text_width_bmp((unsigned char *)text_tmp, font, 0);
							r.h = font->h;

							if (option_type != 0 && (option_type[count] == 11))
								r.w += 7;

							surface_fader(sfc, sel_factor, sel_factor, sel_factor, 1, &r);
						} 

						if (option_type != 0 && (option_type[count] < 0)) {
							SDL_Rect r;
							r.x = 16;
							r.y = y;
							r.w = get_text_width_bmp((unsigned char *)text_tmp, font, 0);
							r.h = font->h;
							surface_fader(sfc, 0.5F, 0.5F, 0.5F, 1, &r);
						} 

						y += font->h - 2;
					} 

					count++;

					j = 0;

					sel++;
				} else {
					j++;
				} 

				i++;
			} 

			y += 2;
		}

		{
			/* Delete the options that have been drawn and shouldn't: */
			SDL_Rect r;
			r.x = 0;
			r.y = max_y - 16;
			r.w = w;
			r.h = y - (max_y - 16);
			SDL_FillRect(sfc, &r, 0);
		}

		/* frame: */
		if (title != 0)
			y = font->h + 8;
		else
			y = 2;

		draw_menu_frame(sfc, 0, y, w, max_y - y);

		/* flechas de scroll: */
		if (enter_factor == 1) {
			if (*first_option > 0) {
				Uint32 color = SDL_MapRGB(sfc->format, 255, 255, 255);
				putpixel(sfc, w / 2, y + 13, color);
				putpixel(sfc, (w / 2) - 1, y + 14, color);
				putpixel(sfc, (w / 2), y + 14, color);
				putpixel(sfc, (w / 2) + 1, y + 14, color);

				putpixel(sfc, (w / 2) - 2, y + 15, color);
				putpixel(sfc, (w / 2) - 1, y + 15, color);
				putpixel(sfc, (w / 2), y + 15, color);
				putpixel(sfc, (w / 2) + 1, y + 15, color);
				putpixel(sfc, (w / 2) + 2, y + 15, color);
			} 

			if ((*first_option + (MAX_OPTIONS)) < n_options) {
				Uint32 color = SDL_MapRGB(sfc->format, 255, 255, 255);
				putpixel(sfc, w / 2, max_y - 13, color);
				putpixel(sfc, (w / 2) - 1, max_y - 14, color);
				putpixel(sfc, (w / 2), max_y - 14, color);
				putpixel(sfc, (w / 2) + 1, max_y - 14, color);

				putpixel(sfc, (w / 2) - 2, max_y - 15, color);
				putpixel(sfc, (w / 2) - 1, max_y - 15, color);
				putpixel(sfc, (w / 2), max_y - 15, color);
				putpixel(sfc, (w / 2) + 1, max_y - 15, color);
				putpixel(sfc, (w / 2) + 2, max_y - 15, color);
			} 
		} 

		if (enter_factor < 0.510)
			surface_fader(sfc, 1, 1, 1, enter_factor*2, 0);

		return sfc;
	} 

	return 0;
} /* draw_menu */


void draw_menu_frame(SDL_Surface *sfc, int x, int y, int dx, int dy, int a)
{
	if (frame_corner[0] == 0) {
		int i, j;
		//  float /*d,x,y,*/c;
		Uint32 color;
		/* Compute the frame graphics: */
		frame_corner[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 32, RMASK, GMASK, BMASK, AMASK);
		frame_corner[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 32, RMASK, GMASK, BMASK, AMASK);
		frame_corner[2] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 32, RMASK, GMASK, BMASK, AMASK);
		frame_corner[3] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 32, RMASK, GMASK, BMASK, AMASK);
		frame_v[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 1, 32, RMASK, GMASK, BMASK, AMASK);
		frame_h[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 16, 32, RMASK, GMASK, BMASK, AMASK);
		frame_v[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 1, 32, RMASK, GMASK, BMASK, AMASK);
		frame_h[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 16, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(frame_corner[0], 0, 0);
		SDL_SetAlpha(frame_corner[1], 0, 0);
		SDL_SetAlpha(frame_corner[2], 0, 0);
		SDL_SetAlpha(frame_corner[3], 0, 0);
		SDL_SetAlpha(frame_v[0], 0, 0);
		SDL_SetAlpha(frame_v[1], 0, 0);
		SDL_SetAlpha(frame_h[0], 0, 0);
		SDL_SetAlpha(frame_h[1], 0, 0);

		for (i = 0;i < 16;i++) {
			if (linear_base[i] == 0)
				color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, 0);

			if (linear_base[i] == 1)
				color = SDL_MapRGBA(frame_corner[0]->format, 255, 255, 255, 255);

			if (linear_base[i] == 2)
				color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, a);

			putpixel(frame_v[0], i, 0, color);

			putpixel(frame_h[0], 0, i, color);

			putpixel(frame_v[1], 15 - i, 0, color);

			putpixel(frame_h[1], 0, 15 - i, color);
		} // for

		for (i = 0;i < 16;i++) {
			for (j = 0;j < 16;j++) {
				if (corner_base1[j + i*16] == 0)
					color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, 0);

				if (corner_base1[j + i*16] == 1)
					color = SDL_MapRGBA(frame_corner[0]->format, 255, 255, 255, 255);

				if (corner_base1[j + i*16] == 2)
					color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, a);

				putpixel(frame_corner[0], i, j, color);

				putpixel(frame_corner[3], 15 - i, 15 - j, color);

				if (corner_base2[j + i*16] == 0)
					color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, 0);

				if (corner_base2[j + i*16] == 1)
					color = SDL_MapRGBA(frame_corner[0]->format, 255, 255, 255, 255);

				if (corner_base2[j + i*16] == 2)
					color = SDL_MapRGBA(frame_corner[0]->format, 0, 0, 0, a);

				putpixel(frame_corner[2], i, 15 - j, color);

				putpixel(frame_corner[1], 15 - i, j, color);
			} // for
		} // for

	} 


	SDL_Rect r;

	int i, j;

	r.x = x;

	r.y = y;

	SDL_BlitSurface(frame_corner[0], 0, sfc, &r);

	r.x = x + dx - 16;

	r.y = y;

	SDL_BlitSurface(frame_corner[1], 0, sfc, &r);

	r.x = x;

	r.y = y + dy - 16;

	SDL_BlitSurface(frame_corner[2], 0, sfc, &r);

	r.x = x + dx - 16;

	r.y = y + dy - 16;

	SDL_BlitSurface(frame_corner[3], 0, sfc, &r);

	for (i = y + 16;i < y + dy - 16;i++) {
		r.x = x;
		r.y = i;
		SDL_BlitSurface(frame_v[0], 0, sfc, &r);
		r.x = x + dx - 16;
		r.y = i;
		SDL_BlitSurface(frame_v[1], 0, sfc, &r);
	} 

	for (i = x + 16;i < x + dx - 16;i++) {
		r.x = i;
		r.y = y;
		SDL_BlitSurface(frame_h[0], 0, sfc, &r);
		r.x = i;
		r.y = y + dy - 16;
		SDL_BlitSurface(frame_h[1], 0, sfc, &r);
	} 

	{
		Uint8 *p;

		for (j = 10;j < dy - 10;j++) {
			p = (Uint8 *)sfc->pixels + (y + j) * sfc->pitch + (x + 10) * 4;

			for (i = 10;i < dx - 10;i++, p += 4) {
				/*
				 FIXME:
				 for some weird reason, changing the AOFFSET here doesn't work
				 instead on alpha it uses red (ROFFSET)...
				 probably has something to do with bitshifting and endianness

				 this works, but should be fixed!
				 */

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				*((Uint32 *)p) = ((*(Uint32 *)p) | ((Uint32)a << (ROFFSET * 8)));
#else
				*((Uint32 *)p) = ((*(Uint32 *)p) | ((Uint32)a << (AOFFSET * 8)));
#endif
			}
		}
	}
}


void draw_menu_frame(SDL_Surface *sfc, int x, int y, int dx, int dy)
{
	draw_menu_frame(sfc, x, y, dx, dy, 192);
}

SOUNDT load_sfx(char *folder, char *default_folder, char *sample)
{
	char tmp[256];
	SOUNDT stmp;

	sprintf(tmp, "%s%s", folder, sample);

	if (Sound_file_test(tmp)) {
		stmp = Sound_create_sound(tmp);
#ifdef F1SPIRIT_DEBUG_MESSAGES

		output_debug_message("%s: %p\n", tmp, stmp);
#endif

		return stmp;
	} 

	sprintf(tmp, "%s%s", default_folder, sample);

	stmp = Sound_create_sound(tmp);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("%s: %p\n", tmp, stmp);

#endif

	return stmp;
} /* load_sfx */
