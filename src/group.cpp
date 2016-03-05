#include "graph.h"


void groupRegionEntropy(Graph& graph, GroupSet& groupSet, char* file) {
    graph.calc();
    cout<<"Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";

    groupSet.setTotalPopulation(graph.nodesLst.size());

    //	int jumping_walkers=0;
    int set_no=0;
    vector<int> set_size;

    readCoreFiles(graph, set_no, set_size,file);

    cout<<"set count: "<<set_no<<endl;
    for (int i=0;i<set_size.size();i++) cout<<"set "<<i<<" size: "<<set_size[i]<<endl;

    cout<<"no groups: "<<groupSet.getGroupsSize()<<endl;
    double *H= new double[set_no];
    for (int i=0;i<set_no;i++)
    {
        GroupSet regionGroups(set_size[i],groupSet.getGroupsSize());
        H[i]=0;
        for (int userID=0;userID<graph.nodesLst.size();userID++)
        {
            int region=graph.nodesLst[userID].getRegion();
            if (region==i)
            {
                set<int>::iterator it;
                for ( it=graph.nodesLst[userID].groups.begin() ; it != graph.nodesLst[userID].groups.end(); it++ )
                {
                    regionGroups.addLogicalMembership(*it);
                }
            }
        }
        regionGroups.evalProbablities();
        for (int gid=0;gid<groupSet.getGroupsSize();gid++)
        {
            if ((0<regionGroups.getProbability(gid))&&(regionGroups.getProbability(gid)<1)) H[i]-=(regionGroups.getProbability(gid)*log2(regionGroups.getProbability(gid)))+((1-regionGroups.getProbability(gid))*log2(1-regionGroups.getProbability(gid)));
        }
    }
    groupSet.evalProbablities();
    double totalH=0;

    for (int gid=0;gid<groupSet.getGroupsSize();gid++)
    {
        if (groupSet.getProbability(gid)>0) totalH-=(groupSet.getProbability(gid)*log2(groupSet.getProbability(gid)))+((1-groupSet.getProbability(gid))*log2(1-groupSet.getProbability(gid)));
    }
    cout<<"H(v)="<<totalH<<";";
    for (int i=0;i<set_no;i++)
    {
        cout<<"\t"<<H[i];
    }
    cout<<"\n";
    double conditionalH=0;
    for (int i=0;i<set_no;i++)
    {
        conditionalH+=(set_size[i]*1.0/graph.noNodes)*H[i];
    }
    cout<<"H(v|c)="<<conditionalH<<endl;
    cout<<"Relative decrease in entropy: "<<1-conditionalH/totalH<<endl;
    return;
}


