#include<iostream>
#include<vector>
#include<stdio.h>
#include<algorithm>
#include<cstring> 

using namespace std;

typedef pair<int, double> PAIR;

#define MNIST_IMAGE_SZ 784
typedef unsigned char uint8;


struct MnistBinImage
{
    uint8   Data[MNIST_IMAGE_SZ];
    int     nLabel;

    float Distance(MnistBinImage &other)
    {
        float dist = 0;
        for (int i = 0; i < MNIST_IMAGE_SZ; i++)
        {
            float diff = (float)Data[i] - other.Data[i];
            dist += diff*diff;
        }
        return dist;
    }
};


class SimpleMnistData
{
public:
    SimpleMnistData():trainList(), testList(){};
    ~SimpleMnistData() { Release(); };

    vector<MnistBinImage*> trainList;
    vector<MnistBinImage*> testList;

    bool Load(const char *trainfile, const char *testfile, const char *testlabelfile)
    {
        return (LoadTrainData(trainfile, trainList)&& LoadTestData(testfile, testlabelfile, testList));
    }
    void Release() 
    { 
        Release(trainList);
        Release(testList);
    }
private:
    bool LoadTrainData(const char *datafile, vector<MnistBinImage*> &list)
    {
        FILE *fp = fopen(datafile, "r");
        if (!fp)return false;
        list.clear();

        char buf[8192];
        int count = 0;
        while (fgets(buf, 8192, fp))
        {
            MnistBinImage *item = new MnistBinImage();
            Line2Array(buf, *item);
            list.push_back(item);
        }

        fclose(fp);
        return true;
    }
    bool LoadTestData(const char *datafile,const char* labelfile, vector<MnistBinImage*> &list)
    {
        FILE *datafp = fopen(datafile, "r");
        if (!datafp)return false;
        FILE *labelfp = fopen(labelfile, "r");
        if (!labelfile)
        {
            fclose(datafp);
            return false;
        }
        list.clear();
#define BUF_LEN 8192
        char buf[BUF_LEN];
        int count = 0;
        while (fgets(buf, BUF_LEN, datafp))
        {
            int len = strlen(buf)-1;
            if(!fgets(buf + len, BUF_LEN - len, labelfp))
                break;

            MnistBinImage *item = new MnistBinImage();
            Line2Array(buf, *item);
            list.push_back(item);
        }

        fclose(datafp);
        fclose(labelfp);
        return true;
    }

    void Line2Array(char *pline, MnistBinImage &buf)
    {
        int len = strlen(pline), index = 0;
        for (int i = 0; i < len; i++)
        {
            if (pline[i] == '0')
                buf.Data[index] = 0;
            else if (pline[i] == '1')
                buf.Data[index] = 1;
            else
                continue;
            index++;
            if (index >= MNIST_IMAGE_SZ)
                break;
        }
        buf.nLabel = pline[len - 2] - '0';  //图像的标签
    }

    void Release(vector<MnistBinImage*> &list) {
        for (size_t i = 0; i < list.size(); i++)
        {
            delete list[i];
        }
        list.clear();
    }
};


class KNN
{
    int constK;//KNN算法的参数K
public:
    KNN(int K=10) { constK = K; };
    ~KNN() {};

    void Classify(vector<MnistBinImage*> &trainData, vector<MnistBinImage*>& testData, vector<int> & resultLabel)
    {
        vector<PAIR> distList;
        for (size_t i = 0; i < testData.size(); i++)
        {
            
            MnistBinImage* item = testData[i];
            GetTestDistList(trainData, *item, distList);
            Sort(distList);
            int label = GetLabelByK(distList);
            resultLabel.push_back(label);

            if(i%100 == 0)
                printf("iter = %d\r\n", i);
        }
    }
private:

    int GetLabelByK(const vector<PAIR> &distList)
    {
        int len = constK;
        if (len > distList.size())
            len = distList.size();

        int count[10] = {}, maxCount = 0, maxLabel = -1;

        for (int i = 0; i < len; i++)
        {
            count[distList[i].first]++;
            if (count[distList[i].first] > maxCount)
            {
                maxCount = count[distList[i].first];
                maxLabel = distList[i].first;
            }
        }
        return maxLabel;
    }

    void GetTestDistList(vector<MnistBinImage*> &trainData, MnistBinImage& item, vector<PAIR>& distList)
    {
        distList.clear();
        for (size_t j = 0; j < trainData.size(); j++)
        {
            PAIR p;
            p.second = trainData[j]->Distance(item);
            p.first = trainData[j]->nLabel;

            distList.push_back(p);
        }
    }
    void Sort(vector<PAIR> &list) 
    {
        sort(list.begin(), list.end(),[](const PAIR& a, const PAIR& b) -> bool { return a.second < b.second; });
    }
};

static float GetCorrectRatio(vector<MnistBinImage*> testList, vector<int> &resultList)
{
    if (testList.size() != resultList.size())
    {
        printf("Error: testList.size() != resultList.size\r\n");
        return -1;
    }
    int correctCount = 0;
    for (size_t i = 0; i < testList.size(); i++)
    {
        if (testList[i]->nLabel == resultList[i])
            correctCount++;
    }
    return (float)correctCount / testList.size();
}

int main()
{
    SimpleMnistData mnistDataset;
    mnistDataset.Load("./mnist/train.format", "./mnist/test.format", "./mnist/test.predict");

    KNN knn(10);

    vector<int> resultList;
    knn.Classify(mnistDataset.trainList, mnistDataset.testList, resultList);

    float ratio = GetCorrectRatio(mnistDataset.testList, resultList);
    printf("correct ratio = %5.3f\r\n", ratio);

    printf("Press any key to continue!!");
    getchar();
    return 0;
}
