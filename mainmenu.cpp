#ifdef _WIN32
#include "windows.h"
#else
#include <stddef.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include "string.h"
#include "stdio.h"
#include "math.h"
#include "ctype.h"

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

#include "glprintf.h"

#ifndef __WINDOWS__
char *strupr(char *in)
 {
    static char buffer[1024];
    char *c;

    for (c = buffer;*in != 0;c++, in++)
     {
	*c = toupper(*in);
     }
    *c = 0;

    return(buffer);
 }
#endif

extern int SCREEN_X;
extern int SCREEN_Y;
extern int COLOUR_DEPTH;
extern bool fullscreen;
extern int shadows;
extern int detaillevel;
extern bool sound;
extern int level;
extern int up_key,down_key,left_key,right_key,fire_key,pause_key;
extern char mapname[128];
extern bool show_radar;
List<char> mapnames;

extern int mainmenu_status;
extern int mainmenu_substatus;
extern C3DObject *nethertittle;
unsigned char old_keyboard[SDLK_LAST];

void save_configuration(void);
void load_configuration(void);


int mainmenu_cycle(int width,int height)
{
	int i;
	int retval=0;
	unsigned char *keyboard;

	SDL_PumpEvents();
	keyboard = SDL_GetKeyState(NULL);

	switch(mainmenu_status) {
	case 0:
		mainmenu_substatus++;
		if (mainmenu_substatus>=40) {
			mainmenu_substatus=0;
			mainmenu_status=1;
		} /* if */ 
		break;
	case 1:
		mainmenu_substatus++;
		if ((keyboard[fire_key] && !old_keyboard[fire_key]) ||
			(keyboard[SDLK_1] && !old_keyboard[SDLK_1])) {
			mainmenu_status=4;
			mainmenu_substatus=0;
		} /* if */ 
		if (keyboard[SDLK_2] && !old_keyboard[SDLK_2]) {
			mainmenu_status=6;
			mainmenu_substatus=0;
		} /* if */ 
		if (keyboard[SDLK_3] && !old_keyboard[SDLK_3]) {
			mainmenu_status=2;
			mainmenu_substatus=0;
		} /* if */ 
		if (keyboard[SDLK_4] && !old_keyboard[SDLK_4]) {
			/* Change the MAP: */ 
			if (mapnames.EmptyP()) {
				/* Fill the mapnames list: */ 
#ifdef _WIN32
				WIN32_FIND_DATA finfo;
				HANDLE h;

				h=FindFirstFile("maps/*.map",&finfo);
				if (h!=INVALID_HANDLE_VALUE) {
					if (strcmp(finfo.cFileName,".")!=0 &&
						strcmp(finfo.cFileName,"..")!=0) {
						char *name;
						name=new char[strlen(finfo.cFileName)+1];
						strcpy(name,finfo.cFileName);
						mapnames.Add(name);
					} /* if */ 

					while(FindNextFile(h,&finfo)==TRUE) {

						if (strcmp(finfo.cFileName,".")!=0 &&
							strcmp(finfo.cFileName,"..")!=0) {
							char *name;
							name=new char[strlen(finfo.cFileName)+1];
							strcpy(name,finfo.cFileName);
							mapnames.Add(name);
						} /* if */ 
					} /* while */ 
				} /* if */ 
				
#else
				DIR *dp;
				struct dirent *ep;
				  
				dp = opendir ("./maps/");
				if (dp != NULL)
				 {
				    while (ep = readdir (dp))
				     {
					if ((strstr(ep->d_name,".map") + 4) == ep->d_name + strlen(ep->d_name))
					 {
						char *name;
						name=new char[strlen(ep->d_name)+1];
						strcpy(name,ep->d_name);
						mapnames.Add(name);
					 }
				     }
				    (void) closedir (dp);
				 }
#endif
				/* Look for the actualmap: */ 
				mapnames.Rewind();
				while(!mapnames.EndP() && strcmp(mapnames.GetObj(),mapname)!=0) mapnames.Next();

			} /* if */ 

			if (!mapnames.EmptyP()) {
				mapnames.Next();
				if (mapnames.EndP()) mapnames.Rewind();
				strcpy(mapname,mapnames.GetObj());
			} /* if */ 

			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_5] && !old_keyboard[SDLK_5]) {
			mainmenu_status=5;
			mainmenu_substatus=0;
		} /* if */ 
		break;
	case 2:
		mainmenu_substatus++;
		if (mainmenu_substatus>=40) {
			mainmenu_substatus=0;
			mainmenu_status=3;
		} /* if */ 
		break;
	case 3:
		if (keyboard[SDLK_1] && !old_keyboard[SDLK_1]) {
			switch(SCREEN_X) {
			case 320:
				SCREEN_X=400;
				SCREEN_Y=300;
				break;
			case 400:
				SCREEN_X=640;
				SCREEN_Y=480;
				break;
			case 640:
				SCREEN_X=800;
				SCREEN_Y=600;
				break;
			case 800:
				SCREEN_X=1024;
				SCREEN_Y=768;
				break;
			case 1024:
				SCREEN_X=320;
				SCREEN_Y=240;
				break;
			default:
				SCREEN_X=640;
				SCREEN_Y=480;
			} /* switch */ 
			retval=3;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_2] && !old_keyboard[SDLK_2]) {
			switch(COLOUR_DEPTH) {
			case 8:COLOUR_DEPTH=16;
				break;
			case 16:COLOUR_DEPTH=24;
				break;
			case 24:COLOUR_DEPTH=32;
				break;
			default:
				COLOUR_DEPTH=8;
			} /* switch */ 
			retval=3;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_3] && !old_keyboard[SDLK_3]) {
			if (fullscreen) fullscreen=false;
					   else fullscreen=true;
			retval=3;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_4] && !old_keyboard[SDLK_4]) {
			shadows++;
			if (shadows>=3) shadows=0;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_5] && !old_keyboard[SDLK_5]) {
			detaillevel++;
			if (detaillevel>=5) detaillevel=0;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_6] && !old_keyboard[SDLK_6]) {
			if (sound) sound=false;
				  else sound=true;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_7] && !old_keyboard[SDLK_7]) {
			level++;
			if (level>=4) level=0;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_8] && !old_keyboard[SDLK_8]) {
			if (show_radar) show_radar=false;
					   else show_radar=true;
			save_configuration();
		} /* if */ 
		if (keyboard[SDLK_9] && !old_keyboard[SDLK_9]) {
			mainmenu_status=0;
			mainmenu_substatus=0;
		} /* if */ 
		break;
	case 4:
		mainmenu_substatus++;
		if (mainmenu_substatus>=40) {
			retval=1;
		} /* if */ 
		break;
	case 5:
		mainmenu_substatus++;
		if (mainmenu_substatus>=40) {
			retval=2;
		} /* if */ 
		break;
	case 6:
		mainmenu_substatus++;
		if (mainmenu_substatus>=40) {
			mainmenu_status=7;
			mainmenu_substatus=0;
		} /* if */ 
		break;
	case 7:
		{
			int i;

			for(i=0;i<SDLK_LAST;i++) {
				if (keyboard[i] && !old_keyboard[i]) {
					switch(mainmenu_substatus) {
					case 0:up_key=i;
						   break;
					case 1:down_key=i;
						   break;
					case 2:left_key=i;
						   break;
					case 3:right_key=i;
						   break;
					case 4:fire_key=i;
						   break;
					case 5:pause_key=i;
						   break;
					} /* sritch */ 
					mainmenu_substatus++;
					if (mainmenu_substatus==7) {
						mainmenu_status=0;
						mainmenu_substatus=0;
						save_configuration();
					} /* if */ 
				} /* if */ 
			} /* for */ 
		}
		break;
	} /* if */ 

	for(i=0;i<SDLK_LAST;i++) old_keyboard[i]=keyboard[i];

	return retval;
} /* mainmenu_cycle */ 


