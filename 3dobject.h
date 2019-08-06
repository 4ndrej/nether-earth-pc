#ifndef __BRAIN_3DOBJECT
#define __BRAIN_3DOBJECT

class C3DObject {
public:
	C3DObject();
	C3DObject(char *file,char *texturedir);					/* Contruir a partir de un fichero		*/ 
	~C3DObject();

	void refresh_display_lists(void);

	bool loadASC(char *file);
	bool loadASE(char *file,char *texturedir);

	bool valid(void);
	void draw(void);
	void draw(float r,float g,float b);
	void draw_notexture(float r,float g,float b);
	void draw_notexture(float r,float g,float b,float a);

	void drawcmc(float r,float g,float b);

	void CalculaNormales(int *smooth);

	float normalize(void);						/* Sets the maximum axis size to 1.0 or -1.0 */ 
	float normalize(float c);					/* Sets the maximum axis size to c or -c */ 
	float normalizexy(float c);					/* Sets the maximum X and Y axis size to c or -c */ 
	void makepositive(void);					/* Sets the minimum coordinates to 0,0,0 */ 
	void makepositivex(void);					/* Sets the minimum coordinates in X axis to 0 */ 
	void makepositivey(void);					/* Sets the minimum coordinates in Y axis to 0 */ 
	void makepositivez(void);					/* Sets the minimum coordinates in Z axis to 0 */ 
	void moveobject(float x,float y,float z);


	int npuntos;
	int ncaras;
	float *puntos;
	float *normales;
	int *caras;

	float *r,*g,*b;	/* Color de las caras: */ 

	int display_list;

	CMC cmc;

	float *tx,*ty;
	unsigned int *textures;
};

#endif
