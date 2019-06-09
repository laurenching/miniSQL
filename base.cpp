//
//  base.cpp
//  minisql
//
//  Created by yyy on 2019/5/30.
//  Copyright © 2019 yyy. All rights reserved.
//

#include "base.hpp"
#include<iostream>
#include<vector>
using namespace std;
Tuple::Tuple(const Tuple &t)
{
    for(int i=0;i<t._data.size();i++)
    {
        _data.push_back(t._data[i]);
    }
}

int Tuple::getTupleSize()
{
    return (int)_data.size();
}

void Tuple::addData(Data data)
{
    _data.push_back(data);
}
bool Tuple::ifDelete()
{
    return _ifDelete;
}
void Tuple::Delete()
{
    _ifDelete=true;
}
void Tuple::show()
{
    for(int i=0;i<getTupleSize();i++)
    {
        if(_data[i].type==-1)//determine data type, -1 int ,0 float ,1~255 char
            cout<<_data[i].intData<<'\t';
        else if(_data[i].type==0)
            cout<<_data[i].floatData<<'\t';
        else
            cout<<_data[i].stringData<<'\t';
    }
    cout<<endl;
}

vector<Data> Tuple::getData()const
{
    return _data;
}


Table::Table(string tableName,Attribute attr)
{
    _tableName=tableName;
    _attr=attr;
    _index.numberOfIndex=0;
}
Table::Table(const Table&T)
{
    _tableName=T._tableName;
    _attr=T._attr;
    _index=T._index;
    for(int i=0;i<_tuple.size();i++)
    {
        _tuple.push_back(T._tuple[i]);
    }
}
bool Table::setIndex(short indexLocation, string indexName)
{
    bool ifSetIndex=false;
    
    for(int i=0;i<_index.numberOfIndex;i++)
    {
        if(indexName==_index.name[i])//the idex name has been used
        {
            cout<<"The index name has been used!(Setindex Error)"<<endl;
            return ifSetIndex;
        }
        else if(indexLocation==_index.location[i])//the index has already in the table
        {
            cout<<"The index has already in the table!(Setindex Error)"<<endl;
            return ifSetIndex;
        }
    }
    ifSetIndex=true;
    _index.location[_index.numberOfIndex]=indexLocation;
    _index.name[_index.numberOfIndex]=indexName;
    _index.numberOfIndex++;
    
    return ifSetIndex;
}
bool Table::dropIndex(string indexname)
{
    short temp;
    bool ifDropIndex=false;
    for(temp=0;temp<_index.numberOfIndex;temp++)
        if(indexname==_index.name[temp])//find the position of the index you want to delete
            break;
    if(temp==_index.numberOfIndex)//doesn't find the index you want to delete
    {
        cout<<"There is no such index in the table!(Drop Error)"<<endl;
        return ifDropIndex;
    }
    ifDropIndex=true;
    _index.location[temp]=_index.location[_index.numberOfIndex-1];
    _index.name[temp]=_index.name[_index.numberOfIndex-1];//the index you want to delete swaps with the last index
    _index.numberOfIndex--;
    return ifDropIndex;
}
void Table::show()
{
    for(int i=0;i<_attr.numberOfAttribute;i++)
        cout<<_attr.name[i]<<"\t";
    cout<<endl;
    for(int i=0;i<_tuple.size();i++)
        _tuple[i].show();
    
}
void Table::show(int range)
{
    for(int i=0;i<_attr.numberOfAttribute;i++)
        cout<<_attr.name[i]<<"\t";
    cout<<endl;
    for(int i=0;i<range&&i<_tuple.size();i++)
        _tuple[i].show();
    
}

string Table::getTableName()
{
    return _tableName;
}
Attribute Table::getAttribute()
{
    return _attr;
}
Index Table::getIndex()
{
    return _index;
}
vector<Tuple>& Table::getTuple()
{
    return _tuple;
}




