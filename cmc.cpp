#ifdef _WIN32
#include "windows.h"
#endif

#include "math.h"
#include "stdio.h"
#include "vector.h"

#include <GL/gl.h>
#include "GL/glu.h"
#include "GL/glut.h"

#include "myglutaux.h"

#include "cmc.h"


bool point_inside_cmc(float *cmc_p,float *p);
bool plane_collision(float *pa,float *va,float *wa,float *pb,float *vb,float *wb);

/* Order of the faces in a CMC: */ 
int faces[24]={0,1,4,5,
			   2,3,6,7,
			   3,1,7,5,
			   2,0,6,4,
			   6,7,4,5,
			   2,3,0,1};



CMC::CMC()
{
	x[0]=x[1]=0;
	y[0]=y[1]=0;
	z[0]=z[1]=0;
} /* CMC::CMC */ 


CMC::CMC(float *p,int np)
{
	int i,offs;

	if (np>0) {
		x[0]=x[1]=p[0];
		y[0]=y[1]=p[1];
		z[0]=z[1]=p[2];
	} /* if */ 

	for(i=1,offs=3;i<np;i++,offs+=3) {
		if (p[offs]<x[0]) x[0]=p[offs];
		if (p[offs]>x[1]) x[1]=p[offs];
		if (p[offs+1]<y[0]) y[0]=p[offs+1];
		if (p[offs+1]>y[1]) y[1]=p[offs+1];
		if (p[offs+2]<z[0]) z[0]=p[offs+2];
		if (p[offs+2]>z[1]) z[1]=p[offs+2];
	} /* for */ 
} /* CMC::CMC */ 


CMC::CMC(float *px,float *py,float *pz,int np)
{
	int i;

	if (np>0) {
		x[0]=x[1]=px[0];
		y[0]=y[1]=px[0];
		z[0]=z[1]=px[0];
	} /* if */ 

	for(i=1;i<np;i++) {
		if (px[i]<x[0]) x[0]=px[i];
		if (px[i]>x[1]) x[1]=px[i];
		if (py[i]<y[0]) y[0]=py[i];
		if (py[i]>y[1]) y[1]=py[i];
		if (pz[i]<z[0]) z[0]=pz[i];
		if (pz[i]>z[1]) z[1]=pz[i];
	} /* for */ 
} /* CMC::CMC */ 



void CMC::reset(void)
{
	x[0]=x[1]=0;
	y[0]=y[1]=0;
	z[0]=z[1]=0;
} /* CMC::set */ 


void CMC::set(float *p,int np)
{
	int i,offs;

	if (np>0) {
		x[0]=x[1]=p[0];
		y[0]=y[1]=p[1];
		z[0]=z[1]=p[2];
	} /* if */ 

	for(i=1,offs=3;i<np;i++,offs+=3) {
		if (p[offs]<x[0]) x[0]=p[offs];
		if (p[offs]>x[1]) x[1]=p[offs];
		if (p[offs+1]<y[0]) y[0]=p[offs+1];
		if (p[offs+1]>y[1]) y[1]=p[offs+1];
		if (p[offs+2]<z[0]) z[0]=p[offs+2];
		if (p[offs+2]>z[1]) z[1]=p[offs+2];
	} /* for */ 
} /* CMC::set */ 


void CMC::set(float *px,float *py,float *pz,int np)
{
	int i;

	if (np>0) {
		x[0]=x[1]=px[0];
		y[0]=y[1]=px[0];
		z[0]=z[1]=px[0];
	} /* if */ 

	for(i=1;i<np;i++) {
		if (px[i]<x[0]) x[0]=px[i];
		if (px[i]>x[1]) x[1]=px[i];
		if (py[i]<y[0]) y[0]=py[i];
		if (py[i]>y[1]) y[1]=py[i];
		if (pz[i]<z[0]) z[0]=pz[i];
		if (pz[i]>z[1]) z[1]=pz[i];
	} /* for */ 
} /* CMC::set */ 


void CMC::expand(CMC *o2,float *m)
{
	float v[4],out[4];
	int i;

	for(i=0;i<8;i++) {
		v[0]=((i&1)==0 ? o2->x[0]:o2->x[1]);
		v[1]=((i&2)==0 ? o2->y[0]:o2->y[1]);
		v[2]=((i&4)==0 ? o2->z[0]:o2->z[1]);
		v[3]=1;
		ApplyMatrix(v,m,out);
		if (out[0]<x[0]) x[0]=out[0];
		if (out[0]>x[1]) x[1]=out[0];
		if (out[1]<y[0]) y[0]=out[1];
		if (out[1]>y[1]) y[1]=out[1];
		if (out[2]<z[0]) z[0]=out[2];
		if (out[2]>z[1]) z[1]=out[2];
	} /* for */ 
} /* expand */ 


