
#pragma once

#include<stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

#define NUMINROW 3

// 八数码状态
class State {
public:

    int state[NUMINROW][NUMINROW];
    int spaceX;  //空格的x坐标
    int spaceY;  //空格的y坐标
    int hashCode;//当前状态的唯一ID号
    int fn;

    State *parent;

    State()
    {
        spaceX = spaceY = -1;
        parent =  NULL;
        hashCode = 0;
        fn = INT32_MAX;
    }
    State(int a[NUMINROW][NUMINROW])
    {
        memcpy(state, a, sizeof(state));
        parent =  NULL;
        hashCode = HashCode();
        fn = INT32_MAX;

        for (int i = 0; i < NUMINROW; i++)
        {
            for (int j = 0; j < NUMINROW; j++)
            {
                if (a[i][j] == 0)
                {
                    spaceX = j;
                    spaceY = i;
                    return;
                }
            }
        }
    }
    ~State()
    {
        if (!parent)
        {
            delete parent;
            parent = NULL;
        }
        spaceX = spaceY = -1;
    }
    //返回值0：表示相同，否则表示不同
    int Compare(State &stat)const {
        //return memcmp(state, stat.state, sizeof(state));
        return hashCode == stat.hashCode?0:1;
    }
    void CloneTo(State &dst)
    {
        memcpy(dst.state, state, sizeof(state));
        dst.spaceX = spaceX;
        dst.spaceY = spaceY;
        dst.parent = parent;
        dst.hashCode = hashCode;
    }
    void Show() 
    {
        printf("current state:\r\n");
        for (int i = 0; i < NUMINROW; i++)
        {
            for (int j = 0; j < NUMINROW; j++)
            {
                printf("%4d", state[i][j]);
            }
            printf("\r\n");
        }
    }

    int Depth()const 
    {
        int depth = 1;
        State *ptemp = parent;
        while (ptemp)
        {
            depth++;
            ptemp = ptemp->parent;
        }
        return depth;
    }

    //获得两者间不同数字的数量(可作为启发式算法的h(n))
    int GetDiffNum(State &stat)const 
    {
        int num = 0;
        for (int i = 0; i < NUMINROW; i++)
        {
            for (int j = 0; j < NUMINROW; j++)
            {
                if (state[i][j] != stat.state[i][j])
                    num++;
            }
        }
        return num;
    }

    int Gn(State &dst)
    {
        return 1;
    }
    void UpdateFn(State &dst)
    {
#if 1
        fn = Depth() + GetDiffNum(dst);
#else
        fn = Depth() + Gn(dst);
#endif
    }
    bool CanMoveTo(int x, int y)
    {
        if (parent && parent->spaceX == x && parent->spaceY == y)
            return false;
        return (0 <= x && x < NUMINROW && 0 <= y && y < NUMINROW);
    }

    void GenChild(vector<State*>& list)
    {
        State *newOne = NULL;
        if (CanMoveTo( spaceX-1,spaceY))
        {
            newOne = new State();
            moveToLeft(*newOne);
            list.push_back(newOne);
        }
        if (CanMoveTo(spaceX + 1, spaceY))
        {
            newOne = new State();
            moveToRight(*newOne);
            list.push_back(newOne);
        }
        if (CanMoveTo(spaceX , spaceY- 1))
        {
            newOne = new State();
            moveToUp(*newOne);
            list.push_back(newOne);
        }
        if (CanMoveTo(spaceX, spaceY+1))
        {
            newOne = new State();
            moveToDown(*newOne);
            list.push_back(newOne);
        }
    }

    bool IsOddState() {//当前排列是奇排列吗？
        return true;
    }
private:
    bool moveTo(int x, int y, State &dst)
    {
        CloneTo(dst);
        dst.state[spaceY][spaceX] = state[y][x];
        dst.state[y][x] = 0;
        dst.spaceX = x;
        dst.spaceY = y;
        dst.parent = this;
        dst.hashCode = dst.HashCode();
        return true;
    }
    bool moveToLeft(State &dst)
    {
        return moveTo(spaceX - 1, spaceY, dst);
    }
    bool moveToRight(State &dst)
    {
        return moveTo(spaceX+1, spaceY, dst);
    }
    bool moveToUp(State &dst)
    {
        return moveTo(spaceX, spaceY-1, dst);
    }
    bool moveToDown(State &dst)
    {
        return moveTo(spaceX, spaceY + 1, dst);
    }

