#include "SearchAlg.h"
#include "utils.h"
#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <memory.h>


int EightPuzzle::DFS()
{

    if (!Solvable(root, goal))
    {
        printf("问题无法求解！\r\n");
        return -1;
    }

    int depth = 0,step = 0;

    vector<State*> openTable;
    vector<State*> closeTable;
    State *p = new State();
    root.CloneTo(*p);
    openTable.push_back(p);
    while (!openTable.empty())
    {
        //ShowStatus(openTable);
        State *ptmp = openTable.back();
        openTable.pop_back();
        step++;
        if (IsGoal(*ptmp))
        {
            ShowPath(*ptmp);
            printf("step: %d, depth: %d\r\n", step, ptmp->Depth());
            ReleaseMem(closeTable);
            ReleaseMem(openTable);
            return step;
        }
        if (ptmp->Depth() > 23)//深度超过了限制
            continue;
        ptmp->GenChild(openTable);
        closeTable.push_back(ptmp);
    }
    ReleaseMem(closeTable);
    ReleaseMem(openTable);
    printf("DFS failed!!\r\n");

    return -1;
}
State *allStatus[362880] = {};

int EightPuzzle::ASearch()
{
    if (!Solvable(root, goal))
    {
        printf("问题无法求解！\r\n");
        return -1;
    }

    int depth = 0, step = 0;

    vector<State*> openTable;
    vector<State*> closeTable;
    State *p = new State();
    root.CloneTo(*p);
    allStatus[p->hashCode] = p;
    openTable.push_back(p);
    p->UpdateFn(goal);
    while (!openTable.empty())
    {
        //ShowStatus(openTable);
        //getchar();
        State *ptmp = openTable.back();
        openTable.pop_back();

        step++;
        if (IsGoal(*ptmp))
        {
            ShowPath(*ptmp);
            printf("step: %d, depth: %d\r\n", step, ptmp->Depth());
            ReleaseMem(closeTable);
            ReleaseMem(openTable);
            return step;
        }
        closeTable.push_back(ptmp);
        vector<State*> tmplist;
        ptmp->GenChild(tmplist);
        for(vector<State*>::iterator it = tmplist.begin();it!=tmplist.end();)
        {
            State *newone = *it;
            newone->UpdateFn(goal);
#if 1
            State *oldone = allStatus[newone->hashCode];
            if (oldone)//已经产生过该节点了
            {
                if (Fn(*oldone) > Fn(*newone))//新的节点路径更好
                {
                    if (!Erase(closeTable, oldone))
                    {
                        Erase(openTable, oldone);
                    }
                    oldone->parent = newone->parent;
                    oldone->UpdateFn(goal);
                    InsertByDesend(openTable, oldone);
                }
                it = tmplist.erase(it);
                delete newone;  
            }
            else
            {
                it = tmplist.erase(it);
                //加入新节点
                //openTable.push_back(newone);
                InsertByDesend(openTable, newone);
                allStatus[newone->hashCode] = newone;
            }
#else
            //State *oldone = Erase(openTable, newone);
            //if (!oldone)
            //{
            //    oldone = Erase(closeTable, newone);
            //}
            //if (Fn(*oldone) > Fn(*newone))
            State *oldone = Contain(openTable, *newone);
            if (oldone)
            {
                if (Fn(*oldone) > Fn(*newone))//新的节点路径更好
                    oldone->parent = newone->parent;
                it = tmplist.erase(it);
                delete newone;
            }
            else if((oldone = Contain(closeTable, *newone)))
            {
                if (Fn(*oldone) > Fn(*newone))//新点路径更好
                {
                    //删除旧节点
                    remove(closeTable.begin(), closeTable.end(), oldone);
                    delete oldone;
                    //加入新节点
                    openTable.push_back(newone);
                }
                else//旧节点路径更好
                {
                    it = tmplist.erase(it);
                    delete newone;
                }
            }
            else
            {
                it = tmplist.erase(it);
                //加入新节点
                openTable.push_back(newone);
            }
#endif
        }
        //SortByDesend(openTable);
    }
    ReleaseMem(closeTable);
    ReleaseMem(openTable);
    printf("DFS failed!!\r\n");

    return -1;
    return 0;
}
/*
    2 8 3       1 2 3
    1 6 4       8 0 4
    7 0 5       7 6 5
*/

int main()
{
    //int s[NUMINROW][NUMINROW] = { 2,8,3,1,6,4,7,0,5 };
    //int g[NUMINROW][NUMINROW] = { 1,2,3,8,0,4,7,6,5 };

    int s[NUMINROW][NUMINROW] = { 2,3,8,4,6,1,5,0,7 };
    int g[NUMINROW][NUMINROW] = { 0,1,2,3,4,5,6,7,8 };
    State init(s);
    State goal(g);
    EightPuzzle ep(init, goal);
    long start = Milliseconds();
    //ep.DFS();

    ep.ASearch();

    printf("\r\ntime: %ld.\r\nPress any key to quit!!", Milliseconds()-start);
    getchar();
    return 0;
}
