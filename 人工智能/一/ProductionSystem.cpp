#include "ProductionSystem.h"
#include <stdlib.h>
#include <memory.h>

using namespace std;

Rule ruleDB[MAX_SIZE];
Node targetDB[20];			//目标动物
Node featuresDB[MAX_SIZE];	//特征数据库
Fact indata;  			//输入数据 
int rule_num = 0; 		//规则条数
int feature_num = 0; 		//特征数

void readFeature() { //读取特征库 
    FILE *fp;
    if ((fp = fopen("features.txt", "r")) == NULL) {
        printf("Cant open features.txt!\n");
        exit(0);
    }

    int i = 0;
    while ((fscanf(fp, "%d %s", &featuresDB[i].id, &featuresDB[i].name)) != EOF)
        i++; //读入

    feature_num = i;
    fclose(fp);
}

void readRule() { //读取规则库 
    FILE *fp;
    if ((fp = fopen("rules.txt", "r")) == NULL) {
        printf("Cant open rules.txt!\n");
        exit(0);
    }

    int i = 0;
    char ch;
    ruleDB[i].data_num = 0;

    int t = 1;
    while ((ch = fgetc(fp)) != EOF) { //文件结尾 
        int cnt = 0; //规则前提个数 
        while (ch != '\n'&&ch != EOF) { //每一条规则 
            int tmp_id = 0;

            while (ch >= '0'&&ch <= '9') {
                tmp_id = tmp_id * 10 + ch - '0';
                ch = fgetc(fp);
            }
            ruleDB[i].id_set[cnt++] = tmp_id;

            tmp_id = 0; //处理结论
            if (ch == '=') {
                ch = fgetc(fp);
                while (ch >= '0'&&ch <= '9') {
                    tmp_id = tmp_id * 10 + ch - '0';
                    ch = fgetc(fp);
                }
                ruleDB[i].result = tmp_id;
            }
            else if (ch == '&') ch = fgetc(fp);
        }

        ruleDB[i].data_num = cnt; //该规则前提数 
        i++;
    }
    rule_num = i; //规则条数 

    fclose(fp);
    //	for(int i=0;i<rule_num;i++){
    //		for(int j=0;j<rule[i].data_num;j++){
    //			printf("%s",data[rule[i].id_set[j]].name);
    //			printf("%c",j==rule[i].data_num-1?'=':'&');
    //		}
    //		printf("%s\n",data[rule[i].result].name);
    //	}
}

void readTarget() { //读取目标库 
    FILE *fp;
    if ((fp = fopen("target.txt", "r")) == NULL) {
        printf("Cant open target.txt!\n");
        exit(0);
    }

    int i = 0;
    while ((fscanf(fp, "%d %s", &targetDB[i].id, &targetDB[i].name)) != EOF) i++; //读入
    fclose(fp);
}

//返回值表示输入特征的数量
int valueInput() { //输入是否有效 

    int tmp; //输入序号
    int cnt = 0; //特征数 

    while (cnt <=0)
    {
        printf("请输入已知动物特征的序号,以-1结束：\n");
        do {
            scanf("%d", &tmp);
            if (-1 == tmp)
                break;
            else if (tmp >= feature_num || tmp<-1) { //越界 
                printf("特征序号只能在0-%d之间，请重新输入:\n", feature_num - 1);
                fflush(stdin);
                cnt = 0;
                break;
            }
            indata.data[cnt++].id = tmp;
        } while (cnt<MAX_SIZE);
    }
    indata.st_num = cnt; //输入的特征数
    indata.now_num = indata.st_num;

    return cnt;
}

int isOther(int in_id, int result) { //消解冲突 
    int queue[MAX_SIZE], vis[MAX_SIZE]; //加入队列标记 
    int head = 0, tail = 1;
    int flag = 0;

    queue[0] = result; vis[result] = 1;
    while (head != tail&&flag != 1) {
        for (int i = 0; i<rule_num; i++)
            if (queue[head] == ruleDB[i].result) { //结果匹配规则结论 
                for (int j = 0; j<ruleDB[i].data_num; j++)
                    if (indata.data[in_id].id == ruleDB[i].id_set[j])
                        flag = 1;
                    else {
                        if (vis[ruleDB[i].id_set[j]] != 1) {
                            queue[tail++] = ruleDB[i].id_set[j];
                            vis[ruleDB[i].id_set[j]] = 1;
                        }
                    }
            }
        head++;
    }
    return flag;
}

bool isContract(int result) { //是否冲突 
    for (int i = 0; i<indata.now_num; i++)
        if (indata.data[i].statu == 0 && indata.data[i].id != result) { //未访问的输入特征 
            if (isOther(i, result))
                indata.data[i].statu = 2;
            else
                return true;
        }
    return false;
}

void isMatch(int result, int *finish) { //匹配最终结论
    if (result >= feature_num) { //已推理出动物 
        *finish = 1; //任务完成 
        if (isContract(result))
            printf("条件冲突，没有这样的动物！\n");
        else
            printf("它是 %s.\n", targetDB[result - feature_num-1].name);
    }

}

