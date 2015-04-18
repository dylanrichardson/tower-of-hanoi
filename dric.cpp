/*
Dylan Richardson's Custom Functions File

* create menus
* get user input of any type with exceptions
* miscellaneous

Creating a menu
---------------

	menu <menu_var> (<number_of_menu_options>, <menu_title>, <update_function>(OPTIONAL));
	optionList <options_var> = {<first_option>, <second_option>, ..., <last_option>};
	function <functions_var> [] = {<first_option_function>, <second_option_function>, ..., <last_option_function>};
	<menu_var>.setOptions(<options_var>);
	<menu_var>.setFunctions(<functions_var>);
	<menu_var>.open();

User Input
----------

	<Type> <destination_var>;
	input(<destination_var>, <length_of_input>, <type_enum>(OPTIONAL), <options_flags>(OPTIONAL), <exception_function>(OPTIONAL));
	
	<type_enum>:
	STRING   -   char[]
	INT_     -   int
	FLOAT_   -   float
	DOUBLE_  -   double
	CHAR_    -   char
	
	<options_flags>:
	PASSWORD         -    hide characters with *
	NO_AUTO_NEWLINE  -    does not write newline after input is complete
	BEEP             -    makes beeping noise if invalid character is typed
	AUTO_COMPLETE    -    finishes input once <length_of_input> characters have been typed
	LETTERS_ONLY     -    only letters are valid characters
	FILL             -    input only finishes once <length_of_input> characters have been typed
	RECENTS          -    press up and down arrow keys to get recent inputs
	HIGHLIGHT        -    highlight the input
	DEFAULT          -    all <options_flags> are turned off


Miscellaneous
-------------

	void EXIT (void)                -    exits program
	typedef void (*function)()      -    function variable
	typedef void (*updater)(menu*)  -    update function variable
	typedef char optionList[][30]   -    list of menu options variable

*/

#include <iostream>
#include "dric.h"
#include <conio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
//#include <algorithm>

using namespace std;

void doNothingUpdate (menu *m) {}
void goBack (void) {}

HANDLE hConsole;

menu::menu(int num,const char name[])
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	title = new char [strlen(name)];
	strcpy(title,name);
	numOptions = num;
	options = new char* [numOptions];
	for (int i=0; i<numOptions; i++)
		options[i] = new char [30];
	functions = new function [num];
	update = doNothingUpdate;
}

menu::menu(int num,const char name[],updater func)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	title = new char [strlen(name)];
	strcpy(title,name);
	numOptions = num;
	options = new char* [numOptions];
	for (int i=0; i<numOptions; i++)
		options[i] = new char [30];
	functions = new function [num];
	update = func;
}

menu::~menu ()
{
	delete[] options;
	delete[] functions;
}

void menu::setOptions (optionList names)
{
	for (int i=0; i<numOptions; i++)
		strcpy(options[i],names[i]);
}

void menu::setFunctions (function funcs[])
{
	for (int i=0; i<numOptions; i++)
		functions[i] = funcs[i];
}

void menu::setTitle (char name[])
{
	title = new char [strlen(name)];
	strcpy(title,name);
}

void menu::open ()
{
	function f;
	system("cls");
	cout<<title<<endl<<endl;
	show(1);
	f = input(1);
	f();
	update(this);
	if (f!=goBack)
		open();
}

void menu::show (int num)
{
	COORD pos = {0,2};
	SetConsoleCursorPosition(hConsole,pos);
	int i=0;
	for (i; i<num-1; i++)
		cout<<i+1<<". "<<options[i]<<endl;
	SetConsoleTextAttribute(hConsole,112);
	cout<<++i<<". "<<options[i]<<endl; /* set to options[i-1] on dev-c++ 4.x  */
	SetConsoleTextAttribute(hConsole,7);
	for (i; i<numOptions; i++)
		cout<<i+1<<". "<<options[i]<<endl;
}

function menu::input (int num)
{
	int c = 0;
	do
	{
		if (kbhit())
			c = getch();
	} while (c!=72&&c!=80&&c!=13&&(c<49||c>57));
	if (c==72)
	{
		if (--num<1)
			num = numOptions;
		show(num);
		return input(num);
	}
	else if (c==80)
	{
		if (++num>numOptions)
			num = 1;
		show(num);
		return input(num);
	}
	else if (c==13)
		return functions[num-1];
	else
	{
		if (c-49<numOptions)
			num = c-48;
		show(num);
		return input(num);
	}
}

