#ifdef _WIN32
#include "windows.h"
#endif

#include "string.h"
#include <stdlib.h>
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

/* Artificial Intelligence variables: */ 
/*

int *discreetmap;	->	- Will keep a discreet copy of the map, telling whether a robot can occupy a position
						or not. 
						- Each cell of the original map, transform to a set of 2x2 cells of this new map, so
						  each 2x2 square of this cells correspond to one of the positions where a robot can 
						  be.

int *bk_discreetmap;	-> just contains the background.

AI_OPERATOR *searchmap;	-> In this map is where the search process will be performed.

*/

const int WE_SEARCH_DEPTH=12;	/* Search depth for robots with ELECTRONICS	*/ 
//const int WE_SEARCH_DEPTH=80;	/* Search depth for robots with ELECTRONICS	*/ 
const int WOE_SEARCH_DEPTH=4; /* Search depth for robots without ELECTRONICS	*/ 

const int xd[4]={ 1, 0,-1, 0};
const int yd[4]={ 0, 1, 0,-1};
const int xd2[4]={ 0, 0,-1, 0};
const int yd2[4]={ 0, 0, 0,-1};
const int xd3[4]={ 1, 0, 1, 0};
const int yd3[4]={ 0, 1, 0, 1};
const int dangle[4]={0,90,180,270};


extern FILE *debug_fp;

void robot_zone(Vector pos,int *x,int *y,int *dx,int *dy)
{
	*x=int((pos.x-0.5)/0.5);
	*y=int((pos.y-0.5)/0.5);

	if ((int(pos.x*256)%128)==0) *dx=2;
					 	    else *dx=3;
	if ((int(pos.y*256)%128)==0) *dy=2;
						    else *dy=3;
} /* robot_zone */ 


void fill_zone(int *map,int w,int val,int x,int y,int dx,int dy) 
{
	int i,j;

	for(i=0;i<dx;i++) {
		for(j=0;j<dy;j++) {
			map[(y+j)*w+(x+i)]=val;
		} /* for */ 
	} /* for */ 
} /* fill_zone */ 


void NETHER::AI_precomputations(void)
{
	int i;
	int x,y;

	discreetmap=new int[(map_w*2)*(map_h*2)];
	bk_discreetmap=new int[(map_w*2)*(map_h*2)];
	searchmap=new AI_OPERATOR *[(map_w*2)*(map_h*2)];
	atackmap=new int[(map_w*2)*(map_h*2)];
	for(i=0;i<(map_w*2)*(map_h*2);i++) {
		searchmap[i]=new AI_OPERATOR();
		searchmap[i]->used=false;
		atackmap[i]=0;
	} /* for */ 

	/* Setup the terrains for the new map: */ 
	for(y=0;y<map_h;y++) {
		for(x=0;x<map_w;x++) {
			fill_zone(discreetmap,map_w*2,MapTerrain(float(x),float(y)),x*2,y*2,2,2);
			fill_zone(bk_discreetmap,map_w*2,MapTerrain(float(x),float(y)),x*2,y*2,2,2);
		} /* for */ 
	} /* for */ 

	/* Transfer all the buildings to the new map: */ 
	{
		List<BUILDING> l;
		BUILDING *b;

		l.Instance(buildings);
		l.Rewind();

		while(l.Iterate(b)) {
			x=int(b->pos.x/0.5);
			y=int(b->pos.y/0.5);
			fill_zone(discreetmap,map_w*2,T_BUILDING,x,y,2,2);
		} /* while */ 
	}
} /* NETHER::AI_precomputations */ 


void NETHER::AI_deleteprecomputations(void)
{
	int i;

	delete discreetmap;
	discreetmap=0;
	delete bk_discreetmap;
	bk_discreetmap=0;
	for(i=0;i<map_w*2*map_h*2;i++) {
		delete searchmap[i];
		searchmap[i]=0;
	} /* for */ 
	delete searchmap;
	searchmap=0;
} /* NETHER::AI_deleteprecomputations */ 


void NETHER::AI_removebuilding(Vector pos)
{
	int i,j;
	int x,y;
	
	x=int(pos.x/0.5);
	y=int(pos.y/0.5);

	for(i=0;i<2;i++) {
		for(j=0;j<2;j++) {
			discreetmap[(y+j)*(map_w*2)+(x+i)]=bk_discreetmap[(y+j)*(map_w*2)+(x+i)];
		} /* for */ 
	} /* for */ 
} /* NETHER::AI_removebuilding */ 


void NETHER::AI_release(void)
{
	delete discreetmap;
	discreetmap=0;
} /* NETHER::AI_release */ 


void NETHER::AI_newrobot(Vector pos,int owner)
{
	int x,y,dx,dy;

	robot_zone(pos,&x,&y,&dx,&dy);
	if (owner==0) fill_zone(discreetmap,map_w*2,T_ROBOT,x,y,dx,dy);
			 else fill_zone(discreetmap,map_w*2,T_EROBOT,x,y,dx,dy);
} /* NETHER::AI_newrobot */ 


int NETHER::AI_killrobot(Vector pos)
{
	int i,j;
	int x,y,dx,dy;
	int owner;
	
	robot_zone(pos,&x,&y,&dx,&dy);
	owner=discreetmap[y*(map_w*2)+x];
	for(i=0;i<dx;i++) {
		for(j=0;j<dy;j++) {
			discreetmap[(y+j)*(map_w*2)+(x+i)]=bk_discreetmap[(y+j)*(map_w*2)+(x+i)];
		} /* for */ 
	} /* for */ 

	return owner;
} /* NETHER::AI_killrobot */ 


void NETHER::AI_moverobot(Vector oldpos,Vector newpos,int owner)
{
	AI_killrobot(oldpos);
	AI_newrobot(newpos,owner);
} /* NETHER::AI_moverobot */ 


