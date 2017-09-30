#include <iostream>
#include "includes/intervaltree/IntervalTree.h"

#include <set>
#include <map>
#include <fstream>
#include <sstream>

#include <chrono>
#include <iomanip>

using namespace std;

IntervalTree<long, double > *temporalTree;
vector<Interval<long, double> > intervals;

void readBrinkhoff(const char *filename){
	map<long, pair<double, pair<int,int> > >Objects; // id -> (time, (x,y) )

	ifstream infile(filename);
	string line;
	long id = 1;
	while(getline(infile,line)){
		istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;

		Interval<long, double> *tmpInterval = new Interval<long, double>(t1, t2, id++);
		intervals.push_back(*tmpInterval);
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
		Interval<bool,double>* temporalWindow = new Interval<bool, double>(t1,t2,false);
		vector<Interval<long, double> > auxRes;

		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		temporalTree->findOverlapping(temporalWindow->start,temporalWindow->stop,auxRes);
		for(int i=0;i<auxRes.size();i++)
		{
			resArray->insert(auxRes[i].value);
		}
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
		cout << "Usage: ./xtest [trajectoriesFile] [queriesFile] [outfile]" << endl;
		return -1;
	}

	const char* trajectoriesFile 	= argv[1];
	const char* queriesFile 		= argv[2];
	const char* outFile 			= argv[3];

	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	



	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	chrono::high_resolution_clock::time_point start2 = chrono::high_resolution_clock::now();
	readBrinkhoff(trajectoriesFile);
	temporalTree = new IntervalTree<long, double>(intervals);
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "> I-Tree indicators:" << endl;
	cout << "   > Building time = " << right << setw(10);
	cout << chrono::duration_cast<chrono::microseconds>( end - start2 ).count();
	cout << " microseconds" << endl;

	readQueries(queriesFile, outFile);

	return 0;
}
