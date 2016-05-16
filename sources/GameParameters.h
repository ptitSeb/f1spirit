#ifndef _F1SPIRIT_GAME_PARAMETERS
#define _F1SPIRIT_GAME_PARAMETERS


class F1S_GParameters
{

	public:

		F1S_GParameters();

		void instance(F1S_GParameters *p);

		bool load_ascii(char *fname);
		bool load_bin(FILE *fp);
		bool load_bin_AC8(FILE *fp);
		bool load_bin_AC5(FILE *fp);
		bool save_bin(FILE *fp);
		bool read_parameters_token(FILE *fp, char *token);

		int ntracks;
		int race_cars[N_TRACKS];
		float base_handycap[N_TRACKS];
		float handycap_decrement[N_TRACKS];
		float rain_probability[N_TRACKS];
		float PIXEL_CTNTS[6];
		float SPEED_CTNT;
		float MAX_FUEL;
		float fuel_consumption_k1, fuel_consumption_k2;
		float fuel_recharge_speed;
		float repair_speed;
		float road_adherence;
		float grass_adherence;
		float sand_adherence;
		float road_friction;
		float grass_friction;
		float sand_friction;
		float car_type_adherence[6];
		int car_type_spinning_threshold[6];
		int squeal_threshold;
		int spinning_threshold;
		float rtyre_damage_influence;
		float ftyre_damage_influence;
		float engine_damage_influence;
		float brake_damage_influence;
};


#endif
