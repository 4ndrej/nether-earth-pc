#ifndef __BRAIN_CMC
#define __BRAIN_CMC

class CMC {
public:
	CMC();
	CMC(float *p,int np);
	CMC(float *x,float *y,float *z,int np);

	void reset(void);

	void draw(float r,float g,float b);
	void drawabsolute(float r,float g,float b);

	void set(float *p,int np);
	void set(float *x,float *y,float *z,int np);
	void expand(CMC *o2,float *m);

	bool collision(CMC *o2);
	bool collision(float *m,CMC *o2,float *m2);

	bool collision_simple(float *m,CMC *o2,float *m2);

	bool load(FILE *fp);
	bool save(FILE *fp);

	float x[2],y[2],z[2];
};

#endif
