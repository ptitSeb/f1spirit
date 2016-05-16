#ifndef _F1SPIRIT_GAME
#define _F1SPIRIT_GAME


#define SFX_BRAKE_STOCK  1
#define SFX_HIT_WOOD  2
#define SFX_HIT_METAL  3
#define SFX_HIT_CRASH  4
#define SFX_HIT_CAR   5
#define SFX_FUEL   6
#define SFX_WATER   7
#define SFX_SQUEAL   8
#define SFX_CHICANE   9
#define SFX_PASS   10
#define SFX_RAIN   15

#define CAR_GRID_SIZE  64



class F1S_SFX
{

	public:
		int x, y;
		int SFX;
};


class RainDrop
{

	public:
		float inc_x, inc_y, inc_z;
		float x, y, z;
};


class F1SpiritGame
{

		friend class F1SpiritApp;

		friend class CReplayInfo;

		friend class CCar;

	public:
		F1SpiritGame(CPlayerInfo *player, int ntrack, int nplayers, int ncars, int enemy_speed, int *selected_car, int **selected_part, SDL_Surface *f, KEYBOARDSTATE *k);
		F1SpiritGame(CReplayInfo *ri, SDL_Surface *f, KEYBOARDSTATE *k);
		~F1SpiritGame();

		void initialize_track(int ntrack);

		void set_without_music(void) {
			play_music = false;
		};

		bool cycle(KEYBOARDSTATE *k);
		void draw(bool draw_scoreboard);

		void draw_dispersed_hud(PlayerCCar *v);
		void draw_mini_dispersed_hud(PlayerCCar *v);

		int get_nplayers(void);
		Uint32 get_player_time(int p);
		Uint32 get_player_bestlap(int p);
		int get_player_position(int p);
		bool get_racefinished(void);

		void finish_replay(void);

		void draw_nightmode_nostencil(PlayerCCar *v);
		void draw_nightmode_stencil(PlayerCCar *v);
		void draw_rain(PlayerCCar *v, float rain_strength);
		void cycle_rain(PlayerCCar *v, float rain_strength);

		void pauseSFX(void) {
			List<PlayerCCar> l;
			PlayerCCar *c;

			l.Instance(player_cars);
			l.Rewind();
			while(l.Iterate(c)) c->pauseSFX();
		}

		void resumeSFX(void) {
			List<PlayerCCar> l;
			PlayerCCar *c;

			l.Instance(player_cars);
			l.Rewind();
			while(l.Iterate(c)) c->resumeSFX();
		}


		int rain_channel;
		SOUNDT S_rain;

	private:

		CCar *create_car(int type, int body, int engine, int brake, int suspension, int gear, float pr, float pg, float pb, bool player, SOUNDT engine_sound, int engine_channel, float handycap);

		SDL_Surface *font;

		CPlayerInfo *current_player;
		char player_filename[80];
		CTrack *track;
		List<GLTile> tiles;

		SOUNDT S_car_pass;
		SOUNDT S_outof_fuel, S_water_splash, S_squeal;
		SOUNDT S_race_finished, S_chicane;
		SOUNDT S_car_hit1, S_car_hit2, S_car_hit3, S_car_hit4;
		SOUNDT S_car_brake, S_car_engine;
		SOUNDT S_semaphore_high, S_semaphore_low;

		bool play_music;

		/* */
		GLTile *water_tile;
		GLTile *rock_tile;
		GLTile *wood_tiles[3];
		int n_wood_tiles;


		/* REPLAY: */
		char *replay_filename;
		FILE *replay_fp;
		int replay_cycle;


		/* RACE: */
		SDL_Surface *race_car_sfc;
		SDL_Surface *race_minimap_car_sfc, *race_minimap_enemycar_sfc;
		List<PlayerCCar> player_cars;
		List<EnemyCCar> enemy_cars;
		List<F1S_SFX> sfxs;
		GLTile *race_minitrack;
		float race_minimap_zoom;
		int race_time;
		int race_nlaps;
		GLTile *race_semaphore[5];
		int race_first_sfx_channel;
		int race_sfx_channel;

		int race_state;  /* 0: semaphore, 1: racing, 2:finished, 3:fading out */
		int race_state_timmer;
		int race_semaphore_timmer;
		int race_outoffuel_timmer;

		List<RacingCCar> race_positions; /* order of the cars in the race: */
		List<RacingCCar> **car_grid;  /* Used to accelerate the car collisions */
		int car_grid_sx, car_grid_sy;  /* Size of the individual cells of the grid */

		/* hud graphics: */
		GLTile *race_lap_tile[5];
		GLTile *race_rpm_tile[9];
		GLTile *race_damage;
		GLTile *race_fuel;

		/* new hud graphics: */
		GLTile *hud_fixed_1;
		GLTile *hud_fixed_2;
		GLTile *hud_fixed_3, *hud_fixed_3_sidefuel;
		GLTile *hud_fixed_4;
		GLTile *hud_damage;
		GLTile *hud_rpm[11];
		GLTile *hud_gear[6];
		GLTile *hud_speed_font[10];
		GLTile *hud_time_font[12];
		GLTile *minihud_fuel;

		/* Signs: */
		GLTile *signs[10];

		GLTile *race_extra_tiles[4];

		GLTile *road_tile[3];
		GLTile *road_ltile[2];
		GLTile *road_rtile[2];
		GLTile *road_lines;

		char *race_sfx_folder, *race_default_sfx_folder;

		// Weather:
		GLTile *race_cloud[3];
		List<CPlacedGLTile> race_clouds;
		float m_rain_strength, m_current_rain_strength;
		int m_rain_period1, m_rain_period2;
		int m_rain_times;

		// rain:
		List<RainDrop> m_rain_drops;

		/* GAME PARAMETERS: */
		F1S_GParameters parameters;
};

#endif
