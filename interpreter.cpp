#include "interpreter.h"

using namespace std;

Interpreter::Interpreter(){
}

//��query���и�ֵ
void Interpreter::getQuery(){
	string tmp;
    //�õ�һ�е������ַ��������һ���ַ�Ϊ�ֺ�ʱ����
    do{
        cout<<">>> ";
        getline(cin,tmp);
        query+=tmp;
        query+=' ';
    }while(tmp[tmp.length()-1]!=';');
    //�����һ����β��ʶ��
    query[query.length()-2]='\0';
    //����Normalize�����ַ����Ĺ淶��
    Normalize();
}

void Interpreter::Normalize(){
    //�����е�������ŵ�ǰ������һ���ո��Բ�ֶ���
    for(int pos=0;pos<query.length();pos++){
        if(query[pos]=='*'||query[pos]=='='||query[pos]==','||query[pos]=='('||query[pos]==')'||query[pos]=='<'||query[pos]=='>'){
            if(query[pos-1]!=' ')
                query.insert(pos++," ");
            if(query[pos+1]!=' ')
                query.insert(++pos," ");
        }
    }
    //�ڽ�β��һ���ո��Թ淶��
    query.insert(query.length()-2," ");
    //ɾ�������еĶ���ո�
    string::iterator it;
	int flag=0;
	for (it=query.begin();it<query.end();it++){
		if(flag==0&&(*it==' '||*it=='\t')){
			flag=1;
			continue;
		}
		if(flag==1&&(*it==' '||*it=='\t')){
			query.erase(it);
			if(it!=query.begin())
				it--;
			continue;
		}
		if(*it!=' '&&*it!='\t'){
			flag=0;
			continue;
		}
	}
    //������俪ʼ�пո񣬾�ɾ������ո��Թ淶��
    if(query[0]==' ')
        query.erase(query.begin());
    //��query�ĵ�һ����ȫ��ת��ΪСд������֮����ַ����Ľ���
    query=getLower(query, 0);
}

void Interpreter::EXEC(){
    try{
        //�����ַ����ĵ�һ���������������еĲ�������
        if(query.substr(0,6)=="select"){
            EXEC_SELECT();
        }
        //����drop�����������������Ҫ���н�һ���Ľ���
        else if(query.substr(0,4)=="drop"){
            query=getLower(query, 5);
            if(query.substr(5,5)=="table")
                EXEC_DROP_TABLE();
            else if(query.substr(5,5)=="index")
                EXEC_DROP_INDEX();
        }
        else if(query.substr(0,6)=="insert"){
            EXEC_INSERT();
        }
        //createҲ���������
        else if(query.substr(0,6)=="create"){
            query=getLower(query, 7);
            if(query.substr(7,5)=="table"){
                EXEC_CREATE_TABLE();
            }
            else if(query.substr(7,5)=="index"){
                EXEC_CREATE_INDEX();
            }
        }
        else if(query.substr(0,6)=="delete"){
            EXEC_DELETE();
        }
        //����describe�����ַ�ʽ������ʹ�û��߼�
        else if(query.substr(0,8)=="describe"||query.substr(0,4)=="desc"){
            EXEC_SHOW();
        }
        else if(query.substr(0,4)=="exit"&&query[5]=='\0'){
            EXEC_EXIT();
        }
        else if(query.substr(0,8)=="execfile"){
            EXEC_FILE();
        }
        //�������ָ����ܶ�Ӧ�����׳������ʽ����
        else{
            throw input_format_error();
        }
    }
    
    catch(table_exist error){
        cout<<">>> Error: Table has existed!"<<endl;
    }
    catch(table_not_exist error) {
        cout<<">>> Error: Table not exist!"<<endl;
    }
    catch(attribute_not_exist error) {
        cout<<">>> Error: Attribute not exist!"<<endl;
    }
    catch(index_exist error) {
        cout<<">>> Error: Index has existed!"<<endl;
    }
    catch(index_not_exist error) {
        cout<<">>> Error: Index not existed!"<<endl;
    }
    catch(tuple_type_conflict error) {
        cout<<">>> Error: Tuple type conflict!"<<endl;
    }
    catch(primary_key_conflict error) {
        cout<<">>> Error: Primary key conflict!"<<endl;
    }
    catch(data_type_conflict error) {
        cout<<">>> Error: data type conflict!"<<endl;
    }
    catch(index_full error) {
        cout<<">>> Error: Index full!"<<endl;
    }
    catch(unique_conflict error) {
        cout<<">>> Error: unique conflict!"<<endl;
    }
    catch(exit_command error){
        cout<<">>> Bye bye~"<<endl;
        exit(0);
    }
    catch(...){
        cout<<">>> Error: Input format error!"<<endl;
    }
}

