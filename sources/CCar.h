#ifndef _F1SPIRIT_CAR
#define _F1SPIRIT_CAR


class CExtraGFX
{

	public:
		CExtraGFX(CPlacedGLTile *pt);
		~CExtraGFX(void);

		bool cycle(void);

		CPlacedGLTile *t;
		float speed_x, speed_y, speed_z;
		float speed_angle, speed_zoom;
		float a1, a2;
		int timmer;
		int last_time;
};


/*
stock:
   - V6 1500 DOHC SUPER-CHARGER LOW-COMPRESS 580PS
   - V6 1490 DOHC SUPER-CHARGER WIDE-TORUQUE 600PS
   - V8 1200 DOHC-TURBO EASY-CONTROL 700PS
rally:
   - V6 1500 DOHC SUPER-CHARGER LOW-COMPRESS 580PS
   - V6 1490 DOHC SUPER-CHARGER WIDE-TORUQUE 600PS
   - V8 1200 DOHC-TURBO EASY-CONTROL 700PS
F3:
   - V6 2000 DOHC SUPER-CHARGER LOW-COMPRESS 175PS
   - V6 1800 DOHC SUPER-CHARGER WIDE-TORUQUE 165PS
   - V8 2000 DOHC-TURBO EASY-CONTROL 175PS
F3000:
   - V6 1500 DOHC SUPER-CHARGER LOW-COMPRESS 400PS
   - V6 1490 DOHC SUPER-CHARGER WIDE-TORUQUE 450PS
   - V8 1200 DOHC-TURBO EASY-CONTROL 470PS
Endurance:
   - V6 1500 DOHC SUPER-CHARGER LOW-COM PRESS 780PS
   - V6 1490 DOHC SUPER-CHARGER WIDE-TORUQUE 870PS
   - V8 1200 DOHC-TURBO EASY-CONTROL 900PS
F1:
   - V6 1500 DOHC SUPER-CHARGER LOW-COMPRESS 1180PS
   - V6 1490 DOHC SUPER-CHARGER WIDE-TORQUE 1100PS
   - V8 1200 DOHC-TURBO EASY-CONTROL 1200PS
*/

class CEngine
{

	public:
		CEngine(float p, float c, float e, int max_rpm);

		/* V? */
		/* 1200-2000 */
		/* DOHC SUPER-CHARGER / DOHC-TURBO */
		/* LOW-COMPRESS / WIDE-TORQUE / EASY-CONTROL */
		/* ?PS */
		float power, consumption;
		float endurance;
		int max_rpm;

};


/*
   - Strong-Doby CARBON FIBER FRAME
   - Balanced Setting CARBON FIBER FRAME
   - Light weight CARBON FIBER FRAME
*/

class CBody
{

	public:
		CBody(int w, float r, float a, float cs);

		float aerodynamics;
		int weight;
		float damage_absortion;
		float crash_speed;

};


/*
   - Front Disk TYPE-A Rear Disk TYPE A
   - Front Disk TYPE B Rear DRUM
   - Front Disk TYPE-B Rear DISK TYPE B
*/

class CBrake
{

	public:
		CBrake(int f, int b);

		int front;
		int back;
};


/*
   - TWINTUBE GASSHOCK TYPE SOFT-SETTING
   - TWINTUBE ADJUSTAB LE TYPE MEDIUM SETTING
   - MONOTUBE DILSHOCK TYPE HARD-SETTING
*/

class CSuspension
{

	public:
		CSuspension(float t);

		float turn_speed;

};


/*
   - 4SPEED Automatic NEW TYPE
   - 4SPEED Manual 1-13.26, 2-18.26, 3-24.25, 4-27.21
   - 4SPEED Manual HI-GEAR 1-14.29, 2-22.29, 3-23.24, 4-26.21
*/

class CGear
{

	public:
		CGear(bool automatic, float g1, float g2, float g3, float g4);

		bool automatic;
		float gear[4];

};


