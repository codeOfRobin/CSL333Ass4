#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <sys/time.h>

using namespace std;

typedef vector<vector<float> > matrix;
typedef vector<vector<bool> > boolmat;
typedef vector<float> vec;

#define all(a) a.begin(),a.end()
#define print(x) cout<<x
#define prints(x,y) print(x) print(y)
#define print2(x,y) prints(x," ") print(y)
#define print3(x,y) prints(x," ") print2(y,z)

#define iter(i,max) for(int i=0;i<(int)(max);i++)
#define itervec(i,v) for(typeof(v.begin()) i = c.begin(); i != c.end(); i++)

double get_wall_time(){
	struct timeval tim;
	if(gettimeofday(&tim, NULL)) return 0;
	return (double)tim.tv_sec + (double)tim.tv_usec * .000001;
}

class node{
public:
	string name;
	int id;
	vector<int> children;
	vector<int> parents;
	vector<string> values;
	bool is_initialized;
	vec probs;

	node(int c, string s, vector<string> vals){
		id=c;
		name=s;
		values=vals;
		is_initialized=false;
		probs.clear();
	}
};

class graph{
public:
	vector<node> nodes;
	int counter;
	matrix data;
	matrix data_transpose;
	vector<int> iscomplete;
	vector<pair<float, vector<float> > > missing_distributions;
	int data_nonzero;
	int iter_count;
	int smoothing;

	graph(){
		counter=0;
		iter_count=0;
		data_nonzero=0;
		smoothing=0;
	}

	void insert(string s, vector<string> vals){
		node temp(counter,s,vals);
		counter++;
		nodes.push_back(temp);
	}

	int get_node(string name){
		iter(i,nodes.size()) if(name==nodes.at(i).name) return nodes.at(i).id;
		return -1;
	}

	void insert_probs(int nodeNumber, vec inp_probs){
		nodes.at(nodeNumber).probs = inp_probs;
		nodes.at(nodeNumber).is_initialized=true;
	}

	void get_locs(int node_id, int index, vector<pair<int,int> > &locs){
		locs.clear();
		vector<int> parents=nodes.at(node_id).parents;
		parents.push_back(node_id);
		reverse(all(parents));

		iter(i,parents.size()){
			int mod_val = index%(nodes.at(parents.at(i)).values.size());
			index = index/(nodes.at(parents.at(i)).values.size());
			locs.push_back(make_pair(parents.at(i),mod_val));
		}

		reverse(all(locs));
	}

	int get_index(int node_id, vector<pair<int,int> > locs){
		int index=0;
		int multFactor=1;
		for (int i = locs.size()-1; i >=0; --i){
			index+=multFactor*locs.at(i).second;
			multFactor*=nodes.at(locs.at(i).first).values.size();
		}
		return index;
	}

	float get_prob(int node_id, vector<pair<int,int> > locs){
		return nodes.at(node_id).probs.at(get_index(node_id,locs));
	}

	void set_prob(int node_id, vector<pair<int,int> > locs, float val){
		nodes.at(node_id).probs.at(get_index(node_id,locs)) = val;
	}


	void init_nodes(matrix data){
		iter(node,nodes.size()){
			if(!nodes.at(node).is_initialized){
				vec probs;
			}
		}
	}

	void expectation_step(){
		//estimate the distribution of the missing values
		missing_distributions.clear();
		iter(i,data.size()){
			int n_miss = nodes.at(iscomplete.at(i)).values.size();
			vec temp;
			float sum=0;
			iter(j,n_miss){
				vec new_data = data.at(i);
				new_data.at(iscomplete.at(i))=j;
				float t = exp(get_loglikelihood(new_data));
				temp.push_back(t);
				sum+=t;
			}
			iter(j,n_miss){
				temp.at(j)/=sum;
			}
			iter(j,n_miss){
				vec new_data = data.at(i);
				new_data.at(iscomplete.at(i))=j;
				missing_distributions.push_back(make_pair(temp.at(j),new_data));
			}
		}
	}

