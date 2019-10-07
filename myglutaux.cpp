#ifdef _WIN32
#include "windows.h"
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include "GL/gl.h"
#include "GL/glut.h"
#endif

#include "stdio.h"
#include "string.h"
#include "math.h"
#include "vector.h"
#include "cmc.h"
#include "3dobject.h"

#include "bitmap.h"

#include "myglutaux.h"



C3DObject *characters[256]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//int TEXTURE_APPROXIMATION=GL_NEAREST;
int TEXTURE_APPROXIMATION=GL_LINEAR;


float draw3Dtext(char *text,float dx,float dy,float dz,float r,float g,float b)
{
	int i;
	int len=strlen(text);
	float ancho=(float(len-1)*1.8F);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glScalef(dx,dy,dz);
	glTranslatef(-(ancho/2.0F),0,0);

	for(i=0;i<len;i++) {
		if (characters[text[i]]==0) {
			char filename[6];

			filename[0]=text[i];
			filename[1]='.';
			filename[2]='a';
			filename[3]='s';
			filename[4]='c';
			filename[5]=0;
			characters[text[i]]=new C3DObject(filename,"textures/");
			characters[text[i]]->normalize();
		} /* if */ 
		if (characters[text[i]]!=NULL) {
			characters[text[i]]->draw(r,g,b);
		} /* if */ 
		glTranslatef(1.8F,0,0);
	} /* for */ 

	glPopMatrix();

	return (ancho+1.0F)*dx;
} /* draw3Dtext */ 


int iabs(int v)
{
	if (v>=0) return v;
	return -v;
}


int nearest2pow(int n)
{
	int v[]={2,4,8,16,32,64,128,256,512,-1};
	int better,dif,i;

	better=2;
	dif=iabs(n-v[0]);

	i=0;
	while(v[i]!=-1) {
		if (iabs(n-v[i])<dif) {
			better=v[i];
			dif=iabs(n-v[i]);
		} /* if */ 
		i++;
	} /* while */ 

	return better;
} /* nearest2pow */ 


void glutSolidBox(float dx,float dy,float dz)
{
	glBegin(GL_TRIANGLES);

	/* UP */ 
	glNormal3f(0,1.0,0);
	glVertex3f(-dx,-dy,-dz);
	glVertex3f( dx,-dy, dz);
	glVertex3f( dx,-dy,-dz);

	glVertex3f( dx,-dy, dz);
	glVertex3f(-dx,-dy,-dz);
	glVertex3f(-dx,-dy, dz);

	/* DOWN */ 
	glNormal3f(0,-1.0,0);
	glVertex3f( dx, dy, dz);
	glVertex3f(-dx, dy,-dz);
	glVertex3f( dx, dy,-dz);

	glVertex3f(-dx, dy,-dz);
	glVertex3f( dx, dy, dz);
	glVertex3f(-dx, dy, dz);

	/* LEFT */ 
	glNormal3f(1.0,0,0);
	glVertex3f(-dx,-dy,-dz);
	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx, dy,-dz);

	glVertex3f(-dx, dy, dz);
	glVertex3f(-dx,-dy,-dz);
	glVertex3f(-dx,-dy, dz);

	/* RIGHT */ 
	glNormal3f(-1.0,0,0);
	glVertex3f( dx, dy, dz);
	glVertex3f( dx,-dy,-dz);
	glVertex3f( dx, dy,-dz);

	glVertex3f( dx,-dy,-dz);
	glVertex3f( dx, dy, dz);
	glVertex3f( dx,-dy, dz);

	/* FRONT */ 
	glNormal3f(0.0,0,-1.0);
	glVertex3f(-dx,-dy, dz);
	glVertex3f( dx, dy, dz);
	glVertex3f( dx,-dy, dz);

	glVertex3f( dx, dy, dz);
	glVertex3f(-dx,-dy, dz);
	glVertex3f(-dx, dy, dz);

	/* BACK */ 
	glNormal3f(0.0,0,1.0);
	glVertex3f( dx, dy,-dz);
	glVertex3f(-dx,-dy,-dz);
	glVertex3f( dx,-dy,-dz);

	glVertex3f(-dx,-dy,-dz);
	glVertex3f( dx, dy,-dz);
	glVertex3f(-dx, dy,-dz);

	glEnd();
} /* glutSolidBox */ 


