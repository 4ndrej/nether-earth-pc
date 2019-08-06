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

#include "glprintf.h"

extern int frames_per_sec;
extern bool fullscreen;
extern int shadows;
extern bool sound;
extern int up_key,down_key,left_key,right_key,fire_key,pause_key;
extern int level;
extern float MINY,MAXY,MINX,MAXX;
extern bool show_radar;

#ifdef _WRITE_REPORT_
FILE *debug_fp=0;
#endif


NETHER::NETHER(char *mapname)
{

#ifdef _WRITE_REPORT_
	debug_fp=fopen("report.txt","w");
	fprintf(debug_fp,"Creating game...\n");
	fflush(debug_fp);
#endif

	if (shadows==1) {
		lightpos[0]=-1000;
		lightpos[1]=-3000;
		lightpos[2]=5000;
		lightpos[3]=1;
		lightposv.x=lightpos[0];
		lightposv.y=lightpos[1];
		lightposv.z=lightpos[2];
	} else {
		lightpos[0]=0;
		lightpos[1]=0;
		lightpos[2]=5000;
		lightpos[3]=1;
		lightposv.x=lightpos[0];
		lightposv.y=lightpos[1];
		lightposv.z=lightpos[2];
	} /* if */ 

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"loading objects...\n");
	fflush(debug_fp);
#endif

	loadobjects();
		
#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"loading map...\n");
	fflush(debug_fp);
#endif

	/* Load map: */ 
	if (!loadmap(mapname)) {
		map_w=map_h=0;
		map=0;
	} /* if */ 

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Initializing game variables...\n");
	fflush(debug_fp);
#endif

	/* Set camera: */ 
	viewp.x=map_w/2;
	viewp.y=0;
	camera.x=6;
	camera.y=-6;
	camera.z=11;
	zoom=1;

	/* Init game: */ 
	day=0;
	hour=0;
	minute=0;
	second=0;
	shipp.x=4.0;
	shipp.y=2.0;
	shipp.z=3.0;
	ship_op=OP_NONE;
	ship_op2=OP_NONE;
	ship_timemoving=0;

	resources[0][0]=20;
	resources[0][1]=0;
	resources[0][2]=0;
	resources[0][3]=0;
	resources[0][4]=0;
	resources[0][5]=0;
	resources[0][6]=0;
	resources[1][0]=20;
	resources[1][1]=0;
	resources[1][2]=0;
	resources[1][3]=0;
	resources[1][4]=0;
	resources[1][5]=0;
	resources[1][6]=0;
	statistics[0][0]=0;
	statistics[0][1]=0;
	statistics[0][2]=0;
	statistics[0][3]=0;
	statistics[0][4]=0;
	statistics[0][5]=0;
	statistics[0][6]=0;
	statistics[0][7]=0;
	statistics[1][0]=0;
	statistics[1][1]=0;
	statistics[1][2]=0;
	statistics[1][3]=0;
	statistics[1][4]=0;
	statistics[1][5]=0;
	statistics[1][6]=0;
	statistics[1][7]=0;
	recomputestatistics=true;

	game_state=STATE_PLAYING;
	animation_timer=0;
	construction_pointer=0;
	controlled=0;
	game_finished=0;
	game_started=INTRO_TIME;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Creating menus...\n");
	fflush(debug_fp);
#endif

	/* Init status: */ 
	newmenu(GENERAL_MENU);
	redrawmenu=2;
	redrawradar=1;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Initializing AI...\n");
	fflush(debug_fp);
#endif

	/* Init AI: */ 
	AI_precomputations();

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Loading sounds...\n");
	fflush(debug_fp);
#endif

	/* Load sounds: */ 
	S_shot=Mix_LoadWAV("sound/shot.wav");
	S_explosion=Mix_LoadWAV("sound/explosion.wav");
	S_select=Mix_LoadWAV("sound/select.wav");
	S_wrong=Mix_LoadWAV("sound/wrong.wav");
	S_construction=Mix_LoadWAV("sound/construction.wav");

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Game created.\n");
	fflush(debug_fp);
#endif

} /* NETHER::NETHER */ 


NETHER::~NETHER()
{

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Destroying Game...\n");
	fflush(debug_fp);
#endif

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Deleting sounds...\n");
	fflush(debug_fp);
#endif

	Mix_FreeChunk(S_shot);
	Mix_FreeChunk(S_explosion);
	Mix_FreeChunk(S_select);
	Mix_FreeChunk(S_wrong);
	Mix_FreeChunk(S_construction);
	S_shot=0;
	S_explosion=0;
	S_select=0;
	S_wrong=0;
	S_construction=0;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Deleting objects...\n");
	fflush(debug_fp);
#endif

	deleteobjects();

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Deleting AI...\n");
	fflush(debug_fp);
#endif

	AI_deleteprecomputations();

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Deleting map...\n");
	fflush(debug_fp);
#endif

	/* Delete map: */ 
	if (map!=0) delete map;
	map=0;
	map_w=map_h=0;

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Game destroyed.\n");
	fclose(debug_fp);
#endif

} /* NETHER::~NETHER */ 


