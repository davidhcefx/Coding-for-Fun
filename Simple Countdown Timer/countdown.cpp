#include <iostream>
#include <cstdio>
#include <ctime>
#include <sstream>
using namespace std;

// Countdown timer on Windows. Written by davidhcefx, 2019.11.30.

struct Time {
    int min;
    int sec;
};

inline int to_int(const char* str) {
    stringstream ss(str);
    int x;
    ss >> x;
    return x;
}

inline void to_int(const char* str, int* a, int* b) {
    stringstream ss(str);
    ss >> (*a) >> (*b);
}

inline int bound_0_59(const int n) {
    return max(min(n, 59), 0);
}

void get_time(struct Time* cur) {
    time_t t = time(NULL);
    char buf[6];
    strftime(buf, sizeof(buf), "%M %S", localtime(&t));
    to_int(buf, &cur->min, &cur->sec);
}

int main(int argc, char* argv[]) {
    struct Time target, remain{30, 0}, cur;  // defaults to 30 min 0 sec
    bool is_last_min;

    if (argc >= 2) {
        remain.min = bound_0_59(to_int(argv[1]));
        if (argc >= 3) {
            remain.sec = bound_0_59(to_int(argv[2]));
        }
    }
    is_last_min = (remain.min == 0);

    get_time(&cur);
    target.min = cur.min + remain.min;
    target.sec = cur.sec + remain.sec;
    // deals with overflow
    if (target.sec >= 60) {
        target.sec -= 60;
        target.min++;
    }
    if (target.min >= 60) {
        target.min -= 60;  // make it appear "ealier" than current time
    }

    while (true) {
        system("cls");
        printf("+------------------------------------------------------------+\n");
        printf("|+----                                                  ----+|\n");
        printf("||>>>>>             TIME REMAINS:      %02d:%02d           <<<<<||\n", remain.min, remain.sec);
        printf("|+----                                                  ----+|\n");
        printf("+------------------------------------------------------------+\n");
        system("timeout /t 2 >nul");

        remain.sec -= 2;
        if (remain.sec < 0) {
            remain.sec += 60;
            remain.min--;

            // recompute remain each minute
            get_time(&cur);
            remain.min = target.min - cur.min;
            remain.sec = target.sec - cur.sec;
            // deals with underflow
            if (remain.sec < 0) {
                remain.sec += 60;
                remain.min--;
            }
            if (remain.min < 0) {
                remain.min += 60;
            }
        }

        // termination check
        if (cur.min == target.min && !is_last_min) {
            is_last_min = true;
        }
        if (is_last_min && (cur.min != target.min || cur.sec > target.sec)) {
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
