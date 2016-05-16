#ifdef _WIN32
#include "windows.h"
#endif

#ifdef HAVE_GLES
#include <GLES/gl.h>
#define GL_RGBA8 	GL_RGBA
#define GL_CLAMP	GL_CLAMP_TO_EDGE
#else
#include <GL/gl.h>
#endif

#include "SDL.h"
#include "SDL_image.h"

#include "stdio.h"
#include "math.h"

#include "auxiliar.h"

#include "GLTile.h"
#include "SDL_glutaux.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#define USE_GLM 1

#ifdef USE_GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

int nearest_2pow(int n)
{
	int res = 2;

	for (res = 2;res < 2048;res *= 2) {
		if (res >= n)
			return res;
	} 

	return res;
} /* nearest_2pow */


GLuint createTexture(SDL_Surface *sfc, float *tx, float *ty)
{
	GLuint tname = 0;
	int szx, szy;

	if (sfc != 0) {
		SDL_Surface *sfc2 = 0;

		szx = nearest_2pow(sfc->w);
		szy = nearest_2pow(sfc->h);
		*tx = (sfc->w) / float(szx);
		*ty = (sfc->h) / float(szy);

		sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(sfc, 0, 0);
		SDL_BlitSurface(sfc, 0, sfc2, 0);

		//glGenTextures(1, &tname);
		if (!Find_Texture(sfc2, sfc->w, sfc->h, false, false, &tname))  {
			#ifdef HAVE_GLES
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			#else
			glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
			#endif
			glBindTexture(GL_TEXTURE_2D, tname);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
		}
		SDL_FreeSurface(sfc2);
	} else {
		return 0;
	}

	return tname;
}

GLuint createTextureClamp(SDL_Surface *sfc, float *tx, float *ty)
{
	GLuint tname = 0;
	int szx, szy;

	if (sfc != 0) {
		SDL_Surface *sfc2 = 0;

		szx = nearest_2pow(sfc->w);
		szy = nearest_2pow(sfc->h);
		*tx = (sfc->w) / float(szx);
		*ty = (sfc->h) / float(szy);

		sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(sfc, 0, 0);
		SDL_BlitSurface(sfc, 0, sfc2, 0);

//		glGenTextures(1, &tname);
		if (!Find_Texture(sfc2, sfc->w, sfc->h, true, false, &tname))  {
			#ifdef HAVE_GLES
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			#else
			glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
			#endif
			glBindTexture(GL_TEXTURE_2D, tname);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
		}
		SDL_FreeSurface(sfc2);
	} else {
		return 0;
	}

	return tname;
}

GLuint createTextureSmooth(SDL_Surface *sfc, float *tx, float *ty)
{
	GLuint tname = 0;
	int szx, szy;

	if (sfc != 0) {
		SDL_Surface *sfc2 = 0;

		szx = nearest_2pow(sfc->w);
		szy = nearest_2pow(sfc->h);
		*tx = (sfc->w) / float(szx);
		*ty = (sfc->h) / float(szy);

		sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(sfc, 0, 0);
		SDL_BlitSurface(sfc, 0, sfc2, 0);

//		glGenTextures(1, &tname);
		if (!Find_Texture(sfc2, sfc->w, sfc->h, false, true, &tname))  {
			#ifdef HAVE_GLES
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			#else
			glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
			#endif
			glBindTexture(GL_TEXTURE_2D, tname);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
		}
		SDL_FreeSurface(sfc2);
	} else {
		return 0;
	}

	return tname;
}

GLuint createTextureClampSmooth(SDL_Surface *sfc, float *tx, float *ty)
{
	GLuint tname = 0;
	int szx, szy;

	if (sfc != 0) {
		SDL_Surface *sfc2 = 0;

		szx = nearest_2pow(sfc->w);
		szy = nearest_2pow(sfc->h);
		*tx = (sfc->w) / float(szx);
		*ty = (sfc->h) / float(szy);

		sfc2 = SDL_CreateRGBSurface(SDL_SWSURFACE, szx, szy, 32, RMASK, GMASK, BMASK, AMASK);
		SDL_SetAlpha(sfc, 0, 0);
		SDL_BlitSurface(sfc, 0, sfc2, 0);

//		glGenTextures(1, &tname);
		if (!Find_Texture(sfc2, sfc->w, sfc->h, true, true, &tname))  {
			#ifdef HAVE_GLES
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			#else
			glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
			#endif
			glBindTexture(GL_TEXTURE_2D, tname);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, szx, szy, 0, GL_RGBA, GL_UNSIGNED_BYTE, sfc2->pixels);
		}
		SDL_FreeSurface(sfc2);
	} else {
		return 0;
	}

	return tname;
}

