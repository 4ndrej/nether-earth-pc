#ifndef NETHER_HEADER
#define NETHER_HEADER


// #define _WRITE_REPORT_


#define COLISION_TEST_THRESHOLD	9.0
#define INTRO_TIME 60
#define END_TIME 260

/* BULLET SPEED: */ 
#define BULLET_SPEED		0.1f
#define CANNON_PERSISTENCE	40
#define MISSILE_PERSISTENCE	60
#define PHASER_PERSISTENCE	40

/* GAME STATES: */ 
#define STATE_PLAYING		0
#define STATE_CONSTRUCTION	1
#define STATE_PAUSE			2
#define STATE_SAVINGGAME	3
#define STATE_LOADINGGAME	4

/* BUTTON NAMES: */ 
#define TIME_BUTTON		1
#define STATUS_BUTTON	2
#define RESOURCE_BUTTON	3
#define ROBOT1_BUTTON	4
#define ROBOT2_BUTTON	5
#define ROBOT3_BUTTON	6
#define ROBOT4_BUTTON	7
#define COMBAT1_BUTTON	8
#define COMBAT2_BUTTON	9
#define COMBAT3_BUTTON	10
#define COMBAT4_BUTTON	11
#define COMBAT5_BUTTON	12
#define COMBAT6_BUTTON	13
#define ORDERS1_BUTTON	14
#define ORDERS2_BUTTON	15
#define ORDERS3_BUTTON	16
#define ORDERS4_BUTTON	17
#define ORDERS5_BUTTON	18
#define ORDERS_BUTTON	19
#define TARGET1_BUTTON	20
#define TARGET2_BUTTON	21
#define TARGET3_BUTTON	22

#define GENERAL_MENU		0
#define ROBOT_MENU			1
#define DIRECTCONTROL_MENU	2
#define COMBATMODE_MENU		3
#define DIRECTCONTROL2_MENU	4
#define ORDERS_MENU			5
#define SELECTDISTANCE_MENU 6
#define TARGETD_MENU		7
#define TARGETC_MENU		8

#define ALL_MENUS			9

/* SHIP OPERATORS: */ 
#define OP_NONE		   -1
#define	OP_LEFT			0
#define OP_RIGHT		1
#define OP_FORWARD		2
#define OP_BACKWARD		3
#define OP_UP			4

/* TERRAINS: */ 
#define T_GRASS		0
#define T_SAND		1
#define T_MOUNTAINS	2
#define T_HOLE		3
#define T_BUILDING	4
#define T_SHIP		5
#define T_ROBOT		6
#define T_EROBOT	7
#define T_OUT		8

/* RESOURCES: */ 
#define R_GENERAL		0
#define R_ELECTRONICS	1
#define R_NUCLEAR		2
#define R_PHASERS		3
#define R_MISSILES		4
#define R_CANNONS		5
#define R_CHASSIS		6

/* BUILDINGS & WALLS: */ 
#define B_FENCE		0
#define B_WALL1		1
#define B_WALL2		2
#define B_WALL3		3
#define B_WALL4		4
#define B_WALL5		5
#define B_WALL6		6
#define B_FACTORY_ELECTRONICS	7
#define B_FACTORY_NUCLEAR		8
#define B_FACTORY_PHASERS		9
#define B_FACTORY_MISSILES	   10
#define B_FACTORY_CANNONS	   11
#define B_FACTORY_CHASSIS	   12
#define B_WARBASE			   13

/* ROBOT PROGRAMS AND OPERATORS: */ 
#define ROBOTOP_NONE	   -1
#define ROBOTOP_FORWARD		0
#define ROBOTOP_LEFT		1
#define ROBOTOP_RIGHT		2
#define ROBOTOP_CANNONS		3
#define ROBOTOP_MISSILES	4
#define ROBOTOP_PHASERS		5
#define ROBOTOP_NUCLEAR		6

