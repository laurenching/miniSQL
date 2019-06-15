#ifndef _INTERPRETER_H_
#define	_INTERPRETER_H_ 1

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <fstream>
#include "catalogManager.h"
#include "api.h"
#include "base.h"

using namespace std;

class Interpreter{
public:
    Interpreter();
    
    //���ܣ���ȡһ���������Ϣ����������ĸ�ʽ�淶��
    //�쳣�����쳣
	void getQuery();
    //���ܣ����ַ������г����������ж�����ַ����Ĺ���
    //�쳣�������������Ĺؼ��ֲ����ڣ����׳��쳣input_format_error
    void EXEC();
    //���룺select last_name,first_name,birth,state from president where t1<10 and t2>20 ;
    //�����Success�����쳣
    //���ܣ�����ѡ�������֧�ֵ����Լ��
    //�쳣����ʽ�������׳�input_format_error
    //��������ڣ��׳�table_not_exist�쳣
    //������Բ����ڣ��׳�attribute_not_exist�쳣
    //���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣
    void EXEC_SELECT();
    //���룺drop table t1;
    //�����Success�����쳣
    //���ܣ�ɾ����t1
    //�쳣����ʽ�������׳�input_format_error
    //��������ڣ��׳�table_not_exist�쳣
    void EXEC_DROP_TABLE();
    //���룺drop index ID_index on t1;
    //�����Success�����쳣
    //���ܣ��ڱ�t1��ɾ��һ�����ֽ�ID_index������
    //�쳣����ʽ�������׳�input_format_error�쳣
    //��������ڣ��׳�table_not_exist�쳣
    //�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
    //�����Ӧ����û���������׳�index_not_exist�쳣
    void EXEC_DROP_INDEX();
    //���룺create table T1(
    //            NAME char(32),
    //            ID int unique,
    //            SCORE float,
    //            primary key (ID));
    //�����Success�����쳣
    //���ܣ������ݿ��в���һ�����Ԫ��Ϣ
    //�쳣����ʽ�������׳�input_format_error�쳣
    //��������ڣ��׳�table_not_exist�쳣
    void EXEC_CREATE_TABLE();
    //���룺create index ID_index on t1 (id);
    //�����Success�����쳣
    //���ܣ��ڱ��в���һ�����ֽ�ID_index�����������Ӧ����ΪID
    //�쳣����ʽ�������׳�input_format_error�쳣
    //��������ڣ��׳�table_not_exist�쳣
    //�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
    //�����Ӧ�����Ѿ������������׳�index_exist�쳣
    void EXEC_CREATE_INDEX();
    //���룺insert into T1 values('WuZhaoHui',0001,99.99);
    //�����Success�����쳣
    //���ܣ���T1�ڲ���ֵ����Ϣ
    //�쳣��
    void EXEC_INSERT();
    //���룺delete from MyClass where id=1;
    //     delete * from MyClass;
    //where��ֻ����һ����Ϣ
    //�����Success�����쳣
    //���ܣ���Myclass��ɾ��id=1��Ԫ��
    //�쳣����ʽ�������׳�input_format_error�쳣
    //��������ڣ��׳�table_not_exist�쳣
    //������Բ����ڣ��׳�attribute_not_exist�쳣
    //���Where�����е������������Ͳ�ƥ�䣬�׳�data_type_conflict�쳣
    void EXEC_DELETE();
    //���룺describe T1;
    //     ���� desc T1��
    //���ܣ������T1���������ԣ������Ļ�����Ϣ
    void EXEC_SHOW();
    //���룺exit;
    //���ܣ��˳����ݿ�
    void EXEC_EXIT();
    //���룺execfile �ļ�·��
    //���ܣ������ļ�·����ȡ�ļ���Ϣ�����������ݿ�Ĳ���
    void EXEC_FILE();
    
private:
    //�ַ����淶������
    void Normalize();
    //���������ַ����͹淶������ַ���
    string query;
    //���룺����Ӧ���ַ��Ŀ�ͷλ�ã����ô������ַ��Ľ�βλ��
    //��������λ������Ӧ�ĵ��ʵ��ַ���
    //���ܣ���query��ȡ��
    string getWord(int pos,int &end_pos);
    //���룺����Ҫת��Сд���ַ�����posλ��Ϊ����Ӧ�ĵ��ʵĿ�ʼ��λ��
    //�������posλ�õĵ��ʸĳ�Сд��������ĺ�������ַ���
    //���ܣ����ַ���str�е�posλ�ÿ�ͷ�ĵ���ת����Сд�����ڱ�׼��
    string getLower(string str,int pos);
    //���룺����Ӧ���ַ��Ŀ�ͷλ�ã����ô������ַ��Ľ�βλ��
    //��������λ������Ӧ�Ĺ�ϵ����
    //���ܣ���query��ȡ����ϵ����
    string getRelation(int pos,int &end_pos);
    //���룺����Ӧ���ַ��Ŀ�ͷλ�ã����ô������ַ��Ľ�βλ��
    //���������һ�����ͣ�-1��255��
    int getType(int pos,int &end_pos);
    //���������һ��������λ��
    int getBits(int num);
    //���������һ����������λ��������С�����4λ��
    int getBits(float num);
};

template <class Type>
Type stringToNum(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

#endif

