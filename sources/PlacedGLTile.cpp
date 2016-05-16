#ifdef _WIN32
#include "windows.h"
#endif

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"

#include "auxiliar.h"
#include "2DCMC.h"
#include "GLTile.h"
#include "PlacedGLTile.h"
#include "SDL_glutaux.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

CPlacedGLTile::CPlacedGLTile(float ax, float ay, float az, float aa, GLTile *t)
{
	x = ax;
	y = ay;
	z = az;
	angle = aa;
	zoom = 1.0F;
	tile = t;
	different_color = false;


} 


CPlacedGLTile::~CPlacedGLTile()
{
	tile = 0;
} 


void CPlacedGLTile::draw(void)
{
	if (different_color)
		tile->draw(r, g, b, a, x, y, z, angle, zoom);
	else
		tile->draw(x, y, z, angle, zoom);
} 


void CPlacedGLTile::draw(float dx, float dy, float dz, float pangle, float pzoom)
{
	if (tile != 0) {
		if (different_color)
			tile->draw(r, g, b, a, x + dx, y + dy, z + dz, angle + pangle, zoom*pzoom);
		else
			tile->draw(x + dx, y + dy, z + dz, angle + pangle, zoom*pzoom);
	} 
} 





