//
//  base.hpp
//  minisql
//
//  Created by yyy on 2019/5/30.
//  Copyright © 2019 yyy. All rights reserved.
//

#ifndef base_hpp
#define base_hpp
#include<vector>
#include <iostream>

using namespace std;
struct Attribute
{
    int numberOfAttribute;// the total number of the attributes
    short type[32];// -1 int 0 float 1~255 char
    string name[32];// name of the attribute
    bool unique[32];//the attribute is unique or not
    bool hasIndex[32];
    short primarykey;//primary location
    
};

struct Index
{
    string name[10];//name of indexs
    int numberOfIndex;//the total number of index
    short location[10];//index position
};//there can be no more than 10 index
struct Data
{
    int type;//-1 int ,0 float,1~255 char
    int intData;
    float floatData;
    string stringData;
};
typedef enum
{
    Less, //<
    lessEqual, //<=
    Equal,//=
    Greater,//>
    greaterEqual,//>=
    notEqual//!=
}WHERE;

struct Where
{
    Data data;
    WHERE relation;
};//store judgment statement


class Tuple//元組 只增加 不減少
{
private:
    vector<Data> _data;//store all the data in one tuple
    bool _ifDelete;//tuple has been deleted
public:
    Tuple()
    {
        _ifDelete=false;
    };
    Tuple(const Tuple& t);//copy ctor
    ~Tuple();
    vector<Data> getData() const;//return the data
    void addData(Data data);//add data to tuple
    bool ifDelete();
    void Delete();
    int getTupleSize();//return number of data in a tuple
    void show();//show all the data in a tuple
};

class Table
{
private:
    string _tableName;
    vector<Tuple> _tuple;//store all the tuple in a table
    Index _index;//index information
public:
    Attribute _attr;
    Table(){};
    Table(string tableName,Attribute attr);
    Table(const Table & T);
    bool setIndex(short index,string indexName);//set up the index
    bool dropIndex(string indexName);//delete index
    vector<Tuple>&getTuple();//return the tuple
    string getTableName();
    Attribute getAttribute();
    Index getIndex();
    void show();//show all the tuples in a table
    void show(int range);//show some tuples in a table
};
#endif /* base_hpp */
