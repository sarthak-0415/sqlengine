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

class DBSystem {
	
	public:
		
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
		object.createCommand(q);
		
	}
	
	
	//cin.getline(q,200);
	//cout << "initial query:: "<< q <<"\n";
	//object.selectCommand(q);
	return 0;

}