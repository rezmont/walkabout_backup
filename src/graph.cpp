#include "graph.h"

// #define snow_num_groups 10000
// #define CORRELATION_SAMPLES 1000000

double diffclock(unsigned clock1,unsigned clock2) {
    double diffticks=clock1-clock2;
    double diffms=(diffticks)/CLOCKS_PER_SEC;
    return(diffms);
}

Graph graphSynthesizeII(Graph g,float portion,float bridge_link_reduce) {
    /*	int offset=0;
    int last=g.nodesLst.size()*portion;
    int half_edges_cnt=0;
    for(int i=offset;i<last;i++) {
        half_edges_cnt+=g.nodesLst[i].neighbors->size();
        g.nodesLst[i].neighbors->clear();
    }
    printf("# last: %d\n",last);
    while(half_edges_cnt%2!=0)
    {
        half_edges_cnt+=g.nodesLst[last].neighbors->size();
        g.nodesLst[last].neighbors->clear();
        last++;
    }
    printf("# last: %d, # of half edges: %d\n",last,half_edges_cnt);

    while(half_edges_cnt>0)
    {
        if ((half_edges_cnt%10000)==0) printf("# of half edges: %d\n",half_edges_cnt);

        int a=rand()%last;
        while(g.nodesLst[a].neighbors->size()>=g.nodesLst[a].degree) {
            a++;
            a=a%last;
        }

        int 	b = rand()%last;
        int 	b_tmp = b;
        while((g.nodesLst[b].neighbors->size()>=g.nodesLst[b].degree)||(a==b)||(g.nodesLst[a].getNeighborIdx(b)!=-1))
        {
            b++;
            b=b%last;
            if (b_tmp==b)
            {
                int cnd=rand()%last;
                while(g.nodesLst[cnd].neighbors->size()==0)
                {
                    cnd=rand()%last;
                }
                int nei = g.nodesLst[cnd].neighbors[rand()%g.nodesLst[cnd].neighbors->size()];
                g.nodesLst[nei].removeNeighbor(cnd);
                g.nodesLst[cnd].removeNeighbor(nei);

                half_edges_cnt+=2;
            }
        }
        link_add(g.nodesLst[a],g.nodesLst[b]);
        half_edges_cnt-=2;
    }

    printf("1st half finished\n");

    offset=last;
    half_edges_cnt=0;
    last=g.nodesLst.size();
    for(int i=offset;i<last;i++) {
        g.nodesLst[i].degree=g.nodesLst[i].neighbors->size();
        g.nodesLst[i].neighbors->clear();
        half_edges_cnt+=g.nodesLst[i].degree;
    }

    printf("last: %d, # of half edges: %d\n",last,half_edges_cnt);

    while(half_edges_cnt>0) {
        if ((half_edges_cnt%10000)==0) printf("# of half edges: %d\n",half_edges_cnt);

        int a=offset+(rand()%(last-offset));
        while(g.nodesLst[a].neighbors->size()>=g.nodesLst[a].degree)
        {
            a++;
            a=offset+((a-offset)%(last-offset));
        }

        int b=offset+(rand()%(last-offset));
        int b_tmp = b;
        while((g.nodesLst[b].neighbors->size()>=g.nodesLst[b].degree)||(a==b)||(g.nodesLst[a].getNeighborIdx(b)!=-1))
        {
            b++;
            b=offset+((b-offset)%(last-offset));
            if (b_tmp==b)
            {
                int cnd=offset+(rand()%(last-offset));
                while(g.nodesLst[cnd].neighbors->size()==0)
                {
                    cnd=offset+(rand()%(last-offset));
                }
                int nei = g.nodesLst[cnd].neighbors[rand()%g.nodesLst[cnd].neighbors->size()];
                g.nodesLst[nei].removeNeighbor(cnd);
                g.nodesLst[cnd].removeNeighbor(nei);

                half_edges_cnt+=2;
            }
        }
        link_add(g.nodesLst[a],g.nodesLst[b]);
        half_edges_cnt-=2;
    }
    printf("2st half finished\n");*/
    return g;
}


Graph graphSynthesize(Graph graph,float portion,float bridge_link_reduce) {
    int frst=graph.nodesLst.size()*portion;
    int scnd=graph.nodesLst.size();
    int bridgeWidth=0;
    for(int i=0;i<frst;i++) {
        for (unsigned int j=0;j<graph.nodesLst[i].neighbors->size();j++)
            if (graph.nodesLst[i].neighbors->at(j) > frst)
                bridgeWidth++;
    }
    printf("# 1st: %d;\t# 2st: %d;\t# bridgeWidth: %d;\n",frst,scnd,bridgeWidth);

    // map <int,Link> graph_links;
    int max_degree=0;
    int id=0;
    for(int i=0;i<graph.nodesLst.size();i++)
    {
        if (max_degree<graph.nodesLst[i].neighbors->size()) max_degree=graph.nodesLst[i].neighbors->size();
    }

    int final_no_rewire=bridge_link_reduce*bridgeWidth;
    int rewired=0;
    printf("# Ultimate number of rewirings: %d\n",final_no_rewire);
    for(rewired=0;rewired<final_no_rewire;rewired+=2)
    {
        if (rewired%100000==0) printf("number of rewiring performed: %d\n",rewired);
        // for(int nodeA_neiID=0;nodeA_neiID<graph.nodesLst[nodeA].neighbors->size();nodeA_neiID++)
        // {
        // lnk1=rand()%graph_links.size();

        int nodeA=-1;
        int nodeB=-1;
        int nodeC=-1;
        int nodeD=-1;

        int nodeA_neiID=-1;
        int nodeC_neiID=-1;

        float acceptance_prob;//=(float)rand()/(float)RAND_MAX;
        float accept;//=(((float)graph.noNodes)*(graph.nodesLst[nodeA].neighbors->size()))/((float)graph.noEdges);

        bool foundLink=false;
        do
        {
            do{
                nodeA=rand()%frst;//graph.nodesLst.size();
                double acceptance_prob=(double)rand()/(double)RAND_MAX;
                double accept=(((double)graph.nodesLst[nodeA].neighbors->size())/((double)max_degree));
                // printf("%f\t>?\t%f\n",acceptance_prob,accept);
                // getchar();
            }while((acceptance_prob<accept)||(graph.nodesLst[nodeA].neighbors->size()==0));
            int tried=10;
            do{
                nodeA_neiID=rand()%graph.nodesLst[nodeA].neighbors->size();
                nodeB=graph.nodesLst[nodeA].neighbors->at(nodeA_neiID);
                tried--;
            }while((nodeB>frst)&&(tried>0));
            if (nodeB<frst) foundLink=true;
        }while (foundLink==false);

        do
        {
            bool foundLink=false;
            do
            {
                do{
                    nodeC=(rand()%(scnd-frst))+frst;
                    double acceptance_prob=(double)rand()/(double)RAND_MAX;
                    double accept=(((double)graph.nodesLst[nodeC].neighbors->size())/((double)max_degree));
                }while((acceptance_prob<accept)||(graph.nodesLst[nodeC].neighbors->size()==0)||(graph.nodesLst[nodeA].getNeighborIdx(nodeC)!=-1));

                int tried=10;
                do{
                    int nodeC_neiID=rand()%graph.nodesLst[nodeC].neighbors->size();
                    nodeD=graph.nodesLst[nodeC].neighbors->at(nodeC_neiID);
                    tried--;
                }while((nodeD<frst)&&(tried>0));
                if (nodeD>frst) foundLink=true;
            }while (foundLink==false);

        }while((graph.nodesLst[nodeB].getNeighborIdx(nodeD)!=-1));

        // if (no_try>=10) break;

        graph.nodesLst[nodeA].removeNeighbor(nodeB);
        graph.nodesLst[nodeB].removeNeighbor(nodeA);
        graph.nodesLst[nodeC].removeNeighbor(nodeD);
        graph.nodesLst[nodeD].removeNeighbor(nodeC);

        graph.nodesLst[nodeA].addNeighbor(nodeC);
        graph.nodesLst[nodeC].addNeighbor(nodeA);
        graph.nodesLst[nodeB].addNeighbor(nodeD);
        graph.nodesLst[nodeD].addNeighbor(nodeB);
    }

    printf("Synthesized: total rewiring performed: %d;\t number of changed links: %d\n",rewired,rewired*2);
    return graph;
}



