#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"

#include "GL/gl.h"
#include "vector.h"
#include "cmc.h"
#include "3dobject.h"
#include "shadow3dobject.h"

#include "myglutaux.h"

Shadow3DObject::Shadow3DObject() : C3DObject()
{
	shdw_npuntos=0;
	shdw_ncaras=0;
	shdw_puntos=0;
	shdw_caras=0;
} /* Shadow3DObject::Shadow3DObject */ 


Shadow3DObject::Shadow3DObject(char *file,char *texturedir) : C3DObject(file,texturedir)
{
	shdw_npuntos=0;
	shdw_ncaras=0;
	shdw_puntos=0;
	shdw_caras=0;
} /* Shadow3DObject::Shadow3DObject */ 


Shadow3DObject::~Shadow3DObject()
{
	delete shdw_puntos;
	delete shdw_caras;
	shdw_puntos=0;
	shdw_caras=0;
} /* Shadow3DObject::~Shadow3DObject */ 


void Shadow3DObject::DrawShadow(float r,float g,float b,float a)
{
	int i;

	/* Dibuja el objeto: */ 
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,shdw_puntos);
	glColor4f(r,g,b,a);
	glNormal3f(0,1,0);

	if (a!=1) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	} /* if */ 
	glBegin(GL_TRIANGLES);
	for(i=0;i<shdw_ncaras;i++) {
		glArrayElement(shdw_caras[i*3]);
		glArrayElement(shdw_caras[i*3+1]);
		glArrayElement(shdw_caras[i*3+2]);
	} /* for */ 
	glEnd();

	if (a!=1) glDisable(GL_BLEND);
} /* Shadow3DObject::DraqwShadow */ 


void Shadow3DObject::ComputeShadow(Vector light)
{
	int i;
	int pry_npuntos;
	float *pry_puntos;
	int *pry_caras,pry_ncaras;
	float plane[4];
	float l[3];
	float n[3],v[3],w[3],value;
	float p[3];
	float tmp[3];

	plane[0]=0;
	plane[1]=0;
	plane[2]=1;
	plane[3]=0;

	l[0]=float(light.x);
	l[1]=float(light.y);
	l[2]=float(light.z);

	pry_npuntos=npuntos;
	pry_puntos=new float[npuntos*3];
	/* Proyectar TODOS los triángulos sobre el plano Z: */ 
	for(i=0;i<npuntos;i++) {
		p[0]=puntos[i*3];
		p[1]=puntos[i*3+1];
		p[2]=puntos[i*3+2];

		PlaneLineCollision(plane,p,l,tmp);
		
		pry_puntos[i*3]=tmp[0];
		pry_puntos[i*3+1]=tmp[1];
		pry_puntos[i*3+2]=0;
	} /* for */ 

	/* Crear los triángulos proyectadas: */ 
	shdw_ncaras=0;
	pry_ncaras=ncaras;
	pry_caras=new int[ncaras*3];
	for(i=0;i<ncaras;i++) {
		/* Comprobar que el triángulo es visible: */ 

		v[0]=puntos[caras[i*3+1]*3]-puntos[caras[i*3]*3];;
		v[1]=puntos[caras[i*3+1]*3+1]-puntos[caras[i*3]*3+1];
		v[2]=puntos[caras[i*3+1]*3+2]-puntos[caras[i*3]*3+2];
		w[0]=puntos[caras[i*3+2]*3]-puntos[caras[i*3+1]*3];
		w[1]=puntos[caras[i*3+2]*3+1]-puntos[caras[i*3+1]*3+1];
		w[2]=puntos[caras[i*3+2]*3+2]-puntos[caras[i*3+1]*3+2];
		Normalf(v,w,n);

		value=n[0]*l[0]+n[1]*l[1]+n[2]*l[2];
		if (value>0) {
			pry_caras[shdw_ncaras*3]=caras[i*3];
			pry_caras[shdw_ncaras*3+1]=caras[i*3+1];
			pry_caras[shdw_ncaras*3+2]=caras[i*3+2];
			shdw_ncaras++;
		} /* if */ 
	} /* for */ 


	/* Unir los triángulos proyectados: */ 
	/* ... */ 

	/* Copiarlos a las variables del objeto: */ 
	shdw_caras=new int[shdw_ncaras*3];
	for(i=0;i<shdw_ncaras*3;i++) shdw_caras[i]=pry_caras[i];
	delete pry_caras;
	pry_caras=0;
	shdw_npuntos=pry_npuntos;
	shdw_puntos=pry_puntos;
	shdw_cmc.set(shdw_puntos,shdw_npuntos);
} /* Shadow3DObject::ComputeShadow */ 
