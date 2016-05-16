#ifndef __BRAIN_2DCMC
#define __BRAIN_2DCMC

class C2DCMC
{

	public:
		C2DCMC();
		C2DCMC(float *p, int np);
		C2DCMC(float *x, float *y, int np);

		void reset(void);

		void draw(float r, float g, float b, float a);

		void set
		(float *p, int np);
		void set
		(float *x, float *y, int np);
		// void expand(C2DCMC *o2,float *m);

		float get_radius(void);
		float get_sq_radius(void);

		bool collision(C2DCMC *o2);
		bool collision(float x1, float y1, float a1, C2DCMC *o2, float x2, float y2, float a2);
		bool collision(float x1, float y1, float a1, float x2, float y2);
		bool collision_simple(float x1, float y1, C2DCMC *o2, float x2, float y2);
		bool collision_simple(float x1, float y1, float x2, float y2);
		bool collision_point(float x1, float y1, float a1, C2DCMC *o2, float x2, float y2, float a2, float *cx, float *cy, float precision = 1.0F);
		void point_normal(float px, float py, float x, float y, float a, float *nx, float *ny);

		float x[2], y[2];
		float radius, sq_radius;
		bool empty;

		/* Quick values to compute bounding boxes: */
		bool quick;
		float q_px1[4], q_py1[4];
		float q_a[4], q_b[4], q_c[4], q_v[4];
		float q_x1, q_y1, q_a1;
};

#endif
