
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

class Random
{
public:
    Random() {
        srand(static_cast<unsigned int>(time(nullptr)));
    };
    ~Random() {};

    /*产生[start,end]的随机整数*/
    int getInt(int start, int end) {
        return rand() % (end - start + 1) + start;
    }
    /*产生[start,end]的随机浮点数*/
    float getFloat(float start, float end) {
        return (end - start)*(float)rand() / RAND_MAX + start;
    }
};

inline long Milliseconds()
{
    return GetTickCount();
}