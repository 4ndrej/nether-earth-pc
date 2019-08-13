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

#include "glprintf.h"

extern int detaillevel;
extern float MINY,MAXY,MINX,MAXX;


bool NETHER::loadmap(char *file)
{
	FILE *fp;
	int i;

	fp=fopen(file,"r");
	if (fp==0) return false;

	if (2!=fscanf(fp,"%i %i",&map_w,&map_h)) {
		fclose(fp);
		return false;
	} /* if */ 
	map=new int[map_w*map_h];
	if (map==0) {
		fclose(fp);
		return false;
	} /* if */ 

	for(i=0;i<map_w*map_h;i++) {
		int tile;
		char tilestr[16];
		char *tiles[]={"G","S","S2","M","H1",
			           "H2","H3","H4","H5","H6",
					   "GG","SS","MM","?"};
		bool found;

		if (1!=fscanf(fp,"%s",tilestr)) {
			fclose(fp);
			return false;
		} /* if */ 

		tile=0;
		found=false;
		while(!found && tiles[tile][0]!='?') {
			if (strcmp(tiles[tile],tilestr)==0) found=true;
			if (!found) tile++;
		} /* while */ 
		if (tile==10) tile=0;
		if (tile==11) tile=1;
		if (tile==12) tile=3;

		map[i]=tile;
	} /* for */ 

	{
		char tmp[80];

		while(1==fscanf(fp,"%s",tmp)) {
			BUILDING *b;
			float x,y;

			if (strcmp(tmp,"fence")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_FENCE;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"wall1")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_WALL1;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"wall2")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_WALL2;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"wall3")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_WALL3;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"wall4")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_WALL4;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"wall6")==0) {
				if (2!=fscanf(fp,"%f %f",&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y;
				b->pos.z=0;
				b->type=B_WALL6;
				b->owner=0;
				b->status=0;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"factory")==0) {
				char tmp2[80];
				int obj[4]={B_WALL4,B_WALL4,B_WALL2,B_WALL2},i;
				float xo[4]={0,0,1,1};
				float yo[4]={0,2,0,2};

				if (3!=fscanf(fp,"%f %f %s",&x,&y,tmp2)) {
					fclose(fp);
					return false;
				} /* if */ 

				for(i=0;i<4;i++) {
					b=new BUILDING();
					b->pos.x=x+xo[i];
					b->pos.y=y+yo[i];
					b->pos.z=0;
					b->type=obj[i];
					b->owner=0;
					b->status=0;
					buildings.Add(b);
				} /* for */ 

				b=new BUILDING();
				b->pos.x=x;
				b->pos.y=y+1;
				b->pos.z=0;
				b->type=B_FACTORY_ELECTRONICS;
				b->owner=0;
				b->status=0;
				if (strcmp(tmp2,"electronics")==0) b->type=B_FACTORY_ELECTRONICS;
				if (strcmp(tmp2,"nuclear")==0) b->type=B_FACTORY_NUCLEAR;
				if (strcmp(tmp2,"phasers")==0) b->type=B_FACTORY_PHASERS;
				if (strcmp(tmp2,"missiles")==0) b->type=B_FACTORY_MISSILES;
				if (strcmp(tmp2,"cannons")==0) b->type=B_FACTORY_CANNONS;
				if (strcmp(tmp2,"chassis")==0) b->type=B_FACTORY_CHASSIS;
				buildings.Add(b);
			} /* if */  
			if (strcmp(tmp,"warbase")==0) {
				int obj[15]={B_WALL4,B_WALL5,
							 B_WALL4,B_WALL1,B_WALL1,B_WALL2,
							 B_WALL4,B_WARBASE,B_WALL2,
							 B_WALL4,B_WALL1,B_WALL1,B_WALL2,
							 B_WALL4,B_WALL5},i;
				float xo[15]={0.5,1.5,
						 	  0,1,2,3,
							  0.5,1.5,2.5,
							  0,1,2,3,
							  0.5,1.5};
				float yo[15]={0,0,
							  1,1,1,1,
							  2,2,2,
							  3,3,3,3,
							  4,4};
				int o=0;
				
				if (3!=fscanf(fp,"%f %f %i",&x,&y,&o)) {
					fclose(fp);
					return false;
				} /* if */ 

				for(i=0;i<15;i++) {
					b=new BUILDING();
					b->pos.x=x+xo[i];
					b->pos.y=y+yo[i];
					b->pos.z=0;
					b->type=obj[i];
					b->owner=o;
					b->status=0;
					buildings.Add(b);
				} /* for */ 
			} /* if */  
		} /* while */ 
	}

	fclose(fp);
	return true;
} /* NETHER::loadmap */ 