GroupSet loadCommunities(Graph &graph, const char *fname) {
    GroupSet communityList;
    string str;
    ifstream inFile;
    inFile.open(fname);
    if(!inFile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    int node, community;
    while(!inFile.eof()) {
        inFile >> node >> community;

        int userId=node;
        int commId=community;

        // cout<<userId<<"\t"<<commId<<endl;
        if (graph.nodesLst[userId].neighbors->size()==0) continue;
        graph.nodesLst[userId].community=commId;

        int gId=communityList.groupLst.size();
        while (communityList.groupLst.size()<=commId) {
            communityList.groupLst.push_back(Group(gId));
            gId++;
        }
        communityList.groupLst[commId].members->push_back(userId);

        //        cout<<commId<<": ";
        //        for (int i=0; i<communityList.groupLst[commId].members->size()-1; i++) {
        //            int uID = communityList.groupLst[commId].members->at(i);
        //            cout<<uID<<",";
        //        }
        //        int uID = communityList.groupLst[commId].members->at(communityList.groupLst[commId].members->size()-1);
        //        cout<<uID<<endl;
        //        cin.get();
    }
    inFile.close();
    //    for (int g=0; g<communityList.groupLst.size(); g++) {
    //        if (g==12643) {
    //            for (int i=0; i<communityList.groupLst[g].members->size(); i++) {
    //                int userID = communityList.groupLst[g].members->at(i);
    //                int region = graph.nodesLst[userID].getRegion();
    //                cout<<userID<<"\t"<<region<<"\t"<<graph.nodesLst[userID].community<<endl;
    //                cin.get();
    //            }
    //            exit(-1);
    //        }
    //    }
    return communityList;
}


Graph readCommunityFile(Graph graph, int & partCount, vector<int> & partSize, char* fname) {
    ifstream inFile;
    inFile.open(fname);
    if(!inFile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    int node, community;
    while(!inFile.eof()) {
        inFile >> node >> community;

        int id=node;
        int commId=community;
        int region = commId;
        int degree = graph.nodesLst[id].neighbors->size();
        if (region>-1) {
            graph.nodesLst[id].setRegion(region);

            partCount = max(partCount,region);
            while (partSize.size()<=region) {
                partSize.push_back(0);
            }
            partSize[region]++;
        } else if (degree>0) {
            cout<<"WTF!!! Something went wrong while reading the part file: "<<id<<"\t"<<degree<<"\t"<<region<<endl;
        }
    }

    //check here
    bool unknownSeen=false;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((graph.nodesLst[i].getRegion()==-1) && (graph.nodesLst[i].neighbors->size()>0)) {
            if (!unknownSeen) {
                cout<<"# Still some unknown nodes seen"<<endl;
                unknownSeen=true;
                partCount++;
            }
            graph.nodesLst[i].setRegion(partCount);
        }
    }
    cout<<"# Max Set Index: "<<partCount<<endl;
    partCount++;
    return graph;
}


GroupSet graph_group_read(Graph &g, const char *fname) {
    GroupSet gs;
    string str;
    ifstream infile;
    infile.open(fname);
    if(!infile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    while(!infile.eof()) {
        tkns.clear();
        str.clear();
        getline(infile,str);
        Tokenize (str,tkns,"\t");

        int userId=atoi(tkns[0].c_str());
        int groupId=atoi(tkns[1].c_str());

        g.nodesLst[userId].addGroup();
        gs.addMembership(groupId,userId);
    }
    infile.close();
    return gs;
}

GroupSet graph_community_read(Graph &g, const char *fname) {
    GroupSet gs;
    string str;
    ifstream infile;
    infile.open(fname);
    if(!infile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    while(!infile.eof()) {
        tkns.clear();
        str.clear();
        getline(infile,str);
        Tokenize (str,tkns,"\t");

        int userId=atoi(tkns[0].c_str());
        int commId=atoi(tkns[1].c_str());

        g.nodesLst[userId].addGroup();
        g.nodesLst[userId].community=commId;
        gs.addMembership(commId,userId);
    }
    infile.close();
    return gs;
}

void readCoreFiles(Graph& graph, int & coreCount, vector<int> & coreSize,char* file) {
    string str_fname;
    string str;
    string full_fname;

    ifstream infofile;
    infofile.open(file);
    if(!infofile) {
        cout << "Cannot open infile_name: "<<file<<".\n";
        exit(-1);
    }

    int nodeId=0;
    while(!infofile.eof()) {
        str_fname.clear();
        getline(infofile,str_fname);
        if (str_fname.size()==0) continue;
        full_fname = str_fname;
        ifstream infile;
        infile.open(full_fname.c_str());
        if(!infile) {
            cout << "Cannot open set file ("<<coreCount<<"): "<<full_fname<<".\n";
            break;
        } else {
            coreSize.push_back(0);
            vector <string> tkns;
            unsigned int maxDegree=0;
            while(!infile.eof()) {
                str.clear();
                getline(infile,str);
                tkns.clear();
                Tokenize (str,tkns,"\t");
                if (tkns.size()) {
                    int current=atoi(tkns[0].c_str());
                    coreSize.back()++;
                    graph.nodesLst[current].setRegion(coreCount);
                    graph.nodesLst[current].auxID=nodeId;
                    if (maxDegree<graph.nodesLst[current].neighbors->size())
                        maxDegree = graph.nodesLst[current].neighbors->size();
                    nodeId++;
                    //					if (coreCount==7) {
                    //						cout<<str<<" -> "<<current<<"\t"<<graph.nodesLst[current].neighbors->size()<<"\t"<<graph.nodesLst[current].getRegion()<<"\n";
                    //						cin.get();
                    //					}
                }
            }
            cout << "#core("<<coreCount<<")\t"<<str_fname<<"'s size:\t"<<coreSize[coreCount]<<"\t"<<coreSize[coreCount]*100.0/graph.noNodes<<"\t"<<maxDegree<<"\n";
            coreCount++;
        }
        infile.close();
    }
    infofile.close();
    coreSize.push_back(0);
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((graph.nodesLst[i].getRegion()==-1)) {//&&(graph.nodesLst[i].neighbors->size()>0))
            if (graph.nodesLst[i].neighbors->size()>0) coreSize.back()++;
            graph.nodesLst[i].setRegion(coreCount);
        }
    }
    cout << "#core("<<coreCount<<")\t"<<"UNKNOWNS"<<"'s size:\t"<<coreSize[coreCount]<<"\t"<<coreSize[coreCount]*100.0/graph.noNodes<<"\n";
    coreCount++;

    return;
}


void readPartFileAndMakeGraph(Graph &graph , int & set_no, vector<int> & partSize, char* file) {
    string str_fname;
    string str;

    ifstream partFile;
    partFile.open(file);
    if(!partFile) {
        cout << "Cannot open infile_name: "<<file<<".\n";
        exit(-1);
    }

    vector <string> tkns;
    while(!partFile.eof()) {
        str.clear();
        getline(partFile,str);
        tkns.clear();
        Tokenize (str,tkns,"\t");
        if (tkns.size()) {
            int id = atoi(tkns[0].c_str());
            int degree = atoi(tkns[1].c_str());
            int region =-1;
            region = atoi(tkns[2].c_str());

            int prevNoNodes=graph.nodesLst.size();
            if (id>=prevNoNodes) {
                for (int i=prevNoNodes;i<=id;i++) {
                    graph.nodesLst.push_back(Node(i));
                }
            }
            if (region>-1) {
                graph.nodesLst[id].setRegion(region);

                set_no = max(set_no,region);
                while (partSize.size()<=region) {
                    partSize.push_back(0);
                }
                partSize[region]++;
            } else if (degree>0) {
                cout<<"WTF!!! Something went wrong while reading the part file: "<<id<<"\t"<<degree<<"\t"<<region<<endl;
            }
        }
    }
}


void readCoreFile(Graph& graph, int& partCount, vector<int>& partSize, char* file) {
    string str;

    ifstream partFile;
    partFile.open(file);
    if(!partFile) {
        cout << "Cannot open infile_name: "<<file<<".\n";
        exit(-1);
    }

    int nodeId=0;
    vector <string> tkns;
    while(!partFile.eof()) {
        str.clear();
        getline(partFile,str);
        tkns.clear();
        Tokenize (str,tkns,"\t");
        if (tkns.size()) {
            int id = atoi(tkns[0].c_str());
            int degree = atoi(tkns[1].c_str());
            graph.nodesLst[id].auxID=nodeId;
            nodeId++;

            int region =-1;
            region = atoi(tkns[2].c_str());
            if (region>-1) {
                graph.nodesLst[id].setRegion(region);

                partCount = max(partCount,region);
                while (partSize.size()<=region) {
                    partSize.push_back(0);
                }
                partSize[region]++;
            } else if (degree>0) {
                cout<<"WTF!!! Something went wrong while reading the part file: "<<id<<"\t"<<degree<<"\t"<<region<<endl;
            }
        }
    }
    // check here
    partCount++;
    return;
}


void readPartFile(Graph& graph, int& partCount, vector<int> & partSize, char* file) {
    bool debug = false;
    string str;

    ifstream partFile;

    partFile.open(file);
    if(!partFile) {
        cout << "Cannot open part file: "<<file<<".\n";
        exit(-1);
    } else {
        cout<<"Part-file name: "<<file<<endl;
    }

    vector <string> tkns;
    while(!partFile.eof()) {
        str.clear();
        getline(partFile, str);
        if(str[0] == '#') {
            continue;
        }
        tkns.clear();
        Tokenize(str, tkns, "\t");
        if (tkns.size()) {
            int id = atoi(tkns[0].c_str());
            int degree = atoi(tkns[1].c_str());
            int region =-1;
            region = atoi(tkns[2].c_str());

            if (debug) {
                cout<<id<<"\t"<<degree<<"\t"<<region<<endl;
            }

            if (region>-1) {
                graph.nodesLst[id].setRegion(region);

                partCount = max(partCount,region);
                while (partSize.size()<=region) {
                    partSize.push_back(0);
                }
                partSize[region]++;
            } else if (degree>0) {
                cout<<"!!WTF!!! Something went wrong while reading the part file: "<<id<<"\t"<<degree<<"\t"<<region<<endl;
                cout<<"!! "<<str<<endl;
            }
        }
    }

    //check here
    bool unknownSeen=false;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((graph.nodesLst[i].getRegion()==-1) && (graph.nodesLst[i].neighbors->size()>0)) {
            if (!unknownSeen) {
                cout<<"# Still some unknown nodes seen"<<endl;
                unknownSeen=true;
                partCount++;
            }
            graph.nodesLst[i].setRegion(partCount);
        }
    }
    cout<<"# Max Set Index: "<<partCount<<endl;
    partCount++;
    return;
}




//Graph readPartFileAndReturnRegions(Graph graph, int & partCount, vector<int> & partSize, vector< vector <int> > & partMember , char* file, vector<int> & partNames, int partInfoIndex) {
Graph readPartFileAndReturnRegions(Graph graph, int & partCount, vector<int> & partSize, vector< vector <int> > & partMember , char* file, int partInfoIndex) {
    string str_fname;
    string str;

//    cout << partInfoIndex << endl;

    ifstream partFile;
    partFile.open(file);
    if(!partFile) {
        cout << "Cannot open infile_name: "<<file<<".\n";
        exit(-1);
    }
    //	map <int, int> mapPartNamesIndex;
    vector <string> tkns;
    while(!partFile.eof()) {
        str.clear();
        getline(partFile,str);
        if( str[0] =='#') continue;
        tkns.clear();
        Tokenize (str,tkns,"\t");
        if (tkns.size()) {
            int id = atoi(tkns[0].c_str());
            int degree = graph.nodesLst[id].neighbors->size();

            int region =-1;
            region = atoi(tkns[partInfoIndex].c_str());
//            cout << region << endl;
            if (region>-1) {
                graph.nodesLst[id].setRegion(region);
                while (partSize.size()<=region) {
                    partSize.push_back(0);
                    partMember.push_back(vector<int>());
                }
                partSize[region]++;
                partMember[region].push_back(id);

                partCount = partSize.size();
            } else if (degree>0) {
                cout<<"WTF!!! Something went wrong while reading the part file: "<<id<<"\t"<<degree<<"\t"<<region<<endl;
            }
            //			int id = atoi(tkns[0].c_str());
            //			int region =-1;
            //			int part=atoi(tkns[partInfoIndex].c_str());
            //			if ( mapPartNamesIndex.find( part ) == mapPartNamesIndex.end() ) { /* not found in the already seen regions */
            //				region = partNames.size();
            //				mapPartNames[part]=region;
            //				partNames.push_back(atoi(tkns[partInfoIndex].c_str()));
            //			} else { /* found */
            //				region = mapPartNames[part];
            //			}
            //			if (region>-1) {
            //				graph.nodesLst[id].setRegion(region);
            //			} else {
            //				cout<<"WTF!!! Something went wrong while reading the part file\n";
            //				cout<<"Exiting"<<endl;
            //				exit(-1);
            //			}

            //			partCount = partNames.size();
            //
            //			while (partMember.size()<=region) {
            //				partMember.push_back(vector<int>());
            //				partSize.push_back(0);
            //			}
            //			partMember[region].push_back(id);
            //			partSize[region]++;
        }
    }

    //check here
    bool unknownSeen=false;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((graph.nodesLst[i].getRegion()==-1)&&(graph.nodesLst[i].neighbors->size()>0)) {
            if (!unknownSeen) {
                cout<< "Still some unknown nodes seen"<<endl;
                cout<< "Node ID: "<<i<<endl;
                unknownSeen=true;
                partCount++;
            } else {
                cout<< "Node ID: "<<i<<endl;
            }
            graph.nodesLst[i].setRegion(partCount);
            partMember.back().push_back(i);
            partSize.back()++;
        }
    }
    return graph;
}


