//
// Created by Kfor on 2019/6/09.
//

//这个文件构建了B+树的定义，用于存放index以供indexManger操作

#ifndef __BPLUSTREE__
#define __BPLUSTREE__

#include "TreeNode.h"

#endif

template <typename T>
class BPlusTree {
private:
	//重命名TreeNode指针，方便后边进行操作区分
    typedef TreeNode<T>* Tree;
	//特殊结构，用于临时存储查找的key值及所处的位置，方便进行操作
    struct searchNodeParse {
        Tree pNode; //包含对应key的结点指针
        unsigned int index; //key在结点中的index
        bool ifFound; //是否找到该key
    };

public:
    BPlusTree(string mName, int keySize, int degree);//构造函数，赋初始值并分配内存，从磁盘读取数据
    ~BPlusTree();//析构函数
    int searchVal(T &key);//搜索value值，即搜索索引
    bool insertKey(T &key, int val);//在val索引处插入一个key，返回是否成功
    bool deleteKey(T &key);//删除键
    void dropTree(Tree node);//删除树
    void searchRange(T &key1, T &key2, vector<int>& vals, int flag);//搜索两个key之间的值，通过修改vals得到索引结果。flag为节点类型
    void readFromDiskAll();//从磁盘读取数据
    void writtenBackToDiskAll();//写回磁盘
    void readFromDisk(char *p, char* end);//从磁盘读取制定数据
    void printleaf();
    
private:
    void initTree();//初始化
    bool adjustAfterinsert(Tree pNode);//插入后调整
    bool adjustAfterDelete(Tree pNode);//删除后调整
    void findToLeaf(Tree pNode, T key, searchNodeParse &snp);//在叶子节点中找值
    void getFile(string filePath);//获取文件
    int getBlockNum(string tableName);//获取block的id
    
    string fileName;//文件名
    Tree root;//根
    Tree leafHead;//叶节点的最左端，用于遍历结果用
    unsigned int keyNum;//关键字数目
    unsigned int level;//树的层数
    unsigned int nodeNum;//节点数目
    int keySize;//每个key的大小
    int degree;//每个节点的度
};


template <class T>
BPlusTree<T>::BPlusTree(string inName, int keySize, int inDegree):
	fileName(inName),keyNum(0),level(0),nodeNum(0),root(NULL),leafHead(NULL),keySize(keySize),degree(inDegree)
{
	//初始化分配内存并从磁盘读取数据
	//创建索引
    initTree();
    readFromDiskAll();
}

template <class T>
BPlusTree<T>:: ~BPlusTree()
{
    dropTree(root);
    keyNum = 0;
    root = NULL;
    level = 0;
}

//初始化B+树，并分配内存空间
template <class T>
void BPlusTree<T>::initTree()
{
    root = new TreeNode<T>(degree, true);
    keyNum = 0;
    level = 1;
    nodeNum = 1;
    leafHead = root;
}

//用于查找某key值所处的叶结点位置
template <class T>
inline void BPlusTree<T>::findToLeaf(Tree pNode, T key, searchNodeParse &snp)
{
    unsigned int index = 0;
	//在对应结点内查找key值
    if (pNode->findKey(key, index)) {
		//若此结点是叶结点，则查找成功
        if (pNode->isLeaf) {
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = true;
        } else {
			//此结点不是子结点，查找它的下一层
            pNode = pNode->childs[index+1];
            while (!pNode->isLeaf) {
                pNode = pNode->childs[0];
            }
			//因为已找到key值，所以其最底层叶结点index[0]即为该key
            snp.pNode = pNode;
            snp.index = 0;
            snp.ifFound = true;
        }

    } else { //本结点内未找到该key
        if (pNode->isLeaf) {
			//若此结点已经是叶结点则查找失败
            snp.pNode = pNode;
            snp.index = index;
            snp.ifFound = false;
        } else {
			//递归寻找下一层
            findToLeaf(pNode->childs[index], key, snp);
        }
    }

	return;
}

