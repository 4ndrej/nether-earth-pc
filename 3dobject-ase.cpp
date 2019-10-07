#ifdef _WIN32
#include "windows.h"
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "string.h"
#include "stdio.h"

#include "cmc.h"
#include "3dobject.h"

#include "vector.h"
#include "myglutaux.h"


extern void Normal (double vector1[3],double vector2[3],double resultado[3]);


bool lookfor(char *tag,FILE *fp);
int  lookfor2(char *tag,char *tag2,FILE *fp);
bool lookforinside(char *tag,FILE *fp);
bool nexttag(char *tag,FILE *fp);
bool nexttaginside(char *tag,FILE *fp);
bool skipinfo(FILE *fp);
bool readcomment(char *data,FILE *fp);

bool C3DObject::loadASE(char *file,char *texturedir)
{
	int i=0,j=0;
	int *smooth=0;
	int *facematerial=0;
	int **materials=0,nmaterials=0,*nsubmaterials=0;
	char ***material_bitmaps=0;
	char buffer[256];
	FILE *fp;

	fp=fopen(file,"r+");
	if (fp==NULL) return false;

	/* Look for the materials: */ 
	if (!lookfor("MATERIAL_LIST",fp) ||
		!lookfor("MATERIAL_COUNT",fp)) {
		fclose(fp);
		return false;
	} /* if */ 
	
	if (1!=fscanf(fp,"%i",&nmaterials)) {
		fclose(fp);
		return false;
	} /* if */ 

	materials=new int *[nmaterials];
	nsubmaterials=new int[nmaterials];
	material_bitmaps=new char **[nmaterials];

	for(j=0;j<nmaterials;j++) {
		if (!lookfor("MATERIAL",fp) ||
			!lookfor("MATERIAL_CLASS",fp) ||
			!readcomment(buffer,fp)) {
			fclose(fp);
			return false;
		} /* if */ 

		if (strcmp(buffer,"Standard")==0) {
			/* Standard Material, has no Submaterials: */ 
			nsubmaterials[j]=1;
			materials[j]=new int[1];
			materials[j][0]=0;
			material_bitmaps[j]=new char *[1];
			material_bitmaps[j][0]=0;
			if (lookforinside("MAP_DIFFUSE",fp) &&
				lookfor("BITMAP",fp)) {
				char bmpname[256],bmpname2[256];
				int k,l;

				if (!readcomment(bmpname,fp)) {
					fclose(fp);
					return false;
				} /* if */ 

				for(l=0,k=0;bmpname[l]!=0;l++) {
					if (bmpname[l]=='\\' || bmpname[l]=='/') {
						k=l;
					} /* if */ 
				} /* for */ 
				if (k!=0) k++;

				sprintf(bmpname2,"%s/%s",texturedir,bmpname+k);
				material_bitmaps[j][0]=new char[strlen(bmpname2)+1];
				strcpy(material_bitmaps[j][0],bmpname2);
//				materials[j][0]=createTexture(bmpname2);
			} /* if */ 
		} else {
			/* Composed material, has submaterials: */ 
			if (!lookfor("NUMSUBMTLS",fp) ||
				1!=fscanf(fp,"%i",&(nsubmaterials[j]))) {
				fclose(fp);
				return false;
			} /* if */ 

			materials[j]=new int[nsubmaterials[j]];
			for(i=0;i<nsubmaterials[j];i++) materials[j][i]=0;
			material_bitmaps[j]=new char *[nsubmaterials[j]];
			for(i=0;i<nsubmaterials[j];i++) material_bitmaps[j][i]=0;

			for(i=0;i<nsubmaterials[j];i++) {
				if (!lookfor("SUBMATERIAL",fp)) {
					fclose(fp);
					return false;
				} /* if */ 
				if (lookforinside("MAP_DIFFUSE",fp) &&
					lookfor("BITMAP",fp)) {
					char bmpname[256],bmpname2[256];
					int k,l;

					if (!readcomment(bmpname,fp)) {
						fclose(fp);
						return false;
					} /* if */ 

					for(l=0,k=0;bmpname[l]!=0;l++) {
						if (bmpname[l]=='\\' || bmpname[l]=='/') {
							k=l;
						} /* if */ 
					} /* for */ 
					if (k!=0) k++;

					sprintf(bmpname2,"%s/%s",texturedir,bmpname+k);
					material_bitmaps[j][i]=new char[strlen(bmpname2)+1];
					strcpy(material_bitmaps[j][i],bmpname2);
//					materials[j][i]=createTexture(bmpname2);
				} /* if */ 
			} /* for */ 
		} /* if */ 
	} /* if */ 

	if (!lookfor("GEOMOBJECT",fp) ||
		!lookfor("MESH",fp)) {
		fclose(fp);
		return false;
	} /* if */ 

	if (!lookfor("MESH_NUMVERTEX",fp) ||
		1!=fscanf(fp,"%i",&npuntos)) {
		fclose(fp);
		return false;
	} /* if */ 

	puntos=new float[npuntos*3];
	for(i=0;i<npuntos*3;i++) puntos[i]=0.0F;

	if (!lookfor("MESH_NUMFACES",fp) ||
		1!=fscanf(fp,"%i",&ncaras)) {
		fclose(fp);
		return false;
	} /* if */ 
	
	caras=new int[ncaras*3];
	smooth=new int[ncaras];
	facematerial=new int[ncaras];
	r=new float[ncaras];
	g=new float[ncaras];
	b=new float[ncaras];
	for(i=0;i<ncaras*3;i++) {
		caras[i]=0;
	} /* for */ 
	for(i=0;i<ncaras;i++) {
		r[i]=0.5;
		g[i]=0.5;
		b[i]=0.5;
	} /* for */ 

	if (!lookfor("MESH_VERTEX_LIST",fp)) {
		fclose(fp);
		return false;
	} /* if */ 

	for(i=0;i<npuntos;i++) {
		int p;
		float x,y,z;

		if (!lookfor("MESH_VERTEX",fp) ||
			4!=fscanf(fp,"%i %f %f %f",&p,&x,&y,&z)) {
			fclose(fp);
			return false;
		} /* if */ 

		puntos[p*3]=x;
		puntos[p*3+1]=y;
		puntos[p*3+2]=z;
	} /* for */ 

	if (!lookfor("MESH_FACE_LIST",fp)) {
		fclose(fp);
		return false;
	} /* if */ 

	for(i=0;i<ncaras;i++) {
		int p1,p2,p3,s,mid;
		char buffer[32];

		if (!lookfor("MESH_FACE",fp) ||
			7!=fscanf(fp,"%s %s %i %s %i %s %i",buffer,buffer,&p1,buffer,&p2,buffer,&p3)) {
			fclose(fp);
			return false;
		} /* if */ 

		caras[i*3]=p1;
		caras[i*3+1]=p2;
		caras[i*3+2]=p3;

		if (!lookfor("MESH_SMOOTHING",fp) ||
			1!=fscanf(fp,"%i",&s)) {
			fclose(fp);
			return false;
		} /* if */ 
		smooth[i]=s;

		if (!lookfor("MESH_MTLID",fp) ||
			1!=fscanf(fp,"%i",&mid)) {
			fclose(fp);
			return false;
		} /* if */ 
		facematerial[i]=mid;

	} /* for */ 

	if (nmaterials!=0) {
		int v=lookfor2("MESH_FACEMAPLIST","MESH_NUMTVERTEX",fp);

		if (v==1) {
			/* MESH_FACEMAPLIST: */ 
			int f;
			float x,y,z;

			textures=new unsigned int[ncaras];
			tx=new float[ncaras*3];
			ty=new float[ncaras*3];

			for(i=0;i<ncaras;i++) {
				if (!lookfor("MESH_FACEMAP",fp) ||
					1!=fscanf(fp,"%i",&f)) {
					fclose(fp);
					return false;
				} /* if */ 

//				textures[f]=materials[facematerial[f]];
				if (!lookfor("MESH_FACEMAPVERT",fp) ||
					3!=fscanf(fp,"%f %f %f",&x,&y,&z)) {
					fclose(fp);
					return false;
				} /* if */ 
				tx[f*3]=x;
				ty[f*3]=1-y;
				if (!lookfor("MESH_FACEMAPVERT",fp) ||
					3!=fscanf(fp,"%f %f %f",&x,&y,&z)) {
					fclose(fp);
					return false;
				} /* if */ 
				tx[f*3+1]=x;
				ty[f*3+1]=1-y;
				if (!lookfor("MESH_FACEMAPVERT",fp) ||
					3!=fscanf(fp,"%f %f %f",&x,&y,&z)) {
					fclose(fp);
					return false;
				} /* if */ 
				tx[f*3+2]=x;
				ty[f*3+2]=1-y;
			} /* for */   
		} /* if */ 
		if (v==2) {
			/* MESH_NUMTVERTEX: */ 
			int ntv,ntf,n,p1,p2,p3;
			float *tv,x,y;

			if (1!=fscanf(fp,"%i",&ntv) ||
				!lookfor("MESH_TVERTLIST",fp)) {
				fclose(fp);
				return false;
			} /* if */ 

			tv=new float[ntv*2];
			for(i=0;i<ntv;i++) {
				if (!lookfor("MESH_TVERT",fp) ||
					3!=fscanf(fp,"%i %f %f",&n,&x,&y)) {
					fclose(fp);
					return false;
				} /* if */ 
				tv[n*2]=x;
				tv[n*2+1]=y;
			} /* for */ 

			if (!lookfor("MESH_NUMTVFACES",fp) ||
				1!=fscanf(fp,"%i",&ntf) ||
				!lookfor("MESH_TFACELIST",fp)) {
				fclose(fp);
				return false;
			} /* if */ 

			textures=new unsigned int[ncaras];
			tx=new float[ncaras*3];
			ty=new float[ncaras*3];

			for(i=0;i<ntf;i++) {
				if (!lookfor("MESH_TFACE",fp) ||
					4!=fscanf(fp,"%i %i %i %i",&n,&p1,&p2,&p3)) {
					fclose(fp);
					return false;
				} /* if */ 
				tx[n*3]=tv[p1*2];
				ty[n*3]=1-tv[p1*2+1];
				tx[n*3+1]=tv[p2*2];
				ty[n*3+1]=1-tv[p2*2+1];
				tx[n*3+2]=tv[p3*2];
				ty[n*3+2]=1-tv[p3*2+1];
//				textures[n]=materials[facematerial[n]];
			} /* for */ 

			delete tv;
		} /* if */ 

		/* Create all the materials used by the faces: */ 
		{
			int mid;

			if (!lookfor("MATERIAL_REF",fp) ||
				1!=fscanf(fp,"%i",&mid)) {
				fclose(fp);
				return false;
			} /* if */ 

			for(i=0;i<ncaras;i++) {
				if (facematerial[i]>nsubmaterials[mid]) facematerial[i]=0;
				if (materials[mid][facematerial[i]]==0 &&
					material_bitmaps[mid][facematerial[i]]!=0) {
					materials[mid][facematerial[i]]=createTexture(material_bitmaps[mid][facematerial[i]]);
				} /* if */ 
				textures[i]=materials[mid][facematerial[i]];
			} /* for */ 
		}	
	} /* if */ 

	for(j=0;j<nmaterials;j++) {
		delete materials[j];
		materials[j]=0;
		for(i=0;i<nsubmaterials[j];i++) {
			delete material_bitmaps[j][i];
			material_bitmaps[j][i]=0;
		} /* for */ 
		delete material_bitmaps[j];
		material_bitmaps[j]=0;
	} /* for */ 
	delete material_bitmaps;
	delete nsubmaterials;
	delete materials;

	CalculaNormales(smooth);
	cmc.set(puntos,npuntos);

	delete smooth;
	delete facematerial;

	fclose(fp);
	return true;
} /* C3DObject::loadASE */ 