void CMC::drawabsolute(float r,float g,float b)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	gluLookAt(0,-30,-20,0,0,-2,0,-1,0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	draw(r,g,b);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
} /* CMC::drawabsolute */ 


void CMC::draw(float r,float g,float b)
{
	float c[4];

	glGetFloatv(GL_CURRENT_COLOR,c);
	glColor3f(r,g,b);
	glBegin(GL_LINES);
	glVertex3f(x[0],y[0],z[0]);
	glVertex3f(x[1],y[0],z[0]);

	glVertex3f(x[1],y[0],z[0]);
	glVertex3f(x[1],y[1],z[0]);

	glVertex3f(x[1],y[1],z[0]);
	glVertex3f(x[0],y[1],z[0]);

	glVertex3f(x[0],y[1],z[0]);
	glVertex3f(x[0],y[0],z[0]);

	glVertex3f(x[0],y[0],z[1]);
	glVertex3f(x[1],y[0],z[1]);

	glVertex3f(x[1],y[0],z[1]);
	glVertex3f(x[1],y[1],z[1]);

	glVertex3f(x[1],y[1],z[1]);
	glVertex3f(x[0],y[1],z[1]);

	glVertex3f(x[0],y[1],z[1]);
	glVertex3f(x[0],y[0],z[1]);

	glVertex3f(x[0],y[0],z[0]);
	glVertex3f(x[0],y[0],z[1]);

	glVertex3f(x[1],y[0],z[0]);
	glVertex3f(x[1],y[0],z[1]);

	glVertex3f(x[0],y[1],z[0]);
	glVertex3f(x[0],y[1],z[1]);

	glVertex3f(x[1],y[1],z[0]);
	glVertex3f(x[1],y[1],z[1]);
	glEnd();
	glColor4f(c[0],c[1],c[2],c[3]);
} /* CMC::draw */ 



bool CMC::collision(CMC *o2)
{
	if (o2->x[0]==o2->x[1] || x[0]==x[1]) {
		if (o2->x[0]>x[1] || o2->x[1]<x[0]) return false;
	} else {
		if (o2->x[0]>=x[1] || o2->x[1]<=x[0]) return false;
	} /* if */ 
	if (o2->y[0]==o2->y[1] || y[0]==y[1]) {
		if (o2->y[0]>y[1] || o2->y[1]<y[0]) return false;
	} else {
		if (o2->y[0]>=y[1] || o2->y[1]<=y[0]) return false;
	} /* if */ 
	if (o2->z[0]==o2->z[1] || z[0]==z[1]) {
		if (o2->z[0]>z[1] || o2->z[1]<z[0]) return false;
	} else {
		if (o2->z[0]>=z[1] || o2->z[1]<=z[0]) return false;
	} /* if */ 

	return true;
} /* collision */ 



