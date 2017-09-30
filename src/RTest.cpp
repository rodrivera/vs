#include <iostream>
#include "includes/rtree/RTree.h"

#include <set>
#include <map>
#include <fstream>
#include <sstream>

#include <chrono>
#include <iomanip>

using namespace std;

RTree<long, double, 1, float>* temporalTree;

class Interval
{
public:
	double timeIn[1];
	double timeOut[1];

	Interval();
	~Interval() {};
	Interval(double tIn, double tOut)
	{
		timeIn[0] = tIn;
		timeOut[0] = tOut;
	}
};

static bool auxTemporalSearch(long id, void* arg)
{
	set<long>* resultArray = (set<long>*)arg;
	resultArray->insert(id);
	return true;
}

void readIntervals(const char *filename){
	map<long, pair<double, pair<int,int> > >Objects; // id -> (time, (x,y) )

	ifstream infile(filename);
	string line;
	long id = 1;
	while(getline(infile,line)){
		istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;

		Interval *tmpInterval = new Interval(t1,t2);
		temporalTree->Insert(tmpInterval->timeIn,tmpInterval->timeOut,id++);

	}

}

void readQueries(const char *inFilename, const char *outFilename){
	ifstream infile(inFilename);
	ofstream outfile(outFilename);
	string line;
	int cont = 1;

	chrono::microseconds duration(0);

	while(getline(infile,line)){
		istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;
		set<long>* resArray = new set<long>;
		Interval* temporalWindow = new Interval(t1,t2);

		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		temporalTree->Search(temporalWindow->timeIn, temporalWindow->timeOut, auxTemporalSearch, (void*)resArray );
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		duration += chrono::duration_cast<chrono::microseconds>( end - start );	

		outfile << "Test #" << cont++ << endl;
		for (set<long>::iterator it=resArray->begin(); it!=resArray->end(); ++it){
			outfile << *it << " ";
		}
		outfile << endl << endl;
	}

	cout << "   > Queries time  = " << right << setw(10);
	cout << duration.count();
	cout << " microseconds" << endl;
	outfile.close();
}

int main(int argc, char const *argv[])
{

	if(argc != 4)
	{
		cout << "Usage: ./fnrtest [intervalsFile] [queriesFile] [outFile]" << endl;
		return -1;
	}

	const char* intervalsFile 	= argv[1];
	const char* queriesFile 		= argv[2];
	const char* outFile 			= argv[3];
	
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	temporalTree = new RTree<long, double, 1, float>();



	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	chrono::high_resolution_clock::time_point start2 = chrono::high_resolution_clock::now();
	readIntervals(intervalsFile);

	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "> R-Tree indicators:" << endl;
	cout << "   > Building time = " << right << setw(10);
	cout << chrono::duration_cast<chrono::microseconds>( end - start2 ).count();
	cout << " microseconds" << endl;

	readQueries(queriesFile, outFile);
	//cout << endl;

	return 0;
}