bool nexttag(char *tag,FILE *fp)
{
	int c;

	do{
		c=fgetc(fp);
	}while(c!='*' && c!=EOF);
	if (c==EOF) return false;

	if (1!=fscanf(fp,"%s",tag)) return false;

	return true;
} /* nexttag */ 



bool nexttaginside(char *tag,FILE *fp)
{
	int parentheses=0;
	bool instring=false;
	int c;

	do{
		c=fgetc(fp);
		if (c=='\"') {
			if (instring) instring=false;
				     else instring=true;
		} /* if */ 
		if (!instring && c=='{') parentheses++;
		if (!instring && c=='}') parentheses--;
	}while(instring || (c!='*' && c!=EOF && parentheses>=0));
	if (c==EOF || parentheses<0) return false;

	if (1!=fscanf(fp,"%s",tag)) return false;

	return true;
} /* nexttaginside */ 


bool skipinfo(FILE *fp,int parentheses)
{
	bool instring=false;
	int c;

	do{
		c=fgetc(fp);
		if (c=='\"') {
			if (instring) instring=false;
				     else instring=true;
		} /* if */ 
		if (!instring && c=='{') parentheses++;
		if (!instring && c=='}') parentheses--;
	}while((c!='\n' || parentheses!=0 || instring==true) && c!=EOF);
	if (c==EOF) return false;

	return true;
} /* skipinfo */ 