Graph markNodeRegionAndReturnRegions(Graph graph, int & set_no, vector<int> & partSize, vector< vector <int> > & sets , char* file) {
    string str_fname;
    string str;
    string path("");
    string full_fname;

    ifstream infoFile;
    infoFile.open(file);
    if(!infoFile) {
        cout << "Cannot open infile_name: "<<file<<".\n";
        exit(-1);
    }

    while(!infoFile.eof()) {
        str_fname.clear();
        getline(infoFile,str_fname);
        if (str_fname.size()==0) continue;
        full_fname = path + str_fname;
        ifstream inFile;
        inFile.open(full_fname.c_str());
        if(!inFile) {
            cout << "Cannot open set file ("<<set_no<<"): "<<full_fname<<".\n";
            break;
        } else {
            sets.push_back(vector<int>());
            partSize.push_back(0);
            vector <string> tkns;
            while(!inFile.eof()) {
                str.clear();
                getline(inFile,str);
                tkns.clear();
                Tokenize (str,tkns,"\t");
                if (tkns.size()) {
                    int current=atoi(tkns[0].c_str());
                    partSize.back()++;
                    graph.nodesLst[current].setRegion(set_no);
                    sets.back().push_back(current);
                }
            }
            cout << "#set("<<set_no<<")\t"<<str_fname<<"'s size:\t"<<partSize[set_no]<<"\n";
            set_no++;
        }
        inFile.close();
    }
    infoFile.close();

    bool seenUnknown=false;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((graph.nodesLst[i].getRegion()==-1) && (graph.nodesLst[i].neighbors->size()>0)) {
            if (!seenUnknown) {
                sets.push_back(vector<int>());
                partSize.push_back(0);
            }
            seenUnknown = true;
            graph.nodesLst[i].setRegion(set_no);
            sets.back().push_back(i);
            if(graph.nodesLst[i].neighbors->size()>0) partSize.back()++;
        }
    }
    cout << "#set("<<set_no<<")\t"<<"UNKNOWNS"<<"'s size:\t"<<partSize[set_no]<<"\n";
    if (seenUnknown) {
        set_no++;
    }
    return graph;
}

Graph graph_load1(const char *fname) {
    FILE *fh = fopen(fname, "r");
    int edge[2];
    int nodes = 0;
    char *line=NULL;
    size_t linesize;
    // struct graph *g;
    Graph graph;

    while (getline(&line, &linesize, fh)>0) {
        if (sscanf(line,"%d %d", &edge[0], &edge[1])==2 ) {
            if (nodes < edge[0]) nodes=edge[0];
            if (nodes < edge[1]) nodes=edge[1];
        }
        //		printf("%d <-> %d", edge[0], edge[1]);
        //		exit(-1);
    }
    rewind(fh);

    for (int i = 0; i <= nodes; i++) {
        graph.nodesLst.push_back(Node(i));
    }

    while (getline(&line, &linesize, fh)>0) {
        if ( sscanf(line, "%d %d", &edge[0], &edge[1])==2 )
            link_add(graph.nodesLst[edge[0]], graph.nodesLst[edge[1]]);
    }
    fclose(fh);

    for (unsigned int i = 0; i < graph.nodesLst.size(); i++) {
        if (graph.nodesLst[i].neighbors->size()>0) graph.noNodes++;
        graph.noEdges+=graph.nodesLst[i].neighbors->size();
    }
    graph.noEdges/=2;
    delete(line);

    return graph;
}

Graph graph_load2(const char *fname) {
    Graph graph;
    string str;
    ifstream infile;
    infile.open(fname,fstream::in);
    if(!infile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    while(!infile.eof()) {
        tkns.clear();
        str.clear();
        getline(infile,str);
        if( str[0] =='#') continue;
        Tokenize (str,tkns,"\t");
        if (tkns.size()>=2) {
            int edge0=atoi(tkns[0].c_str());
            int edge1=atoi(tkns[1].c_str());
            int maxNodeId=max(edge0,edge1);
            int nodes=graph.nodesLst.size();
            for (int i = nodes; i <= maxNodeId; i++)	graph.nodesLst.push_back(Node(i));
            link_add(graph.nodesLst[edge0], graph.nodesLst[edge1]);
        }
    }
    infile.close();
    for (unsigned int i = 0; i < graph.nodesLst.size(); i++) {
        if (graph.nodesLst[i].neighbors->size()>0) graph.noNodes++;
        graph.noEdges+=graph.nodesLst[i].neighbors->size();
    }
    graph.noEdges/=2;

    return graph;
}


Graph graph_new_load(const char *fname, bool calc) {
    Graph graph;
    ifstream inFile;
    inFile.open(fname,fstream::in);
    if(!inFile) {
        cout << "Cannot open infile_name: "<<fname<<".\n";
        exit(-1);
    }
    int src, dst;
    int line_cnt=0;
//	while(!inFile.eof()) {
    while(inFile >> src >> dst) {
        line_cnt++;
        if (src==1624987 || dst ==1624987) cout<<line_cnt<<"\t"<< src << "\t" << dst <<"\n";
        int maxNodeId=max(src,dst);
        int prevNoNodes=graph.nodesLst.size();
        if (maxNodeId>=prevNoNodes) {
            //			graph.nodesLst.resize(maxNodeId+1);
            //			for (unsigned int i=prevNoNodes;i<=maxNodeId; i++) {
            //				graph.nodesLst[i].id=i;
            //			}
            for (int i=prevNoNodes;i<=maxNodeId;i++) {
                graph.nodesLst.push_back(Node(i));
            }
        }
        //		cout<<"nodeLstSize: "<<graph.nodesLst.size()<<"\tmaxNodeId:"<<maxNodeId<<endl;

        link_add(graph.nodesLst[src], graph.nodesLst[dst]);

        //		cout << src << "\t" << dst;
        //		std::cin.get();
    }
    inFile.close();

    if (calc) {
        for (unsigned int i = 0; i < graph.nodesLst.size(); i++) {
            if (graph.nodesLst[i].neighbors->size()>0) graph.noNodes++;
            graph.noEdges+=graph.nodesLst[i].neighbors->size();
        }
        graph.noEdges/=2;
    }

    return graph;
}




void dumpGraph(Graph graph, char* fileName) {
    printf("Printing into : %s\n", fileName);
    FILE *oFile;
    if (( oFile = fopen(fileName, "w")) == NULL)
    {
        printf("Error opening file : %s\n", fileName);
    }
    graph.calc();
    fprintf(oFile,"#Nodes: %d\n#SeprateNodes: %d\n#Edges: %d\n",graph.noNodes,graph.noSeprateNodes,(int)graph.noEdges);

    for(unsigned int nodeA=0;nodeA<graph.nodesLst.size();nodeA++) {
        for(unsigned int nodeA_neiID=0;nodeA_neiID<graph.nodesLst[nodeA].neighbors->size();nodeA_neiID++) {
            fprintf(oFile,"%d\t%d\n",nodeA,graph.nodesLst[nodeA].neighbors->at(nodeA_neiID));
        }
    }
    fclose(oFile);
    return;
}

void dumpGraph_CC(Graph graph, char* fileName) {
    graph.calc();
    printf("Printing into : %s\n", fileName);
    FILE *oFile;
    if (( oFile = fopen(fileName, "w")) == NULL) {
        printf("Error opening file : %s\n", fileName);
    }
    fprintf(oFile,"#Nodes: %d\n#SeprateNodes: %d\n#Edges: %d\n",graph.noNodes,graph.noSeprateNodes,(int)graph.noEdges);

    for(unsigned int nodeA=0;nodeA<graph.nodesLst.size();nodeA++) {
        if(graph.nodesLst[nodeA].hop==-1) continue;
        for(unsigned int nodeA_neiID=0;nodeA_neiID<graph.nodesLst[nodeA].neighbors->size();nodeA_neiID++) {
            unsigned int nodeB=graph.nodesLst[nodeA].neighbors->at(nodeA_neiID);
            if(graph.nodesLst[nodeB].hop==-1) continue;
            if (nodeB>nodeA) {
                fprintf(oFile,"%d\t%d\n",nodeA,nodeB);
            }
        }
    }
    fclose(oFile);
    return;
}


void cleanUp(Graph& graph, char* dumpDir, char* nettype)
{
    int nodeID=1;
    for(unsigned int node=0;node<graph.nodesLst.size();node++) {
        if (graph.nodesLst[node].neighbors->size()>0) {
            graph.nodesLst[node].id=nodeID;
            nodeID++;
        }
        else graph.nodesLst[node].id=-1;
    }
    ofstream myfile;
    char fname[128];
    sprintf(fname,"%s/%s_mapping.txt",dumpDir,nettype);
    myfile.open (fname);
    myfile<<"#old\t#new\n";
    for(unsigned int node=0;node<graph.nodesLst.size();node++)	{
        for(unsigned int nei_ID;nei_ID<graph.nodesLst[node].neighbors->size();nei_ID++) {
            int nei=graph.nodesLst[node].neighbors->at(nei_ID);
            //			graph.nodesLst[node].neighborsId.push_back(graph.nodesLst[nei].id);
        }
        myfile<<node<<"\t"<<graph.nodesLst[node].id<<endl;
    }
    myfile.close();
    return;
}

void dumpGraph4Metis(Graph graph, char* fileName) {
    if ((graph.nodesLst[0].neighbors->size()>0)&&(graph.nodesLst[0].id!=-1)) {
        cout<<"ERROR: nodeID's should be shifted"<<endl;
        exit(-1);
    }
    vector <int> nodeIdx=vector<int>(graph.nodesLst.size(),0);
    int thisNodeIDX=1;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if(graph.nodesLst[i].neighbors->size()>0) {
            nodeIdx[i]=thisNodeIDX;
            thisNodeIDX++;
        }
    }

    printf("Printing into : %s\n", fileName);
    FILE *oFile;
    if (( oFile = fopen(fileName, "w")) == NULL) {
        printf("Error opening file : %s\n", fileName);
    }
    graph.calc();
    fprintf(oFile,"%d\t%f\n",graph.noNodes,graph.noEdges/2);
    for(unsigned int nodeA=1;nodeA<graph.nodesLst.size();nodeA++) {
        if (graph.nodesLst[nodeA].neighbors->size()==0) continue;
        fprintf(oFile,"%d",1); // First item is the node's size
        for(unsigned int nodeA_neiID=0;nodeA_neiID<graph.nodesLst[nodeA].neighbors->size();nodeA_neiID++) {
            int nei=graph.nodesLst[nodeA].neighbors->at(nodeA_neiID);
            fprintf(oFile,"\t%d",nodeIdx[graph.nodesLst[nei].id]);
            //graph.nodesLst[nei].removeNeighbor(nodeA);
        }
        fprintf(oFile,"\n");
    }
    fclose(oFile);

    strcat(fileName,".indexes");
    printf("Printing into : %s\n", fileName);
    if (( oFile = fopen(fileName, "w")) == NULL) {
        printf("Error opening file : %s\n", fileName);
    }
    for(unsigned int nodeA=1;nodeA<graph.nodesLst.size();nodeA++) {
        fprintf(oFile,"%d\t%d",graph.nodesLst[nodeA].id,nodeIdx[graph.nodesLst[nodeA].id]);
        fprintf(oFile,"\n");
    }
    fclose(oFile);

}


