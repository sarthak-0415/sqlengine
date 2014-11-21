/* ASSUMPTIONS 
1. run code using c++11 compiler  "g++ DBsystem.cpp -std=c++11"
2. LRU gets updated only when getRecord() is called
*/

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
#include <utility>
#include <cstring>
#include <climits>
#include <DBsystem>

using namespace std;

int call_join(string rname,string sname,string join_condition){

	string rn = "countries";
	string sn = "countries";

	int rnum = table_name_num[rn];
	int snum = table_name_num[sn];

	string query_r = "select * from countries where id<2;";
	string query_s = "select * from countries where id<2;";
	
	cout<< "for loop one \n";
	string result_from_pages_r = selectCommand(query_r);
	cout<< "for loop two \n";
	string result_from_pages_s = selectCommand(query_s);

	vector<string> r = order_results(result_from_pages_s, join_condition,rnum);
	vector<string> s = order_results(result_from_pages_r, join_condition,snum);
	
	vector<string> result_after_join = sort_merge_join(r,rname,s,sname,join_condition);
	return 0;

}

vector<string> sort_merge_join(vector<string> r,string rname,vector<string> s,string sname,string join_condition){

	int i=1,j=1,n=r.size(),m=s.size();

	int rnum = table_name_num(rname);
	int snum = table_name_num(sname);

	int r_join_att = find_orderby_att_no(join_condition,rnum);
	int s_join_att = find_orderby_att_no(join_condition,snum);
	
	while(i<=n && j<=m){
		vector<string> records_split_hash_r =split(r[i], '#');
		vector<string> records_split_r =split(records_split_hash_r[1], ',');

		vector<string> records_split_hash_s =split(s[i], '#');
		vector<string> records_split_s =split(records_split_hash_s[1], ',');

		do{
			if(records_split_r[rnum] > records_split_s[snum])
				j+j+1;
			else{
				if(records_split_s[snum] < records_split_r[rnum])
					i=i+1
				else{
					cout << "yes we did it"<<endl;
				}
			}
		}
	}
}
