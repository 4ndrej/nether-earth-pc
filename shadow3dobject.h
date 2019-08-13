#ifndef __BRAIN_SHADOW3DOBJECT
#define __BRAIN_SHADOW3DOBJECT

class Shadow3DObject : public C3DObject {
public:
	Shadow3DObject();
	Shadow3DObject(char *file,char *texturedir);						/* Contruir a partir de un fichero		*/ 
	~Shadow3DObject();


	void DrawShadow(float r,float g,float b,float a);
	void ComputeShadow(Vector light);

	
	/* 2D Shadow: */ 
	int shdw_npuntos;
	int shdw_ncaras;
	float *shdw_puntos;
	int *shdw_caras;
	CMC shdw_cmc;
};

#endif
