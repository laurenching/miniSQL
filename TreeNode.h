//
// Created by Kfor on 2019/6/09.
//

//这个文件构建了一个模板类TreeNode，用来存放B+树的节点以及实现相关的操作

#ifndef __TREENODE__
#define __TREENODE__


#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "base.hpp"
#include "exception.h"
#include "bufferManager.h"


template <typename T>
class TreeNode {
public:
    unsigned int num;//该结点内key数量
    TreeNode* parent;//指向父节点
    vector <T> keys;//存放key
    vector <int> vals;//存放value，其实际意义为该处的索引
    vector <TreeNode*> childs;//指向子结点
    TreeNode* nextLeafNode;//指向下一个叶结点的
    bool isLeaf;//判断是否为叶节点
    int degree;//节点的度

    TreeNode(int inDegree, bool Leaf = false);//构造函数，如果为叶节点就创建叶
    ~TreeNode(){};//默认的析构函数
    bool isRoot();//判断是否为根节点
    bool findKey(T key, unsigned int &index);//根据输入的key来搜索，返回值表明是否搜索到，index引用返回值的index
    TreeNode* splitNode(T &key);//分裂节点，返回下一个节点。在B+树分裂时使用
    unsigned int addKey(T &key);//在枝干节点中添加key，返回position
    unsigned int addKey(T &key, int val);//在叶节点中添加key，返回postion
    bool deleteKeyByIndex(unsigned int index);//删除index对应的key并返回index
    TreeNode* nextLeaf();//返回下一个叶节点的指针

    bool findRange(unsigned int index, T& key, vector<int>& vals);//起始的index和终止的key，返回结果的vector
    bool findRange2(unsigned int index, vector<int>& vals);
    
    void printl();
};

template <class T>
TreeNode<T>::TreeNode(int inDegree, bool Leaf)://给各个属性赋初值
	num(0),parent(NULL),nextLeafNode(NULL),isLeaf(Leaf),degree(inDegree)
{
    for (unsigned i = 0; i < degree+1; i++) {//根据这个节点的度数先赋好初始的空值
        childs.push_back(NULL);
        keys.push_back(T());
        vals.push_back(int());
    }
    childs.push_back(NULL);//给最后一个位置添加一个child，即孩子数要比key数多一
}

template <class T>
bool TreeNode<T>::isRoot() {//根据是否有父亲节点来判断是否为根节点
    if (parent != NULL)
		return false;
    else
		return true;
}

template <class T>
bool TreeNode<T>::findKey(T key, unsigned int &index) {
    if (num == 0) { //结点内key数量为0
        index = 0;
        return false;
    }
    else {
        //判断key值是否超过本结点内最大值(key不在本结点内)
        if (keys[num-1] < key) {
            index = num;
            return false;
		//判断key值是否小于本结点内最小值(key不在本结点内)
        }
        else if (keys[0] > key) {
            index = 0;
            return false;
        }
        else if (num <= 20) {
		//结点内key数量较少时直接线性遍历搜索即可
            for (unsigned int i = 0; i < num; i++) {
                if (keys[i] == key) {
                    index = i;
                    return true;
                }
                else if (keys[i] < key)
                    continue;
                else if(keys[i] > key) {
                    index = i;
                    return false;
                }
            }
        }
        else if(num > 20) {
		//结点内key数量过多时采用二分搜索
            unsigned int left = 0, right = num-1, pos = 0;

            while (right > left+1) {
                pos = (right + left) / 2;
                if (keys[pos] == key) {
                    index = pos;
                    return true;
                }
                else if (keys[pos] < key) {
                    left = pos;
                }
                else if (keys[pos] > key) {
                    right = pos;
                }
            }

            if (keys[left] >= key) {
                index = left;
                return (keys[left] == key);
            }
            else if (keys[right] >= key) {
                index = right;
                return (keys[right] == key);
            }
            else if(keys[right] < key) {
                index = right ++;
                return false;
            }
        }//二分搜索结束
    }
    return false;
}