void glutPrint(char *str)
{
	int i;

	for(i=0;str[i]!=0;i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
} /* glutPrint */ 


void glutPrintxy(float x,float y,char *str)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(x,y);
	glutPrint(str);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
} /* glutPrint */ 


void Normal (double vector1[3],double vector2[3],double resultado[3])
{
	double norma;
	resultado[0]=vector1[1]*vector2[2]-vector2[1]*vector1[2];
    resultado[1]=vector1[2]*vector2[0]-vector2[2]*vector1[0];
	resultado[2]=vector1[0]*vector2[1]-vector2[0]*vector1[1];
	norma=sqrt(resultado[0]*resultado[0]+
			   resultado[1]*resultado[1]+
			   resultado[2]*resultado[2]);
	resultado[0]/=norma;
	resultado[1]/=norma;
	resultado[2]/=norma;
} /* Normal */ 


void Normalf (float vector1[3],float vector2[3],float resultado[3])
{
	float norma;
	resultado[0]=vector1[1]*vector2[2]-vector2[1]*vector1[2];
    resultado[1]=vector1[2]*vector2[0]-vector2[2]*vector1[0];
	resultado[2]=vector1[0]*vector2[1]-vector2[0]*vector1[1];
	norma=sqrt(resultado[0]*resultado[0]+
			   resultado[1]*resultado[1]+
			   resultado[2]*resultado[2]);
	resultado[0]/=norma;
	resultado[1]/=norma;
	resultado[2]/=norma;
} /* Normal */ 


float ObtainYAngle(Vector *v)
{
	double x=v->x,z=v->z;
	double h=sqrt(x*x+z*z);
	double cy,sy;

	if (h==0) return 0;

	cy=x/h;
	sy=z/h;
//	printf("%f %f %f\n",cy,sy,h);
	return float((atan2(cy,sy)*180))/3.141592F;
} /* ObtainYAngle */ 


void ApplyMatrix(float v[4],float matrix[16],float out[4])
{
	out[0]=v[0]*matrix[0]+v[1]*matrix[4]+v[2]*matrix[8]+v[3]*matrix[12];
	out[1]=v[0]*matrix[1]+v[1]*matrix[5]+v[2]*matrix[9]+v[3]*matrix[13];
	out[2]=v[0]*matrix[2]+v[1]*matrix[6]+v[2]*matrix[10]+v[3]*matrix[14];
	out[3]=v[0]*matrix[3]+v[1]*matrix[7]+v[2]*matrix[11]+v[3]*matrix[15];

} /* ApplyMatrix */ 


void ApplyMatrix2(float v[4],float matrix[16],float out[4])
{
	out[0]=v[0]*matrix[0]+v[1]*matrix[1]+v[2]*matrix[2]+v[3]*matrix[3];
	out[1]=v[0]*matrix[4]+v[1]*matrix[5]+v[2]*matrix[6]+v[3]*matrix[7];
	out[2]=v[0]*matrix[8]+v[1]*matrix[9]+v[2]*matrix[10]+v[3]*matrix[11];
	out[3]=v[0]*matrix[12]+v[1]*matrix[13]+v[2]*matrix[14]+v[3]*matrix[15];

} /* ApplyMatrix2 */ 


