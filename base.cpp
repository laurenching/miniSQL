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

int Tuple::get_tuple_size()
{
    return (int)_data.size();
}

void Tuple::adddata(Data data)
{
    _data.push_back(data);
}
void Tuple::show()
{
    for(int i=0;i<get_tuple_size();i++)
    {
        if(_data[i].type==-1)
            cout<<_data[i].int_data<<'\t';
        else if(_data[i].type==0)
            cout<<_data[i].float_data<<'\t';
        else
            cout<<_data[i].string_data<<'\t';
    }
    cout<<endl;
}
vector<Data> Tuple::getdata()const
{
    return _data;
}
Table::Table(string tablename,Attribute attr)
{
    _tablename=tablename;
    _attr=attr;
    primarykey_location=-1;
    _index.number_of_index=0;
}
Table::Table(const Table&T)
{
    _tablename=T._tablename;
    _attr=T._attr;
    _index=T._index;
    for(int i=0;i<_tuple.size();i++)
    {
        _tuple.push_back(T._tuple[i]);
    }
}
bool Table::setindex(short index_location, string indexname)
{
    bool if_setindex=false;
    
    for(int i=0;i<_index.number_of_index;i++)
    {
        if(indexname==_index.name[i])
        {
            cout<<"The index name has been used!(Setindex Error)"<<endl;
            return if_setindex;
        }
        else if(index_location==_index.location[i])
        {
            cout<<"The index has already in the table!(Setindex Error)"<<endl;
            return if_setindex;
        }
    }
    if_setindex=true;
    _index.location[_index.number_of_index]=index_location;
    _index.name[_index.number_of_index]=indexname;
    _index.number_of_index++;
    
    return if_setindex;
}
bool Table::dropindex(string indexname)
{
    short temp;
    bool if_dropindex=false;
    for(temp=0;temp<_index.number_of_index;temp++)
        if(indexname==_index.name[temp])
            break;
    if(temp==_index.number_of_index){
        cout<<"There is no such index in the table!(Drop Error)"<<endl;
        return if_dropindex;
    }
    if_dropindex=true;
    _index.location[temp]=_index.location[_index.number_of_index-1];
    _index.name[temp]=_index.name[_index.number_of_index-1];
    _index.number_of_index--;
    return if_dropindex;
}
void Table::show()
{
    for(int i=0;i<_attr.number_of_attribute;i++)
        cout<<_attr.name[i]<<"\t";
    cout<<endl;
    for(int i=0;i<_tuple.size();i++)
      _tuple[i].show();
    
}
int Table::one_tuple_size()
{
    int size=0;
    for(int i=0;i<_attr.number_of_attribute;i++)
    {
        if(_attr.data_type[i]==-1)
            size+=sizeof(int);
        else if(_attr.data_type[i]==0)
            size+=sizeof(float);
        else
            size+=_attr.data_type[i];
    }
    return size;
}
void Table::adddata(Tuple t)
{
    _tuple.push_back(t);
}
string Table::get_table_name()
{
    return _tablename;
}
Attribute Table::get_attribute()
{
    return _attr;
}
Index Table::get_index()
{
    return _index;
}
vector<Tuple>& Table::get_Tuple()
{
    return _tuple;
}
void Table::setprimarykey(short p)
{
    primarykey_location=p;
}



