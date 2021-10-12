

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iostream>

#define MAX_JOB_NUM 5
#define MAX_MACHINE_NUM 4
#define MAX_CHROMOSOME_NUM 20 //
#define INVALID_FITNESS -1

using namespace std;

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
Random random;


/*
下标选择器：无重复的随机选出下标。
其中的方法可以重复调用，例如如果需要选择出5个小标，则可以连续调用五次int Choose().
*/
class IndexChooser
{
public:
    IndexChooser(int length) {
        Reset(length);
    }
    void Reset(int length) {
        index.resize(length);
        for (int i = 0; i < length; i++)
            index[i] = i;
    }
    int Choose() {
        int temp = random.getInt(0, index.size() - 1);
        int a = index[temp];
        index.erase(index.begin() + temp);
        return a;
    }
    void Choose(int &a, int &b) {
        a = Choose();
        b = Choose();
    }
private:
    vector<int> index;
};


class Process
{
public:
    int machineId;
    int time;
    void Set(int id, int t) {
        machineId = id;
        time = t;
    }
    void Show(){
        printf("(%d,%d)", machineId, time);
    }
};

class Job
{
public:
    Process procInfo[MAX_MACHINE_NUM];
    int procNum;  //工序的数量

    Job() {
        procNum = 0;
    }
    void Append(int machineId, int time) {
        procInfo[procNum].Set(machineId, time);
        procNum++;
    }
    void Reset() {
        procNum = 0;
    }
    void Show() {
        printf("Process Num: %d.\r\n", procNum);
        for (int i = 0; i < procNum; i++){
            procInfo[i].Show();
        }
        printf("\r\n");
    }
};
class JobManager
{
public:
    JobManager() {};
    ~JobManager() {};

    bool Init() {
        printf("input job and machine:\r\n");
        scanf("%d%d", &jobNum, &machineNum);

        for (int i = 0; i < jobNum; i++) {
            int p;
            printf("input job%d  process:\r\n", i);
            scanf("%d", &p);
            printf("input machine and time:\r\n");
            for (int j = 0; j < p; j++) {
                int m, t;
                scanf("%d%d", &m, &t);
                jobsInfo[i].Append(m, t);
            }
        }
        return true;
    }
    void Show() {
        printf("####################################\r\n");
        printf("Job Num: %d, Machine Num: %d\r\n", jobNum, machineNum);
        for (int i = 0; i < jobNum; i++) {
            jobsInfo[i].Show();
        }
        printf("####################################\r\n");
    }
    int GetMachineNum() const { return machineNum; }
    int GetJobNum() const { return jobNum; }

    Job jobsInfo[MAX_JOB_NUM];   //工件信息
private:
    int machineNum;          //机器数量
    int jobNum;              //工件数量
};

class Chromosome
{
public:
    Chromosome() {};
    ~Chromosome() {}
    Chromosome(const Chromosome &other) {
        length = other.length;
        memcpy(Genes, other.Genes, length * sizeof(int));
        fitness = other.fitness;
        cFitness = other.cFitness;
    }
    Chromosome &operator=(const Chromosome &other) {
        length = other.length;
        memcpy(Genes, other.Genes, length * sizeof(int));
        fitness = other.fitness;
        cFitness = other.cFitness;
        return *this;
    }

    void RandInit(int len) { //随机初始化
        IndexChooser index(len);
        length = len;
        for (int i = 0; i < len; i++)
            Genes[i] = index.Choose();

        fitness = INVALID_FITNESS;
        cFitness = INVALID_FITNESS;
    }

    int GetLength()const { return length; }
    const int *GetGene() { return Genes; }
    float GetFitness()const { return fitness; }
    void SetFitness(float fit) { fitness = fit; }
    float GetCFitness()const { return cFitness; }
    void SetCFitness(float cfit) { cFitness = cfit; }
    void Show() {
        for (int i = 0; i < length; i++)
        {
            printf("%d", Genes[i]);
        }
        printf(": %f\r\n", fitness);
    }

    void Mutation(int n = 1) {
        if (n > length) n = length;
        IndexChooser index(length);
        int first, second, temp;
        for (int k = 0; k < n; k++)
        {
            index.Choose(first, second);
            temp = Genes[first];
            Genes[first] = Genes[second];
            Genes[second] = temp;
        }
    }

