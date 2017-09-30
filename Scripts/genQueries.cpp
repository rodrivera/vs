#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

int main(int argc, char const *argv[])
{

	if (argc < 3){
		cout << "Usage: ./gen num_queries percT" << endl;
		cout << "percT must be a float number between 0 and 100 (inclusive)." << endl;
		return -1;
	}

	double minT = 0.0, maxT = 100.0;
	double dT = maxT-minT;

	int num = atoi(argv[1]);
	float percT = atof(argv[2])/100;

	if (percT < 0 || percT > 100){
		cout << "percT must be a float number between 0 and 100 (inclusive)." << endl;
		return -1;
	}

	srand(time(NULL));

	double rT = (1.0-percT)*dT;

	for (int i = 0; i < num; ++i){
		double qT1 = (rT != 0)? fmod(rand(),rT) + minT : minT;
		double qT2 = qT1 + percT*dT;

		cout << qT1 << " " << qT2 << endl;
	}

	return 0;
}
