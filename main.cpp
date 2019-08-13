#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "list.h"
#include "vector.h"

#include "cmc.h"
#include "3dobject.h"
#include "shadow3dobject.h"
#include "piece3dobject.h"
#include "nether.h"

/*						GLOBAL VARIABLES INITIALIZATION:							*/ 

int SCREEN_X=640;
int SCREEN_Y=480;

int COLOUR_DEPTH=32;
bool shadows=true;
int detaillevel=4;
bool sound=true;
int up_key=SDLK_q,down_key=SDLK_a,left_key=SDLK_o,right_key=SDLK_p,fire_key=SDLK_SPACE,pause_key=SDLK_F1;
int level=1;
int mainmenu_status=0;
int mainmenu_substatus=0;
bool fullscreen=false;
char mapname[128]="original.map";
C3DObject *nethertittle=0;

/* DRAWING REGION AROUND THE SHIP: */ 
float MINY=-8,MAXY=8,MINX=-8,MAXX=8;

/* Redrawing constant: */ 
const int REDRAWING_PERIOD=20;

/* Frames per second counter: */ 
int frames_per_sec=0;
int frames_per_sec_tmp=0;
int init_time=0;


/* Surfaces: */ 
SDL_Surface *screen_sfc;

NETHER *game=0;

void save_configuration(void);
void load_configuration(void);
int mainmenu_cycle(int width,int height);
void mainmenu_draw(int width,int height);

/*						AUXILIAR FUNCTION DEFINITION:							*/ 


void pause(unsigned int time)
{
	unsigned int initt=SDL_GetTicks();

	while((SDL_GetTicks()-initt)<time);
} /* pause */ 


SDL_Surface *initialization(int flags) 
{
    const SDL_VideoInfo* info=0;
    int bpp=0;
	SDL_Surface *screen;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0) {
        fprintf(stderr,"Video initialization failed: %s\n",SDL_GetError());
		return 0;
    } /* if */ 

    info=SDL_GetVideoInfo();

    if(!info) {
        fprintf(stderr,"Video query failed: %s\n",SDL_GetError());
		return 0;
    } /* if */ 

	if (fullscreen) {
		bpp=COLOUR_DEPTH;
	} else {
	    bpp=info->vfmt->BitsPerPixel;
	} /* if */ 

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,1);

    flags=SDL_OPENGL|flags;

	screen=SDL_SetVideoMode(SCREEN_X,SCREEN_Y,bpp,flags);
    if(screen==0) {
        fprintf(stderr,"Video mode set failed: %s\n",SDL_GetError());
		return 0;
    } /* if */ 

	pause(400);
	if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024)) {
		return 0;
	} /* if */ 

	SDL_WM_SetCaption("Nether Earth REMAKE v0.41", 0);
	SDL_ShowCursor(SDL_DISABLE);

	return screen;
} /* initialization */ 


void finalization()
{
	Mix_CloseAudio();
	SDL_Quit();
} /* finalization */ 



#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char** argv)
{
#endif

	int time,act_time;
	SDL_Event event;
    bool quit = false;

	load_configuration();

	screen_sfc = initialization((fullscreen ? SDL_FULLSCREEN : 0));
	if (screen_sfc==0) return 0;

	time=init_time=SDL_GetTicks();

	while (!quit) {
		while( SDL_PollEvent( &event ) ) {
            switch( event.type ) {
                /* Keyboard event */
                case SDL_KEYDOWN:
					if (event.key.keysym.sym==SDLK_F12) quit = true;

					if (event.key.keysym.sym==SDLK_RETURN) {
						SDLMod modifiers;

						modifiers=SDL_GetModState();

						if ((modifiers&KMOD_ALT)!=0) {
							/* Toogle FULLSCREEN mode: */ 
							if (game!=0) game->refresh_display_lists();
							if (nethertittle!=0) nethertittle->refresh_display_lists();
							if (game!=0) game->deleteobjects();
							if (fullscreen) fullscreen=false;
									   else fullscreen=true;
							SDL_QuitSubSystem(SDL_INIT_VIDEO);
							SDL_InitSubSystem(SDL_INIT_VIDEO);
							if (SDL_WasInit(SDL_INIT_VIDEO)) {
								SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
								SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
								SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
								SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
								SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
								SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,1);

								screen_sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL|(fullscreen ? SDL_FULLSCREEN : 0));
								if (game!=0) game->loadobjects();
								SDL_WM_SetCaption("Nether Earth REMAKE v0.41", 0);
								SDL_ShowCursor(SDL_DISABLE);
							} else {
								quit = true;
							} /* if */ 
						} /* if */ 
					} /* if */ 
                    break;

                /* SDL_QUIT event (window close) */
                case SDL_QUIT:
                    quit = true;
                    break;
            } /* switch */ 
        } /* while */ 

		act_time=SDL_GetTicks();
		if (act_time-time>=REDRAWING_PERIOD)
		{
			frames_per_sec_tmp+=1;
			if ((act_time-init_time)>=1000) {
				frames_per_sec=frames_per_sec_tmp;
				frames_per_sec_tmp=0;
				init_time=act_time;
			} /* if */ 

			do {
				time+=REDRAWING_PERIOD;
				if ((act_time-time)>4*REDRAWING_PERIOD) time=act_time;
			
				if (game!=0) {
					if (!game->gamecycle(SCREEN_X,SCREEN_Y)) {
						delete game;
						game=0;
						mainmenu_status=0;
						mainmenu_substatus=0;
					} /* if */  
				} else {
					int val=mainmenu_cycle(SCREEN_X,SCREEN_Y);
					if (val==1) game=new NETHER(mapname);
					if (val==2) quit=true;
					if (val==3) {
						if (game!=0) game->refresh_display_lists();
						if (nethertittle!=0) nethertittle->refresh_display_lists();
						if (game!=0) game->deleteobjects();
						SDL_QuitSubSystem(SDL_INIT_VIDEO);
						SDL_InitSubSystem(SDL_INIT_VIDEO);
						if (SDL_WasInit(SDL_INIT_VIDEO)) {
							
							SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
							SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
							SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
							SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
							SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
							SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,1);

							screen_sfc = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_OPENGL|(fullscreen ? SDL_FULLSCREEN : 0));
							if (game!=0) game->loadobjects();
							SDL_WM_SetCaption("Nether Earth REMAKE v0.41", 0);
							SDL_ShowCursor(SDL_DISABLE);
						} else {
							quit = true;
						} /* if */ 
					} /* if */ 
				} /* if */ 
				act_time=SDL_GetTicks();
			} while(act_time-time>=REDRAWING_PERIOD);

			if (game!=0) {
				game->gameredraw(SCREEN_X,SCREEN_Y);
			} else {
				mainmenu_draw(SCREEN_X,SCREEN_Y);
			} /* if */ 
		} /* if */ 
	} /* while */ 

	delete game;

	finalization();

	return 0;
}









