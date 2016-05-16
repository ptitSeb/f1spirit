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
#include "randomc.h"

#include "debug.h"

#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

extern bool sound;

void carEngineSoundEffect(int chan, void *stream, int len, void *udata)
{
	CCar *car = (CCar *)udata;

	float f;
	unsigned int i, j, k;
	Sint16 *ip, *ip2;
	SOUNDT ces = car->getcarEngineSound();

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("carEngineSoundEffect of %i\n",chan);
#endif


	if (car->get_state()!=2) {
		// state = 2 is when the car has crashed, thus we don't have to play the engine:

		// "f" is the factor at which we are modifying the pitch of the engine sample
		// if "f=2", the sound will be with twice the pitch of the original engine sample.
		// the formula weare using is: f = ((rpm-2000)/2000)+0.5
		f = (car->get_rpm() - 2000.0F) / 2000.0F;

		if (f < 0)
			f = 0;

		f = 0.5F + f;

		ip = (Sint16 *)stream;
		ip2 = (Sint16 *)ces->abuf;

		// Fill the S_carengine_working with a pitch modified version of the engine sample (S_carengine)
		// Since the sample is stereo and 16bits, each sample is "4 bytes"
		// "j" is the index that moves over the S_carengine_working sound, 
		// and k = j*f is the index that moves over the S_carengine sound.
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("carengine_position of channel %i is %i  (f: %g)\n",chan,car->carengine_position,f);
#endif
		for (i = 0, j = 0;i < len;i += 4, j++) {
			k = car->carengine_position + (unsigned int)(j * f);

			while (k*4 >= ces->alen)
				k -= ces->alen / 4;

			// Stereo mixing: 
			ip[j*2] = ip2[k * 2];  // L 

			ip[j*2 + 1] = ip2[k * 2 + 1]; // R 
		} 
		car->carengine_position = k;
	} else {
		// When the car has crashed, just set the buffer to all 0:

		ip = (Sint16 *)stream;
		for (i = 0, j = 0;i < len;i += 4, j++) {
			ip[j*2]=0;
			ip[j*2+1]=0;
		}
		car->carengine_position = 0;
	} // if
}