void EXIT (void)
{
	return;//exit(0);
}

bool RETURN_FALSE (char ch)
{
	return false;
}

void input (void *dest,int length,Type type)
{
	INPUT_(dest,length,type,DEFAULT,RETURN_FALSE);
}

void input (void *dest,int length,Type type,int options)
{
	INPUT_(dest,length,type,options,RETURN_FALSE);
}

void input (void *dest,int length)
{
	INPUT_(dest,length,STRING,DEFAULT,RETURN_FALSE);
}

void input (void *dest,int length,int options)
{
	INPUT_(dest,length,STRING,options,RETURN_FALSE);
}

void input (void *dest,int length,Type type,bool (*except)(char))
{
	INPUT_(dest,length,type,DEFAULT,except);
}

void input (void *dest,int length,Type type,int options,bool (*except)(char))
{
	INPUT_(dest,length,type,options,except);
}

void input (void *dest,int length,bool (*except)(char))
{
	INPUT_(dest,length,STRING,DEFAULT,except);
}

void input (void *dest,int length,int options,bool (*except)(char))
{
	INPUT_(dest,length,STRING,options,except);
}

void INPUT_ (void *dest,int length,Type type,int options,bool (*except)(char))
{
	if (type==INT_ARRAY||type==BOOL_)
		return;
	char temp[length];
	for (int i=0;i<length;i++)
		temp[i] = '\0';
	char ch;
	if (type!=INT_||type!=FLOAT_||type!=DOUBLE_)
		length--;
	int i = 0;
	int recents = 0;
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (options&HIGHLIGHT)
	{
		SetConsoleTextAttribute(hConsole,112);
		for (int j=0;j<length;j++)
			cout<<" ";
		for (int j=0;j<length;j++)
			cout<<"\b";
	}
	while (!((options&AUTO_COMPLETE)&&i>=length))
	{
		ch=getch();
		if (ch==8)
		{
			if (i>0)
			{
				if (temp[i]!='\0')
				{
					SHIFT_LEFT(temp,i);
					cout<<"\b";
					for (int j=i-1;j<length+1;j++)
						cout<<((options&PASSWORD)?(char)254:temp[j]);
					for (int j=i-1;j<length+1;j++)
						cout<<"\b";
					i--;
				}
				else
				{
					temp[--i] = '\0';
					cout<<"\b \b";
				}
			}
			else
				cout<<(options&BEEP?"\a":"");
		}
		else if (ch==13&&(!(options&FILL)||i>=length))
			break;
		else if (type==INT_&&(ch<48||ch>57)&&ch!=45)
			cout<<(options&BEEP?"\a":"");
		else if ((type==FLOAT_||type==DOUBLE_)&&(ch<48||ch>57)&&ch!=46&&ch!=45)
			cout<<(options&BEEP?"\a":"");
		else if ((options&LETTERS_ONLY)&&!(ch>=65&&ch<=90)&&!(ch>=97&&ch<=122))
			cout<<(options&BEEP?"\a":"");
		else if (except(ch))
			cout<<(options&BEEP?"\a":"");
		else if (ch==-32)
		{
			if (kbhit())
			{
				ch = getch();
				if (ch==72&&(options&RECENTS))
					SHOW_RECENTS(recents++,temp);
				else if (ch==80&&(options&RECENTS))
					SHOW_RECENTS(recents--,temp);
				else if (ch==75&&i>0)
				{
					cout<<"\b";
					i--;
				}
				else if (ch==77&&temp[i]!='\0')
				{
					cout<<temp[i];
					i++;
				}
			}
		}
		else if (i>=length)
			cout<<(options&BEEP?"\a":"");
		else
		{
			if (temp[i]!='\0')
			{
				if (SHIFT_RIGHT(temp,i,length))
				{
					cout<<((options&PASSWORD)?(char)254:ch);
					for (int j=i+1;j<length+1;j++)
						cout<<((options&PASSWORD)?(char)254:temp[j]);
					for (int j=i+1;j<length+1;j++)
						cout<<"\b";
					temp[i] = ch;
					i++;
				}
				else
					cout<<(options&BEEP?"\a":"");
			}
			else
			{
				cout<<((options&PASSWORD)?(char)254:ch);
				temp[i] = ch;
				i++;
			}
		}
		
	}
	if (options&HIGHLIGHT)
	{
		SetConsoleTextAttribute(hConsole,7);
		for (int j=i;j<length;j++)
			cout<<" ";
		for (int j=0;j<length;j++)
			cout<<"\b";
		cout<<temp;
	}
	if (!(options&NO_AUTO_NEWLINE))
		cout<<endl;
	switch (type)
	{
		case STRING:
			strcpy(static_cast<char*>(dest),temp);
			break;
		case INT_:
			*static_cast<int*>(dest) = atoi(temp);
			break;
		case FLOAT_:
			*static_cast<float*>(dest) = atof(temp);
			break;
		case DOUBLE_:
			*static_cast<double*>(dest) = atof(temp);
			break;
		case CHAR_:
			*static_cast<char*>(dest) = temp[0];
			break;
	}
}

