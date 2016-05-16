#ifndef _F1SPIRIT_APP
#define _F1SPIRIT_APP

#define APP_STATE_DISCLAIMER 1
#define APP_STATE_KONAMI  2
#define APP_STATE_TITLE   3
#define APP_STATE_GAMESTART  4
#define APP_STATE_HISCORE  5
#define APP_STATE_MENU   6
#define APP_STATE_RACE   7
#define APP_STATE_RACE_RESULT 8
#define APP_STATE_TRACKLOAD  9
#define APP_STATE_ENDSEQUENCE 10
#define APP_STATE_REPLAYMANAGER 11
#define APP_STATE_GAMEOPTIONS 12
#define APP_STATE_END   -1


#define MSG_ACK    0
#define MSG_NACK   1
#define MSG_REGISTER  2 /* va acompañado de 'size' (2 bytes) + 'name' (size bytes) */
#define MSG_UNREGISTER  3
#define MSG_CHAT   4 /* namesize + name + msgsize + msg */
#define MSG_NAME_QUERY  5
#define MSG_NAME   6
#define MSG_ADDCLIENT  7
#define MSG_SUBCLIENT  8
#define MSG_ACTIVITY_CHECK 9
#define MSG_ACTIVITY_ANSWER 10


class HiScoreEntry
{

	public:
		char name[32];
		int points;
		int totaltime;
};


class F1SComputer
{

	public:
		F1SComputer();
		~F1SComputer();

		int activity_counter; /* is used by the server to detect if any client has lost connection */
		char name[32];
		IPaddress ip;
		TCPsocket tcp_socket;
		UDPsocket udp_socket;
		int tcp_port;
		int udp_port;
};

class ChatMessage
{

	public:
		char name[32];
		char message[256];
};


class F1SpiritApp
{

	public:
		F1SpiritApp();
		~F1SpiritApp();

		bool cycle(KEYBOARDSTATE *k);
		void draw();

		bool load_configuration(char *file);
		bool save_configuration(char *file);
		bool read_configuration_token(FILE *fp, char *token);

		void blank_hiscores(void);
		void hiscores_from_players(void);
		bool load_hiscores(void);
		bool save_hiscores(void);
		void add_hiscore_points(char *pname, int points, int time);
		void add_hiscore_time(char *pname, int time, int race);
		void add_hiscore_bestlap(char *pname, int time, int race);

		/* STATE SPECIFICS: */
		int disclaimer_cycle(KEYBOARDSTATE *k);
		int konami_cycle(KEYBOARDSTATE *k);
		int title_cycle(KEYBOARDSTATE *k);
		int gamestart_cycle(KEYBOARDSTATE *k);
		int hiscore_cycle(KEYBOARDSTATE *k);
		int menu_cycle(KEYBOARDSTATE *k);
		int race_cycle(KEYBOARDSTATE *k);
		int gameoptions_cycle(KEYBOARDSTATE *k);
		int race_result_cycle(KEYBOARDSTATE *k);
		int trackload_cycle(KEYBOARDSTATE *k);
		int endsequence_cycle(KEYBOARDSTATE *k);
		int replaymanager_cycle(KEYBOARDSTATE *k);

		void disclaimer_draw(void);
		void konami_draw(void);
		void title_draw(void);
		void gamestart_draw(void);
		void hiscore_draw(void);
		void menu_draw(void);
		void race_draw(void);
		void gameoptions_draw(void);
		void race_result_draw(void);
		void trackload_draw(void);
		void endsequence_draw(void);
		void replaymanager_draw(void);

		void menu_create_menu(void);

		bool m_skip_intro_screens;

	private:

		int state;
		int state_cycle;
		SDL_Surface *font;
		SDL_Surface *small_font;

		CPlayerInfo *friendly_player;
		CPlayerInfo *current_player;
		char player_filename[80];

		SOUNDT S_wrong, S_pause;

		/* CHEATS: */
		bool cheat_max_point;

		/* STATE SPECIFICS: */

		/* DISCLAIMER: */
		GLTile *disclaimer_image;

		/* KONAMI: */
		GLTile *konami_image1, *konami_image2;

		/* TITLE: */
		GLTile *title_parta;
		GLTile *title_partb;
		GLTile *title_partc;
		GLTile *title_partd;
		GLTile *title_complete;
		GLTile *title_konami, *title_braingames, *title_space, *title_start;
		int title_state;
		F1SpiritGame *title_game;
		CReplayInfo *title_replay;
		KEYBOARDSTATE title_replay_keyboard;

