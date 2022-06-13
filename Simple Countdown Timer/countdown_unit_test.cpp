#include <iostream>
#include <cstdio>
#include <ctime>
#include <sstream>
using namespace std;

/******************************* Original Program *******************************/

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

/******************************* Test code *******************************/

#include <cassert>

Time to_time(int min, int sec) {
    Time result{min, sec};
    return result;
}

bool equal(const Time t1, const Time t2) {
    return t1.min == t2.min && t1.sec == t2.sec;
}

void test_to_int() {
    assert(to_int("12345") == 12345);
    assert(to_int("-12") == -12);
}

void test_to_int_3() {
    int a = 0, b = 0;

    to_int("123 456", &a, &b);
    assert(a == 123 && b == 456);
}

void test_bound_0_59() {
    assert(bound_0_59(47) == 47);
    assert(bound_0_59(-1) == 0);
    assert(bound_0_59(99) == 59);
}

void test_time_add() {
    Time res, t1, t2;

    t1 = to_time(51, 52);
    t2 = to_time(1, 2);
    time_add(&res, &t1, &t2);
    assert(equal(res, to_time(52, 54)));

    // sec overflow
    t1 = to_time(51, 52);
    t2 = to_time(1, 22);
    time_add(&res, &t1, &t2);
    assert(equal(res, to_time(53, 14)));

    // min overflow
    t1 = to_time(51, 52);
    t2 = to_time(50, 2);
    time_add(&res, &t1, &t2);
    assert(equal(res, to_time(41, 54)));

    // both overflow
    t1 = to_time(51, 52);
    t2 = to_time(50, 52);
    time_add(&res, &t1, &t2);
    assert(equal(res, to_time(42, 44)));
}

void test_time_sub() {
    Time res, t1, t2;

    t1 = to_time(10, 11);
    t2 = to_time(5, 7);
    time_sub(&res, &t1, &t2);
    assert(equal(res, to_time(5, 4)));

    // sec overflow
    t1 = to_time(10, 11);
    t2 = to_time(5, 17);
    time_sub(&res, &t1, &t2);
    assert(equal(res, to_time(4, 54)));

    // min overflow
    t1 = to_time(10, 11);
    t2 = to_time(25, 7);
    time_sub(&res, &t1, &t2);
    assert(equal(res, to_time(45, 4)));

    // both overflow
    t1 = to_time(10, 11);
    t2 = to_time(25, 17);
    time_sub(&res, &t1, &t2);
    assert(equal(res, to_time(44, 54)));
}

int main() {
    test_to_int();
    test_to_int_3();
    test_bound_0_59();
    test_time_add();
    test_time_sub();
    return 0;
}
