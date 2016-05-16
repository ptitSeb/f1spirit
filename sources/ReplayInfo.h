#ifndef _F1SPIRIT_REPLAYINFO
#define _F1SPIRIT_REPLAYINFO



class CReplayCarStatus
{

	public:
		float x, y, z, a;
		float speed;
		float speed_x, speed_y, speed_a;
		float fuel;
		int current_gear;
		float rpm;
		int brake_timmer, turn_timmer;
		float old_speed_x, old_speed_y;
		int state_timmer, state;
		float damage_ftyre, damage_rtyre, damage_brake, damage_engine;

		int road_offset_change;
		float road_offset;
};


class CReplayEvent
{

	public:
		CReplayEvent();
		~CReplayEvent();



		int cycle;
		int type;

		/* keyboard event: */
		int key, state;

		/* control frame: */
		int n_playercars;
		CReplayCarStatus **playercar_status;
		int n_enemycars;
		CReplayCarStatus **enemycar_status;
};




class CReplayInfo
{

	public:
		CReplayInfo(char *filename, char *folder);
		CReplayInfo(FILE *fp);
		~CReplayInfo();

		class F1SpiritGame *create_game(SDL_Surface *font, KEYBOARDSTATE *k);
		bool replay_cycle(int cycle, KEYBOARDSTATE *k, F1SpiritGame *game);

		/* Decoders for different versions of F1-Spirit: */
		void version000_decoder(FILE *fp);
		void version_ac6_decoder(FILE *fp);
		void version_ac7_decoder(FILE *fp);
		void version_ac8_decoder(FILE *fp);
		void version_ac9_decoder(FILE *fp);

		/* basic info: */
		bool decoded;
		char version_tag[7];
		char *filename;
		int track_num;
		char *track_name;
		CPlayerInfo *player;
		int *times;
		int length;
		int year, month, day, hour, min, sec;

		/* game parameters: */
		F1S_GParameters parameters;

		/* car configuration: */
		int n_enemy_cars;
		int *ec_type;
		float *ec_r, *ec_g, *ec_b, *ec_handycap;
		int *ec_body, *ec_engine, *ec_brake, *ec_suspension, *ec_gear;
		int *ec_AI_type;

		int n_player_cars;
		int *pc_type;
		float *pc_r, *pc_g, *pc_b, *pc_handycap;
		int *pc_body, *pc_engine, *pc_brake, *pc_suspension, *pc_gear;

		/* event list: */
		List<CReplayEvent> events;
};


/* Replay file format: */
/*



6 bytes: version_tag = "F1S000"

1 byte: year
1 byte: month
1 byte: mday
1 byte: hour
1 byte: min
1 byte: sec

? bytes: player info
 - 1 byte: player name length = l1
 - l1 bytes: player name
 - 21 bytes: race points
 - 84 bytes: race times (21 * 4 bytes)
 - 1 byte: nplayers
 - 25*nplayers bytes: keyboard configuration (up,down,left,right,acc,brake + 1 byte: zoom_speed)
 - nplayers bytes: joystick
 - nplayers bytes: default_camera
 - 1 byte: music volume
 - 1 byte: sfx volume
 - 1 byte scoreboard type

1 byte: track num
1 byte: track name length = l2
l2 bytes: track name

1 byte: nº enemy cars = l3
(22 + 73)*l3 bytes: car configuration + status

1 byte: nº player cars
(22 + 73)*l3 bytes: car configuration + status

eventos
2 bytes: cycle
1 byte: event
 0: keyboard -> +3 bytes
 1: car status -> + 73 bytes * ncars (enemy + player)
 2: quit
 3: player car completes a lap -> +1 byte
 4: race finished


*/

#endif