    int HashCode()// 康托展开
    {   
        int fact[10] = { 1,1,2,6,24,120,720,5040,40320,362880 };
        int num = 0;
        int len = NUMINROW*NUMINROW;
        int *ptr = &state[0][0];
        for (int i = 0; i < len; ++i) {
            int cnt = 0; // 在 i 之后，比 i 还小的有几个
            for (int j = i + 1; j < len; ++j)
                if (ptr[i] > ptr[j]) ++cnt;
            num += cnt * fact[len - i - 1];
        }
        return num + 1;
    }
};


class EightPuzzle
{
public:
    EightPuzzle(State& s, State& g)
    {
        s.CloneTo(root);
        g.CloneTo(goal);
    }
    ~EightPuzzle()
    {

    }

    //深度优先搜索
    int DFS();

    //A算法
    int ASearch();
    bool Solvable(State &s, State &g) { 
        if (s.IsOddState() == g.IsOddState())
            return true;
        else
            return false;
    };
private:
    State root;
    State goal;

    bool IsGoal(State &temp)
    {
        return 0==goal.Compare(temp);
    }

    void ShowPath(State &temp)
    {
        if (temp.parent)
            ShowPath(*temp.parent);
        temp.Show();
    }
    void ReleaseMem(vector<State*> &list)
    {
        for (int i = 0; i < list.size(); i++)
        {
            delete list[i];
        }
        list.clear();
    }

    int Fn(const State &aNode)
    {
        return aNode.fn;
    }

    //按照Fn降序方式排序
    void SortByDesend(vector<State*> &list)
    {
        sort(list.begin(), list.end(),
            [this](const State *a, const State *b) -> bool { return Fn(*a)> Fn(*b); });

    }

    //以降序方式插入新的状态
    void InsertByDesend(vector<State*> &list, State *pcur)
    {
        int val = Fn(*pcur);
        vector<State*>::iterator it = list.begin();
        for(; it!=list.end(); it++)
        {
            State *pEle = *it;
            if (Fn(*pEle) < val)
            {
                list.insert(it, pcur);
                return;
            }
        }
        list.push_back(pcur);
    }

    //检查list中是否包含cur
    State *Contain(vector<State*> list, const State & cur)
    {
        for each (State* var in list)
        {
            if (cur.Compare(*var) == 0)
                return var;
        }
        return NULL;
    }
    //删除与pcur相同的状态
    State * Erase(vector<State*> &list, State *pcur)
    {
        vector<State*>::iterator itc=list.begin();
        for (; itc < list.end(); itc++)
        {
            //if((*itc)->hashCode == pcur->hashCode)
            if ((*itc)->Compare(*pcur) == 0)
            {
                State *ret = *itc;
                itc = list.erase(itc);
                return ret;
            }
        }
        return NULL;
    }

    void ShowStatus(const vector<State*> &list)
    {
        int i, sz = list.size();
        int step = 28;
        sz = sz / step * step;

        printf("********************\r\n");
        for ( i = 0; i < sz; i+= step)
        {
            for (int j = 0; j < step; j++)
            {
                printf("%4d    ", Fn(*list[i + j]));
            }
            printf("\r\n");
            for (int j = 0; j < NUMINROW; j++)
            {
                for (int n = 0; n < step; n++)
                {
                    State *pcur = list[i + n];
                    for (int m = 0; m < NUMINROW; m++)
                    {
                        if(pcur->state[j][m]==0)
                            printf(" *");
                        else
                            printf("%2d", pcur->state[j][m]);
                    }
                    printf("  ");
                }
                printf("\r\n");
            }
            //if(sz<list.size())printf("\r\n");
            if (i < sz)printf("--------------\r\n");
        }
        for (int j = sz; j < list.size(); j++)
        {
            printf("%4d    ", Fn(*list[j]));
            if (j == list.size() - 1)
                printf("\r\n");
        }
        for (int j = 0; j < NUMINROW; j++)
        {
            for (int n = 0; n < list.size()-sz; n++)
            {
                State *pcur = list[i + n];
                for (int m = 0; m < NUMINROW; m++)
                {
                    if (pcur->state[j][m] == 0)
                        printf(" *");
                    else
                        printf("%2d", pcur->state[j][m]);
                }
                printf("  ");
            }
            printf("\r\n");
        }
        //printf("\r\n");
    }
};

