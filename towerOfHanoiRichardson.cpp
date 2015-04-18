#include <iostream>
#include "dric.h"
#include "cpic.h"
#include <stack>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <fstream>

using namespace std;

class Disc
{
	private:
		CPIC *cpic;
		CPIC *cpicAlt;
		char pic[12];
		int size;
		bool selected;
	public:
		Disc (int);
		~Disc ();
		void toggle ();
		void loadCpic (string);
		void loadCpicAlt (string);
		int getSize () {return size;}
		void display (short,short);
		void displayPic ();
};

void newGame ();
void newAIGame ();
void aiMove (int,int,int);
void gameLoop ();
bool gameOver ();
bool moveDisc (int,int);
int selectRod (Disc*,int);
void initRods ();
void delRods ();
void displayGame ();
void displayCount ();
void displayRods ();
void display (stack<Disc*> &rod,short,short);
void print (stack<Disc*> &s,short,short);
void resetRods ();
HANDLE hOut;
CPIC *Stand,*Base,*Cursor,*Clear;
stack<Disc*> Rods[3];
int Count = 0;
const short DWIDTH = 13;
const short DHEIGHT = 1;
const short RSIZE = 5*DHEIGHT;
const short RGAP = 7;
const short RYOFF = 2;
const short RXOFF = 0;
const short CYOFF = 2;
const short CXOFF = 4;
const short CWIDTH = 5;
const short CHEIGHT = 6;

int main() {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	Stand = new CPIC(DWIDTH,DHEIGHT);
	Base = new CPIC(DWIDTH,DHEIGHT);
	Cursor = new CPIC(CWIDTH,CHEIGHT);
	Clear = new CPIC(CWIDTH,CHEIGHT);
	ifstream infile;
	infile.open("cpics/stand.cpic", ios::binary | ios::in);
	Stand->load(infile);
	infile.close();
	infile.open("cpics/base.cpic", ios::binary | ios::in);
	Base->load(infile);
	infile.close();
	infile.open("cpics/cursor.cpic", ios::binary | ios::in);
	Cursor->load(infile);
	infile.close();
	infile.open("cpics/clear.cpic", ios::binary | ios::in);
	Clear->load(infile);
	infile.close();
	
	menu mainMenu (3, "Tower of Hanoi");
	optionList mainOptions = {"Human", "Computer","Exit"};
	function mainFunctions [] = {newGame, newAIGame, goBack};
	mainMenu.setOptions(mainOptions);
	mainMenu.setFunctions(mainFunctions);
	mainMenu.open();
	
	delRods();
	delete Stand;
	delete Base;
	delete Cursor;
	delete Clear;
	return 0;
}

void newGame ()
{
	resetRods();
	gameLoop();
}

void resetRods ()
{
	delRods();
	initRods();
}

void newAIGame ()
{
	resetRods();
	Count = 0;
	aiMove(5,1,3);
	displayRods();
	cout<<"                                                       "<<endl;
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+3)});
	cout<<"The computer won in "<<Count<<" moves!"<<endl;
	system("pause");
}

void aiMove (int numDiscs, int from, int to)
{
	if (numDiscs==1)
	{
		Sleep(200);
		displayGame();
		moveDisc(from,to);
		Count++;
	}
	else
	{
		aiMove(numDiscs-1,from,6-from-to);
		Sleep(200);
		displayGame();
		moveDisc(from,to);
		Count++;
		aiMove(numDiscs-1,6-from-to,to);
	}
}

void gameLoop ()
{
	Count = 0;
	while (!gameOver())
	{
		displayGame();
		if (!moveDisc(0,0))
			return;
		Count++;
	}
	displayRods();
	cout<<"                                                       "<<endl;
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+3)});
	cout<<"You won in "<<Count<<" moves!"<<endl;
	system("pause");
}

void displayGame ()
{
	system("cls");
	displayRods();
	displayCount();
}

void displayCount ()
{
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+3)});
	cout<<"Use the arrow keys to move the cursor. Press q to quit."<<endl;
	cout<<"Moves: "<<Count<<endl;
}

bool gameOver ()
{
	if (Rods[2].size()==5)
		return true;
	return false;
}

