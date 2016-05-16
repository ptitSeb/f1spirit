#ifndef __BRAIN_GL_TILE
#define __BRAIN_GL_TILE

/* OpenGL HW accelerated 2D tiles */

extern int reload_textures;

class GLTile
{

	public:
		GLTile();
		GLTile(char *fname);
		GLTile(SDL_Surface *sfc);
		GLTile(char *fname, int x, int y, int dx, int dy);
		GLTile(SDL_Surface *sfc, int x, int y, int dx, int dy);
		GLTile(SDL_Surface **sfcs, int *start_x, int *start_y, int nsfcs, int dx, int dy);
		GLTile(int x, int y, int dx, int dy);

		~GLTile();

		void set_repeat(void);
		void set
		(char *fname);
		void set
		(SDL_Surface *sfc);

		void set_smooth(void);
		void set_smooth(char *fname);
		void set_smooth(SDL_Surface *sfc);

		void set_clamp(void);
		void set_clamp(char *fname);
		void set_clamp(SDL_Surface *sfc);

		void set_hotspot(int hx, int hy);

		void draw(void);
		void draw(float x, float y, float z, float angle, float scale);

		void draw(float r, float g, float b, float a);
		void draw(float r, float g, float b, float a, float x, float y, float z, float angle, float scale);

		void draw_cmc(void);
		void draw_cmc(float dx, float dy, float dz, float angle, float zoom);
		void draw_cmc(float r, float g, float b, float a);
		void draw_cmc(float r, float g, float b, float a, float dx, float dy, float dz, float angle, float zoom);

		int get_dx(void) {
			return g_dx;
		};

		int get_dy(void) {
			return g_dy;
		};

		int get_hot_x(void) {
			return hot_x;
		};

		int get_hot_y(void) {
			return hot_y;
		};

		Uint32 get_pixel(int x, int y);

		class C2DCMC *get_cmc(void)
		{
				return cmc;
		};

		void compute_cmc(void);

		void optimize(int block_dx, int block_dy);
		void load_textures(void);
		void free(void);

		SDL_Surface *get_tile(int i) {
			return tile[i];
		};

		static int get_memory_used(void) {
			return memory_used;
		};

		GLuint get_texture(int i) {
			if (textures_loaded != reload_textures)
				load_textures();

			return tex[i];
		};

		//  f1 spirit specific:

		void f1_draw_effect1(int x, int y, int z, int h_tiles, int v_tiles, int timmer1, int timmer2, int time, float r, float g, float b);

	private:
		static int memory_used;

		int textures_loaded;

		int nparts;
		bool smooth;
		bool clamp;

		int g_dx, g_dy;
		int hot_x, hot_y;

		SDL_Surface **tile;
		int *x, *y;
		int *dx, *dy;
		float *tex_coord_x, *tex_coord_y;
		GLuint *tex;

		class C2DCMC *cmc;
};





#endif
