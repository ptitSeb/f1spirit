#ifndef _F1SPIRIT_PLAYERCAR
#define _F1SPIRIT_PLAYERCAR

class PlayerCCar: public RacingCCar
{

	public:
		PlayerCCar();
		~PlayerCCar();

		int vx, vy;
		int vdx, vdy;
		int up, down, left, right, accelerate, brake;

		float c_old_x, c_old_y, c_old_a, c_old_z;
		float c_x, c_y, c_a, c_z, c_a_speed;

		bool wrong_way;

		Uint32 lap_time[MAX_NLAPS];
		int show_lap_time;

		int show_damage_timmer, show_damage_cycle;

		bool inside_pit;
		int pit_time;
		int show_pit_stop;

		int last_sign_showing;
		int times_sign_showed;
};


#endif
