#ifndef __BRAIN_QUATERNION
#define __BRAIN_QUATERNION

class Quaternion {
public:
	Quaternion();
	Quaternion(FILE *fp);

	bool operator==(Quaternion &q);
	bool operator!=(Quaternion &q);

	bool load(FILE *fp);
	bool save(FILE *fp);

	void to_matrix(float *m);

	void to_axis_angle(float *axis,float *angle);
	void from_axis_angle(float *axis,float angle);

	void to_axis_angle(Vector *axis,double *angle);
	void from_axis_angle(Vector axis,double angle);

	void post_multiply(Quaternion *q);
	void pre_multiply(Quaternion *q);

	void rotate_vector(float *in,float *out);
	void rotate_vector(Vector in,Vector &out);

	void normalize(void);

	void print(void);

private:
	double w,x,y,z;
};

#endif
