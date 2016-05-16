#ifndef _F1SPIRIT_AUXILIAR
#define _F1SPIRIT_AUXILIAR


SDL_Surface *load_bmp_font(char *filename, int first, int last);

SDL_Surface *draw_menu(int MAX_OPTIONS, char *title, char *options, int *option_type, int selected, float sel_factor, float enter_factor, SDL_Surface *font, int *first_option);
void draw_menu_frame(SDL_Surface *sfc, int x, int y, int dx, int dy);
void draw_menu_frame(SDL_Surface *sfc, int x, int y, int dx, int dy, int a);

void free_auxiliar_menu_surfaces(void);

SOUNDT load_sfx(char *dir, char *default_dir, char *sample);


#endif
