#ifdef _WIN32
#include "windows.h"
#endif

#include "stdio.h"
#include "string.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "list.h"
#include "vector.h"
#include "cmc.h"
#include "3dobject.h"
#include "shadow3dobject.h"
#include "piece3dobject.h"
#include "nether.h"

void NETHER::newmenu(int menu)
{
	switch(menu) {
	case GENERAL_MENU:
		if (getbutton(TIME_BUTTON)==0) {
			newbutton(TIME_BUTTON,70,455,130,40,"Day: 0","Time: 00:00",0.8f,0,0);
		} /* if */ 
		newbuttondelayed(STATUS_BUTTON,70,400,130,50,"STATUS","INSG  HUMN",0.0,0,0);
		newbuttondelayed(RESOURCE_BUTTON,70,200,130,30,"RESOURCES",0,0.0,0,0);
		act_menu=GENERAL_MENU;
		break;

	case ROBOT_MENU:
		newbuttondelayed(ROBOT1_BUTTON,70,350,130,40,"DIRECT   ","  CONTROL",0,0,0.8f);
		newbuttondelayed(ROBOT2_BUTTON,70,300,130,40,"GIVE     ","   ORDERS",0,0,0.8f);
		newbuttondelayed(ROBOT3_BUTTON,70,250,130,40,"COMBAT   ","     MODE",0,0,0.8f);
		newbuttondelayed(ROBOT4_BUTTON,70,200,130,40,"LEAVE    ","    ROBOT",0,0,0.8f);
		act_menu=ROBOT_MENU;
		break;

	case COMBATMODE_MENU:
		newbuttondelayed(COMBAT1_BUTTON,70,350,130,40,"NUCLEAR  ","     BOMB",0,0,0.8f);
		newbuttondelayed(COMBAT2_BUTTON,70,300,130,40,"FIRE     ","  PHASERS",0,0,0.8f);
		newbuttondelayed(COMBAT3_BUTTON,70,250,130,40,"FIRE     "," MISSILES",0,0,0.8f);
		newbuttondelayed(COMBAT4_BUTTON,70,200,130,40,"FIRE     ","   CANNON",0,0,0.8f);
		newbuttondelayed(COMBAT5_BUTTON,70,150,130,40,"MOVE     ","    ROBOT",0,0,0.8f);
		newbuttondelayed(COMBAT6_BUTTON,70,100,130,40,"STOP     ","   COMBAT",0,0,0.8f);
		act_menu=COMBATMODE_MENU;
		break;

	case ORDERS_MENU:
		newbuttondelayed(ORDERS1_BUTTON,70,350,130,40,"STOP AND ","   DEFEND",0,0,0.8f);
		newbuttondelayed(ORDERS2_BUTTON,70,300,130,40,"ADVANCE  "," ?? MILES",0,0,0.8f);
		newbuttondelayed(ORDERS3_BUTTON,70,250,130,40,"RETREAT  "," ?? MILES",0,0,0.8f);
		newbuttondelayed(ORDERS4_BUTTON,70,200,130,40,"SEARCH & ","  DESTROY",0,0,0.8f);
		newbuttondelayed(ORDERS5_BUTTON,70,150,130,40,"SEARCH & ","  CAPTURE",0,0,0.8f);
		act_menu=ORDERS_MENU;
		break;

	case SELECTDISTANCE_MENU:
		if (controlled->program==PROGRAM_ADVANCE)
			newbuttondelayed(ORDERS_BUTTON,70,400,130,40,"ADVANCE  "," ?? MILES",0,0,0.8f);
		if (controlled->program==PROGRAM_RETREAT)
			newbuttondelayed(ORDERS_BUTTON,70,400,130,40,"RETREAT  "," ?? MILES",0,0,0.8f);
		act_menu=SELECTDISTANCE_MENU;
		break;

	case TARGETD_MENU:
		newbuttondelayed(ORDERS_BUTTON,70,400,130,40,"SEARCH & ","  DESTROY",0,0,0.8f);

		newbuttondelayed(TARGET1_BUTTON,70,300,130,40,"ENEMY    ","   ROBOTS",0,0,0.8f);
		newbuttondelayed(TARGET2_BUTTON,70,250,130,40,"ENEMY    ","FACTORIES",0,0,0.8f);
		newbuttondelayed(TARGET3_BUTTON,70,200,130,40,"ENEMY    "," WARBASES",0,0,0.8f);
		act_menu=TARGETD_MENU;
		break;

	case TARGETC_MENU:
		newbuttondelayed(ORDERS_BUTTON,70,400,130,40,"SEARCH & ","  CAPTURE",0,0,0.8f);

		newbuttondelayed(TARGET1_BUTTON,70,300,130,40,"NEUTRAL  ","FACTORIES",0,0,0.8f);
		newbuttondelayed(TARGET2_BUTTON,70,250,130,40,"ENEMY    ","FACTORIES",0,0,0.8f);
		newbuttondelayed(TARGET3_BUTTON,70,200,130,40,"ENEMY    "," WARBASES",0,0,0.8f);
		act_menu=TARGETC_MENU;
		break;
	
	} /* switch */ 
	redrawmenu=true;
} /* NETHER::newmenu */ 