Graph randomizeGraph(Graph graph,float pr) {
    // map <int,Link> graph_links;
    unsigned int max_degree=0;
    //	int id=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if (max_degree<graph.nodesLst[i].neighbors->size()) max_degree=graph.nodesLst[i].neighbors->size();
    }

    int final_no_rewire=pr*graph.noEdges;
    int rewired=0;

    printf("Ultimate number of rewirings: %d\n",final_no_rewire);
    for(rewired=0;rewired<final_no_rewire;rewired+=2)
    {
        if (rewired%100000==0) printf("number of rewiring performed: %d\n",rewired);
        // for(int nodeA_neiID=0;nodeA_neiID<graph.nodesLst[nodeA].neighbors->size();nodeA_neiID++)
        // {
        // lnk1=rand()%graph_links.size();

        int nodeA=-1;
        int nodeB=-1;
        int nodeC=-1;
        int nodeD=-1;

        int nodeA_neiID=-1;
        int nodeC_neiID=-1;

        float acceptance_prob;//=(float)rand()/(float)RAND_MAX;
        float accept;//=(((float)graph.noNodes)*(graph.nodesLst[nodeA].neighbors->size()))/((float)graph.noEdges);

        do{
            nodeA=rand()%graph.nodesLst.size();
            double acceptance_prob=(double)rand()/(double)RAND_MAX;
            double accept=(((double)graph.nodesLst[nodeA].neighbors->size())/((double)max_degree));
            // printf("%f\t>?\t%f\n",acceptance_prob,accept);
            // getchar();
        }while((acceptance_prob<accept)||(graph.nodesLst[nodeA].neighbors->size()==0));

        nodeA_neiID=rand()%graph.nodesLst[nodeA].neighbors->size();
        nodeB=graph.nodesLst[nodeA].neighbors->at(nodeA_neiID);
        do {
            do {
                nodeC=rand()%graph.nodesLst.size();
                double acceptance_prob=(double)rand()/(double)RAND_MAX;
                double accept=(((double)graph.nodesLst[nodeC].neighbors->size())/((double)max_degree));
            } while((acceptance_prob<accept)||(graph.nodesLst[nodeC].neighbors->size()==0)||(graph.nodesLst[nodeA].getNeighborIdx(nodeC)!=-1)||(nodeA==nodeC));

            int nodeC_neiID=rand()%graph.nodesLst[nodeC].neighbors->size();
            nodeD=graph.nodesLst[nodeC].neighbors->at(nodeC_neiID);
        }while(((graph.nodesLst[nodeB].getNeighborIdx(nodeD)!=-1)||(nodeD==nodeB)));

        // if (no_try>=10) break;

        graph.nodesLst[nodeA].removeNeighbor(nodeB);
        graph.nodesLst[nodeB].removeNeighbor(nodeA);
        graph.nodesLst[nodeC].removeNeighbor(nodeD);
        graph.nodesLst[nodeD].removeNeighbor(nodeC);

        graph.nodesLst[nodeA].addNeighbor(nodeC);
        graph.nodesLst[nodeC].addNeighbor(nodeA);
        graph.nodesLst[nodeB].addNeighbor(nodeD);
        graph.nodesLst[nodeD].addNeighbor(nodeB);
    }

    printf("total rewiring performed: %d;\t number of changed links: %d\n",rewired,rewired*2);
    return graph;
}

int link_add(Node &n1, Node &n2) {
    if ((n1.neighbors->size()>INT_MAX)||(n1.neighbors->size()>INT_MAX)) {
        printf("%d to %d\n",n1.id, n2.id);
        printf("number of neighbors: %d and %d",n1.neighbors->size(), n2.neighbors->size());
        getchar();
    }

    //do not add self edges (the case in livejournal)
    if (n1.id == n2.id) {
//		printf("check file, probabilty of Errors, loop in %d and %d\n",n1.id,n2.id);
        return -1;
    }

    n1.addNeighbor(n2.id);
    n2.addNeighbor(n1.id);
    return 1;

    /*	int exists1=n1.addAndCkeckNeighbor(n2.id);
    if (exists1==-1)
        printf("check file, probability of Errors, link already exists between % and %\n",n1.id,n2.id);

    int exists2=n2.addAndCkeckNeighbor(n1.id);
    if (exists2==-1)
        printf("check file, probability of Errors, link already exists between % and %\n",n2.id,n1.id);

    if ((exists1==-1)||(exists2==-1))
        return -1;
    else
        return 0;*/
}

void displayGraph(Graph g) {
    for (unsigned int i=0;i<g.nodesLst.size();i++) {
        printf("%d\t",g.nodesLst[i].id);
        for (unsigned int j=0;j<g.nodesLst[i].neighbors->size();j++) {
            printf("%d\t",g.nodesLst[i].neighbors->at(j));
        }
        printf("\n");
    }
}

void checkDegreeDistrib(Graph g1, Graph g2) {
    int total_diff=0;
    if (g1.nodesLst.size()!=g2.nodesLst.size()) {
        printf("unequal sized graphs\n");
        //exit -1;
    }
    for(unsigned int i=0;i<g1.nodesLst.size();i++) {
        if(g1.nodesLst[i].neighbors->size()!=g2.nodesLst[i].neighbors->size()) {
            printf("unequal number of neighbors in nodes \n");
            //exit -1;
        }
        for(unsigned int j=0;j<g1.nodesLst[i].neighbors->size();j++) {
            if(g1.nodesLst[i].neighbors->at(j)!=g2.nodesLst[i].neighbors->at(j))
                total_diff++;
        }
    }
    printf ("total number of diffrence: %d\n",total_diff);
}


