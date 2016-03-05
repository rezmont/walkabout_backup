#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <set>
#include <list>
#include <queue>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <algorithm>
#include <stdlib.h>
#include <ctime>

#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive_impl.hpp>
#include <boost/serialization/binary_object.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_list.hpp>


#include <QDataStream>
#include <QFile>

using namespace std;

typedef boost::shared_ptr< std::vector<int> > neighbors_t;

class MiniNode {
public:
	int id;
	int degree;

    MiniNode (int i, int d) {
        id=i;
        degree=d;
	}

    MiniNode () {
        id=-1;
        degree=-1;
    }

    int getID() {
        return id;
    }

    int getDergree() {
        return degree;
    }
};

inline bool degreeCompare (MiniNode a, MiniNode b) {
	return a.degree > b.degree;
}

class Node {
	int region;
public:
	int id;
	int groupsCount;
	neighbors_t neighbors;
	set <int> groups;

	int visits;
	int count;
	int hop;

	int time_to_stationary;
    int auxID;
	double eigenvector_centrality;
    int community;
    Node() : neighbors(new vector<int>()) {
		//	Node() {
		id		= -1;
		visits	= 0;
		count	= 0;
        auxID	= -1;
		time_to_stationary	= 0;
		eigenvector_centrality	= 0;
		hop = -1;
		region=-1;
        community=-1;
		groupsCount=0;
	}

	Node(int i) : neighbors(new vector<int>()) {
		id		= i;
		visits	= 0;
		count	= 0;
        auxID	= -1;
		time_to_stationary	= 0;
		eigenvector_centrality	= 0;
		hop = -1;
		region=-1;
        community=-1;
		groupsCount=0;
	}

	void setRegion(int rgn) {
		this->region=rgn;
	}

	int getRegion() {
		return this->region;
	}

    void addGroup() {
		groupsCount++;
	}

	void clearNode() {
		visits	= 0;
		count	= 0;
        auxID	= -1;
		time_to_stationary	= 0;
		eigenvector_centrality	= 0;
		hop = -1;

	}


	int getNeighborIdx(int nei) {
		int idx=-1;
		for(unsigned int i=0;i<neighbors->size();i++) {
			if (nei==neighbors->at(i)) {
				idx=i;
			}
		}
		return idx;
	}

	int removeNeighbor(int nei) {
		int idx=-1;
		for(unsigned int i=0;i<neighbors->size();i++) {
			if (nei==neighbors->at(i)) {
				neighbors->erase(neighbors->begin()+i);
				idx=i;
				break;
			}
		}
		return idx;
	}

	int addNeighbor(int nei) {
		time_to_stationary++;
		neighbors->push_back(nei);
		return 1;
	}

	int addAndCkeckNeighbor(int nei)
	{
		int idx=-1;
		for(unsigned int i=0;i<neighbors->size();i++) {
			if (nei==neighbors->at(i)) {
				idx=i;
			}
		}
		if(idx==-1)
		{
			neighbors->push_back(nei);
			idx=neighbors->size()-1;
		}
		return idx;
	}
};


inline bool operator==(Node a, Node b) {
	return a.id == b.id;
}


class Graph {
private:
//	friend class boost::serialization::access;
//	template<class Archive>
//	void serialize(Archive & ar, const unsigned int version) {
//		ar & this->nodesLst;
//	}
public:
	int noNodes;
	int noSeprateNodes;
	double noEdges;
	vector <Node> nodesLst;
	Graph() {
		noNodes=0;
		noEdges=0;
		noSeprateNodes=0;
	}
	void calc() {
		noNodes=0;
		noEdges=0;
		for (unsigned int i=0;i<nodesLst.size();i++)
		{
			if (nodesLst[i].neighbors->size()>0) noNodes++;
			else noSeprateNodes++;
			noEdges+=nodesLst[i].neighbors->size();
		}
	}

	void storeBin(const char *fname) {

		ofstream foutput;
		foutput.open(fname,ios::out|ios::binary);
		if (foutput.is_open()){
			int s = this->nodesLst.size();
			// outputs number of nodes
			foutput.write((char *)(&s),sizeof(unsigned int));
			for (int i=0 ; i<s; i++) {
				unsigned int eListSize = this->nodesLst[i].neighbors->size();
				foutput.write((char *)(&eListSize),sizeof(unsigned int));
				for (unsigned int j=0 ; j<eListSize ; j++) {
					int dest = this->nodesLst[i].neighbors->at(j);
					foutput.write((char *)(&dest),sizeof(int));
					std::cout << i << "\t"<< dest << std::endl; 
				}
			}
			foutput.close();
		} else {
			std::cout<< "Error creating the file"<<std::endl;
		}

	}