void Interpreter::EXEC_CREATE_INDEX(){
    CatalogManager CM;
    API API;
    string index_name;
    string table_name;
    string attr_name;
    int check_index;
    index_name=getWord(13, check_index);
    check_index++;
    if(getLower(query, check_index).substr(check_index,2)!="on")
        throw 1;//��ʽ����
    table_name=getWord(check_index+3, check_index);
    if(!CM.hasTable(table_name))
        throw table_not_exist();//table not exist
    if(query[check_index+1]!='(')
        throw 1;//��ʽ����
    attr_name=getWord(check_index+3, check_index);
    if(query[check_index+1]!=')'||query[check_index+3]!='\0')
        throw 1;//��ʽ����
    API.createIndex(table_name, index_name, attr_name);
    cout<<">>> SUCCESS"<<endl;
}

void Interpreter::EXEC_DROP_INDEX(){
    API API;
    string table_name;
    string index_name;
    int check_index;
    //�õ�index������
    index_name=getWord(11, check_index);
    check_index++;
    //��ʽ��������
    if(getLower(query, check_index).substr(check_index,2)!="on")
        throw 1;//��ʽ����
    //�õ�table������
    table_name=getWord(check_index+3, check_index);
    //���table������֮���ж����ַ��������Ǹ�ʽ����
    if(query[check_index+1]!='\0')
        throw 1;//�������
    API.dropIndex(table_name, index_name);
    cout<<">>> SUCCESS"<<endl;
}

void Interpreter::EXEC_EXIT(){
    //�����Ҫ�˳���ֱ���׳�һ��exit command
    throw exit_command();
}

void Interpreter::EXEC_FILE(){
    int check_index=0;
    int start_index=0;
    string tmp_query;
    //�õ��ļ�·��
    string file_path=getWord(9, check_index);
    //���·�����ж�����ַ������Ǹ�ʽ����
    if(query[check_index+1]!='\0')
        throw 1;//��ʽ����
    string::iterator it;
    //�������ļ�������,����
    fstream fs(file_path);
    //�����ַ���������
    stringstream ss;
    //���ļ����е��ַ����뵽�ַ�������
    ss<<fs.rdbuf();
    //��ȡ���е��ַ���
    tmp_query=ss.str();
    //��ִ��һ��
    check_index=0;
    do{
        while(tmp_query[check_index]!='\n')
            check_index++;
        query=tmp_query.substr(start_index,check_index-start_index);
        check_index++;
        start_index=check_index;
        Normalize();
        EXEC();
    }while (tmp_query[check_index]!='\0');
}

void Interpreter::EXEC_SHOW(){
    CatalogManager CM;
    string table_name;
    int check_index;
    //�õ���һ�����ʵĽ�����λ��
    getWord(0, check_index);
    //�õ��������
    table_name=getWord(check_index+1, check_index);
    //���ֶ�����ַ�������ʽ����
    if(query[check_index+1]!='\0')
        throw 1;//�������
    CM.showTable(table_name);
}

