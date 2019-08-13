#ifndef __BRAIN_GLUTAUX
#define __BRAIN_GLUTAUX


float draw3Dtext(char *text,float dx,float dy,float dz,float r,float g,float b);

void glutSolidBox(float dx,float dy,float dz);

void glutPrint(char *str);
void glutPrintxy(float x,float y,char *str);

void Normal(double vector1[3],double vector2[3],double resultado[3]);
void Normalf(float vector1[3],float vector2[3],float resultado[3]);

float ObtainYAngle(Vector *v);

void ApplyMatrix(float v[4],float matrix[16],float out[4]);
void ApplyMatrix2(float v[4],float matrix[16],float out[4]);
void MulMatrix(float m2[16],float m1[16],float out[16]);

/* plane is a vector with the coeficients A,B,C,D of the plane equation */ 
/* p and v are a point of the line and the director vector				*/ 
/* crossp is where the collision point is returned						*/ 
bool PlaneLineCollision(float plane[4],float p[3],float v[3],float crossp[3]);

bool LineLineCollision(float p0[3],float v0[3],float p1[3],float v1[3],float *l1,float *l2);

void DistributeVector(Vector v,Vector v2,Vector *r1,Vector *r2);

double determinante_d(double m[9]);
float determinante_f(float m[9]);

bool kramer_d(double m[9],double c[3],double sol[3]);
bool kramer_f(float m[9],float c[3],float sol[3]);

bool gauss_solve_d(double *m,double *ctnt,double *s,int neq);

double det_d(double *m,int size);

double distancia_a_recta(Vector p,Vector pr,Vector vr);

unsigned int createTexture(char *fname,int x,int y,int sz);
unsigned int createTexture(char *fname);

#endif