	void storeTxt(const char *fname) {

		ofstream ofs;
		ofs.open(fname,std::ofstream::out);
		if (ofs.is_open()){
			int s = this->nodesLst.size();
			// outputs number of nodes
			ofs << s << std::endl;
			for (int i=0 ; i<s; i++) {
				unsigned int eListSize = this->nodesLst[i].neighbors->size();
				ofs << i << " " <<eListSize;
				for (unsigned int j=0 ; j<eListSize ; j++) {
					int dest = this->nodesLst[i].neighbors->at(j);
					ofs << "," << dest; 
				}
				ofs << endl;
			}
			ofs.close();
		} else {
			std::cout<< "Error creating the file"<<std::endl;
		}

	}

	friend QDataStream &operator <<(QDataStream &ds, const Graph &g);
	friend QDataStream &operator >>(QDataStream &ds, Graph &g);
};

QDataStream &operator <<(QDataStream &ds, const Graph &g);
QDataStream &operator >>(QDataStream &ds, Graph &g);

typedef boost::shared_ptr< std::vector<int> > members_t;

class Group
{
public:
	int id;
	int region;
    int membersCount;
	int mappedMembersCount;
	double probabilty;
	members_t members;


	Group(int idx) : members(new std::vector<int>(0)) {
		this->id=idx;
		mappedMembersCount=0;
		region=-1;
		probabilty = 0;
	}

	void setRegion(int r) {
		region = r;
	}

	int getRegion() {
		return region;
	}

	double getProbability() {
		return probabilty;
	}

	void setProbability(double prob) {
		probabilty=prob;

	}

	vector <int> getMembers() {
		if (members->size()>0)
			return *members;
		else
			return vector <int>();
	}

    void incMembersCount() {
        membersCount++;
    }

	int getMembersCount() {
		return members->size();
	}

	void setMappedMembersCount(int cnt) {
		mappedMembersCount=cnt;
	}

	int getMappedMembersCount() {
		return mappedMembersCount;
	}
};

inline bool operator==(Group a, Group b) {
    return a.id == b.id;
}


class GroupSet {

public:
	int totalPopulation;
    vector <Group> groupLst;

	GroupSet()	{ totalPopulation=0; }
	GroupSet(int totPop, int count) {
		totalPopulation=totPop;
        for (int i=0;i<count;i++) groupLst.push_back(Group(i));
	}
	~GroupSet() {
        groupLst.clear();
	}

	int getGroupsSize()	{
        return groupLst.size();
	}

	void addMembership(int groupId,int memberId) {
		unsigned int gId=groupLst.size();
        while (groupLst.size()<=groupId) {
            groupLst.push_back(Group(gId));
			gId++;
		}
        groupLst[groupId].members->push_back(memberId);
	}

    void addLogicalMembership(int groupId)
    {
        int gId=groupLst.size();
        while (groupId>=gId)
        {
            groupLst.push_back(Group(gId));
            gId++;
        }
        groupLst[groupId].incMembersCount();
    }

    void evalProbablities() {
        for (unsigned int i=0;i<groupLst.size();i++) groupLst[i].setProbability(groupLst[i].getMembersCount()/(totalPopulation*1.0));
    }

    vector<int> getGroupMembers(int gid) {
        return groupLst[gid].getMembers();
    }

    int getGroupMembersCount(int gid)
    {
        return groupLst[gid].getMembersCount();
    }

//    int getGroupID(int gid) {
//        return groupLst[gid].id;
//    }

    void setTotalPopulation(int totPop) {
        totalPopulation=totPop;
    }

    double getProbability(int gid)
    {
        return groupLst[gid].getProbability();
    }

    void setProbability(int gid,double prob) {
        groupLst[gid].setProbability(prob);
    }

    void setGroupRegion(int gid, int regionID ) {
        groupLst[gid].setRegion(regionID);
    }

    int getGroupRegion(int gid) {
        return groupLst[gid].getRegion();
    }

    int getMappedMembersCount(int gid) {
        return groupLst[gid].getMappedMembersCount();
    }