void groupRegionEntropyPerGroup(Graph& graph,GroupSet& groupSet, char* file)
{
    graph.calc();
    cout<<"Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";

    groupSet.setTotalPopulation(graph.nodesLst.size());

    int set_no=0;
    vector<int> set_size;

    readCoreFiles(graph, set_no, set_size,file);

    cout<<"no groups: "<<groupSet.getGroupsSize()<<endl;
    int groupsCount=0;//groupSet.getGroupsSize();
    //	vector<double> H=vector<double>(set_no,0);
    vector<groupEntropy> groupEntropyVec;
    for (int g=0;g<groupSet.getGroupsSize();g++)
    {
        //		double totalPr=0;
        vector<double> ThisGroupProbs=vector<double>(set_no,0);
        vector<double> HThisGroup=vector<double>(set_no,0);
        vector<int> groupUsers=groupSet.getGroupMembers(g);
        if (groupUsers.size()<100) continue;
        groupsCount++;
        for (int userNo=0;userNo<groupUsers.size();userNo++)
        {
            int userID = groupUsers[userNo];
            int  region = graph.nodesLst[userID].getRegion();
            ThisGroupProbs[region]++;
            //			totalPr++;
        }

        for (int i=0;i<set_no;i++)
        {
            ThisGroupProbs[i]=ThisGroupProbs[i]/set_size[i];
            if ((0<ThisGroupProbs[i])&&(ThisGroupProbs[i]<1)) HThisGroup[i]-=((ThisGroupProbs[i])*log2(ThisGroupProbs[i]))+((1-ThisGroupProbs[i])*log2(1-ThisGroupProbs[i]));
        }
        double totalPr=(groupUsers.size()*1.0)/graph.nodesLst.size();
        double totalH = -((totalPr)*log2(totalPr))-((1-totalPr)*log2(1-totalPr));
        //		cout<<"H(g"<<g<<")="<<totalH<<";";
        double conditionalH=0;
        for (int i=0;i<set_no;i++)
        {
            //			cout<<"\t"<<HThisGroup[i];
            conditionalH+=(set_size[i]*1.0/graph.nodesLst.size())*HThisGroup[i];
        }
        //		cout<<"\t--------- Group Population: "<<groupUsers.size();
        //		cout<<endl<<"H(g"<<g<<"|c)="<<conditionalH;
        //		cout<<endl<<"Relative decrease in entropy: "<<1-conditionalH/totalH<<endl;

        groupEntropyVec.push_back(groupEntropy(g,groupUsers.size(),totalH,conditionalH,1-conditionalH/totalH));
        //		if (g>20) exit(0);
    }
    sort(groupEntropyVec.begin(),groupEntropyVec.end(),relDecAcc);
    cout<<"groupID\tgroupPop\trelativeDecrease"<<endl;
    double unconditionalH=0,conditionalH=0;
    for (int g=0;g<groupEntropyVec.size();g++)
    {
        cout<<groupEntropyVec[g].groupID<<"\t"<<groupEntropyVec[g].groupPop<<"\t"<<groupEntropyVec[g].unconditional<<"\t"<<groupEntropyVec[g].conditional<<"\t"<<groupEntropyVec[g].relativeDecrease<<endl;
        unconditionalH+=groupEntropyVec[g].unconditional;
        conditionalH+=groupEntropyVec[g].conditional;
    }
    cout<<"\t--------- Group count: "<<groupEntropyVec.size()<<" from total "<<groupSet.getGroupsSize()<<" groups"<<endl;
    cout<<"H(V)="<<unconditionalH<<endl;
    cout<<"H(V|c)="<<conditionalH<<endl;
    cout<<"Relative decrease in entropy: "<<1-conditionalH/unconditionalH<<endl;
    return;
}


void groupMembership(char* regionFile, char* groupFile, char* outFile) {
    int regionCount=0;
    vector<int> regionSizes;

    Graph g;
    readPartFileAndMakeGraph(g,regionCount,regionSizes,regionFile);
}

