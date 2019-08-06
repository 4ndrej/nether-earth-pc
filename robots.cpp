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


const float ms[4][3]={{0.0078125,0.015625,0.03125},
					{0.00390625,0.0078125,0.03125},
					{0,0.0078125,0.015625},
					{0,0,0.03125}};

const int rs[4][3]={{2,3,5},
					{1,2,5},
					{0,2,3},
					{0,0,5}};
extern int detaillevel;

ROBOT::ROBOT() 
{
	traction=-1;
	firetimer=0;
	strength=100;
	pieces[0]=false;
	pieces[1]=false;
	pieces[2]=false;
	pieces[3]=false;
	pieces[4]=false;

	electronics_state=0;
	chassis_state=0;
} /* ROBOT::ROBOT */ 


bool ROBOT::valid(void)
{
	if (traction==-1) return false;
	if (pieces[0]==false &&
		pieces[1]==false &&
		pieces[2]==false &&
		pieces[3]==false) return false;
	return true;
} /* ROBOT::valid */ 


CMC NETHER::RobotCMC(ROBOT *r,int owner)
{
	CMC cmc;
	float m[16]={1,0,0,0,
				 0,1,0,0,
				 0,0,1,0,
				 0,0,0,1};

	switch(r->traction) {
	case 0:
		cmc=piece_tile[owner][0]->cmc;
		m[14]=1.0;
		break;
	case 1:
		cmc=piece_tile[owner][1]->cmc;
		m[14]=0.35;
		break;
	case 2:
		cmc=piece_tile[owner][2]->cmc;
		m[14]=0.45;
		break;
	} /* switch */ 

	if (r->pieces[0]) {
		cmc.expand(&(piece_tile[owner][3]->cmc),m);
		m[14]+=0.5;
	} /* if */ 
	if (r->pieces[1]) {
		cmc.expand(&(piece_tile[owner][4]->cmc),m);
		m[14]+=0.35;
	} /* if */ 
	if (r->pieces[2]) {
		cmc.expand(&(piece_tile[owner][5]->cmc),m);
		m[14]+=0.5;
	} /* if */ 
	if (r->pieces[3]) {
		cmc.expand(&(piece_tile[owner][6]->cmc),m);
		m[14]+=0.8;
	} /* if */ 
	if (r->pieces[4]) {
		cmc.expand(&(piece_tile[owner][7]->cmc),m);
	} /* if */ 

	return cmc;
} /* ROBOT::buildCMC */ 




void NETHER::DrawRobot(ROBOT *robot,int owner,bool shadows)
{
	float r[2]={0.9,0.4};
	float g[2]={0.9,0.4};
	float b[2]={0.9,0.4};
	float z=0;
	float bipod_v=0;
	Vector light;

	light=lightposv;
	light=light/light.z;

	glPushMatrix();
	switch(robot->traction) {
	case 0:
		if (robot->chassis_state>=0) {
			if (robot->chassis_state<32) {
				bipod_v=robot->chassis_state*0.00390625;
			} else {
				bipod_v=(64-robot->chassis_state)*0.00390625;
			} /* if */ 
		} else {
			if (robot->chassis_state>-31) {
				bipod_v=robot->chassis_state*0.00390625;
			} else {
				bipod_v=(-63-robot->chassis_state)*0.00390625;
			} /* if */ 
		} /* if */ 
		if (!shadows) {
			glPushMatrix();
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][8]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][8]->draw_notexture(r[owner],g[owner],b[owner]);
			glPushMatrix();
			glTranslatef(bipod_v,0,0);
			if (detaillevel>=3) piece_tile[owner][9]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][9]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-bipod_v,0,0);
			if (detaillevel>=3) piece_tile[owner][10]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][10]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
//			piece_tile[owner][0]->draw(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][8]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPushMatrix();
			glTranslatef(bipod_v,0,0);
			piece_tile[owner][9]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-bipod_v,0,0);			
			piece_tile[owner][10]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
//			piece_tile[owner][0]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=1.0;
		break;
	case 1:
		if (!shadows) {
			glPushMatrix();
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][1]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][1]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][1]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);					  
			glPopMatrix();
		} /* if */ 
		z+=0.35;
		break;
	case 2:
		z+=(cos(robot->chassis_state/15.0)*0.1)-0.1;
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][2]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][2]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][2]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=0.45;
		break;
	} /* switch */ 

	if (robot->pieces[0]) {
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][3]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][3]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][3]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=0.5;
	} /* if */ 
	if (robot->pieces[1]) {
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][4]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][4]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][4]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=0.35;
	} /* if */ 
	if (robot->pieces[2]) {
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][5]->draw(r[owner],g[owner],b[owner]);
						    else piece_tile[owner][5]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][5]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=0.5;
	} /* if */ 
	if (robot->pieces[3]) {
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (detaillevel>=3) piece_tile[owner][6]->draw(r[owner],g[owner],b[owner]);
					        else piece_tile[owner][6]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle,0,0,1);
			piece_tile[owner][6]->DrawShadow(robot->angle,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
		z+=0.8;
	} /* if */ 
	if (robot->pieces[4]) {
		if (!shadows) {
			glPushMatrix();
			glTranslatef(0,0,z);
			glRotatef(robot->angle,0,0,1);
			if (!robot->pieces[3]) glTranslatef(-0.2,0,0);
			glRotatef(robot->electronics_state,0,0,1);
			if (detaillevel>=3) piece_tile[owner][7]->draw(r[owner],g[owner],b[owner]);
					 	    else piece_tile[owner][7]->draw_notexture(r[owner],g[owner],b[owner]);
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(-z*light.x,-z*light.y,0.05);
			glRotatef(robot->angle+robot->electronics_state,0,0,1);
			piece_tile[owner][7]->DrawShadow(robot->angle+robot->electronics_state,lightposv,0,0,0,0.5);
			glPopMatrix();
		} /* if */ 
	} /* if */ 
	glPopMatrix();

} /* NETHER::DrawRobot */ 




