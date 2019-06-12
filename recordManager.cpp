
//
//  recordManager.cpp
//  minisql
//
//  Created by yyy on 2019/6/5.
//  Copyright © 2019 yyy. All rights reserved.
//

#include "recordＭanager.hpp"
#define PATH "./database/data/"
void RecordManager::createTableFile(string tableName)//創建一個table文件
{
    string tableFile =PATH+tableName;
    FILE *file=fopen(tableFile.c_str(),"w");
    fclose(file);
}
void RecordManager::deleteTableFile(string tableName)//刪除一個table文件
{
    string tableFile=PATH+tableName;
    remove(tableFile.c_str());
}

void RecordManager::insertRecord(string tableName, Tuple &tuple)//insert one tuple to a table
{
    string tableFile=PATH+tableName;
    CatalogManager catalogManager;
    Attribute attr=catalogManager.getAttribute(tableName); //獲得該表的屬性
    vector<Data> data=tuple.getData();//得到要插入的data數組
    Table table=selectRecord(tableName);//得到要插入的表的資訊
    vector<Tuple> &tuples =table.getTuple();//得到一個該表的元組數組
    //判斷該表是否存在
    if(catalogManager.hasTable(tableName)==false)
       throw tableNotExist();
    //判斷插入屬性是否合法
    for(int i=0;i<data.size();i++)//遍歷data數組
    {
        if(data[i].type!=attr.type[i])
            throw tupleTypeConflict();
    }
    
    //判斷主鍵是否衝突
    if(attr.primarykey>=0)//如果有主鍵
    {
        for(int i=0;i<tuples.size();i++)//遍歷所有元組
        {
            if(tuples[i].ifDeleted()==true)//如果該元組還未被刪除
                continue;
            vector<Data> tupleData=tuples[i].getData();//獲得某元組的data數組
            switch(data[attr.primarykey].type)//判斷要插入的元組的主鍵
            {
                case -1://int
                    if(data[attr.primarykey].intData==tupleData[i].intData)
                        throw primarykeyConflict();
                    break;
                case 0://float
                    if(data[attr.primarykey].floatData==tupleData[i].floatData)
                        throw  primarykeyConflict();
                    break;
                default://string
                    if(data[attr.primarykey].stringData==tupleData[i].stringData)
                        throw primarykeyConflict();
            }
        }
    }
    
    //判斷unique屬性是否衝突
    for(int i=0;i<attr.numberOfAttribute;i++)//遍歷所有屬性
    {
        if(attr.unique[i]==true)// i 標註unique屬性的位置
        {
            for(int j=0;j<tuples.size();j++)//遍歷所有元組
            {
                if(tuples[j].ifDeleted()==true)//跳過已被刪除的元組
                    continue;
                vector<Data> tupleData=tuples[i].getData();
                switch(data[i].type)
                {
                    case -1:
                        if(data[i].intData==tupleData[j].intData)
                            throw uniqueConflict();
                        break;
                    case 0:
                        if(data[i].floatData==tupleData[j].floatData)
                            throw  uniqueConflict();
                        break;
                    default:
                        if(data[i].stringData==tupleData[j].stringData)
                            throw uniqueConflict();
                }
            }
        }
    }
    //異常檢測結束
    
    
    
    
    char*p;
    int blockNum=-1;//the total number of block occupied in data file
    do {
        p=bufferManager.getPage(tableFile,blockNum+1);
        blockNum++;
    }while(p[0]!='\0');//now,get the blocknumber
    if(blockNum<=0)
        blockNum=1;
    
    p=bufferManager.getPage(tableFile,blockNum-1);//point to the last block head
    int insertLoacation;
    for(insertLoacation=0;p[insertLoacation]!='\0'&&insertLoacation<PAGESIZE;insertLoacation++);//找到第一個可插入位
    int tupleLength=calculateTupleLength(tuple);//要插入元組的長度
    tupleLength+=data.size()+7;//插入文件裡的除了數據還有data.size()個空格 再加“ ”\0""\n"再加上有效長度計算4位
    int insertBlock;//紀錄tuple插入的塊號
    //判斷當前塊剩餘空間是否足夠插入這組tuple
    if(PAGESIZE-insertLoacation>=tupleLength)//足夠插入
    {
        insertBlock=blockNum-1;
        insertTupleToTable(p,insertLoacation,tupleLength,data);//insert
       // int pageId=BufferManage.getPageId(tableFile,blockNum-1);
      //BufferManager.modifyPage(pageId);
    }
    else{//新增一塊
        insertBlock=blockNum;
        char *p=bufferManager.getPage(tableFile,blockNum);
        insertTupleToTable(p, 0, tupleLength, data);
       // int pageId=BufferManage.getPageId(tableFile,blockNum);
      //BufferManager.modifyPage(pageId);
    }
    
    int pageId=bufferManager.getPageId(tableFile,insertBlock);
    bufferManager.modifyPage(pageId);

    
    //更新索引
    IndexManager indexmanager(tableName);
    for(int i =0; i<attr.numberOfAttribute;i++)
    {
        if(attr.hasIndex[i]==true)
        {
            string attrName=attr.name[i];
            string filePath="INDEX_FILE_"+attrName+"_"+tableName;
            vector<Data> indexdata=tuple.getData();
            indexmanager.insertIndex(filePath,indexdata[i],insertBlock);
        }
    }
}


