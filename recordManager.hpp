//
//  recordManager.hpp
//  minisql
//
//  Created by 楊淯晴 on 2019/6/5.
//  Copyright © 2019 楊淯晴. All rights reserved.
//

#ifndef recordManager_hpp
#define recordManager_hpp
#include<iostream>
#include"base.hpp"
#include <string>
#include<sstream>
#include "indexManager.h"
#include "catalogManager.h"
#include "bufferManager.h"
#include "exception.h"
#define INF 1000000
extern BufferManager bufferManager;

using namespace std;
class RecordManager
{
public:
    void createTableFile(string tableName);//創建表
    void deleteTableFile(string tableName);//刪除表
    void insertRecord(string tableName,Tuple & tuple);//插入數據
    int deleteRecord(string tableName);//刪除全部數據
    int deleteRecord(string tableName,string deleteAttr,Where where);//刪除部分數據
    Table selectRecord(string tableName,string resultTableName="tempTable");//返回整張表
    Table selectRecord(string tableName,string selectAttr,Where where,string resultTableName="tempTable");  //返回部分表(select語句)
    void createIndex(IndexManager & indexmanager,string tableName,string indexAttr);//建立索引
private:
    int calculateTupleLength(Tuple &tuple);//計算元組長度
    int getTupleLength(char *p);//獲得元組長度
    void insertTupleToTable(char *p,int offset,int length,const vector<Data>&data);//插入record 輔助模塊
    Tuple readTuple(const char*p,Attribute attr);//讀取一個元組
    void searchWithIndex(string tableName,string searchAttr,Where where,vector<int>&blockId);//用索引查找符合的塊
    int  deleteInBlock(string tableName , int blockId , Attribute attr , int index , Where where);//在塊中有條件刪除
    void selectInBlock(string tableName , int blockId , Attribute attr , int index , Where where , vector<Tuple>& tuples);//在塊中有條件搜索
    
    template <typename T>
    bool ifSatisfied(T where1 , T where2 , WHERE relation) {//是否滿足where關係
        bool satisfied=false;
        switch(relation) {
            case Less:{
                if (where1 < where2)
                    satisfied=true;
            };break;
            case lessEqual:{
                if (where1 <= where2)
                    satisfied=true;
            };break;
            case Equal:{
                if (where1 == where2)
                    satisfied=true;
            };break;
            case greaterEqual:{
                if (where1 >= where2)
                    satisfied=true;
            };break;
            case Greater:{
                if (where1 > where2)
                    satisfied=true;
            };break;
            case notEqual:{
                if (where1 !=where2)
                    satisfied=true;
            };break;
        }
        return satisfied;
    }
};
#endif /* recordManager_hpp */