    void setMappedMembersCount(int gid, int cnt) {
        groupLst[gid].setMappedMembersCount(cnt);
    }
};

class groupEntropy
{
public:
	int groupID;
	int groupPop;
	double unconditional;
	double conditional;
	double relativeDecrease;
	groupEntropy(int id,int pop, double un, double con,double rel) {
		groupID=id;
		groupPop=pop;
		unconditional=un;
		conditional=con;
		relativeDecrease=rel;
	}
};


inline bool operator==(groupEntropy a, groupEntropy b) {
	return (a.groupID == b.groupID);
}

inline bool relDecAcc(groupEntropy a, groupEntropy b) {
	return (a.relativeDecrease > b.relativeDecrease);
}

class groupRegionMembership {
public:
	int groupID;
	int groupPop;
	double maxRegionMembership;
	groupRegionMembership(int id,int pop, double max) {
		groupID=id;
		groupPop=pop;
		maxRegionMembership=max;
	}
};

inline bool operator==(groupRegionMembership a, groupRegionMembership b) {
	return (a.groupID == b.groupID);
}

inline bool memberAcc(groupRegionMembership a, groupRegionMembership b) {
	return (a.maxRegionMembership > b.maxRegionMembership);
}


int link_add(Node &n1, Node &n2);
Graph graph_load1(const char *fname);
Graph graph_load2(const char *fname);
Graph graph_new_load(const char *fname, bool calc=false);
GroupSet graph_group_read(Graph &,const char *fname);
GroupSet graph_community_read(Graph &g, const char *fname);
GroupSet loadCommunities(Graph &g, const char *fname);
Graph randomizeGraph(Graph graph,float pr);
void displayGraph(Graph g);
void dumpGraph(Graph graph, char* fileName);
void dumpGraph_CC(Graph graph, char* fileName);
void cleanUp(Graph& graph, char* dumpDir, char* nettype);
void dumpGraph4Metis(Graph graph, char* fileName);
void checkDegreeDistrib(Graph g1, Graph g2);

Graph BFS(int center,Graph g);
void RegionBFS(int center,Graph& g,bool relaxed, double& looseHalfEdges);

void checkRegions(Graph graph, char* partFile, int partFileType ,char* outFile, int partInfoIndex, bool measureLCC);
void fullCheckRegions(Graph graph, char* regionFile, int regionFileType, char* outFile, int partInfo);
Graph k_core_analysis(Graph g, int max_k);
Graph graphSynthesize(Graph g,float portion,float removal);

Graph graph_new_erdos(int n, int m);
Graph graph_new_ba(int n, int total_edges);
Graph graph_new_small_world(int n, int m, double p);
void readCoreFiles(Graph& graph, int & set_no, vector<int> & set_size,char* file);
void readCoreFile(Graph& graph, int & set_no, vector<int> & set_size,char* file);
void readPartFile(Graph& graph, int& set_no, vector<int> & set_size, char* file);
Graph readCommunityFile(Graph graph, int & set_no, vector<int> & set_size, char* file);

void readPartFileAndMakeGraph(Graph& graph, int & set_no, vector<int> & set_size, char* file);

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);

// Implemented in Group.cpp

void groupRegionEntropy(Graph& graph,GroupSet& allGroups, char* file);
void groupRegionEntropyPerGroup(Graph& graph,GroupSet& allGroups, char* file);
//void groupRegionMembershipCounter(Graph& graph,GroupSet& allGroups, char* file,double tresh);
void groupRegionMembershipCounter(Graph& graph,GroupSet allGroups, char* regionFile, char* outfile, int regionFileType, double tresh, int min_group_members_count, int max_group_members_count, int min_nodeDegree, int max_nodeDegree);
void groupMembershipDump(Graph& graph,GroupSet& allGroups, char* dumpdir, char* nettype);
void groupMembership(char* regionFile, char* groupFile, char* outFile);

void communityRegionMembershipCounter(Graph& graph, GroupSet groupSet, char* regionFile, char* outFile, int regionFileType); //, double tresh, int min_group_members_count, int max_group_members_count, int min_nodeDegree, int max_nodeDegree);
void testHighDegreeCoreNeighborhood(Graph& graph, GroupSet groupSet, char* regionFile, char* outFile, int regionFileType);
void communityProperties(Graph& graph, GroupSet groupSet, char* outFile);

double diffclock(unsigned clock1,unsigned clock2);
