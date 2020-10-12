#include <iostream>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <windows.h>
using namespace std;
#define APPROX_1SEC 700

// Countdown timer on Windows. Written by davidhcefx, 2019.11.30.

struct Time {
    int min;
    int sec;
};

inline int toint(char* str) {
    stringstream ss(str);
    int x;
    ss >> x;
    return x;
}

inline void to2int(char* str, int& a, int& b) {
    stringstream ss(str);
    ss >> a >> b;
}

inline int bound59(int n) {
    return max(min(n, 59), 0);
}

void now(int& min, int& sec) {
    time_t t = time(NULL);
    char buf[6];
    strftime(buf, sizeof(buf), "%M %S", localtime(&t));
    to2int(buf, min, sec);
}

int main(int argc, char* argv[])
{
    Time target, remain;
    int min, sec;
    bool lastmin = false;

    remain.min = 30;  // default values
    remain.sec = 0;
    if (argc >= 2) {
        remain.min = bound59(toint(argv[1]));
        if (argc >= 3) {
            remain.sec = bound59(toint(argv[2]));
        }
    }
    now(min, sec);
    target.min = min + remain.min;
    target.sec = sec + remain.sec;
    // deals with overflow
    if (target.sec >= 60) {
        target.sec -= 60;
        target.min++;
    }
    if (target.min >= 60) {
        target.min -= 60; // make it appear "ealier" than current time
    }

    while (true) {
        system("cls");
        printf("+------------------------------------------------------------+\n");
        printf("|+----                                                  ----+|\n");
        printf("||>>>>>             TIME REMAINS:      %02d:%02d           <<<<<||\n", remain.min, remain.sec);
        printf("|+----                                                  ----+|\n");
        printf("+------------------------------------------------------------+\n");
        Sleep( APPROX_1SEC );
        // re-compute remaining time
        now(min, sec);
        remain.min = target.min - min;
        remain.sec = target.sec - sec;
        // deals with underflow
        if (remain.sec < 0) {
            remain.sec += 60;
            remain.min--;
        }
        if (remain.min < 0) {
            remain.min += 60;
        }
        // termination check
        if (min == target.min && !lastmin) {
            lastmin = true;
        }
        if (lastmin && (min != target.min || sec > target.sec)) {
            break;
        }
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
