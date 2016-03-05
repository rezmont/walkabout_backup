#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include "graph.h"

class Statistic
{
public:
	int noVisits;
	int noEnterance;
	float ewmaIntVisible;
	float ewmaIntVisited;
	int lastTransition;		//last transition to current state of visibility
	int lastTimeVisible;	//last transition to current state of visibility
	int lastTimeVisited;	//last transition to current state of visibility
	float visibility;		//visibility as defined by moji for individual walks
	
	Statistic() {
		noVisits = 0;
		noEnterance = 0;
		ewmaIntVisible=-1;
		ewmaIntVisited=-1;
		lastTransition=-1;	//last transition to current state of visibility
		lastTimeVisible=-1;	//last transition to current state of visibility
		lastTimeVisited=-1;	//last transition to current state of visibility
		visibility=-1;		//visibility as defined by moji for individual walks
	}
};

class WalkedNode//: public {
{
public:
	int id;
	int degree;
	vector <int> visitedWalks;
	int vis;				//visibility as defined by moji
	map <int,Statistic> stat;
	
	WalkedNode(int iden,int deg)
	{
		id=iden;
		degree=deg;
		vis=0;
	}
};

class VisitedNode {
public:
	int id;
	int degree;

	float num_visit;
	vector<float> num_visit_array;

	float est_num_visit;
	float est_num_visit_var;
//	float est_num_visit_conf;

	float meanInvNumVisit;
	float varInvNumVisit;

	vector<float> est_num_visit_array;

	VisitedNode(int iden,int deg) {
		id=iden;
		degree=deg;

		num_visit=0;
		num_visit_array = vector<float>(0);

		est_num_visit = 0;
		est_num_visit = 0;
		est_num_visit_var=0;
//		est_num_visit_conf=0;
		est_num_visit_array = vector<float>(0);

		meanInvNumVisit=0;
		varInvNumVisit=0;
	}

	VisitedNode(int iden,int deg, int numBatch) {
		id=iden;
		degree=deg;

		num_visit=0;
		num_visit_array = vector<float>(numBatch,0);

		est_num_visit = 0;
		est_num_visit_var=0;
//		est_num_visit_conf=0;
		est_num_visit_array = vector<float>(numBatch,0);

		meanInvNumVisit=0;
		varInvNumVisit=0;
	}
	
	void visited_by(int numBatch) {
		num_visit++;
		num_visit_array[numBatch]++;
	}
};

class VisitedNode_old {
public:
	int id;
	int degree;
	int visited_walks;
	int last_visited_walk;
	int auxVar;

	int num_visit;

	float est_num_visit;
	float est_num_visit_stdDev;
	float est_num_visit_conf;
	vector<float> est_num_visit_array;

	VisitedNode_old(int iden,int deg) {
		id=iden;
		degree=deg;
		visited_walks=0;
		last_visited_walk=-1;
		auxVar=-1;

		num_visit=0;

		est_num_visit = 0;
		est_num_visit = 0;
		est_num_visit_stdDev=0;
		est_num_visit_conf=0;
		est_num_visit_array = vector<float>(0);
	}

	VisitedNode_old(int iden,int deg, int numBatch) {
		id=iden;
		degree=deg;
		visited_walks=0;
		last_visited_walk=-1;
		auxVar=-1;

		num_visit=0;

		est_num_visit = 0;
		est_num_visit_stdDev=0;
		est_num_visit_conf=0;
		est_num_visit_array = vector<float>(numBatch,0);
	}

	void visited_by(int walker_id) {
		walker_id = 0;
		num_visit++;
	}

	void calcMovingAverages (int wl, int nw, float alpha, float beta) {
//		double theta=(num_visits*1.0)/(wl*nw*degree*1.0);
//		if(mv_avg_theta==0)
//			mv_avg_theta=theta;
//		else
//		{
//			mv_avg_theta=(1-alpha)*(mv_avg_theta)+alpha*theta;
//			// DevTata(i,t) = (1-b)*DevTeta(i,t-1) + b*|Teta(i,t)-AvgTeta(i,t)|
//			if(mv_dev_theta==0)
//				mv_dev_theta=abs(theta-mv_avg_theta);
//			else
//				mv_dev_theta=(1-beta)*mv_avg_theta+beta*abs(theta-mv_avg_theta);
//		}
	}
};


using namespace std;

void randomWalkEscapeRate(Graph graph, int partIndex , double nwPortion, int walk_len, int simulationRounds, char* dumpdir, char* nettype, int seed);

void randomWalkDiscoveredNodes(Graph graph, int num_walker, int walk_len, char* dumpdir, char* nettype, int seed);
void randomWalkAndDump(Graph graph,int walk_len,int initial_point,char *fileName);
int nextNodeIdSnowBall(Node n);

vector < vector <WalkedNode> > aggregateWalkers(int num_walker,int walk_len);
vector < vector <WalkedNode> > aggregateWalkers_visibles_conv(int num_walker,int walk_len);
void aggregateWalkers_visibles_pop(int num_walker,int walk_len,char * dumpdir,char *nettype, float coreScale);
void randomWalkDistanceSets(Graph graph, int num_walker, int walk_len , int seed, char* file);
void randomWalkTopolgyHigh(Graph graph, int num_walker, int walk_len, char *dumpdir, char *nettype, int seed, char* file);
void randomWalkTransitionsSets(Graph graph, float num_walker_p, int jump_len, int seed, char* file, int regionFileType);
void partRWTransitionCounter(Graph graph, char* dumpDir, char* nettype, int num_walker, int walkLen, int seed, char* regionFile, int regionFileType, char* outFile);
void extendRegionCore(Graph graph,int num_walker, int walk_len,int fileType, char* RW_dist_file, char * dumpdir,char *nettype, bool justLCC, int betweenRegions);
void randomWalkDistanceCenter(Graph graph, int num_walker, int walk_len, int seed, char* file0,int anchor);
void randomWalkAndProcess(Graph graph, int maxWL, float num_walker_p, char* dumpdir, char* nettype, int seed, bool excludeInit);
void pageRank(Graph graph, int minWL, int maxWL, int stepWL, char* dumpdir, char* netType, int walkType);
void evalMixing(Graph graph, float nwPortion, char* dumpdir, char* nettype, int seed);
void randomWalkAndLog(Graph graph, int num_walker, int walk_len ,char* dumpdir, char* nettype, int seed);
vector < vector <WalkedNode> > aggregateWalkers_visibles(int num_walker,int walk_len,int walker_idx);
void quickSort(vector <WalkedNode> &arr, int left, int right);
vector < vector <WalkedNode> > classifyVisitedNodes (vector <WalkedNode> arr,int num_walker);

void randomWalkDiscoveryRate(Graph graph, int num_walker, int walk_len, int seed, char* file0);
void randomWalkAndProcessDiscThresh(Graph graph, int num_walker, int walk_len, char* dumpdir, char* nettype, int seed, float mn,float mx);
void randomWalkSuperSpaceTransitionsSets(Graph graph, int num_walker, int walk_len, int jump_len, int seed, char* file);