bool lookfor(char *tag,FILE *fp)
{
	char tagname[256];

	for(;;) {
		if (!nexttag(tagname,fp)) return false;
		if (strcmp(tag,tagname)==0) return true;
		skipinfo(fp,0);
	} /* forever */ 

	return false;
} /* lookfor */ 


int lookfor2(char *tag,char *tag2,FILE *fp)
{
	char tagname[256];

	for(;;) {
		if (!nexttag(tagname,fp)) return 0;
		if (strcmp(tag,tagname)==0) return 1;
		if (strcmp(tag2,tagname)==0) return 2;
		skipinfo(fp,0);
	} /* forever */ 

	return 0;
} /* lookfor2 */ 



bool lookforinside(char *tag,FILE *fp)
{
	char tagname[256];

	for(;;) {
		if (!nexttaginside(tagname,fp)) return false;
		if (strcmp(tag,tagname)==0) return true;
		skipinfo(fp,0);
	} /* forever */ 

	return false;
} /* lookforinside */ 


bool readcomment(char *data,FILE *fp)
{
	int c;

	while(fgetc(fp)!='\"');

	do{
		c=fgetc(fp);
		*data=c;
		data++;
	}while(c!='\"' && c!=EOF);
	if (c==EOF) return false;
	data--;
	*data=0;

	return true;
} /* readcomment */ 
