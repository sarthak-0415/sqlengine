#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>
#include <utility>
#include <fstream>

using namespace std;

typedef vector<int > vd;
typedef vector<long long int > vl;

#define sd(n) scanf("%d",&n)
#define sc(n) scanf("%c",&n)
#define sf(n) scanf("%f",&n)
#define pd(n) printf("%d\n",n)
#define pb push_back

#define LL long long 

#define ALL(c) c.begin(), c.end()
#define UB(s, e, x) upper_bound(s, e, x)
#define LB(s, e, x) lower_bound(s, e, x)
#define REV(s, e) reverse(s, e);
#define SZ(c) c.size()
#define SET(p) memset(p, -1, sizeof(p))
#define CLR(p) memset(p, 0, sizeof(p))
#define UPPER(s) transform(s.begin(),s.end(),s.begin(),std::ptr_fun<int, int>(std::toupper));
#define LOWER(s) transform(s.begin(),s.end(),s.begin(),std::ptr_fun<int, int>(std::tolower));
#define COUNT(a,val) count(a.begin(),a.end(),val)


vector<string> command;
vector<string> condition;
vector<string> columns;
vector<string> distinct;
vector<string> orderby;
vector<string> groupby;
vector<string> having;
vector<string> tables;


map < string , vector < string > > ColumnMap;
map < string , map < string, string > > TableMap;
map <string, vector<string> > JoinTablesToColumns;

#define FIXED_SIZE (10)
#define MAX_RECORD_SIZE (1024)
#define MAX_FILE_SIZE (1000006)
#define MAX_RECORD_COUNT (2)

int NUM_TABLES;
int PAGE_SIZE,NUM_PAGES;
string PATH;
int TABLE_COUNT=0;
set < string > TableSet;
// set of name of all tables

void readConfig(string configFilePath) {
	ColumnMap.clear();
	TABLE_COUNT=0;
	PATH="";
	PAGE_SIZE=0;NUM_PAGES=0;
	NUM_TABLES=0;

	string STRING;
	ifstream infile;
	infile.open (configFilePath.c_str());
	getline(infile,STRING); // Saves the line in STRING.

	int ptr=0;
	while(STRING[ptr]!=' ')
		ptr++;
	istringstream(STRING.substr(ptr)) >> PAGE_SIZE;

	getline(infile,STRING); // Saves the line in STRING.
	while(STRING[ptr]!=' ')
		ptr++;
	istringstream(STRING.substr(ptr)) >> NUM_PAGES;

	getline(infile,STRING); // Saves the line in STRING.
	while(STRING[ptr]!=' ')
		ptr++;
	PATH = STRING.substr(ptr);


	int flag=0;
	string tempname;
	string TempString = "";
    string TempType = "";
    map < string, string > TempMap;
    vector < string > TempVector;
	while(!infile.eof())	 // To get you all the lines.
	{
		getline(infile,STRING); // Saves the line in STRING.
		if(flag==1)
		{
			flag=0;

			TableSet.insert(STRING);
			tempname=STRING;

		}
		else if(STRING == "BEGIN")
		{
			TABLE_COUNT++;
			flag=1;
		}
		else if(STRING!="END" && STRING.length())// neither END nor null string
		{
			ptr=0;
			TempString = "";
			while(STRING[ptr]!=',' && ptr<STRING.size()){
                if(STRING[ptr]!=' ' && STRING[ptr]!=',' && STRING[ptr]!='\n')
                    TempString += STRING[ptr];
                ptr++;
            }
            TempType = "";
            while(ptr<STRING.size()){
                if(STRING[ptr]!=' ' && STRING[ptr]!=',' && STRING[ptr]!='\n')
                    TempType += STRING[ptr];
                ptr++;
            }
            if(ColumnMap.find(tempname)==ColumnMap.end())
                ColumnMap[tempname] = TempVector;
            ColumnMap[tempname].push_back(TempString);
            TableMap[tempname][TempString] = TempType;
        }
    }
    PATH = PATH.substr(1,PATH.size()-1);
    infile.close();
}