void checkRegions(Graph graph, char* regionFile, int regionFileType, char* outFile, int partInfo, bool measureLCC) {
    cout.precision(3);
    graph.calc();

    cout<<"Graph no nodes: "<<graph.noNodes<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";

    int partCount=0;
    vector<int> partSize;
    vector < vector<int> > partMember;

    //	vector < int > setNames;
    //	vector < string > setNames;

    if (regionFileType==0) {
        cout<<"Reading info file"<<endl;
        graph=markNodeRegionAndReturnRegions(graph, partCount, partSize, partMember, regionFile);
        //		for (int i=0;i<(set_no);i++){
        //			//			char* tmp;
        //			//			sprintf(tmp,"%d",i);
        //			//			setNames.push_back(string(tmp));
        //			setNames.push_back(i);
        //		}
    } else if (regionFileType==1) {
        cout<<"Reading part file"<<endl;
        graph=readPartFileAndReturnRegions(graph, partCount, partSize, partMember, regionFile, partInfo);
    } else if (regionFileType==2) {
        cout<<"Reading part file"<<endl;
        graph=readPartFileAndReturnRegions(graph, partCount, partSize, partMember, regionFile, partInfo);
    } else printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");

    cout<<"set_no: "<<partCount<<endl;
    cout<<"partSize.size(): "<<partSize.size()<<endl;
    cout<<"sets.size(): "<<partMember.size()<<endl;
    cout<<endl;

    vector<double> regionDeg(partCount,0);
    vector<double> regionDegIntra(partCount,0);
    vector<double> regionDegExtra(partCount,0);
    vector<int> centreCandidateId(partCount,-1);
    vector<int> centreCandidateDeg(partCount,-1);

    vector<float> LLC_Percentage(partCount,-1);
    vector<float> inLCC_avgDeg(partCount,-1);
    vector<float> outLCC_avgDeg(partCount,-1);


    double graphDeg=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if ((i<0) || (i>graph.nodesLst.size())) {
            cout<<"Shit, integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcRegion=graph.nodesLst[i].getRegion();

        regionDeg[srcRegion]+=graph.nodesLst[i].neighbors->size();
        graphDeg+=graph.nodesLst[i].neighbors->size();

        if ((centreCandidateId[srcRegion]==-1)||(centreCandidateDeg[srcRegion]<graph.nodesLst[i].neighbors->size())) {
            centreCandidateId[srcRegion]=i;
            centreCandidateDeg[srcRegion]=graph.nodesLst[i].neighbors->size();
        }
        for (unsigned int neiID=0;neiID<graph.nodesLst[i].neighbors->size();neiID++) {
            int nei=graph.nodesLst[i].neighbors->at(neiID);
            int dstRegion=graph.nodesLst[nei].getRegion();
            if  ((srcRegion>=0) && (srcRegion<partCount)) { 	// src region in known
                if ((dstRegion>=0) && (dstRegion<partCount)) { // dst region in known
                    if (srcRegion==dstRegion) {
                        regionDegIntra[srcRegion]++;
                    } else {
                        regionDegExtra[srcRegion]++;
                    }
                } else {
                    cout<<"out of range dst "<<dstRegion<<" from node: "<<i<<endl;
                }
            } else {
                cout<<"out of range src "<<srcRegion<<" from node: "<<nei<<endl;
            }
        }
    }

    /* BEGIN measuring the LCC */
    // 	bool measureLCC = true;
    if (measureLCC) {
        for (int region=0;region<partCount;region++) {
            for(unsigned int i=0;i<graph.nodesLst.size();i++)
                graph.nodesLst[i].hop=-1;
            int ccNo=0;
            //            int disconnectedNodesCnt=0;
            //            int disconnectedNodesDegrees=0;
            int connectedNodesCntSum=0;
            int connectedNodesCntMax=0;
            int connectedNodesSumDegreesMax=0;
            while (centreCandidateId[region]!=-1) {
                cout<<"====> CC_No: "<<ccNo<<endl;
                double temp;
                RegionBFS (centreCandidateId[region],graph,false,temp);
                centreCandidateId[region]=-1;
                centreCandidateDeg[region]=-1;
                ccNo++;
                int inThisCC=0;
                int inThisCCSumDegree=0;
                for(unsigned int j=0;j<partMember[region].size();j++) {
                    int i=partMember[region][j];
                    if(graph.nodesLst[i].neighbors->size()==0) continue;
                    if(graph.nodesLst[i].getRegion()==region) {
                        if(graph.nodesLst[i].hop==-1) {
                            //                            if (ccNo==1) {
                            //                                disconnectedNodesCnt++;
                            //                                disconnectedNodesDegrees+=graph.nodesLst[i].neighbors->size();
                            //                            }
                            if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
                                centreCandidateId[region]=i; // remove if only one CC is needed.
                                centreCandidateDeg[region]=graph.nodesLst[i].neighbors->size();
                            }
                        } else {
                            graph.nodesLst[i].setRegion(-1);
                            inThisCC++;
                            inThisCCSumDegree+=graph.nodesLst[i].neighbors->size();
                        }
                    }
                }
                connectedNodesCntSum += inThisCC;
                if (connectedNodesCntMax<inThisCC) {
                    connectedNodesCntMax		= inThisCC;
                    connectedNodesSumDegreesMax = inThisCCSumDegree;
                }
                if ((partSize[region]-connectedNodesCntSum)<connectedNodesCntMax) {
                    centreCandidateId[region]=-1;
                }
                cout<<" --> This CC's portion: "<<(inThisCC*1.0)/(partSize[region]*1.0)<<endl;
            }
            if (connectedNodesCntMax==partSize[region]) cout<<"# region "<<region<<" with population "<<partSize[region]<<" is a connected component\n";
            else {
                cout<<"# region "<<region<<" with population "<<partSize[region]<<" is NOT a connected component - Not in component= "<<partSize[region]-connectedNodesCntMax<<" - CC's portion: "<<(connectedNodesCntMax*1.0)/(partSize[region])<<"\t";
                cout<<"# Number of counted CC's: "<<ccNo<<endl;
                cout<<"CC's average degree: "<<connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0)<<" - separate nodes average degree: "
                   <<(regionDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0)<<endl;
            }

            LLC_Percentage[region]=(connectedNodesCntMax*1.0)/(partSize[region]);
            inLCC_avgDeg[region]=connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0);
            outLCC_avgDeg[region]=(regionDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0);
        }
    }
    /* END of measuring the LCC */
    cout<<"# Dumping outFiles"<<endl;

    /* calculate the index of each name */
    //	vector<int> setPlaceInSetNames=vector<int>(set_no,-2);
    //	for (int i=0;i<set_no;i++){
    //		for (int j=0;j<set_no;j++){
    //			//			char str [1024];
    //			//			sprintf(str,"%d",i); //converts to decimal base.
    //			//			if (setNames[j].compare(str)==0) {
    //			if (setNames[j]==i) {
    //				setPlaceInSetNames[i]=j;
    //				break;
    //			}
    //		}
    //	}

    /* Matrix */
    //	cout<<"#BEGIN Inter-region Connectivity Matrix latex"<<endl;
    //	for (int src=0;src<set_no;src++) {
    //		int srcRegion=setPlaceInSetNames[src];
    //		cout<<"\t&\t$R"<<setNames[srcRegion]<<"$";
    //	}
    //	cout<<"\t\\\\";
    //	cout<<endl;
    //	for (int dst=0;dst<set_no;dst++)
    //	{
    //		int dstRegion=setPlaceInSetNames[dst];
    //		cout<<"$R"<<setNames[dstRegion]<<"$";
    //		for (int src=0;src<set_no;src++) {
    //			int srcRegion=setPlaceInSetNames[src];
    //			cout<<"\t&\t"<<edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion];
    //		}
    //		cout<<"\t\\\\";
    //		cout<<endl;
    //	}
    //	cout<<"#END Inter-region Connectivity Matrix latex\n";
    //	cout<<endl;
    //	cout<<endl;

    ofstream ofile;
    ofile.open(outFile,ios::out);

    ofile<<"#Region"<<"\t&";
    ofile<<"\t"<<"\%Nodes"<<"\t&";
    ofile<<"\t"<<"\%Edges"<<"\t&";
    ofile<<"\t"<<"\%Int Links"<<"\t&";
    ofile<<"\t"<<"\%Ext Links"<<"\t&";
    ofile<<"\t"<<"Int. Bias"<<"\t&";
    ofile<<"\t"<<"Ext. Bias"<<"\t&";

    ofile<<"\t"<<"AvgDeg"<<"\t&";
    ofile<<"\t"<<"Modularity"<<"\t&";

    ofile<<"\t"<<"Homogeneity"<<"\t\\\\";
    ofile<<endl;
    for (int dst=0;dst<partCount;dst++) {
        //		int dstRegionName=setPlaceInSetNames[dst];
        int dstRegion=dst;

        //		ofile<<"$R"<<setNames[dstRegionName]<<"$\t&";
        //		ofile<<"$r"<<setNames[dstRegion]<<"$\t&";
        ofile<<"$r"<<dstRegion<<"$\t&";
        ofile<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&"; //region size
        double random=(regionDeg[dstRegion]*100.0)/(graphDeg*1.0);
        ofile<<"\t"<<random<<"\t&"; 	//region half edges

        double internal=regionDegIntra[dstRegion]*100.0/regionDeg[dstRegion];

        ofile<<"\t"<<internal<<"\t&";	// region internal connectivity
        double external=((regionDeg[dstRegion]-regionDegIntra[dstRegion])*100.0)/(regionDeg[dstRegion]);
        ofile<<"\t"<<external<<"\t&";	// region external connectivity

        ofile<<"\t"<<(internal-random)/random<<"\t&"; 				// region internal connectivity bias
        ofile<<"\t"<<(external-(100-random))/(100-random)<<"\t&"; 	// region external connectivity bias


        ofile<<"\t"<< regionDeg[dstRegion]/partSize[dstRegion] <<"\t&";
        double module= (regionDegIntra[dstRegion]/regionDeg[dstRegion]) - (regionDeg[dstRegion]/graphDeg);
        ofile<<"\t"<<module<<"\t&";

        ofile<<"\t"<<LLC_Percentage[dstRegion] <<"\t\\\\";
        ofile<<endl;
    }
    ofile<<"#End Stats"<<endl;
    ofile.close();


    //	ofstream ofile;
    char fname[1024];
    sprintf(fname,"%s.tab",outFile);
    //	strcat(fname,"_parts.txt");
    ofile.open((const char*)fname,ios::out);
    for (int dst=0;dst<partCount;dst++) {
        //		int dstRegion=setPlaceInSetNames[dst];
        int dstRegion=dst;

        if (regionDeg[dstRegion]<1) continue;

        //		ofile<<""<<setNames[dstRegion]<<"\t";
        ofile<<""<<dstRegion<<"\t";
        ofile<<""<<partSize[dstRegion]<<"\t"; //region size
        ofile<<""<<regionDeg[dstRegion]<<"\t"; //region deg

        double random=(regionDeg[dstRegion]*100.0)/(graphDeg*1.0);
        ofile<<""<<random<<"\t"; 	//region half edges
        double internal=regionDegIntra[dstRegion]*100.0/regionDeg[dstRegion];
        ofile<<""<<internal<<"\t";	// region internal connectivity
        double external=((regionDeg[dstRegion]-regionDegIntra[dstRegion])*100.0)/(regionDeg[dstRegion]);
        ofile<<""<<external<<"\t";	// region external connectivity

        ofile<<""<<(internal-random)/random<<"\t"; 				// region internal connectivity bias
        ofile<<""<<(external-(100-random))/(100-random)<<"\t"; 	// region external connectivity bias

        ofile<<""<< regionDeg[dstRegion]/partSize[dstRegion] <<"\t";
        double module= (regionDegIntra[dstRegion]/regionDeg[dstRegion]) - (regionDeg[dstRegion]/graphDeg);
        ofile<<""<<module<<"\t";

        ofile<<""<<LLC_Percentage[dstRegion] <<"";
        ofile<<endl;
    }
    ofile.close();
    return;
}