		/* GAMESTART: */
		GLTile *gamestart_1;
		GLTile *gamestart_2;

		/* HISCORE: */
		List<HiScoreEntry> hiscore_list;
		int hiscore_time[N_TRACKS];
		char *hiscore_time_names[N_TRACKS];
		int hiscore_bestlap[N_TRACKS];
		char *hiscore_bestlap_names[N_TRACKS];

		int hiscore_action;
		int hiscore_showing;
		int hiscore_timmer;
		GLTile *hiscore_table[26];
		bool hiscore_table_regen;

		/* MENU: */
		GLTile *menu_flag;
		GLTile *menu_background;
		char *menu_title[2];
		char *menu_options[2];
		int menu_selected[2];
		int menu_selected_timmer[2];
		int menu_noptions[2];
		int menu_first_option[2];
		int menu_option_type[2][256];
		int menu_option_parameter[2][256];
		int menu_prev_nmenus;
		int menu_x[2];
		bool menu_redefining_key;
		bool menu_force_rebuild_menu;
		int menu_state;
		int menu_current_menu;
		unsigned char menu_editing[80];
		int menu_fading;
		int menu_fading_ctnt;
		bool menu_editing_valid;
		GLTile *menu_trackviewing_background;
		GLTile *menu_locked;
		bool menu_track_locked;
		char menu_stored_player_name[80];
		bool menu_server_registered;

		int menu_selected_track;
		int menu_selected_nplayers;

		GLTile *menu_track, *menu_track_frame, *menu_points_frame;
		int menu_showing_points;
		int menu_showing_track;
		int menu_showing_track_timmer;
		int menu_showing_track_timmer2;

		class F1SpiritTrackViewer *menu_track_viewer;

		GLTile *menu_car_top[3], *menu_car_side[3], *menu_car_frame[3];
		GLTile *menu_info_frame;
		int menu_showing_car_type;
		int menu_showing_car;
		int menu_showing_car_timmer;

		GLTile *menu_piece[6], *menu_piece_frame[6];
		int menu_showing_piece_type;
		int menu_showing_piece;
		int menu_showing_piece_timmer;
		int menu_showing_piece_x, menu_desired_showing_piece_x;

		GLTile *menu_design;
		int menu_showing_design_timmer;
		int **menu_selected_part;
		int menu_selected_car[4];
		int menu_selecting_player;

		int menu_readme_start_y, menu_readme_move_y;
		List<GLTile> menu_readme_line;

		SOUNDT S_menu_move, S_menu_select;

		int menu_multiplayer_n_enemycars;
		int menu_multiplayer_enemy_speed;

		/* MENU - NETWORK: */

		class F1Shttp *menu_register_http;
		int menu_server_register_timmer;
		int menu_client_register_timmer;
		GLTile *menu_playerlist_frame, *menu_chat_frame;
		int menu_playerlist_timmer;
		int menu_chat_timmer;
		List<ChatMessage> menu_chat_messages;
		SDLNet_SocketSet menu_socket_set;
		bool menu_local_is_server;
		List<F1SComputer> menu_registered_clients;
		List<F1SComputer> menu_available_servers;
		F1SComputer menu_local_computer;
		F1SComputer *menu_selected_server;

		/* RACE: */
		int race_state;
		int race_state_timmer;
		F1SpiritGame *race_game;
		int race_desired_action; /* 0 : quit, 1 : restart */

		/* RACE RESULT: */
		int raceresult_state;
		int raceresult_timmer;
		int raceresult_selected;
		char raceresult_editing[80];
		bool raceresult_trackrecord;
		bool raceresult_laprecord;
		bool raceresult_replaysaved;

		/* END SEQUENCE: */
		int endsequence_state;
		int endsequence_timmer;
		GLTile *endsequence_images[5];
		int endsequence_returnstate;

		/* REPLAY MANAGER: */
		int replaymanager_state;
		int replaymanager_timmer;
		int replaymanager_first;
		F1SpiritGame *replaymanager_game;
		bool replaymanager_replay_playing;
		int replaymanager_replay_viewport;
		int replaymanager_replay_desiredviewport;
		KEYBOARDSTATE replaymanager_replay_keyboard;
		List<CReplayInfo> replaymanager_replays;
		List<GLTile> replaymanager_replaytiles;
		GLTile *replaymanager_frame1, *replaymanager_frame2;
};

#endif
