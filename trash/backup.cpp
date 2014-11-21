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
int global_table_number=0;
string configFilePath ="config.txt";

// structure for page
struct page_struct
{
	int page_no;
	int data_size;
	int table_no;
	string page_data;
	int start_id;
	int end_id;
	bool modified;
};
//end

// strcture for table
struct table_struct
{
	string attribute;
	string att_type;
};
//end

//struct for select_query
struct select_att
{
	vector<string> column_list;
	bool distinct;
	vector<string> table_name;
	string where_clause;
	string groupby_clause;
	string having_clause;
	string orderby_clause;
};
//end

//split fucntion BEGIN
vector<string> split(const string &s, char delim) 
{
    vector<string> elems;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) 
    {
        elems.push_back(item);
    }
    return elems;
}
//END

//double space to space
string check_spaces(string query)
{
	for(string::iterator i=query.begin() ; i!=query.end() ;++i)
	{
		if(*i == '\t')
		{
			*i = ' ';
		}
	}
	for(string::iterator i=query.begin() ; i!=query.end() ;++i)
	{
		if(*i == ' ')
		{
			if(*(i+1) == ' ')
			{
				query.erase(i);
				i--;
			}
		}
	}
	return query;
}
//END

//converts string to lower case
string myToLower(string query)
{

	for(string::iterator p=query.begin() ; p!=query.end() ;++p)
	{
		switch(*p)
		{
		  case 'A':*p='a'; break;
		  case 'B':*p='b'; break;
		  case 'C':*p='c'; break;
		  case 'D':*p='d'; break;
		  case 'E':*p='e'; break;
		  case 'F':*p='f'; break;
		  case 'G':*p='g'; break;
		  case 'H':*p='h'; break;
		  case 'I':*p='i'; break;
		  case 'J':*p='j'; break;
		  case 'K':*p='k'; break;
		  case 'L':*p='l'; break;
		  case 'M':*p='m'; break;
		  case 'N':*p='n'; break;
		  case 'O':*p='o'; break;
		  case 'P':*p='p'; break;
		  case 'Q':*p='q'; break;
		  case 'R':*p='r'; break;
		  case 'S':*p='s'; break;
		  case 'T':*p='t'; break;
		  case 'U':*p='u'; break;
		  case 'V':*p='v'; break;
		  case 'W':*p='w'; break;
		  case 'X':*p='x'; break;
		  case 'Y':*p='y'; break;
		  case 'Z':*p='z'; break;
		}
	}
	//cout <<endl << "to lower funct :: "<< query;
	return query;
}
//END

//removes all spaces from string
string remove_space(string str)
{
	string delimiter;
	delimiter = " ";
	while(str.find(delimiter)!= string::npos)
	{
		str = str.erase(str.find(delimiter));
	}
	return str;
}

typedef vector<int> pageNum_list; //vector of page number so that one table can have multiple pages


class sort_records_structre
{
	public:
		string record;
		string id;
};

bool sort_record_asc(const sort_records_structre& d1,const sort_records_structre& d2)
{
	return d1.id < d2.id;
}
bool sort_record_desc(const sort_records_structre& d1,const sort_records_structre& d2)
{
	return d1.id > d2.id;
}

class DBSystem 
{
	
	public:
		
		int PAGESIZE;
		int NUM_PAGES;
		string PATH_FOR_DATA;

		map <string, int> table_name_num;    		// table_name --> table_number

		table_struct att;
		vector<pair<int , table_struct> > table_num_att;		// table_no. --> table_attributes, att_type
		
		map <int, pageNum_list> tableNum_pageNum; 			// table_number --> page_no.
		
		
		map<int ,page_struct> page; //page implementation
		
		queue<int> LRUque;			//page_noo.
		map<int , page_struct> LRUdata;   //page_no. --> page_data
		int LRUsize = 5 ;    		//size of max elements in LRU //needs to be changed afterwards
		//deque<string> LRU;
		

	void readConfig(string configFilePath,int numpages) //changed
	{
		string line;

		ifstream file;
		file.open(configFilePath);

		if(file.is_open())
		{
			for (int i = 0; i < 3; ++i)				// for reading the structure of our DB 
			{
				getline(file,line);
				//istringstream iss(line);
				vector<string> tokens=split(line, ' ');
				
				switch(i){
					case 0:
						PAGESIZE = stoi(tokens[1]);
						//cout << line<<"\n"<<PAGESIZE<<"\n";
						break;
					case 1:
						LRUsize = stoi(tokens[1]);
						NUM_PAGES = numpages;		// to be changed		//changed
						//LRUsize=10;
						//NUM_PAGES =100;
						//cout << line<<"\n"<<NUM_PAGES<<"\n";
						break;
					case 2:
						PATH_FOR_DATA = tokens[1];
						//cout << line<<"\n"<<PATH_FOR_DATA<<"\n";
						break;
				}
			}

			while(getline(file,line))
			{						// for reading each table structure (att)
				if (line =="BEGIN")
				{
					getline(file,line);

					pageNum_list empty_page_list;

					
					table_name_num.insert(make_pair(line, global_table_number) ) ;
					tableNum_pageNum.insert(make_pair(global_table_number , empty_page_list));		// this is the problem for first page

					getline(file,line);
					while (line != "END"){
						//istringstream iss(line);
						vector<string> tokens=split(line, ',');
						//cout << line<<"\n";
						//cout << tokens[0] << "\t" <<tokens[1]<<"\n";

						att.attribute = tokens[0];
						att.att_type = tokens[1];

						//cout << att.attribute << "\t" <<att.att_type << "\n";
						table_num_att.push_back(make_pair(global_table_number , att));

						getline(file,line);
					}
					global_table_number ++;
				}
			}

			file.close();

			page_struct temp_page;

			temp_page.data_size = 0;
			temp_page.table_no=-1;
			temp_page.start_id = -1;
			temp_page.end_id =-1;
			temp_page.page_data.resize(PAGESIZE);
			temp_page.page_data = '\0';
			temp_page.modified =0;
			for(int i =0;i<numpages;i++)
			{   //changed
				temp_page.page_no =i;
				page.insert(make_pair(i,temp_page));
			}

			//LRUsize = NUM_PAGES/8 ;
			/*

			//to be deleted
			cout << "in readconfig \n";
			 for( map<string, int>::iterator ii=table_name_num.begin(); ii!=table_name_num.end(); ++ii)
			   {
			       cout << (*ii).first << ": " << (*ii).second  << endl;
			   }
		
			for(int i = 0; i < table_num_att.size(); i++)
			{
		  	   cout << table_num_att[i].first<< endl;
		  	   cout<< table_num_att[i].second.attribute <<" "<<table_num_att[i].second.att_type <<endl;
			}
			*/
		}
		else
			cout<<"unable to open config file";
	}