void addToFact(int rule_id, int *finish) { //匹配更新 
    for (int i = 0; i < ruleDB[rule_id].data_num; i++) //标记访问
    {
        for (int k = 0; k < indata.now_num; k++)
        {
            if (indata.data[k].id == ruleDB[rule_id].id_set[i])
                indata.data[k].statu = 1;
        }
    }

    ruleDB[rule_id].matched = 1;

    int vis = 0;  //标记结论是否存在 
    for (int i = 0; i < indata.now_num; i++)
    {
        if (indata.data[i].id == ruleDB[rule_id].result)   //匹配结论
        {
            vis = 1; 
            break;
        }
    }

    if (vis == 0) {
        indata.data[indata.now_num].id = ruleDB[rule_id].result;
        //printf("add the %d\n",indata.data[indata.now_num].id);
        indata.now_num++;
    }

    isMatch(ruleDB[rule_id].result, finish);
}

void count_unum(int *unusef, int *u_num) { //未访问特征数 
    int vis[MAX_SIZE]; //未访问事实加组标记 
    memset(vis, 0, sizeof(vis));

    for (int i = 0; i<indata.now_num; i++) {
        if (indata.data[i].statu == 0 && vis[indata.data[i].id] != 1) {
            unusef[*u_num] = indata.data[i].id;
            ++(*u_num);
            vis[indata.data[i].id] = 1;
        }
    }

}

void mayMatch(int tflag, int rule_id, int *getflag) { //匹配可能性结论 
    Node *res;
    int res_id;
    res = featuresDB;
    //printf("result=%d\n",rule[rule_id].result);
    res_id = ruleDB[rule_id].result;
    getflag[ruleDB[rule_id].result] = 1;

    if (res_id>23) {
        res = targetDB;
        res_id -= 24;
    }

    if (tflag == 0)
        printf("条件不完全，但它有%s", res[res_id].name);
    else
        printf("和%s", res[res_id].name);
}

void guess() { //可能结论 
    int tflag, ecnt; //相同个数
    int unusef[MAX_SIZE], u_num = 0;
    int getflag[MAX_SIZE];//若已推出这个"可能结论"，就置为1
    memset(getflag, 0, sizeof(getflag));

    count_unum(unusef, &u_num);

    tflag = 0;
    for (int i = 0; i<rule_num; i++) {
        ecnt = 0;
        for (int j = 0; j<ruleDB[i].data_num; j++)
            for (int k = 0; k<u_num; k++)
                if (unusef[k] == ruleDB[i].id_set[j])
                    ecnt++;
        if (ecnt * 2 >= ruleDB[i].data_num&&getflag[ruleDB[i].result] != 1) {
            mayMatch(tflag, i, getflag);
            tflag = 1;
        }
    }
    if (tflag == 0)
        printf("条件不足，不能推出它是什么动物！\n");
    else
        printf("的部分特征.\n");
}

void forwardWork() { //正向推理 
    int finish = 0; //推理是否完成 0 1
    int factNew = 1; //一次推理是否得出新事实
    int fitFlag; //1 可推理 
    int fitPart;  //1 部分符合 

    while (!finish&&factNew == 1) {
        factNew = 0;

        for (int i = 0; i<rule_num&&ruleDB[i].matched != 1 && finish == 0; i++) {  //取出一条规则 
            if (indata.now_num >= ruleDB[i].data_num) {  //现有事实数不小于当前规则条件数 
                fitFlag = 1;
                for (int j = 0; j<ruleDB[i].data_num&&fitFlag == 1; j++) { //比较是否匹配该规则  
                    fitPart = 0;
                    for (int k = 0; k<indata.now_num; k++)
                        if (indata.data[k].id == ruleDB[i].id_set[j])
                            fitPart = 1; //部分匹配 
                    fitFlag = fitPart;
                }

                if (fitFlag == 1) { //与该规则匹配 
                                    //printf("匹配第%d条规则\n",i);
                    factNew = 1;
                    addToFact(i, &finish); //更新到事实库 
                    fitFlag = 0;
                }
            }
        }
    }
    if (factNew == 0) //当没有推出任何动物，看是否极有可能得出一些结论 
        guess();
}



//////////////////////////////////////////////////////////////////////

void InitDatabase() { //初始化数据库 
              //读取文件 
    readFeature();
    readRule();
    readTarget();
}

void init() { //初始化状态值 	
    for (int i = 0; i<MAX_SIZE; i++) {
        ruleDB[i].matched = 0; //未匹配成功
        indata.neFlag = 0;
        indata.data[i].statu = 0; //特征未访问 
    }
}

void showFeatureDB() { //界面 
    printf("\n\t******************动物识别系统******************\t\n\n\t");
    for (int i = 0; i<feature_num; i++) {
        printf("[%2d] %-12s", featuresDB[i].id, featuresDB[i].name);
        if (i % 3 == 2) printf("\n\t");
    }
    printf("\n\t***********************************************\t\n");
}



int main() { //处理主函数  
    InitDatabase();
    char ch;
    do {
        //init();
        system("cls");
        showFeatureDB();

        valueInput(); //输入特征信息

        forwardWork();	//正向推理

        printf("\n按任意键继续,按'n'或'N'退出:");
        ch = getchar(); 
        ch = getchar();
    } while (ch != 'n'&&ch != 'N');  //结束标记
    return 0;
}