bool CMC::collision(float *m,CMC *o2,float *m2)
{
	float a_points[24];
	float b_points[24];
	CMC *a_cmc,*b_cmc;

	/* Calcular las coordenadas "mundo" de los vértices de las cmcs: */ 
	{
		float v[4],out[4];
		int i;
		for(i=0;i<8;i++) {
			v[0]=((i&1)==0 ? x[0]:x[1]);
			v[1]=((i&2)==0 ? y[0]:y[1]);
			v[2]=((i&4)==0 ? z[0]:z[1]);
			v[3]=1;
			ApplyMatrix(v,m,out);
			a_points[i*3]=out[0];
			a_points[i*3+1]=out[1];
			a_points[i*3+2]=out[2];

			v[0]=((i&1)==0 ? o2->x[0]:o2->x[1]);
			v[1]=((i&2)==0 ? o2->y[0]:o2->y[1]);
			v[2]=((i&4)==0 ? o2->z[0]:o2->z[1]);
			v[3]=1;
			ApplyMatrix(v,m2,out);
			b_points[i*3]=out[0];
			b_points[i*3+1]=out[1];
			b_points[i*3+2]=out[2];
		} /* for */ 
	}

	/* Hacer una primera precolisión: */ 
	a_cmc=new CMC(a_points,8);
	b_cmc=new CMC(b_points,8);
	if (!a_cmc->collision(b_cmc)) return false;

//	a_cmc->drawabsolute(1,1,1);
//	b_cmc->drawabsolute(1,1,1);

	/* Colisión detallada: */ 
	/* PARTE 1: Colisión de caras */ 
	{
		int i,j,k;
		float plane_a_points[12];
		float plane_b_points[12];
		CMC *plane_a_cmc,*plane_b_cmc;

		float pa[4],va[4],wa[4];
		float pb[4],vb[4],wb[4];

		for(i=0;i<6;i++) {
			for(k=0;k<4;k++) {
				plane_a_points[k*3]=a_points[faces[i*4+k]*3];
				plane_a_points[k*3+1]=a_points[faces[i*4+k]*3+1];
				plane_a_points[k*3+2]=a_points[faces[i*4+k]*3+2];
			} /* for */ 
			plane_a_cmc=new CMC(plane_a_points,4);

			if (plane_a_cmc->collision(b_cmc)) {
				pa[0]=a_points[faces[i*4]*3];
				pa[1]=a_points[faces[i*4]*3+1];
				pa[2]=a_points[faces[i*4]*3+2];
				pa[3]=1;

				va[0]=a_points[faces[i*4+1]*3]-a_points[faces[i*4]*3];
				va[1]=a_points[faces[i*4+1]*3+1]-a_points[faces[i*4]*3+1];
				va[2]=a_points[faces[i*4+1]*3+2]-a_points[faces[i*4]*3+2];
				va[3]=1;

				wa[0]=a_points[faces[i*4+2]*3]-a_points[faces[i*4]*3];
				wa[1]=a_points[faces[i*4+2]*3+1]-a_points[faces[i*4]*3+1];
				wa[2]=a_points[faces[i*4+2]*3+2]-a_points[faces[i*4]*3+2];
				wa[3]=1;

				for(j=0;j<6;j++) {
					for(k=0;k<4;k++) {
						plane_b_points[k*3]=b_points[faces[j*4+k]*3];
						plane_b_points[k*3+1]=b_points[faces[j*4+k]*3+1];
						plane_b_points[k*3+2]=b_points[faces[j*4+k]*3+2];
					} /* for */ 
					plane_b_cmc=new CMC(plane_b_points,4);

					if (plane_b_cmc->collision(plane_a_cmc)) {
						pb[0]=b_points[faces[j*4]*3];
						pb[1]=b_points[faces[j*4]*3+1];
						pb[2]=b_points[faces[j*4]*3+2];
						pb[3]=1;

						vb[0]=b_points[faces[j*4+1]*3]-b_points[faces[j*4]*3];
						vb[1]=b_points[faces[j*4+1]*3+1]-b_points[faces[j*4]*3+1];
						vb[2]=b_points[faces[j*4+1]*3+2]-b_points[faces[j*4]*3+2];
						vb[3]=1;

						wb[0]=b_points[faces[j*4+2]*3]-b_points[faces[j*4]*3];
						wb[1]=b_points[faces[j*4+2]*3+1]-b_points[faces[j*4]*3+1];
						wb[2]=b_points[faces[j*4+2]*3+2]-b_points[faces[j*4]*3+2];
						wb[3]=1;

						/* Colisión entre planos: */ 
						if (plane_collision(pa,va,wa,pb,vb,wb)) {
							delete plane_b_cmc;
							delete plane_a_cmc;
							delete a_cmc;
							delete b_cmc;
							return true;
						} /* if */ 
					} /* if */ 

					delete plane_b_cmc;
				} /* for */ 

				delete plane_a_cmc;
			} /* if */ 
		} /* for */ 
	}

	/* PARTE 2: Test de inclusión */ 
	{
		/* A in B: */ 
		if (point_inside_cmc(a_points,b_points)) return true;

		/* B in A: */ 
		if (point_inside_cmc(b_points,a_points)) return true;
	}

	delete a_cmc;
	delete b_cmc;

	return false;
} /* collision */ 