void MulMatrix(float m2[16],float m1[16],float out[16])
{
	out[0]=m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8]+m1[3]*m2[12];
	out[1]=m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9]+m1[3]*m2[13];
	out[2]=m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10]+m1[3]*m2[14];
	out[3]=m1[0]*m2[3]+m1[1]*m2[7]+m1[2]*m2[11]+m1[3]*m2[15];

	out[4]=m1[4]*m2[0]+m1[5]*m2[4]+m1[6]*m2[8]+m1[7]*m2[12];
	out[5]=m1[4]*m2[1]+m1[5]*m2[5]+m1[6]*m2[9]+m1[7]*m2[13];
	out[6]=m1[4]*m2[2]+m1[5]*m2[6]+m1[6]*m2[10]+m1[7]*m2[14];
	out[7]=m1[4]*m2[3]+m1[5]*m2[7]+m1[6]*m2[11]+m1[7]*m2[15];

	out[8]=m1[8]*m2[0]+m1[9]*m2[4]+m1[10]*m2[8]+m1[11]*m2[12];
	out[9]=m1[8]*m2[1]+m1[9]*m2[5]+m1[10]*m2[9]+m1[11]*m2[13];
	out[10]=m1[8]*m2[2]+m1[9]*m2[6]+m1[10]*m2[10]+m1[11]*m2[14];
	out[11]=m1[8]*m2[3]+m1[9]*m2[7]+m1[10]*m2[11]+m1[11]*m2[15];

	out[12]=m1[12]*m2[0]+m1[13]*m2[4]+m1[14]*m2[8]+m1[15]*m2[12];
	out[13]=m1[12]*m2[1]+m1[13]*m2[5]+m1[14]*m2[9]+m1[15]*m2[13];
	out[14]=m1[12]*m2[2]+m1[13]*m2[6]+m1[14]*m2[10]+m1[15]*m2[14];
	out[15]=m1[12]*m2[3]+m1[13]*m2[7]+m1[14]*m2[11]+m1[15]*m2[15];

} /* MulMatrix */ 


bool PlaneLineCollision(float plane[4],float p[3],float v[3],float crossp[3])
{
	float lambda;
	float tmp1,tmp2;

	tmp1=plane[0]*v[0]+plane[1]*v[1]+plane[2]*v[2];
	if (tmp1==0) return false;

	tmp2=plane[0]*p[0]+plane[1]*p[1]+plane[2]*p[2]+plane[3];
	lambda=-tmp2/tmp1;

	crossp[0]=p[0]+v[0]*lambda;
	crossp[1]=p[1]+v[1]*lambda;
	crossp[2]=p[2]+v[2]*lambda;
	return true;
} /* PlaneLineCollision */ 



void DistributeVector(Vector v,Vector v2,Vector *r1,Vector *r2)
{
	double norma;
	double tmp;

	norma=v.normalize();

	tmp=v.x*v2.x+v.y*v2.y+v.z*v2.z;

	r1->x=tmp*v.x;
	r1->y=tmp*v.y;
	r1->z=tmp*v.z;

	r2->x=v2.x-r1->x;
	r2->y=v2.y-r1->y;
	r2->z=v2.z-r1->z;
} /* DistributeVector */ 


double determinante_d(double m[9])
{
	return m[0]*(m[4]*m[8]-m[7]*m[5]) - 
		   m[1]*(m[3]*m[8]-m[5]*m[6]) + 
		   m[2]*(m[3]*m[7]-m[6]*m[4]);
} /* determinante_d */ 


float determinante_f(float m[9])
{
	return m[0]*(m[4]*m[8]-m[7]*m[5]) - 
		   m[1]*(m[3]*m[8]-m[5]*m[6]) + 
		   m[2]*(m[3]*m[7]-m[6]*m[4]);
} /* determinante_f */ 


bool kramer_d(double m[9],double c[3],double sol[3])
{
	double mt[9];
	double d;

	d=determinante_d(m);
	if (d==0) return false;

	mt[0]=c[0];
	mt[1]=m[1];
	mt[2]=m[2];

	mt[3]=c[1];
	mt[4]=m[4];
	mt[5]=m[5];

	mt[6]=c[2];
	mt[7]=m[7];
	mt[8]=m[8];
	
	sol[0]=determinante_d(mt)/d;

	mt[0]=m[0];
	mt[1]=c[0];
	mt[2]=m[2];

	mt[3]=m[3];
	mt[4]=c[1];
	mt[5]=m[5];

	mt[6]=m[6];
	mt[7]=c[2];
	mt[8]=m[8];
	
	sol[1]=determinante_d(mt)/d;

	mt[0]=m[0];
	mt[1]=m[1];
	mt[2]=c[0];

	mt[3]=m[3];
	mt[4]=m[4];
	mt[5]=c[1];

	mt[6]=m[6];
	mt[7]=m[7];
	mt[8]=c[2];
	
	sol[2]=determinante_d(mt)/d;

	return true;
} /* kramer_d */ 