void groupRegionMembershipCounter(Graph& graph, GroupSet groupSet, char* regionFile, char* outFile, int regionFileType, double tresh, int min_group_members_count, int max_group_members_count, int min_nodeDegree, int max_nodeDegree) {
    graph.calc();
    for (int userID=0;userID<graph.nodesLst.size();userID++) {
        graph.nodesLst[userID].count = graph.nodesLst[userID].neighbors->size();
        graph.nodesLst[userID].neighbors->clear();
    }
    cout<<"here"<<endl;
    //	for (int i; i<groupSet.groupsVec.size(); i++) {
    //		cout<<groupSet.groupsVec[i].members->size()<<endl;
    //	}
    //	exit(-1);

    cout<<"# Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"# Graph no edges: "<<graph.noEdges<<"\n";
    if (max_group_members_count==-1) max_group_members_count=INT_MAX;
    if (min_group_members_count==-1) min_group_members_count=0;
    if (max_nodeDegree==-1) max_nodeDegree=INT_MAX;
    if (min_nodeDegree==-1) min_nodeDegree=0;

    int set_no=0;
    vector<int> set_size;

    if (regionFileType==0) {
        cout<<"# Reading info file"<<endl;
        readCoreFiles(graph, set_no, set_size, regionFile);
    } else if (regionFileType==1) {
        cout<<"# Reading part file"<<endl;
        readPartFile(graph, set_no, set_size, regionFile);
    } else  if (regionFileType==2) {
        cout<<"# Reading community file"<<endl;
        graph=readCommunityFile(graph, set_no, set_size, regionFile);
    } else printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");

    int knownRegionsSize = 0;
    for (int i=0;i<set_no;i++)
        knownRegionsSize+=set_size[i];
    groupSet.setTotalPopulation(knownRegionsSize);

    cout<<"# no known nodes: "<<knownRegionsSize<<endl;
    cout<<"# no groups: "<<groupSet.getGroupsSize()<<endl;
    cout<<"# no groups: "<<groupSet.groupLst.size()<<endl;
    cout<<"# no sets: "<<set_no<<endl;

    //	for (int i=0; i<groupSet.groupsVec.size(); i++) {
    //		cout<<groupSet.groupsVec[i].members->size()<<endl;
    //	}

    vector<double> groupDeg(groupSet.groupLst.size(),0);
    int mappedGroup=0;
    for (int g=0; g<groupSet.groupLst.size(); g++) {
        vector<double> ThisGroupProbs=vector<double>(set_no,0);

        double groupMembersCount=0;
        for (int i=0; i<groupSet.groupLst[g].members->size(); i++) {
            int userID = groupSet.groupLst[g].members->at(i);
            groupDeg[g]+=graph.nodesLst[userID].count;

            if (graph.nodesLst[userID].count>max_nodeDegree) continue;
            if (graph.nodesLst[userID].count<min_nodeDegree) continue;
            int region = graph.nodesLst[userID].getRegion();

            if ( (region>=0) && (region<set_no) ) {
                ThisGroupProbs[region]++;
                groupMembersCount++;
            } else {
                //				cout<<region<<"\t"<<graph.nodesLst[userID].count<<endl;
            }
        }

        if (groupMembersCount>max_group_members_count) continue;
        if (groupMembersCount<min_group_members_count) continue;

        double maxMembershipProb=0;
        int embeddingRegion=-1;

        for (int i=0;i<set_no;i++) {
            ThisGroupProbs[i]=ThisGroupProbs[i]/groupMembersCount;
            if (ThisGroupProbs[i]>maxMembershipProb) {
                maxMembershipProb=ThisGroupProbs[i];
                embeddingRegion=i;
            }
        }
        if (embeddingRegion!=-1) {
            groupSet.setGroupRegion(g,embeddingRegion);
            groupSet.setProbability(g,maxMembershipProb);
            groupSet.setMappedMembersCount(g,groupMembersCount);
            mappedGroup++;
        }
    }

    cout<<"# All groups size: "<<groupSet.getGroupsSize()<<" groups..."<<endl;
    cout<<"# mapped groups: "<<mappedGroup<<" groups..."<<endl;


    //	sort(groupRegionMembershipVec.begin(),groupRegionMembershipVec.end(),memberAcc);
    //	cout<<"# groupID\tgroupPop\tmaxMembership"<<endl;
    //	int markedGroups=0;
    //	for (int g=0;g<groupRegionMembershipVec.size();g++) {
    //		cout<<"# "<<groupRegionMembershipVec[g].groupID<<"\t"<<groupRegionMembershipVec[g].groupPop<<"\t"<<groupRegionMembershipVec[g].maxRegionMembership<<endl;
    //		if(groupRegionMembershipVec[g].maxRegionMembership>=tresh) markedGroups++;
    //	}
    //	cout<<"# \t--------- Group count: "<<groupRegionMembershipVec.size()<<" from total "<<groupSet.getGroupsSize()<<" groups"<<endl;
    //	cout<<"# Groups with "<<(int)(tresh*100)<<"% users from same region: "<<markedGroups<<endl;

    //	cout <<"# sparse version"<<endl;
    //	for (int i=0;i<regionGrpMembership.size();i++) {
    //		for (int j=0;j<regionGrpMembership[i].size();j++) {
    //			cout <<"> 1\t" << i<<"\t"<<j<<"\t"<<regionGrpMembership[i][j]*100<<endl;
    //		}
    //	}
    //	for (int i=0;i<set_no;i++) {
    //		cout <<">>\t"<<i<<"\tR" <<i<<"\t"<<regionFilteredPop[i]/filteredPop<<"\t"<<regionFilteredPop[i]<<"\t"<<filteredPop<<endl;
    //	}
    //	cout<<endl;
    //
    //	cout<<"#Group\tEmbeddingRegion\tPop\tmappedPop\tConf\n";
    //	for (int g=0;g<groupSet.getGroupsSize();g++) {
    //		if(groupSet.getGroupRegion(g)!=-1) {
    //			cout<<groupSet.getGroupID(g)<<"\t"<<groupSet.getGroupRegion(g)<<"\t"<<groupSet.getGroupMembersCount(g)<<"\t"<<groupSet.getMappedMembersCount(g)<<"\t"<<groupSet.getProbability(g)<<endl;
    //		}
    //	}

    cout<<outFile<<endl;
    ofstream ofile;
    ofile.open(outFile);

    for (int g=0; g<groupSet.groupLst.size(); g++) {
        if (groupDeg[g]>0) {
            if(groupSet.groupLst[g].region!=-1) {
                ofile<<g<<"\t"<<groupSet.getGroupRegion(g)<<"\t"<<groupSet.getGroupMembersCount(g)<<"\t"<<groupSet.getMappedMembersCount(g)<<"\t"<<groupSet.getProbability(g)<<endl;
            } else  {
                cout<<"Unknown group. Group size "<<groupSet.groupLst[g].members->size()<<endl;
            }
        }
    }
    ofile.flush();
    ofile.close();
    cout<<"done"<<endl;
    return;
}


