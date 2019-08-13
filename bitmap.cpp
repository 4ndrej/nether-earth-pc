#include "stdio.h"

#include "bitmap.h"

Bitmap::Bitmap(char *file)
{
	FILE *fp;
	int i,j;

	dx=0;
	dy=0;
	bitcount=24;
	r=0;
	g=0;
	b=0;

	fp=fopen(file,"rb");
	if (fp==NULL) return;

	/* Tag: */ 
	if (fgetc(fp)!='B' || fgetc(fp)!='M') return;

	/* Saltarse Header: */ 
	for(i=0;i<12;i++) fgetc(fp);

	/* Info-Header: */ 
	for(i=0;i<4;i++) fgetc(fp);

	dx=fgetc(fp);
	dx+=fgetc(fp)<<8;
	fgetc(fp);
	fgetc(fp);
	dy=fgetc(fp);
	dy+=fgetc(fp)<<8;
	fgetc(fp);
	fgetc(fp);

	r=new unsigned char[dx*dy];
	g=new unsigned char[dx*dy];
	b=new unsigned char[dx*dy];

	for(i=0;i<28;i++) fgetc(fp);

	for(i=dy-1;i>=0;i--) {
		for(j=0;j<dx;j++) {
			b[i*dx+j]=fgetc(fp);
			g[i*dx+j]=fgetc(fp);
			r[i*dx+j]=fgetc(fp);
//			fgetc(fp);
		} /* for */ 
		if ((dx*3)%4!=0) {
			for(j=0;j<(4-(dx*3)%4);j++) fgetc(fp);
		} /* if */ 
	} /* for */ 

	fclose(fp);

} /* Bitmap::Bitmap */ 


Bitmap::~Bitmap()
{
	delete []r;
	delete []g;
	delete []b;
} /* Bitmap::~Bitmap */ 