void NETHER::loadobjects(void)
{
	/* Load 3D objects: */ 
	char *tnames[12]={"models/grass1.ase","models/rough.ase","models/rocks.ase","models/heavyrocks.ase",
					  "models/hole1.asc","models/hole2.asc","models/hole3.asc",
					  "models/hole4.asc","models/hole5.asc","models/hole6.asc",
					  "models/grass2.ase","models/grass3.ase"};
	char *bnames[9]={"models/lowwall1.ase","models/lowwall2.ase","models/lowwall3.ase",
					 "models/highwall1.ase","models/factory.ase","models/fence.asc",
					 "models/flag.asc","models/highwall2.ase","models/warbase.ase"};
	char *pnames[11]={"models/h-bipod.ase","models/h-tracks.ase","models/h-antigrav.ase",
					 "models/h-cannon.ase","models/h-missiles.ase","models/h-phasers.ase",
					 "models/h-nuclear.ase","models/h-electronics.ase",
					 "models/h-bipod-base.ase","models/h-bipod-rleg.ase","models/h-bipod-lleg.ase"};
	char *pnames2[11]={"models/e-bipod.ase","models/e-tracks.ase","models/e-antigrav.ase",
					  "models/e-cannon.ase","models/e-missiles.ase","models/e-phasers.ase",
					  "models/nuclear.asc","models/e-electronics.ase",
					  "models/e-bipod-base.ase","models/e-bipod-rleg.ase","models/e-bipod-lleg.ase"};
	char *bullnames[3]={"models/bullet1.asc","models/bullet2.asc","models/bullet3.asc"};
	float pscale[11]={0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.45,0.375,0.375};
	float bscale[9]={0.5,0.5,0.5,
					 0.5,0.5,1.0,
					 0.25,0.5,0.5}; 
	float bullscale[3]={0.05,0.3,0.4};

	float r[12]={0.0    ,0.7f ,0.6f  ,0.5f ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0};
	float g[12]={0.733f ,0.5f ,0.45f ,0.4f ,0.733f ,0.733f ,0.733f ,0.733f ,0.733f ,0.733f ,0.733f ,0.733f};
	float b[12]={0.0    ,0.0  ,0.0   ,0.0  ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0    ,0.0};
	int i;

	n_objs=12;
	tile=new C3DObject *[n_objs];
	tile_r=new float[n_objs];
	tile_g=new float[n_objs];
	tile_b=new float[n_objs];
	for(i=0;i<n_objs;i++) {
		tile[i]=new C3DObject(tnames[i],"textures/");
		tile[i]->normalize(0.50f);
		tile[i]->makepositive();
		tile_r[i]=r[i];
		tile_g[i]=g[i];
		tile_b[i]=b[i];
	} /* for */ 
	
	tile[4]->moveobject(0,0,-0.05);
	tile[5]->moveobject(0,0,-0.05);
	tile[6]->moveobject(0,0,-0.05);
	tile[7]->moveobject(0,0,-0.05);
	tile[8]->moveobject(0,0,-0.05);
	tile[9]->moveobject(0,0,-0.05);

	n_buildings=9;
	building_tile=new Shadow3DObject *[n_buildings];
	for(i=0;i<n_buildings;i++) {
		building_tile[i]=new Shadow3DObject(bnames[i],"textures/");
		building_tile[i]->normalize(bscale[i]);
		building_tile[i]->makepositive();
	} /* for */ 
	building_tile[5]->moveobject(0,0,0.01);
	building_tile[6]->moveobject(0.4,0.4,0.0);

	n_pieces=11;
	piece_tile[0]=new Piece3DObject *[n_pieces];
	piece_tile[1]=new Piece3DObject *[n_pieces];
	for(i=0;i<n_pieces;i++) {
		piece_tile[0][i]=new Piece3DObject(pnames[i],"textures/");
		piece_tile[0][i]->normalize(pscale[i]);
		piece_tile[0][i]->makepositive();
		piece_tile[1][i]=new Piece3DObject(pnames2[i],"textures/");
		piece_tile[1][i]->normalize(pscale[i]);
		piece_tile[1][i]->makepositive();
	} /* for */ 
	piece_tile[0][0]->moveobject(-0.5,-0.5,0.0);
	piece_tile[0][1]->moveobject(-0.5,-0.5,0.0);
	piece_tile[0][2]->moveobject(-0.5,-0.5,0.2);
	piece_tile[0][3]->moveobject(-0.5,-0.5,0.0);
	piece_tile[0][4]->moveobject(-0.5,-0.45,0.0);
	piece_tile[0][5]->moveobject(-0.5,-0.5,0.0);
	piece_tile[0][6]->moveobject(-0.5,-0.5,0.0);
	piece_tile[0][7]->moveobject(-0.32,-0.3,0.0);
	piece_tile[0][8]->moveobject(-0.45,-0.45,0.6);
	piece_tile[0][9]->moveobject(-0.4,-0.5,0.0);
	piece_tile[0][10]->moveobject(-0.4,0.2,0.0);

	piece_tile[1][0]->moveobject(-0.5,-0.5,0.0);
	piece_tile[1][1]->moveobject(-0.5,-0.5,0.0);
	piece_tile[1][2]->moveobject(-0.5,-0.5,0.2);
	piece_tile[1][3]->moveobject(-0.5,-0.5,0.0);
	piece_tile[1][4]->moveobject(-0.5,-0.45,0.0);
	piece_tile[1][5]->moveobject(-0.5,-0.5,0.0);
	piece_tile[1][6]->moveobject(-0.5,-0.5,0.0);
	piece_tile[1][7]->moveobject(-0.32,-0.3,0.0);
	piece_tile[1][8]->moveobject(-0.45,-0.45,0.6);
	piece_tile[1][9]->moveobject(-0.4,-0.5,0.0);
	piece_tile[1][10]->moveobject(-0.4,0.2,0.0);

	ship=new Shadow3DObject("models/ship.asc","textures/");
	ship->normalize(0.5f);
	ship->makepositive();

	n_bullets=3;
	bullet_tile=new Piece3DObject *[n_bullets];
	for(i=0;i<n_bullets;i++) {
		bullet_tile[i]=new Piece3DObject(bullnames[i],"textures/");
		bullet_tile[i]->normalize(bullscale[i]);
	} /* for */ 
	
	ship->ComputeShadow(lightposv);
	for(i=0;i<n_buildings;i++) building_tile[i]->ComputeShadow(lightposv);
	for(i=0;i<n_pieces;i++) piece_tile[0][i]->ComputeFixedShadows(lightposv);
	for(i=0;i<n_pieces;i++) piece_tile[1][i]->ComputeFixedShadows(lightposv);
	for(i=0;i<n_bullets;i++) bullet_tile[i]->ComputeFixedShadows(lightposv);

	construction_tile[0]=new C3DObject("models/construction1.asc","textures/");
	construction_tile[1]=new C3DObject("models/construction2.asc","textures/");
	construction_tile[2]=new C3DObject("models/construction3.asc","textures/");
	construction_tile[0]->normalize(10.0);
	construction_tile[1]->normalize(9.0);
	construction_tile[2]->normalize(7.0);

	message_tile[0]=new C3DObject("models/go.ase","textures/");
	message_tile[1]=new C3DObject("models/youwin.ase","textures/");
	message_tile[2]=new C3DObject("models/gameover.ase","textures/");
	message_tile[0]->normalize(4.0);
	message_tile[1]->normalize(4.0);
	message_tile[2]->normalize(4.0);
} /* NETHER::loadobjects */ 


