#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <deque>
#include <queue>
#include <list>

using namespace std;

//split fucntion BEGIN
vector<string> split(const string &s, char delim) 
{
    vector<string> elems;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
//END


struct select_att{
	vector<string> column_list;
	bool distinct;
	vector<string> table_name;
	string where_clause;
	string groupby_clause;
	string having_clause;
	string orderby_clause;
};


class DBSystem {
	
	public:
		void selectCommand(string query) {
			
			select_att query_object;
			string temp_query;
			string delimiter;
			string token1;
			string token2;
			temp_query = query;
			
			transform(temp_query.begin(), temp_query.end(), temp_query.begin(),(int (*)(int))tolower);
					
			cout << "passed query " << query << "\n";
			
			//for orderby 
			delimiter = "orderby";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 8, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				query_object.orderby_clause = token1;
				temp_query = token2;
			}	
			
			//having
			delimiter = "having";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 7, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				query_object.having_clause = token1;
				temp_query = token2;
			}	
			
			//group_by
			delimiter = "groupby";
			
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 8, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				query_object.groupby_clause = token1;
				temp_query = token2;
			}

			//where
			delimiter = "where";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 6, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				query_object.where_clause = token1;
				temp_query = token2;
			}
			
			//from
			delimiter = "from";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 5, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				vector<string> tokens=split(token1, ',');
				for( vector<string>::iterator ii=tokens.begin(); ii!=tokens.end(); ++ii)
				{
				   query_object.table_name.push_back(*ii);
				}

				cout<<"\nfrom clause: ";
				for( vector<string>::iterator itr = query_object.table_name.begin() ; itr != query_object.table_name.end() ; ++itr)
					cout<<(*itr);
				temp_query = token2;
			}
			else
				cout<<"invalid query";
			
			//select
			delimiter = "select";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 7, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				vector<string> select_tokens=split(token1, ',');
				for( vector<string>::iterator ii=select_tokens.begin(); ii!=select_tokens.end(); ++ii)
				{
				   query_object.column_list.push_back(*ii);
				}

				cout<<"\nselect clause: ";
				for( vector<string>::iterator itr = query_object.column_list.begin() ; itr != query_object.column_list.end() ; ++itr)
					cout<<(*itr);

				temp_query = token2;
			}
			else
				cout<<"invalid query";	

			cout <<  "\nclause:: \n" << query_object.orderby_clause  << "\n" << query_object.having_clause << "\n" << query_object.groupby_clause <<"\n" << query_object.where_clause;
			cout << temp_query;
			/*
			vector<string> tokens=split(temp_query, 'from');
			query_object.table_name = split(tokens[1] , ',');
			strcpy(temp_query,tokens[0]);

			
			//select
			vector<string> tokens=split(temp_query, 'select');
			query_object.column_list = split(tokens[1] , ',');
			strcpy(temp_query,tokens[0]);
		
			
			//printing
			cout << "SELECT : " << query_object.column_list ;
			*/
			/*	
			Use any SQL parser to parse the input query. Perform all validations (table
			name, attributes, datatypes, operations). Print the query tokens as specified
			below.
			*/
		}
};

int main(){
	DBSystem object;

	int cases;
	cin >> cases;
	char q[200];
	cin.getline(q,200);
	for (int i = 0; i < cases; ++i)
	{
		cin.getline(q,200);
		cout << "\ninitial query:: "<< q <<"\n";
		object.selectCommand(q);
		
	}

	
	//cin.getline(q,200);
	//cout << "initial query:: "<< q <<"\n";
	//object.selectCommand(q);
	return 0;

}