float NETHER::RobotSpeed(int traction,int terrain)
{
	if (terrain<4 && traction<3) return ms[terrain][traction];

/*	switch(terrain) {
	case T_GRASS:
		if (traction==0) return 0.0078125;
		if (traction==1) return 0.015625;
		return 0.03125;
		break;
	case T_SAND:
		if (traction==0) return 0.00390625;
		if (traction==1) return 0.0078125;
		return 0.03125;
		break;
	case T_MOUNTAINS:
		if (traction==0) return 0;
		if (traction==1) return 0.0078125;
		return 0.015625;
		break;
	case T_HOLE:
		if (traction==0) return 0;
		if (traction==1) return 0;
		return 0.03125;
		break;
	} /* switch */ 
	return 0;
} /* NETHER::RobotSpeed */ 


int NETHER::RobotRotationSpeed(int traction,int terrain)
{
	if (terrain<4 && traction<3) return rs[terrain][traction];

/*	switch(terrain) {
	case T_GRASS:
		if (traction==0) return 2;
		if (traction==1) return 3;
		return 5;
		break;
	case T_SAND:
		if (traction==0) return 1;
		if (traction==1) return 2;
		return 5;
		break;
	case T_MOUNTAINS:
		if (traction==0) return 0;
		if (traction==1) return 2;
		return 3;
		break;
	case T_HOLE:
		if (traction==0) return 0;
		if (traction==1) return 0;
		return 5;
		break;
	} /* switch */ 
	return 0;
} /* NETHER::RobotRotationSpeed */ 


bool NETHER::Walkable(int traction,int terrain)
{
	if (RobotSpeed(traction,terrain)!=0) return true;

	return false;
} /* NETHER::Walkable */ 



float ROBOT::piecez(int piece)
{
	float z=0;
	switch(traction) {
	case 0:
		z+=1.0;
		break;
	case 1:
		z+=0.35;
		break;
	case 2:
		z+=0.25;
		break;
	} /* switch */ 

	/* CANON; */ 
	if (pieces[0]) {
		if (piece==0) return z;
		z+=0.5;
	} /* if */ 
	/* MISSILES: */ 
	if (pieces[1]) {
		if (piece==1) return z;
		z+=0.35;
	} /* if */ 
	/* PHASER: */ 
	if (pieces[2]) {
		if (piece==2) return z;
		z+=0.5;
	} /* if */ 
	/* NUCLEAR: */ 
	if (pieces[3]) {
		if (piece==3) return z;
		z+=0.8;
	} /* if */ 

	return z;
} /* ROBOT::piecez */ 