bool CMC::collision_simple(float *m,CMC *o2,float *m2)
{
	float a_points[24];
	float b_points[24];
	CMC *a_cmc,*b_cmc;

	/* Calcular las coordenadas "mundo" de los vértices de las cmcs: */ 
	{
		float v[4],out[4];
		int i;
		for(i=0;i<8;i++) {
			v[0]=((i&1)==0 ? x[0]:x[1]);
			v[1]=((i&2)==0 ? y[0]:y[1]);
			v[2]=((i&4)==0 ? z[0]:z[1]);
			v[3]=1;
			ApplyMatrix(v,m,out);
			a_points[i*3]=out[0];
			a_points[i*3+1]=out[1];
			a_points[i*3+2]=out[2];

			v[0]=((i&1)==0 ? o2->x[0]:o2->x[1]);
			v[1]=((i&2)==0 ? o2->y[0]:o2->y[1]);
			v[2]=((i&4)==0 ? o2->z[0]:o2->z[1]);
			v[3]=1;
			ApplyMatrix(v,m2,out);
			b_points[i*3]=out[0];
			b_points[i*3+1]=out[1];
			b_points[i*3+2]=out[2];
		} /* for */ 
	}

	/* Hacer una primera precolisión: */ 
	a_cmc=new CMC(a_points,8);
	b_cmc=new CMC(b_points,8);
	if (!a_cmc->collision(b_cmc)) return false;

	return true;
} /* collision_simple */ 



bool point_inside_cmc(float *cmc_p,float *p)
{
	int faces[24]={0,1,4,5,
				   2,3,6,7,
				   3,1,7,5,
				   2,0,6,4,
				   6,7,4,5,
				   2,3,0,1};
	int i;

	float pa[4],pb[4],v[4],w[4];
	float paeq[4],pbeq[4];

	for(i=0;i<6;i+=2) {
		pa[0]=cmc_p[faces[i*4]*3];
		pa[1]=cmc_p[faces[i*4]*3+1];
		pa[2]=cmc_p[faces[i*4]*3+2];
		pa[3]=1;

		pb[0]=cmc_p[faces[(i+1)*4]*3];
		pb[1]=cmc_p[faces[(i+1)*4]*3+1];
		pb[2]=cmc_p[faces[(i+1)*4]*3+2];
		pb[3]=1;

		v[0]=cmc_p[faces[i*4+1]*3]-cmc_p[faces[i*4]*3];
		v[1]=cmc_p[faces[i*4+1]*3+1]-cmc_p[faces[i*4]*3+1];
		v[2]=cmc_p[faces[i*4+1]*3+2]-cmc_p[faces[i*4]*3+2];
		v[3]=1;

		w[0]=cmc_p[faces[i*4+2]*3]-cmc_p[faces[i*4]*3];
		w[1]=cmc_p[faces[i*4+2]*3+1]-cmc_p[faces[i*4]*3+1];
		w[2]=cmc_p[faces[i*4+2]*3+2]-cmc_p[faces[i*4]*3+2];
		w[3]=1;

		/* Calcular las ecuaciónes de los planos: */ 
		/* Coeficientes A,B,C: */ 
		paeq[0]=pbeq[0]=v[1]*w[2]-w[1]*v[2];
		paeq[1]=pbeq[1]=v[2]*w[0]-w[2]*v[0];
		paeq[2]=pbeq[2]=v[0]*w[1]-w[0]*v[1];
		/* Termino independiente: */ 
		paeq[3]=-(paeq[0]*pa[0]+paeq[1]*pa[1]+paeq[2]*pa[2]);
		pbeq[3]=-(pbeq[0]*pb[0]+pbeq[1]*pb[1]+pbeq[2]*pb[2]);

		if (((paeq[0]*p[0]+paeq[1]*p[1]+paeq[2]*p[2]+paeq[3])*
			 (pbeq[0]*p[0]+pbeq[1]*p[1]+pbeq[2]*p[2]+pbeq[3]))>0) return false;
	} /* for */ 

	return true;
} /* point_inside_cmc */ 