class CCar
{

		friend class EnemyCCar;

		friend class F1SpiritGame;

		friend class CReplayInfo;

		friend void carEngineSoundEffect(int chan, void *stream, int len, void *udata);

	public:
		CCar(CEngine *e, CBody *body, CBrake *brake, CSuspension *s, CGear *gear, int type, int model, float r, float g, float b, bool player, SOUNDT engine_sound, int engine_channel, GLTile **pextra_tiles, SDL_Surface *car_sfc, SDL_Surface *car_minimap_sfc, class F1SpiritGame *game);
		~CCar();

		void pauseSFX() {
			if (carengine_channel>=0) Mix_Pause(carengine_channel);
		}

		void resumeSFX() {
			if (carengine_channel>=0) Mix_Resume(carengine_channel);
		}

		bool save_configuration(FILE *fp);

		bool load_status(FILE *fp);
		bool save_status(FILE *fp);

		void draw_mini(float a, float x, float y, float z, float angle, float zoom);

		void set_x(float x) {
			tcar->x = x;
			tshadow->x = x + 2;
		};

		void set_y(float y) {
			tcar->y = y;
			tshadow->y = y + 2;
		};

		void set_z(float z) {
			tcar->z = z;
			tshadow->z = z;
		};

		void set_a(float a) {
			tcar->angle = a;
			tshadow->angle = a;
		};

		float get_x(void) {
			return tcar->x;
		};

		float get_y(void) {
			return tcar->y;
		};

		float get_z(void) {
			return tcar->z;
		};

		float get_a(void) {
			return tcar->angle;
		};

		void set_speed_x(float x) {
			speed_x = x;
		};

		void set_speed_y(float y) {
			speed_y = y;
		};

		void set_speed_a(float a) {
			speed_a = a;
		};

		int get_state(void) {
			return state;
		};

		void set_state(int s) {
			if (state == 0 && s == 1)
				rpm = 0;

			state = s;

			state_timmer = 0;
		};

		void stop_car(void) {
			speed = 0;
			rpm = 0;
			current_gear = 0;
			old_speed_x = speed_x = 0;
			old_speed_y = speed_y = 0;
		};

		float get_speed_x(void) {
			return speed_x;
		};

		float get_speed_y(void) {
			return speed_y;
		};

		float get_speed_a(void) {
			return speed_a;
		};

		float get_speed(void) {
			return speed;
		};

		float get_max_speed(void);

		int get_terrain(void) {
			return old_terrain;
		};

		float get_fuel(void) {
			return fuel;
		};

		void set_fuel(float f) {
			fuel = f;

			if (fuel > max_fuel)
				fuel = max_fuel;
		};

		float get_rpm(void) {
			return rpm;
		};

		float get_relative_rpm(void) {
			return float(rpm) / engine->max_rpm;
		};

		int get_currentgear(void) {
			return current_gear;
		};

		float get_trackstrength(void) {
			return float(sqrt((old_speed_x -speed_x)*(old_speed_x - speed_x) + (old_speed_y - speed_y)*(old_speed_y - speed_y)));
		};

		C2DCMC *get_cmc(void) {
			return tcar->get_cmc();
		};

		bool cycle(bool accelerate, bool brake, int gear, int turn, class CRoadPiece *current_piece, class CTrack *track, List<class RacingCCar> *cars, List<class F1S_SFX> *sfx, int sfx_volume, bool check_trackcollision, int terrain, float position, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy);

		int get_ntiles(void) {
			return ntiles;
		};

		GLTile *get_tile(int i) {
			return tiles[i];
		};

		GLTile *get_shadow_tile(int i) {
			return shadow_tiles[i];
		};

		CPlacedGLTile *get_placedtile(void) {
			return tcar;
		};

		CPlacedGLTile *get_shadow_placedtile(void) {
			return tshadow;
		};