template <class T>
TreeNode<T>* TreeNode<T>::splitNode(T &key)
{
    unsigned int minmumNodeNum = (degree - 1) / 2;//计算最小节点数量
    TreeNode* newNode = new TreeNode(degree, this->isLeaf);//创建新节点

    /*
    if (newNode == NULL) {
        cout << "Problems in allocate momeory of TreeNode in splite node of " << key << endl;
        exit(2);
    }
	*/

    if (isLeaf) {//当前结点为叶节点
        key = keys[minmumNodeNum + 1];
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {//将右边的key值拷贝至新节点内
            newNode->keys[i-minmumNodeNum-1] = keys[i];
            keys[i] = T();
            newNode->vals[i-minmumNodeNum-1] = vals[i];
            vals[i] = int();
        }
        newNode->nextLeafNode = this->nextLeafNode;//将节点相连
        this->nextLeafNode = newNode;
        newNode->parent = this->parent;

        newNode->num = minmumNodeNum;//调整两结点内key数量
        this->num = minmumNodeNum + 1;
    }
    else if (!isLeaf) {  //非叶结点情况
        key = keys[minmumNodeNum];
        for (unsigned int i = minmumNodeNum + 1; i < degree+1; i++) {//拷贝子节点指针至新节点
            newNode->childs[i-minmumNodeNum-1] = this->childs[i];
            newNode->childs[i-minmumNodeNum-1]->parent = newNode;
            this->childs[i] = NULL;
        }
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {//拷贝key值至新节点
            newNode->keys[i-minmumNodeNum-1] = this->keys[i];
            this->keys[i] = T();
        }
		
        this->keys[minmumNodeNum] = T();//调整节点相互位置关系
        newNode->parent = this->parent;

        newNode->num = minmumNodeNum;//调整节点内key数量
        this->num = minmumNodeNum;
    }

    return newNode;
}


template <class T>
unsigned int TreeNode<T>::addKey(T &key)
{
	//本结点内无key
    if (num == 0) {
        keys[0] = key;
        num ++;
        return 0;
    }
    else {
		//查找是否Key值已经存在
        unsigned int index = 0;
        bool exist = findKey(key, index);
        if (exist) {
			/*
            cout << "Error:In add(T &key),key has already in the tree!" << endl;
            exit(3);
			*/
        }
        else { //不存在，可以进行插入
			//调整其他key值
            for (unsigned int i = num; i > index; i--)
                keys[i] = keys[i-1];
            keys[index] = key;

			//调整子结点指针情况
            for (unsigned int i = num + 1; i > index+1; i--)
                childs[i] = childs[i-1];
            childs[index+1] = NULL;
            num++;

            return index;
        }
    }

    return 0;
}

template <class T>
unsigned int TreeNode<T>::addKey(T &key, int val)
{	//若非叶结点，无法插入
    if (!isLeaf) {
		/*
        cout << "Error:add(T &key,int val) is a function for leaf nodes" << endl;
		*/
        return -1;
    }

	//结点内没有key值
    if (num == 0) {
        keys[0] = key;
        vals[0] = val;
        num ++;
        return 0;
    }
    else { //正常插入
        unsigned int index = 0;
        bool exist = findKey(key, index);
        if (exist) {
			/*
	        cout << "Error:In add(T &key, int val),key has already in the tree!" << endl;
	        exit(3);
			*/
        }
        else {
			//逐个调整key值
            for (unsigned int i = num; i > index; i--) {
                keys[i] = keys[i-1];
                vals[i] = vals[i-1];
            }
            keys[index] = key;
            vals[index] = val;
            num++;
            return index;
        }
    }

    return 0;
}

template <class T>
bool TreeNode<T>::deleteKeyByIndex(unsigned int index)
{
	//index错误，超过本结点范围
    if(index > num) {
		/*
        cout << "Error:In removeAt(unsigned int index), index is more than num!" << endl;
		*/
        return false;
    } else { //正常进行删除
        if (isLeaf) { //叶结点情况
			//逐个调整key值
            for (unsigned int i = index; i < num-1; i++) {
                keys[i] = keys[i+1];
                vals[i] = vals[i+1];
            }
            keys[num-1] = T();
            vals[num-1] = int();
        } else { //枝干结点情况
			//调整key值
            for(unsigned int i = index; i < num-1; i++)
                keys[i] = keys[i+1];

			//调整子结点指针
            for(unsigned int i = index+1;i < num;i ++)
                childs[i] = childs[i+1];

            keys[num-1] = T();
            childs[num] = NULL;
        }

        num--;
        return true;
    }

    return false;
}

template <class T>
TreeNode<T>* TreeNode<T>::nextLeaf() {
	return nextLeafNode;
}

template <class T>
bool TreeNode<T>::findRange(unsigned int index, T& key, std::vector<int>& valsout) {
	unsigned int i;
	for (i = index; i < num && keys[i] <= key; i++)
		valsout.push_back(vals[i]);
	if (keys[i] >= key)
		return true;
	else
		return false;
}

template <class T>
bool TreeNode<T>::findRange2(unsigned int index, std::vector<int>& valsout) {
	unsigned int i;
	for (i = index; i < num; i++)
		valsout.push_back(vals[i]);

	return false;
}

template <class T>//将当前节点上的每个key都输出
void TreeNode<T>::printl() {
    for (int i = 0; i < num; i++)
        cout << "->" << keys[i];
    cout << endl;
    
}

#endif