	void maximization_step(){
		iter(i,nodes.size()){
			int rev_val = nodes.at(i).probs.size()/nodes.at(i).values.size();
			vec probs_sum(rev_val,0);
			vec probsval;
			vector<vector<pair<int,int> > > locs_vec;
			iter(j,nodes.at(i).probs.size()){
				vector<pair<int,int> > locs;
				get_locs(i,j,locs);
				locs_vec.push_back(locs);
				float probs = get_count_vec(locs)+smoothing;
				probs_sum.at(j%rev_val)+=probs;
				probsval.push_back(probs);
			}
			iter(j,nodes.at(i).probs.size()){
				set_prob(i,locs_vec.at(j),probsval.at(j)/probs_sum.at(j%rev_val));
			}
		}
	}

	void init(string filename){
		//read network from file
		string line;
	  	ifstream myfile(filename.c_str()); 
	  	
	  	while(getline(myfile,line)){
	  		istringstream iss(line);
	  		string temp_word;
	  		iss>>temp_word;
	  		if(temp_word=="variable"){
	  			//line describing variable
	  			string ft_name;
	  			iss>>ft_name;
	    		ft_name.erase(remove(all(ft_name),'\"'),ft_name.end());
	    		//got name of node
	    		getline(myfile,line);
	    		istringstream nss(line);
	    		string words;
	    		vector<string> values;
	    		while(nss>>words){
	    			if(words=="{"){
	    				//opening bracket detected
	    				nss>>words;
	    				while(words!="};"){
	    					//until closing bracket is detected
	    					words.erase(remove(all(words),'\"'),words.end());
	    					values.push_back(words);
	    					nss>>words;
	    				}
	    			}
	    		}
	    		insert(ft_name,values);
	    		cout<<"inserted node "<<ft_name<<endl;
	  		}
	  		else if(temp_word=="probability"){
	  			//line describing probability
	  			iss>>temp_word;
	  			iss>>temp_word;

	  			temp_word.erase(remove(all(temp_word),'\"'),temp_word.end());
	  			int node_id = get_node(temp_word);

	  			iss>>temp_word;
	  			while(temp_word!=")"){
	  				temp_word.erase(remove(all(temp_word),'\"'),temp_word.end());
                    int parent_id = get_node(temp_word);
                    nodes.at(node_id).parents.push_back(parent_id);
                    nodes.at(parent_id).children.push_back(node_id);
 					iss>>temp_word;
	  			}

	  			cout<<"inserted parents for node "<<node_id<<endl;
	  			getline(myfile,line);
	  			istringstream nss(line);
	  			vec probs(0,0);
	  			while(nss>>temp_word){
	  				if(temp_word=="table"){
	  					nss>>temp_word;
	  					while(temp_word!=";"){
	  						probs.push_back(atof(temp_word.c_str()));
	  						nss>>temp_word;
	  					}
	  				}
	  			}
	  			insert_probs(node_id,probs);
	  			cout<<"inserted probabilities for node "<<node_id<<endl;
	  		}
	  	}
	}

	float get_loglikelihood(vector<float> settings){
		float sum=0;
		iter(i,nodes.size()){
			vector<pair<int,int> > parents_settings;
			iter(j,nodes.at(i).parents.size()){
				parents_settings.push_back(make_pair(nodes.at(i).parents.at(j),(int)(settings.at(nodes.at(i).parents.at(j)))));
			}
			sum+=log(get_prob(i,parents_settings));
		}
		return sum;
	}

	float get_complete_probs(){
		iter(i,nodes.size()){
			int rev_val = nodes.at(i).probs.size()/nodes.at(i).values.size();
			vec probs_sum(rev_val,0);
			vec probsval;
			vector<vector<pair<int,int> > > locs_vec;
			iter(j,nodes.at(i).probs.size()){
				vector<pair<int,int> > locs;
				get_locs(i,j,locs);
				locs_vec.push_back(locs);
				float probs = get_count_vec(locs)+smoothing;
				probs_sum.at(j%rev_val)+=probs;
				probsval.push_back(probs);
			}
			iter(j,nodes.at(i).probs.size()){
				set_prob(i,locs_vec.at(j),probsval.at(j)/probs_sum.at(j%rev_val));
			}
		}
	}