void NETHER::AI_availableoperators(Vector pos,int angle,int traction,List<AI_OPERATOR> *l)
{
	int i;
	int x,y,dx,dy;
	int terrain;
	int n_turns;
	int dif;
	int cost;
	AI_OPERATOR *op;

	robot_zone(pos,&x,&y,&dx,&dy);

	for(i=0;i<4;i++) {
		terrain=AI_WorseMapTerrain(x+xd2[i],y+yd2[i],2+xd3[i],2+yd3[i]);
		if (terrain<=T_HOLE &&
			Walkable(traction,terrain)) {

			/* Rotation cost: */ 
			dif=dangle[i]-angle;
			if (dif>360) dif-=360;
			if (dif<0) dif+=360;
			n_turns=0;
			if (dif==90 || dif==270) n_turns=1;
			if (dif==180) n_turns=2;
			{
				float tmp=float(RobotRotationSpeed(terrain,traction));
				cost=n_turns*int(90/tmp);
			}

			/* Displacement cost: */ 
			terrain=AI_WorseMapTerrain(x+xd[i],y+yd[i],2,2);
			{
				float tmp=RobotSpeed(terrain,traction);
				cost+=int(0.5/tmp);
			}

			/* Build a new AI_operator: */ 
			op=new AI_OPERATOR();
			op->cost=cost;
			op->previous=-1;
			op->deadend=false;
			if (dif==0) op->first_robotop=ROBOTOP_FORWARD;
			if (dif==90) op->first_robotop=ROBOTOP_RIGHT;
			if (dif==180) op->first_robotop=ROBOTOP_RIGHT;
			if (dif==270) op->first_robotop=ROBOTOP_LEFT;
			op->newpos=Vector(pos.x+xd[i]*0.5,pos.y+yd[i]*0.5,pos.z);
			l->Add(op);
		} /* if */ 
	} /* for */ 
} /* NETHER::AI_availableoperators */ 