void NETHER::drawmap(bool shadows)
{
	int i,j;
	int o,m[8]={13,15,17,19,7,23,21,25};
	List<BUILDING> l;
	BUILDING *b;
	Vector light;

	light.x=lightpos[0];
	light.y=lightpos[1];
	light.z=lightpos[2];
	light=light/light.z;

	glMatrixMode(GL_MODELVIEW);
	
	if (!shadows) {
		glPushMatrix();
		for(j=0;j<map_h;j++) {
			if (j>=(viewp.y+MINY) &&
				j<=(viewp.y+MAXY)) {
				glPushMatrix();
				for(i=0;i<map_w;i++) {
					if (i>=(viewp.x+MINX) &&
						i<=(viewp.x+MAXX)) {
						o=map[i+j*map_w];

						if (o==0) {
							/* GRASS: */ 
							if (((i*3+j*7)%m[(i+j)%8])==0) o=10;
							if (((i*3+j*7)%m[(i+j+1)%8])==0) o=11;
						} else {
						} /* if */ 
						if (detaillevel>=1) tile[o]->draw(tile_r[o],tile_g[o],tile_b[o]);
									    else tile[o]->draw_notexture(tile_r[o],tile_g[o],tile_b[o]);
					} /* if */ 
					glTranslatef(1.0,0.0,0.0);
				} /* for */ 
				glPopMatrix();
			} /* if */ 
			glTranslatef(0.0,1.0,0.0);
		} /* for */ 
		glPopMatrix();
	} /* if */ 

	l.Instance(buildings);
	l.Rewind();
	while(l.Iterate(b)) {	

		if (b->pos.y>=(viewp.y+MINY) &&
			b->pos.y<=(viewp.y+MAXY) &&
			b->pos.x>=(viewp.x+MINX) &&
			b->pos.x<=(viewp.x+MAXX)) {
			glPushMatrix();
			glTranslatef(float(b->pos.x),float(b->pos.y),float(b->pos.z));
			switch(b->type) {
			case B_FENCE:if (!shadows) {
							if (detaillevel>=2) building_tile[5]->draw(0.2f,0.2f,0.2f);
											else building_tile[5]->draw_notexture(0.2f,0.2f,0.2f);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[5]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL1:if (!shadows) {
							if (detaillevel>=2) building_tile[0]->draw(0.5,0.5,0.5);
											else building_tile[0]->draw_notexture(0.5,0.5,0.5);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[0]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL2:if (!shadows) {
							if (detaillevel>=2) building_tile[1]->draw(0.5,0.5,0.5);
											else building_tile[1]->draw_notexture(0.5,0.5,0.5);
						 } else { 
							 glTranslatef(0,0,0.05f);
							 building_tile[1]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL3:if (!shadows) {
							if (detaillevel>=2) building_tile[2]->draw(0.3f,0.3f,0.3f);
											else building_tile[2]->draw_notexture(0.5,0.5,0.5);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[2]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL4:if (!shadows) {
							if (detaillevel>=2) building_tile[3]->draw(0.5,0.5,0.5);
											else building_tile[3]->draw_notexture(0.5,0.5,0.5);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[3]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL5:if (!shadows) {
							if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											else building_tile[4]->draw_notexture(0.5,0.5,0.5);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_WALL6:if (!shadows) {
							if (detaillevel>=2) building_tile[7]->draw(0.3f,0.3f,0.3f);
											else building_tile[7]->draw_notexture(0.5,0.5,0.5);
						 } else {
							 glTranslatef(0,0,0.05f);
							 building_tile[7]->DrawShadow(0,0,0,0.5);
						 } /* if */ 
						 break;
			case B_FACTORY_ELECTRONICS:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][7]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][7]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break;
			case B_FACTORY_NUCLEAR:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][6]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][6]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break;
			case B_FACTORY_PHASERS:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][5]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][5]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break;
			case B_FACTORY_MISSILES:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][4]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][4]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break;
			case B_FACTORY_CANNONS:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][3]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][3]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break; 
			case B_FACTORY_CHASSIS:
						 if (!shadows) {
							 if (detaillevel>=2) building_tile[4]->draw(0.5,0.5,0.5);
											 else building_tile[4]->draw_notexture(0.5,0.5,0.5);
							 glPushMatrix();
							 glTranslatef(0.5,0.5,1);
							 piece_tile[0][1]->draw_notexture(0.8f,0.8f,0.8f);
							 glPopMatrix();
						 } else {
							 glPushMatrix();
							 glTranslatef(0,0,0.05f);
							 building_tile[4]->DrawShadow(0,0,0,0.5);
							 glTranslatef(float(-light.x),float(-light.y),0);
							 piece_tile[0][1]->DrawShadow(0,lightposv,0,0,0,0.5);
							 glPopMatrix();
						 } /* if */ 

						 if (b->owner==1) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.3f,0.3f,0.8f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 if (b->owner==2) {
							if (!shadows) {
								glTranslatef(0,-1,1);
								building_tile[6]->draw(0.8f,0.3f,0.3f);
							} else {
								glTranslatef(float(-light.x),float(-light.y)-1,0.05f);
								building_tile[6]->DrawShadow(0,0,0,0.5);
							} /* if */ 
						 } /* if */ 
						 break;

			case B_WARBASE:if (!shadows) {
							   if (detaillevel>=2) building_tile[8]->draw(0.5,0.5,0.5);
											   else building_tile[8]->draw_notexture(0.5,0.5,0.5);
						   } else {
							   glTranslatef(0,0,0.05f);
							   building_tile[8]->DrawShadow(0,0,0,0.5);
						   } /* if */ 

						   if (b->owner==1) {
							   if (!shadows) {
								   glTranslatef(0,-2,1);
								   building_tile[6]->draw(0.3f,0.3f,0.8f);
							   } else {
								   glTranslatef(float(-light.x),float(-light.y)-2,0.05f);
								   building_tile[6]->DrawShadow(0,0,0,0.5);
							   } /* if */ 
						   } /* if */ 
						   if (b->owner==2) {
							  if (!shadows) {
								 glTranslatef(0,-2,1);
								 building_tile[6]->draw(0.8f,0.3f,0.3f);
							  } else {
								 glTranslatef(float(-light.x),float(-light.y)-2,0.05f);
								 building_tile[6]->DrawShadow(0,0,0,0.5);
							  } /* if */ 
						   } /* if */ 
						   break;
			} /* switch */ 
			glPopMatrix();
		} /* if */ 
	} /* while */ 

} /* drawmap */ 


