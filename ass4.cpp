#include <iostream>
#include <string>
#include <vector>
#include <utlility>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <math.h>
#include <cstdlib>

using namespace std;

typedef vector<vector<float> > matrix;
typedef vector<vector<bool> > boolmat;
typedef vector<float> vec;

#define all(a) a.begin(),a.end()
#define print(x) cout<<x<<
#define print(x,y) print(x) print(y)
#define print(x,y,z) print(x) print(y) print(z)

#define iter(i,max) for(int i=0;i<(int)(max);i++)
#define itervec(i,v) for(typeof(v.begin()) i = c.begin(); i != c.end(); i++)

class node{
public:
	string name;
	int id;
	vector<int> children;
	vector<int> parents;
	vector<string> values;
	bool is_initialized
	vec probs;

	node(int c, string s, vector<string> vals){
		id(c);
		name(s);
		values(vals);
		is_initialized=false;
		probs.clear();
	}
};

class graph{
public:
	vector<node> nodes;
	int counter;
	matrix records;
	int iter_count;

	network(){
		counter=0;
	}

	void insert(string s, vector<string> vals){
		node temp(counter,s,vals);
		nodes.push_back(temp);
	}

	int get_node(string name){
		itervec(i,nodes) if(name==i.name) return i.id;
		return -1;
	}

	void insert_probs(int nodeNumber, vec inp_probs){
		nodes.at(nodeNumber).probs = inp_probs;
		nodes.at(nodeNumber).is_initialized=true;
	}

	void get_locs(int node_id, int index, vector<pair<int,int> > &locs){

	}

	int get_index(int node_id, vector<pair<int,int> > &locs){
		//TODO: write the mapping from index to value in the array.
		return 1;
	}

	float get_prob(int node_id, vector<int> locs){
		return nodes.at(node_id).probs.at(get_index(node_id,locs));
	}

	void set_prob(int node_id, vector<int> locs, float val){
		nodes.at(node_id).probs.at(get_index(node_id,locs)) = val;
	}


	void init_nodes(matrix data){
		itervec(node,nodes){
			if(!node.is_initialized){
				vec probs;

			}
		}
	}

	void expectation_step(){

	}

	void calculate_probabilities()
	void maximization_step(){
		if(!iter_count){
			//if it is the first iteration, update by max-count values

		}
	}

	void init(string filename){
		//read network from file
		string line;
	  	ifstream myfile(filename); 
	  	string temp;
	  	string name;
	  	vector<string> values;
	  	
	    if (myfile.is_open()){
	    	while (!myfile.eof()){
	    		stringstream ss;
	      		getline (myfile,line);
	      		
	      		ss.str(line);
	     		ss>>temp;

	     		if(temp.compare("variable")==0){  
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++){
     					ss2>>temp;	
     				}
     				values.clear();
     				while(temp.compare("};")!=0){
     					values.push_back(temp);
     					ss2>>temp;
    				}

    				//trimming the quotes
    				name.replace("\"","");
    				itervec(i,values) i.replace("\"","");
    				insert(name, values);
	     		}
	     		else if(temp.compare("probability")==0){
	                 
	     				ss>>temp;
	     				ss>>temp;
	     				
	     				int node_id = get_node(temp);

	                    ss>>temp;
	                    values.clear();
	     				while(temp.compare(")")!=0){
	                        int parent_id = get_node(temp);
	                        nodes.at(node_id).parents.push_back(parent_id);
	                        nodes.at(parent_id).children.push_back(node_id);
	     					ss>>temp;
	    				}

	    				getline (myfile,line);
	     				stringstream ss2;
	                    
	     				ss2.str(line);
	     				ss2>> temp;
	     				ss2>> temp;
	                    
	     				vector<float> curr_CPT;
	                    string::size_type sz;
	     				while(temp.compare(";")!=0){
	     					curr_CPT.push_back(atof(temp.c_str()));
	     					ss2>>temp;
	    				}
	                    
	                    insert_probs(node_id,curr_CPT);
	     		}
	    	}
	    	myfile.close();
	  	}	
	}
	
	void readRecords(string filePath)
	{
    ifstream myfile(filePath);
    string line;
    if (myfile.is_open())
    {
        while (! myfile.eof() )
        {
            stringstream ss;
            getline (myfile,line);
            vector<string> lineInRecords;
            char tab2[1024];
            strcpy(tab2, line.c_str());
            char * pch;
            pch = strtok (tab2," \\ ,");
            int index=0;
            string word;
            while (pch != NULL)
            {
                word=string(pch);
                word=word.substr(1,word.size()-2);
                if (strcmp(word.c_str(), "?")==0)
                {
                    missingIndices.push_back(index);
                }
                lineInRecords.push_back(word);
                pch = strtok (NULL, " ,.-");
                
                index++;
            }
            records.push_back(lineInRecords);
            lineInRecords.clear();
            
        }
    }
    
}
};