#define PROGRAM_NONE	   -1
#define PROGRAM_FORWARD		0
#define PROGRAM_STOPDEFEND	1
#define PROGRAM_ADVANCE		2
#define PROGRAM_RETREAT		3
#define PROGRAM_DESTROY		4
#define PROGRAM_CAPTURE		5

#define P_PARAM_ROBOTS		1
#define P_PARAM_WARBASES	2
#define P_PARAM_NFACTORIES	3
#define P_PARAM_EFACTORIES	4

/* ARTIFICIAL INTELLIGENCE STATES: */ 
#define AI_STATE_EXPANDING	0
#define AI_STATE_FIGHTING	1
#define AI_STATE_DEFENDING	2
#define AI_STATE_CONQUERING	3
#define AI_STATE_DESTROYING	4





class STATUSBUTTON {
public:
	int ID;
	int sx,sy;
	int x,y;
	char text1[80];
	char text2[80];

	int status;
	float r,g,b;
};


class BUILDING {
public:
	int type;
	Vector pos;
	int owner;

	int status;		/* This variable controls the status of the building: if its free, or being captured	*/ 
					/* by some of the players.																*/ 
};


class ROBOT {
public:
	ROBOT();
	bool valid(void);
	float piecez(int piece);
	bool bullethit(int type); 

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

	/* Animation variables: */ 
	int electronics_state;
	int chassis_state;
};


class BULLET {
public:
	BULLET(void);
	BULLET(int t,Vector p,int a,ROBOT *r);

	int type;
	int step;

	Vector pos;
	int angle;
	ROBOT *owner;	/* The robot who fired this bullet */ 

	CMC cmc;
};


class EXPLOSION {
public:
	EXPLOSION(void);
	EXPLOSION(Vector p,int sz);

	Vector pos;
	int step;
	int size;
};


class PARTICLE {
public:
	PARTICLE(void);
	PARTICLE(Vector p,Vector speed1,Vector speed2,float sz1,float sz2,float r,float g,float b,float a1,float a2,int lifetime);

	Vector pos,speed1,speed2;
	float size1,size2;
	float r,g,b;
	float a1,a2;
	int lifetime,acttime;
};


class AI_OPERATOR {
public:
	Vector newpos;
	int first_robotop;
	int cost;
	int previous;
	bool deadend;
	bool used;
};


class NETHER {
public:
	NETHER(char *mapname);
	~NETHER();

	void loadobjects();
	void deleteobjects();

	bool gamecycle(int w,int h);
	void gameredraw(int w,int h);
	void refresh_display_lists(void);

	bool save_game(char *filename);
	bool load_game(char *filename);
	bool save_debug_report(char *filename);

private:
	bool cycle(unsigned char *keyboard);
	bool construction_cycle(unsigned char *keyboard);
	bool option_cycle(unsigned char *keyboard);

	void draw(int w,int h);
	void draw_game(bool shadows);
	void draw_radar(void);
	void draw_status(void);
	void construction_draw(int w,int h);
	void options_draw(int w,int h);

	bool loadmap(char *file);
	void drawmap(bool shadows);

	float MapMaxZ(float x[2],float y[2]);
	int MapTerrain(float x, float y);
	int WorseMapTerrain(float x[2], float y[2]);

	bool ShipCollision(C3DObject *ship,float x,float y,float z);
	bool RobotCollision(ROBOT *r,bool complete);
	bool BulletCollision(BULLET *b,ROBOT **r);

	void DrawBullet(BULLET *bullet,bool shadows);
	void DrawRobot(ROBOT *robot,int owner,bool shadows);
	void RobotCost(int player,ROBOT *r,int *res);
	int  RobotCost(ROBOT *r);
	CMC  RobotCMC(ROBOT *r,int owner);
	CMC  BulletCMC(BULLET *r);
	void DrawParticle(PARTICLE *p);
	bool CycleParticle(PARTICLE *p);

	float RobotSpeed(int traction,int terrain);
	int RobotRotationSpeed(int traction,int terrain);
	bool Walkable(int traction,int terrain);