    //交叉操作：产生两个新的染色体，分别是this以及mother
    void CrossOver(Chromosome &mother) {
        IndexChooser index(length);

        int start, end;
        index.Choose(start, end);
        if (start > end) {
            int t = start;
            start = end;
            end = t;
        }
        int buf[MAX_JOB_NUM];
        memcpy(buf, Genes, length * sizeof(int));
        GetGenFromMother(Genes, mother.Genes, length, start, end);
        GetGenFromMother(mother.Genes, buf, length, start, end);
    }

private:
    //检查[a,b]之间是否包含基因gene
    bool Contain(int dat[], int len, int gene, int a, int b) {
        for (int i = a; i <= b; i++)
        {
            if (dat[i] == gene)return true;
        }
        return false;
    }

    //从mother中将区间[a,b]之外的基因复制过来
    void GetGenFromMother(int child[], int mother[],int len, int start, int end)
    {
        int index = 0;
        for (int i = 0; i < start; i++)
        {
            while (index<len && Contain(child, len, mother[index], start, end))index++;
            child[i] = mother[index];
            index++;
        }
        for (int i = end+1; i < len; i++)
        {
            while (index<len && Contain(child, len, mother[index], start, end))index++;
            child[i] = mother[index];
            index++;
        }
    }
private:
    int Genes[MAX_JOB_NUM];
    int length;
    float fitness; //适应度
    float cFitness;//累计适应度。选择个体的时候需要用到
};

class Population
{
public:
    Population(int iterNum = 50,float mutation = 0.1f, float cross = 0.6f) {
        nIterNum = iterNum; fMutation = mutation; fCross = cross;
    };
    ~Population() {};
    void Init() {
        jobs.Init();
        int genelen = jobs.GetJobNum();

        //初始化种群。
        for (int i = 0; i < chromosomes.size(); i++)
            chromosomes[i].RandInit(genelen);
    }
    void UpdateFitness(){
        for (int i = 0; i < chromosomes.size(); i++)
            chromosomes[i].SetFitness(CalFitness(jobs, chromosomes[i]));
    }  
    void Show() {
        jobs.Show();
        for (int i = 0; i < chromosomes.size(); i++)
            chromosomes[i].Show();
    }
    void MakeNewGeneration();
private:
    float CalFitness(const JobManager &jobManager, Chromosome &chromosome);
    void Sort() {
        sort(chromosomes.begin(), chromosomes.end(),
            [](const Chromosome &a, const Chromosome &b) -> bool { return a.GetFitness()< b.GetFitness(); });
    }
    void SelectNewGeneration();
    void Crossover();
    void Mutation();
    void UpdateElitist() {
        nElitistNum = chromosomes.size() * 3 / 100;
        if (nElitistNum < 1)nElitistNum = 1;
    }

    /*
    个体的选择使用锦标赛法(Tournament Selection)，其策略为从整个种群中抽取n个体，
    让他们进行竞争(锦标赛)，抽取其中的最优的个体。参加锦标赛的个体为整个种群，y默认值为3。

    使用锦标赛法的优势为
    时间复杂度更小，即O(n)
    易并行化处理
    不易陷入局部最优点
    不需要对所有的适应度值进行排序处理过程
    */
    Chromosome& TournamentSelection(int n = 3) {
        IndexChooser index(chromosomes.size());
        Chromosome *pBest = NULL;
        for (int i = 0; i < n; i++)
        {
            Chromosome *pCur = &chromosomes[index.Choose()];
            if (!pBest)
                pBest = pCur;
            else if (pBest->GetFitness()> pCur->GetFitness())
                pBest = pCur;
        }
        return *pBest;

    }
private:
    vector<Chromosome> chromosomes=vector<Chromosome>(MAX_CHROMOSOME_NUM);
    JobManager jobs;

    float fMutation;//变异率
    float fCross;//交叉概率
    int   nIterNum;  //迭代的次数
    int   nElitistNum;//精英的数量
};