	void read_records(string filePath){
		vector<vector<string> > records;
	    ifstream myfile(filePath.c_str());
	    string line;

	    while(getline(myfile,line)){
	    	vector<string> words(nodes.size(),"");
	    	istringstream iss(line);
	    	iter(i,words.size()){
	    		string temp;
	    		iss >> temp;
	    		temp.erase(remove(all(temp),'\"'),temp.end());
	    		words.at(i) = temp;
	    	}
	    	records.push_back(words);
	    }

    	myfile.close();
    	data.clear();
    	data_nonzero=0;

    	iter(i,records.size()){
    		vec temp;
    		int iscomplete_i=-1;
    		iter(j,records.at(i).size()){
    			string rv = records.at(i).at(j);
    			if(rv=="?"){
    				temp.push_back(-1);
    				iscomplete_i=j;
    			}
    			else{
    				int loc = find(all(nodes.at(j).values),rv) - nodes.at(j).values.begin();
    				temp.push_back(loc);
    			}
    		}
    		data.push_back(temp);
    		iscomplete.push_back(iscomplete_i);
    	}
    	print("file read successfully\n");

    	iter(i,data.at(0).size()){
    		vec temp;
    		iter(j,data.size()){
    			temp.push_back(data.at(j).at(i));
    		}
    		data_transpose.push_back(temp);
    	}
	}

	void write_bif(string inPath,string outPath){
	    ifstream myfile(inPath.c_str());
	    ofstream outFile(outPath.c_str());
	    string line;
	    int table_count=0;
	    int ex=0;
	    if (myfile.is_open())
	    {
	        getline (myfile,line);
	        while (! myfile.eof() )
	        {
	            if (line.find("table") != string::npos)
	            {
	                outFile<<"\ttable ";
	                iter(i,nodes.at(table_count).probs.size()){
	                	outFile<<nodes.at(table_count).probs.at(i)<<" ";
	                }
	                table_count++;
	                outFile<<";";
	            }
	            else
	            {
	                outFile<<line;
	            }
	            outFile<<"\n";
	            ex++;
	            getline (myfile,line);
	        }
	    }

	}

	int get_count(int col, float val){
		int count(0);
		iter(i,data_transpose.at(col).size()){
			if(data_transpose.at(col).at(i)==val) count++;
		}
		return count;
	}

	float get_count_vec(vector<pair<int,int> > locs){
		int count(0);
		iter(i,data.size()){
			bool should_be_counted=true;
			iter(j,locs.size()){
				if(data.at(i).at(locs.at(j).first)!=locs.at(j).second) should_be_counted=false;
			}
			if(should_be_counted) count++;
		}
		iter(i,missing_distributions.size()){
			bool should_be_counted=true;
			iter(j,locs.size()){
				if(missing_distributions.at(i).second.at(locs.at(j).first)!=locs.at(j).second) should_be_counted=false;
			}
			if(should_be_counted) count+=missing_distributions.at(i).first;
		}
		return count;
	}

	int get_sum(int col){
		float sum(0);
		iter(i,data_transpose.at(col).size()) sum+=data_transpose.at(col).at(i);
		return sum;
	}

	vector<pair<int,int> > join_vec(vector<int> a, vector<int> b){
		vector<pair<int,int> > out;
		iter(i,a.size()){
			out.push_back(make_pair(a.at(i),b.at(i)));
		}
		return out;
	}
	float get_diff(graph g){
		float diff;
		iter(i,nodes.size()){
			iter(j,nodes.at(i).probs.size()){
				diff+=fabs(nodes.at(i).probs.at(j)-g.nodes.at(i).probs.at(j));
			}
		}
		return diff;
	}
};

int main(int argc, char* argv[]){
	double t_init=get_wall_time();
	graph g;
	g.init("./alarm.bif");
	graph g2;
	g2.init("./gold_alarm.bif");
	g.read_records("./records.dat");
	g.smoothing=0;
	int i=10;
	cout<<"Initial error:"<<g.get_diff(g2)<<endl;
	double t_final=get_wall_time();
	while(t_final-t_init<570){
		g.maximization_step();
		g.expectation_step();
		cout<<"EM step complete, error:"<<g.get_diff(g2)<<",in time "<<t_final<<"."<<endl;
		t_final=get_wall_time();	
	}
	g.write_bif("./alarm.bif","./solved_alarm.bif");
	cout<<g.data.size()<<" "<<g.data.at(0).size()<<endl;
	return 0;
}