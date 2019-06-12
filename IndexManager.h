//
// Created by Kfor on 2019/6/09.
//
#ifndef __INDEXMANAGER__
#define __INDEXMANAGER__

#include <string>
#include <map>
#include "base.hpp"
#include "bufferManager.h"
#include "bPlusTree.h"

class indexManager{
public:
    indexManager(string tableName){};
    ~indexManager();
    void createIndex(string filePath,int type);
    void insertIndex(string filePath,Data data,int blockId);
    void dropIndex(string filePath,Data data);
    void deleteIndex(string filePath,Data data);
    void searchRange(string filePath,Data data1,Data data2,vector<int>& vals);
private:
    int getDegree(int type);
    int getKeySize(int type);

    typedef map<string,BPlusTree<int> *> intMap;
    typedef map<string,BPlusTree<float> *> floatMap;
    typedef map<string,BPlusTree<string> *> stringMap;
    int static const TYPE_FLOAT = 0;
    int static const TYPE_INT = -1;

    intMap indexIntMap;
    floatMap indexFloatMap;
    stringMap indexStringMap;
}