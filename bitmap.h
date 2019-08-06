#ifndef __BRAIN_BITMAP
#define __BRAIN_BITMAP

class Bitmap {
public:
	Bitmap(char *file);
	~Bitmap();

	bool valid(void) {return (r!=0 && g!=0 && b!=0);};

	int getdx(void) {return dx;};
	int getdy(void) {return dy;};

	unsigned char getr(int i) {return r[i];};
	unsigned char getg(int i) {return g[i];};
	unsigned char getb(int i) {return b[i];};

private:
	int dx,dy;
	int bitcount;
	unsigned char *r,*g,*b;
};


#endif