void SHOW_RECENTS (int num,char temp[])
{
	//cout<<num;	
}

void SHIFT_LEFT (char temp[],int i)
{
	if (temp[i]=='\0')
	{
		temp[i-1] = '\0';
		return;
	}
	else
	{
		temp[i-1] = temp[i];
		SHIFT_LEFT(temp,++i);
	}
}

bool SHIFT_RIGHT (char temp[],int i,int length)
{
	if (temp[length-1]!='\0')
	{
		return false;
	}
	else if (temp[i+1]=='\0')
	{
		temp[i+1] = temp[i];
		return true;
	}
	else
	{
		bool ret = SHIFT_RIGHT(temp,i+1,length);
		temp[i+1] = temp[i];
		return ret;
	}
}

void fillArr (char *arr,int x)
{
	for (int i=0;i<x;i++)
		arr[i] = '\0';
}

void fillArr2 (char **arr,int y,int x)
{
	for (int i=0;i<y;i++)
		for (int j=0;j<x;j++)
			arr[i][j] = '\0';
}

void ftoa (float oldNum,char *str,int dec)
{
	ostringstream num;
	num << oldNum;
	strncpy(str,num.str().c_str(),dec);
	str[dec] = '\0';
}

const char* substr (char* str,int index,int length)
{
	string temp = str;
	return temp.substr(index,length).c_str();
}

const char* substr (char* str,int index)
{
	string temp = str;
	return temp.substr(index).c_str();
}

void gcf3 (int *a,int *b,int *c)
{
	bool aswitch = false, bswitch = false, cswitch = false;
	if (*a<0)
	{
		*a = -(*a);
		aswitch = true;
	}
	if (*b<0)
	{
		*b = -(*b);
		bswitch = true;
	}
	if (*c<0)
	{
		*c = -(*c);
		cswitch = true;
	}
	int d=1;
	int i=1;
	while(i<=*a&&i<=*b&&i<=*c)
	{
		if(*a%i==0&&*b%i==0&&*c%i==0)
			d=i;
		i++;
	}
	*a = *a/d;
	*b = *b/d;
	*c = *c/d;
	if (aswitch)
		*a = -*a;
	if (bswitch)
		*b = -*b;
	if (cswitch)
	{
		*a = -*a;
		*b = -*b;
	}
}

int gcf (int a,int b)
{
	int d = 1;
	int i = 1;
	int absa = abs(a);
	int absb = abs(b);
	while(i<=absa&&i<=absb)
	{
		if(a%i==0&&b%i==0)
			d = i;
		i++;
	}
	return d;
}

void simplifyFrac (int *n,int *d)
{
	bool nswitch = false, dswitch = false;
	if (*n<0)
	{
		*n = -(*n);
		nswitch = true;
	}
	if (*d<0)
	{
		*d = -(*d);
		dswitch = true;
	}
	int a = gcf(*n,*d);
	*n /= a;
	*d /= a;
	if (nswitch^dswitch)
		*n = -(*n);
}

int factorize (int n,int dest[])
{
	int count = 0;
	while (n%2==0)
	{
		dest[count++] = 2;
		n = n/2;
	}
	for (int i=3;i<=sqrt(n);i=i+2)
	{
		while (n%i==0)
		{
			dest[count++] = i;
			n = n/i;
		}
	}
	if (n>2)
		dest[count++] = n;
	return count;
}

int simplifySqrt (int radicand)
{
	int numpfactors;
	int n = (int)log(log(radicand))+10;
	int *pfactors = new int [n];
	numpfactors = factorize(radicand,pfactors);
	int out = 1;
	for (int i=0;i<numpfactors;i++)
		if (pfactors[i]==pfactors[i+1])
			out = out * pfactors[i++];
	delete[] pfactors;
	return out;
}