float NETHER::MapMaxZ(float x[2],float y[2])
{
	int i,j;
	float z=0;
	int o;

	for(i=int(x[0]);float(i)<x[1];i++) {
		for(j=int(y[0]);float(j)<y[1];j++) {
			if (i>=0 && i<map_w &&
				j>=0 && j<map_h) {
				o=map[i+j*map_w];
				if (tile[o]->cmc.z[0]>z) z=tile[o]->cmc.z[0];
				if (tile[o]->cmc.z[1]>z) z=tile[o]->cmc.z[1];
			} /* if */ 
		} /* for */ 
	} /* for */ 

	return z;
} /* MapMaxZ */ 


int NETHER::MapTerrain(float x, float y)
{
	switch(map[int(x)+int(y)*map_w]) {
	case 0:
		return T_GRASS;
		break;
	case 1:
	case 2:
		return T_SAND;
		break;
	case 3:
		return T_MOUNTAINS;
		break;
	default:
		return T_HOLE;
		break;
	} /* switch */ 
} /* NETHER::MapTerrain */ 


int NETHER::WorseMapTerrain(float x[2], float y[2])
{
	int t,t2;

	t=MapTerrain(x[0]+0.001f,y[0]+0.001f);
	t2=MapTerrain(x[1]-0.001f,y[0]+0.001f);
	if (t2==T_HOLE || t==T_HOLE) return T_HOLE;
	if ((t2==T_MOUNTAINS && (t==T_SAND || t==T_GRASS)) ||
		(t2==T_SAND && t==T_GRASS)) t=t2;
	t2=MapTerrain(x[0]+0.001f,y[1]-0.001f);
	if (t2==T_HOLE) return T_HOLE;
	if ((t2==T_MOUNTAINS && (t==T_SAND || t==T_GRASS)) ||
		(t2==T_SAND && t==T_GRASS)) t=t2;
	t2=MapTerrain(x[1]-0.001f,y[1]-0.001f);
	if (t2==T_HOLE) return T_HOLE;
	if ((t2==T_MOUNTAINS && (t==T_SAND || t==T_GRASS)) ||
		(t2==T_SAND && t==T_GRASS)) t=t2;
	return t;
} /* NETHER::WorseMapTerrain */ 