void mainmenu_draw(int width,int height)
{
	float lightpos2[4]={0,0,1000,0};
	float tmpls[4]={1.0F,1.0F,1.0F,1.0};
	float tmpld[4]={0.6F,0.6F,0.6F,1.0};
	float tmpla[4]={0.2F,0.2F,0.2F,1.0};
    float ratio;

	if (nethertittle==0) {
		nethertittle=new C3DObject("models/tittle.asc","textures/");
		nethertittle->normalize(7.0);
	} /* if */ 

	/* Enable Lights, etc.: */ 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_AMBIENT,tmpla);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,tmpld);
	glLightfv(GL_LIGHT0,GL_SPECULAR,tmpls);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel( GL_SMOOTH );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );
	glDisable( GL_SCISSOR_TEST );  
	glEnable( GL_DEPTH_TEST );
	
	glLightfv(GL_LIGHT0,GL_POSITION,lightpos2);
    glClearColor(0,0,0,0.0);
    glViewport(0,0,width,height);
	ratio=(float)width/float(height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 30.0, ratio, 1.0, 1024.0 );
	gluLookAt(0,0,30,0,0,0,0,1,0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	switch(mainmenu_status) {
	case 0:
		glTranslatef(0,3,40-mainmenu_substatus);
		nethertittle->draw(1.0,1.0,1.0);
		break;
	case 1:
		glTranslatef(0,3,0);
		glPushMatrix();
		glRotatef(sin(mainmenu_substatus*0.02)*5.0f,0,1,0);
		nethertittle->draw(1.0,1.0,1.0);
		glPopMatrix();
		glColor3f(0.5,0.5,1.0);
		glTranslatef(-6,-6,0);
		scaledglprintf2(0.005,0.005,"1 - START NEW GAME   ");
		glTranslatef(0,-1,0);
		scaledglprintf2(0.005,0.005,"2 - REDEFINE KEYBOARD");
		glTranslatef(0,-1,0);
		scaledglprintf2(0.005,0.005,"3 - OPTIONS          ");
		glTranslatef(0,-1,0);
		scaledglprintf2(0.005,0.005,"4 - MAP: %s",mapname);
		glTranslatef(0,-1,0);
		scaledglprintf2(0.005,0.005,"5 - EXIT GAME        ");
		break;
	case 2:
	case 4:
	case 5:
	case 6:
		glTranslatef(0,3,mainmenu_substatus);
		nethertittle->draw(1.0,1.0,1.0);
		break;
	case 3:
		glColor3f(0.5,0.5,1.0);
		glTranslatef(0,3.5,0);
		if (SCREEN_X== 320) scaledglprintf(0.005,0.005,"1 - RESOLUTION:  320x240");
		if (SCREEN_X== 400) scaledglprintf(0.005,0.005,"1 - RESOLUTION:  400x300");
		if (SCREEN_X== 640) scaledglprintf(0.005,0.005,"1 - RESOLUTION:  640x480");
		if (SCREEN_X== 800) scaledglprintf(0.005,0.005,"1 - RESOLUTION:  800x600");
		if (SCREEN_X==1024) scaledglprintf(0.005,0.005,"1 - RESOLUTION: 1024x768");
		glTranslatef(0,-1,0);
		if (COLOUR_DEPTH== 8) scaledglprintf(0.005,0.005,"2 - COLOR DEPTH:  8bit  ");
		if (COLOUR_DEPTH==16) scaledglprintf(0.005,0.005,"2 - COLOR DEPTH: 16bit  ");
		if (COLOUR_DEPTH==24) scaledglprintf(0.005,0.005,"2 - COLOR DEPTH: 24bit  ");
		if (COLOUR_DEPTH==32) scaledglprintf(0.005,0.005,"2 - COLOR DEPTH: 32bit  ");
		glTranslatef(0,-1,0);
		if (fullscreen) scaledglprintf(0.005,0.005,"3 - FULLSCREEN          ");
				   else scaledglprintf(0.005,0.005,"3 - WINDOWED            ");
		glTranslatef(0,-1,0);
		if (shadows==0) scaledglprintf(0.005,0.005,"4 - SHADOWS: OFF        ");
		if (shadows==1) scaledglprintf(0.005,0.005,"4 - SHADOWS: ON - DIAG  ");
		if (shadows==2) scaledglprintf(0.005,0.005,"4 - SHADOWS: ON - VERT  ");
		glTranslatef(0,-1,0);
		if (detaillevel==0) scaledglprintf(0.005,0.005,"5 - DETAIL: LOWEST      ");
		if (detaillevel==1) scaledglprintf(0.005,0.005,"5 - DETAIL: LOW         ");
		if (detaillevel==2) scaledglprintf(0.005,0.005,"5 - DETAIL: MEDIUM      ");
		if (detaillevel==3) scaledglprintf(0.005,0.005,"5 - DETAIL: HIGH        ");
		if (detaillevel==4) scaledglprintf(0.005,0.005,"5 - DETAIL: HIGHEST     ");
		glTranslatef(0,-1,0);
		if (sound) scaledglprintf(0.005,0.005,"6 - SOUND: ON           ");
			  else scaledglprintf(0.005,0.005,"6 - SOUND: OFF          ");
		glTranslatef(0,-1,0);
		if (level==0) scaledglprintf(0.005,0.005,"7 - LEVEL: EASY         ");
		if (level==1) scaledglprintf(0.005,0.005,"7 - LEVEL: NORMAL       ");
		if (level==2) scaledglprintf(0.005,0.005,"7 - LEVEL: HARD         ");
		if (level==3) scaledglprintf(0.005,0.005,"7 - LEVEL: IMPOSSIBLE   ");
		glTranslatef(0,-1,0);
		if (show_radar) scaledglprintf(0.005,0.005,"8 - RADAR: ON           ");
				   else scaledglprintf(0.005,0.005,"8 - RADAR: OFF          ");
		glTranslatef(0,-1,0);
		scaledglprintf(0.005,0.005,"9 - BACK                ");
		break;
	case 7:
		{
			char tmp[256];

			glColor3f(0.5,0.5,1.0);
			glTranslatef(0,5,0);
			scaledglprintf(0.005,0.005,"REDEFINE KEYBOARD");
			glTranslatef(0,-2,0);
			if (mainmenu_substatus!=0) glColor3f(0.5,0.5,1.0);
								  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR UP: %s",strupr(SDL_GetKeyName((SDLKey)up_key)));
			scaledglprintf(0.005,0.005,tmp);
			glTranslatef(0,-1,0);
			if (mainmenu_substatus!=1) glColor3f(0.5,0.5,1.0);
						 	 	  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR DOWN: %s",strupr(SDL_GetKeyName((SDLKey)down_key)));
			scaledglprintf(0.005,0.005,tmp);
			glTranslatef(0,-1,0);
			if (mainmenu_substatus!=2) glColor3f(0.5,0.5,1.0);
								  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR LEFT: %s",strupr(SDL_GetKeyName((SDLKey)left_key)));
			scaledglprintf(0.005,0.005,tmp);
			glTranslatef(0,-1,0);
			if (mainmenu_substatus!=3) glColor3f(0.5,0.5,1.0);
								  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR RIGHT: %s",strupr(SDL_GetKeyName((SDLKey)right_key)));
			scaledglprintf(0.005,0.005,tmp);
			glTranslatef(0,-1,0);
			if (mainmenu_substatus!=4) glColor3f(0.5,0.5,1.0);
								  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR FIRE: %s",strupr(SDL_GetKeyName((SDLKey)fire_key)));
			scaledglprintf(0.005,0.005,tmp);

			glTranslatef(0,-1,0);
			if (mainmenu_substatus!=5) glColor3f(0.5,0.5,1.0);
								  else glColor3f(1.0,0.0,0.0);
			sprintf(tmp,"PRESS A KEY FOR PAUSE/MENU: %s",strupr(SDL_GetKeyName((SDLKey)pause_key)));
			scaledglprintf(0.005,0.005,tmp);

			glColor3f(0.5,0.5,1.0);
			glTranslatef(0,-2,0);
			scaledglprintf(0.005,0.005,"PG.UP/PG.DOWN CHANGE THE ZOOM");

			if (mainmenu_substatus>5) {
				glColor3f(1,1,1);
				glTranslatef(0,-2,0);
				scaledglprintf(0.005,0.005,"PRESS ANY KEY TO RETURN TO MAIN MENU");
			} /* if */ 
		}
		break;
	} /* switch */ 

	SDL_GL_SwapBuffers();
} /* NETHER::draw */ 



