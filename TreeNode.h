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
    vector <TreeNode*> childs;//指向子结点
    TreeNode* nextLeafNode;//指向下一个叶结点的
    bool isLeaf;
    int degree;
    vector <int> vals;

    TreeNode(int inDegree, bool Leaf = false);
    ~TreeNode(){};
    bool isRoot();
    bool findKey(T key, unsigned int &index);
    TreeNode* splitNode(T &key);
    unsigned int addKey(T &key);
    unsigned int addKey(T &key, int val);
    bool deleteKeyByIndex(unsigned int index);
    TreeNode* nextLeaf();

    bool findRange(unsigned int index, T& key, vector<int>& vals);
    bool findRange2(unsigned int index, vector<int>& vals);
    
    void printl();
};





template <class T>
TreeNode<T>::TreeNode(int in_degree, bool Leaf):
	num(0),parent(NULL),nextLeafNode(NULL),isLeaf(Leaf),degree(in_degree)
{
    for (unsigned i = 0; i < degree+1; i++) {
        childs.push_back(NULL);
        keys.push_back(T());
        vals.push_back(int());
    }
    childs.push_back(NULL);
}

template <class T>
bool TreeNode<T>::isRoot() {
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
	//最小结点数量
    unsigned int minmumNodeNum = (degree - 1) / 2;
	//创建新结点
    TreeNode* newNode = new TreeNode(degree, this->isLeaf);

    /*
    if (newNode == NULL) {
        cout << "Problems in allocate momeory of TreeNode in splite node of " << key << endl;
        exit(2);
    }
	*/

	//当前结点为叶结点情况
    if (isLeaf) {
        key = keys[minmumNodeNum + 1];
		//将右半部分key值拷贝至新结点内
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {
            newNode->keys[i-minmumNodeNum-1] = keys[i];
            keys[i] = T();
            newNode->vals[i-minmumNodeNum-1] = vals[i];
            vals[i] = int();
        }
		//将新结点放置到本结点右边
        newNode->nextLeafNode = this->nextLeafNode;
        this->nextLeafNode = newNode;
        newNode->parent = this->parent;

		//调整两结点内key数量
        newNode->num = minmumNodeNum;
        this->num = minmumNodeNum + 1;
    }
    else if (!isLeaf) {  //非叶结点情况
        key = keys[minmumNodeNum];
		//拷贝子结点指针至新结点
        for (unsigned int i = minmumNodeNum + 1; i < degree+1; i++) {
            newNode->childs[i-minmumNodeNum-1] = this->childs[i];
            newNode->childs[i-minmumNodeNum-1]->parent = newNode;
            this->childs[i] = NULL;
        }
		//拷贝key值至新结点
        for (unsigned int i = minmumNodeNum + 1; i < degree; i++) {
            newNode->keys[i-minmumNodeNum-1] = this->keys[i];
            this->keys[i] = T();
        }
		//调整结点相互位置关系
        this->keys[minmumNodeNum] = T();
        newNode->parent = this->parent;

		//调整结点内key数量
        newNode->num = minmumNodeNum;
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