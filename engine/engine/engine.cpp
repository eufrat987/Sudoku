// sudokuengine.cpp: Definiuje punkt wejœcia dla aplikacji konsolowej.
//

#include "stdafx.h"


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>

#define Generator 13
#define Breaker 21
#define Diff_factor 35

using namespace std;

int tab[9][9], gtab[9][9], mem[9][9];
bool pom[9][9];
int mode, minc, ileminc, hardx, hardy, diff_eng = 0;
bool diff;
fstream file, logfile;
vector<int>v;

enum res { off = -1, ok = 0, blad_plik = -10, not_good = -3, brak_roz = -5, wiele_roz = 1, blad_gen = -15 };
res state = off;

struct arrstack
{
	int tab[9][9];
	arrstack * prev = NULL;
}*stos = NULL;

void push(int tab[9][9])
{
	if (!stos)
	{
		stos = new arrstack;
		for (int i = 0; i<9; i++)
			for (int j = 0; j<9; j++)
				stos->tab[i][j] = tab[i][j];
	}
	else
	{
		arrstack* tmp = new arrstack;
		for (int i = 0; i<9; i++)
			for (int j = 0; j<9; j++)
				tmp->tab[i][j] = tab[i][j];
		tmp->prev = stos;
		stos = tmp;
	}
}

bool pop(int tab[9][9])
{
	if (!stos)return false;
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			tab[i][j] = stos->tab[i][j];
	arrstack* del = stos;
	stos = stos->prev;
	delete del;
	return true;
}

bool loadfile()
{
	logfile << "open file(read): engine" << endl;
	file.open("engine", ios::in);
	if (!file.good())
	{
		state = blad_plik;
		logfile << "blad pliku" << endl;
		return false;
	}
	file >> mode;
	logfile << "mode:";
	if (mode == Generator)
	{
		file >> diff;
		logfile << "generator" << endl;
	}
	else
	{
		logfile << "breaker" << endl;
		for (int i = 0; i<9; i++)
			for (int j = 0; j<9; j++)
				file >> tab[i][j];
	}
	file.close();
	logfile << "close file: engine" << endl;
	return true;
}

int licz(int c)
{
	int sum = 0;
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j] == c)sum++;
	return sum;
}

void dajpom()
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j]>0)pom[i][j] = 1;
			else pom[i][j] = 0;
}

void cyfpomhelp(int x, int y)
{
	for (int i = 0; i<9; i++)
		pom[i][y] = 1;
	for (int i = 0; i<9; i++)
		pom[x][i] = 1;
	int tmpx = (x / 3) * 3, tmpy = (y / 3) * 3;
	for (int i = tmpx; i<tmpx + 3; i++)
		for (int j = tmpy; j<tmpy + 3; j++)
			pom[i][j] = 1;
}

void cyfpom(int c)
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j] == c)cyfpomhelp(i, j);
}

bool good(int tab[9][9])
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j] >= 0 && tab[i][j]<10)
			{
				if (tab[i][j]>0)
				{
					int c = tab[i][j];
					for (int k = 0; k<9; k++)
						if ((tab[k][j] == c&&k != i) || (tab[i][k] == c&&k != j))return false;
					int mi = (i / 3) * 3, mj = (j / 3) * 3;
					for (int x = mi; x<mi + 3; x++)
						for (int y = mj; y<mj + 3; y++)
							if (tab[x][y] == c&&x != i&&y != j)
								return false;
				}
			}
			else return false;
			return true;
}

bool fullsud(int tab[9][9])
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j] == 0)return false;
	return true;
}

bool emptysud()
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			if (tab[i][j]>0)return false;
	return true;
}

bool wstaw(int c)
{
	dajpom();
	cyfpom(c);
	int sum = 0, memx, memy;
	for (int x = 0; x<9; x += 3)
		for (int y = 0; y<9; y += 3)
		{
			sum = 0;
			for (int i = x; i<x + 3; i++)
				for (int j = y; j<y + 3; j++)
					if (pom[i][j] == 0)
					{
						memx = i;
						memy = j;
						sum++;
					}
			if (sum == 1)
			{
				tab[memx][memy] = c;
				return true;
			}
			if (sum<ileminc&&sum>0)
			{
				ileminc = sum;
				minc = c;
				hardx = x;
				hardy = y;
			}
		}
	return false;
}

bool krok()
{
	minc = ileminc = 10;
	for (int i = 1; i<10; i++)if (wstaw(i))return true;
	return false;
}

bool err()
{
	for (int c = 1; c<10; c++)
	{
		if (licz(c) == 9)continue;
		dajpom();
		cyfpom(c);
		for (int i = 0; i<9; i++)
			for (int j = 0; j<9; j++)
				if (pom[i][j] == 0)goto jump;
		return true;
	jump:
		;
	}
	return false;
}

void hard()
{
	if (minc == 10)return;
	diff_eng++;
	dajpom();
	cyfpom(minc);
	for (int i = hardx; i<hardx + 3; i++)
		for (int j = hardy; j<hardy + 3; j++)
			if (pom[i][j] == 0)
			{
				tab[i][j] = minc;
				push(tab);
				tab[i][j] = 0;
			}
	pop(tab);
}

bool empty_pop()
{
	int tmp[9][9];
	if (pop(tmp))while (pop(tmp));
	else return false;
	return true;
}

