//
//  base.hpp
//  minisql
//
//  Created by 楊淯晴 on 2019/5/30.
//  Copyright © 2019 楊淯晴. All rights reserved.
//

#ifndef base_hpp
#define base_hpp
#include<vector>
#include <iostream>

using namespace std;
struct Attribute
{
    int number_of_attribute;
    short data_type[32];
    string name[32];
    bool unique[32];

};
struct Index
{
    string name[10];
    int number_of_index;
    short location[10];
};
struct Data
{
    int type;//-1 int ,0 float,
    int int_data;
    float float_data;
    string string_data;
};
typedef enum
{
    Less, //<
    Less_equal, //<=
    Equal,//=
    Greater,//>
    Greater_equal,//>=
    Not_equal//!=
}WHERE;
struct Where
{
    Data data;
    WHERE relation;
};
class Tuple
{
private:
    vector<Data> _data;
public:
    Tuple(){};
    Tuple(const Tuple& t);
    ~Tuple();
    vector<Data> getdata() const;
    void adddata(Data data);
    int get_tuple_size();
    void show();
};

class Table
{
private:
    string _tablename;
    vector<Tuple> _tuple;
    Index _index;
    short primarykey_location;
public:
    int number_of_block;
    int one_tuple_size();
    
public:
    Attribute _attr;
    Table(){};
    Table(string tablename,Attribute attr);
    Table(const Table & T);
    bool setindex(short index,string indexname);
    bool dropindex(string indexname);
    void setprimarykey(short p);
    void adddata(Tuple t);
    vector<Tuple>&get_Tuple();
    string get_table_name();
    Attribute get_attribute();
    Index get_index();
    void show();
};

#endif /* base_hpp */
