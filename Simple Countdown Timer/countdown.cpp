#include <iostream>
#include <cstdio>
#include <ctime>
#include <sstream>
using namespace std;

// Countdown timer on Windows. Written by davidhcefx, 2019.11.30.

typedef struct Time {
    int min;
    int sec;
} Time;

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

void get_cur_time(Time* cur) {
    time_t t = time(NULL);
    char buf[6];
    strftime(buf, sizeof(buf), "%M %S", localtime(&t));
    to_int(buf, &cur->min, &cur->sec);
}

// compute t1 + t2; both min and sec will be bounded to [0, 59]
void time_add(Time* result, const Time* t1, const Time* t2) {
    result->min = t1->min + t2->min;
    result->sec = t1->sec + t2->sec;
    // deals with overflow
    if (result->sec >= 60) {
        result->sec -= 60;
        result->min++;
    }
    if (result->min >= 60) {
        result->min -= 60;
    }
}

// compute t1 - t2; both min and sec will be bounded to [0, 59]
void time_sub(Time* result, const Time* t1, const Time* t2) {
    result->min = t1->min - t2->min;
    result->sec = t1->sec - t2->sec;
    // deals with underflow
    if (result->sec < 0) {
        result->sec += 60;
        result->min--;
    }
    if (result->min < 0) {
        result->min += 60;
    }
}

void show_remain(const Time* remain) {
    system("cls");
    printf("+------------------------------------------------------------+\n");
    printf("|+----                                                  ----+|\n");
    printf("||>>>>>             TIME REMAINS:      %02d:%02d           <<<<<||\n", \
           remain->min, remain->sec);
    printf("|+----                                                  ----+|\n");
    printf("+------------------------------------------------------------+\n");
}

void show_final_msg_and_pause() {
    printf("\n\n");
    printf("_______                   ______  /  ______                  _____     \n");
    printf("   |     |    |\\    /|   |          |             |     |   |     |    |\n");
    printf("   |     |    | \\__/ |   |______    |______       |     |   |_____|    |\n");
    printf("   |     |    |      |   |                |       |     |   |          |\n");
    printf("   |     |    |      |   |______    ______|       |_____|   |          .\n");
    printf("\n\n\n\n\n\n");
    system("pause");
}

int main(int argc, char* argv[]) {
    Time remain{30, 0}, target, cur;  // default: 30 min 0 sec
    bool is_last_min;

    if (argc >= 2) {
        remain.min = bound_0_59(to_int(argv[1]));
        if (argc >= 3) {
            remain.sec = bound_0_59(to_int(argv[2]));
        }
    }
    is_last_min = (remain.min == 0);
    get_cur_time(&cur);
    time_add(&target, &cur, &remain);  // target might appear "ealier" than cur

    while (true) {
        show_remain(&remain);
        system("timeout /t 2 >nul");
        remain.sec -= 2;

        if (remain.sec < 0) {
            remain.sec += 60;  // deals with underflow
            remain.min--;

            // recompute remain every minute
            get_cur_time(&cur);
            time_sub(&remain, &target, &cur);
        }

        // termination check
        if (cur.min == target.min && !is_last_min) {
            is_last_min = true;
        }
        if (is_last_min && (cur.min != target.min || cur.sec > target.sec)) {
            break;
        }
    }

    show_final_msg_and_pause();
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
