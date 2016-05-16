#ifndef MOG_AUXILIAR
#define MOG_AUXILIAR

#ifndef M_PI
#define M_PI 3.141592654F
#endif

#ifndef _WIN32
#define min(a,b) ((a)<(b) ? (a) : (b))
#define max(a,b) ((a)>(b) ? (a) : (b))

#endif
//#define abs(a) ((a)<0 ? (-(a)) : (a))


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
// PPC values:
#define RMASK  0xff000000
#define GMASK  0x00ff0000
#define BMASK  0x0000ff00
#define AMASK  0x000000ff
#define AOFFSET 3
#define BOFFSET 2
#define GOFFSET 1
#define ROFFSET 0

#else
// Intel values:
#define RMASK  0x000000ff
#define GMASK  0x0000ff00
#define BMASK  0x00ff0000
#define AMASK  0xff000000
#define AOFFSET 3
#define BOFFSET 2
#define GOFFSET 1
#define ROFFSET 0

#endif

#ifndef _WIN32
#ifndef HAVE_STRLWR

char *strlwr (char *str);

#endif
#endif

bool substr(char *str, char *substr);
void pause(unsigned int time);


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void maximumpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void draw_rectangle(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel);
void draw_rectangle_vp(SDL_Surface *surface, int x, int y, int w, int h, Uint32 pixel, SDL_Rect *vp);
void draw_line(SDL_Surface *SDL, int x1, int y1, int x2, int y2, Uint32 pixel);
void draw_thick_line(SDL_Surface *SDL, int x1, int y1, int x2, int y2, Uint32 pixel, int thickness);
void area_fill(SDL_Surface *SDL, int x, int y, Uint32 pixel);
void fast_area_fill(SDL_Surface *SDL, int x, int y, Uint32 pixel);

void surface_shifter(SDL_Surface *surface, int r_inc, int g_int, int b_inc, int a_inc, SDL_Rect *r);
void surface_fader(SDL_Surface *surface, float r_factor, float g_factor, float b_factor, float a_factor, SDL_Rect *r);
void surface_shader(SDL_Surface *surface, float factor, int red, int green, int blue, int alpha, SDL_Rect *r);
void surface_bicolor(SDL_Surface *surface, float factor, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2, SDL_Rect *r);

void surface_fader_mask(SDL_Surface *surface, SDL_Surface *mask, float r_factor, float g_factor, float b_factor, float a_factor, SDL_Rect *r);


SDL_Surface *load_maskedimage(char *image, char *mask, char *path);

void surface_automatic_alpha(SDL_Surface *sfc);
void surface_bw(SDL_Surface *sfc, int threshold);
void surface_mask_from_bitmap(SDL_Surface *sfc, SDL_Surface *mask, int x, int y);

bool segment_collision(int s1x1, int s1y1, int s1x2, int s1y2,
                       int s2x1, int s2y1, int s2x2, int s2y2,
                       int *cx, int *cy);

/*
void print_left(char *text,TTF_Font *font,int r,int g,int b,SDL_Surface *sfc,int x,int y);
void print_centered(char *text,TTF_Font *font,int r,int g,int b,SDL_Surface *sfc,int x,int y);
int get_text_width(char *text, TTF_Font *font);

SDL_Surface *multiline_text_surface(char *text,int line_dist,TTF_Font *font,SDL_Color c);
SDL_Surface *multiline_text_surface_limited(char *text,int line_dist,TTF_Font *font,SDL_Color c,int max_w);

*/

void print_left_bmp(unsigned char *text, SDL_Surface *font, SDL_Surface *sfc, int x, int y, int spacing);
void print_centered_bmp(unsigned char *text, SDL_Surface *font, SDL_Surface *sfc, int x, int y, int spacing);
int get_text_width_bmp(unsigned char *text, SDL_Surface *font, int spacing);

/* Blits orig into dest, and sets the alpha channel of all the blitted pixels to the right value: */
void surface_blit_alpha(SDL_Surface *orig, SDL_Rect *o_r, SDL_Surface *dest, SDL_Rect *d_r);

char get_key_ascii(int key);

/* functions to write/read from files: */

bool save_float(FILE *fp, float f);
bool load_float(FILE *fp, float *f);

bool save_int(FILE *fp, int f);
bool load_int(FILE *fp, int *f);


#endif