void Interpreter::EXEC_DELETE(){
    API API;
    CatalogManager CM;
    Where where_delete;
    int check_index;
    string table_name;
    string attr_name;
    string relation;
    if(getLower(query, 7).substr(7,4)!="from")
        throw 1;
    table_name=getWord(12, check_index);
    if(!CM.hasTable(table_name))
        throw table_not_exist();
    
    //����ɾ��������Ϣ�����
    if(query[check_index+1]=='\0'){
        attr_name="";
        API.deleteRecord(table_name, attr_name, where_delete);
        cout<<">>> SUCCESS"<<endl;
        return;
    }
    
    if(getLower(query, check_index+1).substr(check_index+1,5)!="where")
        throw 1;//��ʽ����
    attr_name=getWord(check_index+7, check_index);
    if(!CM.hasAttribute(table_name, attr_name))
        throw attribute_not_exist();
    relation=getRelation(check_index+1, check_index);
    if(relation=="<")
        where_delete.relation_character=LESS;
    else if(relation=="< =")
        where_delete.relation_character=LESS_OR_EQUAL;
    else if(relation=="=")
        where_delete.relation_character=EQUAL;
    else if(relation=="> =")
        where_delete.relation_character=GREATER_OR_EQUAL;
    else if(relation==">")
        where_delete.relation_character=GREATER;
    else if(relation=="! =")
        where_delete.relation_character=NOT_EQUAL;
    else
        throw 1;//��ʽ����
    string value_delete=getWord(check_index+1, check_index);
    
    Attribute tmp_attr=CM.getAttribute(table_name);
    for(int i=0;i<tmp_attr.num;i++)
    {
        if(attr_name==tmp_attr.name[i]){
            where_delete.data.type=tmp_attr.type[i];
            switch (where_delete.data.type) {
                case -1:
                    try {
                        where_delete.data.datai=stringToNum<int>(value_delete);
                    } catch (...) {
                        throw data_type_conflict();//ת��ʧ��
                    }
                    break;
                case 0:
                    try {
                        where_delete.data.dataf=stringToNum<float>(value_delete);
                    } catch (...) {
                        throw data_type_conflict();//ת��ʧ��
                    }
                    break;
                default:
                    try {
                        if(!(value_delete[0]=='\''&&value_delete[value_delete.length()-1]=='\'')&&!(value_delete[0]=='"'&&value_delete[value_delete.length()-1]=='"'))
                            throw 1;//��ʽ����ȷ
                        where_delete.data.datas=value_delete.substr(1,value_delete.length()-2);
                    } catch (...) {
                        throw data_type_conflict();//ת��ʧ��
                    }
                    break;
            }
            break;
        }
    }
    API.deleteRecord(table_name, attr_name, where_delete);
    cout<<">>> SUCCESS"<<endl;
}

void Interpreter::EXEC_INSERT(){
    API API;
    CatalogManager CM;
    string table_name;
    int check_index;
    Tuple tuple_insert;
    Attribute attr_exist;
    if(getLower(query, 7).substr(7,4)!="into")
        throw input_format_error();
    table_name=getWord(12, check_index);
    if(getLower(query, check_index+1).substr(check_index+1,6)!="values")
        throw input_format_error();
    check_index+=8;
    if(query[check_index]!='(')
        throw input_format_error();
    if(!CM.hasTable(table_name))
        throw table_not_exist();
    attr_exist=CM.getAttribute(table_name);
    check_index--;
    int num_of_insert=0;
    //�������ڵ�����Ԫ�ؽ��б���
    while(query[check_index+1]!='\0'&&query[check_index+1]!=')'){
        if(num_of_insert>=attr_exist.num)
            throw 1;//��������ƥ��
        check_index+=3;
        string value_insert=getWord(check_index, check_index);
        Data insert_data;
        insert_data.type=attr_exist.type[num_of_insert];
        switch (attr_exist.type[num_of_insert]) {
            case -1:
                try {
                    insert_data.datai=stringToNum<int>(value_insert);
                } catch (...) {
                    throw data_type_conflict();//ת��ʧ��
                }
                break;
            case 0:
                try {
                    insert_data.dataf=stringToNum<float>(value_insert);
                } catch (...) {
                    throw data_type_conflict();//ת��ʧ��
                }
                break;
            default:
                try {
                    if(!(value_insert[0]=='\''&&value_insert[value_insert.length()-1]=='\'')&&!(value_insert[0]=='"'&&value_insert[value_insert.length()-1]=='"'))
                        throw input_format_error();//��ʽ����ȷ
                    if(value_insert.length()-1>attr_exist.type[num_of_insert])
                        throw input_format_error();//���ȳ�������
                    insert_data.datas=value_insert.substr(1,value_insert.length()-2);
                }
                catch(input_format_error error){
                    throw input_format_error();
                }
                catch (...) {
                    throw data_type_conflict();//ת��ʧ��
                }
                break;
        }
        tuple_insert.addData(insert_data);
        num_of_insert++;
    }
    if(query[check_index+1]=='\0')
        throw input_format_error();//��ʽ����
    if(num_of_insert!=attr_exist.num)
        throw input_format_error();//�������������ȷ
    API.insertRecord(table_name, tuple_insert);
    cout<<">>> SUCCESS"<<endl;
}

