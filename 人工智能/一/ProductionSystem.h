#pragma once

#include <stdio.h>

#define MAX_SIZE  50

struct Node {  				//特征类 
    int id; 				//编号
    char name[MAX_SIZE];  	//名称 
};

struct Statu { 		//状态类 
    int statu; 		//0未使用 1使用过一次 2没有使用但不影响 
    int id;
}; //可替换修改

struct Fact { 				//数据库 
    int st_num; 			//初始事实数
    int now_num; 			//当前事实数，不断更新加入中间结论
    int neFlag; 			//条件不足置1 
    Statu data[MAX_SIZE]; 	//特征数组 
};

struct Rule {  			//规则库 
    int data_num; 		//前提个数
    int id_set[10]; 	//前提编号数组 
    int result; 		//最终结论
    int matched;  		//是否已匹配成功  0 1
}; 		//规则库 