template <class T>
bool BPlusTree<T>::insertKey(T &key, int val)
{
    searchNodeParse snp;
	//根结点不存在
    if (!root)
		initTree();
	//查找插入值是否存在
    findToLeaf(root, key, snp);
    if (snp.ifFound) { //已存在
		/*
        cout << "Error:in insert key to index: the duplicated key!" << endl;
		*/
        return false;
    } else { //不存在，可以插入
        snp.pNode->addKey(key, val);
		//插入后结点满，需要进行调整
        if (snp.pNode->num == degree) {
            adjustAfterinsert(snp.pNode);
        }
        keyNum++;
        return true;
    }

    return false;
}

//用于插入某key后若结点满，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterinsert(Tree pNode)
{
    T key;
    Tree newNode = pNode->splitNode(key);
    nodeNum++;

	//当前结点为根结点情况
    if (pNode->isRoot()) {
        Tree root = new TreeNode<T>(degree, false);
        if (root == NULL) {
			/*
            cout << "Error: can not allocate memory for the new root in adjustAfterinsert" << endl;
            exit(1);
			*/
        } else {
            level ++;
            nodeNum ++;
            this->root = root;
            pNode->parent = root;
            newNode->parent = root;
            root->addKey(key);
            root->childs[0] = pNode;
            root->childs[1] = newNode;
            return true;
        }
    } else { //当前结点非根结点
        Tree parent = pNode->parent;
        unsigned int index = parent->addKey(key);

        parent->childs[index+1] = newNode;
        newNode->parent = parent;
		//递归进行调整
        if(parent->num == degree)
            return adjustAfterinsert(parent);

        return true;
    }

    return false;
}

template <class T>
int BPlusTree<T>::searchVal(T& key)
{
    if (!root)
        return -1;
    searchNodeParse snp;
    findToLeaf(root, key, snp);

    if (!snp.ifFound)
        return -1;
    else
        return snp.pNode->vals[snp.index];
}

template <class T>
bool BPlusTree<T>::deleteKey(T &key)
{
    searchNodeParse snp;
	//根结点不存在
    if (!root) {
		/*
        cout << "ERROR: In deleteKey, no nodes in the tree " << fileName << "!" << endl;
		*/
        return false;
    } else { //正常进行操作
		//查找位置
        findToLeaf(root, key, snp);
        if (!snp.ifFound) { //找不到该key
			/*
            cout << "ERROR: In deleteKey, no keys in the tree " << fileName << "!" << endl;
			*/
            return false;
        } else { //正常找到进行删除
            if (snp.pNode->isRoot()) { //当前为根结点
                snp.pNode->deleteKeyByIndex(snp.index);
                keyNum--;
                return adjustAfterDelete(snp.pNode);
            } else {
                if (snp.index == 0 && leafHead != snp.pNode) {
					//key存在于枝干结点上
					//到上一层去更新枝干层
                    unsigned int index = 0;

                    Tree now_parent = snp.pNode->parent;
                    bool if_found_inBranch = now_parent->findKey(key, index);
                    while (!if_found_inBranch) {
                        if(now_parent->parent)
                            now_parent = now_parent->parent;
                        else
                            break;
                        if_found_inBranch = now_parent->findKey(key,index);
                    }

                    now_parent -> keys[index] = snp.pNode->keys[1];

                    snp.pNode->deleteKeyByIndex(snp.index);
                    keyNum--;
                    return adjustAfterDelete(snp.pNode);

                } else { //同时必然存在于叶结点
                    snp.pNode->deleteKeyByIndex(snp.index);
                    keyNum--;
                    return adjustAfterDelete(snp.pNode);
                }
            }
        }
    }

    return false;
}