bool plane_collision(float *pa,float *va,float *wa,float *pb,float *vb,float *wb)
{
	int i;
	float m[9],mat[9];
	float det,t_det;
	int lambda;
	float range_a[2],range_b[2];

	/* Determinar cual de las 4 lambdas será el parámetro l[par]: */ 
	/* lambda = l4 */ 
	m[0]=va[0];	m[1]=wa[0];	m[2]=-vb[0];
	m[3]=va[1];	m[4]=wa[1];	m[5]=-vb[1];
	m[6]=va[2];	m[7]=wa[2];	m[8]=-vb[2];
	det=determinante_f(m);
	lambda=4;
	for(i=0;i<9;i++) mat[i]=m[i];

	/* lambda = l3 */ 
	m[0]=va[0];	m[1]=wa[0];	m[2]=-wb[0];
	m[3]=va[1];	m[4]=wa[1];	m[5]=-wb[1];
	m[6]=va[2];	m[7]=wa[2];	m[8]=-wb[2];
	t_det=determinante_f(m);
	if (fabs(t_det)>det) {
		for(i=0;i<9;i++) mat[i]=m[i];
		det=t_det;
		lambda=3;
	} /* if */ 

	/* lambda = l2 */ 
	m[0]=va[0];	m[1]=-vb[0];	m[2]=-wb[0];
	m[3]=va[1];	m[4]=-vb[1];	m[5]=-wb[1];
	m[6]=va[2];	m[7]=-vb[2];	m[8]=-wb[2];
	t_det=determinante_f(m);
	if (fabs(t_det)>det) {
		for(i=0;i<9;i++) mat[i]=m[i];
		det=t_det;
		lambda=2;
	} /* if */ 

	/* lambda = l1 */ 
	m[0]=wa[0];	m[1]=-vb[0];	m[2]=-wb[0];
	m[3]=wa[1];	m[4]=-vb[1];	m[5]=-wb[1];
	m[6]=wa[2];	m[7]=-vb[2];	m[8]=-wb[2];
	t_det=determinante_f(m);
	if (fabs(t_det)>det) {
		for(i=0;i<9;i++) mat[i]=m[i];
		det=t_det;
		lambda=1;
	} /* if */ 

	if (det==0) {
		/* Los planos están alineados: */ 

		/* Substituir todos los puntos de un plano en el otro y viceversa, y ver si alguno pertenece: */ 
		/* ... */ 

		return false;
	} /* if */ 

	/* Buscar la ecuación de la recta intersección de los 2 planos: */ 
	float ct[3];
	float lambdas[3];
	float r_p[3],r_p2[3],r_v[3];

	ct[0]=pb[0]-pa[0];
	ct[1]=pb[1]-pa[1];
	ct[2]=pb[2]-pa[2];
	kramer_f(mat,ct,lambdas);
	switch(lambda){
	case 1:r_p[0]=pa[0]+lambdas[0]*wa[0];
		   r_p[1]=pa[1]+lambdas[0]*wa[1];
		   r_p[2]=pa[2]+lambdas[0]*wa[2];
		   break;
	case 2:r_p[0]=pa[0]+lambdas[0]*va[0];
		   r_p[1]=pa[1]+lambdas[0]*va[1];
		   r_p[2]=pa[2]+lambdas[0]*va[2];
		   break;
	case 3:
	case 4:r_p[0]=pa[0]+lambdas[0]*va[0]+lambdas[1]*wa[0];
		   r_p[1]=pa[1]+lambdas[0]*va[1]+lambdas[1]*wa[1];
		   r_p[2]=pa[2]+lambdas[0]*va[2]+lambdas[1]*wa[2];
		   break;
	} /* switch */ 
	
	switch(lambda) {
	case 1:ct[0]=(pb[0]-pa[0])-va[0];
		   ct[1]=(pb[1]-pa[1])-va[1];
		   ct[2]=(pb[2]-pa[2])-va[2];
		   break;
	case 2:ct[0]=(pb[0]-pa[0])-wa[0];
		   ct[1]=(pb[1]-pa[1])-wa[1];
		   ct[2]=(pb[2]-pa[2])-wa[2];
		   break;
	case 3:ct[0]=(pb[0]-pa[0])+vb[0];
		   ct[1]=(pb[1]-pa[1])+vb[1];
		   ct[2]=(pb[2]-pa[2])+vb[2];
		   break;
	case 4:ct[0]=(pb[0]-pa[0])+wb[0];
		   ct[1]=(pb[1]-pa[1])+wb[1];
		   ct[2]=(pb[2]-pa[2])+wb[2];
		   break;
	} /* switch */ 
	kramer_f(mat,ct,lambdas);
	switch(lambda){
	case 1:r_p2[0]=pa[0]+va[0]+lambdas[0]*wa[0];
		   r_p2[1]=pa[1]+va[1]+lambdas[0]*wa[1];
		   r_p2[2]=pa[2]+va[2]+lambdas[0]*wa[2];
		   break;
	case 2:r_p2[0]=pa[0]+lambdas[0]*va[0]+wa[0];
		   r_p2[1]=pa[1]+lambdas[0]*va[1]+wa[1];
		   r_p2[2]=pa[2]+lambdas[0]*va[2]+wa[2];
		   break;
	case 3:
	case 4:r_p2[0]=pa[0]+lambdas[0]*va[0]+lambdas[1]*wa[0];
		   r_p2[1]=pa[1]+lambdas[0]*va[1]+lambdas[1]*wa[1];
		   r_p2[2]=pa[2]+lambdas[0]*va[2]+lambdas[1]*wa[2];
		   break;
	} /* switch */ 

	r_v[0]=r_p2[0]-r_p[0];
	r_v[1]=r_p2[1]-r_p[1];
	r_v[2]=r_p2[2]-r_p[2];

	/* Buscar el rango de l[par] que cae dentro de a: */ 
	{
		float tmp;
		float cutpoint;
		float tmp_p[3];
		bool first;

		range_a[0]=range_a[0]=0;
		first=true;

		if (LineLineCollision(r_p,r_v,pa,va,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				range_a[0]=tmp;
				range_a[1]=tmp;
				first=false;
			} /* if */ 
		} /* if */ 

		if (LineLineCollision(r_p,r_v,pa,wa,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_a[0]=tmp;
					range_a[1]=tmp;
					first=false;
				} else {
					if (range_a[0]>tmp) range_a[0]=tmp;
					if (range_a[1]<tmp) range_a[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		tmp_p[0]=pa[0]+wa[0];
		tmp_p[1]=pa[1]+wa[1];
		tmp_p[2]=pa[2]+wa[2];
		if (LineLineCollision(r_p,r_v,tmp_p,va,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_a[0]=tmp;
					range_a[1]=tmp;
					first=false;
				} else {
					if (range_a[0]>tmp) range_a[0]=tmp;
					if (range_a[1]<tmp) range_a[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		tmp_p[0]=pa[0]+va[0];
		tmp_p[1]=pa[1]+va[1];
		tmp_p[2]=pa[2]+va[2];
		if (LineLineCollision(r_p,r_v,tmp_p,wa,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_a[0]=tmp;
					range_a[1]=tmp;
					first=false;
				} else {
					if (range_a[0]>tmp) range_a[0]=tmp;
					if (range_a[1]<tmp) range_a[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 

	}
	if (range_a[0]>=range_a[1]) return false;

	/* Buscar el rango de l[par] que cae dentro de b: */ 
	{
		float tmp;
		float cutpoint;
		float tmp_p[3];
		bool first;

		range_b[0]=range_b[0]=0;
		first=true;

		if (LineLineCollision(r_p,r_v,pb,vb,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				range_b[0]=tmp;
				range_b[1]=tmp;
				first=false;
			} /* if */ 
		} /* if */ 

		if (LineLineCollision(r_p,r_v,pb,wb,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_b[0]=tmp;
					range_b[1]=tmp;
					first=false;
				} else {
					if (range_b[0]>tmp) range_b[0]=tmp;
					if (range_b[1]<tmp) range_b[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		tmp_p[0]=pb[0]+wb[0];
		tmp_p[1]=pb[1]+wb[1];
		tmp_p[2]=pb[2]+wb[2];
		if (LineLineCollision(r_p,r_v,tmp_p,vb,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_b[0]=tmp;
					range_b[1]=tmp;
					first=false;
				} else {
					if (range_b[0]>tmp) range_b[0]=tmp;
					if (range_b[1]<tmp) range_b[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		tmp_p[0]=pb[0]+vb[0];
		tmp_p[1]=pb[1]+vb[1];
		tmp_p[2]=pb[2]+vb[2];
		if (LineLineCollision(r_p,r_v,tmp_p,wb,&tmp,&cutpoint)) {
			if (cutpoint>=0 && cutpoint<=1) {
				if (first) {
					range_b[0]=tmp;
					range_b[1]=tmp;
					first=false;
				} else {
					if (range_b[0]>tmp) range_b[0]=tmp;
					if (range_b[1]<tmp) range_b[1]=tmp;
				} /* if */ 
			} /* if */ 
		} /* if */ 	}
	if (range_b[0]>=range_b[1]) return false;

	/* Si se solapan los rangos hay colisión: */ 
	if (range_a[0]<range_b[1] &&
		range_b[0]<range_a[1]) return true;
	
	return false;
} /* plane_collision */ 


bool CMC::load(FILE *fp)
{
	if (6!=fscanf(fp,"%f %f %f %f %f %f",&(x[0]),&(x[1]),&(y[0]),&(y[1]),&(z[0]),&(z[1]))) return false;

	return true;
} /* CMC::load */ 


bool CMC::save(FILE *fp)
{
	fprintf(fp,"%.8f %.8f\n%.8f %.8f\n%.8f %.8f\n",x[0],x[1],y[0],y[1],z[0],z[1]);

	return true;
} /* CMC::save */ 
