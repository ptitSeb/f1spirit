#ifndef __BRAIN_PLACED_GL_TILE
#define __BRAIN_PLACED_GL_TILE


class CPlacedGLTile
{

		friend class CTrack;

	public:
		CPlacedGLTile(float x, float y, float z, float angle, GLTile *t);
		~CPlacedGLTile();

		void set_tile(GLTile *t) {
			tile = t;
		};

		void set_color(float pr, float pg, float pb, float pa) {
			r = pr;
			g = pg;
			b = pb;
			a = pa;
			different_color = true;
		};

		void set_nocolor(void) {
			different_color = false;
		};

		void draw(void);
		void draw(float dx, float dy, float dz, float angle, float zoom);

		C2DCMC *get_cmc(void) {
			return tile->get_cmc();
		};

		float x, y, z, angle, zoom;
		bool different_color;
		float r, g, b, a;

		GLTile *tile;
};


#endif
