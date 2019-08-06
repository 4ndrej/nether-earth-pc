#ifdef _WIN32
#include "windows.h"
#endif
#include "GL/gl.h"

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "GL/gl.h"
#include "GL/glut.h"

void glprintf(const char *fmt, ...)						
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	int i,tl;

	if (fmt==0) return;

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	tl=strlen(text);
	for(i=0;i<tl;i++) {
		length+=glutStrokeWidth(GLUT_STROKE_MONO_ROMAN,text[i]);	
	} /* for /*/ 

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glNormal3f(0.0,0.0,1.0);
	for(i=0;i<tl;i++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,text[i]);
	} /* for */ 
} /* glprintf */ 



void scaledglprintf(float sx,float sy,const char *fmt, ...)						
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	int i,tl;

	if (fmt==0) return;

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	tl=strlen(text);
	for(i=0;i<tl;i++) {
		length+=glutStrokeWidth(GLUT_STROKE_MONO_ROMAN,text[i]);	
	} /* for /*/ 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(sx,sy,1.0f);

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glNormal3f(0.0,0.0,1.0);
	for(i=0;i<tl;i++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,text[i]);
	} /* for */ 

	glPopMatrix();
} /* scaledglprintf */ 



void scaledglprintf2(float sx,float sy,const char *fmt, ...)						
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	int i,tl;

	if (fmt==0) return;

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	tl=strlen(text);
	for(i=0;i<tl;i++) {
		length+=glutStrokeWidth(GLUT_STROKE_MONO_ROMAN,text[i]);	
	} /* for /*/ 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(sx,sy,1.0f);

	glNormal3f(0.0,0.0,1.0);
	for(i=0;i<tl;i++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,text[i]);
	} /* for */ 

	glPopMatrix();
} /* scaledglprintf2 */ 



void fittedglprintf(float sx,float sy,const char *fmt, ...)						
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	int i,tl;

	if (fmt==0) return;

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	tl=strlen(text);
	for(i=0;i<tl;i++) {
		length+=glutStrokeWidth(GLUT_STROKE_MONO_ROMAN,text[i]);	
	} /* for /*/ 

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(sx/length,sy/(119.05f+33.33f),1.0f);

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glNormal3f(0.0,0.0,1.0);
	for(i=0;i<tl;i++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,text[i]);
	} /* for */ 

	glPopMatrix();
} /* fittedglprintf */ 
