
#include <vector>
#include <iostream>
#include "utils.h"

using namespace std;
static Random random;
#define PI 3.1415926

class Fitness {
public:
    virtual double operator() (const vector<double>& pos) = 0;
};

//保存最终结果，解与值
class Solution {
public:
    vector<double> gb_position;
    double gb_value;
    Solution(vector<double>& position, double& value) :
        gb_position(position),
        gb_value(value)
    {}
    void print() {
        printf("gb position:");
        for (int i = 0; i < gb_position.size(); i++)
            printf("%7.4f,", gb_position[i]);
        printf("\r\ngb value: %f\r\n", gb_value);
    }
};

#define MAX_DIMENSION 10  //问题空间的最大维度
//粒子类
class Particle {
public:
    //配置参数
    static double velRange[MAX_DIMENSION][2];//速度的取值范围
    static double posRange[MAX_DIMENSION][2];//位置的取值范围
    static double phi1;
    static double phi2;
    static Fitness *pFitnessFunc;
    static bool   bFindMax;             //搜索最大值（取false就是搜索最小值）
    static vector<double> gb_position;  //全局最优的位置
    static double gb_value;             //全局最优的取值

private:
    vector<double> velocity;    //当前粒子的速度
    vector<double> position;    //当前粒子的位置
    vector<double> pb_position; //当前粒子最优的位置
    double pb_value;            //当前粒子最优的值

public:
    //随机初始化粒子的位置和速度
    Particle(int dimension) {
        for (int i = 0; i<dimension; ++i) {
            position.push_back(random.getFloat(posRange[i][0], posRange[i][1]));
            velocity.push_back(random.getFloat(velRange[i][0], velRange[i][1]));            
        }
        pb_value = bFindMax ? INT_MIN : INT_MAX;
        pb_position = position;
    }

    void updateVelocity() {
        for (int d = 0; d<velocity.size(); ++d) {
            double a1 = random.getFloat(0,1);
            double a2 = random.getFloat(0, 1);
            double tmp = 0.8 * velocity[d] +
                phi1*a1*(pb_position[d] - position[d]) +
                phi2*a2*(gb_position[d] - position[d]);

            velocity[d] = LimitValue(tmp, velRange[d]);
        }
    }

    void updatePosition() {
        for (int d = 0; d<position.size(); ++d) {
            position[d] = LimitValue(position[d] + velocity[d], posRange[d]);
        }
    }
    void updateFitness() {
        double fitnessValue = (*pFitnessFunc)(position);
        if(betterThan(fitnessValue , pb_value)){
            pb_value = fitnessValue;
            pb_position = position;
        }
        if (betterThan(fitnessValue, gb_value)) {
            gb_value = fitnessValue;
            gb_position = position;

            printf("update gb_value: %6.3f,", gb_value);
            printPos();
        }
    }

    void printPos() {
        printf("Position: ");
        for (int i = 0; i < position.size(); ++i) {
            printf("%5.3f ", position[i]);
        }
        printf("\r\n");
    }
    void printVel() {
        printf("velocity:");
        for (int i = 0; i < velocity.size(); ++i) {
            printf("%5.3f ", velocity[i]);
        }
        printf("\r\n");
    }
    void print() {
        printVel();
        printPos();
        printf("\r\npb_position: ");
        for (int i = 0; i < pb_position.size(); ++i) {
            printf("%5.3f ", pb_position[i]);
        }
        printf("\r\npb_value: %5.3\r\n", pb_value);
    }
private:
    double LimitValue(double val, double range[2]) {
        if (val > range[1])
            return range[1];
        else if (val < range[0])
            return range[0];
        return val;
    }
    //用于比较两个值的优劣
    bool betterThan(double a, double b) {
        if (bFindMax)
            return a>b;
        else
            return a < b;
    }
};

//粒子类
double Particle::velRange[MAX_DIMENSION][2];
double Particle::posRange[MAX_DIMENSION][2];
double Particle::phi1 = 1.49445;
double Particle::phi2 = 1.49445;
vector<double> Particle::gb_position;  //全局最优位置
double Particle::gb_value = INT_MIN;
Fitness *Particle::pFitnessFunc = NULL;
bool   Particle::bFindMax = true;

class Swarm {
private:
    int max_time;            //最大迭代次数
    vector<Particle> particles; //粒子群
public:

    //构造参数很多，可以写成外部配置文件
    Swarm(int population_size, int dimension, int max_time,
        Fitness* fitnessFunction, bool findMax,
        double posRange[][2],double velRange[][2]) :
        max_time(max_time)
    {
        Particle::bFindMax = findMax;
        Particle::pFitnessFunc = fitnessFunction;
        
        for (int i = 0; i < dimension; i++)
        {
            Particle::velRange[i][0]=velRange[i][0];
            Particle::velRange[i][1] = velRange[i][1];
            Particle::posRange[i][0] = posRange[i][0];
            Particle::posRange[i][1] = posRange[i][1];
        }
        Particle::gb_position=vector<double>(dimension, 0);
        Particle::gb_value = (findMax)? INT_MIN:INT_MAX;

        for (int i = 0; i<population_size; i++) {
            Particle par(dimension);
            //par.pb_value = (*fitnessFunction)(par.position);
            par.updateFitness();
            particles.push_back(par);
        }
    }

    void updateVelocity() {
        for (int i = 0; i < particles.size(); i++)
            particles[i].updateVelocity();
    }
    void updatePosition() {
        for (int i = 0; i < particles.size(); i++)
            particles[i].updatePosition();
    }
    void updateFitness() {
        for (int i = 0; i < particles.size(); i++)
            particles[i].updateFitness();
    }

    Solution run() {
        float curValue = INT_MAX;
        for (int t=0; t<max_time; t++) {
            updateFitness();
            //if (t % 20 == 0)
            //{
            //    if (abs(curValue - Particle::gb_value) < 0.0001f)
            //        break;
            //    curValue = Particle::gb_value;
            //}
            updateVelocity();
            updatePosition();
        }

        Solution solution(Particle::gb_position, Particle::gb_value);
        return solution;
    }
};

class Eggholder : public Fitness
{
public:
    Eggholder() {}

    double operator() (const vector<double>& pos) {
        double  x = pos[0];
        double y = pos[1];
        return -(y + 47)*sin(sqrt(abs(y + x / 2 + 47))) - x*sin(sqrt(abs(x - (y + 47))));
        //return abs(sin(PI*(x - 3)) / (PI*(x - 3)))*abs(sin(PI*(y - 3)) / (PI*(y - 3)));
    }
};
int main()
{
    Eggholder egg;
    //double posRange[2][2] = { {0,8},{ 0,8 } };
    //double velRange[2][2] = { { -1,1 },{ -1,1 } };
    double posRange[2][2] = { { -512,512 },{ -512,512 } };
    double velRange[2][2] = { { -50,50 },{ -50,50 } };

    Swarm pso(150, 2, 1000, &egg,false, posRange, velRange);
    pso.run();

}