void fullCheckRegions(Graph graph, char* regionFile, int regionFileType, char* outFile, int partInfo) {
    cout.precision(3);
    graph.calc();
    cout<<"Graph no nodes: "<<graph.noNodes<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";

    int partCount=0;
    vector<int> partSize;
    vector < vector<int> > partMembers;
    //	vector < int > setNames;
    //	vector < string > setNames;

    //	if (strcmp(regionInfoFile,"-1")!=0) {
    if (regionFileType==0) {
        cout<<"Reading info file"<<endl;
        graph=markNodeRegionAndReturnRegions(graph, partCount, partSize, partMembers, regionFile);
        //		for (int i=0;i<(set_no);i++){
        //			//			char* tmp;
        //			//			sprintf(tmp,"%d",i);
        //			//			setNames.push_back(string(tmp));
        //			setNames.push_back(i);
        //		}
        //	setNames.push_back("U");
    } else if (regionFileType==1) {
        cout<<" Reading part file"<<endl;
        graph=readPartFileAndReturnRegions(graph, partCount, partSize, partMembers, regionFile, partInfo);
    } else if (regionFileType==2) {
        cout<<" Reading community file"<<endl;
        graph=readPartFileAndReturnRegions(graph, partCount, partSize, partMembers, regionFile, partInfo);
    } else printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");

    cout<<"set_no: "<<partCount<<endl;
    cout<<"partSize.size(): "<<partSize.size()<<endl;
    cout<<"sets.size(): "<<partMembers.size()<<endl;
    cout<<endl;

    vector< vector<double> > edgeCount(partCount,vector<double>(partCount,0));
    vector<double> regionDeg(partCount,0);
    vector<int> centreCandidateId(partCount,-1);
    vector<int> centreCandidateDeg(partCount,-1);

    vector<float> LLC_Percentage(partCount,-1);
    //    vector<float> inLCC_avgDeg(set_no,-1);
    //    vector<float> outLCC_avgDeg(set_no,-1);

    //	vector<int> regionDeg(set_no,0);
    double graphDeg=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++)
    {
        if ((i<0) || (i>graph.nodesLst.size())) {
            cout<<"Shit, integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcRegion=graph.nodesLst[i].getRegion();
        regionDeg[srcRegion]+=graph.nodesLst[i].neighbors->size();
        graphDeg+=graph.nodesLst[i].neighbors->size();
        if ((centreCandidateId[srcRegion]==-1)||(centreCandidateDeg[srcRegion]<graph.nodesLst[i].neighbors->size())) {
            centreCandidateId[srcRegion]=i;
            centreCandidateDeg[srcRegion]=graph.nodesLst[i].neighbors->size();
        }
        for (unsigned int neiID=0;neiID<graph.nodesLst[i].neighbors->size();neiID++) {
            int nei=graph.nodesLst[i].neighbors->at(neiID);
            int dstRegion=graph.nodesLst[nei].getRegion();
            if  ((srcRegion>=0) && (srcRegion<partCount)) {
                if ((dstRegion>=0) && (dstRegion<partCount)) {
                    edgeCount[srcRegion][dstRegion]++;
                } else {
                    cout<<"out of range dst "<<dstRegion<<" from node: "<<i<<endl;
                }
            } else {
                cout<<"out of range src "<<srcRegion<<" from node: "<<nei<<endl;
            }
        }
    }
    //	for (int i=0;i<set_no;i++) edgeCount[i][i]/=2;
    //	cout<<"Inter-region edge count"<<endl;
    //	for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //		cout<<srcRegion<<" ["<<setNames[srcRegion]<<"]:";
    //		for (int dstRegion=0;dstRegion<set_no;dstRegion++) {
    //			cout<<"\t"<<edgeCount[srcRegion][dstRegion];
    //			//			regionDeg[srcRegion]+=edgeCount[srcRegion][dstRegion];
    //		}
    //		cout<<endl;
    //	}
    //	cout<<endl;
    //	cout<<endl;

    cout<<"regionId"<<"\t"<<"centreCandidateId"<<"\t"<<"centreCandidateDeg"<<"\t"<<"regionSize"<<"\t"<<"sumDegree"<<endl;
    for (int region=0;region<partCount;region++) {
        cout<<region<<"\t"<<region<<"\t"<<centreCandidateId[region]<<"\t"<<centreCandidateDeg[region]<<"\t"<<partSize[region]<<"\t"<<regionDeg[region]<<"\t"<<regionDeg[region]/(double)partSize[region]<<endl;
    }
    cout<<endl;


    /* BEGIN measuring the LCC */
    for (int region=0;region<partCount;region++) {
        for(unsigned int i=0;i<graph.nodesLst.size();i++)
            graph.nodesLst[i].hop=-1;
        int ccNo=0;
        int connectedNodesCntSum=0;
        int connectedNodesCntMax=0;
        int connectedNodesSumDegreesMax=0;
        while (centreCandidateId[region]!=-1) {
//			cout<<"====> CC_No: "<<ccNo<<endl;
            double temp;
            RegionBFS (centreCandidateId[region],graph,false,temp);
            centreCandidateId[region]=-1;
            centreCandidateDeg[region]=-1;
            ccNo++;
            int inThisCC=0;
            int inThisCCSumDegree=0;
            for(unsigned int j=0;j<partMembers[region].size();j++) {
                int i=partMembers[region][j];
                if(graph.nodesLst[i].neighbors->size()==0) continue;
                if(graph.nodesLst[i].getRegion()==region) {
                    if(graph.nodesLst[i].hop==-1) {
                        if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
                            centreCandidateId[region]=i; // remove if only one CC is needed.
                            centreCandidateDeg[region]=graph.nodesLst[i].neighbors->size();
                        }
                    } else {
                        graph.nodesLst[i].setRegion(-1);
                        inThisCC++;
                        inThisCCSumDegree+=graph.nodesLst[i].neighbors->size();
                    }
                }
            }
            connectedNodesCntSum += inThisCC;
            if (connectedNodesCntMax<inThisCC) {
                connectedNodesCntMax		= inThisCC;
                connectedNodesSumDegreesMax = inThisCCSumDegree;
            }
            if ((partSize[region]-connectedNodesCntSum)<connectedNodesCntMax) {
                centreCandidateId[region]=-1;
            }
            if (ccNo>1000) break;
//			cout<<" --> This CC's portion: "<<(inThisCC*1.0)/(partSize[region]*1.0)<<endl;
        }
        if (connectedNodesCntMax==partSize[region]) cout<<"# region "<<region<<" with population "<<partSize[region]<<" is a connected component\n";
        else {
            cout<<"# region "<<region<<" with population "<<partSize[region]<<" is NOT a connected component - Not in component= "<<partSize[region]-connectedNodesCntMax<<" - CC's portion: "<<(connectedNodesCntMax*1.0)/(partSize[region])<<"\t";
            cout<<"# Number of counted CC's: "<<ccNo<<endl;
            cout<<"CC's average degree: "<<connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0)<<" - separate nodes average degree: "
               <<(regionDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0)<<endl;
        }
        LLC_Percentage[region]=(connectedNodesCntMax*1.0)/(partSize[region]);
        //			inLCC_avgDeg[region]=connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0);
        //			outLCC_avgDeg[region]=(regionDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0);
    }
    /* END measuring the LCC */

    // calculate the index of each name
    //	vector<int> setPlaceInSetNames=vector<int>(set_no,0);
    //	for (int i=0;i<set_no;i++){
    //		for (int j=0;j<set_no;j++){
    //			//			char str [33];
    //			//			sprintf(str,"%d",i); //converts to decimal base.
    //			//			if (setNames[j].compare(str)==0) {
    //			if (setNames[j]==i) {
    //				setPlaceInSetNames[i]=j;
    //				break;
    //			}
    //		}
    //	}

    /* Matrix */
    //	cout<<"#BEGIN Inter-region Connectivity Matrix latex"<<endl;
    //	for (int src=0;src<set_no;src++) {
    //		int srcRegion=setPlaceInSetNames[src];
    //		cout<<"\t&\t$R"<<setNames[srcRegion]<<"$";
    //	}
    //	cout<<"\t\\\\";
    //	cout<<endl;
    //	for (int dst=0;dst<set_no;dst++)
    //	{
    //		int dstRegion=setPlaceInSetNames[dst];
    //		cout<<"$R"<<setNames[dstRegion]<<"$";
    //		for (int src=0;src<set_no;src++) {
    //			int srcRegion=setPlaceInSetNames[src];
    //			cout<<"\t&\t"<<edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion];
    //		}
    //		cout<<"\t\\\\";
    //		cout<<endl;
    //	}
    //	cout<<"#END Inter-region Connectivity Matrix latex\n";
    //	cout<<endl;
    //	cout<<endl;

    ofstream ofile;
    char fname[1024];
    sprintf(fname,"%s.txt",outFile);
    ofile.open((const char*)fname,ios::out);

    ofile<<"#Region"<<"\t&";
    //    ofile<<"\t"<<"\Core Size"<<"\t&";
    //    ofile<<"\t"<<"\Core Avg. Deg"<<"\t&";
    ofile<<"\t"<<"\%Nodes"<<"\t&";
    ofile<<"\t"<<"\%Edges"<<"\t&";
    ofile<<"\t"<<"\%Int Links"<<"\t&";
    ofile<<"\t"<<"\%Ext Links"<<"\t&";
    ofile<<"\t"<<"Int. Bias"<<"\t&";
    ofile<<"\t"<<"Ext. Bias"<<"\t&";

    ofile<<"\t"<<"AvgDeg"<<"\t&";
    ofile<<"\t"<<"Modularity"<<"\t&";

    ofile<<"\t"<<"Homogeneity"<<"\t\\\\";
    ofile<<endl;
    for (int dst=0;dst<partCount;dst++) {
        int dstRegion=dst;

        if (regionDeg[dstRegion]<1) continue;

        ofile<<"$R"<<dstRegion<<"$\t&";

        ofile<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&"; //region size
        double random=(regionDeg[dstRegion]*100.0)/(graphDeg*1.0);
        ofile<<"\t"<<random<<"\t&"; 	//region half edges

        double internal=edgeCount[dstRegion][dstRegion]*100.0/regionDeg[dstRegion];
        ofile<<"\t"<<internal<<"\t&";	// region internal connectivity
        double external=((regionDeg[dstRegion]-edgeCount[dstRegion][dstRegion])*100.0)/(regionDeg[dstRegion]);
        ofile<<"\t"<<external<<"\t&";	// region external connectivity

        ofile<<"\t"<<(internal-random)/random<<"\t&"; 				// region internal connectivity bias
        ofile<<"\t"<<(external-(100-random))/(100-random)<<"\t&"; 	// region external connectivity bias


        ofile<<"\t"<< regionDeg[dstRegion]/partSize[dstRegion] <<"\t&";
        double module= (edgeCount[dstRegion][dstRegion]/regionDeg[dstRegion]) - (regionDeg[dstRegion]/graphDeg);
        ofile<<"\t"<<module<<"\t&";

        ofile<<"\t"<<LLC_Percentage[dstRegion] <<"\t\\\\";
        ofile<<endl;
    }
    ofile<<"#End Stats"<<endl;
    ofile.close();

    sprintf(fname,"%s.tab",outFile);
    ofile.open((const char*)fname,ios::out);
    for (int dst=0;dst<partCount;dst++) {
        int dstRegion=dst;//setPlaceInSetNames[dst];

        if (regionDeg[dstRegion]==0) continue;

        ofile<<""<<dstRegion<<"\t";
        ofile<<""<<partSize[dstRegion]<<"\t"; //region size
        ofile<<""<<regionDeg[dstRegion]<<"\t"; //region deg

        double random=(regionDeg[dstRegion]*100.0)/(graphDeg*1.0);
        ofile<<""<<random<<"\t"; 	//region half edges
        double internal=edgeCount[dstRegion][dstRegion]*100.0/regionDeg[dstRegion];
        ofile<<""<<internal<<"\t";	// region internal connectivity
        double external=((regionDeg[dstRegion]-edgeCount[dstRegion][dstRegion])*100.0)/(regionDeg[dstRegion]);
        ofile<<""<<external<<"\t";	// region external connectivity

        ofile<<""<<(internal-random)/random<<"\t"; 				// region internal connectivity bias
        ofile<<""<<(external-(100-random))/(100-random)<<"\t"; 	// region external connectivity bias

        ofile<<""<< regionDeg[dstRegion]/partSize[dstRegion] <<"\t";
        double module= (edgeCount[dstRegion][dstRegion]/regionDeg[dstRegion]) - (regionDeg[dstRegion]/graphDeg);
        ofile<<""<<module<<"\t";

        ofile<<""<<LLC_Percentage[dstRegion] <<"";
        ofile<<endl;
    }
    ofile.close();

    //	cout<<"#BEGIN Inter-region Connectivity Prob\n";
    //	cout<<"\t"<<"%Nodes"<<"\t&\t"<<"%Edges"<<"\t&&";
    //	for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //		cout<<"\t$R"<<setNames[srcRegion]<<"$\t&";
    //	}
    //	cout<<"&\t"<< "LLC_Percentage" <<"\t&";
    //	cout<<"\t"<< "inLCC_avgDeg" <<"\t&";
    //	cout<<"\t"<< "outLCC_avgDeg" <<"\t\\\\";
    //	cout<<endl;
    //	for (int dstRegion=0;dstRegion<set_no;dstRegion++)
    //	{
    //		cout<<"$R"<<setNames[dstRegion]<<"$";
    //		cout<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&\t"<<(regionDeg[dstRegion]*100.0)/(graphDeg*1.0)<<"\t&&";
    //		for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //			cout<<"\t"<<edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion]<<"\t&";
    //		}
    //		cout<<"&\t"<< LLC_Percentage[dstRegion] <<"\t&";
    //		cout<<"\t"<< inLCC_avgDeg[dstRegion] <<"\t&";
    //		cout<<"\t"<< outLCC_avgDeg[dstRegion] <<"\t\\\\";
    //		cout<<endl;
    //	}
    //	cout<<"#END Inter-region Connectivity Prob\n";
    //	cout<<endl;
    //	cout<<endl;
    //
    //
    //	cout<<"Inter-region Connectivity Bias\n";
    //	cout<<"\t"<<"% Nodes"<<"\t&\t"<<"% Edges"<<"\t&&";
    //	for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //		cout<<"\t$R"<<srcRegion<<"$\t&";
    //	}
    //	cout<<"&\t"<< "LLC_Percentage" <<"\t&";
    //	cout<<"\t"<< "inLCC_avgDeg" <<"\t&";
    //	cout<<"\t"<< "outLCC_avgDeg" <<"\t\\\\";
    //	cout<<endl;
    //	for (int dstRegion=0;dstRegion<set_no;dstRegion++) {
    //		cout<<"$R"<<dstRegion<<"$";
    //		cout<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&\t"<<(regionDeg[dstRegion]*100.0)/(graphDeg*1.0)<<"\t&&";
    //		for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //			cout<<"\t"<<((edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion])-((regionDeg[dstRegion]*100.0)/(graphDeg*1.0)))/((regionDeg[dstRegion]*100.0)/(graphDeg*1.0))<<"\t&";
    //		}
    //		cout<<"&\t"<< LLC_Percentage[dstRegion] <<"\t&";
    //		cout<<"\t"<< inLCC_avgDeg[dstRegion] <<"\t&";
    //		cout<<"\t"<< outLCC_avgDeg[dstRegion] <<"\t\\\\";
    //		cout<<endl;
    //	}
    //	cout<<endl;
    //	cout<<endl;


    //	cout<<"#Begin Pajek"<<endl;
    //	cout<<"*Vertices "<<set_no<<endl;
    //	for (int dstRegion=0;dstRegion<set_no;dstRegion++) {
    //		float size=log10(1+partSize[dstRegion]*100.0/graph.noNodes)/2;
    //		float shade=log10(1+regionDeg[dstRegion]*100.0/graphDeg)/2;
    //		cout<<dstRegion+1<<" \""<<setNames[dstRegion]<<"\" 0.0 0.0 0.0 x_fact "<<size<<" y_fact "<<size<<" ic RGB ("
    //				<<shade<<","<<shade<<","<<shade<<")"<<endl;
    //	}
    //	cout<<"*Edges"<<endl;
    //	for (int srcRegion=0;srcRegion<set_no;srcRegion++) {
    //		for (int dstRegion=0;dstRegion<set_no;dstRegion++) {
    //			float width=log10(1+edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion])/2;
    //			cout<<srcRegion+1<<" "<<dstRegion+1<<" "<<width<<endl;
    //		}
    //	}
    //	cout<<"#End Pajek"<<endl;

    /* Pajek */
    //	cout<<"####Begin Pajek"<<endl;

    //    char fname[1024];
    sprintf(fname,"%s",outFile);
    strcat(fname,".gexf");
    ofile.open((const char*)fname,ios::out);

    ofile<<"<gexf xmlns=\"http://www.gexf.net/1.2draft\" xmlns:viz=\"http://www.gexf.net/1.2draft/viz\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd\" version=\"1.2\">"<<endl;

    ofile<<"<graph mode=\"static\" defaultedgetype=\"directed\">"<<endl;
    ofile<<"<nodes>"<<endl;


    vector <double> size = vector<double>(partCount,0);
    vector <double> r = vector<double>(partCount,0);
    vector <double> a = vector<double>(partCount,0);
    double aMax = 0;
    double rMax = 0;
    double sizeMax = 0;
    for (int dstRegion=0;dstRegion<partCount;dstRegion++) {
        size[dstRegion]=log10(1+log10(1+partSize[dstRegion]*100.0/graph.noNodes))*100;
        // size[dstRegion]=sqrt(partSize[dstRegion]*100.0/graph.noNodes)*10;
        r[dstRegion]=partSize[dstRegion]*100.0/graph.noNodes;
        a[dstRegion]=regionDeg[dstRegion]*100.0/graphDeg;
        aMax = aMax<a[dstRegion] ? a[dstRegion] : aMax;
        rMax = rMax<r[dstRegion] ? r[dstRegion] : rMax;
        sizeMax = sizeMax<size[dstRegion] ? size[dstRegion] : sizeMax;
    }
    for (int dstRegion=0;dstRegion<partCount;dstRegion++) {
        ofile<<"\t<node id=\""<<dstRegion<<"\" label=\"R"<<dstRegion<<"\">"<<endl;
        ofile<<"\t\t<viz:size value=\""<<size[dstRegion]<<"\"/>"<<endl;
//        ofile<<"\t\t<viz:size value=\""<<100<<"\"/>"<<endl;
//        ofile<<"\t\t<viz:color r=\""<<int((1-size[dstRegion]/sizeMax)*155)<<"\" g=\""<<int((1-size[dstRegion]/sizeMax)*155)<<"\" b=\""<<int((1-size[dstRegion]/sizeMax)*155)<<"\" a=\""<<1<<"\"/>"<<endl;
        ofile<<"\t\t<viz:color r=\""<<0<<"\" g=\""<<0<<"\" b=\""<<int((1-r[dstRegion]/rMax)*155)+100<<"\" a=\""<<1<<"\"/>"<<endl;
        ofile<<"\t\t<viz:shape value=\"disc\"/>"<<endl;
        ofile<<"\t</node>"<<endl;
    }
    ofile<<"</nodes>"<<endl;
    ofile<<"<edges>"<<endl;
    int c=0;
    for (int srcRegion=0;srcRegion<partCount;srcRegion++) {
        vector <double> thick = vector<double>(partCount);
        vector <double> r = vector<double>(partCount);
        vector <double> b = vector<double>(partCount);
        vector <double> a = vector<double>(partCount);
        double aMax = 0;
        for (int dstRegion=0;dstRegion<partCount;dstRegion++) {
//			thick[dstRegion] = ( edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion] )*log10(regionDeg[srcRegion]);
            thick[dstRegion] = ( edgeCount[srcRegion][dstRegion]/regionDeg[srcRegion] );
            r[dstRegion]     = ( edgeCount[srcRegion][dstRegion]/regionDeg[srcRegion] );
            a[dstRegion]     = ( edgeCount[srcRegion][dstRegion]/regionDeg[srcRegion] );
            aMax = aMax<a[dstRegion] ? a[dstRegion] : aMax;
//			float thickness=edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion];
//			float shade=((edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion])-((regionDeg[dstRegion]*100.0)/(graphDeg*1.0)));///((regionDeg[dstRegion]*100.0)/(graphDeg*1.0));
//			ofile<<"\t<edge id=\""<<c<<"\" source=\""<<srcRegion<<"\" target=\""<<dstRegion<<"\" label=\""<<int(thickness)<<"\" weight=\""<<thickness/10<<"\">"<<endl;
//			if (shade>0)
//				ofile<<"\t\t<viz:color r=\""<<min(255,int(shade*256))<<"\" g=\""<<0<<"\" b=\""<<0<<"\" a=\""<<1-thickness/100<<"\"/>"<<endl;
//			else
//				ofile<<"\t\t<viz:color r=\""<<0<<"\" g=\""<<0<<"\" b=\""<<min(255,int(abs(shade*256)))<<"\" a=\""<<1-thickness/100<<"\"/>"<<endl;
//			ofile<<"\t</edge>"<<endl;
//			c++;
        }

        for (int dstRegion=0;dstRegion<partCount;dstRegion++) {
            if (dstRegion==srcRegion) continue;
            ofile<<"\t<edge id=\""<<c<<"\" source=\""<<srcRegion<<"\" target=\""<<dstRegion<<
                   "\" label=\""<<edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion]<<"-"<<edgeCount[srcRegion][dstRegion]*100.0/graph.noEdges<<
                   "\" weight=\""<<thick[dstRegion]*50<<"\">"<<endl;
//			ofile<<"\t\t<viz:color r=\""<<int(r[dstRegion]*256)<<"\" g=\""<<0<<"\" b=\""<<0<<"\" a=\""<<int((aMax-a[dstRegion])*256)<<"\"/>"<<endl;
//            ofile<<"\t\t<viz:color r=\""<<int((1-r[dstRegion])*155)<<"\" g=\""<<int((1-r[dstRegion])*155)<<"\" b=\""<<int((1-r[dstRegion])*155)<<"\" a=\""<<1<<"\"/>"<<endl;
            ofile<<"\t\t<viz:color r=\""<<int((1-r[dstRegion])*255)<<"\" g=\""<<0<<"\" b=\""<<0<<"\" a=\""<<1<<"\"/>"<<endl;
            ofile<<"\t</edge>"<<endl;
            c++;
        }
    }
    ofile<<"</edges>"<<endl;
    ofile<<"</graph>"<<endl;
    ofile<<"</gexf>"<<endl;
    ofile.close();


    cout<<"END Full check region"<<endl;

    //	cout<<"XXXXX matrix"<<endl;
    //	for (int dstRegion=0;dstRegion<set_no;dstRegion++)
    //	{
    //		cout<<dstRegion<<"\t"<<setNames[dstRegion]<<"\t";
    //		cout<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&\t"<<(regionDeg[dstRegion]*100.0)/(graphDeg*1.0);
    //		for (int srcRegion=0;srcRegion<set_no;srcRegion++)
    //		{
    //			cout<<"\t"<<((edgeCount[srcRegion][dstRegion]*100.0/regionDeg[srcRegion])-((regionDeg[dstRegion]*100.0)/(graphDeg*1.0)))/((regionDeg[dstRegion]*100.0)/(graphDeg*1.0))<<"\t&";
    //		}
    //		cout<<endl;
    //	}
    //	cout<<endl;
    //	cout<<endl;
    return;
}

