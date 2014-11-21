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

using namespace std;

vector<pair<string,string> > sort(){
	vector<pair<string,string> > result;

	string t1 = "sarthak";
	string t2 = "agarwal";
	result.push_back(make_pair(t1,t2));
	return result;
}

int main(){
	vector<pair<string,string> > result_after_join = sort();
	cout << result_after_join[0].first;
	cout << result_after_join[0].second;	
	return 0;

}