void update_gtab()
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			gtab[i][j] = tab[i][j];
}

bool engine()
{
	diff_eng = 0;
	bool kilkarozw = false, rozw = false;
	if (!good(tab))
	{
		state = not_good;
		return false;
	}
	while (1)
	{
		while (krok());
		if (fullsud(tab))
		{
			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 9; j++)
					mem[i][j] = tab[i][j];
			state = ok;
			if (kilkarozw)
			{
				state = wiele_roz;
				break;
			}
			rozw = kilkarozw = true;
			if (!pop(tab))break;
		}
		if (err())
		{
			if (!pop(tab))
			{
				if (!rozw)state = brak_roz;
				break;
			}
		}
		else hard();
		if (diff_eng > 1000 && mode == Generator)
		{
			state = off;
			break;
		}
	}
	if(state==ok||state==wiele_roz)
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				tab[i][j] = mem[i][j];
	return true;
}

void engineoff()
{
	logfile << "open file(write): engine" << endl;
	file.open("engine", ios::out);
	file << state << " " << diff_eng << " " << endl;
	if (state == ok || state == wiele_roz)
	{
		if (mode == Generator)
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
					file << gtab[i][j] << " ";
				file << endl;
			}
		file << endl;
		for (int i = 0; i<9; i++)
		{
			for (int j = 0; j<9; j++)
				file << tab[i][j] << " ";
			file << endl;
		}
	}
	file.close();
	logfile << "close file: engine" << endl;
}

void update_tab()
{
	for (int i = 0; i<9; i++)
		for (int j = 0; j<9; j++)
			tab[i][j] = gtab[i][j];
}

bool gen_push()
{
	if (fullsud(gtab))
	{
		state = blad_gen;
		return false;
	}
	int x, y, push_imp = 0;
	do
	{
		push_imp++;
		x = rand() % 9;
		y = rand() % 9;
	} while (gtab[x][y] != 0 && push_imp <= 243);
	for (int i = 0; i < 9 && gtab[x][y] != 0; i++)
		for (int j = 0; j < 9; j++)
			if (tab[i][j] == 0) { x = i; y = j; break; }

	for (int i = 1; i < 10; i++)
	{
		gtab[x][y] = i;
		if (good(gtab))return true;
	}
	return false;
}

int gtab_x, gtab_y, gtab_val;
bool gen_pop()
{
	if (emptysud())return false;

	do
	{
		gtab_x = rand() % 9;
		gtab_y = rand() % 9;
	} while (gtab[gtab_x][gtab_y] == 0);
	gtab_val = gtab[gtab_x][gtab_y];
	gtab[gtab_x][gtab_y] = 0;
	return true;
}

int run_engine()
{
	logfile << "engine:\n[on]";
	engine();
	logfile << "[off:" << "dif:" << diff_eng << " st:" << state << "]";
	if (empty_pop())logfile << " [clear stack]" << endl;
	else logfile << "\n";
	return diff_eng;
}

bool pop_sequence()
{
	if (!gen_pop())
	{
		state = blad_gen;
		return false;
	}
	update_tab();
	run_engine();
	return true;
}

void generator()
{
	while (state != ok)
	{
		for (int i = 0; i < 9; i++)for (int j = 0; j < 9; j++)gtab[i][j] = 0;
		for (int i = 0; i < Diff_factor&&gen_push(); i++);
		update_tab();
		run_engine();
		run_engine();
		update_gtab();
		while (state == ok)if (!pop_sequence())return;
		gtab[gtab_x][gtab_y] = gtab_val;
		update_tab();
		run_engine();
	}
}

//------------------------------------------------------------
void rand_v()
{
	bool here;
	v.clear();
	do
	{
		here = false;
		int val = (rand() % 9) + 1;
		for (int i = 0; i < v.size(); i++)
			if (v[i] == val)here = true;
		if (!here)v.push_back(val);
	} while (v.size() != 9);
}

bool gv2krok()
{
	if (fullsud(gtab))return false;
	int x, y;
	do
	{
		x = rand() % 9;
		y = rand() % 9;
	} while (gtab[x][y] != 0);
	rand_v();
	for (int i = 0; i < 9; i++)
	{
		gtab[x][y] = v[i];
		if (!good(gtab))continue;
		update_tab();
		run_engine();
		if (state == 0)
		{
			if (diff_eng > 0)return false;
			return true;
		}
		if (state != 1)continue;
		if (diff_eng > 0)return true;
	}
	return false;
}

void generator_v2()
{
	while (diff_eng == 0 || state != 0)
	{
		cout << " .";
		for (int i = 0; i < 9; i++)for (int j = 0; j < 9; j++)gtab[i][j] = 0;
		while (gv2krok());
	}
}
//------------------------------------------------------------
int main()
{
	logfile.open("log.txt", ios::out);
	logfile << "start" << endl;
	srand(time(NULL));
	if (loadfile());
	{
		switch (mode)
		{
		case Generator:
			logfile << "-[[generator on]]-\n";
			if (diff)generator_v2();
			else generator();
			logfile << "-[[generator off]]-\n";
			break;
		case Breaker:
			run_engine();
			break;
		}
	}
	engineoff();
	logfile << "exit value:" << state << endl;
	logfile.close();
	return state;
}

