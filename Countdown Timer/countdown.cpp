#include <iostream>
#include <cstdio>
#include <sstream>
#include <chrono>
#include <windows.h>
using namespace std;

// Countdown timer on Windows. Written by davidhcefx, 2019.11.30.

int toint(char* str) {
	stringstream ss(str);
	int x;
	ss >> x;
	return x;
}

void decrease(int& min, int& sec) {
	if (sec == 0) {
		min -= 1;
		sec = 59;
	} else {
		sec -= 1;
	}
}

int main(int argc, char* argv[])
{
	int min = 30;
	int sec = 0;

	if (argc >= 2) {
		min = toint(argv[1]);
		if (argc >= 3) {
			sec = toint(argv[2]);
		}
	}

	while (min >= 0) {
		auto start = chrono::high_resolution_clock::now();
		system("cls");
		printf("+------------------------------------------------------------+\n");
		printf("|+----                                                  ----+|\n");
		printf("||>>>>>             TIME REMAINS:      %02d:%02d           <<<<<||\n", min, sec);
		printf("|+----                                                  ----+|\n");
		printf("+------------------------------------------------------------+\n");
		decrease(min, sec);
		// time spent in milliseconds
		chrono::duration<double> diff = chrono::high_resolution_clock::now() - start;
		Sleep((int)(1000 - diff.count()));
	}
    printf("\n\n");
    printf("_______                   ______  /  ______                  _____     \n");
	printf("   |     |    |\\    /|   |          |             |     |   |     |    |\n");
	printf("   |     |    | \\__/ |   |______    |______       |     |   |_____|    |\n");
	printf("   |     |    |      |   |                |       |     |   |          |\n");
	printf("   |     |    |      |   |______    ______|       |_____|   |          .\n");
	printf("\n\n\n\n\n\n");
	system("pause");

	return 0;
}



/*  Ascii Work by davidhcefx.
+------------------------------------------------------------+
|+----                                                  ----+|
||>>>>>             TIME REMAINS:      29:59           <<<<<||
|+----                                                  ----+|
+------------------------------------------------------------+

_______                   ______  /  ______                  _____
   |     |    |\    /|   |          |             |     |   |     |    |
   |     |    | \__/ |   |______    |______       |     |   |_____|    |
   |     |    |      |   |                |       |     |   |          |
   |     |    |      |   |______    ______|       |_____|   |          .
*/