//����Ҫtable����ʾ
void Interpreter::EXEC_SELECT(){
    API API;
    CatalogManager CM;
    string table_name;
    vector<string> attr_name;
    vector<string> target_name;
    vector<Where> where_select;
    string tmp_target_name;
    string tmp_value;
    Where tmp_where;
    string relation;
    Table output_table;
    char op=0;
    int check_index;
    int flag=0;//�ж��Ƿ�Ϊselect *
    if(getWord(7, check_index)=="*")
    {
        flag=1;
        check_index++;
    }
    else{
        check_index=7;
        while(1){
            attr_name.push_back(getWord(check_index, check_index));
            if(query[++check_index]!=',')
                break;
            else
                check_index+=2;
        }
    }
    if(getLower(query, check_index).substr(check_index,4)!="from")
        throw input_format_error();//��ʽ����
    check_index+=5;
    table_name=getWord(check_index, check_index);
    if(!CM.hasTable(table_name))
        throw table_not_exist();
    Attribute tmp_attr=CM.getAttribute(table_name);
    if(!flag){
        for(int index=0;index<attr_name.size();index++){
            if(!CM.hasAttribute(table_name, attr_name[index]))
                throw attribute_not_exist();
        }
    }
    else{
        for(int index=0;index<tmp_attr.num;index++){
            attr_name.push_back(tmp_attr.name[index]);
        }
    }
    check_index++;
    if(query[check_index]=='\0')
        output_table=API.selectRecord(table_name, target_name, where_select,op);
    else{
        if(getLower(query, check_index).substr(check_index,5)!="where")
            throw input_format_error();//��ʽ����
        check_index+=6;
        while(1){
            tmp_target_name=getWord(check_index, check_index);
            if(!CM.hasAttribute(table_name, tmp_target_name))
                throw attribute_not_exist();
            target_name.push_back(tmp_target_name);
            relation=getRelation(check_index+1, check_index);
            if(relation=="<")
                tmp_where.relation_character=LESS;
            else if(relation=="< =")
                tmp_where.relation_character=LESS_OR_EQUAL;
            else if(relation=="=")
                tmp_where.relation_character=EQUAL;
            else if(relation=="> =")
                tmp_where.relation_character=GREATER_OR_EQUAL;
            else if(relation==">")
                tmp_where.relation_character=GREATER;
            else if(relation=="! =")
                tmp_where.relation_character=NOT_EQUAL;
            else
                throw input_format_error();//��ʽ����
            tmp_value=getWord(check_index+1, check_index);
            for(int i=0;i<tmp_attr.num;i++)
            {
                if(tmp_target_name==tmp_attr.name[i]){
                    tmp_where.data.type=tmp_attr.type[i];
                    switch (tmp_where.data.type) {
                        case -1:
                            try {
                                tmp_where.data.datai=stringToNum<int>(tmp_value);
                            } catch (...) {
                                throw data_type_conflict();//ת��ʧ��
                            }
                            break;
                        case 0:
                            try {
                                tmp_where.data.dataf=stringToNum<float>(tmp_value);
                            } catch (...) {
                                throw data_type_conflict();//ת��ʧ��
                            }
                            break;
                        default:
                            try {
                                if(!(tmp_value[0]!='\''&&tmp_value[tmp_value.length()-1]!='\'')&&!(tmp_value[0]!='"'&&tmp_value[tmp_value.length()-1]!='"'))
                                    throw input_format_error();//��ʽ����ȷ
                                tmp_where.data.datas=tmp_value.substr(1,tmp_value.length()-2);
                            }
                            catch(input_format_error error){
                                throw input_format_error();
                            }
                            catch (...) {
                                throw data_type_conflict();//ת��ʧ��
                            }
                    }
                    break;
                }
            }
            
            where_select.push_back(tmp_where);
            if(query[check_index+1]=='\0')
                break;
            else if(getLower(query, check_index+1).substr(check_index+1,3)=="and")//�����ϵ������and
                op=1;
            else if(getLower(query, check_index+1).substr(check_index+1,2)=="or")//�����ϵ������or
                op=0;
            else
                throw 1;
            getWord(check_index+1, check_index);
            check_index++;
        }
        
        output_table=API.selectRecord(table_name, target_name, where_select,op);
    }
    
    //�������������
    
    Attribute attr_record=output_table.attr_;
    int use[32]={0};
    if(attr_name.size()==0){
        for(int i=0;i<attr_record.num;i++)
            use[i]=i;
    }
    else{
        for(int i=0;i<attr_name.size();i++)
            for(int j=0;j<attr_record.num;j++){
                if(attr_record.name[j]==attr_name[i])
                {
                    use[i]=j;
                    break;
                }
            }
    }
    vector<Tuple> output_tuple=output_table.getTuple();
    int longest=-1;
    for(int index=0;index<attr_name.size();index++){
        if((int)attr_record.name[use[index]].length()>longest)
            longest=(int)attr_record.name[use[index]].length();
    }
    for(int index=0;index<attr_name.size();index++){
        int type=attr_record.type[use[index]];
        if(type==-1){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<getBits(output_tuple[i].getData()[use[index]].datai)){
                    longest=getBits(output_tuple[i].getData()[use[index]].datai);
                }
            }
        }
        if(type==0){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<getBits(output_tuple[i].getData()[use[index]].dataf)){
                    longest=getBits(output_tuple[i].getData()[use[index]].dataf);
                }
            }
        }
        if(type>0){
            for(int i=0;i<output_tuple.size();i++){
                if(longest<output_tuple[i].getData()[use[index]].datas.length()){
                    longest=(int)output_tuple[i].getData()[use[index]].datas.length();
                }
            }
        }
    }
    longest+=1;
    for(int index=0;index<attr_name.size();index++){
        if(index!=attr_name.size()-1){
            for(int i=0;i<(longest-attr_record.name[use[index]].length())/2;i++)
                printf(" ");
            printf("%s",attr_record.name[use[index]].c_str());
            for(int i=0;i<longest-(longest-attr_record.name[use[index]].length())/2-attr_record.name[use[index]].length();i++)
                printf(" ");
            printf("|");
        }
        else{
            for(int i=0;i<(longest-attr_record.name[use[index]].length())/2;i++)
                printf(" ");
            printf("%s",attr_record.name[use[index]].c_str());
            for(int i=0;i<longest-(longest-attr_record.name[use[index]].length())/2-attr_record.name[use[index]].length();i++)
                printf(" ");
            printf("\n");
        }
    }
    for(int index=0;index<attr_name.size()*(longest+1);index++){
        cout<<"-";
    }
    cout<<endl;
    for(int index=0;index<output_tuple.size();index++){
        for(int i=0;i<attr_name.size();i++)
        {
            switch (output_tuple[index].getData()[use[i]].type) {
                case -1:
                    if(i!=attr_name.size()-1){
                        int len=output_tuple[index].getData()[use[i]].datai;
                        len=getBits(len);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%d",output_tuple[index].getData()[use[i]].datai);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        int len=output_tuple[index].getData()[use[i]].datai;
                        len=getBits(len);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%d",output_tuple[index].getData()[use[i]].datai);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                case 0:
                    if(i!=attr_name.size()-1){
                        float num=output_tuple[index].getData()[use[i]].dataf;
                        int len=getBits(num);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%.2f",output_tuple[index].getData()[use[i]].dataf);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        float num=output_tuple[index].getData()[use[i]].dataf;
                        int len=getBits(num);
                        for(int i=0;i<(longest-len)/2;i++)
                            printf(" ");
                        printf("%.2f",output_tuple[index].getData()[use[i]].dataf);
                        for(int i=0;i<longest-(longest-len)/2-len;i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                default:
                    string tmp=output_tuple[index].getData()[use[i]].datas;
                    if(i!=attr_name.size()-1){
                        for(int i=0;i<(longest-tmp.length())/2;i++)
                            printf(" ");
                        printf("%s",tmp.c_str());
                        for(int i=0;i<longest-(longest-(int)tmp.length())/2-(int)tmp.length();i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        string tmp=output_tuple[index].getData()[i].datas;
                        for(int i=0;i<(longest-tmp.length())/2;i++)
                            printf(" ");
                        printf("%s",tmp.c_str());
                        for(int i=0;i<longest-(longest-(int)tmp.length())/2-(int)tmp.length();i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
            }
        }
    }
}

void Interpreter::EXEC_CREATE_TABLE(){
    //�������
    string table_name;
    //��λ����λ��
    int check_index;
    table_name=getWord(13,check_index);
    //���������ʼ��
    Index index_create;
    index_create.num=0;
    //��������
    Attribute attr_create;
    string attr_name;
    int primary=-1;
    int attr_num=0;
    while(1){
        check_index+=3;
        //����Ѿ�������string�ˣ����˳�
        if(query[check_index]=='\0'){
            if(query[check_index-2]=='\0')
                throw 1;
            else
                break;
        }
        //�õ�һ�����Ե�����
        attr_name=getWord(check_index, check_index);
        //�����������ǲ���primary
        string check_primary(attr_name);
        check_primary=getLower(check_primary, 0);
        if(check_primary=="primary"){
            int tmp_end=check_index;
            string check_key=getWord(tmp_end+1, tmp_end);
            //�ټ��������Ĺؼ����ǲ���key��������ǣ���primary������������
            if(check_key!="key"){
                attr_create.name[attr_num]=attr_name;
                break;
            }
            //�������Ե�primary
            else{
                check_index=tmp_end+3;
                string unique_name=getWord(check_index, check_index);
                int hasset=1;
                for(int find_name=0;find_name<attr_create.num;find_name++){
                    if(attr_create.name[find_name]==unique_name){
                        hasset=0;
                        primary=find_name;
                        attr_create.unique[find_name]=true;
                        check_index+=2;
                        break;
                    }
                }
                if(hasset)
                    throw 1;
                continue;
            }
        }
        //�������primary key����ֱ�Ӱ�����������ֲ����ȥ
        else
            attr_create.name[attr_num]=attr_name;
        check_index++;
        //�õ�һ�����Ե�����
        attr_create.type[attr_num]=getType(check_index, check_index);
        attr_create.unique[attr_num]=false;
        //�ж�֮����û��unique�ؼ���
        if(query[check_index+1]=='u'||query[check_index+1]=='U'){
            query=getLower(query, 0);
            //����У�����Ϊunique�������ͷΪu�����ǲ�Ϊunique���Ǿ���һ������
            if(getWord(check_index+1, check_index)=="unique"){
                attr_create.unique[attr_num]=true;
            }
            else
                throw 1;
        }
        //�������Ե�����
        attr_num++;
        attr_create.num=attr_num;
    }
    //����CatalogManager���������Ϣ�����ȥ
    API API;
    API.createTable(table_name, attr_create, primary, index_create);
    cout<<">>> SUCCESS"<<endl;
}



void Interpreter::EXEC_DROP_TABLE(){
    API API;
    string table_name;
    int check_index;
    //�õ�table������
    table_name=getWord(11, check_index);
    //���table������֮���ж����ַ��������Ǹ�ʽ����
    if(query[check_index+1]!='\0')
        throw 1;//�������
    API.dropTable(table_name);
    cout<<">>> SUCCESS"<<endl;
}

//�õ�һ��λ�õ���������
int Interpreter::getType(int pos, int &end_pos){
    string type=getWord(pos, end_pos);
    if(type=="int")
        return -1;
    else if(type=="float")
        return 0;
    else if(type=="char"){
        end_pos+=3;
        string length=getWord(end_pos, end_pos);
        end_pos+=2;
        return atoi(length.c_str())+1;
    }
    throw 1;
}

//ȡ��һ�����ʣ�ͬʱ��end_pos���µ����ʺ���Ǹ��ո��λ�ô�
string Interpreter::getWord(int pos,int &end_pos){
    string PartWord="";
    for(int pos1=pos;pos1<(int)query.length();pos1++){
        if(query[pos1]==' '||query[pos1]=='\0')
        {
            PartWord=query.substr(pos,pos1-pos);
            //cout<<PartWord<<endl;
            end_pos=pos1;
            return PartWord;
        }
    }
    return PartWord;
}

//�õ�һ���ַ���Сд��ʽ
string Interpreter::getLower(string str,int pos){
    for(int index=pos;;index++){
        if(str[index]==' '||str[index]=='\0')
            break;
        else if(str[index]>='A'&&str[index]<='Z')
            str[index]+=32;
    }
    return str;
}

//�õ�һ����ϵ���ŵ��ַ����������ϵ�����������ַ������ַ��л����һ���ո���"< ="
string Interpreter::getRelation(int pos,int &end_pos){
    string PartWord="";
    for(int pos1=pos;pos1<(int)query.length();pos1++){
        if(query[pos1]==' ')
            continue;
        if(query[pos1]!='<'&&query[pos1]!='>'&&query[pos1]!='='&&query[pos1]!='!')
        {
            PartWord=query.substr(pos,pos1-pos-1);
            //cout<<PartWord<<endl;
            end_pos=pos1-1;
            return PartWord;
        }
    }
    return PartWord;
}

//���ݳ����õ����ε����ֵĳ���
int Interpreter::getBits(int num){
    int bit=0;
    if(num==0)
        return 1;
    if(num<0){
        bit++;
        num=-num;
    }
    while(num!=0){
        num/=10;
        bit++;
    }
    return bit;
}

//���ݳ����õ�С�������ֵĳ���
int Interpreter::getBits(float num){
    int bit=0;
    if((int)num==0)
        return 4;
    if(num<0){
        bit++;
        num=-num;
    }
    int integer_part=num;
    while(integer_part!=0){
        bit++;
        integer_part/=10;
    }
    return bit+3;//Ϊ�˱���С����ĺ�λ
}

