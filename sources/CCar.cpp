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
extern TRanrotBGenerator *rg;

void carEngineSoundEffect(int chan, void *stream, int len, void *udata);


CEngine::CEngine(float p, float c, float e, int m)
{
	max_rpm = m;
	power = p;
	consumption = c;
	endurance = e;
} 


CBody::CBody(int w, float d, float a, float cs)
{
	aerodynamics = a;
	weight = w;
	damage_absortion = d;
	crash_speed = cs;
} 


CBrake::CBrake(int f, int b)
{
	front = f;
	back = b;
} 

CSuspension::CSuspension(float ts)
{
	turn_speed = ts;
} 

CGear::CGear(bool a, float g1, float g2, float g3, float g4)
{
	automatic = a;
	gear[0] = g1;
	gear[1] = g2;
	gear[2] = g3;
	gear[3] = g4;
} 


CCar::CCar(CEngine *e, CBody *bo, CBrake *br, CSuspension *s, CGear *ge, int type, int model, float r, float g, float b, bool pplayer, SOUNDT engine_sound, int engine_channel, GLTile **pextra_tiles, SDL_Surface *car_sfc, SDL_Surface *car_minimap_sfc, F1SpiritGame *game)
{
	last_col_angle = 0;
	last_col_nx = 0, last_col_ny = 0;

	old_terrain = 0;

	engine = e;
	body = bo;
	brake = br;
	suspension = s;
	gear = ge;

	m_game = game;

	c_type = type;
	c_r = r;
	c_g = g;
	c_b = b;
	c_body = 0;
	c_engine = 0;
	c_brake = 0;
	c_suspension = 0;
	c_gear = 0;
	c_handycap = 1.0;

	damage_ftyre = 0;
	damage_rtyre = 0;
	damage_brake = 0;
	damage_engine = 0;

	new_damage = 0;

	player = pplayer;

	extra_tiles = pextra_tiles;

	if (type == 1) {
		/* Rally car (that requires extra tiles for jumping) */
		ntiles = 8;
		tiles = new GLTile * [8];
		shadow_tiles = new GLTile * [8];
	} else {
		ntiles = 6;
		tiles = new GLTile * [6];
		shadow_tiles = new GLTile * [6];
	} { 

		int i;

		/* graphics tiles: */

		for (i = 0;i < ntiles;i++) {
			tiles[i] = new GLTile(car_sfc, model * 32, i * 32, 32, 32);
			tiles[i]->set_clamp();
			tiles[i]->set_hotspot(16, 16);
		} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("tiles generated.\n");

#endif

		/* shadows: */
		{
			int i, j;
			Uint32 c;
			SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, car_sfc->w, car_sfc->h, 32, RMASK, GMASK, BMASK, AMASK);

			for (i = 0;i < sfc->h;i++) {
				for (j = 0;j < sfc->w;j++) {
					c = getpixel(car_sfc, j, i);

					if ((c&AMASK) == AMASK) {
						putpixel(sfc, j, i, SDL_MapRGBA(sfc->format, 0, 0, 0, 128));
					} else {
						putpixel(sfc, j, i, 0);
					} 
				} 
			} 

			for (i = 0;i < ntiles;i++) {
				shadow_tiles[i] = new GLTile(sfc, model * 32, i * 32, 32, 32);
				shadow_tiles[i]->set_hotspot(16, 16);
			} 

			SDL_FreeSurface(sfc);
		}
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Shadow tiles generated.\n");

#endif

	}

	{
		SDL_Rect r2;
		SDL_Surface *sfc = SDL_CreateRGBSurface(SDL_SWSURFACE, car_minimap_sfc->w, car_minimap_sfc->h, 32, RMASK, GMASK, BMASK, AMASK);
		Uint32 color1 = SDL_MapRGBA(sfc->format, 255, 0, 0, 255);
		Uint32 color2 = SDL_MapRGBA(sfc->format, int(255 * r), int(255 * g), int(255 * b), 255);
		r2.x = 0;
		r2.y = 0;
		r2.w = car_minimap_sfc->w;
		r2.h = car_minimap_sfc->h;
		SDL_SetAlpha(car_minimap_sfc, 0, 0);
		SDL_BlitSurface(car_minimap_sfc, &r2, sfc, &r2);
		{
			int i, j;

			for (i = 0;i < sfc->w;i++) {
				for (j = 0;j < sfc->h;j++) {
					if (getpixel(sfc, i, j) == color1)
						putpixel(sfc, i, j, color2);
				} 
			} 
		}

		//  surface_fader(sfc,r,g,b,1,0);

		mini_tile = new GLTile(sfc);
		mini_tile->set_hotspot(sfc->w / 2, sfc->h / 2);
		//  SDL_FreeSurface(sfc);
	}

	tcar = new CPlacedGLTile(0, 0, 0, 0, tiles[0]);

	tshadow = new CPlacedGLTile(0, 0, 0, 0, shadow_tiles[0]);

#ifdef F1SPIRIT_DEBUG_MESSAGES

	output_debug_message("Creating sounds....");
#endif

	if (player) {
		if (sound) {
			/* Sound: */
			S_carengine = engine_sound;
			carengine_channel = engine_channel;

			carengine_position = 0;
			carengine_sounding = false;
		} 

		fuel = max_fuel = m_game->parameters.MAX_FUEL;
	} else {
		S_carengine = 0;
		carengine_channel = -1;
		fuel = max_fuel = m_game->parameters.MAX_FUEL * 10;
	} 


#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("OK\n");

#endif

	speed_x = 0;

	speed_y = 0;

	speed_a = 0;

	current_gear = 0;

	rpm = 0;

	speed = 0;

	brake_timmer = 0;

	turn_timmer = 0;

	water_splash_timmer = 0;

	tyre_squeal_timmer = 0;

	chicane_timmer = 0;

	spinning_start_timmer = 0;

	spinning_timmer = 0;

	spinning_speed = 0;

	jumping_magnitude = 0;

	jumping_timmer = 0;

	old_speed_x = 0;

	old_speed_y = 0;

	state = 0;
	state_timmer = 0;

} 


CCar::~CCar()
{
	int i;

	extra_tiles = 0;

	delete engine;
	delete body;
	delete brake;
	delete suspension;
	delete gear;

	for (i = 0;i < ntiles;i++) {
		delete tiles[i];
		tiles[i] = 0;

		delete shadow_tiles[i];
		shadow_tiles[i] = 0;
	} 

	delete []tiles;

	tiles = 0;

	delete []shadow_tiles;

	shadow_tiles = 0;

	ntiles = 0;

	delete mini_tile;

	mini_tile = 0;

	delete tcar;

	tcar = 0;

	delete tshadow;

	tshadow = 0;

	if (carengine_channel >= 0 && sound) {
		Mix_HaltChannel(carengine_channel);
		Mix_UnregisterAllEffects(carengine_channel);
	}

	S_carengine = 0;
/*
	if (S_carengine_working.abuf != 0) {
		delete []S_carengine_working.abuf;
		S_carengine_working.abuf = 0;
	} 
*/
} 