//用于删除某key后可能出现结点空情况，对整棵树进行调整
template <class T>
bool BPlusTree<T>::adjustAfterDelete(Tree pNode)
{
    unsigned int minmumKeyNum = (degree - 1) / 2;
	//三种不需要调整的情况
    if (((pNode->isLeaf) && (pNode->num >= minmumKeyNum)) ||
	    ((degree != 3) && (!pNode->isLeaf) && (pNode->num >= minmumKeyNum - 1)) ||
		((degree == 3) && (!pNode->isLeaf) && (pNode->num < 0))) {
        return  true;
    }
    if (pNode->isRoot()) { //当前结点为根结点
        if (pNode->num > 0) //不需要调整
            return true;
        else { //正常需要调整
            if (root->isLeaf) { //将成为空树情况
                delete pNode;
                root = NULL;
                leafHead = NULL;
                level--;
                nodeNum--;
            }
            else { //根节点将成为左头部
                root = pNode -> childs[0];
                root -> parent = NULL;
                delete pNode;
                level--;
                nodeNum--;
            }
        }
    } else { //非根节点情况
        Tree parent = pNode->parent, brother = NULL;
        if (pNode->isLeaf) { //当前为叶节点
            unsigned int index = 0;
            parent->findKey(pNode->keys[0], index);

			//选择左兄弟
            if ((parent->childs[0] != pNode) && (index + 1 == parent->num)) {
                brother = parent->childs[index];
                if(brother->num > minmumKeyNum) {
                    for (unsigned int i = pNode->num; i > 0; i--) {
                        pNode->keys[i] = pNode->keys[i-1];
                        pNode->vals[i] = pNode->vals[i-1];
                    }
                    pNode->keys[0] = brother->keys[brother->num-1];
                    pNode->vals[0] = brother->vals[brother->num-1];
                    brother->deleteKeyByIndex(brother->num-1);

                    pNode->num++;
                    parent->keys[index] = pNode->keys[0];
                    return true;

                } else {
                    parent->deleteKeyByIndex(index);

                    for (int i = 0; i < pNode->num; i++) {
                        brother->keys[i+brother->num] = pNode->keys[i];
                        brother->vals[i+brother->num] = pNode->vals[i];
                    }
                    brother->num += pNode->num;
                    brother->nextLeafNode = pNode->nextLeafNode;

                    delete pNode;
                    nodeNum--;

                    return adjustAfterDelete(parent);
                }

            } else {
                if(parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if(brother->num > minmumKeyNum) {
                    pNode->keys[pNode->num] = brother->keys[0];
                    pNode->vals[pNode->num] = brother->vals[0];
                    pNode->num++;
                    brother->deleteKeyByIndex(0);
                    if(parent->childs[0] == pNode)
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];
                    return true;

                } else {
                    for (int i = 0; i < brother->num; i++) {
                        pNode->keys[pNode->num+i] = brother->keys[i];
                        pNode->vals[pNode->num+i] = brother->vals[i];
                    }
                    if (pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);
                    pNode->num += brother->num;
                    pNode->nextLeafNode = brother->nextLeafNode;
                    delete brother;
                    nodeNum--;

                    return adjustAfterDelete(parent);
                }
            }

        } else { //枝干节点情况
            unsigned int index = 0;
            parent->findKey(pNode->childs[0]->keys[0], index);
            if ((parent->childs[0] != pNode) && (index + 1 == parent->num)) {
                brother = parent->childs[index];
                if (brother->num > minmumKeyNum - 1) {
                    pNode->childs[pNode->num+1] = pNode->childs[pNode->num];
                    for (unsigned int i = pNode->num; i > 0; i--) {
                        pNode->childs[i] = pNode->childs[i-1];
                        pNode->keys[i] = pNode->keys[i-1];
                    }
                    pNode->childs[0] = brother->childs[brother->num];
                    pNode->keys[0] = parent->keys[index];
                    pNode->num++;

                    parent->keys[index]= brother->keys[brother->num-1];

                    if (brother->childs[brother->num])
                        brother->childs[brother->num]->parent = pNode;
                    brother->deleteKeyByIndex(brother->num-1);

                    return true;

                } else {
                    brother->keys[brother->num] = parent->keys[index];
                    parent->deleteKeyByIndex(index);
                    brother->num++;

                    for (int i = 0; i < pNode->num; i++) {
                        brother->childs[brother->num+i] = pNode->childs[i];
                        brother->keys[brother->num+i] = pNode->keys[i];
                        brother->childs[brother->num+i]-> parent= brother;
                    }
                    brother->childs[brother->num+pNode->num] = pNode->childs[pNode->num];
                    brother->childs[brother->num+pNode->num]->parent = brother;

                    brother->num += pNode->num;

                    delete pNode;
                    nodeNum --;

                    return adjustAfterDelete(parent);
                }

            } else {
                if (parent->childs[0] == pNode)
                    brother = parent->childs[1];
                else
                    brother = parent->childs[index+2];
                if (brother->num > minmumKeyNum - 1) {

                    pNode->childs[pNode->num+1] = brother->childs[0];
                    pNode->keys[pNode->num] = brother->keys[0];
                    pNode->childs[pNode->num+1]->parent = pNode;
                    pNode->num++;

                    if (pNode == parent->childs[0])
                        parent->keys[0] = brother->keys[0];
                    else
                        parent->keys[index+1] = brother->keys[0];

                    brother->childs[0] = brother->childs[1];
                    brother->deleteKeyByIndex(0);

                    return true;
                } else {

                    pNode->keys[pNode->num] = parent->keys[index];

                    if(pNode == parent->childs[0])
                        parent->deleteKeyByIndex(0);
                    else
                        parent->deleteKeyByIndex(index+1);

                    pNode->num++;

                    for (int i = 0; i < brother->num; i++) {
                        pNode->childs[pNode->num+i] = brother->childs[i];
                        pNode->keys[pNode->num+i] = brother->keys[i];
                        pNode->childs[pNode->num+i]->parent = pNode;
                    }
                    pNode->childs[pNode->num+brother->num] = brother->childs[brother->num];
                    pNode->childs[pNode->num+brother->num]->parent = pNode;

                    pNode->num += brother->num;

                    delete brother;
                    nodeNum--;

                    return adjustAfterDelete(parent);
                }

            }

        }

    }

    return false;
}