#define MAX(A,B) ((A)>(B)?(A):(B))
float Population::CalFitness(const JobManager &jobManager, Chromosome &chromosome)
{
    int totalTime = 0, machineTime[MAX_MACHINE_NUM] = {};
    int c[MAX_JOB_NUM][MAX_MACHINE_NUM] = {}; //参考教材，c[i][j]表示工件i在机器j上加工需要的时间
    const int *pGene = chromosome.GetGene();
    
    //根据教材中的公式6.5.1计算
    int j1 = pGene[0];
    c[j1][0] = jobManager.jobsInfo[j1].procInfo[0].time;

    //根据教材中的公式6.5.2计算
    for (int k = 1; k < jobManager.jobsInfo[j1].procNum; k++)
        c[j1][k] = c[j1][k-1]+ jobManager.jobsInfo[j1].procInfo[k].time;

    //根据教材中的公式6.5.3计算
    for (int i = 1; i < jobManager.GetJobNum(); i++)
    {
        int ji = pGene[i];
        int ji_1 = pGene[i-1];
        c[ji][0] = c[ji_1][0] + jobManager.jobsInfo[ji].procInfo[0].time;
    }

    //根据教材中的公式6.5.4计算
    for (int i = 1; i < jobManager.GetJobNum(); i++)
    {
        int ji = pGene[i];
        int ji_1 = pGene[i - 1];
        for (int k = 1; k < jobManager.jobsInfo[i].procNum; k++)
        {
            c[ji][k]=MAX(c[ji_1][k], c[ji][k-1])+ jobManager.jobsInfo[ji].procInfo[k].time;
        }
    }
    int jn = pGene[jobManager.GetJobNum() - 1];
    int m = jobManager.GetMachineNum() - 1;
    return c[jn][m];
}

void Population::MakeNewGeneration()
{
    UpdateFitness();
    Sort();

    for (int i = 0; i < MAX_CHROMOSOME_NUM; i++)
        chromosomes[i].Show();
    printf("Start to make new generation===============================\r\n");

    for (int i = 0; i < nIterNum; i++)
    {
        UpdateElitist();
        SelectNewGeneration();
        Crossover();
        Mutation();
        Sort();

        printf("min fitness: ");
        chromosomes[0].Show();
    }
    printf("Finished!!===================================\r\n");
    //for (int i = 0; i < MAX_CHROMOSOME_NUM; i++)
    //    genes[i].Show();
}


/*
功能描述：根据适应度选择新的一代.采用了精英保留的算法，即适应度最好的个体会保留下来。
*/
void Population::SelectNewGeneration()
{
    float sumOfFit = 0;
    int sz = chromosomes.size(), j = 0,i = 0;
    for (i = 0; i < sz; i++)
        sumOfFit += 1.0f/ chromosomes[i].GetFitness();

    float temp = 0;
    for (i = 0; i < sz; i++)
    {
        temp += 1.0f/ chromosomes[i].GetFitness();
        chromosomes[i].SetCFitness(temp / sumOfFit);
    }
    vector<Chromosome> newGen(sz);
    for (int i = 0; i < nElitistNum; i++)
        newGen[i] = chromosomes[i];

    for (i = nElitistNum; i < sz; i++)
    {
        float p = random.getFloat(0, 1.0f);
        if (p < chromosomes[0].GetCFitness())
            newGen[i] = chromosomes[0];
        else
        {
            for (int j = 0; j < sz-1; j++)
            {
                if (chromosomes[j].GetCFitness() <= p && p < chromosomes[j + 1].GetCFitness())
                {
                    newGen[i] = chromosomes[j+1];
                    break;
                }
            }
            if(j>=sz)
                newGen[i] = chromosomes[sz-1];
        }
    }
    chromosomes = newGen;
}

//选择两个基因进行单点交叉
void Population::Crossover()
{
    int first = 0;
    Chromosome *father = NULL, *mother = NULL;

    for (int i = nElitistNum; i < chromosomes.size(); i++)
    {
        float p = random.getFloat(0, 1.0f);
        if (p < fCross)
        {
            first++;
            if ((first & 1) == 1)
            {
                father = &chromosomes[i];
            }
            else
            {
                mother = &chromosomes[i];
                father->CrossOver(*mother);
                mother->SetFitness(CalFitness(jobs, *mother));
                father->SetFitness(CalFitness(jobs, *father));
            }
        }
    }
}

void Population::Mutation()
{
    for (int i = nElitistNum; i < chromosomes.size(); i++)
    {
        for (int j = 0; j < chromosomes[i].GetLength()/2; j++)
        {
            float p = random.getFloat(0, 1.0f);
            if (p < fMutation)
            {
                chromosomes[i].Mutation(1);
                chromosomes[i].SetFitness(CalFitness(jobs, chromosomes[i]));
            }
        }
    }
}

int main() 
{
    Population ga(100);
    ga.Init();
    ga.MakeNewGeneration();

    //ga.UpdateFitness();
    //ga.Show();
}


