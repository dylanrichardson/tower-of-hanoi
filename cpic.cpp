#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include "cpic.h"
#include <conio.h>
#include <math.h>

using namespace std;

HANDLE hConsoleb2c;

CPIC::CPIC(int w, int h)
{
	hConsoleb2c = GetStdHandle(STD_OUTPUT_HANDLE);
	colors = new unsigned char [(w*h)+1];
	width = w;
	height = h;
	
}

void CPIC::save(ofstream& of) 
{
	of.write((char*)colors, width*height*sizeof(unsigned char));
	of.write((char*)&width, sizeof(int)); 
	of.write((char*)&height, sizeof(int)); 
}

void CPIC::load(ifstream& inf) 
{ 
	inf.read((char*)colors, width*height*sizeof(unsigned char)); 
	inf.read((char*)&width, sizeof(int)); 
	inf.read((char*)&height, sizeof(int)); 
}

void CPIC::setColors(unsigned char *c) 
{ 
	 memcpy(colors, c, width*height);
}

void CPIC::display (void)
{
	//hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			SetConsoleTextAttribute(hConsoleb2c,colors[width*y+x]);
			cout<<" ";
			SetConsoleTextAttribute(hConsoleb2c,7);
		}
		cout<<endl;
	}
}

void CPIC::display (short x, short y)
{
	//hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (short j=0; j<height; j++)
	{
		SetConsoleCursorPosition(hConsoleb2c,{x,(short)(y+j)});
		for (short i=0; i<width; i++)
		{
			SetConsoleTextAttribute(hConsoleb2c,colors[width*j+i]);
			cout<<" ";
			SetConsoleTextAttribute(hConsoleb2c,7);
		}
		//cout<<endl;
	}
}

void CPIC::displayAlpha (short x, short y)
{
	//hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int j=0; j<height; j++)
	{
		SetConsoleCursorPosition(hConsoleb2c,{x,(short)(y+j)});
		for (int i=0; i<width; i++)
		{
			if (colors[width*j+i]==-1)
				SetConsoleCursorPosition(hConsoleb2c,{(short)(x+i+1),(short)(y+j)});
			else
			{
				SetConsoleTextAttribute(hConsoleb2c,colors[width*j+i]);
				cout<<" ";
			}
			SetConsoleTextAttribute(hConsoleb2c,7);
		}
		//cout<<endl;
	}
}

int CPIC::getSize ()
{
	return width*height;
}

CPIC* bmp2cpic (char *path)
{
	HBITMAP hBmp = (HBITMAP) LoadImage(GetModuleHandle(NULL), _T(path), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	if(!hBmp) // failed to load bitmap
	{
		cout<<"Failed to load bmp"<<endl;
	    return NULL;
	}
	
	//getting the size of the picture
	BITMAP bm;
	GetObject(hBmp, sizeof(bm), &bm);
	int width(bm.bmWidth),
	    height(bm.bmHeight);
	
	//creating a bitmapheader for getting the dibits
	BITMAPINFOHEADER bminfoheader;
	::ZeroMemory(&bminfoheader, sizeof(BITMAPINFOHEADER));
	bminfoheader.biSize        = sizeof(BITMAPINFOHEADER);
	bminfoheader.biWidth       = width;
	bminfoheader.biHeight      = -height;
	bminfoheader.biPlanes      = 1;
	bminfoheader.biBitCount    = 32;
	bminfoheader.biCompression = BI_RGB;
	
	bminfoheader.biSizeImage = width * 4 * height;
	bminfoheader.biClrUsed = 0;
	bminfoheader.biClrImportant = 0;
	
	//create a buffer and let the GetDIBits fill in the buffer
	unsigned char* pPixels = new unsigned char[(width * 4 * height)];
	if( !GetDIBits(CreateCompatibleDC(0), hBmp, 0, height, pPixels, (BITMAPINFO*) &bminfoheader, DIB_RGB_COLORS)) // load pixel info 
	{ 
	    //return if fails but first delete the resources
	    DeleteObject(hBmp);
	    delete [] pPixels; // delete the array of objects
		
		cout<<"Failed to GetDiBits"<<endl;
	    return NULL;
	}
	unsigned char *colors = new unsigned char [width*height+1];
	int rgb[16][3] = {{0,0,0},{0,0,128},{0,128,0},{0,128,128},
					  {128,0,0},{128,0,128},{128,128,0},{192,192,192},
					  {128,128,128},{0,0,255},{0,255,0},{0,255,255},
					  {255,0,0},{255,0,255},{255,255,0},{255,255,255}};
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			int diff = 255*255*3;
			int low = diff;
			colors[width*y+x] = 0;
			for (int i=0; i<16; i++)
			{
				diff = pow(rgb[i][0]-pPixels[(width*y+x)*4+2],2)+pow(rgb[i][1]-pPixels[(width*y+x)*4+1],2)+pow(rgb[i][2]-pPixels[(width*y+x)*4+0],2);
				//if (pPixels[(width*y+x)*4+2]<200)
				//	cout<<"Diff: "<<diff<<endl;
				if (diff<low)
				{
					colors[width*y+x] = 16*i;
					low = diff;
				}
			}
			//if (pPixels[(width*y+x)*4+2]<200)
			//	cout<<"X: "<<x<<" Y: "<<y<<" R: "<<(int)pPixels[(width*y+x)*4+2]<<" G: "<<(int)pPixels[(width*y+x)*4+1]<<" B: "<<(int)pPixels[(width*y+x)*4]<<" #: "<<(int)colors[width*y+x]<<endl;
			//cout<<(int)colors[width*y+x]<<endl;
		}
	}
	CPIC *cpic = new CPIC (width,height);
	cpic->setColors(colors);
	DeleteObject(hBmp);
	delete [] pPixels;
	delete [] colors;
	return cpic;
}