template <class T>
void BPlusTree<T>::dropTree(Tree node)
{
	//空树
    if (!node)
		return;
	//非叶节点
    if (!node->isLeaf) {
        for(unsigned int i = 0; i <= node->num; i++) {
            dropTree(node->childs[i]);
            node->childs[i] = NULL;
        }
    }
    delete node;
    nodeNum--;
    return;
}

template <class T>
void BPlusTree<T>::searchRange(T& key1, T& key2, vector<int>& vals, int flag)
{
	//空树
	if (!root)
		return;

	if (flag == 2) {
		searchNodeParse snp1;
		findToLeaf(root, key1, snp1);

		bool finished = false;
		Tree pNode = snp1.pNode;
		unsigned int index = snp1.index;

		do {
			finished = pNode->findRange2(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else if (flag == 1) {
		searchNodeParse snp2;
		findToLeaf(root, key2, snp2);

		bool finished = false;
		Tree pNode = snp2.pNode;
		unsigned int index = snp2.index;

		do {
			finished = pNode->findRange2(index, vals);
			index = 0;
			if (pNode->nextLeafNode == NULL)
				break;
			else
				pNode = pNode->nextLeaf();
		} while (!finished);
	} else {
		searchNodeParse snp1, snp2;
	    findToLeaf(root, key1, snp1);
		findToLeaf(root, key2, snp2);
		bool finished = false;
		unsigned int index;

		if (key1 <= key2) {
			Tree pNode = snp1.pNode;
			index = snp1.index;
			do {
				finished = pNode->findRange(index, key2, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		} else {
			Tree pNode = snp2.pNode;
			index = snp2.index;
			do {
				finished = pNode->findRange(index, key1, vals);
				index = 0;
				if (pNode->nextLeafNode == NULL)
					break;
				else
					pNode = pNode->nextLeaf();
			} while (!finished);
		}
	}
	//}

	sort(vals.begin(),vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
	return;
}

//获取文件大小
template <class T>
void BPlusTree<T>::getFile(string fname) {
    FILE* f = fopen(fname.c_str() , "r");
    if (f == NULL) {
        f = fopen(fname.c_str(), "w+");
        fclose(f);
        f = fopen(fname.c_str() , "r");
    }
    fclose(f);
    return;
}

template <class T>
int BPlusTree<T>::getBlockNum(string tableName)
{
    char* p;
    int block_num = -1;
    do {
        p = buffer_manager.getPage(tableName , block_num + 1);
        block_num++;
    } while(p[0] != '\0');
    return block_num;
}

template <class T>
void BPlusTree<T>::readFromDiskAll()
{
    string fname = "./database/index/" + fileName;
    //string fname = fileName;
    getFile(fname);
    int block_num = getBlockNum(fname);

	if (block_num <= 0)
        block_num = 1;

	for (int i = 0; i < block_num; i++) {
        //获取当前块的句柄
        char* p = buffer_manager.getPage(fname, i);
        //char* t = p;
        //遍历块中所有记录

		readFromDisk(p, p+PAGESIZE);
    }
}


template <class T>
void BPlusTree<T>::readFromDisk(char* p, char* end)
{
    T key;
    int value;
    
    for (int i = 0; i < PAGESIZE; i++)
        if (p[i] != '#')
            return;
        else {
            i += 2;
            char tmp[100];
            int j;
            
            for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
                tmp[j++] = p[i];
            tmp[j] = '\0';
            string s(tmp);
            stringstream stream(s);
            stream >> key;
            
            memset(tmp, 0, sizeof(tmp));
            
            i++;
            for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
                tmp[j++] = p[i];
            tmp[j] = '\0';
            string s1(tmp);
            stringstream stream1(s1);
            stream1 >> value;
            
            insertKey(key, value);
        }
}


template <class T>
void BPlusTree<T>::writtenBackToDiskAll()
{
    string fname = "./database/index/" + fileName;
    //string fname = fileName;
    getFile(fname);
	int block_num = getBlockNum(fname);

    Tree ntmp = leafHead;
	int i, j;
    
    for (j = 0, i = 0; ntmp != NULL; j++) {
		char* p = buffer_manager.getPage(fname, j);
        int offset = 0;
        
		memset(p, 0, PAGESIZE);
        
        for (i = 0; i < ntmp->num; i++) {
            p[offset++] = '#';
            p[offset++] = ' ';
            
            copyString(p, offset, ntmp->keys[i]);
            p[offset++] = ' ';
            copyString(p, offset, ntmp->vals[i]);
            p[offset++] = ' ';
		}
        
        p[offset] = '\0';

		int page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);
        
        ntmp = ntmp->nextLeafNode;
    }

    while (j < block_num) {
		char* p = buffer_manager.getPage(fname, j);
		memset(p, 0, PAGESIZE);

		int page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);

        j++;
    }

	return;

}


template <class T>
void BPlusTree<T>::printleaf()//输出所有叶节点
{
    Tree p = leafHead;
    while (p != NULL) {
        p->printl();
        p = p->nextLeaf();
    }
    
    return;
}


template <typename T>//用来将data拷贝为字符串
void copyString(char* p , int& offset , T data) {
    stringstream stream;
    stream << data;
    string s1 = stream.str();
    for (int i = 0;i < s1.length();i++,offset++)
        p[offset] = s1[i];
}