bool moveDisc (int from, int to)
{
	int num;
	Disc *disc = NULL;
	(from) ? num = from : num = selectRod(disc,1);
	if (num==-1)
		return false;
	disc = Rods[num-1].top();
	disc->toggle();
	displayRods();
	Rods[num-1].pop();
	(to) ? num = to : num = selectRod(disc,num);
	if (num==-1)
		return false;
	Rods[num-1].push(disc);
	disc->toggle();
	return true;
}

void clearCursor ()
{
	for (int i=0;i<3;i++)
		Clear->display((DWIDTH+RGAP)*(i)+CXOFF,RYOFF+RSIZE+CYOFF);
}

int selectRod (Disc *disc, int pos)
{
	clearCursor();
	Cursor->display((DWIDTH+RGAP)*(pos-1)+CXOFF,RYOFF+RSIZE+CYOFF);
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+5)});
	int c = 0;
	do
	{
		if (kbhit())
			c = getch();
	} while (c!=75&&c!=77&&c!=13&&c!=113);
	cout<<"            "<<endl;
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+5)});
	if (c==75)
	{
		pos--;
		if (!pos)
			pos = 3;
		return selectRod(disc,pos);
	}
	else if (c==77)
	{
		pos++;
		if (pos==4)
			pos = 1;
		return selectRod(disc,pos);
	}
	else if (c==13)
	{
		if ((!disc&&Rods[pos-1].empty())||(disc&&!Rods[pos-1].empty()&&(disc->getSize()>Rods[pos-1].top()->getSize())))
		{
			cout<<"Invalid rod."<<endl;
			return selectRod(disc,pos);
		}
		return pos;
	}
	else if (c==113)
	{
		return -1;
	}
	else
		return selectRod(disc,pos);
}

void initRods ()
{
	for (int i=5;i>0;i--)
	{
		Rods[0].push(new Disc(i));
		Rods[0].top()->loadCpic("cpics/disc"+to_string(i)+".cpic");
		Rods[0].top()->loadCpicAlt("cpics/dischl"+to_string(i)+".cpic");
	}
	
}

void delRods ()
{
	for (int i=0;i<3;i++)
	{
		while (!Rods[i].empty())
			Rods[i].pop();
	}
}

void displayRods ()//display all rods
{
	for (short i=0;i<3;i++)
		display(Rods[i],(DWIDTH+RGAP)*i+RXOFF,RYOFF);
	SetConsoleCursorPosition(hOut,{0,(short)(RSIZE+RYOFF+CHEIGHT+CYOFF+3)});
}

void display (stack<Disc*> &rod, short x, short y)//display rod
{
	int l = rod.size();
	for (short i=0;i<5-l;i++)
	{
		SetConsoleCursorPosition(hOut,{x,y});
		Stand->display(x,y);
		y++;
	}
	print(rod,x,y);
	Base->display(x,y+l);
	SetConsoleCursorPosition(hOut,{x,(short)(y+l)});
}

void print (stack<Disc*> &rod, short x, short y)//display rod discs
{
	if (rod.empty())
		return;
	Disc *temp = rod.top();
	rod.pop();
	temp->display(x,y);
	print(rod,x,y+1);
	rod.push(temp);
}

Disc::Disc (int s)
{
	size = s;
	for (int i=0;i<11;i++)
		pic[i] = ' ';
	pic[11] = '\0';
	for (int i=5-s;i<6+s;i++)
		pic[i] = '*';
	selected = false;
}

Disc::~Disc ()
{
	delete cpic;
	delete cpicAlt;
}

void Disc::display (short x, short y)//display disc
{
	if (selected)
		cpicAlt->display(x,y);
	else
		cpic->display(x,y);
}

void Disc::displayPic ()
{
	cout<<pic<<endl;
}

void Disc::toggle ()
{
	if (selected)
		selected = false;
	else
		selected = true;
}

void Disc::loadCpic (string path)
{
	ifstream infile;
	infile.open(path, ios::binary | ios::in);
	cpic = new CPIC (DWIDTH,DHEIGHT);
	cpic->load(infile);
	infile.close();
}

void Disc::loadCpicAlt (string path)
{
	ifstream infile;
	infile.open(path, ios::binary | ios::in);
	cpicAlt = new CPIC (DWIDTH,DHEIGHT);
	cpicAlt->load(infile);
	infile.close();
}


