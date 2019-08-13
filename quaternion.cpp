#include "math.h"
#include "stdio.h"
#include "vector.h"

#include "myglutaux.h"

#include "quaternion.h"


Quaternion::Quaternion() 
{
	w=1;
	x=y=z=0;
} /* Quaternion::Quaternion */ 


Quaternion::Quaternion(FILE *fp) 
{
	load(fp);
	normalize();
} /* Quaternion::Quaternion */ 


bool Quaternion::load(FILE *fp) 
{
	float t1,t2,t3,t4;

	if (4!=fscanf(fp,"%f %f %f %f",&t1,&t2,&t3,&t4)) return false;
	w=t1;
	x=t2;
	y=t3;
	z=t4;


	return true;
} /* Quaternion::load */ 


bool Quaternion::save(FILE *fp) 
{
	fprintf(fp,"%f %f %f %f\n",float(w),float(x),float(y),float(z));

	return true;
} /* Quaternion::save */ 


void Quaternion::to_matrix(float *m) 
{
	m[0]=float(1-2*y*y-2*z*z);
	m[1]=float(2*x*y+2*w*z);
	m[2]=float(2*x*z-2*w*y);
	m[3]=0;

	m[4]=float(2*x*y-2*w*z);
	m[5]=float(1-2*x*x-2*z*z);
	m[6]=float(2*y*z+2*w*x);
	m[7]=0;

	m[8]=float(2*x*z+2*w*y);
	m[9]=float(2*y*z-2*w*x);
	m[10]=float(1-2*x*x-2*y*y);
	m[11]=0;

	m[12]=0;
	m[13]=0;
	m[14]=0;
	m[15]=1;
} /* Quaternion::to_matrix */ 


void Quaternion::to_axis_angle(float *axis,float *angle)
{
	double scale;

	normalize();

	*angle=float(2*acos(w));
	scale=x*x+y*y+z*z;
	if (scale==0) {
		axis[0]=0;
		axis[1]=0;
		axis[2]=1;
	} else {
		axis[0]=float(x/scale);
		axis[1]=float(y/scale);
		axis[2]=float(z/scale);
	} /* if */ 
} /* Quaternion::to_axis_angle */ 


void Quaternion::from_axis_angle(float *axis,float angle)
{	
	double sa;

	sa=sin(angle/2);

	w=cos(angle/2);
	x=axis[0]*sa;
	y=axis[1]*sa;
	z=axis[2]*sa;
	normalize();
} /* from_axis_angle */ 


void Quaternion::to_axis_angle(Vector *axis,double *angle)
{
	double scale;

	normalize();

	*angle=float(2*acos(w));
	scale=x*x+y*y+z*z;
	if (scale==0) {
		axis->x=0;
		axis->y=0;
		axis->z=1;
	} else {
		axis->x=x/scale;
		axis->y=y/scale;
		axis->z=z/scale;
	} /* if */ 
} /* Quaternion::to_axis_angle */ 


void Quaternion::from_axis_angle(Vector axis,double angle)
{	
	double sa;

	sa=sin(angle/2);

	w=cos(angle/2);
	x=axis.x*sa;
	y=axis.y*sa;
	z=axis.z*sa;
	normalize();
} /* from_axis_angle */ 


void Quaternion::post_multiply(Quaternion *q)
{
	double ww,xx,yy,zz;

	ww=w*q->w - x*q->x - y*q->y - z*q->z;
	xx=w*q->x + x*q->w + y*q->z - z*q->y;
	yy=w*q->y + y*q->w + z*q->x - x*q->z;
	zz=w*q->z + z*q->w + x*q->y - y*q->x;

	w=ww;
	x=xx;
	y=yy;
	z=zz;
	normalize();
} /* Quaternion::multiply */ 


void Quaternion::pre_multiply(Quaternion *q)
{
	double ww,xx,yy,zz;

	ww=q->w*w - q->x*x - q->y*y - q->z*z;
	xx=q->w*x + q->x*w + q->y*z - q->z*y;
	yy=q->w*y + q->y*w + q->z*x - q->x*z;
	zz=q->w*z + q->z*w + q->x*y - q->y*x;

	w=ww;
	x=xx;
	y=yy;
	z=zz;
	normalize();
} /* Quaternion::multiply */ 


void Quaternion::rotate_vector(float *in,float *out) 
{
	float matrix[16];

	normalize();
	to_matrix(matrix);
	ApplyMatrix(in,matrix,out);
} /* Quaternion::rotate_vector */ 



void Quaternion::rotate_vector(Vector in,Vector &out) 
{
	float inv[4]={float(in.x),float(in.y),float(in.z),0},outv[4]={float(out.x),float(out.y),float(out.z),0};
	float matrix[16];

	normalize();
	to_matrix(matrix);
	ApplyMatrix(inv,matrix,outv);
	out.x=outv[0];
	out.y=outv[1];
	out.z=outv[2];
} /* Quaternion::rotate_vector */ 


void Quaternion::normalize(void) 
{
	double norm;

	norm=sqrt(w*w+x*x+y*y+z*z);
	if (norm!=0) {
			w/=norm;
			x/=norm;
			y/=norm;
			z/=norm;
		} else {
			w=1;
			x=y=z=0;
		} /* if */ 
} /* Quaternion::normalize */ 

void Quaternion::print(void) 
{
	printf("[%g,(%g,%g,%g)]",w,x,y,z);
} /* Quaternion::print */ 


bool Quaternion::operator==(Quaternion &q)
{
	if (w!=q.w ||
		x!=q.x ||
		y!=q.y ||
		z!=q.z) return false;

	return true;
} /* Quaternion::operator== */ 


bool Quaternion::operator!=(Quaternion &q)
{
	if (w!=q.w ||
		x!=q.x ||
		y!=q.y ||
		z!=q.z) return true;

	return false;
} /* Quaternion::operator!= */ 
