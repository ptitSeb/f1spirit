#ifndef __BRAIN_SDL_GLUTAUX
#define __BRAIN_SDL_GLUTAUX

GLuint createTexture(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureClamp(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureSmooth(SDL_Surface *sfc, float *tx, float *ty);
GLuint createTextureClampSmooth(SDL_Surface *sfc, float *tx, float *ty);
//GLuint createTextureFromScreen(int x, int y, int dx, int dy, float *tx, float *ty);


/*
void gl_print_left(int x,int y,char *text,int r,int g,int b,TTF_Font *font);
void gl_print_center(int x,int y,char *text,int r,int g,int b,TTF_Font *font);
*/
void gl_line(int x1, int y1, int x2, int y2, float r, float g, float b);

/* CRC Fucntion */
void CRC_BuildTable();
unsigned int CRC32( unsigned int crc, void *buffer, unsigned int count );

/* Texture manager */
void Init_TexManager();
bool Find_Texture(SDL_Surface *sfc, int w, int h, bool clamp, bool smooth, GLuint *ID);
void TM_glDeleteTextures(GLuint n, GLuint *first);

/* GLES drawing */
void glesSpecial(bool what);
void glesBindTexture(GLenum what, GLuint texid);
void glesBegin(GLenum what);
void glesTexCoord2f(GLfloat a, GLfloat b);
void glesNormal3f(GLfloat x, GLfloat y, GLfloat z);
void glesVertex3f(GLfloat a, GLfloat b, GLfloat c);
void glesColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glesEnd();
void glesTranslatef(GLfloat x, GLfloat y, GLfloat z);
void glesRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glesScalef(GLfloat x, GLfloat y, GLfloat z);
void glesPushMatrix();
void glesPopMatrix();

#endif