bool NETHER::AI_expandoperators(int x,int y,int angle,int traction,int previous,int oldcost,int depth)
{
	int newpos;
	int i;
	int terrain,terrain2;
	int n_turns;
	int dif;
	int cost;
	bool deadend=true; 

	for(i=0;i<4;i++) {
		newpos=previous+xd[i]+yd[i]*(map_w*2);
		if (newpos!=searchmap[previous]->previous) {
			terrain=AI_WorseMapTerrain(x+xd2[i],y+yd2[i],2+xd3[i],2+yd3[i]);
			if (terrain<=T_HOLE &&
				Walkable(traction,terrain)) {
				cost=oldcost;

				/* Rotation cost: */ 
				dif=dangle[i]-angle;
				if (dif>360) dif-=360;
				if (dif<0) dif+=360;
				n_turns=0;
				if (dif==90 || dif==270) n_turns=1;
				if (dif==180) n_turns=2;

				terrain2=AI_WorseMapTerrain(x,y,2,2);
				cost+=n_turns*int(90/float(RobotRotationSpeed(traction,terrain2)));

				/* Displacement cost: */ 
				cost+=int(0.5/RobotSpeed(traction,terrain));

				/* Replace the an old operator by a new one: */ 
				if (!searchmap[newpos]->used ||
					searchmap[newpos]->cost>cost) {

					/* Set the new AI_operator: */  
					searchmap[newpos]->used=true;
					searchmap[newpos]->cost=cost;
					searchmap[newpos]->previous=previous;
					searchmap[newpos]->deadend=false;
					if (dif==0) searchmap[newpos]->first_robotop=ROBOTOP_FORWARD;
					if (dif==90) searchmap[newpos]->first_robotop=ROBOTOP_RIGHT;
					if (dif==180) searchmap[newpos]->first_robotop=ROBOTOP_RIGHT;
					if (dif==270) searchmap[newpos]->first_robotop=ROBOTOP_LEFT;
					searchmap[newpos]->newpos=Vector((x+xd[i])*0.5,(y+yd[i])*0.5,0); 

					/* Continue the search process: */ 
					if (depth>0) {
						if (!AI_expandoperators(x+xd[i],y+yd[i],dangle[i],traction,newpos,cost,depth-1)) {
							deadend=false;
						} /* if */ 
					} else {
						deadend=false;
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} /* for */ 

	searchmap[previous]->deadend=deadend;

	return deadend;
} /* NETHER::AI_expandoperators */ 


int NETHER::AI_searchengine(Vector pos,int angle,int goaltype,Vector goalpos,int traction,int depth)
{
	int offs;
	int x,y,dx,dy;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Expanding de search tree\n");
	fflush(debug_fp);
#endif

	/* Expand the search tree: */ 
	robot_zone(pos,&x,&y,&dx,&dy);

	/* Build a new AI_operator: */ 
	offs=y*(map_w*2)+x;
	searchmap[offs]->used=true;
	searchmap[offs]->cost=0;
	searchmap[offs]->previous=-1;
	searchmap[offs]->first_robotop=ROBOTOP_NONE;
	searchmap[offs]->newpos=pos;
	searchmap[offs]->deadend=false;
	AI_expandoperators(x,y,angle,traction,y*(map_w*2)+x,0,depth);

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Expanding de search tree finished\n");
	fflush(debug_fp);
#endif


	/* ADVANCE PROGRAM: */ 
	if (goaltype==PROGRAM_ADVANCE) {
		int i,j;
		double further;
		int mincost;
		bool first=true;
		AI_OPERATOR *op,*bestop=0;

		for(i=-depth;i<depth;i++) {
			for(j=-depth;j<depth;j++) {
				if ((x+i)>=0 && (x+i)<(map_w*2) &&
					(y+j)>=0 && (y+j)<(map_h*2) &&
					(i!=0 || j!=0)) {
					op=searchmap[(y+j)*(map_w*2)+(x+i)];
					if (op->used) {
						if (first ||
							(op->deadend==bestop->deadend &&
							 (op->newpos.y>further ||
							  (op->newpos.y==further && op->cost<mincost))) ||
							 (!op->deadend && bestop->deadend)) {
							bestop=op;
							further=op->newpos.y;
							mincost=op->cost;
							first=false;
						} /*if */ 
					} /* if */ 
				} /* if */ 
			} /* for */ 
		} /* for */ 

		if (bestop!=0) {
			int rop;
			AI_OPERATOR *prev;

			prev=bestop;
			while(bestop->previous!=-1 && searchmap[bestop->previous]->used) {
				prev=bestop;
				bestop=searchmap[bestop->previous];
			} /* while */ 
			rop=prev->first_robotop;

			AI_resetsearch(pos,depth);
			return rop;
		} else {
			AI_resetsearch(pos,depth);
			return ROBOTOP_NONE;
		} /* if */ 

		AI_resetsearch(pos,depth);
		if (bestop!=0) return bestop->first_robotop;
				  else return ROBOTOP_NONE;
	} /* if */ 

	/* RETREAT PROGRAM: */ 
	if (goaltype==PROGRAM_RETREAT) {
		int i,j;
		double further;
		int mincost;
		bool first=true;
		AI_OPERATOR *op,*bestop=0;

		for(i=-depth;i<depth;i++) {
			for(j=-depth;j<depth;j++) {
				if ((x+i)>=0 && (x+i)<(map_w*2) &&
					(y+j)>=0 && (y+j)<(map_h*2) &&
					(i!=0 || j!=0)) {
					op=searchmap[(y+j)*(map_w*2)+(x+i)];
					if (op->used) {
						if (first ||
							(op->deadend==bestop->deadend &&
							 (op->newpos.y<further ||
							  (op->newpos.y==further && op->cost<mincost))) ||
							(!op->deadend && bestop->deadend)) {
							bestop=op;
							further=op->newpos.y;
							mincost=op->cost;
							first=false;
						} /*if */ 
					} /* if */ 
				} /* if */ 
			} /* for */ 
		} /* for */ 

		if (bestop!=0) {
			int rop;
			AI_OPERATOR *prev;

			prev=bestop;
			while(bestop->previous!=-1 && searchmap[bestop->previous]->used) {
				prev=bestop;
				bestop=searchmap[bestop->previous];
			} /* while */ 
			rop=prev->first_robotop;

			AI_resetsearch(pos,depth);
			return rop;
		} else {
			AI_resetsearch(pos,depth);
			return ROBOTOP_NONE;
		} /* if */ 

		AI_resetsearch(pos,depth);
		if (bestop!=0) return bestop->first_robotop;
				  else return ROBOTOP_NONE;
	} /* if */ 

	/* CAPTURE PROGRAM: */ 
	if (goaltype==PROGRAM_CAPTURE) {
		int i,j;
		double closer;
		int mincost;
		bool first=true;
		AI_OPERATOR *op,*bestop=0;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Search engine for CAPTURE START\n");
	fflush(debug_fp);
#endif

		for(i=-depth;i<depth;i++) {
			for(j=-depth;j<depth;j++) {
				if ((x+i)>=0 && (x+i)<(map_w*2) &&
					(y+j)>=0 && (y+j)<(map_h*2) &&
					(i!=0 || j!=0)) {
					op=searchmap[(y+j)*(map_w*2)+(x+i)];
					if (op->used) {
						if (first ||
							((op->deadend==bestop->deadend || (op->newpos+Vector(0.5,0.5,0)-goalpos).norma()==0) &&
							 ((op->newpos+Vector(0.5,0.5,0)-goalpos).norma()<closer ||
							  ((op->newpos+Vector(0.5,0.5,0)-goalpos).norma()==closer && op->cost<mincost))) ||
							(!op->deadend && bestop->deadend && closer!=0)) {
							bestop=op;
							closer=(op->newpos+Vector(0.5,0.5,0)-goalpos).norma();
							mincost=op->cost;
							first=false;
						} /*if */ 
					} /* if */ 
				} /* if */ 
			} /* for */ 
		} /* for */ 

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Search engine for CAPTURE FINISHED\n");
	fflush(debug_fp);
#endif

		if (bestop!=0) {
			int rop;
			AI_OPERATOR *prev;

			prev=bestop;
			while(bestop->previous!=-1 && searchmap[bestop->previous]->used) {
				prev=bestop;
				bestop=searchmap[bestop->previous];
			} /* while */ 
			rop=prev->first_robotop;

			AI_resetsearch(pos,depth);
			return rop;
		} else {
			AI_resetsearch(pos,depth);
			return ROBOTOP_NONE;
		} /* if */ 

		AI_resetsearch(pos,depth);
		if (bestop!=0) return bestop->first_robotop;
				  else return ROBOTOP_NONE;
	} /* if */ 


	/* DESTROY PROGRAM: */ 
	if (goaltype==PROGRAM_DESTROY) {
		int i,j;
		double closer;
		AI_OPERATOR *op;
		int mincost,mincost2;
		bool first=true,first2=true;
		AI_OPERATOR *bestop=0;	/* Best operator to get closer to the nearest robot */ 
		AI_OPERATOR *bestop2=0;	/* Best operator to reach an attaking position		*/ 

		for(i=-depth;i<depth;i++) {
			for(j=-depth;j<depth;j++) {
				if ((x+i)>=0 && (x+i)<(map_w*2) &&
					(y+j)>=0 && (y+j)<(map_h*2) &&
					(i!=0 || j!=0)) {
					op=searchmap[(y+j)*(map_w*2)+(x+i)];
					if (op->used) {
						if (first ||
							((op->deadend==bestop->deadend || (op->newpos+Vector(0.5,0.5,0)-goalpos).norma()==0) &&
							 ((op->newpos+Vector(0.5,0.5,0)-goalpos).norma()<closer ||
							  ((op->newpos+Vector(0.5,0.5,0)-goalpos).norma()==closer && op->cost<mincost))) ||
							(!op->deadend && bestop->deadend && closer!=0)) {
							bestop=op;
							closer=(op->newpos+Vector(0.5,0.5,0)-goalpos).norma();
							mincost=op->cost;
							first=false;
						} /*if */ 
						if (atackmap[(y+j)*(map_w*2)+(x+i)]!=0 ||
							atackmap[(y+j+1)*(map_w*2)+(x+i)]!=0 ||
							atackmap[(y+j)*(map_w*2)+(x+i+1)]!=0 ||
							atackmap[(y+j+1)*(map_w*2)+(x+i+1)]!=0) {
							if (first2 ||
								op->cost<mincost2) {
								bestop2=op;
								mincost2=op->cost;
								first2=false;
							} /*if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} /* for */ 
		} /* for */ 

		/* It's better to reach an attacking position if this is possible: */ 
		if (bestop2!=0) bestop=bestop2;

		if (bestop!=0) {
			int rop;
			AI_OPERATOR *prev;

			prev=bestop;
			while(bestop->previous!=-1 && searchmap[bestop->previous]->used) {
				prev=bestop;
				bestop=searchmap[bestop->previous];
			} /* while */ 
			rop=prev->first_robotop;

			AI_resetsearch(pos,depth);
			return rop;
		} else {
			AI_resetsearch(pos,depth);
			return ROBOTOP_NONE;
		} /* if */ 

		AI_resetsearch(pos,depth);
		if (bestop!=0) return bestop->first_robotop;
				  else return ROBOTOP_NONE;
	} /* if */ 

	return ROBOTOP_NONE;
} /* AI_searchengine */ 


void NETHER::AI_resetsearch(Vector pos,int depth)
{
	int i,j;
	int x,y,dx,dy;

	robot_zone(pos,&x,&y,&dx,&dy);

	for(i=-depth;i<depth;i++) {
		for(j=-depth;j<depth;j++) {
			if ((x+i)>=0 && (x+i)<(map_w*2) &&
				(y+j)>=0 && (y+j)<(map_h*2)) {
				searchmap[(y+j)*(map_w*2)+(x+i)]->used=false;
			} /* if */ 
		} /* for */ 
	} /* for */ 

} /* AI_resetsearch */ 


int NETHER::AI_WorseMapTerrain(int x,int y,int dx,int dy)
{
	int t=T_GRASS,t2;
	int i,j;

	for(i=0;i<dx;i++) {
		for(j=0;j<dy;j++) {
			if (x+i<0 || x+i>=map_w*2 ||
				y+j<0 || y+j>=map_h*2) return T_OUT;
			t2=discreetmap[(y+j)*(map_w*2)+x+i];
			if (t2>t) t=t2;
		} /* if */ 
	} /* for */ 

	return t;
} /* NETHER::AI_WorseMapTerrain */ 


int NETHER::AI_program_advance(int amount,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces)
{
	/* First of all, delete the robot from the discreet map: */ 
	int op=ROBOTOP_NONE;
	int type;
	List<AI_OPERATOR> l;
	Vector tmp_goal;

	op=AI_program_stopdefend(&tmp_goal,pos,angle,traction,electronics,player,pieces);
	if (op!=ROBOTOP_NONE) return op;

	type=AI_killrobot(pos);

	AI_availableoperators(pos,angle,traction,&l);

	if (!l.EmptyP()) {
		/* Choose one operator: */ 
		AI_OPERATOR *aiop;

		if (electronics) {
			op=AI_searchengine(pos,angle,PROGRAM_ADVANCE,Vector(0,0,0),traction,WE_SEARCH_DEPTH);
		} else {
			if ((rand()%4)!=0) {
				op=AI_searchengine(pos,angle,PROGRAM_ADVANCE,Vector(0,0,0),traction,WOE_SEARCH_DEPTH);
			} else {
				AI_rankoperators_advance(&l);
				aiop=AI_chooseoperator(&l,8);
				op=aiop->first_robotop;
			} /* if */ 
		} /* if */ 
	} /* if */ 

	/* Reconstruct the decreet map: */ 
	if (type==T_ROBOT) AI_newrobot(pos,0);
				  else AI_newrobot(pos,2);

	return op;
} /* NETHER::AI_program_advance */ 


int NETHER::AI_program_retreat(int amount,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces)
{
	/* First of all, delete the robot from the discreet map: */ 
	int op=ROBOTOP_NONE;
	int type;
	List<AI_OPERATOR> l;
	Vector tmp_goal;

	op=AI_program_stopdefend(&tmp_goal,pos,angle,traction,electronics,player,pieces);
	if (op!=ROBOTOP_NONE) return op;

	type=AI_killrobot(pos);

	AI_availableoperators(pos,angle,traction,&l);

	if (!l.EmptyP()) {
		/* Choose one operator: */ 
		AI_OPERATOR *aiop;

		if (electronics) {
			op=AI_searchengine(pos,angle,PROGRAM_RETREAT,Vector(0,0,0),traction,WE_SEARCH_DEPTH);
		} else {
			if ((rand()%4)!=0) {
				op=AI_searchengine(pos,angle,PROGRAM_RETREAT,Vector(0,0,0),traction,WOE_SEARCH_DEPTH);
			} else {
				AI_rankoperators_retreat(&l);
				aiop=AI_chooseoperator(&l,8);
				op=aiop->first_robotop;
			} /* if */ 
		} /* if */ 
	} /* if */ 

	/* Reconstruct the decreet map: */ 
	if (type==T_ROBOT) AI_newrobot(pos,0);
				  else AI_newrobot(pos,1);

	return op;
} /* NETHER::AI_program_retreat */ 


int NETHER::AI_program_capture(int goal,Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces)
{
	/* First of all, delete the robot from the discreet map: */ 
	int op=ROBOTOP_NONE;
	int type;
	List<AI_OPERATOR> l;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"AI_program_capture START\n");
	fflush(debug_fp);
#endif

	op=AI_program_stopdefend(program_goal,pos,angle,traction,electronics,player,pieces);
	if (op!=ROBOTOP_NONE) return op;

	type=AI_killrobot(pos);

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Searching available operators\n");
	fflush(debug_fp);
#endif

	AI_availableoperators(pos,angle,traction,&l);

	if (!l.EmptyP()) {
		/* Choose one operator: */ 
		AI_OPERATOR *aiop;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Seeking a goal\n");
	fflush(debug_fp);
#endif

		{
			/* Seek a goal: */ 
			List<BUILDING> l;
			BUILDING *b;
			bool anygoal=false;
			float distance,minimumdistance;

			*program_goal=Vector(-1,-1,-1);

			l.Instance(buildings);
			l.Rewind();
			while(l.Iterate(b)) {
				if (goal==P_PARAM_WARBASES && 
					b->type==B_WARBASE && b->owner!=player &&
					AI_WorseMapTerrain(int((b->pos.x+2.0)/0.5),int(b->pos.y/0.5),2,2)<=T_HOLE) {
					distance=float(((b->pos+Vector(2.5,0.5,0))-pos).norma());
					if (!anygoal || distance<minimumdistance) {
						anygoal=true;
						minimumdistance=distance;
						*program_goal=b->pos+Vector(2.5,0.5,0);
					} /* if */ 
				} /* if */ 
				if (goal==P_PARAM_NFACTORIES && 
					(b->type==B_FACTORY_ELECTRONICS ||
					 b->type==B_FACTORY_NUCLEAR ||
					 b->type==B_FACTORY_PHASERS ||
					 b->type==B_FACTORY_MISSILES ||
					 b->type==B_FACTORY_CANNONS ||
					 b->type==B_FACTORY_CHASSIS) && b->owner==0 &&
					AI_WorseMapTerrain(int((b->pos.x+1.0)/0.5),int(b->pos.y/0.5),2,2)<=T_HOLE) {
					distance=float(((b->pos+Vector(1.5,0.5,0))-pos).norma());
					if (!anygoal || distance<minimumdistance) {
						anygoal=true;
						minimumdistance=distance;
						*program_goal=b->pos+Vector(1.5,0.5,0);
					} /* if */ 
				} /* if */ 
				if (goal==P_PARAM_EFACTORIES && 
					(b->type==B_FACTORY_ELECTRONICS ||
					 b->type==B_FACTORY_NUCLEAR ||
					 b->type==B_FACTORY_PHASERS ||
					 b->type==B_FACTORY_MISSILES ||
					 b->type==B_FACTORY_CANNONS ||
					 b->type==B_FACTORY_CHASSIS) && b->owner!=0 && b->owner!=player &&
					AI_WorseMapTerrain(int((b->pos.x+1.0)/0.5),int(b->pos.y/0.5),2,2)<=T_HOLE) {
					distance=float(((b->pos+Vector(1.5,0.5,0))-pos).norma());
					if (!anygoal || distance<minimumdistance) {
						anygoal=true;
						minimumdistance=distance;
						*program_goal=b->pos+Vector(1.5,0.5,0);
					} /* if */ 
				} /* if */ 
			} /* while */ 
		} 

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Searhing a plan to reach the goal\n");
	fflush(debug_fp);
#endif

		if (program_goal->x!=-1 &&
			(*program_goal)!=pos) {
			if (electronics) {
				op=AI_searchengine(pos,angle,PROGRAM_CAPTURE,*program_goal,traction,WE_SEARCH_DEPTH);
			} else {
				if ((rand()%4)!=0) {
					op=AI_searchengine(pos,angle,PROGRAM_CAPTURE,*program_goal,traction,WOE_SEARCH_DEPTH);
				} else {
					AI_rankoperators_capture(&l,*program_goal);
					aiop=AI_chooseoperator(&l,8);
					op=aiop->first_robotop;
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} /* if */ 

	/* Reconstruct the decreet map: */ 
	if (type==T_ROBOT) AI_newrobot(pos,0);
				  else AI_newrobot(pos,1);

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"AI_program_capture FINISHED\n");
	fflush(debug_fp);
#endif

	return op;
} /* NETHER::AI_program_capture */ 


int NETHER::AI_RealShotPaths(int x,int y,int player,int persistence)
{
	int rsp=0;
	int i;
//	int persistence=CANNON_PERSISTENCE;

	for(i=2;i<int((persistence*BULLET_SPEED)/0.5)+2 && (x+i<map_w*2);i++) {
		if (discreetmap[x+i+y*(map_w*2)]==T_BUILDING ||
			discreetmap[x+i+(y+1)*(map_w*2)]==T_BUILDING) break;
		if (player==1) {
			if (discreetmap[x+i+y*(map_w*2)]==T_ROBOT ||
				discreetmap[x+i+(y+1)*(map_w*2)]==T_ROBOT) break;
			if (discreetmap[x+i+y*(map_w*2)]==T_EROBOT ||
				discreetmap[x+i+(y+1)*(map_w*2)]==T_EROBOT) {
				rsp|=1;
				break;
			} /* if */ 
		} else {
			if (discreetmap[x+i+y*(map_w*2)]==T_EROBOT ||
				discreetmap[x+i+(y+1)*(map_w*2)]==T_EROBOT) break;
			if (discreetmap[x+i+y*(map_w*2)]==T_ROBOT ||
				discreetmap[x+i+(y+1)*(map_w*2)]==T_ROBOT) {
				rsp|=1;
				break;
			} /* if */ 
		} /* if */ 
	} /* for */ 

	for(i=1;i<int((persistence*BULLET_SPEED)/0.5)+1 && (x-i>=0);i++) {
		if (discreetmap[x-i+y*(map_w*2)]==T_BUILDING ||
			discreetmap[x-i+(y+1)*(map_w*2)]==T_BUILDING) break;
		if (player==1) {
			if (discreetmap[x-i+y*(map_w*2)]==T_ROBOT ||
				discreetmap[x-i+(y+1)*(map_w*2)]==T_ROBOT) break;
			if (discreetmap[x-i+y*(map_w*2)]==T_EROBOT ||
				discreetmap[x-i+(y+1)*(map_w*2)]==T_EROBOT) {
				rsp|=4;
				break;
			} /* if */ 
		} else {
			if (discreetmap[x-i+y*(map_w*2)]==T_EROBOT ||
				discreetmap[x-i+(y+1)*(map_w*2)]==T_EROBOT) break;
			if (discreetmap[x-i+y*(map_w*2)]==T_ROBOT ||
				discreetmap[x-i+(y+1)*(map_w*2)]==T_ROBOT) {
				rsp|=4;
				break;
			} /* if */ 
		} /* if */ 
	} /* for */ 

	for(i=2;i<int((persistence*BULLET_SPEED)/0.5)+2 && (y+i<map_h*2);i++) {
		if (discreetmap[x+(y+i)*(map_w*2)]==T_BUILDING ||
			discreetmap[(x+1)+(y+i)*(map_w*2)]==T_BUILDING) break;
		if (player==1) {
			if (discreetmap[x+(y+i)*(map_w*2)]==T_ROBOT ||
				discreetmap[(x+1)+(y+i)*(map_w*2)]==T_ROBOT) break;
			if (discreetmap[x+(y+i)*(map_w*2)]==T_EROBOT ||
				discreetmap[(x+1)+(y+i)*(map_w*2)]==T_EROBOT) {
				rsp|=2;
				break;
			} /* if */ 
		} else {
			if (discreetmap[x+(y+i)*(map_w*2)]==T_EROBOT ||
				discreetmap[(x+1)+(y+i)*(map_w*2)]==T_EROBOT) break;
			if (discreetmap[x+(y+i)*(map_w*2)]==T_ROBOT ||
				discreetmap[(x+1)+(y+i)*(map_w*2)]==T_ROBOT) {
				rsp|=2;
				break;
			} /* if */ 
		} /* if */ 
	} /* for */ 

	for(i=1;i<int((persistence*BULLET_SPEED)/0.5)+1 && (y-i>=0);i++) {
		if (discreetmap[x+(y-i)*(map_w*2)]==T_BUILDING ||
			discreetmap[(x+1)+(y-i)*(map_w*2)]==T_BUILDING) break;
		if (player==1) {
			if (discreetmap[x+(y-i)*(map_w*2)]==T_ROBOT ||
				discreetmap[(x+1)+(y-i)*(map_w*2)]==T_ROBOT) break;
			if (discreetmap[x+(y-i)*(map_w*2)]==T_EROBOT ||
				discreetmap[(x+1)+(y-i)*(map_w*2)]==T_EROBOT) {
				rsp|=8;
				break;
			} /* if */ 
		} else {
			if (discreetmap[x+(y-i)*(map_w*2)]==T_EROBOT ||
				discreetmap[(x+1)+(y-i)*(map_w*2)]==T_EROBOT) break;
			if (discreetmap[x+(y-i)*(map_w*2)]==T_ROBOT ||
				discreetmap[(x+1)+(y-i)*(map_w*2)]==T_ROBOT) {
				rsp|=8;
				break;
			} /* if */ 
		} /* if */ 
	} /* for */ 

	return rsp;
} /* NETHER::AI_RealShotPaths */ 


int NETHER::AI_program_destroy(int goal,Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces)
{
	/* First of all, delete the robot from the discreet map: */ 
	int op=ROBOTOP_NONE;
	int type=AI_killrobot(pos);
	List<AI_OPERATOR> lops;

	AI_availableoperators(pos,angle,traction,&lops);

	if (!lops.EmptyP()) {
		/* Choose one operator: */ 
		AI_OPERATOR *aiop;

		if (goal!=P_PARAM_ROBOTS) {
			/* Seek a goal: */ 
			List<BUILDING> l;
			BUILDING *b;
			bool anygoal=false;
			float distance,minimumdistance;

			*program_goal=Vector(-1,-1,-1);

			l.Instance(buildings);
			l.Rewind();
			while(l.Iterate(b)) {
				if (goal==P_PARAM_WARBASES && 
					b->type==B_WARBASE && b->owner!=player &&
					AI_WorseMapTerrain(int((b->pos.x+2.0)/0.5),int(b->pos.y/0.5),2,2)<=T_HOLE) {
					distance=float(((b->pos+Vector(2.5,0.5,0))-pos).norma());
					if (!anygoal || distance<minimumdistance) {
						anygoal=true;
						minimumdistance=distance;
						*program_goal=b->pos+Vector(2.5,0.5,0);
					} /* if */ 
				} /* if */ 
				if (goal==P_PARAM_EFACTORIES && 
					(b->type==B_FACTORY_ELECTRONICS ||
					 b->type==B_FACTORY_NUCLEAR ||
					 b->type==B_FACTORY_PHASERS ||
					 b->type==B_FACTORY_MISSILES ||
					 b->type==B_FACTORY_CANNONS ||
					 b->type==B_FACTORY_CHASSIS) && b->owner!=0 && b->owner!=player &&
					AI_WorseMapTerrain(int((b->pos.x+1.0)/0.5),int(b->pos.y/0.5),2,2)<=T_HOLE) {
					distance=float(((b->pos+Vector(1.5,0.5,0))-pos).norma());
					if (!anygoal || distance<minimumdistance) {
						anygoal=true;
						minimumdistance=distance;
						*program_goal=b->pos+Vector(1.5,0.5,0);
					} /* if */ 
				} /* if */ 
			} /* while */ 

			if (program_goal->x!=-1 &&
				(*program_goal)!=pos) {
				if (electronics) {
					op=AI_searchengine(pos,angle,PROGRAM_CAPTURE,*program_goal,traction,WE_SEARCH_DEPTH);
				} else {
					if ((rand()%4)!=0) {
						op=AI_searchengine(pos,angle,PROGRAM_CAPTURE,*program_goal,traction,WOE_SEARCH_DEPTH);
					} else {
						AI_rankoperators_capture(&lops,*program_goal);
						aiop=AI_chooseoperator(&lops,8);
						op=aiop->first_robotop;
					} /* if */ 
				} /* if */ 
			} else {
				if (program_goal->x!=-1) op=ROBOTOP_NUCLEAR;
			} /* if */ 

		} else {
			/* Find the nearest position to destroy an enemy robot: */ 
			List<ROBOT> l;
			ROBOT *r;
			int x,y,dx,dy,i,j,k;
			bool collided;
			bool first=true;
			float distance;
			int persistence=CANNON_PERSISTENCE;
			if (pieces[1]) persistence=MISSILE_PERSISTENCE;

			*program_goal=Vector(-1,-1,-1);
			for(i=0;i<map_w*2*map_h*2;i++) atackmap[i]=0;

			/* Find the nearest FIRE position: */ 
			l.Instance(robots[2-player]);
			l.Rewind();
			while(l.Iterate(r)) {
				if (first ||
					(*program_goal-pos).norma()<distance) {
					first=false;
					distance=float((*program_goal-pos).norma());
					*program_goal=r->pos;
				} /* if */ 

				robot_zone(r->pos,&x,&y,&dx,&dy);
				for(i=0;i<dx;i++) {
					for(j=0;j<dy;j++) {
						collided=false;
						for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
							if (x+i+k<0 || x+i+k>=map_w*2 ||
								y+j<0 || y+j>=map_h*2 ||
								discreetmap[(y+j)*(map_w*2)+(x+i+k)]>3) {
								collided=true;
							} else {
								atackmap[(y+j)*(map_w*2)+(x+i+k)]|=4;
							} /* if */ 
						} /* for */ 

						collided=false;
						for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
							if (x+i-k<0 || x+i-k>=map_w*2 ||
								y+j<0 || y+j>=map_h*2 ||
								discreetmap[(y+j)*(map_w*2)+(x+i-k)]>3) {
								collided=true;
							} else {
								atackmap[(y+j)*(map_w*2)+(x+i-k)]|=1;
							} /* if */ 
						} /* for */ 

						collided=false;
						for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
							if (x+i<0 || x+i>=map_w*2 ||
								y+j+k<0 || y+j+k>=map_h*2 ||
								discreetmap[(y+j+k)*(map_w*2)+(x+i)]>3) {
								collided=true;
							} else {
								atackmap[(y+j+k)*(map_w*2)+(x+i)]|=8;
							} /* if */ 
						} /* for */ 

						collided=false;
						for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
							if (x+i<0 || x+i>=map_w*2 ||
								y+j-k<0 || y+j-k>=map_h*2 ||
								discreetmap[(y+j-k)*(map_w*2)+(x+i)]>3) {
								collided=true;
							} else {
								atackmap[(y+j-k)*(map_w*2)+(x+i)]|=2;
							} /* if */ 
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* while */ 

			if (!first) {
				robot_zone(pos,&x,&y,&dx,&dy);
				if ((atackmap[y*(map_w*2)+x]!=0 ||
					atackmap[(y+1)*(map_w*2)+x]!=0 ||
					atackmap[y*(map_w*2)+x+1]!=0 ||
					atackmap[(y+1)*(map_w*2)+x+1]!=0)) {
					int prsp=0,mrsp=0,crsp=0,rsp=0;
					if (pieces[2]) prsp=AI_RealShotPaths(x,y,player,PHASER_PERSISTENCE);
					if (pieces[1]) mrsp=AI_RealShotPaths(x,y,player,MISSILE_PERSISTENCE);
					if (pieces[0]) crsp=AI_RealShotPaths(x,y,player,CANNON_PERSISTENCE);
					rsp=prsp|mrsp|crsp;
					
					if (rsp!=0) {
						int dirmask=0;
						if (angle==0) dirmask=1;
						if (angle==90) dirmask=2;
						if (angle==180) dirmask=4;
						if (angle==270) dirmask=8;
						if ((rsp&dirmask)!=0) {
							if ((prsp&dirmask)!=0) {
								op=ROBOTOP_PHASERS;
							} else {
								if ((mrsp&dirmask)!=0) {
									op=ROBOTOP_MISSILES;
								} else {
									op=ROBOTOP_CANNONS;
								} /* if */ 
							} /* if */ 
						} else {
							dirmask*=2;
							if (dirmask>=16) dirmask=1;
							if ((rsp&dirmask)!=0) {
								op=ROBOTOP_RIGHT;
							} else {
								op=ROBOTOP_LEFT;
							} /* if */ 
						} /* if */ 
					} else {
						if (electronics) {
							op=AI_searchengine(pos,angle,PROGRAM_DESTROY,*program_goal,traction,WE_SEARCH_DEPTH);
						} else {
							if ((rand()%4)!=0) {
								op=AI_searchengine(pos,angle,PROGRAM_DESTROY,*program_goal,traction,WOE_SEARCH_DEPTH);
							} else {
								AI_rankoperators_capture(&lops,*program_goal);
								aiop=AI_chooseoperator(&lops,8);
								op=aiop->first_robotop;
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} else {
					if (electronics) {
						op=AI_searchengine(pos,angle,PROGRAM_DESTROY,*program_goal,traction,WE_SEARCH_DEPTH);
					} else {
						if ((rand()%4)!=0) {
							op=AI_searchengine(pos,angle,PROGRAM_DESTROY,*program_goal,traction,WOE_SEARCH_DEPTH);
						} else {
							AI_rankoperators_capture(&lops,*program_goal);
							aiop=AI_chooseoperator(&lops,8);
							op=aiop->first_robotop;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} else {
				/* There are no enemy robots: */ 
				op=ROBOTOP_NONE;
			} /* if */ 
		} /* if */ 
	} /* if */ 

	/* Reconstruct the decreet map: */ 
	if (type==T_ROBOT) AI_newrobot(pos,0);
				  else AI_newrobot(pos,1);

	return op;
} /* NETHER::AI_program_destroy */ 


int NETHER::AI_program_stopdefend(Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces)
{
	/* First of all, delete the robot from the discreet map: */ 
	int op=ROBOTOP_NONE;
	int type=AI_killrobot(pos);
	List<AI_OPERATOR> lops;

	AI_availableoperators(pos,angle,traction,&lops);

	if (!lops.EmptyP()) {
		/* Choose one operator: */ 

		/* Find the nearest position to destroy an enemy robot: */ 
		List<ROBOT> l;
		ROBOT *r;
		int x,y,dx,dy,i,j,k;
		bool collided;
		int persistence=CANNON_PERSISTENCE;
		if (pieces[1]) persistence=MISSILE_PERSISTENCE;

		*program_goal=Vector(-1,-1,-1);
		for(i=0;i<map_w*2*map_h*2;i++) atackmap[i]=0;

		/* Find the nearest FIRE position: */ 
		l.Instance(robots[2-player]);
		l.Rewind();
		while(l.Iterate(r)) {
			robot_zone(r->pos,&x,&y,&dx,&dy);
			for(i=0;i<dx;i++) {
				for(j=0;j<dy;j++) {
					collided=false;
					for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
						if (x+i+k<0 || x+i+k>=map_w*2 ||
							y+j<0 || y+j>=map_h*2 ||
							discreetmap[(y+j)*(map_w*2)+(x+i+k)]>3) {
							collided=true;
						} else {
							atackmap[(y+j)*(map_w*2)+(x+i+k)]|=4;
						} /* if */ 
					} /* for */ 

					collided=false;
					for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
						if (x+i-k<0 || x+i-k>=map_w*2 ||
							y+j<0 || y+j>=map_h*2 ||
							discreetmap[(y+j)*(map_w*2)+(x+i-k)]>3) {
							collided=true;
						} else {
							atackmap[(y+j)*(map_w*2)+(x+i-k)]|=1;
						} /* if */ 
					} /* for */ 

					collided=false;
					for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
						if (x+i<0 || x+i>=map_w*2 ||
							y+j+k<0 || y+j+k>=map_h*2 ||
							discreetmap[(y+j+k)*(map_w*2)+(x+i)]>3) {
							collided=true;
						} else {
							atackmap[(y+j+k)*(map_w*2)+(x+i)]|=8;
						} /* if */ 
					} /* for */ 

					collided=false;
					for(k=1;!collided && k<int((persistence*BULLET_SPEED)/0.5);k++) {
						if (x+i<0 || x+i>=map_w*2 ||
							y+j-k<0 || y+j-k>=map_h*2 ||
							discreetmap[(y+j-k)*(map_w*2)+(x+i)]>3) {
							collided=true;
						} else {
							atackmap[(y+j-k)*(map_w*2)+(x+i)]|=2;
						} /* if */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* while */ 


		if (!robots[2-player].EmptyP()) {
			robot_zone(pos,&x,&y,&dx,&dy);
			if ((atackmap[y*(map_w*2)+x]!=0 ||
				atackmap[(y+1)*(map_w*2)+x]!=0 ||
				atackmap[y*(map_w*2)+x+1]!=0 ||
				atackmap[(y+1)*(map_w*2)+x+1]!=0)) {
				int prsp=0,mrsp=0,crsp=0,rsp=0;
				if (pieces[2]) prsp=AI_RealShotPaths(x,y,player,PHASER_PERSISTENCE);
				if (pieces[1]) mrsp=AI_RealShotPaths(x,y,player,MISSILE_PERSISTENCE);
				if (pieces[0]) crsp=AI_RealShotPaths(x,y,player,CANNON_PERSISTENCE);
				rsp=prsp|mrsp|crsp;
				
				if (rsp!=0) {
					int dirmask=0;

					/* This is just to make the program not to think that the robot doesn't have any goal: */ 
					*program_goal=Vector(0,0,0);	

					if (angle==0) dirmask=1;
					if (angle==90) dirmask=2;
					if (angle==180) dirmask=4;
					if (angle==270) dirmask=8;
					if ((rsp&dirmask)!=0) {
						if ((prsp&dirmask)!=0) {
							op=ROBOTOP_PHASERS;
						} else {
							if ((mrsp&dirmask)!=0) {
								op=ROBOTOP_MISSILES;
							} else {
								op=ROBOTOP_CANNONS;
							} /* if */ 
						} /* if */ 
					} else {
						dirmask*=2;
						if (dirmask>=16) dirmask=1;
						if ((rsp&dirmask)!=0) {
							op=ROBOTOP_RIGHT;
						} else {
							op=ROBOTOP_LEFT;
						} /* if */ 
					} /* if */ 
				} else {
					/* There are no enemy robots at sight: */ 
					op=ROBOTOP_NONE;
				} /* if */ 
			} else {
				/* There are no enemy robots at sight: */ 
				op=ROBOTOP_NONE;
			} /* if */ 
		} else {
			/* There are no enemy robots: */ 
			op=ROBOTOP_NONE;
		} /* if */ 

	} /* if */ 

	/* Reconstruct the decreet map: */ 
	if (type==T_ROBOT) AI_newrobot(pos,0);
				  else AI_newrobot(pos,1);

	return op;
} /* NETHER::AI_program_stopdefend */ 


void NETHER::AI_rankoperators_advance(List<AI_OPERATOR> *l)
{
	AI_OPERATOR *op1,*op2;
	LLink<AI_OPERATOR> *p1,*p2;
	bool changes;
	
	/* Bubble sort: */ 
	do {
		changes=false;
		p1=p2=0;
		l->Rewind();
		while(!l->EndP()) {
			p1=p2;
			p2=l->GetPos();
			if (p1!=0 && p2!=0) {
				op1=p1->GetObj();
				op2=p2->GetObj();
				if ((op2->newpos.y>op1->newpos.y) ||
					(op2->newpos.y==op1->newpos.y && op2->cost<op1->cost)) {
					p1->Setobj(op2);
					p2->Setobj(op1);
					changes=true;
				} /* if */ 
			} /* if */ 
			l->Next();
		} /* while */ 
	}while(changes);
	
} /* NETHER::AI_rankoperators_advance */ 


void NETHER::AI_rankoperators_retreat(List<AI_OPERATOR> *l)
{
	AI_OPERATOR *op1,*op2;
	LLink<AI_OPERATOR> *p1,*p2;
	bool changes;
	
	/* Bubble sort: */ 
	do {
		changes=false;
		p1=p2=0;
		l->Rewind();
		while(!l->EndP()) {
			p1=p2;
			p2=l->GetPos();
			if (p1!=0 && p2!=0) {
				op1=p1->GetObj();
				op2=p2->GetObj();
				if ((op2->newpos.y<op1->newpos.y) ||
					(op2->newpos.y==op1->newpos.y && op2->cost<op1->cost)) {
					p1->Setobj(op2);
					p2->Setobj(op1);
					changes=true;
				} /* if */ 
			} /* if */ 
			l->Next();
		} /* while */ 
	}while(changes);
	
} /* NETHER::AI_rankoperators_retreat */ 


void NETHER::AI_rankoperators_capture(List<AI_OPERATOR> *l,Vector goal)
{
	AI_OPERATOR *op1,*op2;
	LLink<AI_OPERATOR> *p1,*p2;
	bool changes;
	float dist1 = 0;
	float dist2 = 0;
	int c1,c2;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Rank operators START\n");
	fflush(debug_fp);
#endif

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Rank operators START\n");
	fflush(debug_fp);
#endif
	
	/* Bubble sort: */ 
	do {
		changes=false;
		p1=p2=0;
		l->Rewind();
		while(!l->EndP()) {
			p1=p2;
			p2=l->GetPos();
			if (p1!=0 && p2!=0) {
				op1=p1->GetObj();
				op2=p2->GetObj();
				dist1 = (op1->newpos-goal).norma();
				dist2 = (op2->newpos-goal).norma();
				c1=(0.1)<(dist1-dist2);
				c2=0;
				if ((0.1)>(dist1-dist2) &&
					(-0.1)<(dist1-dist2)) c2=op2->cost<op1->cost;
				if (c1 || c2) {

#ifdef _WRITE_REPORT_
		fprintf(debug_fp,"Change: (%.16f,%i) <> (%.16f,%i) %i %i\n",dist1,op1->cost,dist2,op2->cost,c1,c2);
		fflush(debug_fp);
#endif
					p1->Setobj(op2);
					p2->Setobj(op1);
					changes=true;
				} /* if */ 
			} /* if */ 
			l->Next();
		} /* while */ 
	}while(changes);

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Rank operators FINISHED\n");
	fflush(debug_fp);
#endif

} /* NETHER::AI_rankoperators_capture */ 


AI_OPERATOR *NETHER::AI_chooseoperator(List<AI_OPERATOR> *l,int factor)
{
	if (factor==0) {
		l->Rewind();
		return l->GetObj();
	} else {
		l->Rewind();
		while(!l->LastP() && (rand()%factor)==0) l->Next();
		return l->GetObj();
	} /* if */ 
} /* NETHER::AI_chooseoperator */ 


int  NETHER::AI_robothere(Vector pos)
{
	int x,y;
	int robot;
	
	x=int(pos.x/0.5);
	y=int(pos.y/0.5);
	
	robot=discreetmap[y*(map_w*2)+x];

	if (robot!=T_ROBOT && robot!=T_EROBOT) robot=0;

	return robot;
} /* AI_robothere */ 


