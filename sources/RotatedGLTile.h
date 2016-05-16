#ifndef __BRAIN_ROTATED_GL_TILE
#define __BRAIN_ROTATED_GL_TILE


class CRotatedGLTile
{

		friend class CTrack;

	public:
		CRotatedGLTile(float x, float y, float z, float angle1, float angle2, GLTile *t);
		~CRotatedGLTile();

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

		float x, y, z, angle1, angle2;
		bool different_color;
		float r, g, b, a;

		GLTile *tile;
};


#endif
