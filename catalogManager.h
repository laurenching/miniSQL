#ifndef _CATALOG_MANAGER_H_
#define _CATALOG_MANAGER_H_ 1

#include <iostream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include "bufferManager.h"
#include "base.hpp"
#include "exception.h"
//#include "const.h"

#ifndef TABLE_MANAGER_PATH
#define TABLE_MANAGER_PATH "./database/catalog/catalog_file"
#endif

using namespace std; 

extern BufferManager bufferManager;

class CatalogManager{
public:
    //���룺���������Զ���������ţ���������
    //�����void
    //���ܣ���catalog�ļ��в���һ�����Ԫ��Ϣ
    //�쳣������Ѿ�����ͬ�����ı���ڣ����׳�table_exist�쳣
    void createTable(string table_name, Attribute attribute, int primary, Index index);
    //���룺����
    //�����void
    //���ܣ���catalog�ļ���ɾ��һ�����Ԫ��Ϣ
    //�쳣����������ڣ��׳�table_not_exist�쳣
    void dropTable(string table_name);
    //���룺����
    //�����bool
    //���ܣ����Ҷ�Ӧ���Ƿ���ڣ����ڷ���true�������ڷ���false
    //�쳣�����쳣
    bool hasTable(string table_name);
    //���룺������������
    //�����bool
    //���ܣ����Ҷ�Ӧ�����Ƿ���ĳһ���ԣ�����з���true�����û�з���false
    //�쳣����������ڣ��׳�table_not_exist�쳣
    bool hasAttribute(string table_name, string attr_name);
    //���룺����
    //��������Զ���
    //���ܣ���ȡһ���������
    //�쳣����������ڣ��׳�table_not_exist�쳣
    Attribute getAttribute(string table_name);
    //���룺��������������������
    //�����void
    //���ܣ���catalog�ļ��и��¶�Ӧ���������Ϣ����ָ�������Ͻ���һ��������
    //�쳣����������ڣ��׳�table_not_exist�쳣�������Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
    //�����Ӧ�����Ѿ������������׳�index_exist�쳣
    void createIndex(string table_name, string attr_name, string index_name);
   	//���룺������������
    //�������������Ӧ��������
    //���ܣ�ͨ����������λ������
    //�쳣����������ڣ��׳�table_not_exist�쳣�������Ӧ���������ڣ��׳�index_not_exist�쳣
    string IndextoAttr(string table_name, string index_name);
    //���룺������������
    //�����void
    //���ܣ�ɾ����Ӧ��Ķ�Ӧ�����ϵ�����
    //�쳣����������ڣ��׳�table_not_exist�쳣�������Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
    //�����Ӧ����û���������׳�index_not_exist�쳣
    void dropIndex(string table_name, string index_name);
    //���룺����
    //�����void
    //���ܣ���ʾ�����Ϣ
    //�쳣����������ڣ��׳�table_not_exist�쳣
    void showTable(string table_name);
private:
    //����ת�ַ�����bitΪ����λ��
    string num2str(int num,short bit);
    //�ַ���ת����
    int str2num(string str);
    //�õ����еı������
    string getTableName(string buffer, int start, int &rear);
    //���ر����ļ��е�λ��,���ؾ���λ�ã����ô����������ڵĿ���Ϣ
    int getTablePlace(string name, int& suitable_block);
    //���ظñ��index
    Index getIndex(string table_name);
    //��ȡ�ļ���С
    int getBlockNum(string table_name);
};

#endif