void NETHER::deleteobjects(void)
{
	int i;

	/* Delete objects: */ 
	for(i=0;i<n_objs;i++) delete tile[i];
	delete tile;
	tile=0;
	delete tile_r;
	tile_r=0;
	delete tile_g;
	tile_g=0;
	delete tile_b;
	tile_b=0;
	delete ship;
	ship=0;
	for(i=0;i<n_buildings;i++) delete building_tile[i];
	delete building_tile;
	building_tile=0;
	for(i=0;i<n_pieces;i++) delete piece_tile[0][i];
	for(i=0;i<n_pieces;i++) delete piece_tile[1][i];
	delete piece_tile[0];
	delete piece_tile[1];
	piece_tile[0]=0;
	piece_tile[1]=0;
	delete construction_tile[0];
	delete construction_tile[1];
	delete construction_tile[2];
	construction_tile[0]=0;
	construction_tile[1]=0;
	construction_tile[2]=0;
	delete message_tile[0];
	delete message_tile[1];
	delete message_tile[2];
	for(i=0;i<n_bullets;i++) delete bullet_tile[i];
	delete bullet_tile;
	bullet_tile=0;
} /* NETHER::deleteobjects */ 


void NETHER::refresh_display_lists(void)
{
	int i;
 
 	for(i=0;i<n_objs;i++) {
 		tile[i]->refresh_display_lists();
 	} /* for */ 
 
 	ship->refresh_display_lists();
 
 	for(i=0;i<n_buildings;i++) {
 		building_tile[i]->refresh_display_lists();
 	} /* for */ 
 
 	for(i=0;i<n_pieces;i++) {
 		piece_tile[0][i]->refresh_display_lists();
 		piece_tile[1][i]->refresh_display_lists();
 	} /* for */ 
 
 	for(i=0;i<3;i++) {
 		construction_tile[i]->refresh_display_lists();
 	} /* for */ 
} /* NETHER::refresh_display_lists */ 



bool NETHER::gamecycle(int w,int h)
{
	int i;
	bool retval=true;
	unsigned char *keyboard;

	SDL_PumpEvents();
	keyboard = SDL_GetKeyState(NULL);

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Cycle start.\n");
	fprintf(debug_fp,"game_state: %i\n",game_state);
	fflush(debug_fp);
#endif

	switch(game_state) {
	case STATE_PLAYING:
		retval=cycle(keyboard);
		break;
	case STATE_CONSTRUCTION:
		retval=construction_cycle(keyboard);
		break;
	case STATE_PAUSE:
	case STATE_SAVINGGAME:
	case STATE_LOADINGGAME:
		retval=option_cycle(keyboard);
		break;
	} /* switch */ 

	for(i=0;i<SDLK_LAST;i++) old_keyboard[i]=keyboard[i];

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Cycle end: %i\n",retval);
	fflush(debug_fp);
#endif

	return retval;
} /* NETHER::gamecycle */ 


void NETHER::gameredraw(int w,int h)
{

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Redraw start.\n");
	fprintf(debug_fp,"game_state: %i\n",game_state);
	fflush(debug_fp);
#endif

	switch(game_state) {
	case STATE_PLAYING:
		draw(w,h);
		break;
	case STATE_CONSTRUCTION:
		construction_draw(w,h);
		break;
	case STATE_PAUSE:
	case STATE_SAVINGGAME:
	case STATE_LOADINGGAME:
		draw(w,h);
		options_draw(w,h);
		break;
	} /* switch */ 

	SDL_GL_SwapBuffers();

#ifdef _WRITE_REPORT_
	fprintf(debug_fp,"Redraw end.");
	fflush(debug_fp);
#endif

} /* gameredraw */ 


