#ifndef _F1_SPIRIT_TRACK
#define _F1_SPIRIT_TRACK


class CTyreMark
{

	public:
		float r, g, b;
		float x, y, z;
		float x2, y2, z2;
		int t;

		float qx[4], qy[4];
};


/* The CTRack class only contains the background of the track (including people), but */
/* does not contain the cars */

class CTrack
{

		friend class F1SpiritGame;

		friend class TrackEditorApp;

		friend class F1SpiritApp;

		friend class EnemyCCar;

	public:
		CTrack();
		CTrack(int number, FILE *fp, List<GLTile> *tiles);
		~CTrack();

		void save(FILE *fp, List<GLTile> *tiles);

		void cycle(void);
		void draw(SDL_Rect *vp, float x, float y, float zoom, List<CPlacedGLTile> *extras, GLTile **road_textures, GLTile **lroad_textures, GLTile **rroad_textures, GLTile *road_line);
		SDL_Surface *draw_minimap(int w, int h, float *zoom);
		SDL_Surface *draw_vertical_minimap(int w, int h, int nlaps, float *zoom);

		int get_dx() {
			return dx;
		};

		int get_dy() {
			return dy;
		};

		List<CRoadPiece> *get_road(void) {
			return &road;
		};

		CRotatedGLTile *collision(float x, float y, float z, float a, C2DCMC *cmc);
		CRotatedGLTile *collision(float x, float y, float z);

		void add_tile(CRotatedGLTile *t);
		void del_tile(float x, float y);
		void rotate_tile(float x, float y, float inc);
		void rotate_tile2(float x, float y, float inc);
		void move_tiles(float x, float y, float z);

		void set_background(int type, List<GLTile> *tiles);
		int get_background_type(void) {
			return background_type;
		};

		int get_track_number(void) {
			return track_number;
		};

		const char *get_track_name(void);

		void change_size(int ndx, int ndy);

		float get_length(void);
		float get_position(float x, float y); // given the position of an object, returns the distance from the startline

		void scale_road(float factor);

		CRoadPiece *get_start_position(int n, float *x, float *y, float *a);

		bool over_water(float x, float y, GLTile *water_tile);
		bool over_rock(float x, float y, GLTile *rock_tile);

		void add_tyremark(CTyreMark *tm, int position) {
			tyre_marks[position]->Add(tm);
		};

		int track_sign_to_show2(float x, float y, float position, GLTile **arrow_tiles); // This one just looks for arrow tiles in the map

	private:
		int dx, dy;
		int n_parts_h, n_parts_v;
		int part_h, part_v;

		int background_type; /* 0 : none, 1 : grass, 2 : sand */
		SDL_Surface *background_sfc;
		GLuint background_tex;
		int textures_loaded;

		int load_time;

		List<CRoadPiece> road;
		List<List<CTyreMark> > tyre_marks;

		List<CRotatedGLTile> ***background;

		bool water_tiles_found;
		List<CRotatedGLTile> water_tiles; /* used to accelerate the collision detection with water */

		bool rock_tiles_found;
		List<CRotatedGLTile> rock_tiles; /* used to accelerate the collision detection with rocks */

		float pit_x[2], pit_y[2];
		float pit_angle;

		int track_number;

		bool sign_list_geenrated_p;
		List<float> sign_positions;
		List<int> sign_sign;


};

#endif
