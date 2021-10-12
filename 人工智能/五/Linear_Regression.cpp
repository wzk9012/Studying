#include <stdio.h>
#include <time.h>
#include <iostream>
#include "utils.h"

#define batch 1
#define num 5000
#define learn_rate 0.0001
#define LR_a 2
#define LR_b 3
#define USE_NORMALIZATION 1


using namespace std;


#define MAX_ELEM_NUM 10


/*
表示函数a0+a1*x1+a1*x2+...++an*xn
*/
class LinearFunc
{
public:

    double Value(double coeff[], double x[], int length)
    {
        double v = coeff[0];
        for (int i = 1; i < length; i++)
            v += coeff[i] * x[i];
        return v;
    }

    //针对xi的偏微分
    double GradX(double coeff[], int index)
    {
        if (0 == index)return 0;
        return coeff[index];
    }
};
class LinearRegression {
    double *x;
    double *y;
    int m;
    Random random;
public:
    double *theta;
    LinearRegression(double x[], double y[], int m) {
        this->x = x;
        this->y = y;
        this->m = m;
    }
    void train(double alpha, int iterations)
    {
        double *J = new double[iterations];
        this->theta = gradient_descent(x, y, alpha, iterations, J, m);
        //cout << "J = ";
        //for (int i = 0; i < iterations; ++i) {
        //    cout << J[i] << " ";
        //}
        cout << endl << "Theta: " << theta[0] << " " << theta[1] << endl;
    }

    double predict(double x) { return h(x, theta); }
private:
    //计算模型损失
     double Cost(double x[], double y[], double theta[], int m)
    {
        double cost = 0;
        for (int i = 0; i < m; i++)
        {
            double val = h(x[i], theta);
            val -= y[i];
            cost += val *val;
        }
        return cost / m;
    }
    //计算单个预测值
    double h(double x, double theta[])
    {
        return theta[0] + theta[1] * x;
    }
    //预测
    void Predictions(double x[], double theta[], double dst[], int m)
    {
        for (int i = 0; i < m; i++) {
            dst[i] = h(x[i], theta);
        }
    }
    
#if USE_NORMALIZATION
    double *gradient_descent(double x[], double y[], double alpha, int iters, double *J, int m)
    {
        double *theta = new double[2];
        double *predictions = new double[m];
        theta[0] = random.getFloat(-1, 1) - 100;
        theta[1] = random.getFloat(0, 1);

        for (int i = 0; i < iters; i++) {

            //计算新的预测结果，并保存到predictions中
            Predictions(x, theta, predictions, m);

            //新的结果与目标值减法，结果保存到predictions
            ArraySub(predictions, y, predictions, m);

            double grad = 2.0f*ArraySum(predictions, m) ;
            theta[0] = theta[0] - alpha *grad;
            //theta[0] = theta[0] - ((grad>0)?(0.5):(-0.5));

            ArrayMul(predictions, x, predictions, m);

            grad = 2.0f*ArraySum(predictions, m);

            theta[1] = theta[1] - alpha *grad;

            double c = Cost(x, y, theta, m);
            if(i%100==1)
                printf("iter = %d, cost = %f.\r\n", i, c);
            if (c < 1e-6)break;
        }
        return theta;
    }
#else
    //梯度下降
    double *gradient_descent(double x[], double y[], double alpha, int iters, double *J, int m)
    {
        double *theta = new double[2];
        double *predictions = new double[m];
        theta[0] = random.getFloat(-1, 1) - 100;
        theta[1] = random.getFloat(0, 1);

        double lr_b = 0, lr_w = 0;

        for (int i = 0; i < iters; i++) {

            //计算新的预测结果，并保存到predictions中
            Predictions(x, theta, predictions, m);

            //新的结果与目标值减法，结果保存到predictions
            ArraySub(predictions, y, predictions, m);

            double grad = 2.0f*ArraySum(predictions, m) / m;
            lr_b += grad*grad;
            theta[0] = theta[0] - 40*alpha *grad / sqrt(lr_b);

            ArrayMul(predictions, x, predictions, m);

            grad = 2.0f*ArraySum(predictions, m) / m;
            lr_w += grad*grad;

            theta[1] = theta[1] - alpha *grad / sqrt(lr_w);

            double c = Cost(x, y, theta, m);
            if(i%50==1)printf("iter = %d, cost = %f.\r\n", i, c);
            if (c < 1e-6)break;
        }
        return theta;
    }
#endif

};



#if USE_NORMALIZATION
int main()
{
    double fTrueX[MAX_ELEM_NUM] = { 165,165,157,170,175,165,155,170 };
    double fTrueY[MAX_ELEM_NUM] = { 48,57,50,54,64,61,43,59 };

    double meanX, varX, meanY, varY;
    ArrayNormalization(fTrueX, 8, meanX, varX);
    ArrayNormalization(fTrueY, 8, meanY, varY);

    LinearRegression regression(fTrueX, fTrueY, 8);
    regression.train(0.01f, 100000);

    printf("minX = %f, maxX = %f.\r\n", meanX, varX);
    printf("minY = %f, maxY = %f.\r\n", meanY, varY);


    regression.theta[0] = regression.theta[0] * varY + varY- regression.theta[1] * meanX*varY/ varX;
    regression.theta[1] = regression.theta[1] * varY / varX;
    printf("b = %f, w = %f.\r\n", regression.theta[0], regression.theta[1]);

    //Regression re(LR_a, LR_b);
    //double *lcq=re.train();
    //re.predict(lcq[0], lcq[1]);
}

#else
int main()
{
    double fTrueX[MAX_ELEM_NUM] = { 165,165,157,170,175,165,155,170 };
    double fTrueY[MAX_ELEM_NUM] = { 48,57,50,54,64,61,43,59 };


    LinearRegression regression(fTrueX, fTrueY, 8);
    regression.train(10.0f, 100000);

   
    //Regression re(LR_a, LR_b);
    //double *lcq=re.train();
    //re.predict(lcq[0], lcq[1]);
}

#endif