#ifndef GLPRINTF_HEADER
#define GLPRINTF_HEADER

void glprintf(const char *fmt, ...);
void scaledglprintf(float sx,float sy,const char *fmt, ...);	/* center alignment */ 
void scaledglprintf2(float sx,float sy,const char *fmt, ...);	/* left alignment */ 
void fittedglprintf(float sx,float sy,const char *fmt, ...);

#endif
