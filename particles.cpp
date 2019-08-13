#ifdef _WIN32
#include "windows.h"
#endif

#include "string.h"
#include "stdio.h"
#include "math.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "list.h"
#include "vector.h"
#include "cmc.h"
#include "3dobject.h"
#include "shadow3dobject.h"
#include "piece3dobject.h"
#include "myglutaux.h"
#include "nether.h"

PARTICLE::PARTICLE(void)
{
	size1=size2=0;
	r=g=b=0;
	a1=a2=0;
	lifetime=acttime=0;
} /* PARTICLE::PARTICLE */ 


PARTICLE::PARTICLE(Vector p,Vector spd1,Vector spd2,float sz1,float sz2,float rp,float gp,float bp,float a1p,float a2p,int lt)
{
	pos=p;
	speed1=spd1;
	speed2=spd2;
	size1=sz1;
	size2=sz2;
	r=rp;
	g=gp;
	b=gp;
	a1=a1p;
	a2=a2p;
	lifetime=lt;
	acttime=0;

} /* PARTICLE::PARTICLE */ 



void NETHER::DrawParticle(PARTICLE *p)
{
	float val,val2;
	float sz;

	val2=float(p->acttime)/float(p->lifetime);
	val=1-val2;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(p->pos.x,p->pos.y,p->pos.z);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	sz=val*p->size1+val2*p->size2;
	glNormal3f(0,0,1);

	glBegin(GL_TRIANGLES);
	glColor4f(p->r,p->g,p->b,val*p->a1+val2*p->a2);
	glVertex3f(0,0,0);
	glColor4f(p->r,p->g,p->b,0);
	glVertex3f(sz,0,0);
	glVertex3f(0,sz,0);

	glColor4f(p->r,p->g,p->b,val*p->a1+val2*p->a2);
	glVertex3f(0,0,0);
	glColor4f(p->r,p->g,p->b,0);
	glVertex3f(0,sz,0);
	glVertex3f(-sz,0,0);

	glColor4f(p->r,p->g,p->b,val*p->a1+val2*p->a2);
	glVertex3f(0,0,0);
	glColor4f(p->r,p->g,p->b,0);
	glVertex3f(-sz,0,0);
	glVertex3f(0,-sz,0);

	glColor4f(p->r,p->g,p->b,val*p->a1+val2*p->a2);
	glVertex3f(0,0,0);
	glColor4f(p->r,p->g,p->b,0);
	glVertex3f(0,-sz,0);
	glVertex3f(sz,0,0);

	glEnd();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);


	glPopMatrix();

} /* NETHER::DrawParticle */ 



bool NETHER::CycleParticle(PARTICLE *p)
{
	float val,val2;

	val2=float(p->acttime)/float(p->lifetime);
	val=1-val2;

	p->pos=p->pos+(p->speed1*val+p->speed2*val2);

	p->acttime++;
	if (p->acttime>=p->lifetime) return false;
	return true;
} /* NETHER::CyclePArticle */ 