int RecordManager::deleteRecord(string tableName)//刪除該表所有元組
{
    string tableFile=PATH+tableName;
    CatalogManager  catalogmanager;
    Attribute attr =catalogmanager.getAttribute(tableName);
    IndexManager indexmanager(tableName);
    int deleteRecordNumber=0;//刪除的元組數

    //檢查該表是否存在
    if(catalogmanager.hasTable(tableName)==false)
        throw tableNotExist();
    char *p;
    int blockNum=-1;//該表所佔block
    do {
        p=bufferManager.getPage(tableFile,blockNum+1);
        blockNum++;
    }while(p[0]!='\0');
    
    if(blockNum<=0)
        return 0;
    
    for(int i=0;i<blockNum;i++)//遍歷所有block
    {
        char *p=bufferManager.getPage(tableFile,i);
        char *r =p;
        while(*p!='\0'&&p<r+PAGESIZE)
        {
            
            Tuple tuple=readTuple(p, attr);//讀取一個元組
            
            //更新索引
            for(int j=0;j<attr.numberOfAttribute;j++)
            {
                if(attr.hasIndex[j]==true){//如果該屬性有索引
                    string attrName=attr.name[i];
                    string filePath="INDEX_FILE"+attrName+"_"+tableName;
                    vector<Data> indexdata=tuple.getData();
                    indexmanager.deleteIndexByKey(filePath,indexdata[j]);
                }
            }
            //
            int tupleLength=getTupleLength(p);
            p+=tupleLength;
            *(p-2)='1';
            deleteRecordNumber++;
        }
        int pageId=bufferManager.getPageId(tableFile,i);//獲得page
        bufferManager.modifyPage(pageId);
    }
    return deleteRecordNumber;
}