	void populatePageInfo() 
	{
		int page_curr = 0, assigned = 0;
		string line, path_name;
		
		ifstream file ;
		int current_page =0;
		for( map<string, int>::iterator ii=table_name_num.begin(); ii!=table_name_num.end(); ++ii)
		{
			int i=0;
			int temp_record_id =-1;

			path_name = PATH_FOR_DATA + (*ii).first+  ".csv";					
			file.open(path_name);

			if ( file.is_open() )
			{
				while(getline(file,line))
				{
					temp_record_id ++;    //  id should be record id not the line number
					int str_size = line.size();		//modify the sie to plus 3

					for(i=current_page;i<NUM_PAGES;i++)
					{
						if (str_size <= (PAGESIZE - page[i].data_size) )
						{
							if(page[i].data_size == 0){
								page[i].start_id = temp_record_id;
								page[i].end_id = temp_record_id;
								page[i].data_size = str_size +2 + to_string(temp_record_id).size();
								page[i].table_no = (*ii).second;
								page[i].page_data = to_string(temp_record_id) + "#" + line + "&";
								page[i].modified =1;

								//the tablenum_pagenum map is int->string. the existing string is read into p1. we modify the string to include the new page number and add the new string to the map
								pageNum_list p1;
								for( vector<int>::iterator iii=(tableNum_pageNum[page[i].table_no]).begin(); iii!=(tableNum_pageNum[page[i].table_no]).end(); ++iii)
									p1.push_back(*(iii));
							
								p1.push_back(i);
								tableNum_pageNum[page[i].table_no] = p1;
							}
							else
							{
								page[i].end_id = temp_record_id;
								page[i].data_size +=  str_size +2 + to_string(temp_record_id).size();
								page[i].page_data += to_string(temp_record_id) + "#" + line + "&";
								page[i].modified =1;
							}
							break;
						}
						else
							current_page++;
					}
				}
				file.close();
			}
			else
				cout<<"unable to open csv file: "<<path_name;

			current_page =i+1;
		}

		/*
		for( map<int, page_struct>::reverse_iterator ii=page.rbegin(); ii!=page.rend(); ++ii)
		{
		   cout << (*ii).first << ": " << (*ii).second.end_id << endl;
		}
		*/
		/*
		for(map<int,vector<int> >::iterator i=tableNum_pageNum.begin(); i!=tableNum_pageNum.end(); ++i){

			for( vector<int>::iterator itr = tableNum_pageNum[(*i).first].begin() ; itr != tableNum_pageNum[(*i).first].end() ; ++itr)
				printf("%d\n",(*itr) );
		}
		*/
	}

	int insertRecord(string tablename, string record)
	{	
		int hit_miss=1;

		int temp_table_no =-1;
		
		if(table_name_num.count(tablename) >0 )
		{
			temp_table_no= table_name_num[tablename];

			vector<int> temp_page_list = tableNum_pageNum[temp_table_no];

			int temp_page_size = temp_page_list.size();
			
			int last_page;

			if(temp_page_size > 0)
			{
				vector<int>::iterator it =  max_element(temp_page_list.begin() , temp_page_list.end()) ;
				last_page = *it;
			}
			else
			{
				last_page =-1;
			}	

			//printf("last page: %d\n",last_page );
			int str_size = record.size();
			int temp_record_id=-1;
			//vector<string> tokens=split(record, ',');
			//int temp_record_id = stoi(tokens[0]);	

			if (str_size <= (PAGESIZE - page[last_page].data_size) && (last_page >=0) )
			{
				//temp_page.start_id = page[i].start_id;
				temp_record_id = page[last_page].end_id +1;
				
				page[last_page].end_id = temp_record_id;
				page[last_page].data_size +=  str_size +2 + to_string(temp_record_id).size();
				page[last_page].page_data += to_string(temp_record_id) + "#" + record + "&";
				page[last_page].modified= 1;
			}
			else
			{
				if(last_page == -1)
					temp_record_id = 0;
				else
				{
					temp_record_id = ++page[last_page].end_id;
				}
					

				int new_page = ++last_page;

				while(page[new_page].data_size != 0)
					new_page++;
				
				if(page[new_page].data_size == 0  && new_page<NUM_PAGES)
				{
					page[new_page].start_id = temp_record_id;
					page[new_page].end_id = temp_record_id;
					page[new_page].data_size = str_size +2 + to_string(temp_record_id).size();
					page[new_page].table_no = temp_table_no;
					page[new_page].page_data = to_string(temp_record_id) + "#" + record + "&";
					page[new_page].modified= 1;

					pageNum_list p1;
					for( vector<int>::iterator iii=(tableNum_pageNum[temp_table_no]).begin(); iii!=(tableNum_pageNum[temp_table_no]).end(); ++iii)
						p1.push_back(*(iii));
				
					//p1.push_back(tableNum_pageNum[temp_table_no]);
					p1.push_back(new_page);

					//tableNum_pageNum.insert (make_pair (temp_table_no , p1));
					tableNum_pageNum[temp_table_no] = p1;
				}
				else
				{
					printf("no more size available\n");
					hit_miss =0;
				}
			}
		}
		
		
		else
		{
			printf("No such table exists!\n");
		}
		return hit_miss;
		/*
		for( map<int, page_struct>::reverse_iterator ii=page.rbegin(); ii!=page.rend(); ++ii)
		{
		   cout << (*ii).first << ": " << (*ii).second.end_id << endl;
		}
		*/

		/* Get the last page for the corresponding Table in main memory, if
		not already present.
		If the page has enough free space, then append the record to the
		page else, get new free page and add record to the new page.
		Do not flush modified page immediately.*/
	}