		void set_tile(int i) {
			tcar->set_tile(tiles[i]);
			tshadow->set_tile(shadow_tiles[i]);
		};

		void set_carengine_channel(int ch) {
			carengine_channel = ch;
		};

		float get_ftyre_damage(void) {
			return damage_ftyre;
		};

		float get_rtyre_damage(void) {
			return damage_rtyre;
		};

		float get_brake_damage(void) {
			return damage_brake;
		};

		float get_engine_damage(void) {
			return damage_engine;
		};

		void set_ftyre_damage(float d) {
			damage_ftyre = d;

			if (damage_ftyre < 0)
				damage_ftyre = 0;

			if (damage_ftyre > 1)
				damage_ftyre = 1;
		};

		void set_rtyre_damage(float d) {
			damage_rtyre = d;

			if (damage_rtyre < 0)
				damage_rtyre = 0;

			if (damage_rtyre > 1)
				damage_rtyre = 1;
		};

		void set_brake_damage(float d) {
			damage_brake = d;

			if (damage_brake < 0)
				damage_brake = 0;

			if (damage_brake > 1)
				damage_brake = 1;
		};

		void set_engine_damage(float d) {
			damage_engine = d;

			if (damage_engine < 0)
				damage_engine = 0;

			if (damage_engine > 1)
				damage_engine = 1;
		};

		bool collision(float x, float y, float z);
		bool collision(float x, float y, float z, float angle, C2DCMC *cmc);

		bool get_new_damage(void) {
			return new_damage;
		};

		void set_new_damage(bool d) {
			new_damage = d;
		};

		SOUNDT getcarEngineSound(void) {
			return S_carengine;
		}

		/* 0 : no collision */
		/* 1 : track collision */
		/* 2 : car collision: car stored in "result" */
		int track_cars_collision(float x, float y, float z, float angle, CTrack *track, List<RacingCCar> *cars, RacingCCar **c_collided, CRotatedGLTile **t_collided, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy);
		RacingCCar *cars_collision(float x, float y, float z, float angle, List<RacingCCar> *cars, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy);
		RacingCCar *cars_collision(float x, float y, float z, List<RacingCCar> *cars, List<RacingCCar> **car_grid, int car_grid_sx, int car_grid_sy);

		void add_drawing_extras(List<CPlacedGLTile> *extras);

		/* Debug: */
		float last_col_angle;
		float last_col_nx, last_col_ny;

		int chicane_timmer;

	private:

		/* Characteristics: */
		CEngine *engine;
		CBody *body;
		CBrake *brake;
		CSuspension *suspension;
		CGear *gear;

		F1SpiritGame *m_game;
		bool player;

		/* Physics: */
		float speed;
		float speed_x, speed_y, speed_a;
		float max_fuel, fuel;
		int current_gear;
		float rpm;
		int brake_timmer;
		int turn_timmer;

		int spinning_start_timmer;
		float spinning_speed;
		int spinning_timmer;

		int jumping_magnitude;
		int jumping_timmer;

		float old_speed_x, old_speed_y;
		int old_terrain;

		int state_timmer;
		int state; /* 0:start line, 1:driving, 2:crashed, 3: reappeared */

		/* Damage: */
		float damage_ftyre, damage_rtyre, damage_brake, damage_engine;
		bool new_damage;

		/* Configuration: */
		int c_type;
		float c_r, c_g, c_b;
		int c_body, c_engine, c_brake, c_suspension, c_gear;
		float c_handycap;

		/* Graphics: */
		GLTile **extra_tiles;
		CPlacedGLTile *tcar, *tshadow;
		int ntiles;
		GLTile **tiles;
		GLTile **shadow_tiles;
		GLTile *mini_tile;

		List<CExtraGFX> extra_gfx;

		/* Sound: */
		int tyre_squeal_timmer;
		int water_splash_timmer;
		int carengine_position;
		SOUNDT S_carengine;
		int carengine_channel;
		bool carengine_sounding;
};



#endif