int RecordManager::deleteRecord(string tableName, string deleteAttr, Where where)
{
    string tableFile=PATH+tableName;
    CatalogManager  catalogmanager;
    Attribute attr =catalogmanager.getAttribute(tableName);
    //檢查該表是否存在
    if(catalogmanager.hasTable(tableName)==false)
        throw tableNotExist();
    
    int deleteIndex=-1;//目標屬性位置
    bool attrHasIndex=false;//有無索引
    for(int i=0;i<attr.numberOfAttribute;i++)
    {
        if(attr.name[i]==deleteAttr)
        {
            deleteIndex=i;
            if(attr.hasIndex[i]==true)
                attrHasIndex=true;
            break;
        }
    }
    if(deleteIndex==-1)
        throw attributeNotExist();
    else if(attr.type[deleteIndex]!=where.data.type)
        throw dataTypeConflict();
    //異常處理結束
    
    int deleteRecordNum=0;
    if(attrHasIndex==true&&where.relation!=notEqual)
    {
        vector<int>blockId;
        searchWithIndex(tableName,deleteAttr,where,blockId);//用索引要找符合的塊
        for(int i=0;i<blockId.size();i++)
        {            deleteRecordNum+=deleteInBlock(tableName,blockId[i],attr,deleteIndex,where);
        }
    }
    else//遍歷每個塊
    {
        char*p;
        int blockNum=-1;//the total number of block occupied in data file
        do {
            p=bufferManager.getPage(tableFile,blockNum+1);
            blockNum++;
        }while(p[0]!='\0');//now,get the blocknumber
        
        if(blockNum<=0)
            return 0;
        for(int i=0;i<blockNum;i++)//遍歷每個塊
        {
            deleteRecordNum+=deleteInBlock(tableName,i,attr,deleteIndex,where);
        }
    }
    return deleteRecordNum;
}
Table RecordManager::selectRecord(string tableName,string resultTableName)
{
    
    string tableFile = PATH + tableName;
    CatalogManager catalog_manager;
     Attribute attr = catalog_manager.getAttribute(tableName);
    
    if (!catalog_manager.hasTable(tableName)) {
        throw tableNotExist();
    }
    
    int blockNum=-1;//該表所佔block
    char *p;
    do {
        p=bufferManager.getPage(tableFile,blockNum+1);
        blockNum++;
    }while(p[0]!='\0');
    
    if (blockNum <= 0)
        blockNum = 1;
        
    Table table(resultTableName , attr);
    vector<Tuple>& tuples = table.getTuple();
    
    for (int i = 0;i < blockNum;i++) {

        char* p = bufferManager.getPage(tableFile , i);
        char* k = p;
        while (*p != '\0' && p < k + PAGESIZE) {
            Tuple tuple = readTuple(p , attr);//讀取一個tuple
            if (tuple.ifDeleted() == false)//如果該紀元組沒有被刪除
                tuples.push_back(tuple);
            int len = getTupleLength(p);
            p = p + len;//跳至下一個tuple
        }
    }
    return table;
}
Table RecordManager::selectRecord(string tableName,string selectAttr,Where where,string resultTableName)
{
   
    string tableFile = PATH + tableName;
    CatalogManager catalogManager;
    Attribute attr = catalogManager.getAttribute(tableName);
   
    if (!catalogManager.hasTable(tableName)) {
        throw tableNotExist();
    }
    int index = -1;//目標屬性編號
    bool hasIndex = false;//是否有索引
    for (int i = 0;i < attr.numberOfAttribute;i++) {
        if (attr.name[i] == selectAttr) {
            index = i;//獲得屬性編號
            if (attr.hasIndex[i] == true)
               hasIndex = true;
            break;
        }
    }

    if (index == -1) {
        throw attributeNotExist();
    }

    else if (attr.type[index] != where.data.type) {
        throw dataTypeConflict();
    }
    //異常處理完成
    
    Table table(resultTableName , attr);
    std::vector<Tuple>& tuples = table.getTuple();
    if (hasIndex == true && where.relation!= notEqual) {
        vector<int> blockId;//存放符合條件的blockid
        searchWithIndex(tableName , selectAttr , where , blockId);
        for (int i = 0; i < blockId.size();i++)
        {
            selectInBlock(tableName, blockId[i] , attr , index , where , tuples);
        }
    }
    else {
        char *p;
        int blockNum=-1;//該表所佔block
        do {
            p=bufferManager.getPage(tableFile,blockNum+1);
            blockNum++;
        }while(p[0]!='\0');
        if (blockNum <= 0)
            blockNum= 1;
        //遍历所有块
        for (int i = 0;i < blockNum;i++)
        {
            selectInBlock(tableName , i , attr , index , where , tuples);
        }
    }
    return table;
}