bool kramer_f(float m[9],float c[3],float sol[3])
{
	float mt[9];
	float d;

	d=determinante_f(m);
	if (d==0) return false;

	mt[0]=c[0];
	mt[1]=m[1];
	mt[2]=m[2];

	mt[3]=c[1];
	mt[4]=m[4];
	mt[5]=m[5];

	mt[6]=c[2];
	mt[7]=m[7];
	mt[8]=m[8];
	
	sol[0]=determinante_f(mt)/d;

	mt[0]=m[0];
	mt[1]=c[0];
	mt[2]=m[2];

	mt[3]=m[3];
	mt[4]=c[1];
	mt[5]=m[5];

	mt[6]=m[6];
	mt[7]=c[2];
	mt[8]=m[8];
	
	sol[1]=determinante_f(mt)/d;

	mt[0]=m[0];
	mt[1]=m[1];
	mt[2]=c[0];

	mt[3]=m[3];
	mt[4]=m[4];
	mt[5]=c[1];

	mt[6]=m[6];
	mt[7]=m[7];
	mt[8]=c[2];
	
	sol[2]=determinante_f(mt)/d;

	return true;
} /* kramer_f */ 


bool gauss_solve_d(double *m,double *ctnt,double *s,int neq)
{
	int i,j,k;
	int *order;
	bool found;
	double c,det;
	double tmp;

//	FILE *fp;

//	fp=fopen("out.txt","w");

	det=det_d(m,neq);
	if (det==0) return false;

//	fprintf(fp,"%f\n",float(det));

	order=new int[neq];
	for(i=0;i<neq;i++) order[i]=-1;

	for(i=0;i<neq;i++) {
//		int a,b;
//		fprintf(fp,"%i\n",i);
//		for(a=0;a<neq;a++) {
//			for(b=0;b<neq;b++) {
//				fprintf(fp,"%.6f  ",float(m[a*neq+b]));
//			} /* for */ 
//			fprintf(fp,"|  %.6f\n",float(ctnt[a]));
//		} /* for */ 

		/* Buscar una ecuaci�n con coeficiente de la variable i diferente de 0: */ 
		for(j=0,found=false,tmp=0;j<neq;j++) {
			if (fabs(m[i+j*neq])>tmp) {
				found=true;
				for(k=0;k<i;k++) {
					if (order[k]==j) found=false;
				} /* for */ 
				if (found) {
					order[i]=j;
					tmp=fabs(m[i+j*neq]);
				} /* if */ 
			} /* if */ 
		} /* for */ 

		if (!found) return false;

		/* Colocar un 1: */ 
		c=m[i+order[i]*neq];
		for(j=0;j<neq;j++) {
			m[order[i]*neq+j]/=c;
		} /* for */ 
		ctnt[order[i]]/=c;

		/* Llenar la columna de ceros: */ 
		for(j=0;j<neq;j++) {
			if (j!=order[i]) {
				c=m[i+j*neq];
				
				for(k=0;k<neq;k++) {
					m[j*neq+k]-=c*m[order[i]*neq+k];
				} /* for */ 
				m[j*neq+i]=0;
				ctnt[j]-=c*ctnt[order[i]];
			} /* if */ 
		} /* for */ 
	} /* for */ 

	for(i=0;i<neq;i++) {
		s[i]=ctnt[order[i]]/m[order[i]*neq+i];
	} /* for */ 

	delete order;
	return true;
} /* gauss_solve_d */ 


double det_d(double *m,int size)
{
	double acum=0;
	double *mt;
	int i,j,k;

	if (size==1) return m[0];

	mt=new double[(size-1)*(size-1)];

	for(i=0;i<size;i++) {
		double val;
		val=m[i];
		if (val!=0) {
			if ((i&1)!=0) val=-val;
			
			for(j=0;j<size-1;j++) {
				for(k=0;k<size-1;k++) {
					if (k<i) {
						mt[j*(size-1)+k]=m[(j+1)*size+k];
					} else {
						mt[j*(size-1)+k]=m[(j+1)*size+k+1];
					} /* if */ 
				} /* for */ 
			} /* for */ 

			acum+=val*det_d(mt,size-1);
		} /* if */ 
	} /* for */ 

	delete mt;

	return acum;
} /* det_d */ 