void seperate(string cmd)//init
{


    int cc=0;
    string token;
    while(cmd[cc]!='\0')
    {
        if(cmd[cc]==',' || cmd[cc]==' ' || cmd[cc]=='(' || cmd[cc]==')' || cmd[cc]==';')
        {
            if(token!="")
            {
                command.pb(token);

            }
            token = "";
            cc++;
            continue;
        }
        token += cmd[cc];
        cc++;
        
    }
    if(token!="")
        command.pb(token);

    /*for(int i=0;i<command.size();i++)
    {
        cout << command[i]<<endl<<endl;
    }*/
        return ;


    }
    void getDistinct()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "distinct")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    distinct.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<distinct.size();i++)
    //    cout << distinct[i]<<endl;
        return ;

    }
    void getCondition()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "where")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    condition.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<condition.size();i++)
    //    cout << condition[i]<<endl;
        return ;

    }
    void getColumn()
    {
        if(distinct.size()!=0)
        {
            columns = distinct;
            return ;
        }
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "select")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    columns.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<columns.size();i++)
    //    cout << columns[i]<<endl;
    //for (std::set<string>::iterator it=columns.begin(); it!=columns.end(); ++it)
    //    std::cout << *it <<endl;

        return ;

    }
    void getOrderby()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "orderby")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    orderby.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<orderby.size();i++)
    //    cout << orderby[i]<<endl;


        return ;

    }
    void getGroupby()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "groupby")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    groupby.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<groupby.size();i++)
    //    cout << groupby[i]<<endl;


        return ;

    }
    void getHaving()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "having")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" && command[i]!="orderby") 
                    having.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<having.size();i++)
    //    cout << having[i]<<endl;


        return ;

    }
    void getTables()
    {
        int index = 0;
        int i;
        for(i=0;i<command.size();i++)
        {
            if(command[i] == "from")
            {
                index = i;
                break;
            }
        }
        if(i!=command.size())
        {
            for(i=index+1;i<command.size();i++)
            {
                if(command[i]!="from" && command[i]!="where" && command[i]!="having" && command[i]!="groupby" 
                    && command[i]!="orderby" &&  command[i]!="inner" &&  command[i]!="outer") 
                    tables.pb(command[i]);
                else break;
            }
        }
    //for(i=0;i<tables.size();i++)
    //    cout << tables[i]<<endl;


        return ;

    }
    void PrintVector(vector<string> v)
    {
        if(v.size()==0)
            printf("NA");
        else
        {
            cout << v[0];

        }
        for(int i=1;i<v.size();i++)
        {
            cout << "," << v[i];
        }
        cout << endl;

    }


map<string,string> attr_values;//mapping attribute names to their values

void assign_values(vector<string> attributes, string record)
{

    int attr_no=0;
    string val="";
    for(int i=0;i<record.length();i++)
    {
        if(record[i] ==',')
        {
            attr_values[attributes[attr_no]] = val;
            attr_no++;
            val="";

        }
        else
        {
            if(record[i]!='\n' && record[i]!=' ' && record[i]!=',')
                val+=record[i];
        }
    }
    attr_values[attributes[attr_no]] = val;

}

void PrintVectorCond(vector<string> v)
{
    if(v.size()==0)
        printf("NA");
    else
    {
        cout << v[0];

    }
    for(int i=1;i<v.size();i++)
    {
        cout << " " << v[i];
    }
    cout << endl;

}

/*-----------------------------------------------------------------*/
string JoinAttribute;
void extractJointables()
{

    string tabname="";
    string colname="";
    int flag=0;
    for(int i=0;i<columns.size();i++)
    {
       // cout << "entered"<< endl;
        tabname="";
        colname="";
        flag=0;
        for(int j=0;j<columns[i].length();j++)
        {
            if(columns[i][j]=='.')
            {
                flag=1;
                continue;
            }
            if(!flag)
            {
                tabname+=columns[i][j];
            }
            else
            {
                colname+=columns[i][j];
            }
            
        }
        JoinTablesToColumns[tabname].pb(colname);


    }
    map < string , vector < string > > :: iterator it;
    for(it=JoinTablesToColumns.begin();it!=JoinTablesToColumns.end();it++){
        cout << (it->first) << endl;
        int sz = (it->second).size();
        for(int i=0;i<sz;i++)
            cout << (it->second)[i] << endl;
        printf("*******\n");
    }
}
/*FileRecord Modify(string record){
    FileRecord ans;
    vector <string> EmptyStringVector;
    ans.cols = EmptyStringVector;
    ans.orders = EmptyStringVector;
    string TempString="";
    for(int i=0;i<columns.size();i++)
        ans.cols.push_back(attr_values[columns[i]]);
    for(int i=0;i<orderby.size();i++)
        ans.orders.push_back(attr_values[orderby[i]]);
    return ans;
}*/

