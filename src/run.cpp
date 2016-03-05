#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "statistic.h"

using namespace std;

int main ( int argc, char **argv ) {
    Graph graph;
    Graph graph_tmp;

    char* inFile;
    int kCoreParam = -1;
    bool node_level = false;
    int num_nodes = 10000, num_edges = 50000, graph_type = 1;
    int hops = 1;
    int trans_len = 0;
    int ss_trans_len = 0;
    int  warmup, maxWalkLen = -1, minWalkLen = 10, stepWalkLen = 10, Debug=0;
    int simulationRounds = 10;
    int partIndex = -1;
    int initial_point=0;
    float sw_param;
    int num_walker = 10;
    float num_walker_p = 0.2;
    int num_estimatio_round = 1;
    int d_repeat = 0;
    int graph_seed = 1, walk_seed = 1, randomize = 0;
    float prob=-1;
    float portion=-1;
    float brdgW=-1;
    int margin=1;
    float coreScale=1;
    float min_disc=-1;
    float max_disc=-1;
    int vis_thrsh=1;
    double mem_thrsh=-1;
    bool excludeInit=false;
    bool measureLCC=false;
    bool fullTest=false;
    int min_group_members=-1;
    int max_group_members=-1;
    int min_nodeDegree=-1;
    int max_nodeDegree=-1;
    bool justLCC=false;
    char* RW_dist_file="-1";
    char* RW_disc_file;
    char* nodePartFile;
    int regionFileType=-1;
    bool batch=false;
    int between=-1;
    int start_node=0 ;
    char* topofile;
    char* outFile;
    char* groupfile;
    char* netType;
    char* dumpDir;
    char* walkDir;
    float cl_rate=1;
    int conv_test=0, visit_dist=0;
    unsigned min_visits=1;
    int task=-1;
    int c, dumponly = 0, dumpclust = 0, hsf_base = 5, hsf_order = 2;
    int num_mrwb = 1, num_snow = 1;
    int center=-1, ignoreInitial = 1;
    netType = dumpDir = walkDir = "-1";
    for ( c=1 ; c < argc ; c++ ) {
        if		( !strcmp ( argv[c], "-nodes"	)) num_nodes	= atoi ( argv[++c] );//Number of nodes
        else if ( !strcmp ( argv[c], "-task"	)) task		= atoi ( argv[++c] );//selects what the code should do
        else if ( !strcmp ( argv[c], "-excludeInit"	)) excludeInit		= true;// exclude 10 initial steps in visit count
        else if ( !strcmp ( argv[c], "-edges"	)) num_edges	= atoi ( argv[++c] );//Number of edges
        else if ( !strcmp ( argv[c], "-model"	)) graph_type	= atoi ( argv[++c] );//Graph type
        else if ( !strcmp ( argv[c], "-hops"	)) hops		= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-wlen"	)) maxWalkLen	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-minWlen"	)) minWalkLen	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-stpWlen"	)) stepWalkLen	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-sw"		)) sw_param	= atof ( argv[++c] );//SW param
        else if ( !strcmp ( argv[c], "-mrwb"	)) num_mrwb	= atoi ( argv[++c] ); // Number of MRW samplers
        else if ( !strcmp ( argv[c], "-pidx"	)) partIndex	= atoi ( argv[++c] ); // Number of MRW samplers
        else if ( !strcmp ( argv[c], "-snow"	)) num_snow	= atoi ( argv[++c] ); // Number of RDS samplers
        else if ( !strcmp ( argv[c], "-warmup"	)) warmup 		= atoi ( argv[++c] ); //How many initial hops before saving samples
        else if ( !strcmp ( argv[c], "-nw"		)) num_walker	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-nwp"	)) num_walker_p	= atof ( argv[++c] );
        else if ( !strcmp ( argv[c], "-trlen"	)) trans_len	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-sstrlen"	)) ss_trans_len	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-vis_thrsh" )) vis_thrsh	= atoi ( argv[++c] );//only counts walks with visits more than the threshold
        else if ( !strcmp ( argv[c], "-mem_thrsh" )) mem_thrsh	= atof ( argv[++c] );//only counts groups with # members more than the threshold
        else if ( !strcmp ( argv[c], "-min_grp_pop" )) min_group_members = atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-max_grp_pop" )) max_group_members = atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-min_nd_deg" )) min_nodeDegree = atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-max_nd_deg" )) max_nodeDegree = atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-prob"	)) prob		= atof ( argv[++c] );//randomization probability
        else if ( !strcmp ( argv[c], "-minDisc"	)) min_disc	= atof ( argv[++c] );//minDiscovery rate
        else if ( !strcmp ( argv[c], "-maxDisc"	)) max_disc	= atof ( argv[++c] );//maxDiscovery rate
        else if ( !strcmp ( argv[c], "-portion"	)) portion		= atof ( argv[++c] );//randomization probability defines size
        else if ( !strcmp ( argv[c], "-brdgW"	)) brdgW		= atof ( argv[++c] );//randomization probability defines reduction in width of the connecting bridge
        else if ( !strcmp ( argv[c], "-cent"	)) center		= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-ignoreInit")) ignoreInitial	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-cScale"	)) coreScale	= atof ( argv[++c] );
        else if ( !strcmp ( argv[c], "-NDLVL"	)) node_level	= true;
        else if ( !strcmp ( argv[c], "-batch"	)) batch		= true;
        else if ( !strcmp ( argv[c], "-bet"		)) between		= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-justLCC"	)) justLCC		= true;
        else if ( !strcmp ( argv[c], "-calcLCC"	)) measureLCC	= true;
        else if ( !strcmp ( argv[c], "-full"	)) fullTest	= true;
        else if ( !strcmp ( argv[c], "-e"		)) num_estimatio_round	= atoi ( argv[++c] );
        else if ( !strcmp ( argv[c], "-dumponly")) dumponly 	= 1;
        else if ( !strcmp ( argv[c], "-debug"	)) Debug		= 1;
        else if ( !strcmp ( argv[c], "-start"	)) start_node 	= atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-cl_rate"	)) cl_rate 	= atof(argv[++c]); //Percentage of nodes considered for clustering
        else if ( !strcmp ( argv[c], "-topofile")) topofile 	= argv[++c];
        else if ( !strcmp ( argv[c], "-groupfile")) groupfile 	= argv[++c];
        else if ( !strcmp ( argv[c], "-outfile" )) outFile 	= argv[++c];
        else if ( !strcmp ( argv[c], "-nettype"	)) netType 	= argv[++c];
        else if ( !strcmp ( argv[c], "-dumpdir"	)) dumpDir 	= argv[++c];
        else if ( !strcmp ( argv[c], "-walkdir"	)) walkDir 	= argv[++c];
        else if ( !strcmp ( argv[c], "-margin"	)) margin	 	= atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-inFile"	)) inFile	 	= argv[++c];
        else if ( !strcmp ( argv[c], "-visits"	)) visit_dist 	= 1 ;
        else if ( !strcmp ( argv[c], "-diam")) d_repeat 	= atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-randomize")) randomize = atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-conv")) { conv_test = 1 ; min_visits = atoi(argv[++c]); }
        else if ( !strcmp ( argv[c], "-walk_seed")) walk_seed = atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-graph_seed")) graph_seed = atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-dumpclust")) dumpclust 	= 1;
        else if ( !strcmp ( argv[c], "-RWdiscFile")) RW_disc_file=argv[++c];
        else if ( !strcmp ( argv[c], "-RWdistFiles")) {
            RW_dist_file=argv[++c];
            regionFileType=0;
        } else if ( !strcmp ( argv[c], "-part")) {
            nodePartFile=argv[++c];
            regionFileType=1;
        } else if ( !strcmp ( argv[c], "-community")) {
            nodePartFile=argv[++c];
            regionFileType=2;
        }

        else if ( !strcmp ( argv[c], "-kCoreParam"))	kCoreParam=atoi(argv[++c]);
        else if ( !strcmp ( argv[c], "-hsf"		)) {
            hsf_base = atoi ( argv[++c] );//HSF base
            hsf_order = atoi ( argv[++c] );//HSF order
        }
        else {
            printf("Unexpected option : %s\n", argv[c]);
            printf("Usage: run [options]\n");
            printf("Options: -nodes num_nodes -edges num_edges -model graph_type -hsf hsf_base hsf_order\n");
            printf("         -sw swparam -hops max_hops -samplers max_samplers -warmup warmup -repeat num_repeat\n");
            printf("         -dumponly -dumpclust -cl_rate clusting_perc -conv min_visits -visits\n");
            printf("         -debug -graph_seed n -walk_seed n -start start_node -topofile topofilename \n");
            exit(1);
        }
    }

    if (task==-1)
    {
        printf("Unexpected option : task can not be -1");
        exit (1);
    }
    switch (task)
    {
    case 1://run walks and dump
    {
        if ((strcmp(netType,"-1")==0)||(strcmp(walkDir,"-1")==0))
        {
            printf("Unexpected option : nettype AND walkdir should be defined\n");
            exit (1);
        }
        if (Debug) num_walker = 1;
        srand(graph_seed);
        switch (graph_type)
        {
        case 1:
            //g=graph_new_random(num_nodes, num_edges); break;
        case 2:
            //g=graph_new_erdos(num_nodes, num_edges); break;
        case 3:
            //g=graph_new_ba(num_nodes, num_edges); break;
        case 4:
            if (prob ==-1)
            {
                printf("Unexpected option : prob should be defined\n");
                exit (1);
            }
            graph = graph_new_load(topofile);
            printf("graph loaded, ready to be randomized.\n");
            graph = randomizeGraph(graph,prob);
            printf("graph randomized.\n");
            break;
        case 5:
            graph = graph_new_load(topofile);
            printf("graph loaded.\n");
            break; //added_edges(topofile, corefile1, corefile2, corefile3, g); break;//load_parts(nodefile, edgefile, corefile1, corefile2, corefile3, g);
        case 6:
            //g=graph_new_hsf(hsf_base,hsf_order); break;
        default:
            exit(-1);
        };
        srand(walk_seed);
        for (int i=0 ; i < num_walker ; i++)
        {
            //intf ("got here\n");
            char fname[1024];

            sprintf(fname,"%s/%s/rawDump_L%d_%d_%s.txt",walkDir,netType,maxWalkLen,i,netType);
            do
            {
                initial_point=rand()%graph.nodesLst.size();
            }while (graph.nodesLst[initial_point].neighbors->size()==0);
            randomWalkAndDump(graph,maxWalkLen,initial_point,fname);
        }
    }
        break;
    case 2: //read walks and aggregate them using normal methods
    {
        vector < vector <WalkedNode> > visits;
        visits=aggregateWalkers(num_walker,maxWalkLen); // this vector has node's degree as the 1st item
        vector <float> avgDeg;
        double sum;
        for (int currentVisitedCnt=0; currentVisitedCnt<num_walker; currentVisitedCnt++)
        {
            sum=0;
            if (visits[currentVisitedCnt].size()>0)
            {
                char fname[1024];
                sprintf(fname,"visited_%d.txt",currentVisitedCnt+1);
                FILE *oFile = fopen(fname, "w");
                fprintf(oFile,"#number of nodes %d\n",visits[currentVisitedCnt].size());
                for (int i=0;i<visits[currentVisitedCnt].size() ; i++)
                {
                    sum+=visits[currentVisitedCnt][i].degree;
                    fprintf(oFile,"%u\t%d\t%d",visits[currentVisitedCnt][i].id,visits[currentVisitedCnt][i].degree,visits[currentVisitedCnt][i].vis);
                    for (int j=0; j<visits[currentVisitedCnt][i].visitedWalks.size(); j++)
                    {
                        //if (visits[currentVisitedCnt][i].visibility[j]>1)
                        //{
                        fprintf(oFile,"\t%d",visits[currentVisitedCnt][i].visitedWalks[j]);
                        //}
                    }
                    fprintf(oFile,"\n");
                }
                fclose(oFile);
                avgDeg.push_back(sum/visits[currentVisitedCnt].size());
            } else {
                avgDeg.push_back(0);
            }

        }
        for (int i=0; i<avgDeg.size(); i++) {
            char fname[1024];
            sprintf(fname,"avgDegree.txt");
            FILE *oFile = fopen(fname, "w");
            fprintf(oFile,"#visited\tavgDegree\tnumber\n");
            for (int i=0; i<avgDeg.size(); i++) {
                fprintf(oFile,"%d\t%f\t%d\n",i+1,avgDeg[i],visits[i].size());
            }
            fclose(oFile);
        }
    }
        break;
    case 3: { //read walks and aggregate them using moji's methods based on visiblity
        vector < vector <WalkedNode> > visits;
        vector <float> avgDeg;
        double sum=0;
        visits=aggregateWalkers_visibles_conv(num_walker,maxWalkLen); // this vector has node's degree as the 1st item
        //	int currentVisitedCnt=1;//visits[0].vis;//visitedWalks.size();
        //int lastVisitedCnt=visits[0].vis;//itedWalks.size();
        for (int currentVisitedCnt=0; currentVisitedCnt<num_walker; currentVisitedCnt++) {
            if (visits[currentVisitedCnt].size()>0) {
                sum=0;
                char fname[1024];
                sprintf(fname,"visitedVisible_%d.txt",currentVisitedCnt+1);
                FILE *oFile = fopen(fname, "w");
                fprintf(oFile,"#number of nodes %d\n",visits[currentVisitedCnt].size());
                for (int i=0;i<visits[currentVisitedCnt].size() ; i++) {
                    sum+=visits[currentVisitedCnt][i].degree;
                    fprintf(oFile,"%u\t%d\t%d",visits[currentVisitedCnt][i].id,visits[currentVisitedCnt][i].degree,visits[currentVisitedCnt][i].vis);
                    for (int j=0; j<visits[currentVisitedCnt][i].visitedWalks.size(); j++)
                    {
                        if (visits[currentVisitedCnt][i].stat[visits[currentVisitedCnt][i].visitedWalks[j]].visibility>1)
                        {
                            fprintf(oFile,"\t%d",visits[currentVisitedCnt][i].visitedWalks[j]);
                        }
                    }
                    fprintf(oFile,"\n");
                }
                fclose(oFile);
                avgDeg.push_back(sum/visits[currentVisitedCnt].size());
            }
            else
            {
                avgDeg.push_back(0);
            }
        }
        for (int i=0; i<avgDeg.size(); i++)
        {
            char fname[1024];
            sprintf(fname,"avgDegreeVisible.txt");
            FILE *oFile = fopen(fname, "w");
            fprintf(oFile,"#visited\tavgDegree\tnumber\n");
            for (int i=0; i<avgDeg.size(); i++)
            {
                fprintf(oFile,"%d\t%f\t%d\n",i+1,avgDeg[i],visits[i].size());
            }
            fclose(oFile);
        }
    }
        break;
    case 4: { //overlap check with multiple number of walks
        vector < vector <WalkedNode> > visits;
        vector <float> avgDeg;
        vector <int> coreSize;
        vector <int> overlap;
        int sum;
        double avg=0;
        int step=5;
        int strt=0;
        int ovlap;
        vector <WalkedNode> history;
        vector <WalkedNode> newVis;
        history.clear();
        newVis.clear();
        // for (num_walker=1;strt-num_walker>0;num_walker+=step)
        for (num_walker=1;strt+num_walker<200;num_walker+=step) {
            //avgDeg.clear();
            visits.clear();

            visits=aggregateWalkers_visibles(num_walker,maxWalkLen,strt);
            // strt=strt-num_walker-1;
            // strt=strt+num_walker;
            for (int currentVisitedCnt=num_walker-1; currentVisitedCnt<num_walker; currentVisitedCnt++) {
                ovlap=0;
                newVis=visits.back();
                coreSize.push_back(visits.back().size());

                if ((history.size()>0)&&(newVis.size()>0)) {
                    int i=0,j=0;
                    while ((i<newVis.size())&&(j<history.size())) {
                        if (newVis[i].id < history[j].id)
                            i++;
                        else if (newVis[i].id > history[j].id)
                            j++;
                        else if (newVis[i].id == history[j].id) {
                            i++;
                            j++;
                            ovlap++;
                        }
                    }
                }
                overlap.push_back(ovlap);
                history=visits.back();

                if (visits[currentVisitedCnt].size()>0) {
                    avg=0;
                    sum=0;
                    char fname[1024];
                    sprintf(fname,"visitedVisible[%d,%d].txt",num_walker,currentVisitedCnt+1);
                    FILE *oFile = fopen(fname, "w");
                    fprintf(oFile,"#number of nodes %d\n",visits[currentVisitedCnt].size());
                    for (int i=0 ; i<visits[currentVisitedCnt].size() ; i++) {
                        //avg=((i*avg)+(visits[currentVisitedCnt][i].degree*1.0))/((i+1)*1.0);
                        sum=sum+visits[currentVisitedCnt][i].degree;
                        fprintf(oFile,"%u\t%d\t%d",visits[currentVisitedCnt][i].id,visits[currentVisitedCnt][i].degree,visits[currentVisitedCnt][i].vis);
                        // for (int j=0; j<visits[currentVisitedCnt][i].visitedWalks.size(); j++)
                        // {
                        // 	if (visits[currentVisitedCnt][i].visibility[j]>1)
                        // 	{
                        // 		fprintf(oFile,"\t%d",visits[currentVisitedCnt][i].visitedWalks[j]);
                        // 	}
                        // }
                        fprintf(oFile,"\n");
                    }
                    fclose(oFile);
                    avg=(double)sum/(double)visits[currentVisitedCnt].size();
                    avgDeg.push_back(avg);
                } else {
                    avgDeg.push_back(0);
                }
            }
            char fname[1024];
            sprintf(fname,"coreSize.txt");
            FILE *oFile = fopen(fname, "w");
            fprintf(oFile,"#walkers\tsize\toverlap\tavgDeg\n");
            for (int i=0; i<coreSize.size(); i++) {
                fprintf(oFile,"%d\t%d\t%d\t%f\n",i+1,coreSize[i],overlap[i],avgDeg[i]);
            }
            fclose(oFile);
        }
        break;
    }

    case 5: {
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)) {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        aggregateWalkers_visibles_pop(num_walker,maxWalkLen,dumpDir,netType,coreScale);
        break;
    }
    case 6: {//run walk and process
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)) {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        srand(graph_seed);
        switch (graph_type) {
        case 3: {
            if ((prob==-1)||(portion==-1)||(brdgW==-1)) {
                printf("Unexpected option : prob and portion and brdgW should be defined\n");
                exit (1);
            }
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            graph_tmp=graph;

            begin_t=clock();
            graph = randomizeGraph(graph,prob);
            end_t=clock();
            printf("# graph Randomized in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            graph = graphSynthesize(graph,portion,brdgW);
            checkDegreeDistrib(graph,graph_tmp);
            end_t=clock();
            printf("# graph Synthesized and checked in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            checkDegreeDistrib(graph,graph_tmp);
            char file_name[128];
            sprintf(file_name,"%s/synthetic_%s_%f_%f.txt",dumpDir,netType,portion,brdgW);
            dumpGraph(graph,file_name);
            end_t=clock();
            printf("# graph Checked and Dumped in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 4: {
            if (prob==-1) {
                printf("Unexpected option : prob should be defined\n");
                exit (1);
            }
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            graph_tmp=graph;

            begin_t=clock();
            graph = randomizeGraph(graph,prob);
            end_t=clock();
            printf("graph Randomized in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            checkDegreeDistrib(graph,graph_tmp);
            char file_name[128];
            sprintf(file_name,"%s/randomized_%s.txt",dumpDir,netType);
            dumpGraph(graph,file_name);
            end_t=clock();
            printf("graph Checked and Dumped in %f secs.\n",double(diffclock(end_t,begin_t)));

            break;
        }
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break; //added_edges(topofile, corefile1, corefile2, corefile3, g); break;//load_parts(nodefile, edgefile, corefile1, corefile2, corefile3, g);
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        // srand(walk_seed);
        if (batch) {
            unsigned begin_t=clock();
            randomWalkAndProcess(graph, maxWalkLen, num_walker_p, dumpDir, netType,walk_seed, excludeInit);
            unsigned end_t=clock();
            printf(">> WalkAbout ran in %f secs. <<\n",double(diffclock(end_t,begin_t)));
        } else {
            randomWalkAndLog(graph,num_walker,maxWalkLen, dumpDir, netType,walk_seed);
        }
        break;
    }
    case 7: //BFS discovery rate
    {
        if (!(((center!=-1)&&(strcmp(dumpDir,"-1")!=0))||(strcmp(RW_disc_file,"-1")!=0))) {
            printf("Unexpected option : nettype AND walkdir or RWdiscFile should be defined\n");
            exit (1);
        }
        switch (graph_type) {
        case 4: {
            if (prob ==-1) {
                printf("Unexpected option : prob should be defined\n");
                exit (1);
            }
            graph = graph_new_load(topofile);
            printf("graph loaded, ready to be randomized.\n");
            graph = randomizeGraph(graph,prob);
            printf("graph randomized.\n");
            break;
        }
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        if (center==-1) {
            cout<<"Random Walk Discovery Rate"<<endl;
            randomWalkDiscoveryRate(graph,num_walker,maxWalkLen,walk_seed,RW_disc_file);
        } else {
            unsigned int maxDeg=0;
            if (center==-2)	{
                for (int i=0;i<graph.nodesLst.size();i++) {
                    if (graph.nodesLst[i].neighbors->size()>maxDeg) {
                        maxDeg=graph.nodesLst[i].neighbors->size();
                        center=i;
                    }
                }
            }
            cout<<"Highest degree node: "<<center<<" - max deg: "<<maxDeg<<endl;
            graph = BFS(center,graph);
            if (strcmp(dumpDir,"-1")!=0) {
                char fileName[128];
                sprintf(fileName,"%s/%s_CC.txt",dumpDir,netType);
                dumpGraph_CC(graph, fileName);
            }

        }
        break;
    }
    case 8: { // RW distance and transitions
        if((nodePartFile == NULL) && (RW_dist_file == NULL) && (center==-1)) {
            printf("Unexpected option : either RWdistFile or a center node should be provided\n");
        }
        srand(graph_seed);
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            printf("Loading graph from bin file...\n");
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        cout<<"here"<<endl;

        if (center!=-1) {
            randomWalkDistanceCenter(graph,num_walker,maxWalkLen,walk_seed,RW_dist_file,center);
        } else if (node_level) {
            randomWalkTopolgyHigh(graph,num_walker,maxWalkLen,dumpDir,netType,walk_seed,RW_dist_file);
        } else if (ss_trans_len!=0)	{
            randomWalkSuperSpaceTransitionsSets(graph,num_walker,maxWalkLen,ss_trans_len,walk_seed,RW_dist_file);
        } else if (trans_len!=0) {
            if (regionFileType==0) {
                randomWalkTransitionsSets(graph, num_walker_p, maxWalkLen, walk_seed, RW_dist_file, regionFileType);
            } else {
                randomWalkTransitionsSets(graph, num_walker_p, maxWalkLen, walk_seed, nodePartFile, regionFileType);
            }
        } else {
            randomWalkDistanceSets(graph, num_walker, maxWalkLen, walk_seed, RW_dist_file);
        }
        break;
    }
    case 9://RW new nodes visited count
    {
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0))
        {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        srand(graph_seed);
        switch (graph_type)
        {
        case 5:
            graph = graph_new_load(topofile);
            printf("Graph Loaded\n");
            break; //added_edges(topofile, corefile1, corefile2, corefile3, g); break;//load_parts(nodefile, edgefile, corefile1, corefile2, corefile3, g);
        default:
            exit(-1);
        };
        randomWalkDiscoveredNodes(graph,num_walker,maxWalkLen, dumpDir,netType,walk_seed);
        break;
    }
    case 10://RW and Process but only walks with discovery rate satisfying an thresh
    {
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)||(min_disc==-1)||(max_disc==-1))
        {
            printf("Unexpected option : nettype AND dumpdir AND minDisc should be defined\n");
            exit (1);
        }
        srand(graph_seed);
        switch (graph_type)
        {
        case 5:
            graph = graph_new_load(topofile);
            printf("Graph Loaded\n");
            break; //added_edges(topofile, corefile1, corefile2, corefile3, g); break;//load_parts(nodefile, edgefile, corefile1, corefile2, corefile3, g);
        default:
            exit(-1);
        };
        randomWalkAndProcessDiscThresh(graph,num_walker,maxWalkLen,dumpDir,netType,walk_seed,min_disc,max_disc);
        break;
    }
    case 11: {// based on group-memberships evaluate the entropy or group region membership confidence
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph new_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };


        GroupSet allGroups = graph_group_read(graph,groupfile);
        //		for (int i; i<allGroups.groupsVec.size(); i++) {
        //			cout<<allGroups.groupsVec[i].members->size()<<endl;
        //		}
        //		exit(-1);


        if (mem_thrsh==-1) {
            if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)) {
                printf("Unexpected option : nettype AND dumpdir\n");
                exit (1);
            }
            groupMembershipDump(graph,allGroups,dumpDir,netType);
        }
        else {
            if(regionFileType==-1) {
                printf("Unexpected option : regionFileType should be provided\n");
                exit (1);
            }

            if (regionFileType==0) {
                groupRegionMembershipCounter(graph, allGroups, RW_dist_file, outFile, regionFileType, mem_thrsh, min_group_members, max_group_members, min_nodeDegree, max_nodeDegree);
            } else {
                groupRegionMembershipCounter(graph, allGroups, nodePartFile, outFile, regionFileType, mem_thrsh, min_group_members, max_group_members, min_nodeDegree, max_nodeDegree);
            }
        }
        break;
    }
    case 12:// k-core analysis and dump graph
    {
        if((kCoreParam==-1)||(strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0))
        {
            printf("Unexpected option : nettype, dumpdir AND kCoreParam should be defined\n");
            exit (1);
        }
        unsigned begin_t=clock();

        graph = graph_new_load(topofile);
        unsigned end_t=clock();
        printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));

        Graph graph2 = k_core_analysis(graph, kCoreParam);
        char fileName[128];
        sprintf(fileName,"%s/%s_%d_core.txt",dumpDir,netType,kCoreParam);
        dumpGraph(graph2, fileName);
        break;
    }
    case 13:// dump for metis
    {
        if((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0))
        {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        unsigned begin_t=clock();
        graph = graph_new_load(topofile);
        unsigned end_t=clock();
        printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));

        cleanUp(graph, dumpDir, netType);

        char fileName[128];
        sprintf(fileName,"%s/%s_4Metis.txt",dumpDir,netType);
        dumpGraph4Metis(graph, fileName);
        break;
    }
    case 14://Region evaluation
    {
        if(regionFileType==-1) {
            printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");
        }
        switch (graph_type) {
        case 0: {
            unsigned begin_t=clock();
            graph = graph_load1(topofile);
            unsigned end_t=clock();
            printf("# graph old_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 1: {
            unsigned begin_t=clock();
            graph = graph_load2(topofile);
            unsigned end_t=clock();
            printf("# graph old_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph new_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };

        if (!fullTest) {
            if (regionFileType==0) /* 0 for info file and 1 for part file */
                checkRegions(graph, RW_dist_file, regionFileType, outFile, 2, measureLCC);
            else if (regionFileType==1)
                checkRegions(graph, nodePartFile, regionFileType, outFile, 2, measureLCC);
            else if (regionFileType==2)
                checkRegions(graph, nodePartFile, regionFileType, outFile, 1, measureLCC);
        } else { /* To measure inter-connectivity of regions */
            if (regionFileType==1)
                fullCheckRegions(graph, nodePartFile, regionFileType, outFile, 2);
            else if (regionFileType==2)
                fullCheckRegions(graph, nodePartFile, regionFileType, outFile, 1);
        }
        break;
    }
    case 15://Region evaluation
    {
        if((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0))
        {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        if(maxWalkLen==-1)
        {
            printf("Unexpected option : Walk length should be specified\n");
            exit (1);
        }
        switch (graph_type)
        {
        case 5:
        {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        randomWalkEscapeRate(graph, partIndex, num_walker_p, maxWalkLen, simulationRounds, dumpDir, netType, walk_seed);
        break;
    }
    case 16: { //evaluate mixing time
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)) {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        srand(graph_seed);
        switch (graph_type) {
        case 3: {
            if ((prob==-1)||(portion==-1)||(brdgW==-1)) {
                printf("Unexpected option : prob and portion and brdgW should be defined\n");
                exit (1);
            }
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            graph_tmp=graph;

            begin_t=clock();
            graph = randomizeGraph(graph,prob);
            end_t=clock();
            printf("# graph Randomized in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            graph = graphSynthesize(graph,portion,brdgW);
            checkDegreeDistrib(graph,graph_tmp);
            end_t=clock();
            printf("# graph Synthesized and checked in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            checkDegreeDistrib(graph,graph_tmp);
            char file_name[128];
            sprintf(file_name,"%s/synthetic_%s_%f_%f.txt",dumpDir,netType,portion,brdgW);
            dumpGraph(graph,file_name);
            end_t=clock();
            printf("# graph Checked and Dumped in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 4: { // randomize a graph
            if (prob==-1) {
                printf("Unexpected option : prob should be defined\n");
                exit (1);
            }
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            graph_tmp=graph;

            begin_t=clock();
            graph = randomizeGraph(graph,prob);
            end_t=clock();
            printf("graph Randomized in %f secs.\n",double(diffclock(end_t,begin_t)));

            begin_t=clock();
            checkDegreeDistrib(graph,graph_tmp);
            char file_name[128];
            sprintf(file_name,"%s/randomized_%s.txt",dumpDir,netType);
            dumpGraph(graph,file_name);
            end_t=clock();
            printf("graph Checked and Dumped in %f secs.\n",double(diffclock(end_t,begin_t)));

            break;
        }
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break; //added_edges(topofile, corefile1, corefile2, corefile3, g); break;//load_parts(nodefile, edgefile, corefile1, corefile2, corefile3, g);
        }
        default:
            exit(-1);
        };
        evalMixing( graph, num_walker_p, dumpDir, netType, walk_seed);
        break;
    }
    case 17: { // compute node assignment based on region anchors.
        if ((strcmp(netType,"-1")==0)||(strcmp(dumpDir,"-1")==0)) {
            printf("Unexpected option : nettype AND dumpdir should be defined\n");
            exit (1);
        }
        srand(graph_seed);
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("%s loaded in %f secs.\n",topofile,double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };

        unsigned begin_t_2=clock();


        if (regionFileType==0) {
            extendRegionCore(graph, num_walker, maxWalkLen, regionFileType, RW_dist_file, dumpDir, netType, justLCC, between);
        } else {
            extendRegionCore(graph, num_walker, maxWalkLen, regionFileType, nodePartFile, dumpDir, netType, justLCC, between);
        }

        unsigned end_t_2=clock();
        printf("%s partitioned in %f secs.\n",topofile,double(diffclock(end_t_2,begin_t_2)));

        break;
    }
    case 18: { /* test read file speed */
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("graph new loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            //			std::cin.get();


            begin_t=clock();
            QFile file(outFile);
            file.open(QIODevice::WriteOnly);
            QDataStream ds(&file);
            ds << graph;
            file.close();
            end_t=clock();
            printf("graph bin dumped in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            //			std::cin.get();


            begin_t=clock();
            Graph graph2;
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph2;
            end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        } }
        break;
    }
    case 19: {// based on group-memberships evaluate the entropy or group region membership confidence
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph new_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        unsigned begin_t=clock();
        GroupSet allCommunities = loadCommunities(graph,groupfile);
        unsigned end_t=clock();
        printf("loaded communities in %f secs.\n\n",double(diffclock(end_t,begin_t)));

        if(regionFileType==-1) {
            printf("Unexpected option : regionFileType should be provided\n");
            exit (1);
        }
        //		testHighDegreeCoreNeighborhood(graph, allCommunities, nodePartFile, outFile, regionFileType);
        communityRegionMembershipCounter(graph, allCommunities, nodePartFile, outFile, regionFileType);

        break;
    }
    case 20: { // RW transition Prob
        if((nodePartFile == NULL) && (RW_dist_file == NULL) && (center==-1)) {
            printf("Unexpected option : either RWdistFile, PartFile or a center node should be provided\n");
            exit(-1);
        }
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            printf("Loading graph from bin file... %s\n", topofile);
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };

        unsigned begin_t=clock();
        if (regionFileType==0) {
            partRWTransitionCounter(graph, dumpDir, netType, num_walker, maxWalkLen, walk_seed, RW_dist_file, regionFileType, outFile);
        } else {
            partRWTransitionCounter(graph, dumpDir, netType, num_walker, maxWalkLen, walk_seed, nodePartFile, regionFileType, outFile);
        }
        unsigned end_t=clock();
        printf("RW for transistions in %f secs.\n\n",double(diffclock(end_t,begin_t)));

        break;
    }
    case 21: { /* Page Rank */
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };

        unsigned begin_t=clock();
        pageRank(graph, minWalkLen, maxWalkLen, stepWalkLen, dumpDir, netType, ignoreInitial);
        unsigned end_t=clock();
        printf("RW for transistions in %f secs.\n\n",double(diffclock(end_t,begin_t)));
        break;
    }
    case 22: { /* error clustering */
        Stats stats;
        unsigned begin_t=clock();
        stats.loadPoints(inFile);
        unsigned end_t=clock();
        printf("Points loaded in %f secs.\n",double(diffclock(end_t,begin_t)));

        begin_t=clock();
        stats.cluster(outFile, margin);
        end_t=clock();
        printf("Clustering in %f secs.\n\n",double(diffclock(end_t,begin_t)));
        break;
    }
    case 23: { /* characterstics of the communities */
        switch (graph_type) {
        case 5: {
            unsigned begin_t=clock();
            graph = graph_new_load(topofile);
            unsigned end_t=clock();
            printf("# graph new_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
            break;
        }
        case 6: {
            QFile file(topofile);
            unsigned begin_t=clock();
            file.open(QIODevice::ReadOnly);
            QDataStream ds2(&file);
            ds2 >> graph;
            unsigned end_t=clock();
            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
            break;
        }
        default:
            exit(-1);
        };
        unsigned begin_t=clock();
        GroupSet allCommunities = loadCommunities(graph,groupfile);
        unsigned end_t=clock();
        printf("loaded communities in %f secs.\n\n",double(diffclock(end_t,begin_t)));
        communityProperties(graph, allCommunities, outFile);

        break;
    }
//    case 24: { /* Calcualte top N reach */
//        switch (graph_type) {
//        case 5: {
//            unsigned begin_t=clock();
//            graph = graph_new_load(topofile);
//            unsigned end_t=clock();
//            printf("# graph new_loaded in %f secs.\n",double(diffclock(end_t,begin_t)));
//            break;
//        }
//        case 6: {
//            QFile file(topofile);
//            unsigned begin_t=clock();
//            file.open(QIODevice::ReadOnly);
//            QDataStream ds2(&file);
//            ds2 >> graph;
//            unsigned end_t=clock();
//            printf("graph bin loaded in %f secs.\n\n",double(diffclock(end_t,begin_t)));
//            break;
//        }
//        default:
//            exit(-1);
//        };
//        unsigned begin_t=clock();
//        calculateHighDegreeReach();
//        unsigned end_t=clock();
//        printf("loaded communities in %f secs.\n\n",double(diffclock(end_t,begin_t)));
//        communityProperties(graph, allCommunities, outFile);
//        break;
    }
        if (dumponly) {
            displayGraph(graph);
        }
        return 0;
    }
}