void communityRegionMembershipCounter(Graph& graph, GroupSet commSet, char* regionFile, char* outFile, int regionFileType) { //, double tresh, int min_group_members_count, int max_group_members_count, int min_nodeDegree, int max_nodeDegree) {
    graph.calc();
    for (int userID=0;userID<graph.nodesLst.size();userID++) {
        graph.nodesLst[userID].count = graph.nodesLst[userID].neighbors->size();
        //		graph.nodesLst[userID].neighbors->clear();
    }

    //	for (int i; i<groupSet.groupsVec.size(); i++) {
    //		cout<<groupSet.groupsVec[i].members->size()<<endl;
    //	}
    //	exit(-1);

    cout<<"# Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"# Graph no edges: "<<graph.noEdges<<"\n";
    //	if (max_group_members_count==-1) max_group_members_count=INT_MAX;
    //	if (min_group_members_count==-1) min_group_members_count=0;
    //	if (max_nodeDegree==-1) max_nodeDegree=INT_MAX;
    //	if (min_nodeDegree==-1) min_nodeDegree=0;

    int set_no=0;
    vector<int> set_size;

    if (regionFileType==0) {
        cout<<"# Reading info file"<<endl;
        readCoreFiles(graph, set_no, set_size, regionFile);
    } else if (regionFileType==1) {
        cout<<"# Reading part file"<<endl;
        readPartFile(graph, set_no, set_size, regionFile);
    } else printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");

    int knownRegionsSize = 0;
    for (int i=0;i<set_no;i++)
        knownRegionsSize+=set_size[i];
    commSet.setTotalPopulation(knownRegionsSize);

    cout<<"# no known nodes: "<<knownRegionsSize<<endl;
    cout<<"# no groups: "<<commSet.getGroupsSize()<<endl;
    cout<<"# no sets: "<<set_no<<endl;

    //	for (int i=0; i<groupSet.groupsVec.size(); i++) {
    //		cout<<groupSet.groupsVec[i].members->size()<<endl;
    //	}

    int mappedGroup=0;
    for (int g=0; g<commSet.groupLst.size(); g++) {
        vector<double> ThisGroupProbs=vector<double>(set_no,0);

        double groupMemmbersCount=0;
        for (int i=0; i<commSet.groupLst[g].members->size(); i++) {
            int userID = commSet.groupLst[g].members->at(i);

            //			if (graph.nodesLst[userID].count>max_nodeDegree) continue;
            //			if (graph.nodesLst[userID].count<min_nodeDegree) continue;
            int region = graph.nodesLst[userID].getRegion();

            if ( (region>=0) && (region<set_no) ) {
                ThisGroupProbs[region]++;
                groupMemmbersCount++;
            } else {
                // cout<<region<<"\t"<<graph.nodesLst[userID].count<<endl;
            }
        }

        //		if (groupMemmbersCount>max_group_members_count) continue;
        //		if (groupMemmbersCount<min_group_members_count) continue;

        double maxMembershipProb=0;
        int embeddingRegion=-1;

        for (int i=0;i<set_no;i++) {
            ThisGroupProbs[i]=ThisGroupProbs[i]/groupMemmbersCount;
            if (ThisGroupProbs[i]>maxMembershipProb) {
                maxMembershipProb=ThisGroupProbs[i];
                embeddingRegion=i;
            }
        }
        if (embeddingRegion!=-1) {
            commSet.setGroupRegion(g,embeddingRegion);
            commSet.setProbability(g,maxMembershipProb);
            commSet.setMappedMembersCount(g,groupMemmbersCount);
            mappedGroup++;
        }
    }

    cout<<"# All groups size: "<<commSet.getGroupsSize()<<" groups..."<<endl;
    cout<<"# mapped groups: "<<mappedGroup<<" groups..."<<endl;

    // int partCount=0;
    vector<double> commDeg(commSet.groupLst.size(),0);
    vector<double> commDegIntra(commSet.groupLst.size(),0);
    vector<double> commDegExtra(commSet.groupLst.size(),0);

    double graphDeg=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((i<0) || (i>graph.nodesLst.size())) {
            cout<<"Shit, integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcComm=graph.nodesLst[i].community;
        if (srcComm==0)
            cout<<"\t"<<srcComm<<" "<<graph.nodesLst[i].id<<" "<<graph.nodesLst[i].neighbors->size()<<endl;
        commDeg[srcComm]+=graph.nodesLst[i].neighbors->size();
        graphDeg+=graph.nodesLst[i].neighbors->size();
        for (unsigned int neiID=0;neiID<graph.nodesLst[i].neighbors->size();neiID++) {
            int nei=graph.nodesLst[i].neighbors->at(neiID);
            int dstComm=graph.nodesLst[nei].community;
            if  ((srcComm>=0) && (srcComm<commSet.groupLst.size())) { 	// src region in known
                if ((dstComm>=0) && (dstComm<commSet.groupLst.size())) { // dst region in known
                    if (srcComm==dstComm) {
                        commDegIntra[srcComm]++;
                    } else {
                        commDegExtra[srcComm]++;
                    }
                } else {
                    cout<<"out of range dst "<<dstComm<<" from node: "<<i<<endl;
                }
            } else {
                cout<<"out of range src "<<srcComm<<" from node: "<<nei<<endl;
            }
        }
    }

    cout<<outFile<<endl;
    ofstream ofile;
    ofile.open(outFile);

    for (int g=0; g<commSet.groupLst.size(); g++) {
        if(commSet.groupLst[g].region!=-1) {
            double module = (commDegIntra[g]/commDeg[g]) - (commDeg[g]/graphDeg);
            ofile<<g
                <<"\t"<<commSet.getGroupRegion(g)
               <<"\t"<<commSet.getGroupMembersCount(g)
              <<"\t"<<commSet.getMappedMembersCount(g)
             <<"\t"<<commSet.getProbability(g)
            <<"\t"<<module
            <<"\t"<<commDeg[g]/commSet.getGroupMembersCount(g)
            <<endl;
        } else if(commSet.groupLst[g].members->size()>0) {
            cout<<"Unknown group. Group size "<<commSet.groupLst[g].members->size()<<endl;
        }
    }
    ofile.flush();
    ofile.close();
    return;
}