void RegionBFS(int center,Graph &g, bool relaxed, double &looseHalfEdges) {
    const int region=g.nodesLst[center].getRegion();
    looseHalfEdges=0;
    g.nodesLst[center].hop=0;
    std::queue <int> Q;
    Q.push(center);

    int pushedThisTime=0;
    int totalChecked=0;
    int old_hop_count =	g.nodesLst[center].hop;
    while(!Q.empty()) {
        int head=Q.front();
        //        if (old_hop_count!=g.nodesLst[head].hop) {
        //            cout<<"hop "<<g.nodesLst[head].hop<<":\t";
        //            cout<<"pushedThisTime "<<pushedThisTime<<" from "<<totalChecked<<" new nodes"<<endl;
        //            cout<<"Q Size: "<<Q.size()<<endl;
        //            pushedThisTime=0;
        //            totalChecked=0;
        //            old_hop_count=g.nodesLst[head].hop;
        //        }
        Q.pop();

        for(unsigned int i=0;i<g.nodesLst[head].neighbors->size();i++) {
            int nei=g.nodesLst[head].neighbors->at(i);
            if(g.nodesLst[nei].hop==-1) {
                if (g.nodesLst[nei].getRegion()==region) {
                    g.nodesLst[nei].hop=g.nodesLst[head].hop+1;
                    //					Q.push(g.nodesLst[nei].id);
                    Q.push(nei);
                    pushedThisTime++;
                } else {
                    looseHalfEdges++;
                }
                totalChecked++;
                //                if(relaxed) {
                //                    if(g.nodesLst[head].getRegion()==region) {
                //                        g.nodesLst[nei].hop=g.nodesLst[head].hop+1;
                //						Q.push(g.nodesLst[nei].id);
                //                        pushedThisTime++;
                //                    }
                //                    else if(g.nodesLst[nei].getRegion()==region) {
                //                        g.nodesLst[nei].hop=g.nodesLst[head].hop+1;
                //						Q.push(g.nodesLst[nei].id);
                //                        pushedThisTime++;
                //                    }
                //                    else {
                //                        looseHalfEdges++;
                //					}
                //                    totalChecked++;
                //                }
                //                else {
                //                }
            }
        }
    }
}

