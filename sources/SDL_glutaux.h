#ifndef __BRAIN_SDL_GLUTAUX
#define __BRAIN_SDL_GLUTAUX

GLuint createTexture(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureClamp(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureSmooth(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureClampSmooth(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureFromScreen(int x, int y, int dx, int dy, float *tx, float *ty);


/*
void gl_print_left(int x,int y,char *text,int r,int g,int b,TTF_Font *font);
void gl_print_center(int x,int y,char *text,int r,int g,int b,TTF_Font *font);
*/

void gl_line(int x1, int y1, int x2, int y2, float r, float g, float b);

#endif
