#ifndef DRIC_H
#define DRIC_H

using namespace std;

class menu;
typedef void (*function)();
typedef void (*updater)(menu*);
typedef char optionList[][30];
void doNothingUpdate (menu*);
void goBack (void);
class menu
{
	public:
		menu (int,const char []);
		menu (int,const char [],updater);
		~menu ();
		void setOptions (optionList);
		void setFunctions (function []);
		void setTitle (char []);
		void open (void);
	protected:
		void show (int);
		function input (int);
		char * title;
		updater update;
		int numOptions;
		char ** options;
		function * functions;
};
void EXIT (void);
/*char RECENT_STRING[10][50];
char RECENT_INT[10][50];
char RECENT_FLOAT[10][50];
char RECENT_DOUBLE[10][50];
char RECENT_CHAR[10][2];*/
enum Options{
	PASSWORD = 1<<0,
	NO_AUTO_NEWLINE = 1<<1,
	BEEP = 1<<2,
	AUTO_COMPLETE = 1<<3,
	LETTERS_ONLY = 1<<4,
	FILL = 1<<5,
	DEFAULT = 1<<6,
	RECENTS = 1<<7,
	HIGHLIGHT = 1<<8
};
enum Type{INT_,FLOAT_,STRING,BOOL_,DOUBLE_,CHAR_,INT_ARRAY};
void input (void*,int,Type);
void input (void*,int,Type,int);
void input (void*,int);
void input (void*,int,int);
void input (void*,int,Type,bool (*)(char));
void input (void*,int,Type,int,bool (*)(char));
void input (void*,int,bool (*)(char));
void input (void*,int,int,bool (*)(char));
void INPUT_ (void*,int,Type,int,bool (*)(char));
void SHOW_RECENTS (int,char []);
bool RETURN_FALSE (char);
void SHIFT_LEFT (char [],int);
bool SHIFT_RIGHT (char [],int,int);
void fillArr(char*,int);
void fillArr2(char**,int,int);
void ftoa (float,char*,int);
const char* substr (char*,int,int);
const char* substr (char*,int);
void gcf3 (int*,int*,int*);
int gcf (int,int);
void simplifyFrac (int*,int*);
int factorize (int,int[]);
int simplifySqrt (int);

#endif
