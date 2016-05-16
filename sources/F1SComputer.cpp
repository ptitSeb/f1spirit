#ifdef KITSCHY_DEBUG_MEMORY
#include "debug_memorymanager.h"
#endif

#ifdef _WIN32
#include "windows.h"
#include "glut.h"
#endif

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_net.h"

#include <curl/curl.h>

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
#include "CCar.h"
#include "2DCMC.h"
#include "F1Shttp.h"
#include "RoadPiece.h"
#include "track.h"
#include "RacingCCar.h"
#include "PlayerCCar.h"
#include "EnemyCCar.h"
#include "GameParameters.h"
#include "ReplayInfo.h"
#include "F1SpiritGame.h"
#include "F1SpiritTrackViewer.h"
#include "F1SpiritApp.h"
#include "F1Spirit-auxiliar.h"

#include "debug.h"

extern int network_tcp_port;
extern int network_udp_port;


F1SComputer::F1SComputer()
{
	name[0] = 0;
	ip.host = 0;
	ip.port = 0;
	tcp_socket = 0;
	udp_socket = 0;
	tcp_port = network_tcp_port;
	udp_port = network_udp_port;
	activity_counter = 0;
} 


F1SComputer::~F1SComputer()
{
	if (tcp_socket != 0) {
		SDLNet_TCP_Close(tcp_socket);
		tcp_socket = 0;
	} 

	if (udp_socket != 0) {
		SDLNet_UDP_Close(udp_socket);
		udp_socket = 0;
	} 
} 