double distancia_a_recta(Vector p,Vector pr,Vector vr)
{
	/* Buscamos el plano que tiene como vector normal 'vr' y pasa por 'p': */ 
	/* x*vr.x + y*vr.y + z*vr.z - (p*vr) = 0 */ 

	/* Buscamos el punto de colisi�n entre la recta y el plano anterior: */ 
	Vector col_point;
	double lambda;
	double tmp1,tmp2;

	tmp1=vr*vr;
	tmp2=vr*pr-(p*vr);
	lambda=-tmp2/tmp1;

	col_point=pr+vr*lambda;

	return (p-col_point).norma();
} /* distancia_a_recta */ 


bool LineLineCollision(float p0[3],float v0[3],float p1[3],float v1[3],float *l1,float *l2)
{
	float m[6];
	float det;

	m[0]=v0[0];		m[1]=-v1[0];	m[2]=p1[0]-p0[0];
	m[3]=v0[1];		m[4]=-v1[1];	m[5]=p1[1]-p0[1];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} /* if */ 

	m[0]=v0[0];		m[1]=-v1[0];	m[2]=p1[0]-p0[0];
	m[3]=v0[2];		m[4]=-v1[2];	m[5]=p1[2]-p0[2];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} /* if */ 

	m[0]=v0[1];		m[1]=-v1[1];	m[2]=p1[1]-p0[1];
	m[3]=v0[2];		m[4]=-v1[2];	m[5]=p1[2]-p0[2];
	det=m[0]*m[4]-m[1]*m[3];
	if (det!=0) {
		*l1=(m[2]*m[4]-m[5]*m[1])/det;
		*l2=(m[0]*m[5]-m[3]*m[2])/det;
		return true;
	} /* if */ 

	return false;
} /* LineLineCollision */ 


unsigned int createTexture(char *fname)
{
	unsigned int tname=0;
	int i,j,k,val;
	int sz;
	GLubyte *textura;
	Bitmap *bmp=NULL;

	bmp=new Bitmap(fname);
 
	if (bmp->valid()) {
		sz=bmp->getdx();
		if (bmp->getdy()<sz) sz=bmp->getdy();
		textura=new GLubyte[sz*sz*4];
		for(j=0,k=0;j<sz;j++) {
			for(i=0;i<sz;i++) {
				val=i+j*bmp->getdx();
				textura[k++]=bmp->getr(val);
				textura[k++]=bmp->getg(val);
				textura[k++]=bmp->getb(val);
				textura[k++]=255;
			} /* for */ 
		} /* for */ 

		glGenTextures(1,&tname);
		glPixelStorei(GL_UNPACK_ALIGNMENT,tname);
		glBindTexture(GL_TEXTURE_2D,tname);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,TEXTURE_APPROXIMATION);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,TEXTURE_APPROXIMATION);
		glTexImage2D(GL_TEXTURE_2D,0,4,sz,sz,0,GL_RGBA,GL_UNSIGNED_BYTE,textura);
		delete textura;
	} else {
		delete bmp;
		return 0;
	} /* if */ 

	return tname;
} /* createTexture */ 


unsigned int createTexture(char *fname,int x,int y,int sz)
{
	unsigned int tname=0;
	int i,j,k,val;
	GLubyte *textura;
	Bitmap *bmp=NULL;

	bmp=new Bitmap(fname);
 
	if (bmp->valid()) {
		textura=new GLubyte[sz*sz*4];
		for(j=0,k=0;j<sz;j++) {
			for(i=0;i<sz;i++) {
				val=(x+i)+(y+j)*bmp->getdx();
				textura[k++]=bmp->getr(val);
				textura[k++]=bmp->getg(val);
				textura[k++]=bmp->getb(val);
				textura[k++]=255;
			} /* for */ 
		} /* for */ 

		glGenTextures(1,&tname);
		glPixelStorei(GL_UNPACK_ALIGNMENT,tname);
		glBindTexture(GL_TEXTURE_2D,tname);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,TEXTURE_APPROXIMATION);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,TEXTURE_APPROXIMATION);
		glTexImage2D(GL_TEXTURE_2D,0,4,sz,sz,0,GL_RGBA,GL_UNSIGNED_BYTE,textura);
		delete textura;
		delete bmp;
	} else {
		delete bmp;
		return 0;
	} /* if */ 

	return tname;
} /* createTexture */ 