Graph BFS(int center, Graph g) {
    g.calc();
    list <int> Q;
    int visited=0;
    Q.clear();
    int old_hop_count =	g.nodesLst[center].hop;
    g.nodesLst[center].hop=0;
    Q.push_back(g.nodesLst[center].id);
    visited++;
    int inward=0;
    int outward=0;
    int surf=0;
    int tot=0;
    printf	("#Center: %d\n",center);
    printf	("#step\tndsVstd\teIn\teSurf\teOut\teTot\n");
    while(!Q.empty()) {
        int head=Q.front();
        if(old_hop_count!=g.nodesLst[head].hop) {
            printf("%d\t",g.nodesLst[head].hop);
            //printf("#visited: %d\tid: %d\tdist: %d\t Q.sz: %d \n",visited,head,g.nodesLst[head].hop,Q.size());//<<covered<<"\t"<<head<<"\n";
            printf("%d\t",visited);//<<covered<<"\t"<<head<<"\n";
            printf("%d\t%d\t%d\t%d\n",inward,surf,outward,tot);// float(ne)/float(tot));//<<covered<<"\t"<<head<<"\n";
            fflush(stdout);
            inward=0;
            outward=0;
            surf=0;
            old_hop_count=g.nodesLst[head].hop;
        }

        Q.pop_front();

        //covered++;
        //printf("%d\t%d\t%d\n",covered,hea,Q.size());//<<covered<<"\t"<<head<<"\n";
        // if(g.nodesLst[head].hop%10==0) printf("reached distance of: %d",g.nodesLst[head].hop);
        for(unsigned int i=0;i<g.nodesLst[head].neighbors->size();i++) {
            tot++;
            int nei=g.nodesLst[head].neighbors->at(i);
            if(g.nodesLst[nei].hop==-1) {
                g.nodesLst[nei].hop=g.nodesLst[head].hop+1;
                Q.push_back(g.nodesLst[nei].id);
                visited++;
                outward++;
            } else if(g.nodesLst[nei].hop==(g.nodesLst[head].hop+1)) {
                outward++;
            } else if(g.nodesLst[nei].hop==g.nodesLst[head].hop) {
                surf++;
            } else if(g.nodesLst[nei].hop==(g.nodesLst[head].hop-1)) {
                inward++;
            }
        }
        //top++;
        // erase(Q.begin());
        //getchar();
    }
    int outSideComponent=0;
    for(unsigned i=0;i<g.nodesLst.size();i++) {
        if((g.nodesLst[i].hop==-1)&&(g.nodesLst[i].neighbors->size()>0)) {
            outSideComponent++;
        }
    }
    if (outSideComponent>0) {
        printf("Not a single component\n");
        cout<<outSideComponent<<" nodes are not in the flooded component"<<endl;
        cout<<"number of nodes: "<<g.noNodes<<endl;
        cout<<"number of singletons: "<<g.noSeprateNodes<<endl;
    } else {
        printf("A single component\n");
    }
    return(g);
}

Graph k_core_analysis(Graph g, int max_k)
{
    for (int k=1;k<=max_k;k++)
    {
        int round=0;
        int edgeremoved=0;
        do
        {
            edgeremoved=0;
            round++;
            for (unsigned int nodeID=0;nodeID<g.nodesLst.size();nodeID++)
            {
                if (g.nodesLst[nodeID].id!=nodeID)
                {
                    cout<<"WHAT THE HELL!!!!"<<endl;
                    exit(-1);
                }
                // cout<<g.nodesLst[nodeID].id<<"\t"<<nodeID<<"\t"<<g.nodesLst[nodeID].neighbors->size()<<endl;
                if(g.nodesLst[nodeID].neighbors->size()<=k)
                {
                    for(unsigned int neiID=0;neiID<g.nodesLst[nodeID].neighbors->size();neiID++)
                    {
                        int nei=g.nodesLst[nodeID].neighbors->at(neiID);
                        if(g.nodesLst[nei].removeNeighbor(nodeID)!=-1)
                            edgeremoved++;
                    }
                    g.nodesLst[nodeID].neighbors->clear();
                }
            }
            cout<<"round "<<round<<" for kCoreParam="<<k<<" finished; "<<edgeremoved<<" edges removed"<<endl;
        }while(edgeremoved>0);
    }
    return g;
}


// creates a new barabasi-albert random graph
Graph graph_new_ba(int n, int total_edges)
{
    int ba_m0 = (total_edges / n) + 1;
    Graph g;// = graph_new(n);
    // 	for (int i = 0; i <= n; i++)
    // 	{
    // 		g.nodesLst.push_back(Node(i));
    // 	}
    //
    // 	int i, j, x, y;
    // 	int edges = 0;
    // 	if (ba_m0 & 1) ba_m0++;
    // 	if (ba_m0 >= n) die();
    // 	for (i = 0; i < ba_m0; i += 2)
    // 	{
    // 		// link_new(&g->nodes[i], &g->nodes[i+1]);
    // 		link_add(g.nodesLst[i],g.nodesLst[i]);
    // 		edges++;
    // 	}
    //
    // 	for (i = ba_m0; i < n; i++)
    // 	{
    // 		for (j = 0; j < (total_edges - (edges-j)) / (n - i); j++)
    // 		{
    // 			again:
    // 			x = i;
    // 			y = rand()%(2*edges);
    // 			if (x == y) goto again;
    // 			if (link_add(g.nodesLst[x],g.nodesLst[y]) < 0)
    // 				goto again;
    // 			edges++;
    // 		}
    // 	}
    return(g);
}


// creats an erdos-renyi random graph
Graph graph_new_erdos(int n, int m)
{
    Graph g;// = graph_new(n);
    // 	for (int i = 0; i <= n; i++)
    // 	{
    // 		g.nodesLst.push_back(Node(i));
    // 	}
    //
    // 	int i, j;
    // 	double p = 2*m/((double)n*(n-1));
    // 	for (i = 0; i < n; i++)
    // 	{
    // 		for (j = i+1; j < n; j++)
    // 		{
    // 			float prob=float(rand())/float(RAND_MAX);
    // 			if (prob <= p)	link_add(g.nodesLst[i],g.nodesLst[j]);
    // 		}
    // 	}
    return(g);
}


// creates a small-world graph with n nodes and m edges
Graph graph_new_small_world(int n, int m, double p)
{
    Graph g;// = graph_new(n);
    // 	for (int i = 0; i <= n; i++)
    // 	{
    // 		g.nodesLst.push_back(Node(i));
    // 	}
    // 	int i, j,
    // 	int degree;
    // 	m -= m % n;
    // 	degree = m / n * 2;
    // 	if ((m / n) * n != m) die();
    // 	for (i = 0; i < n; i++)
    // 	{
    // 		for (j = 1; j <= degree/2; j++)
    // 		{
    // 			int left;
    // 			int right;
    // 			again:
    // 			left = &g->nodes[i];
    // 			right = &g->nodes[(i+j) % g->n];
    // 			float prob=float(rand())/float(RAND_MAX);
    // 			if (real_random() < p)
    // 				left 	= rand()%g.nodesLst.size();
    // 			prob=float(rand())/float(RAND_MAX);
    // 			if (real_random() < p)
    // 				right	= rand()%g.nodesLst.size();
    // 			if (link_add(g.nodesLst[right],g.nodesLst[right])) goto again;
    // 		}
    // 	}
    return(g);
}



void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


QDataStream &operator <<(QDataStream &ds,const Graph &g) {
    unsigned int s=g.nodesLst.size();
    ds << s;
    for (unsigned int i=0 ; i<s; i++) {
        unsigned int sl=g.nodesLst[i].neighbors->size();
        ds << sl;
        for (unsigned int j=0 ; j<sl ; j++) {
            int dest = g.nodesLst[i].neighbors->at(j);
            ds << dest;
        }
    }
    return ds;
}

QDataStream &operator >>(QDataStream &ds,Graph &g) {
    unsigned int s;
    ds >> s;
    g.nodesLst.clear();
    for (unsigned int i=0 ; i<s; i++) {
        g.nodesLst.push_back(Node(i));
    }

    for (unsigned int i=0 ; i<s; i++) {
        unsigned int sl;
        ds >> sl;
        for (unsigned int j=0 ; j<sl ; j++) {
            int dest;
            ds >> dest;
            g.nodesLst[i].neighbors->push_back(dest);
        }
    }
    return ds;
}
