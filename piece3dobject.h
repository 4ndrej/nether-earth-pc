#ifndef __BRAIN_PIECE3DOBJECT
#define __BRAIN_PIECE3DOBJECT

class Piece3DObject : public C3DObject {
public:
	Piece3DObject();
	Piece3DObject(char *file,char *texturedir);						/* Contruir a partir de un fichero		*/ 
	~Piece3DObject();

	void DrawShadow(int angle,Vector light,float r,float g,float b,float a);
	void ComputeFixedShadows(Vector light);
	void ComputeDynamicShadow(int angle,Vector light);
	void ComputeShadow(int angle,Vector light,int *np,int *nc,float **p,int **c,CMC *cmc);
	
	/* 2D Shadow: */ 
	int shdw_npuntos_0,shdw_npuntos_90,shdw_npuntos_180,shdw_npuntos_270,shdw_npuntos_dynamic;
	int shdw_ncaras_0,shdw_ncaras_90,shdw_ncaras_180,shdw_ncaras_270,shdw_ncaras_dynamic;
	float *shdw_puntos_0,*shdw_puntos_90,*shdw_puntos_180,*shdw_puntos_270,*shdw_puntos_dynamic;
	int *shdw_caras_0,*shdw_caras_90,*shdw_caras_180,*shdw_caras_270,*shdw_caras_dynamic;
	CMC shdw_cmc_0,shdw_cmc_90,shdw_cmc_180,shdw_cmc_270,shdw_cmc_dynamic;
};

#endif