GLuint createTextureFromScreen(int x, int y, int dx, int dy, float *tx, float *ty)
{
	GLuint tname = 0;
	int szx, szy;

	szx = nearest_2pow(dx);
	szy = nearest_2pow(dy);
	*tx = (dx) / float(szx);
	*ty = (dy) / float(szy);

	glGenTextures(1, &tname);
	#ifdef HAVE_GLES
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	#else
	glPixelStorei(GL_UNPACK_ALIGNMENT, tname);
	#endif
	glBindTexture(GL_TEXTURE_2D, tname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x, y, dx, dy);

	return tname;
}

void gl_line(int x1, int y1, int x2, int y2, float r, float g, float b)
{
	#ifdef HAVE_GLES
	glColor4f(r, g, b, 1.0f);
	GLfloat vtx[] = {float(x1), float(y1), 
					 float(x2), float(y2) };
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vtx);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	#else
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex3f(float(x1), float(y1), 0);
	glVertex3f(float(x2), float(y2), 0);
	glEnd();
	#endif
}


#define CRC32_POLYNOMIAL     0x04C11DB7

unsigned int CRCTable[ 256 ];

unsigned int Reflect( unsigned int ref, char ch )
{
     unsigned int value = 0;

     // Swap bit 0 for bit 7
     // bit 1 for bit 6, etc.
     for (char i = 1; i < (ch + 1); i++)
     {
          if(ref & 1)
               value |= 1 << (ch - i);
          ref >>= 1;
     }
     return value;
}

void CRC_BuildTable()
{
    unsigned int crc;

    for (unsigned i = 0; i <= 255; i++)
        {
        crc = Reflect( i, 8 ) << 24;
        for (unsigned j = 0; j < 8; j++)
                        crc = (crc << 1) ^ (crc & (1 << 31) ? CRC32_POLYNOMIAL : 0);
        
        CRCTable[i] = Reflect( crc, 32 );
    }
}

unsigned int CRC32( unsigned int crc, void *buffer, unsigned int count )
{
  unsigned int orig = crc;
  unsigned char * p = reinterpret_cast<unsigned char*>(buffer);
  while (count--)
    crc = (crc >> 8) ^ CRCTable[(crc & 0xFF) ^ *p++];
  return crc ^ orig;
}

#define MAX_TEX 1024
GLuint texture_id[MAX_TEX];
unsigned int texture_crc[MAX_TEX];
unsigned int texture_flags[MAX_TEX];
int texture_count[MAX_TEX];
int texture_max;

void Init_TexManager()
{
	for (int i=0; i<MAX_TEX; i++) {
		texture_id[i] = 0;
		texture_crc[i]=0;
		texture_flags[i]=0;
		texture_count[i]=0;
	}
	texture_max = 0;
}

bool Find_Texture(SDL_Surface *sfc, int w, int h, bool clamp, bool smooth, GLuint *ID)
{
	// Calc the CRC
	int buff[3];
	buff[0]=(clamp)?1:0+(smooth)?2:0;
	buff[1]=w;
	buff[2]=h;
	
	unsigned int crc, flags;
	
	flags = CRC32(0, (void*)buff, 3*sizeof(int));
	crc = CRC32(0, sfc->pixels, sfc->pitch*sfc->h);
	
	// Search it
	int i, hole;
	bool ret=true;
	hole = -1;
	for (i=0; i<texture_max; i++) {
		if (!texture_count[i]) 
		{ 
			if (hole==-1) hole = i;
		} 
		else
		if ((texture_flags[i]==flags) && (texture_crc[i]==crc))
			break;
	}
	if (i>=texture_max) {	// not found, add a new one
		GLuint id;
		glGenTextures(1, &id);
		if (texture_max==MAX_TEX) {
			*ID=id;
			return false;
		}	// list is full
		if (hole==-1) {
			hole=texture_max++;
		}
		texture_flags[hole]=flags;
		texture_crc[hole]=crc;
		texture_id[hole]=id;
		i = hole;
		ret=false;
	} else {
	}
	texture_count[i]++;
	*ID=texture_id[i];		// found
	return ret;
}

