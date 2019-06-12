//
// Created by Kfor on 2019/6/09.
//

//此处给出的是index的创建、删除、修改的总控类的声明。
//这个类构建了三个基于B+树的map，分别储存int、float、string类型的三张index表；
//以及若干用于操作这些数据结构的函数。
//这样，其他文件就可以通过调用这个类中的接口，来操作index表，实现加快索引的目的。
#ifndef __INDEXMANAGER__
#define __INDEXMANAGER__

#include <string>
#include <map>
#include <vector>

#include "base.hpp"//基础数据结构
#include "bufferManager.h"//用于读写操作
#include "BPlusTree.h"//作为map打包的基础
#include "catalogManager.h"

using namespace std;

class indexManager{
public:
    indexManager(string tableName);//构造函数
    ~indexManager();//析构函数
    void createIndex(string filePath,int type);//建立索引
    void insertIndex(string filePath,Data data,int blockId);//插入索引
    void dropIndex(string filePath,int type);//删除一个索引
    int findIndex(std::string file_path, Data data);
    void deleteIndexByKey(string filePath,Data data);
    void searchRange(string filePath,Data data1,Data data2,vector<int>& vals);
private:
    int getDegree(int type);//获取到B+树的深度
    int getKeySize(int type);//获取到key的大小

    typedef map<string,BPlusTree<int>*> intMap;//以下针对不同的数据类型构建了三张表
    typedef map<string,BPlusTree<float>*> floatMap;
    typedef map<string,BPlusTree<string>*> stringMap;
    int static const TYPE_FLOAT = 0;
    int static const TYPE_INT = -1;

    intMap indexIntMap;
    floatMap indexFloatMap;
    stringMap indexStringMap;
}