void NETHER::killmenu(int menu)
{
	switch(menu) {
	case GENERAL_MENU:
		killbutton(STATUS_BUTTON);
		killbutton(RESOURCE_BUTTON);
		break;

	case ROBOT_MENU:
		killbutton(ROBOT1_BUTTON);
		killbutton(ROBOT2_BUTTON);
		killbutton(ROBOT3_BUTTON);
		killbutton(ROBOT4_BUTTON);
		break;

	case COMBATMODE_MENU:
		killbutton(COMBAT1_BUTTON);
		killbutton(COMBAT2_BUTTON);
		killbutton(COMBAT3_BUTTON);
		killbutton(COMBAT4_BUTTON);
		killbutton(COMBAT5_BUTTON);
		killbutton(COMBAT6_BUTTON);
		break;

	case ORDERS_MENU:
		killbutton(ORDERS1_BUTTON);
		killbutton(ORDERS2_BUTTON);
		killbutton(ORDERS3_BUTTON);
		killbutton(ORDERS4_BUTTON);
		killbutton(ORDERS5_BUTTON);
		break;

	case SELECTDISTANCE_MENU:
		killbutton(ORDERS_BUTTON);
		break;

	case TARGETD_MENU:
	case TARGETC_MENU:
		killbutton(ORDERS_BUTTON);
		killbutton(TARGET1_BUTTON);
		killbutton(TARGET2_BUTTON);
		killbutton(TARGET3_BUTTON);
		break;

	case ALL_MENUS:
		killbutton(STATUS_BUTTON);
		killbutton(RESOURCE_BUTTON);
		killbutton(ROBOT1_BUTTON);
		killbutton(ROBOT2_BUTTON);
		killbutton(ROBOT3_BUTTON);
		killbutton(ROBOT4_BUTTON);
		killbutton(COMBAT1_BUTTON);
		killbutton(COMBAT2_BUTTON);
		killbutton(COMBAT3_BUTTON);
		killbutton(COMBAT4_BUTTON);
		killbutton(COMBAT5_BUTTON);
		killbutton(COMBAT6_BUTTON);
		killbutton(ORDERS1_BUTTON);
		killbutton(ORDERS2_BUTTON);
		killbutton(ORDERS3_BUTTON);
		killbutton(ORDERS4_BUTTON);
		killbutton(ORDERS5_BUTTON);
		killbutton(ORDERS_BUTTON);
		killbutton(ORDERS_BUTTON);
		killbutton(TARGET1_BUTTON);
		killbutton(TARGET2_BUTTON);
		killbutton(TARGET3_BUTTON);
		break;
	} /* switch */ 
	redrawmenu=true;
} /* killmenu */ 


void NETHER::newbutton(int ID,int x,int y,int sx,int sy,char *t1,char *t2,float r,float g,float b)
{
	STATUSBUTTON *but;

	but=new STATUSBUTTON();

	but->ID=ID;
	but->x=x;
	but->y=y;
	but->sx=sx;
	but->sy=sy;
	if (t1!=0) strcpy(but->text1,t1);
		  else but->text1[0]=0;
	if (t2!=0) strcpy(but->text2,t2);
		  else but->text2[0]=0;
	but->r=r;
	but->g=g;
	but->b=b;
	but->status=-16;

	buttons.Add(but);
	redrawmenu=true;
} /* NETHER::newbutton */ 


void NETHER::newbuttondelayed(int ID,int x,int y,int sx,int sy,char *t1,char *t2,float r,float g,float b)
{
	STATUSBUTTON *but;

	but=new STATUSBUTTON();

	but->ID=ID;
	but->x=x;
	but->y=y;
	but->sx=sx;
	but->sy=sy;
	if (t1!=0) strcpy(but->text1,t1);
		  else but->text1[0]=0;
	if (t2!=0) strcpy(but->text2,t2);
		  else but->text2[0]=0;
	but->r=r;
	but->g=g;
	but->b=b;
	but->status=-32;

	buttons.Add(but);
	redrawmenu=true;
} /* NETHER::newbuttondelayed */ 


void NETHER::killbutton(int ID)
{
	List<STATUSBUTTON> l;
	STATUSBUTTON *b;

	l.Instance(buttons);
	l.Rewind();
	while(l.Iterate(b)) {
		if (b->ID==ID) b->status=1;
	} /* while */ 
	redrawmenu=true;
} /* killbutton */ 


STATUSBUTTON *NETHER::getbutton(int ID)
{
	List<STATUSBUTTON> l;
	STATUSBUTTON *b;

	l.Instance(buttons);
	l.Rewind();
	while(l.Iterate(b)) {
		if (b->ID==ID) return b;
	} /* while */ 

	return 0;
} /* getbutton */ 
