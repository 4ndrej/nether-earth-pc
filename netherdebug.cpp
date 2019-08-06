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


extern int frames_per_sec;
extern bool fullscreen;
extern int shadows;
extern bool sound;
extern int up_key,down_key,left_key,right_key,fire_key,pause_key;
extern int level;


bool NETHER::save_debug_report(char *filename)
{
	int i,j;
	FILE *fp;
	BUILDING *b;
	ROBOT *r;
	BULLET *bul;
	EXPLOSION *e;

	fp=fopen(filename,"w");
	if (fp==0) return false;

/*
	int map_w,map_h;
	int *map;
	float lightpos[4];
	Vector lightposv;
*/

	fprintf(fp,"NETHER EARTH v0.2 Debug Report\n\n");

	fprintf(fp,"MAPW: %i\nMAPH: %i\n",map_w,map_h);

	fprintf(fp,"MAP:\n");
	for(i=0;i<map_h;i++) {
		for(j=0;j<map_w;j++) {
			fprintf(fp,"%i ",map[j+i*map_w]);
		} /* for */ 
		fprintf(fp,"\n");
	} /* for */ 

	fprintf(fp,"LIGHTPOS: %.8f %.8f %.8f %.8f\n",lightpos[0],lightpos[1],lightpos[2],lightpos[3]);
	fprintf(fp,"LIGHTPOSV: ");
	lightposv.save(fp);

/*
	Vector camera,viewp;
	Vector shipp;
	bool shiplanded;
	int ship_op,ship_op2,ship_op3;
*/

	fprintf(fp,"CAMERA: ");
	camera.save(fp);
	fprintf(fp,"VIEWP: ");
	viewp.save(fp);
	fprintf(fp,"SHIPP: ");
	shipp.save(fp);
	if (shiplanded) fprintf(fp,"SHIP LANDED\n");
			   else fprintf(fp,"SHIP NOT LANDED\n");

/*
	List<BUILDING> buildings;
	List<ROBOT> robots[2];
	List<BULLET> bullets;
	List<EXPLOSION> explosions;
*/ 

	fprintf(fp,"\nNº OF BUILDINGS: %i\n",buildings.Length());
	buildings.Rewind();
	while(buildings.Iterate(b)) {
/*
		int type;
		Vector pos;
		int owner;
		int status;	
*/ 
		fprintf(fp,"BUILDING:\n TYPE: %i\n OWNER: %i\n STATUS: %i\n\n",b->type,b->owner,b->status);
		b->pos.save(fp);
	} /* while */ 

	for(i=0;i<2;i++) {
		fprintf(fp,"\nNº OF ROBOTS PLAYER %i: %i\n",i,robots[i].Length());
		robots[i].Rewind();
		while(robots[i].Iterate(r)) {
			char *tractions[3]={"BIPOD","TRACKS","ANTIGRAV"};
			char *pieces[5]={"CANNONS","MISSILES","PHASERS","NUCLEAR","ELECTRONICS"};
/*
			int traction;
			bool pieces[5];
			int program;
			int program_parameter;
			Vector program_goal;
			int op;
			bool shipover;
			int firetimer;
			int strength;
			Vector pos;
			int angle;
			CMC cmc;
*/
			fprintf(fp,"ROBOT:\n");
			fprintf(fp," %s\n",tractions[r->traction]);
			for(j=0;j<5;j++) {
				if (r->pieces[j]) fprintf(fp," %s\n",pieces[j]);
			} /* for */ 
			fprintf(fp," PROGRAM: %i\n PROGRAM PARAMETER: %i\n",r->program,r->program_parameter);
			fprintf(fp," PROGRAM GOAL: ");
			r->program_goal.save(fp);
			fprintf(fp," ACTUAL OPERATOR: %i\n",r->op);
			if (r->shipover) fprintf(fp," HAS THE SHIP OVER IT\n");
						else fprintf(fp," DO NOT HAS THE SHIP OVER IT\n");
			fprintf(fp," FIRETIMER: %i\n STRENGTH: %i\n",r->firetimer,r->strength);
			fprintf(fp," POSITION: ");
			r->pos.save(fp);
			fprintf(fp," ANGLE: %i\n",r->angle);
			fprintf(fp," MINIMUM CONTAINER BOX: \n");
			r->cmc.save(fp);
			fprintf(fp," ELECTRONICS STATE: %i\n CHASSIS STATE: %i\n",r->electronics_state,r->chassis_state);
			fprintf(fp,"\n");
		} /* while */ 
	} /* for */ 

	fprintf(fp,"\nNº BULLETS: %i\n",bullets.Length());

	bullets.Rewind();
	while(bullets.Iterate(bul)) {
/*
		int type;
		int step;
		Vector pos;
		int angle;
		ROBOT *owner;
		CMC cmc;
*/
		fprintf(fp," BULLET:\n TYPE: %i\n STEP: %i\n ANGLE: %i\n",bul->type,bul->step,bul->angle);

		fprintf(fp," POSITION: ");
		bul->pos.save(fp);
		i=robots[0].SearchObjRef(bul->owner);
		if (i==-1) {
			i=robots[1].SearchObjRef(bul->owner);
			fprintf(fp," OWNER: PLAYER 1 ROBOT %i\n",i);
		} else {
			fprintf(fp," OWNER: PLAYER 0 ROBOT %i\n",i);
		} /* if */ 

		fprintf(fp," MINIMUM CONTAINER BOX: \n");
		bul->cmc.save(fp);
		fprintf(fp,"\n");
	} /* while */ 

	fprintf(fp,"\nNº EXPLOSIONS %i\n",explosions.Length());
	explosions.Rewind();
	while(explosions.Iterate(e)) {
/*
		Vector pos;
		int step;
		int size;
*/ 
		fprintf(fp,"EXPLOSION:\n POSITION:\n");
		e->pos.save(fp);
		fprintf(fp," STEP: %i\n SIZE: %i\n\n",e->step,e->size);
	} /* while */ 

/*
	int day,hour,minute,second;
	int resources[2][7];
	ROBOT *controlled;
*/ 

	fprintf(fp,"\nTIME: DAY %i %i:%i:%i\n",day,hour,minute,second);
	fprintf(fp,"\nRESOURCES:\n");
	for(i=0;i<2;i++) {
		fprintf(fp,"PLAYER %i: ",i);
		for(j=0;j<7;j++) {
			fprintf(fp,"%i ",resources[i][j]);
		} /* for */ 
		fprintf(fp,"\n");
	} /* for */ 
	fprintf(fp,"\nROBOT UNDER CONTROL: %i\n",robots[0].SearchObjRef(controlled));

/*
	int act_menu;
	int act_button;
*/ 
	fprintf(fp,"\nMENU %i\nACT BUTTON: %i\n",act_menu,act_button);

	fclose(fp);
	return true;
} /* NETHER::save_debug_report */ 

 