void NETHER::draw(int width,int height)
{
	float lightpos2[4]={0,0,1000,0};
	float tmpls[4]={1.0F,1.0F,1.0F,1.0};
	float tmpld[4]={0.6F,0.6F,0.6F,1.0};
	float tmpla[4]={0.2F,0.2F,0.2F,1.0};
    float ratio;
	int split = int((width*25.0F)/32.0F);
	int splity = 0;

	if (show_radar) splity = int((height*2.0F)/15.0F)+1;
			   else splity = 0;

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
	glEnable( GL_SCISSOR_TEST );  
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glClearStencil(0);
	
	/* Draw the GAME screen: */ 
	glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glClearColor(0,0,0,0.0);
    glViewport(0,splity,split,height-splity);
	ratio=float(split)/float(height-splity);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 30.0, ratio, 1.0, 1024.0 );
	glScissor(0,splity,split,height-splity);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	draw_game(false);
	if (shadows) {
		/* Set STENCIL Buffer: */ 
		glStencilMask(1);
		glEnable(GL_STENCIL_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		glStencilFunc(GL_ALWAYS,1,1);
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		draw_game(true);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		/* Draw shadow poligon: */ 
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_CULL_FACE);

		glColor4f(0.0,0.0,0.0,0.4f);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glStencilFunc(GL_NOTEQUAL,0,1);
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-1.0, 1.0,0.0);
		glVertex3f(-1.0,-1.0,0.0);
		glVertex3f( 1.0, 1.0,0.0);
		glVertex3f( 1.0,-1.0,0.0);
		glEnd();
		glPopMatrix();

		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
	} /* if */ 

	if (game_started>0) {
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		gluPerspective( 30.0, ratio, 1.0, 1024.0 );
		gluLookAt(0,0,30,0,0,0,0,1,0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (game_started>40) glTranslatef(0,0,(game_started-40)*2);
		if (game_started<20) glTranslatef(0,0,(20-game_started)*2);
		message_tile[0]->draw(1.0,1.0,1.0);
	} /* if */ 

	if (game_finished>100) {
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		gluPerspective( 30.0, ratio, 1.0, 1024.0 );
		gluLookAt(0,0,30,0,0,0,0,1,0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (game_finished<120) glTranslatef(0,0,(120-game_finished)*2);
		if (game_finished>240) glTranslatef(0,0,(game_finished-240)*2);
		if (statistics[0][0]==0) message_tile[2]->draw(1.0,1.0,1.0);
						    else message_tile[1]->draw(1.0,1.0,1.0);
	} /* if */ 

	/* Draw the RADAR screen: */ 
	if (show_radar && redrawradar<=1) {

		glLightfv(GL_LIGHT0,GL_POSITION,lightpos2);
		glClearColor(0.0,0.0,0,0);
		glViewport(0,0,split,splity);
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		glOrtho(0,float(split),0,float(splity),-100,100);
		glScissor(0,0,split,splity);
		glScalef(width/640.0,height/480.0,1);
		draw_radar();
	} /* if */ 
	redrawradar--;
	if (redrawradar<0) redrawradar=3;

	/* Draw the STATUS screen: */ 
	if (redrawmenu!=0) {
		redrawmenu--;

		glLightfv(GL_LIGHT0,GL_POSITION,lightpos2);
		glClearColor(0,0,0.2,0);
		glViewport(split,0,width-split,height);
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		glOrtho(0,float(width-split),0,height,-100,100);
		glScissor(split,0,width-split,height);
		glScalef(width/640.0,height/480.0,1);
		draw_status();

/*
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(70.0,10.0,0);
		glColor3f(1.0f,1.0f,1.0f);
		scaledglprintf(0.1f,0.1f,"FPS: %i",frames_per_sec);
		glPopMatrix();
*/
 
	} /* if */ 
} /* NETHER::draw */ 
 


void NETHER::draw_game(bool shadows)
{
	{
		MINY=-8*zoom;
		MINX=-(10+viewp.z*4)*zoom;
		MAXY=(9+viewp.z*4)*zoom;
		MAXX=8*zoom;
	}

	if (!explosions.EmptyP()) {
		int minstep=128;
		List<EXPLOSION> l;
		EXPLOSION *n;
		float offs=0.0,r;

		l.Instance(explosions);
		l.Rewind();
		while(l.Iterate(n)) {
			if (n->size==2 && n->step<minstep) minstep=n->step;
		} /* while */ 

		r=(128-minstep)/256.0;
		offs=sin(minstep)*r;

		gluLookAt(viewp.x+camera.x*zoom+offs,viewp.y+camera.y*zoom+offs,viewp.z+camera.z*zoom,viewp.x+offs,viewp.y+offs,viewp.z,0,0,1);
	} else {
		gluLookAt(viewp.x+camera.x*zoom,viewp.y+camera.y*zoom,viewp.z+camera.z*zoom,viewp.x,viewp.y,viewp.z,0,0,1);
	} /* if */ 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	/* Draw: */ 

	/* Draw the map: */ 
	drawmap(shadows);

	/* Draw the robots and bullets: */ 
	{
		int i;
		List<ROBOT> l;
		List<BULLET> l2;
		ROBOT *r;
		BULLET *b; 

		for(i=0;i<2;i++) {
			l.Instance(robots[i]);
			l.Rewind();
			while(l.Iterate(r)) {
				if (r->pos.y>=(viewp.y+MINY) &&
					r->pos.y<=(viewp.y+MAXY) &&
					r->pos.x>=(viewp.x+MINX) &&
					r->pos.x<=(viewp.x+MAXX)) {
					glPushMatrix();
					glTranslatef(r->pos.x,r->pos.y,r->pos.z);
					DrawRobot(r,i,shadows);
					glPopMatrix();
				} /* if */ 
			} /* while */ 
		} /* for */ 

		l2.Instance(bullets);
		l2.Rewind();
		while(l2.Iterate(b)) {
			if (b->pos.y>=(viewp.y+MINY) &&
				b->pos.y<=(viewp.y+MAXY) &&
				b->pos.x>=(viewp.x+MINX) &&
				b->pos.x<=(viewp.x+MAXX)) {
				glPushMatrix();
				glTranslatef(b->pos.x,b->pos.y,b->pos.z);
				DrawBullet(b,shadows);
				glPopMatrix();
			} /* if */ 
		} /* while */ 
	}

	/* Draw the ship: */ 
	glPushMatrix();
	glTranslatef(shipp.x,shipp.y,shipp.z);
	if (!shadows) ship->draw(0.7,0.7,0.7);
	glPopMatrix();

	if (shadows) {
		float sx,sy;
		float x[2],y[2];
		float minz;
		Vector light;

		light=lightposv;
		light=light/light.z;

		sx=shipp.x-light.x*shipp.z;
		sy=shipp.y-light.y*shipp.z;

		if (controlled==0) {
			x[0]=sx+ship->shdw_cmc.x[0];
			x[1]=sx+ship->shdw_cmc.x[1];
			y[0]=sy+ship->shdw_cmc.y[0];
			y[1]=sy+ship->shdw_cmc.y[1];
			minz=MapMaxZ(x,y);
		} else {
			minz=controlled->pos.z;
		} /* if */ 

		glPushMatrix();
		glTranslatef(sx,sy,minz+0.05);
		if (shadows) ship->DrawShadow(0,0,0,0.5);
		glPopMatrix();
	} 

	/* Draw the extras: */ 
	
	/* Draw nuclear explosions: */ 
	if (!shadows) {
		List<EXPLOSION> l;
		EXPLOSION *n;
		float a,r;

		l.Instance(explosions);
		l.Rewind();
		while(l.Iterate(n)) {
			a=(128.0f-n->step)/80.0f;
			r=1.0;
			if (n->size==0) {
				r=(float(n->step)/512.0f)+0.1;
			} /* if */ 
			if (n->size==1) {
				r=(float(n->step)/96.0f)+0.5;
			} /* if */ 
			if (n->size==2) {
				r=(float(n->step)/48.0f)+1.0;
			} /* if */ 
			if (a<0) a=0;
			if (a>1) a=1;

			glPushMatrix();
			glTranslatef(n->pos.x,n->pos.y,n->pos.z);		
			glColor4f(1.0f,0.5f,0.0,a);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glutSolidSphere(r,8,8);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glPopMatrix();
		} /* while */ 
	}

	/* Draw the particles: */ 
	if (!shadows) {
		List<PARTICLE> l;
		PARTICLE *p;

		l.Instance(particles);
		l.Rewind();
		while(l.Iterate(p)) {
			if (p->pos.y>=(viewp.y+MINY) &&
				p->pos.y<=(viewp.y+MAXY) &&
				p->pos.x>=(viewp.x+MINX) &&
				p->pos.x<=(viewp.x+MAXX)) DrawParticle(p);
		} /* if */ 

	} /* if */ 

} /* NETHER::draw_screen */ 


void NETHER::draw_status(void)
{
    /* Clear the color and depth buffers. */ 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	/* Draw buttons: */ 
	{
		List<STATUSBUTTON> l;
		STATUSBUTTON *b;
		float angle,cf;

		l.Instance(buttons);
		l.Rewind();
		while(l.Iterate(b)) {
			if (b->status>=-16) {
				angle=(float(b->status)*90.0)/16.0;
				cf=float((16-abs(b->status)))/16.0;
				glPushMatrix();
				glTranslatef(b->x,b->y,0);
				glRotatef(angle,0,1,0);

				/* Draw button: */ 
				glColor3f(b->r*cf,b->g*cf,b->b*cf);
				glutSolidBox(b->sx/2,b->sy/2,10.0);
				glTranslatef(0,0,11);

				glColor3f(1.0,1.0,1.0);
				if (b->text1[0]!=0) {
					if (b->text2[0]!=0) {
						glTranslatef(0,-12,0);
						scaledglprintf(0.1f,0.1f,b->text2);
						glTranslatef(0,17,0);
						scaledglprintf(0.1f,0.1f,b->text1);
					} else {
						glTranslatef(0,-3,0);
						scaledglprintf(0.1f,0.1f,b->text1);
					} /* if */ 
				} /* if */ 

				glPopMatrix();
			} /* if */ 
		} /* while */ 
	}

	glPushMatrix();
	switch(act_menu) {
	case GENERAL_MENU:
		{
			STATUSBUTTON *b;
			b=getbutton(STATUS_BUTTON);
			if (b!=0 && b->status==0) {
				statistics[0][7]=robots[0].Length();
				statistics[1][7]=robots[1].Length();

				glColor3f(0.5f,0.5f,1.0f);
				glTranslatef(70,356,0);
				scaledglprintf(0.1f,0.1f,"%i WARBASES %i",statistics[1][0],statistics[0][0]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i ELECTR'S %i",statistics[1][1],statistics[0][1]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i NUCLEAR  %i",statistics[1][2],statistics[0][2]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i PHASERS  %i",statistics[1][3],statistics[0][3]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i MISSILES %i",statistics[1][4],statistics[0][4]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i  CANNON  %i",statistics[1][5],statistics[0][5]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%i CHASSIS  %i",statistics[1][6],statistics[0][6]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"%.2i ROBOTS %.2i",statistics[1][7],statistics[0][7]);

				glColor3f(0.0f,0.8f,0.0f);
				glTranslatef(0,-65,0);
				scaledglprintf(0.1f,0.1f,"GENERAL %.2i",resources[0][0]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"ELECTR' %.2i",resources[0][1]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"NUCLEAR %.2i",resources[0][2]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"PHASERS %.2i",resources[0][3]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"MISSILE %.2i",resources[0][4]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"CANNON  %.2i",resources[0][5]);
				glTranslatef(0,-18,0);
				scaledglprintf(0.1f,0.1f,"CHASSIS %.2i",resources[0][6]);
			} /* if */ 
		}
		break;
	case ROBOT_MENU:
	case DIRECTCONTROL_MENU:
		{
			STATUSBUTTON *b;

			b=getbutton(ROBOT1_BUTTON);
			if (b!=0 && b->status==0) {

				glTranslatef(70,140,0);
				glColor3f(1.0f,1.0f,0.0);
				scaledglprintf(0.1f,0.1f,"-ORDERS-");
				glColor3f(0.5f,0.5f,1.0f);
				switch(controlled->program) {
				case PROGRAM_STOPDEFEND:
					glTranslatef(0,-20,0);
					scaledglprintf(0.1f,0.1f,"STOP");
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"AND");
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"DEFEND");
					break;
				case PROGRAM_ADVANCE:
					glTranslatef(0,-20,0);
					scaledglprintf(0.1f,0.1f,"ADVANCE");
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"%.2i",controlled->program_parameter/2);
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"MILES");
					break;
				case PROGRAM_RETREAT:
					glTranslatef(0,-20,0);
					scaledglprintf(0.1f,0.1f,"RETREAT");
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"%.2i",controlled->program_parameter/2);
					glTranslatef(0,-18,0);
					scaledglprintf(0.1f,0.1f,"MILES");
					break;
				case PROGRAM_DESTROY:
					glTranslatef(0,-20,0);
					scaledglprintf(0.1f,0.1f,"DESTROY");
					switch(controlled->program_parameter) {
					case P_PARAM_ROBOTS:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ROBOTS");
						break;
					case P_PARAM_WARBASES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"WARBASES");
						break;
					case P_PARAM_NFACTORIES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"NEUTRAL");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"FACTORIES");
						break;
					case P_PARAM_EFACTORIES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"FACTORIES");
						break;
					} /* switch */ 
					break;
				case PROGRAM_CAPTURE:
					glTranslatef(0,-20,0);
					scaledglprintf(0.1f,0.1f,"CAPTURE");
					switch(controlled->program_parameter) {
					case P_PARAM_ROBOTS:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ROBOTS");
						break;
					case P_PARAM_WARBASES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"WARBASES");
						break;
					case P_PARAM_NFACTORIES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"NEUTRAL");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"FACTORIES");
						break;
					case P_PARAM_EFACTORIES:
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"ENEMY");
						glTranslatef(0,-18,0);
						scaledglprintf(0.1f,0.1f,"FACTORIES");
						break;
					} /* switch */ 
					break;
				} /* switch */ 

				glTranslatef(0,-44,0);
				glColor3f(1.0f,1.0f,0.0);
				scaledglprintf(0.1f,0.1f,"STRENGTH");
				glTranslatef(0,-18,0);
				glColor3f(1.0f,1.0f,1.0f);
				scaledglprintf(0.1f,0.1f,"%.3i%c",controlled->strength,'%');
			} /* if */ 
		}
		break;

	case COMBATMODE_MENU:
	case DIRECTCONTROL2_MENU:

		glTranslatef(70,40,0);
		glColor3f(1.0f,1.0f,0.0);
		scaledglprintf(0.1f,0.1f,"STRENGTH");
		glTranslatef(0,-18,0);
		glColor3f(1.0f,1.0f,1.0f);
		scaledglprintf(0.1f,0.1f,"%.3i%c",controlled->strength,'%');
		break;

	case ORDERS_MENU:
		{
			STATUSBUTTON *b;

			b=getbutton(ORDERS1_BUTTON);
			if (b!=0 && b->status==0) {
				glTranslatef(70,400,0);
				glColor3f(1.0f,1.0f,1.0f);
				scaledglprintf(0.1f,0.1f,"SELECT");
				glTranslatef(0,-20,0);
				scaledglprintf(0.1f,0.1f,"ORDERS");

				glTranslatef(0,-340,0);
				glColor3f(1.0f,1.0f,0.0);
				scaledglprintf(0.1f,0.1f,"STRENGTH");
				glTranslatef(0,-18,0);
				glColor3f(1.0f,1.0f,1.0f);
				scaledglprintf(0.1f,0.1f,"%.3i%c",controlled->strength,'%');
			} /* if */ 
		}
		break;

	case SELECTDISTANCE_MENU:
		{
			STATUSBUTTON *b;

			b=getbutton(ORDERS_BUTTON);
			if (b!=0 && b->status==0) {
				glTranslatef(70,300,0);
				glColor3f(0.5f,0.5f,1.0f);
				scaledglprintf(0.1f,0.1f,"SELECT");
				glTranslatef(0,-20,0);
				scaledglprintf(0.1f,0.1f,"DISTANCE");

				glColor3f(1.0f,1.0f,0.0);
				glTranslatef(0,-40,0);
				scaledglprintf(0.1f,0.1f,"%.2i MILES",controlled->program_parameter/2);

				glTranslatef(0,-200,0);
				glColor3f(1.0f,1.0f,0.0);
				scaledglprintf(0.1f,0.1f,"STRENGTH");
				glTranslatef(0,-18,0);
				glColor3f(1.0f,1.0f,1.0f);
				scaledglprintf(0.1f,0.1f,"%.3i%c",controlled->strength,'%');
			} /* if */ 
		}
		break;

	case TARGETD_MENU:
	case TARGETC_MENU:
		{
			STATUSBUTTON *b;

			b=getbutton(ORDERS_BUTTON);
			if (b!=0 && b->status==0) {
				glTranslatef(70,350,0);
				glColor3f(0.5f,0.5f,1.0f);
				scaledglprintf(0.1f,0.1f,"SELECT");
				glTranslatef(0,-20,0);
				scaledglprintf(0.1f,0.1f,"TARGET");

				glTranslatef(0,-290,0);
				glColor3f(1.0f,1.0f,0.0);
				scaledglprintf(0.1f,0.1f,"STRENGTH");
				glTranslatef(0,-18,0);
				glColor3f(1.0f,1.0f,1.0f);
				scaledglprintf(0.1f,0.1f,"%.3i%c",controlled->strength,'%');
			} /* if */ 
		}
		break;

	} /* switch */ 
	glPopMatrix();

} /* NETHER::draw_status */ 