	void flushPages()
	{	
		for( map<int, page_struct>::iterator itr=page.begin(); itr!=page.end(); ++itr)
		{
			if((*itr).second.modified ==1)
			{
				(*itr).second.modified =0;
				ofstream file;
				string file_name = "db_" + to_string((*itr).second.page_no) +".txt";
				file.open(file_name);
				if(file.is_open())
				{
					file << (*itr).second.page_data;
					file.close();
				}
				else
					cout<<"unable to open db file"<<endl;	
			}			
		}

		/* Since primary and secondary memory are independent, no need to
		flush modified pages immediately, instead this function will be called
		to write modified pages to disk.
		Write modified pages in memory to disk.*/
	}
	
	int getRecord(string tablename, int recordId)
	{
		int hit_miss=1;
		int found =0;

		int temp_table_no = table_name_num[tablename];

		vector<int> temp_page_list = tableNum_pageNum[temp_table_no];
			
		list<int> temp_pages_of_table_not_in_lru;			//all pages associated to the table not in LRU

		int temp_start_id, temp_end_id;

		for( vector<int>::iterator itr = temp_page_list.begin(); itr != temp_page_list.end(); ++itr)
		{
			int temp_page_no = *itr;

			if(LRUdata.count(temp_page_no) > 0)
			{
				temp_start_id = LRUdata[(temp_page_no)].start_id ;
				temp_end_id = LRUdata[(temp_page_no)].end_id ;

				if((recordId >= temp_start_id) && (recordId<= temp_end_id))
				{
					hit_miss=1;
					printf("HIT\n");
					//return get_record_from_record_id(temp_page_no , recordId);
				}
			}

			else
			{
				temp_pages_of_table_not_in_lru.push_back(temp_page_no);
			}
			
		}

		if ( LRUque.size() == LRUsize )
		{
			int temp = LRUque.front();
			LRUque.pop();	
			LRUdata.erase(temp);
		}
		

		for( list<int>::iterator itr = temp_pages_of_table_not_in_lru.begin(); itr != temp_pages_of_table_not_in_lru.end(); ++itr)
		{
			int temp_page_no = *itr;

			temp_start_id = page[(temp_page_no)].start_id ;
			temp_end_id = page[(temp_page_no)].end_id ;
			
			if((recordId >= temp_start_id) && (recordId<= temp_end_id))
			{
				found =1;

				LRUque.push(temp_page_no);
				LRUdata.insert(make_pair(temp_page_no , page[temp_page_no]));

				hit_miss=0;
				printf("MISS<%d>\n", temp_page_no);
				//return get_record_from_record_id(temp_page_no , recordId);
			}
		}
		if(found ==0)
		{
			hit_miss=0;
			printf("MISS<-1>\n");
			return hit_miss;
		}

		/*	
		Get the corresponding record of the specified table.
		DO NOT perform I/O every time. Each time a request is received, if
		the page containing the record is already in memory, return that
		record else bring corresponding page in memory. You are supposed
		to implement LRU page replacement algorithm for the same. Print
		HIT if the page is in memory, else print
		MISS <pageNumber> where <pageNumber> is the page number of
		memory page which is to be replaced. (You can assume page
		numbers starting from 0. So, you have total 0 to <NUM_PAGES 1>
		pages.) 
		*/	
	}
	
	string get_record_from_record_id(int page_no , int recordId)
	{	
		string line = page[page_no].page_data;
		vector<string> records=split(line, '&');
		int record_no  = recordId - page[page_no].start_id;
		return records[record_no];
	}

