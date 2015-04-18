#ifndef CPIC_H
#define CPIC_H

using namespace std;

class CPIC
{
	private:
		unsigned char *colors;
		int width;
		int height;
	public:
		CPIC (int w, int h); 
	    void save(ofstream& of); 
	    void load(ifstream& inf);
	    void setColors (unsigned char *c);
		void display (void);
		void display (short,short);
		void displayAlpha (short,short);
		int getSize ();
};

CPIC* bmp2cpic (char*);

#endif