void RecordManager::createIndex(IndexManager & indexmanager, string tableName, string indexAttr)
{
    string tableFile= tableName;
    tableFile=PATH+ tableName;
    CatalogManager catalog_manager;
    Attribute attr = catalog_manager.getAttribute(tableName);
    
  
    if (!catalog_manager.hasTable(tableName)) {
        throw tableNotExist();
    }
 
    int index = -1;//目標屬性的編號
    for (int i = 0;i < attr.numberOfAttribute ;i++)
    {//尋找目標屬性的編號
        if (attr.name[i] == indexAttr)
        {
            index = i;
            break;
        }
    }
  
    if (index == -1) {
        throw attributeNotExist();//目標屬性不存在
    }
    //異常處理完成
    
    int blockNum=-1;//the total number of block occupied in data file
    char *p;
    do {
        p=bufferManager.getPage(tableFile,blockNum+1);
        blockNum++;
    }while(p[0]!='\0');//now,get the blocknumber
    
    if (blockNum <= 0)
      blockNum = 1;

     string filePath = "INDEX_FILE_" + indexAttr + "_" + tableName;
   
    for (int i = 0;i < blockNum;i++) {//遍歷所有塊
        char* p = bufferManager.getPage(tableFile , i);
        char* k = p;
        while (*p != '\0' && p < k + PAGESIZE) {
            Tuple tuple = readTuple(p , attr);
            if (tuple.ifDeleted() == false) {
               vector<Data> data = tuple.getData();
               indexmanager.insertIndex(filePath , data[index] , i);
            }
            int len = getTupleLength(p);
            p = p + len;//跳至下一個元組
        }
    }
}
void RecordManager::selectInBlock(string tableName, int blockId, Attribute attr, int index, Where where, vector<Tuple>& tuples)
{
    string tableFile=PATH+tableName;
    bool satisfied=false;
    char* p = bufferManager.getPage(tableName , blockId);
    char* k = p;
    
    while (*p != '\0' && p < k + PAGESIZE) {

        Tuple tuple = readTuple(p , attr);
        if (tuple.ifDeleted() == true) {
            int len = getTupleLength(p);
            p = p + len;
            continue;
        }
        std::vector<Data> data = tuple.getData();
        //根据属性类型选择
        switch(attr.type[index]) {
            case -1:{
                if (ifSatisfied(data[index].intData , where.data.intData , where.relation) == true) {
                    satisfied=true;
                }
            };break;
            case 0:{
                if (ifSatisfied(data[index].floatData, where.data.floatData , where.relation) == true) {
                   satisfied=true;
                }
            };break;
            default:{
                if (ifSatisfied(data[index].stringData , where.data.stringData , where.relation) == true) {
                   satisfied=true;
                }
            };
        }
        if(satisfied)
            tuples.push_back(tuple);
        int len = getTupleLength(p);
        p = p + len;
    }
}
void RecordManager::searchWithIndex(string tableName, string searchAttr, Where where, vector<int>& blockId)
{
    
        IndexManager indexManager(tableName);
        Data tempData;
        string filePath = "INDEX_FILE_" + searchAttr + "_" + tableName;
    
        if (where.relation == Less || where.relation == lessEqual)
        {
            tempData.floatData=-INF;
            tempData.intData=-INF;
            tempData.stringData="";
            switch (where.data.type) {
                case -1:
                    tempData.type=-1;
                    break;
                case 0:
                    tempData.type=0;
                    break;
                default:
                    tempData.type=1;
            }
            indexManager.searchRange(filePath ,tempData, where.data , blockId);
        }
        else if (where.relation == Greater || where.relation == greaterEqual)
        {
            tempData.floatData=INF;
            tempData.intData=INF;
            switch (where.data.type) {
                case -1:
                    tempData.type=-1;
                    break;
                case 0:
                    tempData.type=0;
                    break;
                default:
                    tempData.type=-2;
                    
            }
            indexManager.searchRange(filePath , where.data , tempData, blockId);
        }
    else
    {
        indexManager.searchRange(filePath , where.data , where.data , blockId);
    }
   
}