	void createCommand(string query) 
	{
			string temp_table_name;
			string temp_query;
			string delimiter;
			string token1;
			string token2;
				
			temp_query = myToLower(query);
					
			cout << "passed query " << temp_query << "\n";
			
			//checking table 
			delimiter = "table";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + delimiter.length()+1, temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter) -1);
				//cout <<endl<<"token2::::"<<token2<<endl;
				
				//checking create
				if(token2 != "create")    //check for spaces
				{
					throw "1";
				}

				temp_query = token1;
			}	
			else
				throw "2";

			//table name
			delimiter = "(";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + delimiter.length(), temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				temp_table_name = token2;
				temp_query = token1;
			}
			else
				throw "3";

			//cout << temp_table_name<<endl;
			//remove the last 2 char of temp_query
			
			unsigned found = temp_query.find_last_of(")");
			temp_query.erase(found , temp_query.size()-1);
			//cout <<endl << "temp_query ==" <<endl<<temp_query;

			//now open the config file and start appending also update the table_no->att data structure
			fstream file;
			file.open(configFilePath, fstream::in | fstream::out | fstream::app);
			
			fstream data_file;
			data_file.open(temp_table_name+".data", fstream::in | fstream::out | fstream::app);
			
			fstream csv_file;
			csv_file.open(temp_table_name+".csv", fstream::in | fstream::out | fstream::app);
			
			if(file.is_open() && data_file.is_open() && csv_file.is_open() )
			{
				file << "\nBEGIN\n" << temp_table_name ;			
				
				pageNum_list empty_page_list;
				table_name_num.insert(make_pair(temp_table_name, global_table_number) ) ;
				tableNum_pageNum.insert(make_pair(global_table_number , empty_page_list));		
				
				vector<string> tokens=split(temp_query, ',');
				int temp_count =0;
				for( vector<string>::iterator ii=tokens.begin(); ii!=tokens.end(); ++ii)
				{
					
					cout<<endl<<*ii;
					temp_count++;
					vector<string> temp_tokens=split(*ii, ' ');

					for(vector<string>::iterator i=temp_tokens.begin(); i!=temp_tokens.end(); ++i)
					{
						if((*i).empty())
							temp_tokens.erase(i,i+1);
					}
					att.attribute = temp_tokens[0];				
					att.att_type = temp_tokens[1];
					//cout << endl << att.attribute << ", " << att.att_type;
					file << endl << att.attribute << ", " << att.att_type;
					data_file << att.attribute << ":" << att.att_type <<",";
					table_num_att.push_back(make_pair(global_table_number , att));
				}
			
	     		global_table_number++;

				file <<endl<<"END";
				file.close();
				data_file.close();
				csv_file.close();
			}
			else
				cout <<"\n error opening config while while creating\n";
	}
	
	// returns the sub_clause details in the format: (query_type,comparison_string,att_number)
	string find_att_no(string where_sub_clause, int table_no)
	{
		// =,>=,<=, !=,>,<, LIKE
		string attribute;
		string delimiter;
		int query_type;
		string comparison_string;
		int count=1, found_att=0;
		
		delimiter = "=";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =1;
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+1, where_sub_clause.length());
		}
		
		delimiter = ">=";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =2;	
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+2, where_sub_clause.length());			
		}
		
		delimiter = "<=";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =3;	
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+2, where_sub_clause.length());			
		}
		
		delimiter = "!=";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =4;	
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+2, where_sub_clause.length());			
		}
		
		delimiter = ">";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =5;	
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+1, where_sub_clause.length());			
		}
		
		delimiter = "<";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =6;
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+1, where_sub_clause.length());				
		}
		
		//add the case sensitive thing here for 'like' and 'LIKE'
		delimiter = "like";
		if(where_sub_clause.find(delimiter)!= string::npos)
		{
			attribute = where_sub_clause.substr(0, where_sub_clause.find(delimiter));
			query_type =7;	
			comparison_string = where_sub_clause.substr(where_sub_clause.find(delimiter)+4, where_sub_clause.length());			
		}

		attribute = remove_space(attribute);
		comparison_string = remove_space(comparison_string);
		
		// finding the attribute number
		for(vector<pair<int , table_struct> >::iterator found = table_num_att.begin(); found != table_num_att.end(); ++found)
		{
	   		if( found->first == table_no)
	   		{	   			
	   			string original_att = remove_space((found->second).attribute);
	   			if(original_att != attribute)
	   				{
	   					count++;
	   					//cout<<"\n:att search:"<<(found->second).attribute<<":count:"<<count;
	   				}	
	   			else
	   				{
	   					found_att =1;
	   					break;
	   				}		
	   		}	
		}
		
		if(found_att == 0)
		{
			cout<<"no such attribute like "<< attribute;
			return "error";
		}
		
		else
		{
			return (to_string(query_type) + "," + comparison_string + "," + to_string(count));
		}	
	}
	
	// returns the att_no of orderby clause
	int find_orderby_att_no(string orderby_clause, int table_no)
	{
		int count=1, found_att = 0;;

		// finding the attribute number
		for(vector<pair<int , table_struct> >::iterator found = table_num_att.begin(); found != table_num_att.end(); ++found)
		{
	   		if( found->first == table_no)
	   		{	   			
	   			string original_att = remove_space((found->second).attribute);
	   			if(original_att != orderby_clause)
	   				{
	   					count++;
	   					//cout<<"\n:att search:"<<(found->second).attribute<<":count:"<<count;
	   				}	
	   			else
	   				{
	   					found_att =1;
	   					break;
	   				}		
	   		}	
		}
		return count;
	}

	string order_results(string result, string orderby_clause, int table_no)
	{
		cout << "in order_results\n";
		string query_type, delimiter;
		vector<int> att_nos;
		vector<sort_records_structre> record_vector;
		string after_sorting_result;

		delimiter = " ";
		if(orderby_clause.find(delimiter)!=string::npos)
		{
			query_type = orderby_clause.substr(orderby_clause.find(delimiter) + 1, orderby_clause.length() );	//tells whether asc or desc
			orderby_clause = orderby_clause.substr(0, orderby_clause.find(delimiter));							//tells which clause to order on
			
			cout << "order_by_clause_name\t"<<orderby_clause;

			vector<string> tokens=split(orderby_clause, ',');
			for( vector<string>::iterator it = tokens.begin() ; it != tokens.end() ; ++it)
			{
				att_nos.push_back(find_orderby_att_no((*it),table_no));
			}

			for(vector<int>::iterator itr = att_nos.begin() ; itr != att_nos.end() ; itr++)
				cout << "\natrr \t"<<(*itr)<<endl;
			
			query_type = remove_space(query_type);
			query_type = myToLower(query_type);
			
			vector<string> records_split_amp =split(result, '&');	

			sort_records_structre data;
			
			//cout<<"beginnig to print what is needed for the eyes"<<endl;
			for(vector<string>::iterator record_itr = records_split_amp.begin() ; record_itr != records_split_amp.end() ; record_itr++)
			{
				//cout <<(*record_itr)<<endl;
				data.record = (*record_itr);

				vector<string> records_split_hash =split((*record_itr), '#');
				vector<string> records_split =split(records_split_hash[1], ',');
				
				int order_attribute = (*att_nos.begin()) -1;
				data.id = records_split[order_attribute];

				cout<< data.record<<endl<<data.id<<endl;

				record_vector.push_back(data);
			}
			
			
			cout<<"beginnig to print what is needed for the round of the clock"<<endl;
			
			for(vector<sort_records_structre>::iterator temp_itr = record_vector.begin() ; temp_itr != record_vector.end() ; temp_itr++)
			{
				cout<< (*temp_itr).record<<endl<<(*temp_itr).id<<endl;
			}
			

			query_type = remove_space(query_type);
			query_type = myToLower(query_type);

			if(query_type == "asc")
			{	
				cout <<"in asc"<<endl;
				std::sort(record_vector.begin() , record_vector.end() , sort_record_asc);
				//call merge_sort_inc()
				//result  = merge_sort_inc(tokens[0]);
			}
			else if(query_type == "desc")
			{
				cout <<"in desc"<<endl;
				std::sort(record_vector.begin() , record_vector.end() , sort_record_desc);
				//call merge_sort_desc()
				//result  = merge_sort_desc(tokens[0]);
			}
			else
			{
				cout<<"error";
				return 0;
			}	
			cout<<"beginnig to print what is needed for the eyes"<<endl;
			for(vector<sort_records_structre>::iterator temp_itr = record_vector.begin() ; temp_itr != record_vector.end() ; temp_itr++)
			{
				cout<< (*temp_itr).record<<endl<<(*temp_itr).id<<endl;
				after_sorting_result.append((*temp_itr).record);
				after_sorting_result.append("&");
			}	
		}	
		return after_sorting_result;
	}

	void selectCommand(string query)
	{
		string temp_query;
		string delimiter;
		string token1;
		string token2;
		string rest_query;
		int query_table_num;
		temp_query = query;
		select_att query_object;
		vector<string> temp_results; //stores results of various sub clauses in a vector
		string result; //stores result after computing the sub clauses
      
		temp_query = myToLower(temp_query);
		if(temp_query.find_last_of(')')!=string::npos)
		{
			unsigned found = temp_query.find_last_of(")");
			temp_query.erase(found , temp_query.size()-1);
		}	

		cout << "\npassed query :" << temp_query << "\n";
		
		string select = "select";
		do
		{
			
			if(temp_query.rfind(select)!=string::npos)
			{	
				token1 = temp_query.substr(temp_query.rfind(select), temp_query.length() -1);
				token2 = temp_query.substr(0, temp_query.rfind(select));
				
				temp_query = token1;
              	rest_query = token2;
			}
			else
				throw "1";
			
			//distinct
			delimiter = "distinct";
			if(temp_query.find(delimiter) != string::npos)
			{
				query_object.distinct = 1;
				int found_distinct = temp_query.find(delimiter);
				temp_query.replace(found_distinct,9,"");
				//temp_query.erase(found_distinct , found_distinct + 9);
			}	
			else
				query_object.distinct = 0;

			if(query_object.distinct == 1)
				cout<<"\ndistinct:1";
			else
				cout<<"\ndistinct:0";			
			
			
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
				token2 = temp_query.substr(0, temp_query.find(delimiter)-1);
				
				query_object.where_clause = token1;
				temp_query = token2;
			}

			//from
			delimiter = "from";
			if(temp_query.find(delimiter)!=string::npos)
			{
				token1 = temp_query.substr(temp_query.find(delimiter) + 5, temp_query.length()-2);
				token2 = temp_query.substr(0, temp_query.find(delimiter));
				
				vector<string> tokens=split(token1, ',');
				for( vector<string>::iterator ii=tokens.begin(); ii!=tokens.end(); ++ii)
				{
				   query_object.table_name.push_back(*ii);
				}

				for( vector<string>::iterator itr = query_object.table_name.begin() ; itr != query_object.table_name.end() ; ++itr)
				{
					map<string,int>::iterator found = table_name_num.find(*itr);
					if(found == table_name_num.end())
					{
						throw "2";
					}
					else
						query_table_num = found->second;
				}
				            
              /*   //TO PRINT THE TABLE INCLUDED
				cout<<"\nfrom clause: ";
				for( vector<string>::iterator itr = query_object.table_name.begin() ; itr != query_object.table_name.end() ; ++itr)
					cout<<(*itr)<<" ";
          		*/
				temp_query = token2;
			}
			else
				throw "3";

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
				
				//cout<<"\nquery_object.column_list[0]"<<query_object.column_list[0];
				//cout<<"\nquery_object.column_list.size()"<<query_object.column_list.size();

				if((query_object.column_list.size() == 1 && query_object.column_list[0] == "*"))
				{
					for( vector<string>::iterator ii=query_object.column_list.begin(); ii!=query_object.column_list.end(); ++ii)
					{
						while((*ii).find(" ")!=string::npos)
						{
							(*ii).erase((*ii).find(" "));
						}
					   int flag = 0;
					   for(vector<pair<int , table_struct> >::iterator found = table_num_att.begin(); found != table_num_att.end(); ++found)
						{
					   		if( found->first == query_table_num && (found->second).attribute == *ii)
					   		{
					   			flag=1;
					   		}	
						}
						if(flag==0)
							throw "4";   
					}
				}	

				cout<<"\nselect clause: ";
				for( vector<string>::iterator itr = query_object.column_list.begin() ; itr != query_object.column_list.end() ; ++itr)
					cout<<(*itr)<<" ";

				temp_query = token2;
			}
			else
				throw "5";

			cout << "\norderby_clause :"  << query_object.orderby_clause  << "\nhaving clause :" << query_object.having_clause << "\ngroupby clause :" << query_object.groupby_clause <<"\nwhere clause :" << query_object.where_clause;
			//cout << "\ntemp query :"<<temp_query<<"'"<<endl;
			
			if(rest_query != "")
				temp_query = rest_query;
			else
				break;
		
		}while(temp_query.rfind(select) != string::npos);
		
		/*	
		Use any SQL parser to parse the input query. Perform all validations (table
		name, attributes, datatypes, operations). Print the query tokens as specified
		below.
		*/
      	
      	//page list related to queried table
      	vector<int> temp_page_list = tableNum_pageNum[query_table_num];
      	
      	//seperating sub clauses in the where clause
      	string where_clause_string;
      	string subclause_string;
      	vector<string> where_sub_clause;
      	where_clause_string = query_object.where_clause;
      	
      	int flag_for_and=0;
      	int flag_for_or =0;
      
      	delimiter = " and ";
      	if(where_clause_string.find(delimiter)!=string::npos && flag_for_or ==0)
        {
			while(where_clause_string.find(delimiter)!=string::npos)
			{
	    		subclause_string = where_clause_string.substr(0, where_clause_string.find(delimiter));
	      		where_clause_string = where_clause_string.substr(where_clause_string.find(delimiter) + 5, where_clause_string.length() );
				where_sub_clause.push_back(subclause_string);
            }
			where_sub_clause.push_back(where_clause_string);
			flag_for_and ++;
        }
      	
      	delimiter = " or ";
      	if(where_clause_string.find(delimiter)!=string::npos && flag_for_and ==0)
        {
			while(where_clause_string.find(delimiter)!=string::npos)
			{
				subclause_string = where_clause_string.substr(0, where_clause_string.find(delimiter));
				where_clause_string = where_clause_string.substr(where_clause_string.find(delimiter) + 4, where_clause_string.length() );
				where_sub_clause.push_back(subclause_string);
			}
			where_sub_clause.push_back(where_clause_string);
        	flag_for_or ++;
        }  
     	
      	if(flag_for_and ==0 && flag_for_or == 0)
          	where_sub_clause.push_back(where_clause_string);
      
		
		cout<<"\nwhere sub-clauses: ";

      	for( vector<string>::iterator it = where_sub_clause.begin() ; it != where_sub_clause.end() ; ++it)
		{
			cout<<(*it)<<" ";
			//cout<<find_att_no((*it), query_table_num);
		}
      
      	// checking if orderby has multiple attributes
      	string orderby_clause_string;
      	string att_string;
      	vector<string> orderby_att;
      	orderby_clause_string = query_object.orderby_clause;
      
        delimiter = ",";
      	if(orderby_clause_string.find(delimiter)!=string::npos)
        {
          while(orderby_clause_string.find(delimiter)!=string::npos)
          {
            att_string = orderby_clause_string.substr(0, orderby_clause_string.find(delimiter));
          	orderby_clause_string = orderby_clause_string.substr(orderby_clause_string.find(delimiter) + 1, orderby_clause_string.length() );
			orderby_att.push_back(att_string);
          }
          orderby_att.push_back(orderby_clause_string);
        }
      	else
          orderby_att.push_back(orderby_clause_string);
      
      	
      	cout<<"\norderby attributes: ";

      	for( vector<string>::iterator itr = orderby_att.begin() ; itr != orderby_att.end() ; ++itr)
			cout<<(*itr)<<" ";
      	

      	/* display page nos associated with the table
      	for( vector<int>::iterator itr = temp_page_list.begin() ; itr != temp_page_list.end() ; ++itr)
					cout<<(*itr)<<" ";
         
      	for( vector<int>::iterator itr = temp_page_list.begin() ; itr != temp_page_list.end() ; ++itr)
        {
          page_struct temp_page;
          temp_page = page[(*itr)];
          //cout<<"\n"<<(*itr)<<" page data"<<temp_page.page_data;          
        }
		*/
        
        //for 'AND'
        if(flag_for_and !=0)
		{
			int first =0;
			string temp_result1, temp_result2;
			for( vector<string>::iterator itr = where_sub_clause.begin() ; itr != where_sub_clause.end() ; ++itr)
				{
					first++;
					string clause_details;
					string records, single_record;		
					string sub_clause_result;
		          	int att_no, query_type, comparison_number, int_att;
		          	string str_att;
		          	string comparison_string;
		          	string sub_clause = (*itr);

		          	if(find_att_no(sub_clause, query_table_num)!="error")
		          	{
			        	clause_details = find_att_no((*itr), query_table_num);
			        	att_no = stoi(clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length()));
			        	clause_details = clause_details.substr(0,clause_details.find_last_of(","));
			        	query_type = stoi(clause_details.substr(0,clause_details.find_last_of(",")));
			        	
			        	if(query_type == 7)
			        		comparison_string = clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length());
			        	else
			        		comparison_number =  stoi(clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length()));

			        	cout<<"\n \natt_no:"<<att_no<<"query_type:"<<query_type<<"comparison_number:"<<comparison_number<<"comparison_string"<<comparison_string<<endl;
			        	
				        if(first == 1)
				        {
				        	for( vector<int>::iterator it = temp_page_list.begin() ; it != temp_page_list.end() ; ++it)
				        	{
				        		page_struct temp_page;
				        		
				          		temp_page = page[(*it)];
				          		temp_result1.append(temp_page.page_data);
				        	}
				        }	
			        	
				        if(first%2 != 0)
				        {
				        	while(temp_result1.find("&")!= string::npos)
					        {
					        	single_record = temp_result1.substr(0,temp_result1.find("&")+1);
					        	temp_result1 = temp_result1.substr(temp_result1.find("&")+1, temp_result1.length());
				          		vector<string> tokens = split(single_record, ',');
				          		
				          		single_record.append("&");

				          		if(query_type != 7)
				          			int_att = stoi(tokens[att_no-1]);
				          		else
				          			str_att = tokens[att_no-1];

				          		switch(query_type)
				          		{
				          			case 1:
				          					if(int_att == comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}
				          					break;
				          			case 2:if((int_att == comparison_number)||(int_att > comparison_number))
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}
				          					break;
				          			case 3:if((int_att == comparison_number)||(int_att < comparison_number))
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}
				          					break;
				          			case 4:if(int_att != comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}
				          					break;
				          			case 5:if(int_att > comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}
				          					break;
				          			case 6:if(int_att < comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result2.append(single_record);
				          					}	
				          					break;
				          			case 7: cout<<"not done yet";
				          		}
				        	}
				        	temp_result1 = "0";
				        }
				        else
				        {
				        	while(temp_result2.find("&")!= string::npos)
					        {
					        	single_record = temp_result2.substr(0,temp_result2.find("&")+1);
					        	temp_result2 = temp_result2.substr(temp_result2.find("&")+1, temp_result2.length());
				          		vector<string> tokens = split(single_record, ',');
				          		
				          		single_record.append("&");
				          		
				          		if(query_type != 7)
				          			int_att = stoi(tokens[att_no-1]);
				          		else
				          			str_att = tokens[att_no-1];

				          		switch(query_type)
				          		{
				          			case 1:
				          					if(int_att == comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result1.append(single_record);
				          					}
				          					break;
				          			case 2: if((int_att == comparison_number)||(int_att > comparison_number))
				          					{
				          						cout<<int_att<<" ";
				          						temp_result1.append(single_record);
				          					}
				          					break;
				          			case 3:if((int_att == comparison_number)||(int_att < comparison_number))
				          					{
				          						cout<<int_att<<" ";
				          						temp_result1.append(single_record);
				          					}
				          					break;
				          			case 4:if(int_att != comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result1.append(single_record);
				          					}
				          					break;
				          			case 5:if(int_att > comparison_number)
				          					{
				          						cout<<int_att<<" ";
				          						temp_result1.append(single_record);
				          					}
				          					break;
				          			case 6:if(int_att < comparison_number)
				          					{
				          						cout<<single_record<<"\n ";
				          						temp_result1.append(single_record);
				          					}	
				          					break;
				          			case 7: str_att = myToLower(str_att);
				          					comparison_string = myToLower(comparison_string);
				          					if(str_att == comparison_string)
				          						sub_clause_result.append(single_record);
				          		}
				        	}
				        	temp_result2 = "0";			
				        }
				        // cout<<"\n\ntemp_result1"<<temp_result1;
						//cout<<"\n\ntemp_result2"<<temp_result2;		
			        }
			        else
			        	printf("error!!");
				}
			if(temp_result1.length() != 1)
			{	
				result = temp_result1;
			}
			else
			{	
				result = temp_result2;
			}		 
		}
        //for 'OR'
        else 
        {
	        for( vector<string>::iterator itr = where_sub_clause.begin() ; itr != where_sub_clause.end() ; ++itr)
			{
				string clause_details;
				string sub_clause_result;
	          	int att_no, query_type, comparison_number;
	          	string comparison_string;
	          	string sub_clause = (*itr);

	          	if(find_att_no(sub_clause, query_table_num)!="error")
	          	{
		        	clause_details = find_att_no((*itr), query_table_num);
		        	att_no = stoi(clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length()));
		        	clause_details = clause_details.substr(0,clause_details.find_last_of(","));
		        	query_type = stoi(clause_details.substr(0,clause_details.find_last_of(",")));
		        	
		        	if(query_type == 7)
		        		comparison_string = clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length());
		        	else
		        		comparison_number =  stoi(clause_details.substr(clause_details.find_last_of(",") + 1, clause_details.length()));

		        	cout<<"\n \natt_no:"<<att_no<<"query_type:"<<query_type<<"comparison_number:"<<comparison_number<<"comparison_string:"<<comparison_string<<endl;
		        	
		        	for( vector<int>::iterator it = temp_page_list.begin() ; it != temp_page_list.end() ; ++it)
		        	{
		        		string record;
		        		int int_att;
		        		string str_att;
		        		page_struct temp_page;
		        		
		          		temp_page = page[(*it)];
		          		while(temp_page.page_data.find("&")!= string::npos)
		          		{
			          		record = temp_page.page_data.substr(0,temp_page.page_data.find("&"));
			          		temp_page.page_data = temp_page.page_data.substr(temp_page.page_data.find("&")+1, temp_page.page_data.length());
			          		vector<string> tokens = split(record, ',');
			          		
			          		record.append("&");
			          		
			          		if(query_type != 7)
			          			int_att = stoi(tokens[att_no-1]);
			          		else
			          			str_att = tokens[att_no-1];


			          		switch(query_type)
			          		{
			          			case 1:
			          					if(int_att == comparison_number)
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}
			          					break;
			          			case 2:if((int_att == comparison_number)||(int_att > comparison_number))
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}
			          					break;
			          			case 3:if((int_att == comparison_number)||(int_att < comparison_number))
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}
			          					break;
			          			case 4:if(int_att != comparison_number)
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}
			          					break;
			          			case 5:if(int_att > comparison_number)
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}
			          					break;
			          			case 6:if(int_att < comparison_number)
			          					{
			          						cout<<int_att<<" ";
			          						sub_clause_result.append(record);
			          					}	
			          					break;
			          			case 7: str_att = myToLower(str_att);
			          					comparison_string = myToLower(comparison_string);
			          					cout<<str_att<<endl;
			          					if(str_att == comparison_string)
			          					{
			          						cout<<"found";
			          						sub_clause_result.append(record);
			          					}	
			          		}
			          	}	
		        	}
		        	//cout<<"\n\nrecords that match for the sub clause "<<(*itr)<<" are:"<<endl;
		        	//cout<<sub_clause_result;
		        	temp_results.push_back(sub_clause_result);	
		        }
		        else
		        	printf("error!!");
			}
			for( vector<string>::iterator itr = temp_results.begin() ; itr != temp_results.end() ; ++itr)
				result.append((*itr)); 
		}
		
		//callnt the sorting fucntion
		//cout << orderby_clause_string;
		string temp_fuuly_string =order_results(result , orderby_clause_string, query_table_num);
		

		// to open csv file
      	string line;
        fstream result_csv;
        string table_name_for_file = "result.csv";
        result_csv.open(table_name_for_file, fstream::in | fstream::out | fstream::app);

        if(result_csv.is_open() )
        {
        	result_csv<< result;
        }
        else
        {
          printf("unable to open the csv file");
        }
        result_csv.close();
      				
	}

	void queryType(string query)
	{
		if(query.find("create") != string::npos)
		{
			try
			{
				createCommand(query);
			}
			
			catch(const char* msg)
			{
				printf("\nInvalid query %s", msg);
			}
		}
		else if(query.find("select") != string::npos)	
		{
			try
				{
					selectCommand(query);
				}
				catch(const char* msg)
				{
					printf("\nInvalid query %s", msg);
				}
		}
		else
			throw"error";
	}

	void analysis(DBSystem object)
	{
		int numpages[5] ={300,400,500,600,700};
		int pagesize[5] ={50,100,150,200,250};
		int lrusize[5] ={2,4,6,8,10};
		float ip , gp;

		string analysis_file = "analysis/analysis.txt";
		ofstream file;
		file.open(analysis_file);

		for (int i = 0; i < 5; ++i) //pagesize
		{
			for (int j = 0; j < 5; ++j) //lrusize
			{
				for (int k = 0; k < 5; ++k) //numpages
				{
					string config = "analysis/config" + to_string(i+1) + to_string(j+1);
					object.readConfig(config, numpages[k]);
					object.populatePageInfo();

					ip = insert_performance(object);
					gp = get_performance(object);
					file <<"\nanalysis ::  "<< "\npagesize\t"<< pagesize[i] <<"\nlrusize\t" <<lrusize[j]   <<" \nnumpages\t" <<numpages[k] ;
					file <<"\ninsert_performance\t " << ip  <<"\nget_performance\t " <<gp <<endl;
					//cout <<"\nananlysis::\t"<< ip<<","<<gp;
				}
			}
		}
		file.close();
	}

	int insert_performance( DBSystem object){
		string insert_file = "analysis/insert_test.txt"; 
		string tablename, record;
		ifstream file;
		file.open(insert_file);
		float ip=0;
		string line;
		if(file.is_open()){

			for (int i = 0; i < 100; ++i)				// for reading the structure of our DB 
			{
				getline(file,line);
				vector<string> tokens=split(line, ',');
				tablename=tokens[0];
				record = tokens[1];
				ip += object.insertRecord(tablename,record);
			}
		}
		return ip;
	}

	int get_performance(DBSystem object){
		string insert_file = "analysis/get_test.txt"; 
		string tablename;
		int recordId;
		ifstream file;
		file.open(insert_file);
		float gp=0;
		string line;
		if(file.is_open()){

			for (int i = 0; i < 500; ++i)				// for reading the structure of our DB 
			{
				getline(file,line);
				vector<string> tokens=split(line, ',');
				tablename=tokens[0];
				recordId = stoi(tokens[1]);
				int temp =object.getRecord(tablename,recordId);
				if(temp >=0)
					gp++;
			}
		}
		return gp;
	}

};