CCar *F1SpiritGame::create_car(int type, int body, int engine, int brake, int suspension, int gear, float pr, float pg, float pb, bool player, SOUNDT engine_sound, int engine_channel, float handycap)
{
	CBody *p_body = 0;
	CEngine *p_engine = 0;
	CBrake *p_brake = 0;
	CSuspension *p_suspension = 0;
	CGear *p_gear = 0;
	int model = 0;
	float damage_absortion[3] = {0.4F, 0.2F, 0.0F};

	if (!player)
		gear = 0;

	if (player) {
		model = body;
	} else {
		int n_enemy_models = 8;
		n_enemy_models = (race_car_sfc->w / 32) - 3;
		model = rg->IRandom(0, n_enemy_models - 1) + 3;
	} 

	/* BODY: */
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Body... %i\n", body);

#endif

	switch (type) {

		case 0: { /* STOCK */
				int weight[3] = {1050, 1025, 1000};
				float cs[3] = {170, 150, 130};
				p_body = new CBody(weight[body], damage_absortion[body], 2, cs[body]);
			}

			break;

		case 1:  /* RALLY */
			/* 1020 985 1000 */
			{
				int weight[3] = {975, 950, 925};
				float cs[3] = {180, 165, 150};
				p_body = new CBody(weight[body], damage_absortion[body], 2.5, cs[body]);
			}

			break;

		case 2:  /* F3 */
			/* 460 450 355 */
			{
				int weight[3] = {460, 440, 420};
				float cs[3] = {190, 170, 150};

				p_body = new CBody(weight[body], damage_absortion[body], 1, cs[body]);
			}

			break;

		case 3:  /* F3000 */
			/* 540 540 540 */
			{
				int weight[3] = {560, 540, 520};
				float cs[3] = {190, 170, 150};

				p_body = new CBody(weight[body], damage_absortion[body], 1, cs[body]);
			}

			break;

		case 4:  /* ENDURANCE */
			/* 850 700 756 */
			{
				int weight[3] = {775, 750, 725};
				float cs[3] = {190, 170, 150};

				p_body = new CBody(weight[body], damage_absortion[body], 1.5, cs[body]);
			}

			break;

		case 5:  /* F1 */
			/* 540 540 540 */
			{
				int weight[3] = {560, 540, 520};
				float cs[3] = {200, 175, 150};

				p_body = new CBody(weight[body], damage_absortion[body], 1, cs[body]);
			}

			break;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Engine... %i\n", engine);

#endif

	/* ENGINE: */
	switch (type) {

		case 0:  /* STOCK */

			switch (engine) {

				case 0:
					p_engine = new CEngine(760, 1500, 12, 7920);
					break;

				case 1:
					p_engine = new CEngine(780, 1400, 10, 7920);
					break;

				case 2:
					p_engine = new CEngine(800, 1300, 8, 8000);
					break;

				case 3:
					p_engine = new CEngine(790, 1500, 10, 8000);
					break;

				case 4:
					p_engine = new CEngine(820, 1500, 8, 8100);
					break;

				case 5:
					p_engine = new CEngine(810, 1400, 8, 8000);
					break;
			} 

			break;

		case 1:  /* RALLY */
			switch (engine) {

				case 0:
					p_engine = new CEngine(960, 1500, 12, 7500);
					break;

				case 1:
					p_engine = new CEngine(980, 1400, 10, 7500);
					break;

				case 2:
					p_engine = new CEngine(1000, 1300, 8, 7600);
					break;

				case 3:
					p_engine = new CEngine(990, 1500, 10, 7500);
					break;

				case 4:
					p_engine = new CEngine(1020, 1500, 8, 7700);
					break;

				case 5:
					p_engine = new CEngine(1010, 1400, 8, 7600);
					break;
			} 

			break;

		case 2:  /* F3 */
			switch (engine) {

				case 0:
					p_engine = new CEngine(340, 1500, 12, 10400);
					break;

				case 1:
					p_engine = new CEngine(360, 1400, 10, 10400);
					break;

				case 2:
					p_engine = new CEngine(380, 1300, 8, 10500);
					break;

				case 3:
					p_engine = new CEngine(370, 1500, 10, 10500);
					break;

				case 4:
					p_engine = new CEngine(400, 1500, 8, 10600);
					break;

				case 5:
					p_engine = new CEngine(390, 1400, 8, 10500);
					break;
			} 

			break;

		case 3:  /* F3000 */
			switch (engine) {

				case 0:
					p_engine = new CEngine(400, 1500, 12, 10900);
					break;

				case 1:
					p_engine = new CEngine(440, 1400, 10, 10900);
					break;

				case 2:
					p_engine = new CEngine(480, 1300, 8, 11000);
					break;

				case 3:
					p_engine = new CEngine(460, 1500, 10, 11000);
					break;

				case 4:
					p_engine = new CEngine(520, 1500, 8, 11100);
					break;

				case 5:
					p_engine = new CEngine(500, 1400, 8, 11000);
					break;
			} 

			break;

		case 4:  /* ENDURANCE */
			switch (engine) {

				case 0:
					p_engine = new CEngine(700, 1500, 12, 9900);
					break;

				case 1:
					p_engine = new CEngine(750, 1490, 10, 9900);
					break;

				case 2:
					p_engine = new CEngine(800, 1200, 8, 10000);
					break;

				case 3:
					p_engine = new CEngine(775, 1500, 10, 10000);
					break;

				case 4:
					p_engine = new CEngine(850, 1500, 8, 10100);
					break;

				case 5:
					p_engine = new CEngine(825, 1400, 8, 10000);
					break;
			} 

			break;

		case 5:  /* F1 */
			switch (engine) {

				case 0:
					p_engine = new CEngine(800, 1500, 12, 11400);
					break;

				case 1:
					p_engine = new CEngine(880, 1490, 10, 11400);
					break;

				case 2:
					p_engine = new CEngine(960, 1200, 8, 11500);
					break;

				case 3:
					p_engine = new CEngine(920, 1500, 10, 11500);
					break;

				case 4:
					p_engine = new CEngine(1040, 1500, 8, 11600);
					break;

				case 5:
					p_engine = new CEngine(1000, 1400, 8, 11500);
					break;
			} 

			break;
	} 

	/* BRAKE: */
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Brake... %i\n", brake);

#endif

	switch (brake) {

		case 0:
			p_brake = new CBrake(0, 0);
			break;

		case 1:
			p_brake = new CBrake(1, 2);
			break;

		case 2:
			p_brake = new CBrake(1, 1);
			break;
	} 

	/* SUSPENSION: */
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Suspension... %i\n", suspension);

#endif

	switch (type) {

		case 0:  /* STOCK: */

			switch (suspension) {

				case 0:
					p_suspension = new CSuspension(0.12F);
					break;

				case 1:
					p_suspension = new CSuspension(0.13F);
					break;

				case 2:
					p_suspension = new CSuspension(0.14F);
					break;

				case 3:  /* enemy only: */
					p_suspension = new CSuspension(0.15F);
					break;
			} 

			break;

		case 1:  /* RALLY: */
			switch (suspension) {

				case 0:
					p_suspension = new CSuspension(0.20F);
					break;

				case 1:
					p_suspension = new CSuspension(0.21F);
					break;

				case 2:
					p_suspension = new CSuspension(0.22F);
					break;

				case 3:  /* enemy only: */
					p_suspension = new CSuspension(0.22F);
					break;
			} 

			break;

		default:  /* F3,F3000,ENDURANCE,F1: */
			switch (suspension) {

				case 0:
					p_suspension = new CSuspension(0.13F);
					break;

				case 1:
					p_suspension = new CSuspension(0.135F);
					break;

				case 2:
					p_suspension = new CSuspension(0.14F);
					break;

				case 3:  /* enemy only: */
					p_suspension = new CSuspension(0.20F);
					break;
			} 

			break;
	} 

	/* GEAR: */
#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Gear... %i\n", gear);

#endif

	switch (gear) {
			/*
			 case 0:p_gear=new CGear(true,14.29F,22.29F,23.24F,26.21F);
			     break;
			 case 1:p_gear=new CGear(false,13.26F,18.26F,24.25F,27.21F);
			     break;
			 case 2:p_gear=new CGear(false,14.29F,22.29F,23.24F,26.21F);
			     break;
			*/

		case 0:
			p_gear = new CGear(true, 14.0F, 20.0F, 23.0F, 26.2F);
			break;

		case 1:
			p_gear = new CGear(false, 13.0F, 18.0F, 24.0F, 26.2F);
			break;

		case 2:
			p_gear = new CGear(false, 14.0F, 20.0F, 23.0F, 26.2F);
			break;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Ok\n");

#endif

	if (!player) {
		p_engine->max_rpm = int(p_engine->max_rpm * handycap);

		if (handycap > 1.0)
			p_engine->power *= (handycap + 1.0F) / 2.0F;

		if (p_engine->max_rpm < 0)
			p_engine->max_rpm = 10;

		engine_channel = -1;
	} 

#ifdef F1SPIRIT_DEBUG_MESSAGES
	output_debug_message("Proceeding to create the CCar object...\n");

#endif

	if (pr == -1 || pg == -1 || pb == -1) {
		float r[6][3] = { {1, 0, 1},
			{0, 1, 1},
			{1, 1, 0.5},
			{0, 1, 0},
			{0, 0.25, 1},
			{1, 0, 1}
		};
		float g[6][3] = { {0, 0, 1},
			{1, 1, 1},
			{0, 1, 0.5},
			{0, 0, 1},
			{1, 0.25, 0},
			{0, 0, 1}
		};
		float b[6][3] = { {0, 1, 0},
			{1, 1, 0.5},
			{0, 0, 1},
			{1, 0, 0},
			{1, 0.25, 0},
			{0, 1, 0}
		};
		CCar *c = new CCar(p_engine, p_body, p_brake, p_suspension, p_gear, type, model, r[type][body], g[type][body], b[type][body], player, engine_sound, engine_channel, race_extra_tiles, race_car_sfc, (player ? race_minimap_car_sfc : race_minimap_enemycar_sfc), this);

		c->c_body = body;
		c->c_engine = engine;
		c->c_brake = brake;
		c->c_suspension = suspension;
		c->c_gear = gear;
		c->c_handycap = handycap;
		return c;
	} else {
		CCar *c = new CCar(p_engine, p_body, p_brake, p_suspension, p_gear, type, model, pr, pg, pb, player, engine_sound, engine_channel, race_extra_tiles, race_car_sfc, (player ? race_minimap_car_sfc : race_minimap_enemycar_sfc), this);

		c->c_body = body;
		c->c_engine = engine;
		c->c_brake = brake;
		c->c_suspension = suspension;
		c->c_gear = gear;
		c->c_handycap = handycap;
		return c;
	} 
} 


void CCar::draw_mini(float a, float x, float y, float z, float angle, float zoom)
{
	mini_tile->draw(1, 1, 1, a, x, y, z, angle, zoom);
} 



bool CCar::cycle(bool kaccelerate, bool kbrake, int kgear, int kturn, CRoadPiece *current_piece, CTrack *track, List<RacingCCar> *cars, List<F1S_SFX> *sfx, int sfx_volume, bool check_trackcollision, int terrain, float position, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy)
{
	/* Modelo físico: */
	float old_x = tcar->x;
	float old_y = tcar->y;
	float old_z = tcar->z;
	float old_angle = tcar->angle;
	float PIXEL_CTNT = m_game->parameters.PIXEL_CTNTS[0];

	old_terrain = terrain;

	PIXEL_CTNT = m_game->parameters.PIXEL_CTNTS[min(track->get_track_number(), 5)];

	if (carengine_channel>=0 && !carengine_sounding) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		output_debug_message("Starting the engine sound in channel: %i\n", carengine_channel);
#endif
		Mix_HaltChannel(0);
//		Mix_UnregisterAllEffects(carengine_channel);
		if(!Mix_RegisterEffect(carengine_channel, carEngineSoundEffect, NULL, this)) {
#ifdef F1SPIRIT_DEBUG_MESSAGES
		    output_debug_message("Mix_RegisterEffect: %s\n", Mix_GetError());
#endif
		}
		Mix_PlayChannel(carengine_channel, S_carengine, -1);
		carengine_sounding = true;
	} // if


	switch (get_state()) {

		case 0:
			/* START LINE: */
			state_timmer++;

			if (kaccelerate) {
				//    float c=(engine->max_rpm-abs(rpm))/float(engine->max_rpm);
				rpm += 200;

				if (rpm > engine->max_rpm)
					rpm = float(engine->max_rpm);
			} else {
				rpm -= 50;

				if (rpm < 0)
					rpm = 0;
			} 

			set_tile(0);

			break;

		case 1:

		case 3:
			/* DRIVING: */
			state_timmer++;

			if (get_state() == 3 && state_timmer > 128)
				set_state(1);

			{
				float adherence = 0, friction = 0;

				set_tile(0);
				tcar->set_nocolor();
				tshadow->set_nocolor();


				if (terrain == 0) {
					adherence = m_game->parameters.road_adherence;
					friction = m_game->parameters.road_friction;
				} else {
					if (terrain == 2) {
						adherence = m_game->parameters.sand_adherence;
						friction = m_game->parameters.sand_friction;

						if (track->get_track_number() == 10)
							adherence = m_game->parameters.grass_adherence;
					} else {
						adherence = m_game->parameters.grass_adherence;
						friction = m_game->parameters.grass_friction;
					} 
				} 

				adherence *= m_game->parameters.car_type_adherence[c_type];

				adherence *= 1.0F - (damage_rtyre * m_game->parameters.rtyre_damage_influence);

				if (jumping_timmer != 0)
					adherence = 0;

				/* Physics */
				{
					float ground_speed;
					float ground_speed_raw;

					old_speed_x = speed_x;
					old_speed_y = speed_y;

					ground_speed_raw = float(sqrt(speed_x * speed_x + speed_y * speed_y));
					ground_speed = ground_speed_raw / PIXEL_CTNT;
					//    speed=speed*0.75F+ground_speed*0.25F;
					speed = speed * 0.96F + ground_speed * 0.04F;
					rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));

					if (gear->automatic) {
						/* automatic gears: */
						if (kaccelerate && rpm > engine->max_rpm*0.85 && current_gear < 3) {
							current_gear++;
							rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));
						} 

						if (rpm < engine->max_rpm*0.6 && current_gear > 0) {
							current_gear--;
							rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));
						} 
					} else {
						/* manual gears: */
						if (kgear == -1 && current_gear > 0) {
							current_gear--;
							rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));
						} 

						if (kgear == 1 && current_gear < 3) {
							current_gear++;
							rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));
						} 
					} 

					if (speed < 1)
						current_gear = 0;

					if (speed < 0)
						speed = 0;

					/* Rally rocks: */
					if (jumping_timmer > 0) {
						jumping_timmer--;

						if (jumping_timmer < jumping_magnitude)
							set_tile(7);

						if (jumping_timmer >= jumping_magnitude &&
						        jumping_timmer < jumping_magnitude*2)
							set_tile(0);

						if (jumping_timmer >= jumping_magnitude*2)
							set_tile(6);

						if (jumping_timmer == 0)
							set_tile(0);
					} else {
						if (ground_speed != 0) {
							float vx, vy;
							float px, py;
							bool front = false, back = false;

							vx = speed_x / ground_speed_raw;
							vy = speed_y / ground_speed_raw;

							px = tcar->x + 12 * vx;
							py = tcar->y + 12 * vy;

							if (track->over_rock(px, py, m_game->rock_tile))
								front = true;

							px = tcar->x - 12 * vx;

							py = tcar->y - 12 * vy;

							if (track->over_rock(px, py, m_game->rock_tile))
								back = true;

							if (front) {
								if (back) {
									set_tile(0);
								} else {
									/* jump! */
									int c = int((speed - 50) / 24);

									if (c > 1) {
										jumping_magnitude = c;
										jumping_timmer = c * 3;
									} 

									set_tile(6);
								} 
							} else {
								if (back) {
									set_tile(7);
								} else {
									set_tile(0);
								} 
							} 
						} 
					} 


					if (jumping_timmer == 0) {
						float turn_speed = 0.13F;

						turn_speed = suspension->turn_speed;
						//     if (suspension->hardness==1) turn_speed=0.12F;
						//     if (suspension->hardness==3) turn_speed=0.14F;

						turn_speed *= 1.0F - (damage_ftyre * m_game->parameters.ftyre_damage_influence);

						if (kturn == -1) {
							float f;

							if (speed_a > 0)
								turn_timmer = 0;

							//      f=turn_timmer/25.0F;
							f = turn_timmer / 10.0F;

							if (f > 1)
								f = 1;

							//      f=1;
							speed_a = -float(sqrt(speed) * turn_speed * f);

							turn_timmer++;
						} 

						if (kturn == 1) {
							float f;

							if (speed_a < 0)
								turn_timmer = 0;

							//      f=turn_timmer/25.0F;
							f = turn_timmer / 10.0F;

							if (f > 1)
								f = 1;

							//      f=1;
							speed_a = float(sqrt(speed) * turn_speed * f);

							turn_timmer++;
						} 

						if (kturn == 0) {
							if (turn_timmer > 0)
								turn_timmer--;

							speed_a = 0;
						} 
					} 

					/* Road Piece dependent calculus (stabilize & Z coordinate adjustment) */
					{
						float px, py, pz, pa, pw, pd;
						float dif;

						if (current_piece->get_path_position(tcar->x, tcar->y, &px, &py, &pz, &pa, &pw, &pd)) {
							float offset = 2;

							if (jumping_timmer > 0) {
								float f = 0;

								if (jumping_timmer < jumping_magnitude)
									f = float(jumping_timmer) / float(jumping_magnitude);

								if (jumping_timmer >= jumping_magnitude &&
								        jumping_timmer < jumping_magnitude*2)
									f = 1;

								if (jumping_timmer >= jumping_magnitude*2)
									f = float(jumping_magnitude * 3 - jumping_timmer) / float(jumping_magnitude);

								offset = 2 + jumping_magnitude * f * 4;
							} 

							tcar->z = pz - offset;

							if (tcar->z > ( -8) - offset)
								tcar->z = ( -8) - offset;

							if (tcar->z < ( -64) - offset)
								tcar->z = ( -64) - offset;

							tshadow->z = pz;

							/*
							      if (current_piece->get_z1()>current_piece->get_z2()) {
							       tcar->z=current_piece->get_z2()-offset;
							      } else {
							       tcar->z=current_piece->get_z1()-offset;
							      } // if
							*/
							/* In the straight parts of the roads, the car tends to stabilize: */
							if (kturn == 0) {
								pa += 90;
								dif = pa - tcar->angle;

								if (dif < -180)
									dif += 360;

								if (dif > 180)
									dif -= 360;

								if (dif < -0.5 && dif > -45) {
									speed_a = -float(sqrt(speed) * 0.02);
								} 

								if (dif > 0.5 && dif < 45) {
									speed_a = float(sqrt(speed) * 0.02);
								} 

								if (dif > -0.5 && dif < 0.5)
									tcar->angle += dif;
							} 

						} 
					}


					if (kaccelerate && fuel > 0) {
						float power = engine->power;
						power *= 1.0F - (damage_engine * m_game->parameters.engine_damage_influence);

						//     float c=(engine->max_rpm-abs(rpm))/float(engine->max_rpm);
						rpm += 9000 * (power / (body->weight * gear->gear[current_gear] * gear->gear[current_gear]));

						fuel -= (engine->consumption / m_game->parameters.fuel_consumption_k1) * (rpm / m_game->parameters.fuel_consumption_k2);
						//     fuel-=(engine->consumption/13500.0F)*(rpm/400.0F);
					} 

					if (kbrake && jumping_timmer == 0) {
						if ((brake_timmer % 50) == 0 && speed > 100) {
							if (player) {
								F1S_SFX *s = new F1S_SFX();
								s->x = int(tcar->x);
								s->y = int(tcar->y);
								s->SFX = SFX_BRAKE_STOCK;
								sfx->Add(s);
							} 
						} 

						brake_timmer++;
					} else {
						brake_timmer = 0;
					} 

					if (jumping_timmer == 0) {
						speed = float(rpm * gear->gear[current_gear] * m_game->parameters.SPEED_CTNT);

						/* friction: */
						speed -= speed * body->aerodynamics * friction;

						if (brake_timmer > 0) {
							float speed_reduction = 0;

							switch (brake->back) {

								case 0:

								case 2:
									speed_reduction += 2.0F;
									break;

								case 1:
									speed_reduction += float(1.0 + 2.0F * (25 - min(brake_timmer, 25)) / 25.0F);
									break;
							} 

							switch (brake->front) {

								case 0:

								case 2:
									speed_reduction += 1;
									break;

								case 1:
									speed_reduction += float(1.0 + 2.0F * (25 - min(brake_timmer, 25)) / 25.0F);
									break;
							} 

							speed_reduction *= 1.0F - (damage_brake * m_game->parameters.brake_damage_influence);

							speed -= speed_reduction;
						} 

						if (speed < 0)
							speed = 0;

						/* restablish rpm after the forces: */
						{
							float mr = float(engine->max_rpm);
							mr *= 1.0F - (damage_engine / 16.0F);

							rpm = float(speed / (gear->gear[current_gear] * m_game->parameters.SPEED_CTNT));

							if (rpm < 0)
								rpm = 0;

							if (rpm > mr) {
								/* damage to the motor! */
								rpm -= engine->power * 0.1F;

								if (rpm > mr) {
									damage_engine += 0.005F;

									if (damage_engine >= 1.0)
										damage_engine = 1.0;

									new_damage = true;
								} 
							} 
						}

						speed = float(rpm * gear->gear[current_gear] * m_game->parameters.SPEED_CTNT);

						speed_x = float((speed * PIXEL_CTNT) * sin(tcar->angle * M_PI / 180));
						speed_y = float(( -speed * PIXEL_CTNT) * cos(tcar->angle * M_PI / 180));
					} 

					/* Adherence: */
					/* old_speed_?: true speed relative to the track */
					/* speed_?: speed desired by the car relative to the track */
					{
						float difx = speed_x - old_speed_x;
						float dify = speed_y - old_speed_y;

						float magnitude = float(sqrt(difx * difx + dify * dify));
						float factor;
						int tile = terrain;

						if (magnitude > adherence) {
							factor = adherence / magnitude;
						} else {
							factor = 1;
						} 

						speed_x = old_speed_x + difx * factor;

						speed_y = old_speed_y + dify * factor;

						/* Car out of control: */
						if (spinning_timmer > 0) {
							spinning_timmer--;
							spinning_start_timmer = 0;
							speed_a = spinning_speed;
						} 

						tcar->x = old_x + speed_x;

						tcar->y = old_y + speed_y;

						tcar->angle += speed_a;

						//     if (terrain==2)
						{
							int tmp = int((magnitude - adherence) * 40);

							if (tmp < 0)
								tmp = 0;

							if (tmp > 100)
								tmp = 100;

							tmp = 100 - tmp;

							if (tmp <= 0)
								tmp = 1;

							if (terrain == 1 && speed > 100)
								tmp = 2;

							if (jumping_timmer == 0) {
								if (speed > 50 &&
								        track->over_water(tcar->x, tcar->y, m_game->water_tile)) {
									if (water_splash_timmer == 0) {
										F1S_SFX *s = new F1S_SFX();
										s->x = int(tcar->x);
										s->y = int(tcar->y);
										s->SFX = SFX_WATER;
										sfx->Add(s);
									} 

									tmp = 1;

									tile = 3;

									water_splash_timmer++;
								} else {
									water_splash_timmer = 0;
								} 

								if (tmp < m_game->parameters.squeal_threshold || spinning_timmer > 0) {
									CTyreMark *tm1, *tm2;
									float x1, y1;

									if ((old_x != tcar->x || old_y != tcar->y) && current_piece != 0) {
										tm1 = new CTyreMark();
										tm2 = new CTyreMark();

										x1 = old_x + 2 * float( -sin(old_angle * M_PI / 180));
										y1 = old_y + 2 * float(cos(old_angle * M_PI / 180));
										tm1->x = x1 + 5 * float( -sin((old_angle + 90) * M_PI / 180));
										tm1->y = y1 + 5 * float(cos((old_angle + 90) * M_PI / 180));
										tm1->z = old_z;
										tm2->x = x1 + 7 * float( -sin((old_angle - 90) * M_PI / 180));
										tm2->y = y1 + 7 * float(cos((old_angle - 90) * M_PI / 180));
										tm2->z = old_z;

										x1 = tcar->x + 2 * float( -sin(tcar->angle * M_PI / 180));
										y1 = tcar->y + 2 * float(cos(tcar->angle * M_PI / 180));
										tm1->x2 = x1 + 5 * float( -sin((tcar->angle + 90) * M_PI / 180));
										tm1->y2 = y1 + 5 * float(cos((tcar->angle + 90) * M_PI / 180));
										tm1->z2 = tcar->z;
										tm2->x2 = x1 + 7 * float( -sin((tcar->angle - 90) * M_PI / 180));
										tm2->y2 = y1 + 7 * float(cos((tcar->angle - 90) * M_PI / 180));
										tm2->z2 = tcar->z;

										if (terrain == 0) {
											tm1->r = tm2->r = 0;
											tm1->g = tm2->g = 0;
											tm1->b = tm2->b = 0;
											tm1->t = tm2->t = 0;

											if ((tyre_squeal_timmer % 50) == 0) {
												F1S_SFX *s = new F1S_SFX();
												s->x = int(tcar->x);
												s->y = int(tcar->y);
												s->SFX = SFX_SQUEAL;
												sfx->Add(s);
											} 

											tyre_squeal_timmer++;
										} else {
											tyre_squeal_timmer = 0;
										} 

										if (terrain == 1) {
											tm1->r = tm2->r = 0;
											tm1->g = tm2->g = 0.25F;
											tm1->b = tm2->b = 0;
											tm1->t = tm2->t = 0;
										} 

										if (terrain == 2) {
											tm1->r = tm2->r = 0.375F;
											tm1->g = tm2->g = 0.25F;
											tm1->b = tm2->b = 0.2F;
											tm1->t = tm2->t = 0;
										} 

										track->add_tyremark(tm1, track->get_road()->PositionRef(current_piece));

										track->add_tyremark(tm2, track->get_road()->PositionRef(current_piece));
									} 
								} else {
									tyre_squeal_timmer = 0;
								} 

								if (rg->IRandom(0, tmp - 1) == 0) {
									CExtraGFX *g;
									CPlacedGLTile *pt;
									float x1, y1, x2, y2, x3, y3;
									float a;

									x1 = tcar->x + 2 * float( -sin(tcar->angle * M_PI / 180));
									y1 = tcar->y + 2 * float(cos(tcar->angle * M_PI / 180));

									x2 = x1 + 5 * float( -sin((tcar->angle + 90) * M_PI / 180));
									y2 = y1 + 5 * float(cos((tcar->angle + 90) * M_PI / 180));
									x3 = x1 + 7 * float( -sin((tcar->angle - 90) * M_PI / 180));
									y3 = y1 + 7 * float(cos((tcar->angle - 90) * M_PI / 180));

									pt = new CPlacedGLTile(x2 + (rg->IRandom(0, 6)) - 3, y2 + (rg->IRandom(0, 6)) - 3, tcar->z, tcar->angle, extra_tiles[tile]);
									g = new CExtraGFX(pt);
									g->a1 = 0.5F;
									g->a2 = 0.0F;
									g->last_time = 16;
									g->timmer = 0;
									a = tcar->angle + (rg->IRandom(0, 80)) - 40;
									g->speed_x = float(speed * PIXEL_CTNT / 10.0) * float(sin(a * M_PI / 180));
									g->speed_y = float(speed * PIXEL_CTNT / 10.0) * float( -cos(a * M_PI / 180));
									g->speed_z = 0;
									g->speed_zoom = 0.5F;
									g->speed_angle = 8.0F;
									extra_gfx.Add(g);

									pt = new CPlacedGLTile(x3 + (rg->IRandom(0, 6)) - 3, y3 + (rg->IRandom(0, 6)) - 3, tcar->z, tcar->angle, extra_tiles[tile]);
									g = new CExtraGFX(pt);
									g->a1 = 0.5F;
									g->a2 = 0.0F;
									g->last_time = 16;
									g->timmer = 0;
									a = tcar->angle + (rg->IRandom(0, 80)) - 40;
									g->speed_x = float(speed * PIXEL_CTNT / 10.0) * float(sin(a * M_PI / 180));
									g->speed_y = float(speed * PIXEL_CTNT / 10.0) * float( -cos(a * M_PI / 180));
									g->speed_z = 0;
									g->speed_zoom = 0.5F;
									g->speed_angle = 8.0F;
									extra_gfx.Add(g);
								} 

								/* Car out of control test: */
								if (spinning_timmer <= 0 && jumping_timmer == 0) {
									if (tmp < m_game->parameters.spinning_threshold && kturn != 0 && speed > 100) {
										spinning_start_timmer++;
									} else {
										spinning_start_timmer--;

										if (spinning_start_timmer < 0)
											spinning_start_timmer = 0;
									} 

									if (spinning_start_timmer > m_game->parameters.car_type_spinning_threshold[c_type]) {
										/* car out of control: */
										if (speed_a > 0) {
											spinning_speed = 12.0F;
										} else {
											spinning_speed = -12.0F;
										} 

										spinning_timmer = 60;
									} 
								} 
							} 
						}

						//     

						if (kbrake && speed > 100 && brake_timmer > 10 && jumping_timmer == 0) {
							if ((old_x != tcar->x || old_y != tcar->y) && current_piece != 0) {
								CTyreMark *tm1, *tm2;
								float x1, y1;
								tm1 = new CTyreMark();
								tm2 = new CTyreMark();

								x1 = old_x + 2 * float( -sin(old_angle * M_PI / 180));
								y1 = old_y + 2 * float(cos(old_angle * M_PI / 180));
								tm1->x = x1 + 5 * float( -sin((old_angle + 90) * M_PI / 180));
								tm1->y = y1 + 5 * float(cos((old_angle + 90) * M_PI / 180));
								tm1->z = old_z;
								tm2->x = x1 + 7 * float( -sin((old_angle - 90) * M_PI / 180));
								tm2->y = y1 + 7 * float(cos((old_angle - 90) * M_PI / 180));
								tm2->z = old_z;

								x1 = tcar->x + 2 * float( -sin(tcar->angle * M_PI / 180));
								y1 = tcar->y + 2 * float(cos(tcar->angle * M_PI / 180));
								tm1->x2 = x1 + 5 * float( -sin((tcar->angle + 90) * M_PI / 180));
								tm1->y2 = y1 + 5 * float(cos((tcar->angle + 90) * M_PI / 180));
								tm1->z2 = tcar->z;
								tm2->x2 = x1 + 7 * float( -sin((tcar->angle - 90) * M_PI / 180));
								tm2->y2 = y1 + 7 * float(cos((tcar->angle - 90) * M_PI / 180));
								tm2->z2 = tcar->z;

								if (terrain == 0) {
									tm1->r = tm2->r = 0;
									tm1->g = tm2->g = 0;
									tm1->b = tm2->b = 0;
									tm1->t = tm2->t = 0;
								} 

								if (terrain == 1) {
									tm1->r = tm2->r = 0;
									tm1->g = tm2->g = 0.25;
									tm1->b = tm2->b = 0;
									tm1->t = tm2->t = 0;
								} 

								if (terrain == 2) {
									tm1->r = tm2->r = 0.375F;
									tm1->g = tm2->g = 0.25F;
									tm1->b = tm2->b = 0.2F;
									tm1->t = tm2->t = 0;
								} 

								track->add_tyremark(tm1, track->get_road()->PositionRef(current_piece));

								track->add_tyremark(tm2, track->get_road()->PositionRef(current_piece));
							} 
						} 
					}

				}

				if ((speed_x != 0 ||
				        speed_y != 0 ||
				        speed_a != 0) && speed > 0) {
					int c;
					RacingCCar *c_collided = 0;
					CRotatedGLTile *t_collided = 0;

					if (tcar->x < 0)
						tcar->x = 0;

					if (tcar->y < 0)
						tcar->y = 0;

					if (tcar->x > (track->get_dx()*8) - 1)
						tcar->x = float(track->get_dx() * 8) - 1;

					if (tcar->y > (track->get_dy()*8) - 1)
						tcar->y = float(track->get_dy() * 8) - 1;

					if (tcar->angle < 0)
						tcar->angle += 360;

					if (tcar->angle >= 360)
						tcar->angle -= 360;

					/* Collision detection: */
					if (check_trackcollision) {
						c = track_cars_collision(tcar->x, tcar->y, tcar->z, tcar->angle, track, cars, &c_collided, &t_collided, car_grid, car_grid_sx, car_grid_sy);
					} else {
						c = 0;
						c_collided = cars_collision(tcar->x, tcar->y, tcar->z, tcar->angle, cars, car_grid, car_grid_sx, car_grid_sy);

						if (c_collided != 0)
							c = 2;
					} 

					if (c != 0) {
						spinning_timmer = 0;
						spinning_start_timmer = 0;
					} 

					if (c == 1) {
						/* TRACK COLLISION: */
						int col_type = 0; /* -1 : no collision, 0 : normal collision, 1 : crash */
						float p_x = 0, p_y = 0;
						float l_x = 0, l_y = 0;
						float n_x = 0, n_y = 0;
						float col_speed = 0;
						float alpha1 = 0, alpha2 = 0;

						if (tcar->get_cmc()->collision_point(tcar->x, tcar->y, tcar->angle,
						                                     t_collided->get_cmc(), t_collided->x, t_collided->y, t_collided->angle1, &p_x, &p_y)) {
							float n;

							/* car speed: */
							l_x = speed_x;
							l_y = speed_y;
							n = float(sqrt(l_x * l_x + l_y * l_y));

							if (n != 0) {
								l_x /= n;
								l_y /= n;
							} 

							/* collision normal: */
							//t_collided->get_cmc()->point_normal(p_x,p_y,t_collided->x,t_collided->y,t_collided->angle1,&n_x,&n_y);
							{
								int offset = 2;
								int found = 0;

								n_x = 0;
								n_y = 0;

								while (found == 0) {
									if (!track->collision(tcar->x + offset, tcar->y , tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x++;
									} 

									if (!track->collision(tcar->x + offset, tcar->y + offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x++;
										n_y++;
									} 

									if (!track->collision(tcar->x , tcar->y + offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_y++;
									} 

									if (!track->collision(tcar->x - offset, tcar->y + offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x--;
										n_y++;
									} 

									if (!track->collision(tcar->x - offset, tcar->y , tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x--;
									} 

									if (!track->collision(tcar->x - offset, tcar->y - offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x--;
										n_y--;
									} 

									if (!track->collision(tcar->x , tcar->y - offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_y--;
									} 

									if (!track->collision(tcar->x + offset, tcar->y - offset, tcar->z, tcar->angle, tcar->get_cmc())) {
										found++;
										n_x++;
										n_y--;
									} 

									offset++;
								} 

								n_x /= float(found);

								n_y /= float(found);

								float n = float(sqrt(n_x * n_x + n_y * n_y));

								if (n != 0) {
									n_x /= n;
									n_y /= n;
								} 

								last_col_nx = n_x;

								last_col_ny = n_y;
							}


							/* collision speed: */
							col_speed = (l_x * n_x + l_y * n_y) * speed;

							/* collision angle: */
							alpha1 = float(acos(fabs(col_speed) / fabs(speed)) * 180.0F / M_PI);

							last_col_angle = alpha1;

							/* determine collision type: */
							if (float(fabs(alpha1)) < 35 ||
							        float(fabs(col_speed)) > body->crash_speed) {
								col_type = 1;
							} else {
								col_type = 0;
							} 

						} else {
							col_type = -1;
						} 

						if (col_type == 1) {
							{
								F1S_SFX *s = new F1S_SFX();
								s->x = int(tcar->x);
								s->y = int(tcar->y);
								s->SFX = SFX_HIT_CRASH;
								sfx->Add(s);
							}

							/* Frontal collision: */
							set_state(2);
							speed_x = old_speed_x;
							speed_y = old_speed_y;
							speed = speed * 0.5F;

							/* car damage: */
							{
								int i = rg->IRandom(0, 3);
								float dam = 0.5F;

								dam -= dam * body->damage_absortion;

								switch (i) {

									case 0:
										damage_ftyre += dam;

										if (damage_ftyre > 1.0F)
											damage_ftyre = 1.0F;

										break;

									case 1:
										damage_brake += dam;

										if (damage_brake > 1.0F)
											damage_brake = 1.0F;

										break;

									case 2:
										damage_engine += dam;

										if (damage_engine > 1.0F)
											damage_engine = 1.0F;

										break;

									case 3:
										damage_rtyre += dam;

										if (damage_rtyre > 1.0F)
											damage_rtyre = 1.0F;

										break;
								} 

								new_damage = true;
							}

							/* Loss of fuel: */
							fuel -= 80;

						} 

						if (col_type == 0) {
							{
								F1S_SFX *s = new F1S_SFX();
								int i;

								s->x = int(tcar->x);
								s->y = int(tcar->y);
								s->SFX = SFX_HIT_METAL;

								for (i = 0;i < m_game->n_wood_tiles;i++) {
									if (m_game->wood_tiles[i] == t_collided->tile) {
										s->SFX = SFX_HIT_WOOD;
									} 
								} 

								sfx->Add(s);
							}

							{
								int i = rg->IRandom(0, 3);
								float dam = 0.05F;

								dam = 0.05F * (col_speed / 200.0F);

								dam -= dam * body->damage_absortion;

								switch (i) {

									case 0:
										damage_ftyre += dam;

										if (damage_ftyre > 1.0F)
											damage_ftyre = 1.0F;

										break;

									case 1:
										damage_brake += dam;

										if (damage_brake > 1.0F)
											damage_brake = 1.0F;

										break;

									case 2:
										damage_engine += dam;

										if (damage_engine > 1.0F)
											damage_engine = 1.0F;

										break;

									case 3:
										damage_rtyre += dam;

										if (damage_rtyre > 1.0F)
											damage_rtyre = 1.0F;

										break;
								} 

								new_damage = true;
							}

							/* Side collision: */
							int t = 16;

							/* hit point angle: */
							{
								float dx, dy;
								float n;

								dx = p_x - tcar->x;
								dy = p_y - tcar->y;
								n = float(sqrt(dx * dy + dy * dy));

								if (n > 0) {
									alpha2 = ((float(atan2f(dx / n, -dy / n)) * 180.0F) / M_PI);
								} else {
									alpha2 = 0;
								} 

								alpha2 -= (tcar->angle);

								if (alpha2 > 180)
									alpha2 -= 360;

								if (alpha2 < -180)
									alpha2 += 360;
							}

							if (alpha2 > 0) {
								tcar->angle -= 10;
							} 

							if (alpha2 < 0) {
								tcar->angle += 10;
							} 

							while (t > 0 &&
							        track->collision(tcar->x, tcar->y, tcar->z, tcar->angle, get_cmc()) != 0) {
								tcar->x += 4 * n_x;
								tcar->y += 4 * n_y;
								t--;
							} 

							tcar->x += 4 * n_x;

							tcar->y += 4 * n_y;

							/* Reduce the speed in the direction of the crash: */
							{
								float wx = -n_y;
								float wy = n_x;

								float n = speed_x * n_x + speed_y * n_y;
								float m = speed_x * wx + speed_y * wy;

								speed_x = n * n_x / 2 + m * wx;
								speed_y = n * n_y / 2 + m * wy;
							}
						} 
					} // if

					if (c == 2) {
						/* CARS COLLISION: */
						{
							F1S_SFX *s = new F1S_SFX();
							s->x = int(tcar->x);
							s->y = int(tcar->y);
							s->SFX = SFX_HIT_CAR;
							sfx->Add(s);
						}

						{
							/* Relative speed of the cars: */
							//      float rspeed_x=collided->car->speed_x-speed_x;
							//      float rspeed_y=collided->car->speed_y-speed_y;

							/* To simplify the computation of the collision point, we assume that it's the */
							/* middle point between the centers of the two cars: */
							float cx = (tcar->x + c_collided->car->tcar->x) / 2;
							float cy = (tcar->y + c_collided->car->tcar->y) / 2;
							float vx = tcar->x - cx;
							float vy = tcar->y - cy;
							float vn = float(sqrt(vx * vx + vy * vy));

							if (vn != 0) {
								vx /= vn;
								vy /= vn;
							} 

							float svx1 = speed_x, svy1 = speed_y;

							float svn1 = float(sqrt(svx1 * svx1 + svy1 * svy1));

							if (svn1 != 0) {
								svx1 /= svn1;
								svy1 /= svn1;
							} 

							float svx2 = c_collided->car->speed_x, svy2 = c_collided->car->speed_y;

							float svn2 = float(sqrt(svx2 * svx2 + svy2 * svy2));

							if (svn2 != 0) {
								svx2 /= svn2;
								svy2 /= svn2;
							} 

							/* compute damage: */
							{
								int i = rg->IRandom(0, 3);
								float dam = 0.05F;

								/* simplification: the relative speed of the crash is the substraction of the */
								/*       two speeds of the cars: */

								dam = float(0.05F * (fabs(speed - c_collided->car->speed) / 100.0F));
								dam -= float(dam * body->damage_absortion);

								switch (i) {

									case 0:
										damage_ftyre += dam;

										if (damage_ftyre > 1.0F)
											damage_ftyre = 1.0F;

										c_collided->car->damage_ftyre += dam;

										if (c_collided->car->damage_ftyre > 1.0F)
											c_collided->car->damage_ftyre = 1.0F;

										break;

									case 1:
										damage_brake += dam;

										if (damage_brake > 1.0F)
											damage_brake = 1.0F;

										c_collided->car->damage_brake += dam;

										if (c_collided->car->damage_brake > 1.0F)
											c_collided->car->damage_brake = 1.0F;

										break;

									case 2:
										damage_engine += dam;

										if (damage_engine > 1.0F)
											damage_engine = 1.0F;

										c_collided->car->damage_engine += dam;

										if (c_collided->car->damage_engine > 1.0F)
											c_collided->car->damage_engine = 1.0F;

										break;

									case 3:
										damage_rtyre += dam;

										if (damage_rtyre > 1.0F)
											damage_rtyre = 1.0F;

										c_collided->car->damage_rtyre += dam;

										if (c_collided->car->damage_rtyre > 1.0F)
											c_collided->car->damage_rtyre = 1.0F;

										break;
								} 

								new_damage = true;

								c_collided->car->new_damage = true;
							}

							/* move the cars so that no one collides the other: */
							{
								int t = 16;

								while (t > 0 &&
								        c_collided->car->collision(tcar->x, tcar->y, tcar->z, tcar->angle, get_cmc())) {
									tcar->x += 2 * vx;
									tcar->y += 2 * vy;
									c_collided->car->tcar->x -= 2 * vx;
									c_collided->car->tcar->y -= 2 * vy;
									t--;
								} 

								tcar->x += 2 * vx;

								tcar->y += 2 * vy;

								c_collided->car->tcar->x -= 2 * vx;

								c_collided->car->tcar->y -= 2 * vy;
							}

							/* Set their relative speeds to the average speed among the two cars (in the */
							/* axis of the collision): */
							{
								/* car: */
								float k1 = speed_x * vx + speed_y * vy, k1b = speed_x * vy - speed_y * vx;
								float sx1 = vx * k1, sy1 = vy * k1;
								float sx2 = speed_x - sx1, sy2 = speed_y - sy1;

								/* collided car: */
								float k2 = c_collided->car->speed_x * vx + c_collided->car->speed_y * vy;
								float k2b = c_collided->car->speed_x * vy - c_collided->car->speed_y * vx;
								float csx1 = vx * k2, csy1 = vy * k2;
								float csx2 = c_collided->car->speed_x - csx1, csy2 = c_collided->car->speed_y - csy1;

								speed_x = sx2 + (sx1 + csx1) / 2;
								speed_y = sy2 + (sy1 + csy1) / 2;
								c_collided->car->speed_x = csx2 + (sx1 + csx1) / 2;
								c_collided->car->speed_y = csy2 + (sy1 + csy1) / 2;

								/* Diminish the RPM adequeately: */

								if (svn1 > 0) {
									/* car: */
									float svn1b = svx1 * speed_x + svy1 * speed_y;
									float factor = ((svn1 + svn1b) / 2.0F) / svn1;
									float newrpm = rpm * factor;

									if (newrpm < rpm && newrpm > 0)
										rpm = newrpm;
								} 

								if (svn1 > 0) {
									/* collided car: */
									float svn2b = svx2 * c_collided->car->speed_x + svy2 * c_collided->car->speed_y;
									float factor = ((svn2 + svn2b) / 2.0F) / svn2;
									float newrpm = c_collided->car->rpm * factor;

									if (newrpm < c_collided->car->rpm && newrpm > 0)
										c_collided->car->rpm = newrpm;
								} 

								/* If the cars collide at a very high speed, check if the situation is */
								/* an enemy car colliding with the player from behind, and crash the */
								/* enemy car if affirmative.           */
								if (fabs(k1 - k2) > 100*PIXEL_CTNT) {
									if (player && !c_collided->car->player &&
									        fabs(k2) > 2.5F*fabs(k2b) &&
									        c_collided->position < position &&
									        c_collided->position + (track->get_length() / 2) > position) {
										{
											F1S_SFX *s = new F1S_SFX();
											s->x = int(c_collided->car->tcar->x);
											s->y = int(c_collided->car->tcar->y);
											s->SFX = SFX_HIT_CRASH;
											sfx->Add(s);
										}

										c_collided->car->set_state(2);
										c_collided->car->speed_x = c_collided->car->old_speed_x;
										c_collided->car->speed_y = c_collided->car->old_speed_y;
										c_collided->car->speed = c_collided->car->speed * 0.5F;
									} else {
										if (!player && c_collided->car->player &&
										        fabs(k1) > 2.5F*fabs(k1b) &&
										        c_collided->position > position &&
										        c_collided->position + (track->get_length() / 2) > position) {
											{
												F1S_SFX *s = new F1S_SFX();
												s->x = int(tcar->x);
												s->y = int(tcar->y);
												s->SFX = SFX_HIT_CRASH;
												sfx->Add(s);
											}

											set_state(2);
											speed_x = c_collided->car->old_speed_x;
											speed_y = c_collided->car->old_speed_y;
											speed = c_collided->car->speed * 0.5F;
										} 
									} 
								} 
							}
						}

					} // if
				} 
			}


			break;

		case 2:
			/* CRASHED: */
			{
				jumping_timmer = 0;
				state_timmer++;

				if (state_timmer < 50) {
					set_tile(((state_timmer / 8) % 5) + 1);
				} else {
					if (state_timmer < 100) {
						set_tile((((state_timmer - 50) / 12) % 5) + 1);
					} else {
						set_tile((((state_timmer - 100) / 16) % 5) + 1);
					} 
				} 

				speed *= 0.95F;

				{
					float n = float(sqrt(speed_x * speed_x + speed_y * speed_y));

					if (n == 0)
						n = 1;

					tcar->x += float((speed_x * speed * PIXEL_CTNT) / n);

					tcar->y += float((speed_y * speed * PIXEL_CTNT) / n);
				}

				tcar->z = -33;

				tshadow->x = tcar->x + 2;
				tshadow->y = tcar->y + 2;
				tshadow->z = tcar->z + 2;
				tshadow->angle = tcar->angle;

				if (speed < 1 &&
				        track->get_road() != 0) {
					List<CRoadPiece> l;
					CRoadPiece *rp, *closer = 0;
					float t1, t2, t3, pa, pw;
					float d, min_d;

					l.Instance(*track->get_road());
					l.Rewind();

					while (l.Iterate(rp)) {
						if (rp->get_path_position(tcar->x, tcar->y, &t1, &t2, &t3, &pa, &pw, &d)) {
							if (closer == 0 ||
							        d < min_d) {
								min_d = d;
								closer = rp;
							} 
						} 
					} 

					if (closer != 0) {
						if (closer->get_path_position(tcar->x, tcar->y, &t1, &t2, &t3, &pa, &pw, &d)) {
							float offset = (closer->w1 + closer->w2) / 4 - 16;
							stop_car();
							tcar->angle = pa + 90;
							tcar->x = float(t1 - offset * sin(tcar->angle));
							tcar->y = float(t2 + offset * cos(tcar->angle));
							set_state(3);
						} 
					} else {
						if (current_piece->get_path_position(tcar->x, tcar->y, &t1, &t2, &t3, &pa, &pw, &d)) {
							float offset = (current_piece->w1 + current_piece->w2) / 4 - 16;
							stop_car();
							tcar->angle = pa + 90;
							tcar->x = float(t1 - offset * sin(tcar->angle));
							tcar->y = float(t2 + offset * cos(tcar->angle));
							set_state(3);
						} 
					} 

					//    if (!player) return false;
				} 
			}

			break;
	} 

	tshadow->x = tcar->x + 2;

	tshadow->y = tcar->y + 2;

	tshadow->z = tcar->z + 2;

	tshadow->angle = tcar->angle;

	/* Extra GFX: */
	{
		CExtraGFX *g;
		List<CExtraGFX> todelete;

		extra_gfx.Rewind();

		while (extra_gfx.Iterate(g)) {
			if (!g->cycle()) {
				todelete.Add(g);
			} 
		} 

		while (!todelete.EmptyP()) {
			g = todelete.ExtractIni();
			extra_gfx.DeleteElement(g);
			delete g;
		} 
	}

	if (player && (state == 0 || state == 1 || state == 3)) {
		/* Out of fuel sound: */
		if ((state_timmer % 100) == 0 && fuel < 100) {
			F1S_SFX *s = new F1S_SFX();
			s->x = int(tcar->x);
			s->y = int(tcar->y);
			s->SFX = SFX_FUEL;
			sfx->Add(s);
		} 
	} 

	return true;

} 


bool CCar::collision(float x, float y, float z)
{
	if (fabs(tcar->z - z) < 8)
		return tcar->get_cmc()->collision(tcar->x, tcar->y, tcar->angle, x, y);

	return false;
} 


bool CCar::collision(float x, float y, float z, float angle, C2DCMC *cmc)
{
	if (fabs(tcar->z - z) < 8) {
		return get_cmc()->collision(tcar->x, tcar->y, tcar->angle, cmc, x, y, angle);
	} 

	return false;
} 



int CCar::track_cars_collision(float x, float y, float z, float angle, CTrack *track, List<RacingCCar> *cars, RacingCCar **c_collided, CRotatedGLTile **t_collided, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy)
{
	CRotatedGLTile *rt = 0;
	List<RacingCCar> l;
	RacingCCar *c;
	int gx, gy, x1, x2, y1, y2;
	int i, j;

	rt = track->collision(x, y, z, angle, get_cmc());

	if (rt != 0) {
		*t_collided = rt;
		return 1;
	} 

	if (get_state() == 3)
		return 0;

	gx = int(get_x() / car_grid_sx);

	gy = int(get_y() / car_grid_sy);

	x1 = gx - 1;

	x2 = gx + 1;

	y1 = gy - 1;

	y2 = gy + 1;

	if (x1 < 0)
		x1 = 1;

	if (x2 >= CAR_GRID_SIZE)
		x2 = CAR_GRID_SIZE - 1;

	if (y1 < 0)
		y1 = 1;

	if (y2 >= CAR_GRID_SIZE)
		y2 = CAR_GRID_SIZE - 1;

	for (i = x1;i <= x2;i++) {
		for (j = y1;j <= y2;j++) {
			l.Instance(car_grid[i][j]);
			l.Rewind();

			while (l.Iterate(c)) {
				if (c->car != this && c->car->get_state() != 3) {
					if (c->car->collision(x, y, z, angle, get_cmc())) {
						*c_collided = c;
						return 2;
					} 
				} 
			} 
		} 
	} 

	return 0;
} 


RacingCCar *CCar::cars_collision(float x, float y, float z, float angle, List<RacingCCar> *cars, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy)
{
	List<RacingCCar> l;
	RacingCCar *c;
	int gx, gy, x1, x2, y1, y2;
	int i, j;

	if (get_state() == 3)
		return 0;

	gx = int(get_x() / car_grid_sx);

	gy = int(get_y() / car_grid_sy);

	x1 = gx - 1;

	x2 = gx + 1;

	y1 = gy - 1;

	y2 = gy + 1;

	if (x1 < 0)
		x1 = 1;

	if (x2 >= CAR_GRID_SIZE)
		x2 = CAR_GRID_SIZE - 1;

	if (y1 < 0)
		y1 = 1;

	if (y2 >= CAR_GRID_SIZE)
		y2 = CAR_GRID_SIZE - 1;

	for (i = x1;i <= x2;i++) {
		for (j = y1;j <= y2;j++) {
			l.Instance(car_grid[i][j]);
			l.Rewind();

			while (l.Iterate(c)) {
				if (c->car != this && c->car->get_state() != 3) {
					if (c->car->collision(x, y, z, angle, get_cmc())) {
						return c;
					} 
				} 
			} 
		} 
	} 

	return 0;
} 


RacingCCar *CCar::cars_collision(float x, float y, float z, List<RacingCCar> *cars, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy)
{
	List<RacingCCar> l;
	RacingCCar *c;
	int gx, gy, x1, x2, y1, y2;
	int i, j;

	gx = int(get_x() / car_grid_sx);
	gy = int(get_y() / car_grid_sy);
	x1 = gx - 1;
	x2 = gx + 1;
	y1 = gy - 1;
	y2 = gy + 1;

	if (x1 < 0)
		x1 = 1;

	if (x2 >= CAR_GRID_SIZE)
		x2 = CAR_GRID_SIZE - 1;

	if (y1 < 0)
		y1 = 1;

	if (y2 >= CAR_GRID_SIZE)
		y2 = CAR_GRID_SIZE - 1;

	for (i = x1;i <= x2;i++) {
		for (j = y1;j <= y2;j++) {
			l.Instance(car_grid[i][j]);
			l.Rewind();

			while (l.Iterate(c)) {
				if (c->car != this) {
					if (c->car->collision(x, y, z)) {
						return c;
					} 
				} 
			} 
		} 
	} 

	return 0;
} 



void CCar::add_drawing_extras(List<CPlacedGLTile> *extras)
{
	CExtraGFX *g;

	extras->Add(get_shadow_placedtile());
	extras->Add(get_placedtile());

	extra_gfx.Rewind();

	while (extra_gfx.Iterate(g)) {
		extras->Add(g->t);
	} 
} 


float CCar::get_max_speed(void)
{
	return m_game->parameters.SPEED_CTNT*engine->max_rpm*gear->gear[3];
} 


bool CCar::save_configuration(FILE *fp)
{
	fputc(c_type, fp);

	save_float(fp, c_r);
	save_float(fp, c_g);
	save_float(fp, c_b);
	fputc(c_body, fp);
	fputc(c_engine, fp);
	fputc(c_brake, fp);
	fputc(c_suspension, fp);
	fputc(c_gear, fp);
	save_float(fp, c_handycap);

	return true;
} 


bool CCar::load_status(FILE *fp)
{
	load_float(fp, &(tcar->x));
	load_float(fp, &(tcar->y));
	load_float(fp, &(tcar->z));
	load_float(fp, &(tcar->angle));

	load_float(fp, &speed);

	load_float(fp, &speed_x);
	load_float(fp, &speed_y);
	load_float(fp, &speed_a);

	load_float(fp, &fuel);

	current_gear = fgetc(fp);

	load_float(fp, &rpm);

	brake_timmer = fgetc(fp);
	brake_timmer += fgetc(fp) * 256;
	turn_timmer = fgetc(fp);
	turn_timmer += fgetc(fp) * 256;

	load_float(fp, &old_speed_x);
	load_float(fp, &old_speed_y);

	state_timmer = fgetc(fp);
	state_timmer += fgetc(fp) * 256;
	state = fgetc(fp);
	state += fgetc(fp) * 256;

	load_float(fp, &damage_ftyre);
	load_float(fp, &damage_rtyre);
	load_float(fp, &damage_brake);
	load_float(fp, &damage_engine);

	return true;
} 


bool CCar::save_status(FILE *fp)
{
	save_float(fp, tcar->x);
	save_float(fp, tcar->y);
	save_float(fp, tcar->z);
	save_float(fp, tcar->angle);

	save_float(fp, speed);

	save_float(fp, speed_x);
	save_float(fp, speed_y);
	save_float(fp, speed_a);

	save_float(fp, fuel);

	fputc(current_gear, fp);

	save_float(fp, rpm);

	fputc(brake_timmer&0x0ff, fp);
	fputc(brake_timmer >> 8, fp);
	fputc(turn_timmer&0x0ff, fp);
	fputc(turn_timmer >> 8, fp);

	save_float(fp, old_speed_x);
	save_float(fp, old_speed_y);

	fputc(state_timmer&0x0ff, fp);
	fputc(state_timmer >> 8, fp);
	fputc(state&0x0ff, fp);
	fputc(state >> 8, fp);

	save_float(fp, damage_ftyre);
	save_float(fp, damage_rtyre);
	save_float(fp, damage_brake);
	save_float(fp, damage_engine);

	return true;
} 





CExtraGFX::CExtraGFX(CPlacedGLTile *pt)
{
	t = pt;
	speed_x = speed_y = speed_z = 0;
	speed_angle = 0;
	a1 = 1;
	a2 = 0;
	timmer = 0;
	last_time = 8;
} 


CExtraGFX::~CExtraGFX(void)
{
	delete t;
	t = 0;
} 


bool CExtraGFX::cycle(void)
{
	float f;
	t->x += speed_x;
	t->y += speed_y;
	t->z += speed_z;
	t->angle += speed_angle;
	t->zoom += speed_zoom;

	speed_x *= 0.99F;
	speed_y *= 0.99F;
	speed_z *= 0.99F;
	speed_angle *= 0.99F;
	speed_zoom *= 0.99F;

	f = float(timmer) / float(last_time);

	if (f < 0)
		f = 0;

	if (f > 1)
		f = 1;

	t->set_color(1, 1, 1, a1*(1 - f) + a2*f);

	timmer++;

	if (timmer >= last_time)
		return false;

	return true;
} 




