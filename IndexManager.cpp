//
// Created by Kfor on 2019/6/10.
//

#include "indexManager.h"


indexManager::indexManager(string tableName) {//构造函数，对于每一个attr，先判断有没有index，若有则构建索引
    catalogManager catalog;
    Attribute attr = catalog.getAttribute(tableName);

    for (int i = 0; i < attr.numberOfAttribute; i++) {
        if (attr.has_index[i]){
            createIndex("INDEX_FILE_"+attr.name[i]+tableName,attr.type[i]);
        }
    }
}

indexManager::~indexManager() {//析构函数，在释放时把map中B+树指针全部写回硬盘，并删除释放空间
    for (intMap::iterator itInt = indexIntMap.begin(); itInt != indexIntMap.end(); itInt++) {
        if (itInt->second) {
            itInt->second->writtenBackToDiskAll();
            delete itInt->second;
        }
    }
    for (stringMap::iterator itString = indexStringMap.begin(); itString != indexStringMap.end(); itString++) {
        if (itString->second) {
            itString->second->writtenBackToDiskAll();
            delete itString->second;
        }
    }
    for (floatMap::iterator itFloat = indexFloatMap.begin(); itFloat != indexFloatMap.end(); itFloat++) {
        if(itFloat->second) {
            itFloat->second->writtenBackToDiskAll();
            delete itFloat->second;
        }
    }
    delete intMap;
    delete floatMap;
    delete stringMap;
}


void indexManager::createIndex(string filePath, int type) {//建立索引
    int keySize = getKeySize(type); //获取key的size
    int degree = getDegree(type); //获取需要的degree

    if (type == TYPE_INT) {//在判断完数据类型之后
        BPlusTree<int> *tree = new BPlusTree<int>(filePath, keySize, degree);//根据索引建立对应的B+树
        indexIntMap.insert(intMap::value_type(filePath, tree));//将其放入map中
    }
    else if(type == TYPE_FLOAT) {
        BPlusTree<float> *tree = new BPlusTree<float>(filePath, keySize, degree);
        indexFloatMap.insert(floatMap::value_type(filePath, tree));
    }
    else {
        BPlusTree<string> *tree = new BPlusTree<std::string>(filePath, keySize, degree);
        indexStringMap.insert(stringMap::value_type(filePath, tree));
    }
}


void indexManager::insertIndex(std::string filePath, Data data, int blockId) {
    if (data.type == TYPE_INT) {//在判断完数据类型之后
        intMap::iterator itInt = indexIntMap.find(filePath);//找到对应的路径
        if (itInt == indexIntMap.end()) {
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itInt->second->insertKey(data.intData, blockId);//将数据插入进来
    }
    else if (data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
			return;
        }
        else
            itFloat->second->insertKey(data.floatData, blockId);
    }
    else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if (itString == indexStringMap.end()) {
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itString->second->insertKey(data.stringData, blockId);
    }
}

void indexManager::dropIndex(string filePath, int type) {//删除一个索引
    if (type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(filePath);//首先查找对应的值
        if (itInt == indexIntMap.end()) { //未找到就报错并返回
            cout << "Error: in drop index, no index " << filePath <<" exits" << endl;
			return;
        }
        else {
            delete itInt->second;//删除B+树
            indexIntMap.erase(itInt);//删除这一条
        }
    }
    else if (type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "Error: in drop index, no index " << filePath <<" exits" << endl;
            return;
        } else {
            delete itFloat->second;
            indexFloatMap.erase(itFloat);
        }
    }
    else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if (itString == indexStringMap.end()) {
            cout << "Error: in drop index, no index " << filePath <<" exits" << endl;
            return;
        } else {
            delete itString->second;
            indexStringMap.erase(itString);
        }
    }
}

int indexManager::findIndex(string filePath, Data data) {//查找数据对应的索引
    if (data.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(filePath);
        if (itInt == indexIntMap.end()) { //未找到
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
            return itInt->second->searchVal(data.intData);//找到就返回键值
    }
    else if(data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
            return itFloat->second->searchVal(data.floatData);
    }
    else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if (itString == indexStringMap.end()) {
            cout << "Error: in search index, no index " << filePath <<" exits" << endl;
            return -1;
        }
        else
            return itString->second->searchVal(data.stringData);
    }
}

void indexManager::deleteIndexByKey(string filePath, Data data) {//按照给出的data键值删除索引
    if (data.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(filePath);//先找到位置，判断是否有索引
        if (itInt == indexIntMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itInt->second->deleteKey(data.intData);//如果有索引，就调用B+树接口将其删除
    }
    else if (data.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itFloat->second->deleteKey(data.floatData);
    }
    else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
			itString->second->deleteKey(data.stringData);
    }
}

int indexManager::getDegree(int type) {//计算所需B+树的深度，实际效果相当于取log
    int degree = (PAGESIZE - sizeof(int)) / (getKeySize(type) + sizeof(int));
    if (degree % 2 == 0)
		degree -= 1;
    return degree;
}

int indexManager::getKeySize(int type) {//返回key大小
    if (type == TYPE_FLOAT)
        return sizeof(float);
    else if (type == TYPE_INT)
        return sizeof(int);
    else if (type > 0)
        return type;
    else {
        cout << "ERROR: in getKeySize: invalid type" << endl;
        return -100;
    }
}

void indexManager::searchRange(string filePath, Data data1, Data data2, vector<int>& vals) {//在范围内搜索，给出vector引用作为结果
    int flag = 0;
    //检测数据类型是否匹配
    if (data1.type == -2) {
        flag = 1;
    }
    else if (data2.type == -2) {
        flag = 2;
    }
    
    if (data1.type == TYPE_INT) {
        intMap::iterator itInt = indexIntMap.find(filePath);//寻找是否有索引
        if (itInt == indexIntMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itInt->second->searchRange(data1.intData, data2.intData, vals, flag);//有索引就修改vals为所需结果
    }
    else if (data1.type == TYPE_FLOAT) {
        floatMap::iterator itFloat = indexFloatMap.find(filePath);
        if (itFloat == indexFloatMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itFloat->second->searchRange(data1.floatData, data2.floatData, vals, flag);
    }
    else {
        stringMap::iterator itString = indexStringMap.find(filePath);
        if(itString == indexStringMap.end()) {
            cout << "Error:in search index, no index " << filePath <<" exits" << endl;
            return;
        }
        else
            itString->second->searchRange(data1.stringData, data2.stringData, vals, flag);
    }
}
