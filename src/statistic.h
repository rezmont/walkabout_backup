#ifndef STATISTIC_H
#define STATISTIC_H

//#include <vector>
//#include <map>
//#include <string>
//#include <string.h>
//#include <iostream>
//#include <fstream>
//#include <cmath>
//#include <set>
//#include <list>
//#include <stdio.h>
//#include <limits.h>
//#include <stdlib.h>
//#include <algorithm>
//#include <stdlib.h>
//#include <ctime>

//#include <boost/serialization/vector.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive_impl.hpp>
//#include <boost/serialization/binary_object.hpp>

//#include <boost/shared_ptr.hpp>

#include "walker.h"

using namespace std;

class RandVar
{
public:
	int id;
	int degree;
	float meanInvVisit;
	float varInvVisit;
	double dvr;
	double conf;
	RandVar(int i, int d, float mean, float var, float z, int n);
	void calc(int n, float z);
};

class Cluster
{
public:
	int id;
	float min;
	float max;

	Cluster(int i, double min, double max);
};


inline bool pointDegCompare (RandVar a, RandVar b) {
	return a.degree > b.degree;
}


class Stats
{
public:
	vector<RandVar> points;
	Stats();
	void loadPoints(const char *fname);
	void cluster(const char *fname, int margin);

};

#endif // STATISTIC_H