void testHighDegreeCoreNeighborhood(Graph& graph, GroupSet communityList, char* regionFile, char* outFile, int regionFileType) {
    cout.precision(3);
    graph.calc();
    for (int userID=0;userID<graph.nodesLst.size();userID++) {
        graph.nodesLst[userID].count = graph.nodesLst[userID].neighbors->size();
    }
    cout<<"# Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"# Graph no edges: "<<graph.noEdges<<"\n";

    int partCount=0;
    vector<int> partSize;

    readPartFile(graph, partCount, partSize, regionFile);

    int knownRegionsSize = 0;
    for (int i=0;i<partCount;i++) {
        knownRegionsSize+=partSize[i];
    }
    communityList.setTotalPopulation(knownRegionsSize);

    cout<<"# known nodes: "<<knownRegionsSize<<endl;
    cout<<"# groups: "<<communityList.getGroupsSize()<<endl;
    cout<<"# parts: "<<partCount<<endl;

    vector <int> nodeCommunityEmbedRegion = vector <int> (graph.nodesLst.size(),-2);
    vector < vector <float> > nodeCommunityRegionProbVec = vector <vector <float> > (graph.nodesLst.size(),vector <float>(partCount,-1));
    vector <float> nodeCommunityEmbedProb = vector <float> (graph.nodesLst.size(),-2);

    int mappedCommunities=0;
    int connnetedCommunities=0;
    for (int g=0; g<communityList.groupLst.size(); g++) {
        //        if (g==12643) {
        //            for (int i=0; i<communityList.groupLst[g].members->size(); i++) {
        //                int userID = communityList.groupLst[g].members->at(i);
        //                int region = graph.nodesLst[userID].getRegion();
        //                cout<<userID<<"\t"<<region<<"\t"<<graph.nodesLst[userID].community<<endl;
        //                cin.get();
        //            }
        //            exit(-1);
        //        }

        vector<double> ThisGroupProbs=vector<double>(partCount,0);

        double groupMemmbersCount=0;
        double groupEdgeCount=0;
        for (int i=0; i<communityList.groupLst[g].members->size(); i++) {
            int userID = communityList.groupLst[g].members->at(i);
            int region = graph.nodesLst[userID].getRegion();
            groupEdgeCount+=graph.nodesLst[userID].count;

            if ( (region>=0) && (region<partCount) ) {
                ThisGroupProbs[region]++;
                groupMemmbersCount++;
            } else if (graph.nodesLst[userID].count>0) {
                cout<<"WTF\t"<<region<<"\t"<<graph.nodesLst[userID].count<<endl;
            }
        }

        double maxMembershipProb=0;
        int embeddingRegion=-1;

        for (int i=0;i<partCount;i++) {
            ThisGroupProbs[i]=ThisGroupProbs[i]/groupMemmbersCount;
            if (ThisGroupProbs[i]>maxMembershipProb) {
                maxMembershipProb=ThisGroupProbs[i];
                embeddingRegion=i;
            }
        }

        for (int i=0; i<communityList.groupLst[g].members->size(); i++) {
            int userID = communityList.groupLst[g].members->at(i);
            nodeCommunityEmbedRegion[userID] = embeddingRegion;
            nodeCommunityEmbedProb[userID] = maxMembershipProb;

            for (int p=0;p<partCount;p++) {
                nodeCommunityRegionProbVec[userID][p]=ThisGroupProbs[p];
            }
        }

        if(groupEdgeCount>0) {
            connnetedCommunities++;
            if (embeddingRegion!=-1) {
                mappedCommunities++;
            } else {
                cout<<"Community's No Users: "<< communityList.groupLst[g].members->size()<<"\t";
                cout<<"Community's No Users used for mapping: "<< groupMemmbersCount<<endl;
            }
        }
    }

    cout<<"Total Parts: "<<partCount<<endl;
    vector<int> sumCommMemMappedtoPart=vector<int>(partCount,0);
    int sumNodes=0;
    int notMappedNodes=0;
    for (unsigned int i=0; i<graph.nodesLst.size(); i++) {
        if (graph.nodesLst[i].neighbors->size()>0) {
            if ((graph.nodesLst[i].community==-1) || (nodeCommunityEmbedRegion[i]<0)) {
                cout<<i<<"\t"<<graph.nodesLst[i].getRegion()<<"\t"<<graph.nodesLst[i].community<<"\t"<<nodeCommunityEmbedRegion[i]<<endl;
            }
        }
        if (nodeCommunityEmbedRegion[i]>=0) {
            sumCommMemMappedtoPart[nodeCommunityEmbedRegion[i]]++;
            sumNodes++;
        } else if (graph.nodesLst[i].neighbors->size()>0) {
            notMappedNodes++;
        }
    }
    for (unsigned int i=0; i<partCount; i++) {
        cout<<i<<"\t"<<sumCommMemMappedtoPart[i]<<"\t"<<sumCommMemMappedtoPart[i]*100.0/sumNodes<<endl;
    }
    cout<<"unmapped"<<"\t"<<notMappedNodes<<endl;

    cout<<"# All Community Count: "<<connnetedCommunities<<" Community"<<endl;
    cout<<"# Mapped Communities: "<<mappedCommunities<<" Community"<<endl;

    int keep=100;
    vector< vector< MiniNode > > highDegreeCore = vector< vector< MiniNode > >(partCount,vector < MiniNode >());

    for (unsigned int i=0; i<graph.nodesLst.size(); i++) {
        int region = graph.nodesLst[i].getRegion();
        int degree = graph.nodesLst[i].neighbors->size();

        if(degree>0) {
            highDegreeCore[region].push_back(MiniNode(i,degree));
            //            cout<<i<<"\t"<<degree<<"\t"<<nodeCommunityEmbedRegion[i]<<"\t"<<region<<"\t"<<endl;
            if ((0>region) || (region>=partCount)) {
                cout<<"WTF\t"<<region<<"\t"<<i<<endl;
            }
        }

        for (int region=0;region<partCount;region++) {
            if (highDegreeCore[region].size()>=keep*2) {
                sort(highDegreeCore[region].begin(),highDegreeCore[region].end(),degreeCompare);
                //                highDegreeCore[region].erase(highDegreeCore[region].begin()+keep,highDegreeCore[region].end());
                highDegreeCore[region].resize(keep);
            }
        }
    }


    for (int region=0;region<partCount;region++) {
        sort(highDegreeCore[region].begin(),highDegreeCore[region].end(),degreeCompare);
        //        highDegreeCore[region].erase(highDegreeCore[region].begin()+keep,highDegreeCore[region].end());
        highDegreeCore[region].resize(keep);

        cout<<"part("<<region<<"):"<<highDegreeCore[region].size()<<endl;
        for (vector<MiniNode>::iterator it = highDegreeCore[region].begin() ; it != highDegreeCore[region].end(); ++it) {
            int nodeId=it->id;
            cout<<nodeId<<"("<<it->degree<<");";
        }
        //        for (unsigned int member=0;member<highDegreeCore[region].size();member++) {
        //            cout<<highDegreeCore[region][member].id;<<"("<<highDegreeCore[region][member].degree<<");";
        //        }
        cout<<endl;
    }

    //	vector <int> resCoreRegion = vector <int>();
    //	vector <int> resNighRegion = vector <int>();
    //	vector <float> resConfidence = vector <float>();
    ofstream ofile;
    ofile.open(outFile);

    //	vector <int> coreNeighbors = vector<int>(partCount,0);
    //	vector <int> coreNeighborsInSameRegion = vector<int>(partCount,0);

    cout<<"# Core Neighbors in Communities mapped to the same region"<<endl;
    for (int region=0;region<partCount;region++) {
        float coreNeighbors = 0;
        float coreNeighborsInSameRegion = 0;
        float coreNeighborsInCommMappedToSameRegion = 0;
        for (unsigned int i=0;i<highDegreeCore[region].size();i++) {
            int nodeId=highDegreeCore[region][i].id;
            for (unsigned int nei=0;nei<graph.nodesLst[nodeId].neighbors->size();nei++) {
                int neiId=graph.nodesLst[nodeId].neighbors->at(nei);
                //                ofile << nodeId <<"\t" << neiId << "\t" << graph.nodesLst[nodeId].getRegion() << "\t" << graph.nodesLst[neiId].getRegion() << "\t" << graph.nodesLst[nodeId].community << "\t" << graph.nodesLst[neiId].community << "\t" <<
                //                      nodeCommunityEmbedRegion[neiId] << "\t" << nodeCommunityEmbedProb[neiId] << "\t" << nodeCommunityRegionProbVec[neiId][graph.nodesLst[nodeId].getRegion()] << endl;
                ofile << graph.nodesLst[nodeId].getRegion() << "\t" << nodeCommunityRegionProbVec[neiId][graph.nodesLst[nodeId].getRegion()] << "\n" <<
                         coreNeighbors++;
                if (region==nodeCommunityEmbedRegion[neiId]) {
                    coreNeighborsInCommMappedToSameRegion++;
                }
                if (region==graph.nodesLst[neiId].getRegion()) {
                    coreNeighborsInSameRegion++;
                }
            }
        }
        cout << region << "\t" << coreNeighbors << "\t" << coreNeighborsInCommMappedToSameRegion<<"\t"<<(coreNeighborsInCommMappedToSameRegion*100.0)/(coreNeighbors)<<"\t" << coreNeighborsInSameRegion<<"\t"<<(coreNeighborsInSameRegion*100.0)/(coreNeighbors)<<endl;
        cout.flush();
    }
    ofile.flush();
    ofile.close();
    return;
}