void load_configuration(void)
{
	int v;
	FILE *fp;

	fp=fopen("nether.cfg","r");
	if (fp==0) return;

	if (2!=fscanf(fp,"%i %i",&SCREEN_X,&SCREEN_Y)) return;
	if (1!=fscanf(fp,"%i",&v)) return;
	if (v==0) fullscreen=true;
		 else fullscreen=false;
	if (1!=fscanf(fp,"%i",&shadows)) return;
	if (1!=fscanf(fp,"%i",&detaillevel)) return;

	if (6!=fscanf(fp,"%i %i %i %i %i %i",&up_key,&down_key,&left_key,&right_key,&fire_key,&pause_key)) return;
	if (1!=fscanf(fp,"%i",&v)) return;
	if (v==0) sound=true;
		 else sound=false;

	if (1!=fscanf(fp,"%i",&level)) return;

	if (1!=fscanf(fp,"%s",mapname)) return; 

	fclose(fp);
} /* load_configuration */ 


void save_configuration(void)
{
	FILE *fp;

	fp=fopen("nether.cfg","w");
	if (fp==0) return;

	fprintf(fp,"%i %i\n",SCREEN_X,SCREEN_Y);
	fprintf(fp,"%i %i %i\n",(fullscreen ? 0 : 1),shadows,detaillevel);
	fprintf(fp,"%i %i %i %i %i %i\n",up_key,down_key,left_key,right_key,fire_key,pause_key);
	fprintf(fp,"%i\n",(sound ? 0 : 1));
	fprintf(fp,"%i\n",level);
	fprintf(fp,"%s\n",mapname);

	fclose(fp);
} /* save_configuration */ 