Tuple RecordManager::readTuple(const char *p, Attribute attr)
{
    Tuple tuple;
    p=p+5;
    for(int i=0;i<attr.numberOfAttribute;i++)
    {
        Data data;
        data.type=attr.type[i];
        char temp[50];
        int k;
        for(k=0;*p!=' ';p++)
            temp[k++]=*p;
        temp[k]='\0';
        p++;
        string dataString(temp);
        switch(data.type)
        {
            case -1:
                data.intData=stoi(dataString);
                break;
            case 0:
                data.floatData=stof(dataString);
                break;
            default:
                data.stringData=dataString;
        }
        tuple.addData(data);
     }
    if(*p=='1')
        tuple.Delete();
    return tuple;
    
}
void RecordManager::insertTupleToTable(char *p, int offset, int length, const vector<Data> &data)//插入元組到緩衝裡
{
    string slength=to_string(length);
    while(slength.length()<4)
        slength="0"+slength;//補齊4位
    for(int i=0;i<slength.length();i++)
        p[offset++]=slength[i];//先插入tuple的長度標示
    for(int j=0;j<data.size();j++)
    {
        p[offset]=' ';
        offset++;
        stringstream ss;
        switch (data[j].type) {
            case -1:
            {
                ss<<data[j].intData;
                for(int i=0;i<ss.str().length();i++)
                    p[offset++]=ss.str()[i];
                ss.str().clear();
            }
                break;
            case 0:
            {
                ss<<data[j].floatData;
                for(int i=0;i<ss.str().length();i++)
                    p[offset++]=ss.str()[i];
                ss.str().clear();
            }
                break;
            default:
                for(int i=0;i<data[j].stringData.length();i++)
                    p[offset++]=data[j].stringData[i];
        }
    }
    p[offset++]=' ';
    p[offset++]='\0';
    p[offset]='\n';
    
}
int RecordManager::getTupleLength(char *p)
{
    int tupleLength=0;
    char temp[5];
    int k=0;
    for(k=0;p[k]!=' ';k++)
        temp[k]=p[k];
    temp[k]='\0';
    string stringLength(temp);
    tupleLength=stoi(stringLength);
    return tupleLength;
    
}
int RecordManager::calculateTupleLength(Tuple &tuple)//計算一個元組的有效長度
{
    vector<Data>data=tuple.getData();
    int length=0;
    for(int i=0;i<data.size();i++)
    {
        stringstream ss;
        switch (data[i].type) {
            case -1://int
            {
                ss<<data[i].intData;
                length+=ss.str().length();
                ss.str().clear();
            }
                break;
            case 0:
            {
                ss<<data[i].floatData;
                length+=ss.str().length();
                ss.str().clear();
            }
                break;
            default:
                length+=data[i].stringData.length();
    
        }
    }
    return length;
}


int RecordManager::deleteInBlock(string tableName, int blockId, Attribute attr, int index, Where where)
{
    string tableFile=PATH+tableName;
    char* p = bufferManager.getPage(tableFile , blockId);
    char* k = p;
    int count = 0;//刪除元組數
    bool satisfied=false;//是否滿足條件
    while (*p != '\0' && p < k + PAGESIZE) {//遍歷塊中所有的紀錄
        Tuple tuple = readTuple(p , attr);
        std::vector<Data> d = tuple.getData();
        switch(attr.type[index]) {
            case -1:{//int
                if (ifSatisfied(d[index].intData , where.data.intData , where.relation) == true)
                    satisfied=true;
            };break;
            case 0:{//float
                if (ifSatisfied(d[index].floatData , where.data.floatData , where.relation) == true)
                    satisfied=true;
            };break;
            default:{
                if (isSatisfied(d[index].stringData , where.data.stringData , where.relation) == true)
                    satisfied=true;
            }
        }
        int tupleLength=getTupleLength(p);
        p+=tupleLength;
        if(satisfied)
        {
            *(p-2)='1';
        }
    }
    int pageId = bufferManager.getPageId(tableFile , blockId);
    bufferManager.modifyPage(page_id);
    return count;
}
