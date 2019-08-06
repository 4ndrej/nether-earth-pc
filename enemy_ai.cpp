#ifdef _WIN32
#include "windows.h"
#endif

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
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

//FILE *fp;
extern int level;

/* Only 1 action per turn will be allowed: */ 

void NETHER::AI_enemy(void)
{
	BUILDING *in_danger_warbase=0;
	int state=AI_STATE_EXPANDING;
	int nrobots[3]={0,0,0};	/* EXPANDING / ATTACING / DEFENDING */ 
	Vector mean_factory_position;
	BUILDING *closest_to_factories_warbase=0;
	float distance_to_factories;
	BUILDING *closest_to_enemy_warbase=0;
	float distance_to_enemy;
	int factories[3]={0,0,0};

//	if (fp==0) {
//		fp=fopen("enemyai.txt","w+");
//	} /* if */  

//	fprintf(fp,"---------------------------------- \n");
//	fprintf(fp,"G: %i\nE: %i\nN: %i\nP: %i\nM: %i\nC: %i\nC: %i\n",
//			resources[1][0],resources[1][1],resources[1][2],
//			resources[1][3],resources[1][4],resources[1][5],resources[1][6]);

	/* Analize the situation and determine strategy: */ 
	/*
	AI_STATE_EXPANDING
	AI_STATE_FIGHTING
	AI_STATE_DEFENDING
	AI_STATE_CONQUERING
	*/ 
	/* Rules: */ 
	/*
	1.- If there is a robot force near a WARBASE (closer than 10 squares) -> state = AI_STATE_DEFENDING
	2.- If there is no WARBASE in danger, and there are available FACTORIES near -> state = AI_STATE_EXPANDING
	2.- If there is no WARBASE in danger and there are enough CAPTURING robots working and
		there are more player robots than computer robots -> state = AI_FIGHTING
	4.- If there is no WARBASE in danger and there are enough CAPTURING robots working and
		there are less player robots than computer robots -> state = AI_CONQUERING
	*/ 

	ROBOT *tmpr;	// To test is the entrance to Warbases is free

	tmpr=new ROBOT();
	tmpr->traction=0;
	tmpr->pieces[0]=true;
	tmpr->pieces[1]=false;
	tmpr->pieces[2]=false;
	tmpr->pieces[3]=false;
	tmpr->pieces[4]=false;
	tmpr->angle=0;
	tmpr->program=PROGRAM_FORWARD;
	tmpr->op=ROBOTOP_NONE;
	tmpr->cmc=RobotCMC(tmpr,1);
	tmpr->shipover=false;


	{
		List<BUILDING> l;
		BUILDING *b;
		List<ROBOT> rl;
		ROBOT *r;
		int i;
		int forces[2]={0,0};

		l.Instance(buildings);
		l.Rewind();
		mean_factory_position=Vector(0,0,0);
		while(l.Iterate(b)) {
			if (b->type==B_FACTORY_ELECTRONICS ||
				b->type==B_FACTORY_NUCLEAR ||
				b->type==B_FACTORY_PHASERS ||
				b->type==B_FACTORY_MISSILES ||
				b->type==B_FACTORY_CANNONS) {
				factories[b->owner]++;
				if (b->owner!=2) mean_factory_position=mean_factory_position+b->pos;
			} /* if */ 
		} /* while */ 
		mean_factory_position=mean_factory_position/(factories[0]+factories[1]);

//		fprintf(fp,"Factories: %i %i %i\n", factories[0],factories[1],factories[2]);

		l.Instance(buildings);
		l.Rewind();
		while(l.Iterate(b)) {
			if (b->type==B_WARBASE &&
				b->owner==2) {
				forces[0]=0;
				forces[1]=0;

				tmpr->pos=b->pos+Vector(2.5,0.5,0);
				if (!RobotCollision(tmpr,true)) {
					/* Find the closest WARBASE to the available FACTORIES: */ 
					if (closest_to_factories_warbase==0 ||
						(closest_to_factories_warbase->pos-b->pos).norma()<distance_to_factories) {
						closest_to_factories_warbase=b;
						distance_to_factories=float((closest_to_factories_warbase->pos-b->pos).norma());
					} /* if */ 

					/* Find the closest WARBASE to the enemy: */ 
					if (closest_to_enemy_warbase==0 ||
						closest_to_enemy_warbase->pos.y<distance_to_enemy) {
						closest_to_enemy_warbase=b;
						distance_to_enemy=float(closest_to_enemy_warbase->pos.y);
					} /* if */ 
				} /* if */ 

				/* Test for WARBASEs in danger: */ 
				for(i=0;i<2;i++) {
					rl.Instance(robots[i]);
					rl.Rewind();
					while(rl.Iterate(r)) {
						if ((r->pos-b->pos).norma()<10.0) {
							/* Robot near: */ 
							forces[i]+=RobotCost(r);

							if (i==1) {
								if (forces[0]>forces[1] && 
									(r->program!=PROGRAM_DESTROY ||
									 r->program_parameter!=P_PARAM_ROBOTS)) {
								
//									fprintf(fp,"Program of <%g,%g,%g> -> DESTROY ROBOTS\n",r->pos.x,r->pos.y,r->pos.z);

									r->program=PROGRAM_DESTROY;
									r->program_parameter=P_PARAM_ROBOTS;
									return;
								} /* if */ 
							} /* if */ 
						} /* if */ 
					} /* while */ 
				} /* for */ 
				if (forces[0]>forces[1]) {
					state=AI_STATE_DEFENDING;
					in_danger_warbase=b;
				} /* if */ 

			} /* if */ 
		} /* while */ 
	}


	/* If the warbase in danger id blocked, build robots from another warbase: */ 
	if (in_danger_warbase!=0) {
		tmpr->pos=in_danger_warbase->pos+Vector(2.0,0.5,0);
		if (RobotCollision(tmpr,true)) in_danger_warbase=closest_to_enemy_warbase;
	} /* if */ 

	delete tmpr;
	tmpr=0;

	/* If the enemy has no WARBASES, he cannot do anything: (???) */ 
	if (closest_to_factories_warbase==0) return;


	/* Count the number of robots: */ 
	{
		List<ROBOT> rl;
		ROBOT *r;
		
		rl.Instance(robots[1]);
		rl.Rewind();
		while(rl.Iterate(r)) {
			if (r->program==PROGRAM_CAPTURE) nrobots[0]++;
			if (r->program==PROGRAM_DESTROY) nrobots[1]++;
			if (r->program==PROGRAM_STOPDEFEND) nrobots[2]++;
		} /* while */ 
	}

	if (in_danger_warbase!=0 &&
		(level>=2 ||
		 (level==1 && (rand()%2)==0) ||
		 (level==0 && (rand()%4)==0))) {
		ROBOT *r;
		/* Try to build a ROBOT to defend the WARBASE */ 
		
//		fprintf(fp,"Trying to BUILD a robot to DEFEND the WARBASE \n");

		r=AI_enemy_newrobot(AI_STATE_DEFENDING,in_danger_warbase->pos+Vector(2.5,0.5,0));
		if (r!=0) {
			
//			fprintf(fp,"Achieved.\n");

			r->program=PROGRAM_DESTROY;
			r->program_parameter=P_PARAM_ROBOTS;
		} /* if */ 
	} else {
		if (nrobots[2]>0 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
			/* There are too many robots in STOP & DEFEND: */ 
			List<ROBOT> rl;
			ROBOT *r;
			
			rl.Instance(robots[1]);
			rl.Rewind();
			while(rl.Iterate(r)) {
				if (r->program==PROGRAM_STOPDEFEND) {
					if (nrobots[0]<6 && factories[2]<(factories[1]+factories[0]) && 
						(robots[0].Length()*2)<=nrobots[1]) {
						/* Convert the robot to a conquering one: */ 
						if (factories[1]>factories[0]) {
							r->program=PROGRAM_CAPTURE;
							r->program_parameter=P_PARAM_EFACTORIES;
							return;
						} else {
							r->program=PROGRAM_CAPTURE;
							r->program_parameter=P_PARAM_NFACTORIES;
							return;
						} /* if */ 
					} else {
						if ((robots[0].Length()*2)>nrobots[1]) {
							r->program=PROGRAM_DESTROY;
							r->program_parameter=P_PARAM_ROBOTS;
							return;
						} else {
							r->program=PROGRAM_CAPTURE;
							r->program_parameter=P_PARAM_WARBASES;
							return;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} /* while */ 
		} /* if */ 
		/* Test for near FACTORIES and CAPTURING ROBOTS: */ 
		if (nrobots[0]<6 && factories[2]<(factories[1]+factories[0]) &&
			(robots[0].Length()*2)<=nrobots[1]) {
			/* I need more conquering robots: */ 

			/* Try to make better robots as time passes: */ 
			switch(level) {
			case 0:
				if (nrobots[0]>=1 && resources[1][0]+resources[1][6]<20) return;
				if (nrobots[0]>=1 && resources[1][0]+resources[1][6]<25) return;
				if (nrobots[0]>=2 && resources[1][0]+resources[1][6]<30) return;
				break;
			case 1:
				if (nrobots[0]>=1 && resources[1][0]+resources[1][6]<20) return;
				if (nrobots[0]>=2 && resources[1][0]+resources[1][6]<25) return;
				if (nrobots[0]>=3 && resources[1][0]+resources[1][6]<30) return;
				break;
			default:
				if (nrobots[0]>=2 && resources[1][0]+resources[1][6]<20) return;
				if (nrobots[0]>=3 && resources[1][0]+resources[1][6]<25) return;
				if (nrobots[0]>=4 && resources[1][0]+resources[1][6]<30) return;
				break;
			} /* switch */  

			ROBOT *r=AI_enemy_newrobot(AI_STATE_EXPANDING,closest_to_factories_warbase->pos+Vector(2.5,0.5,0));
//			fprintf(fp,"Trying to BUILD a robot to CONQUER FACTORIES \n");
			if (r!=0) {
				if (factories[1]>factories[0]) {			
					
//					fprintf(fp,"Achieved, it will conquer ENEMY FACTORIES\n");

					r->program=PROGRAM_CAPTURE;
					r->program_parameter=P_PARAM_EFACTORIES;
				} else {

//					fprintf(fp,"Achieved, it will conquer NEUTRAL FACTORIES\n");

					r->program=PROGRAM_CAPTURE;
					r->program_parameter=P_PARAM_NFACTORIES;
				} /* if */ 
			} /* if */ 
		} else {
			/* I've enough conquering robots: */ 
			if (nrobots[1]>10) {
				/* I've enough attacking robots, let's build a conquering one: */ 
//				fprintf(fp,"Trying to BUILD a robot to CONQUER WARBASES \n");
				
				if (resources[1][0]+resources[1][6]<40) return;
				ROBOT *r=AI_enemy_newrobot(AI_STATE_CONQUERING,closest_to_enemy_warbase->pos+Vector(2.5,0.5,0));
				if (r!=0) {

//					fprintf(fp,"Achieved.\n");

					r->program=PROGRAM_CAPTURE;
					r->program_parameter=P_PARAM_WARBASES;
				} /* if */ 
			} else {
				/* I need more attacking robots: */ 
				
				/* Try to make better robots as time passes: */ 
				switch(level) {
				case 0:
					if (nrobots[1]>=1 && resources[1][0]+resources[1][6]<20) return;
					if (nrobots[1]>=1 && resources[1][0]+resources[1][6]<25) return;
					if (nrobots[1]>=2 && resources[1][0]+resources[1][6]<30) return;
					break;
				case 1:
					if (nrobots[1]>=1 && resources[1][0]+resources[1][6]<20) return;
					if (nrobots[1]>=2 && resources[1][0]+resources[1][6]<25) return;
					if (nrobots[1]>=3 && resources[1][0]+resources[1][6]<30) return;
					break;
				default:
					if (nrobots[1]>=2 && resources[1][0]+resources[1][6]<20) return;
					if (nrobots[1]>=3 && resources[1][0]+resources[1][6]<25) return;
					if (nrobots[1]>=4 && resources[1][0]+resources[1][6]<30) return;
					break;
				} /* switch */ 

				ROBOT *r=AI_enemy_newrobot(AI_STATE_FIGHTING,closest_to_enemy_warbase->pos+Vector(2.5,0.5,0));

//				fprintf(fp,"Trying to BUILD a robot to ATTACK ROBOTS \n");
				
				if (r!=0) {

//					fprintf(fp,"Achieved.\n");
					
					r->program=PROGRAM_DESTROY;
					r->program_parameter=P_PARAM_ROBOTS;
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} /* if */ 

} /* NETHER::AI_enemy */ 


ROBOT *NETHER::AI_enemy_newrobot(int state,Vector pos)
{
	int traction=0;
	bool pieces[5]={false,false,false,false,false};
	int rg=resources[1][R_GENERAL];

	if (rg+resources[1][R_CHASSIS]>30 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
		traction=2;
		rg-=10;
	} else {
		if (rg+resources[1][R_CHASSIS]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
			traction=1;
			rg-=5;
		} else {
			traction=0;
			rg-=2;
		} /* if */ 
	} /* if */ 

	switch(state) {
	case AI_STATE_EXPANDING:
		if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>40 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
			pieces[2]=true;
		} else {
			if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>20 &&
				(level>=2 ||
				(level==1 && (rand()%2)==0) ||
				(level==0 && (rand()%4)==0))) {
				pieces[1]=true;
			} else {
				pieces[0]=true;
			} /* if */ 
		} /* if */ 

		if (rg+resources[1][R_ELECTRONICS]>10) pieces[4]=true;
		break;
	case AI_STATE_DEFENDING:
		if (rg+resources[1][R_PHASERS]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[2]=true;
		if (rg+resources[1][R_MISSILES]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[1]=true;
		if (rg+resources[1][R_CANNONS]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[0]=true;
		if (!pieces[0] && !pieces[1] && !pieces[2]) pieces[0]=true;

		if (rg+resources[1][R_ELECTRONICS]>30) pieces[4]=true;
		break;
	case AI_STATE_FIGHTING:
		if (rg+resources[1][R_PHASERS]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[2]=true;
		if (rg+resources[1][R_MISSILES]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[1]=true;
		if (rg+resources[1][R_CANNONS]>20 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[0]=true;
		if (!pieces[0] && !pieces[1] && !pieces[2]) pieces[0]=true;

		if (rg+resources[1][R_ELECTRONICS]>20) pieces[4]=true;
		break;
	case AI_STATE_CONQUERING:
		if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>40 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
			pieces[2]=true;
		} else {
			if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>20 &&
				(level>=2 ||
				(level==1 && (rand()%2)==0) ||
				(level==0 && (rand()%4)==0))) {
				pieces[1]=true;
			} else {
				pieces[0]=true;
			} /* if */ 
		} /* if */ 

		if (rg+resources[1][R_ELECTRONICS]>10) pieces[4]=true;
		break;
	case AI_STATE_DESTROYING:
		if (rg+resources[1][R_NUCLEAR]>40) pieces[3]=true;
		if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>40 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) {
			pieces[2]=true;
		} else {
			if (rg+resources[1][R_CANNONS]+resources[1][R_MISSILES]+resources[1][R_PHASERS]>20 &&
				(level>=2 ||
				(level==1 && (rand()%2)==0) ||
				(level==0 && (rand()%4)==0))) {
				pieces[1]=true;
			} else {
				pieces[0]=true;
			} /* if */ 
		} /* if */ 

		if (rg+resources[1][R_ELECTRONICS]>10 &&
			(level>=2 ||
			(level==1 && (rand()%2)==0) ||
			(level==0 && (rand()%4)==0))) pieces[4]=true;
		break;
	} /* switch */ 

	/* Build the robot: */ 
	{
		int i;
		ROBOT *r;
		int cost[7];

		r=new ROBOT();
		r->traction=traction;
		r->pieces[0]=pieces[0];
		r->pieces[1]=pieces[1];
		r->pieces[2]=pieces[2];
		r->pieces[3]=pieces[3];
		r->pieces[4]=pieces[4];
		RobotCost(1,r,cost);

		for(i=0;i<7;i++) {
			if (resources[1][i]<cost[i]) {
				/* Not enough resources! */ 
				return 0;
			} /* if */ 
		} /* for */ 


		/* Valid robot, build it: */ 
		r->pos=pos;
		r->angle=0;
		r->program=PROGRAM_FORWARD;
		r->op=ROBOTOP_NONE;
		r->cmc=RobotCMC(r,1);
		r->shipover=false;

		if (!RobotCollision(r,true)) {
			robots[1].Add(r);
			AI_newrobot(r->pos,0);

			for(i=0;i<7;i++) resources[1][i]-=cost[i];

			return r;
		} else {
			delete r;
		} /* if */ 
	}

	return 0;

} /* AI_enemy_newrobot */ 


