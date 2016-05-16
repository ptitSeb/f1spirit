#ifndef _F1SPIRIT_PLAYER
#define _F1SPIRIT_PLAYER


int get_player_list(List<char> *names);

struct F1SpiritDATE {
	int day;
	int month;
	int year;
};


class CPlayerInfo
{

	public:
		CPlayerInfo(void);
		CPlayerInfo(char *name, char *password);
		~CPlayerInfo();

		bool load_ac5(FILE *fp); /* loads an OLD (version AC5) player file format */
		bool load(FILE *fp, int ntracks, char *version);
		bool save(FILE *fp);

		void add_player();
		void sub_player();

		char *get_name(void) {
			return name;
		};

		char *get_encrypted_password(void) {
			return encrypted_password;
		};

		int get_joystick(int player) {
			return joystick[player];
		};

		void set_joystick(int player, int v) {
			joystick[player] = v;
		};

		int get_nplayers(void) {
			return nplayers;
		};

		int get_key(int player, int key) {
			switch (key) {

				case 0:
					return up[player];
					break;

				case 1:
					return down[player];
					break;

				case 2:
					return left[player];
					break;

				case 3:
					return right[player];
					break;

				case 4:
					return accelerate[player];
					break;

				case 5:
					return brake[player];
					break;
			} /* switch */

			return SDLK_UNKNOWN;
		} /* get_key */

		void set_key(int player, int key, int k) {
			switch (key) {

				case 0:
					up[player] = k;
					break;

				case 1:
					down[player] = k;
					break;

				case 2:
					left[player] = k;
					break;

				case 3:
					right[player] = k;
					break;

				case 4:
					accelerate[player] = k;
					break;

				case 5:
					brake[player] = k;
					break;
			} /* switch */
		} /* set_key */

		int get_zoom_speed(int player) {
			return zoom_speed[player];
		};

		void set_zoom_speed(int player, int zs) {
			zoom_speed[player] = zs;
		};

		int get_points(void);
		int get_points(int i) {
			return race_points[i];
		};

		Uint32 get_time(int i) {
			return race_time[i];
		};

		Uint32 get_bestlap(int i) {
			return race_bestlap[i];
		};

		int get_position(int i) {
			return race_position[i];
		};

		void set_points(int i, int p);
		void set_time(int i, Uint32 p);
		void set_bestlap(int i, Uint32 p);
		void set_position(int i, int p);

		int get_music_volume(void) {
			return music_volume;
		};

		int get_sfx_volume(void) {
			return sfx_volume;
		};

		void set_music_volume(int v) {
			music_volume = v;
		};

		void set_sfx_volume(int v) {
			sfx_volume = v;
		};

		void encrypt_password(char *password, char *result);

	private:
		char *name;
		char *encrypted_password;
		int race_points[N_TRACKS];
		Uint32 race_time[N_TRACKS];
		Uint32 race_bestlap[N_TRACKS];
		int race_position[N_TRACKS];

		F1SpiritDATE time_date[N_TRACKS];
		F1SpiritDATE lap_date[N_TRACKS];
		F1SpiritDATE position_date[N_TRACKS];

		int music_volume;
		int sfx_volume;

		unsigned int nplayers;
		int *zoom_speed;

		/* keyboard configuration: */
		int *joystick;
		Uint32 *up, *down, *left, *right, *accelerate, *brake;
};


#endif