int main()
{
	DBSystem object;
	int choice, recordId;
	string tablename, record;
	string query;
	string line;
	int numpages =10000; //changed
	int arr[] = {30,35,15,5,10,20,25};

	while(1)
	{
		//printf("enter choice\n");
		cout<< "\n1. readconfig(configFilePath) \n2. populatePageInfo \n3. getRecord(tablename,record) \n4. insertRecord(tablename,record) \n";
		cout<< "5. flushPages()\n6. EnterQuery()\n8. analysis\n9. paranthesis\n0. exit() \n\n\n\n\n\n\n\n\n";
		cin >> choice;

		switch(choice)
		{
			case 1:
				//cout<<"enter configFilePath";
				//cout << "\n>";
				//cin >> configFilePath;
				object.readConfig(configFilePath,numpages); //changed
				break;
			case 2:
				object.populatePageInfo();
				break;
			case 3:
				cout<<"enter tablename and recordId";
				cout <<endl << ">";
				cin >> tablename;
				cout <<endl<< ">";
				cin >>recordId;
				object.getRecord(tablename,recordId);
				break;
			case 4:
				cout<<"enter tablename and record";
				cout <<endl<< ">";
				cin>> tablename;
				cout <<endl<< ">";
				cin>>record;
				object.insertRecord(tablename,record);
				break;
			case 5:
				object.flushPages();
				break;
			case 6:
				try
				{
					cout<< "\n enter query ::\n";
					cout << ">";		
					line = "";
					query = "";

					while(line[line.size() -1] != ';')
					{
						getline(cin,line);
						if(query != "")
							query +=" ";
						query += line;
						cout << ">>";
					}
					
					//getline(cin,query);
					//getline(cin,query);
					query= check_spaces(query);
					query = myToLower(query);
					cout<<"entered query"<<query;
					object.queryType(query);
				}
				catch(const char* msg)
				{
					cout<<"\n Invalid query12\n";
				}
				break;
			case 7:
				//
				break;
			case 8:
				object.analysis(object);
				break;
			case 0:
				exit(0);
				break;

			default:
				cout<<"print correct choice";		

		}
	}
	return 0;
}