/*void ExecuteSelectJoin(){
    string TableName = tables[0];
    //cout << PATH + tables[0] + ".csv" <<endl;
    FILE *fp = fopen((PATH + tables[0] + ".csv").c_str(),"r");
    char BufferArray[ MAX_RECORD_SIZE ];
    vector < string > AttributeVector = ColumnMap[TableName];
    string Record;
    FileCount = 0;
    while(fgets(BufferArray,MAX_RECORD_SIZE,fp)!=NULL){
        Record = string ( BufferArray );
        if(ValidateRecord(Record,TableName,AttributeVector,condition)){
            fRecords.push_back(ModifyJoinRecord(Record));
            if(fRecords.size()>= MAX_RECORD_COUNT){
                WriteRecords();
                fRecords.clear();
            }
        }
    }
    if(fRecords.size()!=0){
        WriteRecords();
        fRecords.clear();
    }
    fclose(fp);
    TwoPhaseMerge();
}*/
/*-----------------------------------------------------------------*/
int main(int argc,char *argv[]){
	string CommandString;
	//string configFilePath(argv[1]);
	//readConfig(configFilePath);
    char BufferArray[MAX_RECORD_SIZE];
    int test;
    //scanf("%d",&test);
    while(gets(BufferArray)!=NULL){

      command.clear();
      condition.clear();
      columns.clear();
      tables.clear();
      having.clear();
      groupby.clear();
      distinct.clear();

		//getline(InputFile,CommandString);
      CommandString = string (BufferArray);
      seperate(CommandString);
      if(CommandString.size()<=1)
         continue;

        /*

    	map < string , vector < string > > :: iterator it;
    	for(it=ColumnMap.begin();it!=ColumnMap.end();it++){
    		cout << (it->first) << endl;
    		int sz = (it->second).size();
    		for(int i=0;i<sz;i++)
    			cout << (it->second)[i] << endl;
    		printf("*******\n");
    	}


        map < string , map < string,string > > :: iterator it;
        for(it=TableMap.begin();it!=TableMap.end();it++){
            map < string, string > :: iterator it2;
            for(it2=(it->second).begin();it2!=(it->second).end();it2++){
                cout << (it2->first) << " " << (it2->second) << endl;
            }
            printf("*********\n");
        }

        */
        /*-----------------------------------------------------------------*/
        int isJoinQuery = 0;

        string Jointable1="",Jointable2 = "";
        for(int i=0;i<command.size();i++)
        {
            if(command[i] == "join" || command[i]=="JOIN")
            {
                isJoinQuery=1;
                Jointable2=command[i+1];
                break;
            }
            
                
        }
        if(isJoinQuery)
        {
            for(int i=0;i<command.size();i++)
            {
                if(command[i]=="on" || command[i]=="ON")
                {
                    int index;
                    for(index=0;index<command[i+1].length();index++)
                    {
                        if(command[i+1][index]=='.')
                            break;
                    }
                    index++;
                    while(index<command[i+1].length())
                    {
                        JoinAttribute+=command[i+1][index];
                        index++;
                    }
                }
            }
            
        }
        /*-----------------------------------------------------------------*/
        if(command[0]=="select"){
          getTables();
          getDistinct();
          getCondition();
          getColumn();
          getOrderby();
          getGroupby();
          getHaving();


          /*cout << "Querytype:" << command[0]<<endl;
          cout <<"Tablename:";PrintVector(tables); 
          cout << "Columns:";PrintVector(columns);
          cout << "Distinct:";PrintVector(distinct);
          cout << "Condition:";PrintVectorCond(condition);
          cout << "Orderby:";PrintVector(orderby);
          cout << "Groupby:";PrintVector(groupby);
          cout << "Having:";PrintVector(having);*/
          /*-----------------------------------------------------------------*/
          if(isJoinQuery)
          {
            Jointable1=tables[0];
            cout << "JOIN TABLES are " << Jointable1 << " " << Jointable2 << endl;  
            extractJointables();
          }
          /*-----------------------------------------------------------------*/



      }
      else if(command[0]=="create"){
          ofstream myfile;
          myfile.open((PATH + command[2]+".data").c_str());
          int i;
          for(i=3;i<command.size()-2;i+=2)
             myfile << (command[i] + ":" + command[i+1]+",");
         myfile << (command[i] + ":" + command[i+1]);
         myfile.close();
         myfile.open((PATH + command[2]+".csv").c_str());
         myfile.close();

    		//myfile.open(configFilePath.c_str(),ios_base::app);
         myfile << endl;
         myfile << "BEGIN" ;
         myfile << endl;
         myfile << command[2] << endl;
         for(i=3;i<command.size()-2;i+=2)
             myfile << (command[i]+", "+command[i+1]) << endl;
         myfile << (command[i]+", "+command[i+1]) << endl;
         myfile << "END";
         myfile.close();
/*
    		cout << "Querytype:" << command[0]<<endl;
    		cout << "Tablename:" << command[2]<<endl;
    		cout << "Attributes:";
 */   		for(i=3;i<command.size()-2;i+=2)
         cout << (command[i]+" "+command[i+1]+",");
         cout << (command[i]+" "+command[i+1])<<endl;

			//readConfig(configFilePath);

     }
     else{
      printf("Query Invalid\n");
  }
}
return 0;
}