bool NETHER::RobotCollision(ROBOT *r,bool complete)
{
	int i;
	List<BUILDING> l;
	BUILDING *b;
	List<ROBOT> l2;
	ROBOT *rt;
	float m1[16]={1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  r->pos.x,r->pos.y,r->pos.z,1};
	float m2[16]={1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  0,0,0,1}; 

	/* Collision with the ship: */ 
	{
		float m2[16]={1,0,0,0,
				 	  0,1,0,0,
					  0,0,1,0,
					  shipp.x,shipp.y,shipp.z,1}; 
		if (r->cmc.collision_simple(m1,&(ship->cmc),m2)) return true;
	}

	if (!complete) return false;

	/* Collision with buildings: */  
	l.Instance(buildings);
	l.Rewind();
	while(l.Iterate(b)) {
		if (((b->pos.x-r->pos.x)*(b->pos.x-r->pos.x)+
			 (b->pos.y-r->pos.y)*(b->pos.y-r->pos.y)+
			 (b->pos.z-r->pos.z)*(b->pos.z-r->pos.z))<COLISION_TEST_THRESHOLD) {
			m2[12]=b->pos.x;
			m2[13]=b->pos.y;
			m2[14]=b->pos.z;

			switch(b->type) {
				case B_FENCE:
					if (r->cmc.collision_simple(m1,&(building_tile[5]->cmc),m2)) return true;
					break;
				case B_WALL1:
					if (r->cmc.collision_simple(m1,&(building_tile[0]->cmc),m2)) return true;
					break;
				case B_WALL2:
					if (r->cmc.collision_simple(m1,&(building_tile[1]->cmc),m2)) return true;
					break;
				case B_WALL3:
					if (r->cmc.collision_simple(m1,&(building_tile[2]->cmc),m2)) return true;
					break;
				case B_WALL4:
					if (r->cmc.collision_simple(m1,&(building_tile[3]->cmc),m2)) return true;
					break;
				case B_WALL5:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					break;
				case B_WALL6:
					if (r->cmc.collision_simple(m1,&(building_tile[7]->cmc),m2)) return true;
					break;
				case B_WARBASE:
					if (r->cmc.collision_simple(m1,&(building_tile[8]->cmc),m2)) return true;
					//m2[13]=b->pos.y-2;
					//m2[14]=b->pos.z+1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_ELECTRONICS:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][7]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_NUCLEAR:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][6]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_PHASERS:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][5]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_MISSILES:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][4]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_CANNONS:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][3]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_CHASSIS:
					if (r->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (r->cmc.collision_simple(m1,&(piece_tile[0][1]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (r->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			} /* switch */ 
			} /* if */ 
	} /* while */ 

	/* Collision with the robots: */ 
	for(i=0;i<2;i++) {
		l2.Instance(robots[i]);
		l2.Rewind();
		while(l2.Iterate(rt)) {
			if (((rt->pos.x-r->pos.x)*(rt->pos.x-r->pos.x)+
				 (rt->pos.y-r->pos.y)*(rt->pos.y-r->pos.y)+
				 (rt->pos.z-r->pos.z)*(rt->pos.z-r->pos.z))<COLISION_TEST_THRESHOLD) {
				if (rt!=r) {
					m2[12]=rt->pos.x;
					m2[13]=rt->pos.y;
					m2[14]=rt->pos.z; 
					if (r->cmc.collision_simple(m1,&(rt->cmc),m2)) return true;
				} /* if */ 
			} /* if */ 
		} /* while */ 
	} /* while */ 

	return false;
} /* NETHER::RobotCollision */ 


bool ROBOT::bullethit(int type)
{
	int npieces=0;
	int damage;

	if (pieces[0]) npieces++;
	if (pieces[1]) npieces++;
	if (pieces[2]) npieces++;
	if (pieces[3]) npieces++;
	if (pieces[4]) npieces++;

	switch(type) {
	case 0:
		/* CANNON: */ 
		damage=24;
		if (traction==0) damage-=2;
		if (npieces==1) damage-=2;
		if (npieces==2) damage-=4;
		if (npieces==3) damage-=8;
		if (npieces==4) damage-=12;
		if (npieces==5) damage-=16;
		break;
	case 1:
		/* CANNON: */ 
		damage=36;
		if (traction==0) damage-=3;
		if (npieces==1) damage-=3;
		if (npieces==2) damage-=6;
		if (npieces==3) damage-=12;
		if (npieces==4) damage-=18;
		if (npieces==5) damage-=24;
		break;
	case 2:
		/* CANNON: */ 
		damage=48;
		if (traction==0) damage-=4;
		if (npieces==1) damage-=4;
		if (npieces==2) damage-=8;
		if (npieces==3) damage-=16;
		if (npieces==4) damage-=24;
		if (npieces==5) damage-=32;
		break;
	} /* switch */ 

	strength-=damage;
	if (strength<=0) return false;
	return true;
} /* bullethit */ 

void NETHER::RobotCost(int player,ROBOT *r,int *res)
{
	int i;
	int tmp;
	int corr[5]={5,4,3,2,1};
	int cost[5]={2,4,4,20,3};

	res[0]=0;
	res[1]=0;
	res[2]=0;
	res[3]=0;
	res[4]=0;
	res[5]=0;
	res[6]=0;

	tmp=0;
	if (r->traction==0) tmp=3;
	if (r->traction==1) tmp=5;
	if (r->traction==2) tmp=10;
	if (resources[player][6]>tmp) {
		res[6]=tmp;
	} else {
		res[6]=resources[player][6];
		res[0]=tmp-resources[player][6];
	} /* if */ 

	for(i=0;i<5;i++) {
		if (r->pieces[i]) {
			if (resources[player][corr[i]]>cost[i]) {
				res[corr[i]]=cost[i];
			} else {
				res[corr[i]]=resources[player][corr[i]];
				res[0]+=cost[i]-resources[player][corr[i]];
			} /* if */ 	
		} /* if */ 
	} /* for */ 
} /* NETHER::RobotCost */ 


int NETHER::RobotCost(ROBOT *r)
{
	int i;
	int tmp=0;
	int cost[5]={2,4,4,20,3};

	if (r->traction==0) tmp=3;
	if (r->traction==1) tmp=5;
	if (r->traction==2) tmp=10;

	for(i=0;i<5;i++) {
		if (r->pieces[i]) tmp+=cost[i];
	} /* for */ 

	return tmp;
} /* NETHER::RobotCost */ 

