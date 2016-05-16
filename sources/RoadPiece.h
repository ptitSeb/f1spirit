#ifndef _F1_SPIRIT_ROADPIECE
#define _F1_SPIRIT_ROADPIECE

class CRoadPiece
{

		friend class F1SpiritGame;

		friend class TrackEditorApp;

		friend class F1SpiritApp;

		friend class CTrack;

		friend class CCar;

	public:
		CRoadPiece();
		CRoadPiece(float x, float y, float z);
		CRoadPiece(FILE *fp);
		~CRoadPiece();

		void save(FILE *fp);

		void draw(void);
		void draw(float orig_x, float orig_y);

		void draw(float offs, int n_textures, GLuint *textures, int n_ltextures, GLuint *ltextures, int n_rtextures, GLuint *rtextures, GLuint line_texture);
		void draw(float orig_x, float orig_y, float offs, int n_textures, GLuint *textures, int n_ltextures, GLuint *ltextures, int n_rtextures, GLuint *rtextures, GLuint line_texture);

		void force_internal_draw(void);

		void get_bbox(float *x, float *y, float *w, float *h);

		bool inside_test(float x, float y);
		bool inside_test(C2DCMC *cmc);

		bool get_path_position(float x, float y, float *px, float *py, float *pz, float *pa, float *pw, float *distance);
		bool distance_to_road(float x, float y, float *distance);
		bool offset_from_road_center(float x, float y, float *offset, float *angle);

		float get_length(void);

		float get_x1(void) {
			return x1;
		};

		float get_y1(void) {
			return y1;
		};

		float get_z1(void) {
			return z1;
		};

		float get_a1(void) {
			return a1;
		};

		float get_w1(void) {
			return w1;
		};

		float get_x2(void) {
			return x2;
		};

		float get_y2(void) {
			return y2;
		};

		float get_z2(void) {
			return z2;
		};

		float get_a2(void) {
			return a2;
		};

		float get_w2(void) {
			return w2;
		};

		int type; /* 0 - invisible */
		/* 1 - normal  */
		int ltype; /* 0 - invisible */
		/* 1 - normal  */
		/* 2 - chicane */
		int rtype; /* 0 - invisible */
		/* 1 - normal  */
		/* 2 - chicane */


	private:
		void internal_draw(void); /* this function draws the RoadPiece to the returned surface */
		/* and returns in 'x' and 'y' the coordinates of the top-left */
		/* corner.              */

		void draw_linear(void);
		void draw_circular_adapted(void);

		float x1, y1, z1, w1;
		float a1;
		float lchicane1, rchicane1;
		int nlines1;

		float x2, y2, z2, w2;
		float a2;
		float lchicane2, rchicane2;
		int nlines2;

		// GLTile *tile;
		int steps;
		float *r_x1, *r_y1;
		float *r_x2, *r_y2;
		float *r_z;
		float *r_cx1, *r_cy1;
		float *r_cx2, *r_cy2;

		float bbox[4];

		/* path: */
		int path_points;
		float *path_x, *path_y, *path_z, *path_a, *path_w;

		bool length_computed;
		float length;
};


#endif
