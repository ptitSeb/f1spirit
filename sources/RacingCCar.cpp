#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_net.h"

#include "F1Spirit.h"
#include "sound.h"
#include "List.h"
#include "2DCMC.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "PlacedGLTile.h"
#include "RotatedGLTile.h"
#include "keyboardstate.h"
#include "CPlayer.h"
#include "RoadPiece.h"
#include "CCar.h"
#include "2DCMC.h"
#include "track.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif




RacingCCar::RacingCCar()
{
	laps = 0;
	first_lap = false;
	position = 0;
} 


RacingCCar::~RacingCCar()
{
	road_position.Delete();

	laps = 0;
	first_lap = false;

} 




bool RacingCCar::save(FILE *fp)
{
	if (car->save_configuration(fp) &&
	        save_status(fp))
		return true;

	return false;
} 


bool RacingCCar::load_status(FILE *fp)
{
	return car->load_status(fp);
} 


bool RacingCCar::save_status(FILE *fp)
{
	return car->save_status(fp);
} 