void NETHER::options_draw(int w,int h)
{
	float lightpos2[4]={0,0,1000,0};
	float tmpls[4]={1.0F,1.0F,1.0F,1.0};
	float tmpld[4]={0.6F,0.6F,0.6F,1.0};
	float tmpla[4]={0.2F,0.2F,0.2F,1.0};
    float ratio;
	int splitx[2]={int(w*0.3),int(w*0.7)};
	int splity[2]={int(h*0.3),int(h*0.7)};

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
	glEnable( GL_SCISSOR_TEST );  
	glEnable( GL_DEPTH_TEST );
	
	/* Draw the MENU: */ 
	glLightfv(GL_LIGHT0,GL_POSITION,lightpos);
    glClearColor(0,0,0,0.0);
    glViewport(splitx[0],splity[0],splitx[1]-splitx[0],splity[1]-splity[0]);
	ratio=float(splitx[1]-splitx[0])/float(splity[1]-splity[0]);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 30.0, ratio, 1.0, 1024.0 );
	glScissor(splitx[0],splity[0],splitx[1]-splitx[0],splity[1]-splity[0]);
	gluLookAt(0,0,30,0,0,0,0,1,0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	if (game_state==STATE_PAUSE) {
		if (option_menu==0) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,5,0);
		scaledglprintf(0.01,0.01,"RETURN TO GAME");

		if (option_menu==1) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,-5,0);
		scaledglprintf(0.01,0.01,"LOAD GAME");

		if (option_menu==2) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,-3,0);
		scaledglprintf(0.01,0.01,"SAVE GAME");

		if (option_menu==3) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,-3,0);
		scaledglprintf(0.01,0.01,"QUIT GAME");
	} /* if */ 

	if (game_state==STATE_SAVINGGAME) {
		int i;
		FILE *fp;
		char filename[80];

		glColor3f(0.5,0.5,1.0);
		glTranslatef(0,6,0);
		scaledglprintf(0.01,0.01,"CHOOSE SLOT TO SAVE");

		if (option_menu==0) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,-4,0);
		scaledglprintf(0.01,0.01,"CANCEL");

		for(i=0;i<4;i++) {
			if (option_menu==(i+1)) glColor3f(1.0,0.0,0.0);
						       else glColor3f(0.5,0.5,1.0);
			glTranslatef(0,-2,0);
			sprintf(filename,"savedgame%i.txt",i);
			fp=fopen(filename,"r");
			if (fp==0) {
				scaledglprintf(0.01,0.01,"SLOT%i - EMPTY",i+1);
			} else {
				scaledglprintf(0.01,0.01,"SLOT%i - GAME SAVED",i+1);
				fclose(fp);
			} /* if */ 
		} /* for */ 
	} /* if */ 

	if (game_state==STATE_LOADINGGAME) {
		int i;
		FILE *fp;
		char filename[80];

		glColor3f(0.5,0.5,1.0);
		glTranslatef(0,6,0);
		scaledglprintf(0.01,0.01,"CHOOSE SLOT TO LOAD");

		if (option_menu==0) glColor3f(1.0,0.0,0.0);
					   else glColor3f(0.5,0.5,1.0);
		glTranslatef(0,-4,0);
		scaledglprintf(0.01,0.01,"CANCEL");

		for(i=0;i<4;i++) {
			if (option_menu==(i+1)) glColor3f(1.0,0.0,0.0);
						       else glColor3f(0.5,0.5,1.0);
			glTranslatef(0,-2,0);
			sprintf(filename,"savedgame%i.txt",i);
			fp=fopen(filename,"r");
			if (fp==0) {
				scaledglprintf(0.01,0.01,"SLOT%i - EMPTY",i+1);
			} else {
				scaledglprintf(0.01,0.01,"SLOT%i - GAME SAVED",i+1);
				fclose(fp);
			} /* if */ 
		} /* for */ 
	} /* if */ 


} /* NETHER::options_draw */ 