void groupMembershipDump(Graph& graph,GroupSet& groupSet, char* dumpdir, char* nettype) {
    char fname[128];
    ofstream ofile;
    sprintf(fname,"%s/%s_GroupPop.txt",dumpdir,nettype);
    ofile.open((const char*)fname,ios::out);
    ofile<<"#Group_id\t#UsersCnt\n";
    for (int grp=0;grp<groupSet.getGroupsSize();grp++) {
        ofile<<grp<<"\t"<<groupSet.getGroupMembersCount(grp)<<endl;
    }
    ofile.close();
    sprintf(fname,"%s/%s_UserGrpCnt.txt",dumpdir,nettype);
    ofile.open((const char*)fname,ios::out);
    ofile<<"#node_id\t#NeiCnt\t#GroupsCnt\n";
    for (int node=0;node<graph.nodesLst.size();node++)
    {
        ofile<<graph.nodesLst[node].id<<"\t"<<graph.nodesLst[node].neighbors->size()<<"\t"<<graph.nodesLst[node].groupsCount<<endl;
    }
}

void communityProperties(Graph& graph, GroupSet commSet, char* outFile) { //, double tresh, int min_group_members_count, int max_group_members_count, int min_nodeDegree, int max_nodeDegree) {
    graph.calc();
    for (int userID=0;userID<graph.nodesLst.size();userID++) {
        graph.nodesLst[userID].count = graph.nodesLst[userID].neighbors->size();
    }

    cout<<"# Graph no nodes: "<<graph.nodesLst.size()<<"\n";
    cout<<"# Graph no edges: "<<graph.noEdges<<"\n";

    cout<<"# no communities: "<<commSet.getGroupsSize()<<endl;

    cout<<"# All groups size: "<<commSet.getGroupsSize()<<" groups..."<<endl;

    vector<double> commDeg(commSet.groupLst.size(),0);
    vector<double> commDegIntra(commSet.groupLst.size(),0);
    vector<double> commDegExtra(commSet.groupLst.size(),0);

    double graphDeg=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((i<0) || (i>graph.nodesLst.size())) {
            cout<<"Shit, integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcComm=graph.nodesLst[i].community;
        if (srcComm==0)
            cout<<"\t"<<srcComm<<" "<<graph.nodesLst[i].id<<" "<<graph.nodesLst[i].neighbors->size()<<endl;
        commDeg[srcComm]+=graph.nodesLst[i].neighbors->size();
        graphDeg+=graph.nodesLst[i].neighbors->size();
        for (unsigned int neiID=0;neiID<graph.nodesLst[i].neighbors->size();neiID++) {
            int nei=graph.nodesLst[i].neighbors->at(neiID);
            int dstComm=graph.nodesLst[nei].community;
            if  (srcComm>=0) {
                if (dstComm>=0) {
                    if (srcComm==dstComm) {
                        commDegIntra[srcComm]++;
                    } else {
                        commDegExtra[srcComm]++;
                    }
                } else {
                    cout<<"out of range dst "<<dstComm<<" from node: "<<i<<endl;
                }
            } else {
                cout<<"out of range src "<<srcComm<<" from node: "<<nei<<endl;
            }
        }
    }

    cout<<outFile<<endl;
    ofstream ofile;
    ofile.open(outFile);

    for (int g=0; g<commSet.groupLst.size(); g++) {
        if (commSet.groupLst[g].members->size()==0)
            continue;
        double module = (commDegIntra[g]/commDeg[g]) - (commDeg[g]/graphDeg);
        ofile<<g
            <<"\t"<<commSet.groupLst[g].members->size()
            <<"\t"<<module
            <<"\t"<<commSet.getGroupMembersCount(g)
            <<"\t"<<commDeg[g]/commSet.getGroupMembersCount(g)
            <<endl;
//        cout<<g
//            <<"\t"<<commSet.groupLst[g].members->size()
//            <<"\t"<<module
//            <<"\t"<<commSet.getGroupMembersCount(g)
//            <<"\t"<<commDeg[g]/commSet.getGroupMembersCount(g)
//            <<endl;
    }
    ofile.flush();
    ofile.close();
    return;
}