void TM_glDeleteTextures(GLuint n, GLuint *first)
{
	int i;
	for (int j=0; j<n; j++) {
		bool del=false;
		for (i=0; i<texture_max; i++) {
			if (texture_id[i]==first[j]) {
				del=true;
				if ((--texture_count[i])==0) {
					glDeleteTextures(1, &(texture_id[i]));
				}
				break;
			}
		}
		if (!del) {
			glDeleteTextures(1, &(first[j]));
		}
	}
	// now, reduce texture_max
	i=texture_max-1;
	while ((i>=0) && (texture_count[i]==0)) {
		texture_max--; i--;
	}
}

#define MAX_VTX		8192
GLfloat	vtx[MAX_VTX*4*3];
GLfloat tex1[MAX_VTX*4*2];
GLushort indices[MAX_VTX*6];
int idx=0;
int ids=0;
GLuint old_tex=0;
bool special=false;

#define CNT2 16
GLfloat* vtx2[CNT2];	// vtx arrays
GLfloat* tex2[CNT2];	// tex arrays
GLfloat* col2[CNT2];	// tex arrays
GLushort* ind2[CNT2];	// indices
GLushort idx2[CNT2];
GLuint 	 gltex2[CNT2];	// associated texture (0 for not used)
GLfloat  cur_col[4];	// current color
GLfloat cur_normal[3];	// current normal
int ids2[CNT2];
int cnt_active;
bool cnt_inited = false;

void glesDoDraw();
void apply(float x, float y, float z, float *vec );
int pushm = 0;
#ifdef USE_GLM
glm::mat4	m;
glm::mat4	pile[10];
#endif

void glesFlushSpecial()
{
	glEnable(GL_TEXTURE_2D);		// because it is called out of the loop...
	for (cnt_active=0; cnt_active<CNT2; cnt_active++) 
		if (gltex2[cnt_active])
		{
			glesDoDraw();
		}
	cnt_active = 0;
}

void glesSpecial(bool what)
{
	if (!cnt_inited) {
		for (int i=0; i<CNT2; i++) {
			vtx2[i] = new GLfloat[MAX_VTX*4*3];
			tex2[i] = new GLfloat[MAX_VTX*4*2];
			col2[i] = new GLfloat[MAX_VTX*4*4];
			ind2[i] = new GLushort[MAX_VTX*6];
			idx2[i] = 0;
			ids2[i] = 0;
			gltex2[i] = 0;
		}
		cnt_active = 0;
		cnt_inited = true;
	}
	if (special) {
		glesFlushSpecial();
		glDisable(GL_TEXTURE_2D);
		for (int i=0; i<CNT2; i++) {
			idx2[i] = 0;
			ids2[i] = 0;
			gltex2[i] = 0;
		}
		cnt_active = 0;
	}
	special=what;

	cur_normal[0] = cur_normal[1] = 0.0f;
	cur_normal[2] = 1.0f;
	cur_col[0] = cur_col[1] = cur_col[2] = cur_col[3] = 1.0f;

	old_tex=0;
	idx=0;
	ids=0;
#ifdef USE_GLM
	pushm=0;
	m=glm::mat4(1.0f);
#endif
}

void glesBindTexture(GLenum what, GLuint texid)
{
	if (!special) {
		if (old_tex!=texid)
			glBindTexture(what, texid);
		old_tex=texid;
	} else {
		if (gltex2[cnt_active]!=texid)
		{
			// check if current has stated yet
			if (gltex2[cnt_active]==0) {
				gltex2[cnt_active] = texid;	// nope, start it!
			} else {
				// lets search for an already started list with same texid
				for (int j=0; j<CNT2; j++)
					if (gltex2[j]==texid) {
						cnt_active = j;
						j = CNT2;
					}
				// if not found, lets used next one, and flush it if needed
				if (gltex2[cnt_active] != texid) {
					// search for an empty
					for (int j=0; j<CNT2; j++)
						if (idx2[j]==0) {
							cnt_active = j;
							j = CNT2;
						}
					// if not found, ake next...
					if (gltex2[cnt_active])
						cnt_active = (cnt_active+1)%CNT2;
					if (gltex2[cnt_active]) {
						glEnable(GL_TEXTURE_2D);
						glesDoDraw();			// flush...
					}
					gltex2[cnt_active] = texid;	// new list
				}
			}
		}
	}
}

void glesBegin(GLenum what)
{
	if (special)
		return;
	(void)what;	// let's ignore it, assume is GL_QUAD
	idx = 0; 
	ids = 0;
}

void glesTexCoord2f(GLfloat a, GLfloat b)
{
	if (special) {
		tex2[cnt_active][idx2[cnt_active]*2+0]=a; 
		tex2[cnt_active][idx2[cnt_active]*2+1]=b;
	} else {
		tex1[idx*2+0]=a; 
		tex1[idx*2+1]=b;
	}
}