bool NETHER::option_cycle(unsigned char *keyboard)
{
	switch(game_state) {
	case STATE_PAUSE:
		if (keyboard[fire_key] && !old_keyboard[fire_key]) {
			switch(option_menu) {
			case 0:
					game_state=STATE_PLAYING;
					break;
			case 1: game_state=STATE_LOADINGGAME;
					option_menu=0;
					break;
			case 2:	game_state=STATE_SAVINGGAME;
					option_menu=0;
					break;
			case 3:	return false;
					break;
			} /* if */ 
		} /* if */ 

		if (keyboard[down_key] && !old_keyboard[down_key]) {
			option_menu++;
			if (option_menu>=4) option_menu=0;
		} /* if */ 

		if (keyboard[up_key] && !old_keyboard[up_key]) {
			option_menu--;
			if (option_menu<0) option_menu=3;
		} /* if */ 
		break;

	case STATE_SAVINGGAME:
		if (keyboard[fire_key] && !old_keyboard[fire_key]) {
			switch(option_menu) {
			case 0:
					game_state=STATE_PAUSE;
					option_menu=2;
					break;
			case 1: 
			case 2:
			case 3:
			case 4:
				{
					char filename[80];
					sprintf(filename,"savedgame%i.txt",option_menu-1);
					save_game(filename);
					//save_debug_report("debugreport.txt");
					game_state=STATE_PAUSE;
					option_menu=2;
					redrawmenu=2;
					redrawradar=1;
				}
			} /* if */ 
		} /* if */ 

		if (keyboard[down_key] && !old_keyboard[down_key]) {
			option_menu++;
			if (option_menu>=5) option_menu=0;
		} /* if */ 

		if (keyboard[up_key] && !old_keyboard[up_key]) {
			option_menu--;
			if (option_menu<0) option_menu=4;
		} /* if */ 
		break;

	case STATE_LOADINGGAME:
		if (keyboard[fire_key] && !old_keyboard[fire_key]) {
			switch(option_menu) {
			case 0:
					game_state=STATE_PAUSE;
					option_menu=1;
					break;
			case 1: 
			case 2:
			case 3:
			case 4:
				{
					char filename[80];
					sprintf(filename,"savedgame%i.txt",option_menu-1);
					killmenu(act_menu);
					load_game(filename);
					newmenu(act_menu);
					redrawmenu=2;
					recomputestatistics=true;
					game_finished=0;
					game_started=INTRO_TIME;
					game_state=STATE_PAUSE;
					option_menu=2;
				}
			} /* switch */ 
		} /* if */ 

		if (keyboard[down_key] && !old_keyboard[down_key]) {
			option_menu++;
			if (option_menu>=5) option_menu=0;
		} /* if */ 

		if (keyboard[up_key] && !old_keyboard[up_key]) {
			option_menu--;
			if (option_menu<0) option_menu=4;
		} /* if */ 
		break;
	} /* switch */ 

	return true;
} /* NETHER::option_cycle */ 


