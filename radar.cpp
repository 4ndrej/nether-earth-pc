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


#include "glprintf.h"

void NETHER::draw_radar(void)
{
    /* Clear the color and depth buffers. */ 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glTranslatef(64.0,8.0,0.0);
	glColor3f(0.5f,0.5f,1.0f);
	scaledglprintf(0.18f,0.4f,"RADAR:");

	glLoadIdentity();
	glScalef(4.0,4.0,4.0);

	/* It will draw the RADAR map, directly taken from the IA variables: */ 
	{
		int x,y;
		int starty,startx;
		int maxy=94,maxx=16;

		startx=(int)((shipp.x-4)*2);
		starty=(int)((shipp.y-23)*2);
		if ((starty+maxy)>(map_h*2)) starty=(map_h*2)-maxy;
		if (starty<0) starty=0;
		if ((startx+maxx)>(map_w*2)) startx=(map_w*2)-maxx;
		if (startx<0) startx=0;

		glNormal3f(0,0,1);
		for(y=0;y<maxy;y++) {
			for(x=0;x<maxx;x++) {
				if (x+startx<(map_w*2) &&
					y+starty<(map_h*2) &&
					discreetmap!=0) {
					switch(discreetmap[x+startx+(y+starty)*(map_w*2)]) {
					case T_GRASS: 
							glColor3f(0.0,1.0,0.0);
							break;
					case T_SAND:
							glColor3f(0.2,0.9,0.0);
							break;
					case T_MOUNTAINS:
							glColor3f(0.4,0.8,0.0);
							break;
					case T_HOLE:
							glColor3f(0.0,0.8,0.0);
							break;
					case T_BUILDING:
							glColor3f(0.0,0.0,0.0);
							break;
					case T_SHIP:
							glColor3f(1.0f,1.0f,1.0f);
							break;
					case T_ROBOT:
							glColor3f(0.0,0.0,1.0f);
							break;
					case T_EROBOT:
							glColor3f(1.0f,0.0,0.0);
							break;	
					} /* switch */ 
					glBegin(GL_QUADS);
					glVertex3f(30+y,maxx-(x+1),0);
					glVertex3f(30+y+1,maxx-(x+1),0);
					glVertex3f(30+y+1,maxx-x,0);
					glVertex3f(30+y,maxx-x,0);
					glEnd();
				} /* if */ 

/*				if (atackmap[x+(y+starty)*(map_w*2)]!=0) {
					glColor3f(1.0,1.0,0.0);
					glBegin(GL_QUADS);
					glVertex3f(30+y,(map_w*2)-(x+1),1);
					glVertex3f(30+y+1,(map_w*2)-(x+1),1);
					glVertex3f(30+y+1,(map_w*2)-x,1);
					glVertex3f(30+y,(map_w*2)-x,1);
					glEnd();
				} /* if */ 
			} /* for */ 
		} /* for */ 

		/* Draw the SHIP: */ 
		x=(int)(shipp.x*2-startx);
		y=(int)(shipp.y*2-starty);
		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_QUADS);
		glVertex3f(30+y,maxx-(x+2),2);
		glVertex3f(30+y+2,maxx-(x+2),2);
		glVertex3f(30+y+2,maxx-x,2);
		glVertex3f(30+y,maxx-x,2);
		glEnd();

	}


} /* NETHER::draw_radar */ 
