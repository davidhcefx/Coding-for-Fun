#include <iostream>
#include <random>
using namespace std;

// My self-written Cpp hash function implementation using mt19937.
// Written by davidhcefx, 2019.9.11

// Hash digest has length of 256-bit
string myhash(int key) {
    int UINTSIZE = sizeof(unsigned int);
    int N = 256 / (UINTSIZE * 8);
    seed_seq seed({key, 4186834, 2308909, 803771, 3231288, 6660692});
    mt19937 mt(seed);
    mt.discard(43);
    string str;
    // Generate each UINTSIZE bytes individually
    for (int i = 0; i < N; i++) {
        unsigned int rnd = mt();
        for (int j = 0; j < UINTSIZE * 2; j++, rnd >>= 4) {
            str.push_back([](char hex){
                if (hex >= 10) {
                    return hex - 10 + 'a';
                } else {
                    return hex + '0';
                }
            }(rnd % 16));
        }
        mt.discard(7); // skip 7 numbers
    }
    return str;
}

int main()
{
    cout << myhash(123456) << "\n";

    return 0;
}
