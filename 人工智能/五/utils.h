
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

/*数组减法*/
inline void ArraySub(double x1[], double x2[], double dst[], int length)
{
    for (int i = 0; i < length; i++)
        dst[i] = x1[i] - x2[i];
}

/*数组乘法*/
inline void ArrayMul(double x1[], double x2[], double dst[], int length)
{
    for (int i = 0; i < length; i++)
        dst[i] = x1[i] * x2[i];
}

inline double ArraySum(double x[],int length)
{
    double s = 0;
    for (int i = 0; i < length; i++)
        s += x[i];
    return s;
}

inline void ArrayStat(double x[], int length,double &mean, double &var)
{
    mean= 0;
    var = 0;

    for (int i = 0; i < length; i++)
    {
        mean += x[i];
        var += x[i]* x[i];
    }
    mean/= length;
    var = sqrt((var*length - mean*mean) / (length*(length - 1)));
}
inline void ArrayNormalization(double x[], int length,double &mean, double &var)
{
    ArrayStat(x, length, mean, var);

    for (int i = 0; i < length; i++)
    {
        x[i] = (x[i] - mean) / var;
    }
}