bool NETHER::ShipCollision(C3DObject *obj,float x,float y,float z)
{
	int i;
	List<BUILDING> l;
	BUILDING *b;
	List<ROBOT> l2;
	ROBOT *r;
	float m1[16]={1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  x,y,z,1};
	float m2[16]={1,0,0,0,
				  0,1,0,0,
				  0,0,1,0,
				  0,0,0,1}; 

	/* Collision with buildings: */  
	l.Instance(buildings);
	l.Rewind();
	while(l.Iterate(b)) {
		if (((b->pos.x-x)*(b->pos.x-x)+
			 (b->pos.y-y)*(b->pos.y-y)+
			 (b->pos.z-z)*(b->pos.z-z))<COLISION_TEST_THRESHOLD) {

			m2[12]=b->pos.x;
			m2[13]=b->pos.y;
			m2[14]=b->pos.z;

			switch(b->type) {
				case B_FENCE:
					if (obj->cmc.collision_simple(m1,&(building_tile[5]->cmc),m2)) return true;
					break;
				case B_WALL1:
					if (obj->cmc.collision_simple(m1,&(building_tile[0]->cmc),m2)) return true;
					break;
				case B_WALL2:
					if (obj->cmc.collision_simple(m1,&(building_tile[1]->cmc),m2)) return true;
					break;
				case B_WALL3:
					if (obj->cmc.collision_simple(m1,&(building_tile[2]->cmc),m2)) return true;
					break;
				case B_WALL4:
					if (obj->cmc.collision_simple(m1,&(building_tile[3]->cmc),m2)) return true;
					break;
				case B_WALL5:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					break;
				case B_WALL6:
					if (obj->cmc.collision_simple(m1,&(building_tile[7]->cmc),m2)) return true;
					break;
				case B_WARBASE:
					if (obj->cmc.collision_simple(m1,&(building_tile[8]->cmc),m2)) return true;
					//m2[13]=b->pos.y-2;
					//m2[14]=b->pos.z+1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_ELECTRONICS:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][7]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_NUCLEAR:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][6]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_PHASERS:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][5]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_MISSILES:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][4]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_CANNONS:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][3]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			case B_FACTORY_CHASSIS:
					if (obj->cmc.collision_simple(m1,&(building_tile[4]->cmc),m2)) return true;
					m2[12]=b->pos.x+0.5;
					m2[13]=b->pos.y+0.5;
					m2[14]=b->pos.z+1;
					if (obj->cmc.collision_simple(m1,&(piece_tile[0][1]->cmc),m2)) return true;
					//m2[12]=b->pos.x;
					//m2[13]=b->pos.y-1;
					//if (b->owner!=0) if (obj->cmc.collision_simple(m1,&(building_tile[6]->cmc),m2)) return true;
					break;
			} /* switch */ 
		} /* if */ 
	} /* while */ 

	/* Collision with the robots: */ 
	for(i=0;i<2;i++) {
		l2.Instance(robots[i]);
		l2.Rewind();
		while(l2.Iterate(r)) {
			if (((r->pos.x-x)*(r->pos.x-x)+
				 (r->pos.y-y)*(r->pos.y-y))<COLISION_TEST_THRESHOLD) {
				m2[12]=r->pos.x;
				m2[13]=r->pos.y;
				m2[14]=r->pos.z; 
				if (obj->cmc.collision_simple(m1,&(r->cmc),m2)) return true;
			} /* if */ 
		} /* while */ 
	} /* while */ 

	return false;
} /* NETHER::ShipCollision */ 



int NETHER::SFX_volume(Vector pos)
{
	float distance=(pos-shipp).norma();

	distance=(distance-8)/8;
	if (distance<1) distance=1;


	return int(128/distance);
} /* SFX_volume */ 
