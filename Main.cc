#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <queue>
#include <cstdlib>
using namespace std;
#define MAX 100000000

ifstream fin;
ofstream fout;

struct mac{ //structure of machines 
	vector<int> adj;
	double wrtime;//working time of a machine
	double owntime;//
	int jobs_waited; //number of jobs that have been waiting in this machine
	int max_waited;
	int job_done; //number of jobs that have been done in this machine
	void initi(){ // initializatin of a machine
		job_done = jobs_waited = max_waited = owntime = 0;
	}
};

struct event{ 
	double time;//time of sending tis job
	int job_num;//job's number
	int mac_num;//Shows which machine operates this job
	bool is_done;//Shows whether job is done or not
	event(double _time, int _job_num, int _mac_num, bool _is_done){
		time = _time;
		job_num = _job_num;
		mac_num = _mac_num;
		is_done = _is_done;
	}
	
	bool operator < (event a) const{//Makes events comperable
		return time > a.time;
	}
};

mac *machines;//group of all machines
int num_mac;//number of all machines
int num_job;//number of all jobs
double *jobs; //time of jobs initally
double *comp_jobs; //time of completion of jobs
double final_time;//time when all jobs done

priority_queue <event> be; //the heap that stores events
void read(){ //Reads the input
	int mac_no,neigh_mac;
	double time_mac;
	fin>>num_mac;
	machines = new mac[num_mac];
	string line;
	getline(fin, line);
	for(int i=0; i<num_mac; i++){
		getline(fin,line);
		stringstream ss(line);
		ss >> mac_no >> time_mac;
		machines[mac_no].wrtime = time_mac;
		while(ss.good()){
			ss >> neigh_mac;
			machines[mac_no].adj.push_back(neigh_mac);
		}
	}
	fin >> num_job;
	jobs=new double[num_job];
	comp_jobs=new double[num_job];
	for(int i=0;i<num_job; i++){
		fin>>jobs[i];
	}
}

void initi(){ //Initialize machines and Sends events to priority queue
	
	for(int i=0;i<num_job;i++)
		be.push(event(jobs[i],i,0,0));
	for(int i=0;i<num_mac;i++)
		machines[i].initi();
}


int squeue(int a){//shortest queue part
	int sze = MAX,nn;
	for(int i=0;i<(int)machines[a].adj.size();i++){
		int x = machines[a].adj[i];
		if(machines[x].jobs_waited < sze){
			nn = x;
			sze = machines[x].jobs_waited;
		}
	}
	return nn;
}

int randneigh(int a){//random part
	int x = rand()%machines[a].adj.size();
	return machines[a].adj[x];
}

void runFactory(bool t){ //Starts the process
	while(!be.empty()){
		event cur = be.top(); be.pop();//current event
		mac &now = machines[cur.mac_num];//current machine
		if(cur.is_done){ 
			if(now.jobs_waited) now.jobs_waited--;
			int x; now.job_done++;
			if(!now.adj.size()){
				comp_jobs[cur.job_num] = cur.time - jobs[cur.job_num];
				final_time = cur.time;
				continue;
			}
			if(t) x = squeue(cur.mac_num);
			else x = randneigh(cur.mac_num);
			cur.mac_num = x; cur.is_done = 0;
			be.push(cur);
		}
		else{ //if current event event is not donw
			if(cur.time <= now.owntime){ 
				now.jobs_waited++;
				now.max_waited = max(now.jobs_waited, now.max_waited);
				now.owntime += now.wrtime;
				cur.time = now.owntime;
			}
			else{
				now.owntime = cur.time+now.wrtime;
				cur.time = now.owntime;
			}
			cur.is_done = 1;
			be.push(cur);
		}
	}
}

void write(){//Writes to the output
	fout.unsetf(ios::floatfield);
	fout.precision(4);
	fout.setf(ios::fixed, ios::floatfield);
	fout << final_time << endl; 
	fout.precision(2);
	fout.setf(ios::fixed, ios::floatfield);
	for(int i=0;i<num_mac;i++){
		fout << i << " " 
			<< machines[i].job_done*machines[i].wrtime/final_time
			<< " " << machines[i].max_waited << endl;
	}
	fout.precision(4);
	fout.setf(ios::fixed, ios::floatfield);
	for(int i=0;i<num_job;i++)
		fout << i << " " << comp_jobs[i] << endl;
}


int main(int argc, char *argv[]){	
	fin.open(argv[1]);
	read();
	fout.open(argv[2]);//shortest queue part
	initi();
	runFactory(0);
	write();
	fout.close();
	fout.open(argv[3]);//random part
	initi();
	runFactory(1);
	write();
	fout.close();
	return 0;
}
