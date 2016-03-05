#include "statistic.h"

RandVar::RandVar(int i, int d, float mean, float var, float z, int n) {
	id           = i;
	degree       = d;
	meanInvVisit = mean;
	varInvVisit  = var/pow(mean,4);

	dvr		= d/mean;
	conf	= d*z*sqrt(varInvVisit/n);
}

void RandVar::calc(int n, float z) {
}

Cluster::Cluster(int i, double n, double x) {
	id  = i;
	min = n;
	max = x;
}

Stats::Stats() {
	points = vector<RandVar>();
}


void Stats::loadPoints(const char *fname) {
	int z = 2.718;
	int n = 10;
	ifstream inFile;
	inFile.open(fname,fstream::in);
	if(!inFile) {
		cout << "Cannot open infile_name: "<<fname<<".\n";
		exit(-1);
	}
	int i, d, visit;
	float meanVis, varVis;
	//	float meanInvVis, varInvVis;
	int lineCnt=0;
	while(!inFile.eof()) {
		lineCnt++;
		inFile >> i >> d >> visit >> meanVis >> varVis ; // >> meanInvVis >> varInvVis;
		if (d>=100) {
			points.push_back(RandVar(i, d, meanVis, varVis, z, n));
		}
		//		cout<<lineCnt<<"\t"<<i<<"\t"<<d<<"\t"<<meanVis<<"\t"<<varVis<<endl;
		//		points.back().calc();
	}
	inFile.close();
}

void Stats::cluster(const char *name, int margin) {
	sort(points.begin(),points.end(),pointDegCompare);
	cout<<"points sorted "<<points.begin()->degree<<endl;
	cout<<"margin "<<margin<<endl;

	ofstream oFile;
	char fname[1024];
	sprintf(fname,"%s_margin_%d",name,margin);
	oFile.open((const char*)fname,fstream::out);
	//	FILE *oFile = fopen(fname, "w");

	vector<Cluster> cluster;
	for (std::vector<RandVar>::iterator itPoint = points.begin() ; itPoint != points.end(); ++itPoint) {
		double x1=itPoint->dvr-itPoint->conf*margin;
		double x2=itPoint->dvr+itPoint->conf*margin;

		int assignedTo=-1;
		int assignedCount=0;
		for (unsigned int i=0; i<cluster.size(); i++) {
			double y1=cluster[i].min;
			double y2=cluster[i].max;
			if ((x1 <= y2) && (y1 <= x2)) {
				assignedTo = i;
				assignedCount++;
			}
		}
		if ( assignedCount == 1 ) {
			oFile << itPoint->id << "\t" << itPoint->degree << "\t" << assignedTo << "\t" << itPoint->meanInvVisit << "\t" << itPoint->dvr << "\t" << itPoint->conf*margin <<"\t0" << endl;
			oFile.flush();
		} else if ( assignedCount == 0 ) {
			assignedTo=cluster.size();
			cluster.push_back(Cluster(assignedTo,x1,x2));
			oFile << itPoint->id << "\t" << itPoint->degree << "\t" << assignedTo << "\t" << itPoint->meanInvVisit << "\t" << itPoint->dvr << "\t" << itPoint->conf*margin <<"\t1" << endl;
			oFile.flush();
		}
	}
	oFile.close();
}

//int main ( int argc, char **argv ) {
//	int task = -1;
//	char nettype[1024], dumpdir[1024], inFName[1024];
//	sprintf (nettype ,"-1");
//	sprintf (dumpdir ,"-1");
//	sprintf (inFName	 ,"-1");
//	for ( int c=1 ; c < argc ; c++ ) {
//		if		( !strcmp ( argv[c], "-task"	) ) task		= atoi ( argv[++c] ); //selects what the code should do
//		else if ( !strcmp ( argv[c], "-nettype"	) ) sprintf (nettype ,"%s", argv[++c]);
//		else if ( !strcmp ( argv[c], "-dumpdir"	) ) sprintf (dumpdir ,"%s", argv[++c]);
//		else if ( !strcmp ( argv[c], "-inFName"	) ) sprintf (inFName ,"%s", argv[++c]);
//	}
//}
