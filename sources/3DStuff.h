#ifndef __3DSTUFF_H_
#define __3DSTUFF_H_

#ifdef HAVE_GLES
#include <GLES/gl.h>
//#include <GLES/glu.h>
#else
#include <GL/gl.h>
//#include "GL/glu.h"
#endif

extern int MINX;
extern int MAXX;
extern int MINY;
extern int MAXY;

extern int desktopW;
extern int desktopH;

extern int SCREENW;
extern int SCREENH;
extern int SCREENX;
extern int SCREENY;

extern float screenScale;

void calcMinMax(int width, int height);

#endif //__3DSTUFF_H_