	int SFX_volume(Vector pos);

	void newbutton(int ID,int x,int y,int sx,int sy,char *t1,char *t2,float r,float g,float b);
	void newbuttondelayed(int ID,int x,int y,int sx,int sy,char *t1,char *t2,float r,float g,float b);
	void killbutton(int ID);
	STATUSBUTTON *getbutton(int ID);
	void newmenu(int menu);
	void killmenu(int menu);

	void AI_enemy(void);
	ROBOT *AI_enemy_newrobot(int state,Vector pos);
	void AI_precomputations(void);
	void AI_deleteprecomputations(void);
	void AI_release(void);
	int  AI_WorseMapTerrain(int x,int y,int dx,int dy);
	void AI_newrobot(Vector pos,int owner);
	int  AI_killrobot(Vector pos);
	void AI_moverobot(Vector oldpos,Vector newpos,int owner);
	void AI_removebuilding(Vector pos);
	void AI_availableoperators(Vector pos,int angle,int traction,List<AI_OPERATOR> *l);
	bool AI_expandoperators(int x,int y,int angle,int traction,int previous,int oldcost,int depth);
	int  AI_searchengine(Vector pos,int angle,int goaltype,Vector goalpos,int traction,int depth);
	void AI_resetsearch(Vector pos,int depth);
	int  AI_program_advance(int amount,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces);
	int  AI_program_retreat(int amount,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces);
	int  AI_program_capture(int goal,Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces);
	int  AI_program_destroy(int goal,Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces);
	int  AI_program_stopdefend(Vector *program_goal,Vector pos,int angle,int traction,bool electronics,int player,bool *pieces);
	void AI_rankoperators_advance(List<AI_OPERATOR> *l);
	void AI_rankoperators_retreat(List<AI_OPERATOR> *l);
	void AI_rankoperators_capture(List<AI_OPERATOR> *l,Vector goal);
	AI_OPERATOR *AI_chooseoperator(List<AI_OPERATOR> *l,int factor);
	int  AI_robothere(Vector pos);
	int  AI_RealShotPaths(int x,int y,int player,int persistence);

	/* Game variables: */ 
	int map_w,map_h;
	int *map;
	float lightpos[4];
	Vector lightposv;
	unsigned char old_keyboard[322];

	float zoom;
	Vector camera,viewp;
	Vector shipp;
	bool shiplanded;
	int ship_op,ship_op2,ship_op3;
	int ship_timemoving;

	List<BUILDING> buildings;
	List<ROBOT> robots[2];
	List<BULLET> bullets;
	List<EXPLOSION> explosions;
	List<PARTICLE> particles;

	int day,hour,minute,second;
	int resources[2][7];
	int statistics[2][8];

	float animation_timer;
	int construction_pointer;
	bool construction[8];
	int game_state;
	ROBOT in_construction;
	ROBOT *controlled;

	int game_finished;
	int game_started;

	/* Graphics: */ 
	int n_objs;
	C3DObject **tile;
	float *tile_r,*tile_g,*tile_b;
	Shadow3DObject *ship;
	int n_buildings;
	Shadow3DObject **building_tile;
	int n_pieces;
	Piece3DObject **piece_tile[2];
	C3DObject *construction_tile[3];
	C3DObject *message_tile[3];
	int n_bullets;
	Piece3DObject **bullet_tile;

	/* Status variables: */ 
	List<STATUSBUTTON> buttons;
	int act_menu;
	int act_button;
	int redrawmenu,redrawradar;
	bool recomputestatistics;

	/* Option/Pause menu variables: */ 
	int option_menu;

	/* Artificial intelligence variables: */ 
	int *discreetmap;
	int *bk_discreetmap;
	AI_OPERATOR **searchmap;
	int *atackmap;

	/* Sonido: */ 
	Mix_Chunk *S_shot,*S_explosion,*S_select,*S_wrong,*S_construction;
	

};

#endif