void glesNormal3f(GLfloat x, GLfloat y, GLfloat z)
{
	if (special) {
		if (cur_normal[0]!=x || cur_normal[1]!=y || cur_normal[2]!=z) {
			glesFlushSpecial();
			cur_normal[0]=x;
			cur_normal[1]=y;
			cur_normal[2]=z;
		}
	}
	glNormal3f(x, y, z);	
}
void glesVertex3f(GLfloat a, GLfloat b, GLfloat c)	
{
	if (special) {
		apply(a, b, c, vtx2[cnt_active]+idx2[cnt_active]*3);
		for (int i=0; i<4; i++)
			col2[cnt_active][idx2[cnt_active]*4+i] = cur_col[i];
		idx2[cnt_active]++;
		if (idx2[cnt_active]%4==0) {
				ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-4; ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-3; ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-2; 
				ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-2; ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-1; ind2[cnt_active][ids2[cnt_active]++]=idx2[cnt_active]-4;
				if (ids2[cnt_active]>MAX_VTX*6-10)
					glesDoDraw();		//flush...
			}
	} else {
		vtx[idx*3+0]=a; vtx[idx*3+1]=b; vtx[idx*3+2]=c; 
		idx++; 
		if (idx%4==0) {
			indices[ids++]=idx-4; indices[ids++]=idx-3; indices[ids++]=idx-2; 
			indices[ids++]=idx-2; indices[ids++]=idx-1; indices[ids++]=idx-4;
			if (ids>MAX_VTX*6-10)
				glesDoDraw();		//flush...
		}
	}
}

void glesColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	cur_col[0] = r; cur_col[1] = g; cur_col[2] = b; cur_col[3] = a;
	glColor4f(r, g, b, a);
}

void glesEnd()
{
	if (special)
		return;
	glesDoDraw();
}

void glesTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	if (special) {
	#ifdef USE_GLM
		m=glm::translate(m, glm::vec3(x, y, z));
		return;
	#else
		glesFlushSpecial();
	#endif
	}
	glTranslatef(x, y, z);
}

void glesRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	if (special) {
	#ifdef USE_GLM
		m=glm::rotate(m, angle, glm::vec3(x, y, z));
		return;
	#else
		glesFlushSpecial();
	#endif
	}
	glRotatef(angle, x, y, z);
}

void glesScalef(GLfloat x, GLfloat y, GLfloat z)
{
	if (special) {
	#ifdef USE_GLM
		m=glm::scale(m, glm::vec3(x, y, z));
		return;
	#else
		glesFlushSpecial();
	#endif
	}
	glScalef(x, y, z);
}

void glesPushMatrix()
{
	if (special) {
	#ifdef USE_GLM
		pile[pushm++]=m;
		return;
	#else
		glesFlushSpecial();
	#endif
	}
	glPushMatrix();
}

void glesPopMatrix()
{
	if (special) {
	#ifdef USE_GLM
		m=pile[--pushm];
		return;
	#else
		glesFlushSpecial();
	#endif
	}
	glPopMatrix();
}


void glesDoDraw()
{
	if (special) {
		if (ids2[cnt_active]==0)
			return;
	} else {
		if (ids==0)
			return;
	}
	if (special) {
		if (old_tex!=gltex2[cnt_active])
			glBindTexture(GL_TEXTURE_2D, gltex2[cnt_active]);
		old_tex = gltex2[cnt_active];
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (special) {
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vtx2[cnt_active]);
		glTexCoordPointer(2, GL_FLOAT, 0, tex2[cnt_active]);
		glColorPointer(4, GL_FLOAT, 0, col2[cnt_active]);
		glDrawElements(GL_TRIANGLES, ids2[cnt_active], GL_UNSIGNED_SHORT, ind2[cnt_active]);
		idx2[cnt_active] = 0; 
		ids2[cnt_active] = 0;
		gltex2[cnt_active] = 0;
		glDisableClientState(GL_COLOR_ARRAY);
	} else {
		glVertexPointer(3, GL_FLOAT, 0, vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, tex1);
		glDrawElements(GL_TRIANGLES, ids, GL_UNSIGNED_SHORT, indices);
		idx = 0; 
		ids = 0;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void inline apply(float x, float y, float z, float *vec )
{
#ifdef USE_GLM
	glm::vec4 pos(x, y, z, 1.0f);
	pos=m*pos;
	vec[0]=pos[0]; vec[1]=pos[1]; vec[2]=pos[2];
#else
	vec[0]=x; vec[1]=y; vec[2]=z;
#endif
}

