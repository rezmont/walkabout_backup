#include "walker.h"
#include "FixedSizeFIFOQ.cpp"

using namespace std;

int nextNodeIdSnowBall(Node n) {
    if (n.neighbors->size()==0) {
        printf("ERROR in graph strucuter\n");
        printf("node %d has no neighbors\n",n.id);
        return (-1);
    } else {
        int idx = rand()%n.neighbors->size();
        return n.neighbors->at(idx);
    }
}


void randomWalkAndDump(Graph graph,int walk_len,int initial_point,char *fileName) {
    FILE *oFile;
    if (( oFile = fopen(fileName, "w")) == NULL) {
        printf("error openning file : %s\n", fileName);
    }
    int cc_size=0;
    for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        if(graph.nodesLst[i].neighbors->size()>0) cc_size++;
    }
    fprintf(oFile,"#%d %d %d\n",(int)graph.noEdges,graph.noNodes,cc_size);
    int next=initial_point;
    for (int n=0; n<walk_len; n++) {
        fprintf(oFile,"%d\t%d\n",next,graph.nodesLst[next].neighbors->size());
        next=nextNodeIdSnowBall(graph.nodesLst[next]);
    }
    fclose(oFile);
}


//implementation of RW's aggregation. visited walks are stored in the visitedWalks in addition to the degree of the node visited.
//number of walkers that have visited the node is visitedWalks.size()
vector < vector <WalkedNode> > aggregateWalkers(int num_walker,int walk_len) {
    //printf("get here 1\n");

    char fileName[128];
    int vertex;
    int deg;
    int noEdges=0,noNodes=0;
    vector <WalkedNode> visited;
    int loc;
    char *line=NULL;
    size_t linesize;
    for (int n=0; n<num_walker; n++)
    {
        //printf("walk %d\n",n);
        sprintf(fileName,"rawDump_L%d_%d.txt",150000,n);

        FILE *fh = fopen(fileName, "r");
        if (getline(&line, &linesize, fh)>0)
            sscanf(line, "#%d %d", &noEdges,&noNodes);
        loc=0;
        //printf("loc: %d\n",loc);
        while (getline(&line, &linesize, fh)>0)
        {
            if ( sscanf(line, "%d\t%d", &vertex, &deg)==2 )
            {
                int last=visited.size();
                while(visited.size()<=vertex)
                {
                    visited.push_back(WalkedNode(last,0));
                    last++;
                }
                if(visited[vertex].visitedWalks.size()>0)
                {
                    if ((visited[vertex].visitedWalks.back()!=n)) visited[vertex].visitedWalks.push_back(n);
                }
                else
                {
                    //visited[vertex].push_back(vertex);
                    visited[vertex].degree=deg;//push_back(deg);
                    visited[vertex].id=vertex;
                    visited[vertex].visitedWalks.push_back(n);
                }
            }
            if (++loc==walk_len) break;
        }
        if (loc!=150000) printf("walk log was not int enough\n");
        //printf("loc: %d\n",loc);
        fclose(fh);
    }

    for (int j=0;j<visited.size();j++) {
        visited[j].vis=visited[j].visitedWalks.size();
    }

    //quickSort (visited,0,visited.size());
    vector < vector <WalkedNode> > res=classifyVisitedNodes(visited,num_walker);
    return (res);
}

//implementation of moji's idea of visited nodes. visibility greater than 1 is signal that shows that a node is visible. lastTransition stores
//the time at which node has changed its status from visible to invisible or vice versa.
vector < vector <WalkedNode> > aggregateWalkers_visibles_conv(int num_walker,int walk_len) {
    float alpha=0.5;

    int maxNoEntrance;

    char fileName[128];
    int vertex;
    int deg;
    int noEdges=0,noNodes=0;
    vector <WalkedNode> visited;
    int loc;
    int samplingStep 	= 1000;
    int latestTransition= 0;
    vector <int> convVec;
    char *line=NULL;
    size_t linesize;
    vector < vector <int> > noVisited;
    vector < vector <int> > noVisible;
    vector < vector <int> > noNewVis;
    vector < vector <int> > noNewInvis;

    for (int n=0; n<num_walker; n++)
    {
        maxNoEntrance=0;

        noVisited.push_back(vector <int>());
        noVisible.push_back(vector <int>());
        noNewVis.push_back(vector <int>());
        noNewInvis.push_back(vector <int>());

        printf("evaluating walk %d\n",n);
        sprintf(fileName,"rawDump_L%d_%d.txt",150000,n);
        FILE *fh = fopen(fileName, "r");

        if (getline(&line, &linesize, fh)>0)
            sscanf(line, "#%d %d", &noEdges,&noNodes);
        loc = 0; //walk_len;
        while (getline(&line, &linesize, fh)>0)
        {
            if ( sscanf(line, "%d\t%d", &vertex, &deg)==2 )
            {
                int last=visited.size();;
                while(visited.size()<=vertex)
                {
                    visited.push_back(WalkedNode(last,0));
                    last++;
                }
                if(visited[vertex].visitedWalks.size()>0)
                {
                    if ((visited[vertex].visitedWalks.back()!=n))
                    {
                        visited[vertex].visitedWalks.push_back(n);
                    }
                }
                else
                {
                    visited[vertex].id=vertex;
                    visited[vertex].degree=deg;

                    visited[vertex].visitedWalks.push_back(n);
                }

                visited[vertex].stat[n].noVisits++;
                visited[vertex].stat[n].lastTimeVisited=loc;
            }
            loc++;
            if (((loc%samplingStep)==0)&&(loc!=0))
            {
                int totVisted=0, totVisible=0, newV=0, newI=0;
                for (int nid=0; nid<visited.size(); nid++)
                {
                    if (visited[nid].visitedWalks.size()>0)
                    {
                        if (visited[nid].visitedWalks.back()==n)
                        {
                            totVisted++;
                            float tmp=visited[nid].stat[n].visibility;
                            visited[nid].stat[n].visibility=(visited[nid].stat[n].noVisits*1.0/loc)*((noEdges*2.0)/(visited[nid].degree*1.0));
                            if (visited[nid].stat[n].visibility<0)
                            {
                                printf("node: %d, loc: %d, noVisits: %d, degree: %d, Edges: %d, visibility: %f\n",visited[nid].id,loc,visited[nid].stat[n].noVisits,visited[nid].degree,noEdges,visited[nid].stat[n].visibility);
                                getchar();
                            }
                            if (tmp==-1)
                            {
                                if (visited[nid].stat[n].visibility>1)
                                {
                                    // newV++;
                                }
                            }
                            else
                            {
                                if ((visited[nid].stat[n].visibility-1)*(tmp-1)<0)
                                {
                                    visited[nid].stat[n].lastTransition=loc;
                                    if(tmp<1)
                                    {
                                        newV++;
                                        visited[nid].stat[n].noEnterance++;
                                        visited[nid].stat[n].lastTimeVisible=loc;

                                        if ((loc-visited[nid].stat[n].lastTimeVisited)<samplingStep)
                                        {
                                            if(visited[nid].stat[n].ewmaIntVisible==-1)
                                            {
                                                visited[nid].stat[n].ewmaIntVisible=loc-visited[nid].stat[n].lastTimeVisible;
                                            }
                                            else
                                            {
                                                visited[nid].stat[n].ewmaIntVisible=(loc-visited[nid].stat[n].lastTimeVisible)*(alpha)+(visited[nid].stat[n].ewmaIntVisible)*(1.0-alpha);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        newI++;
                                    }
                                }
                            }
                            if (visited[nid].stat[n].visibility>1)
                            {
                                totVisible++;
                            }
                            if ((loc-visited[nid].stat[n].lastTimeVisited)<samplingStep)
                            {
                                if(visited[nid].stat[n].ewmaIntVisited==-1)
                                {
                                    visited[nid].stat[n].ewmaIntVisited=loc-visited[nid].stat[n].lastTimeVisited;
                                }
                                else
                                {
                                    visited[nid].stat[n].ewmaIntVisited=(loc-visited[nid].stat[n].lastTimeVisited)*(alpha)+(visited[nid].stat[n].ewmaIntVisited)*(1-alpha);
                                }
                            }
                        }
                    }
                }
                noVisited[n].push_back(totVisted);
                noVisible[n].push_back(totVisible);
                noNewVis[n].push_back(newV);
                noNewInvis[n].push_back(newI);

                if ((walk_len-loc)<1000) break;
            }

        }

        if (loc!=walk_len) printf("walk log was not int enough\n");
        fclose(fh);

        char fname[128];
        sprintf(fname,"visChRate_%d.txt",n);
        FILE *oFile = fopen(fname, "w");
        fprintf(oFile,"#step\tvisited\tvisible\tnewVis\tnewInvis\tchangeRatio\n");
        for (int i=0; i<noVisited[n].size(); i++)
        {
            float tmp=((noNewInvis[n][i]+noNewVis[n][i])*1.0)/(noVisited[n][i]*1.0);
            fprintf(oFile,"%d\t%d\t%d\t%d\t%d\t%f\n",i,noVisited[n][i],noVisible[n][i],noNewVis[n][i],noNewInvis[n][i],tmp);
        }
        fclose(oFile);

        //char fname[128];
        sprintf(fname,"EntreanceDegree_%d.txt",n);
        //FILE *
        oFile = fopen(fname, "w");
        fprintf(oFile,"#NID\tVis\tnoVis\tEnter\tDegree\tewmaVisit\tewmaVisib\n");
        for (int i=0; i<visited.size(); i++)
        {
            if (visited[i].visitedWalks.size()>0)
            {
                if (visited[i].visitedWalks.back()==n)
                {
                    if (visited[i].stat[n].noEnterance>1)
                    {
                        fprintf(oFile,"%d\t%f\t%d\t%d\t%d\t%f\t%f\n",visited[i].id,visited[i].stat[n].visibility,visited[i].stat[n].noVisits,visited[i].stat[n].noEnterance,visited[i].degree,visited[i].stat[n].ewmaIntVisited,visited[i].stat[n].ewmaIntVisible);
                    }
                }
            }
            //float tmp=((noNewInvis[n][i]+noNewVis[n][i])*1.0)/(noVisited[n][i]*1.0);
        }
        fclose(oFile);
    }


    // for (int j=0;j<num_walker;j++)
    // {
    // 	convVec.push_back(0);
    // }
    for (int j=0;j<visited.size() ; j++)
    {
        for (int k=0;k<visited[j].visitedWalks.size() ; k++)
        {
            if (visited[j].stat[visited[j].visitedWalks[k]].visibility>1)
            {
                visited[j].vis++;
                // if (latestTransition<visited[j].lastTransition[k])
                // 	latestTransition=visited[j].lastTransition[k];
                // if (convVec[visited[j].visitedWalks[k]]<visited[j].lastTransition[k])
                // 	convVec[visited[j].visitedWalks[k]]=visited[j].lastTransition[k];
            }
        }
    }
    //quickSort (visited,0,visited.size());
    vector < vector <WalkedNode> > res=classifyVisitedNodes(visited,num_walker);
    return (res);
}

//implementation of moji's idea of visited nodes. visibility greater than 1 is signal that shows that a node is visible. lastTransition stores
//the time at which node has changed its status from visible to invisible or vice versa.
void aggregateWalkers_visibles_pop(int num_walker,int walk_len, char* dumpdir, char* nettype, float coreScale) {
    int coreP=coreScale*num_walker;
    printf("Core evaluation [%d,%d]\n",coreP,num_walker);
    char fileName[128];
    int vertex;
    int deg;
    int noEdges=0,noNodes=0;

    vector <WalkedNode> visited;
    int samplingStep=10000;
    char *line=NULL;
    size_t linesize;
    vector < vector <int> > noVisited;
    vector < vector <int> > noVisible;
    vector < vector <int> > noNewVis;
    vector < vector <int> > noNewInvis;

    vector <int> newCore;
    vector <int> oldCore;

    vector <int> new_core_coarse;
    vector <int> old_Core_coarse;
    vector <int> overlap_trnd_coarse;
    vector <int> core_trnd_coarse;

    vector <int> coreSize;

    vector <int> coreDegree0;
    vector <int> coreDegree10;
    vector <int> coreDegree50;
    vector <int> coreDegree90;
    vector <int> coreDegree100;

    vector <int> added;
    vector <int> removed;


    vector < vector <int> > removed_core_prop;
    vector < vector <double> > sigma_degree_ratio;


    int* loc=new int [num_walker];
    vector <int> est_no_edges;
    vector <int> est_no_edges_old;//=new int [num_walker];
    vector < FILE* > fh;
    for (int n=0; n<num_walker; n++)
    {
        printf("openning walk %d\n",n);
        sprintf(fileName,"%s/%s/rawDump_L5.0M_%d_%s.txt",dumpdir,nettype,n,nettype);
        fh.push_back(fopen(fileName, "r"));
        if (fh[fh.size()-1] == NULL)
        {
            printf("error openning file : %s\n", fileName);
        }
        if (getline(&line, &linesize, fh[n])>0)
            sscanf(line, "#%d %d", &noEdges,&noNodes);
        loc[n] = 1;
        est_no_edges.push_back(0);
    }

    while(loc[num_walker-1]<=walk_len)
    {
        for (int n=0; n<num_walker; n++)
        {
            printf("eval walk %d step %d\n",n,loc[n]);
            int nextsample=loc[n]+samplingStep;
            for(;loc[n]<nextsample;loc[n]++)
            {
                if (getline(&line, &linesize, fh[n])>0)
                {
                    if ( sscanf(line, "%d\t%d", &vertex, &deg)==2 )
                    {
                        int last=visited.size();;
                        while(visited.size()<=vertex)
                        {
                            visited.push_back(WalkedNode(last,0));
                            last++;
                        }
                        if(visited[vertex].visitedWalks.size()>0)
                        {
                            bool alreadyVisited=false;
                            for (int j=0;j<visited[vertex].visitedWalks.size();j++)
                                if ((visited[vertex].visitedWalks[j]==n))
                                    alreadyVisited=true;
                            if (alreadyVisited!=true)
                            {
                                visited[vertex].visitedWalks.push_back(n);
                                est_no_edges[n]+=deg;
                            }
                        }
                        else
                        {
                            visited[vertex].id=vertex;
                            visited[vertex].degree=deg;
                            est_no_edges[n]+=deg;

                            visited[vertex].visitedWalks.push_back(n);
                        }
                        visited[vertex].stat[n].noVisits++;
                    }
                }
            }
            for (int nid=0; nid<visited.size(); nid++)
            {
                if (visited[nid].visitedWalks.size()>0)
                {
                    if (visited[nid].stat.find(n) != visited[nid].stat.end())
                    {
                        visited[nid].stat[n].visibility=(visited[nid].stat[n].noVisits*1.0/loc[n])*((est_no_edges[n]*1.0)/(visited[nid].degree*1.0));
                        if (visited[nid].stat[n].visibility<0)
                        {
                            printf("node: %d, loc: %d, noVisits: %d, degree: %d, Edges: %d, visibility: %f\n",visited[nid].id,loc[n],visited[nid].stat[n].noVisits,visited[nid].degree,noEdges,visited[nid].stat[n].visibility);
                            getchar();
                        }
                    }
                }
            }
            if(n==(num_walker-1))
            {
                vector <int> degree_pool;
                degree_pool.clear();
                oldCore=newCore;
                newCore.clear();
                for (int j=0;j<visited.size() ; j++)
                {
                    visited[j].vis=0;
                    for (int k=0;k<visited[j].visitedWalks.size() ; k++)
                    {
                        if (visited[j].stat[visited[j].visitedWalks[k]].visibility>1)
                        {
                            visited[j].vis++;
                        }
                    }
                    if(visited[j].vis>=(coreScale*num_walker))
                    {
                        newCore.push_back(visited[j].id);
                        degree_pool.push_back(visited[j].degree);
                    }
                }

                vector <int> rmvd_vis;
                rmvd_vis.clear();
                for (int k=0;k<num_walker;k++)
                {
                    rmvd_vis.push_back(0);
                }
                int ovlap=0;
                if ((oldCore.size()>0)&&(newCore.size()>0))
                {
                    int i=0,j=0;
                    while ((i<newCore.size())&&(j<oldCore.size()))
                    {
                        if (newCore[i]< oldCore[j])
                            i++;
                        else if (newCore[i]> oldCore[j])
                        {
                            rmvd_vis[visited[oldCore[j]].vis]++;
                            j++;
                        }
                        else if (newCore[i] == oldCore[j])
                        {
                            i++;
                            j++;
                            ovlap++;
                        }
                    }
                }

                sort(degree_pool.begin(),degree_pool.end());
                if(degree_pool.size()>0)
                {
                    int index=0;
                    coreDegree0.push_back(degree_pool[index]);
                    index=0.1*degree_pool.size();
                    coreDegree10.push_back(degree_pool[index]);
                    index=0.5*degree_pool.size();
                    coreDegree50.push_back(degree_pool[index]);
                    index=0.9*degree_pool.size();
                    coreDegree90.push_back(degree_pool[index]);
                    index=degree_pool.size()-1;
                    coreDegree100.push_back(degree_pool[index]);
                }
                else
                {
                    coreDegree0.push_back(0);
                    coreDegree10.push_back(0);
                    coreDegree50.push_back(0);
                    coreDegree90.push_back(0);
                    coreDegree100.push_back(0);
                }

                removed_core_prop.push_back(rmvd_vis);
                coreSize.push_back(newCore.size());
                removed.push_back(oldCore.size()-ovlap);
                added.push_back(newCore.size()-ovlap);


                vector <double> sigma_degree_ratio_sample;
                sigma_degree_ratio_sample.clear();
                for (int k=0;k<est_no_edges.size();k++)
                {
                    double item;
                    if (est_no_edges_old.size()>0)
                    {
                        if (est_no_edges_old[k]>0) 	item=(est_no_edges[k]*1.0)/(est_no_edges_old[k]*1.0);
                        else 						item=0;
                    }
                    sigma_degree_ratio_sample.push_back(item);
                }
                est_no_edges_old.clear();
                est_no_edges_old=est_no_edges;
                sigma_degree_ratio.push_back(sigma_degree_ratio_sample);

                if(((loc[n]-1)%100000)==0)
                {

                    old_Core_coarse=new_core_coarse;
                    new_core_coarse=newCore;
                    int intersect=0;
                    if ((old_Core_coarse.size()>0)&&(new_core_coarse.size()>0))
                    {
                        int i=0,j=0;
                        while ((i<new_core_coarse.size())&&(j<old_Core_coarse.size()))
                        {
                            if (new_core_coarse[i]< old_Core_coarse[j])
                                i++;
                            else if (new_core_coarse[i]> old_Core_coarse[j])
                                j++;
                            else if (new_core_coarse[i] == old_Core_coarse[j])
                            {
                                i++;
                                j++;
                                intersect++;
                            }
                        }
                    }

                    overlap_trnd_coarse.push_back(intersect);
                    core_trnd_coarse.push_back(new_core_coarse.size());

                    printf("dumping core step %d\n",loc[n]);
                    char fname[128];
                    //int coreP=num_walker*coreScale;
                    int st=loc[n]/100000;
                    sprintf(fname,"%s/%s/coreIden[%d,%d]_step[%d00k].txt",dumpdir,nettype,num_walker,coreP,st);
                    FILE *oFile = fopen(fname, "w");
                    fprintf(oFile,"#id\tid\tdegree\t(size:\t%d)\n",newCore.size());
                    for (int i=0; i<newCore.size(); i++)
                    {
                        fprintf(oFile,"%d\t%d\t%d\n",newCore[i],visited[newCore[i]].id,visited[newCore[i]].degree);
                    }
                    fclose(oFile);
                }
            }
        }
    }

    for (int n=0; n<num_walker; n++)
    {
        fclose(fh[n]);
    }

    char fname[128];
    //int coreP=num_walker*coreScale;
    printf("dumping core sizes\n");
    sprintf(fname,"%s/%s/coreSize[%d,%d].txt",dumpdir,nettype,num_walker,coreP);
    FILE *oFile = fopen(fname, "w");
    fprintf(oFile,"#step\tcoreSz\tadded\tremoved\tdeg0\tdeg10\tde50\tdeg90\tdeg100");
    fprintf(oFile,"\n");
    for (int i=0; i<coreSize.size(); i++)
    {
        fprintf(oFile,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",i*samplingStep,coreSize[i],added[i],removed[i],coreDegree0[i],coreDegree10[i],coreDegree50[i],coreDegree90[i],coreDegree100[i]);
    }
    fclose(oFile);

    sprintf(fname,"%s/%s/coreRemoved[%d,%d].txt",dumpdir,nettype,num_walker,coreP);
    oFile = fopen(fname, "w");
    fprintf(oFile,"#step\tcoreSz\tremoved");
    for(int j=0;j<removed_core_prop[0].size();j++)
    {
        fprintf(oFile,"\trmvd_v%d",j);
    }
    for(int j=0;j<sigma_degree_ratio[0].size();j++)
    {
        fprintf(oFile,"\tsig_deg_w%d",j);
    }
    fprintf(oFile,"\n");
    for (int i=0; i<coreSize.size(); i++)
    {
        fprintf(oFile,"%d\t%d\t%d",i*samplingStep,coreSize[i],removed[i]);
        for(int j=0;j<removed_core_prop[i].size();j++)
        {
            fprintf(oFile,"\t%d",removed_core_prop[i][j]);
        }
        for(int j=0;j<sigma_degree_ratio[i].size();j++)
        {
            fprintf(oFile,"\t%f",sigma_degree_ratio[i][j]);
        }
        fprintf(oFile,"\n");
    }
    fclose(oFile);

    printf("dumping core sizes coarse\n");
    sprintf(fname,"%s/%s/coreOverlap[%d,%d].txt",dumpdir,nettype,num_walker,coreP);
    oFile = fopen(fname, "w");
    fprintf(oFile,"#step\tcoreSz\toverlap\n");
    for (int i=0; i<overlap_trnd_coarse.size(); i++)
    {
        fprintf(oFile,"%d\t%d\t%d\n",(i+1)*100000,core_trnd_coarse[i],overlap_trnd_coarse[i]);
    }
    fclose(oFile);

    return;
}

// RW discovery rate around an anchor
void randomWalkDistanceCenter(Graph graph, int num_walker, int walk_len, int seed, char* file0,int anchor) {
    int coap_anchor;
    int anchor_visits;
    set <int>target_set;
    string str;
    ifstream infile;
    infile.open(file0);
    if(!infile) {
        cout << "Cannot open infile_name: "<<file0<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    while(!infile.eof()) {
        tkns.clear();
        str.clear();
        getline(infile,str);
        Tokenize (str,tkns,"\t");

        int current=atoi(tkns[0].c_str());
        target_set.insert(current);
        // 		int deg=atoi(tkns[1].c_str());
        //
        // 		if(max_deg<deg)
        // 		{
        // 			max_deg=deg;
        // 			top[0]=current;
        // 		}
    }
    infile.close();


    target_set.erase(anchor);

    vector <int> walk_log;
    bool anchor_visited=false;

    int initial_point=0;
    vector <VisitedNode_old> visited;
    srand(seed);
    for (int walker=0;walker<num_walker;walker++) {
        anchor_visited=false;

        do {
            initial_point=rand()%graph.nodesLst.size();
        } while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next=initial_point;
        for (int n=0; n<walk_len; n++) {
            walk_log.push_back(next);

            if(next==anchor) anchor_visited=true;

            int last=visited.size();
            while(visited.size()<=next) {
                visited.push_back(VisitedNode_old(last,-1));
                last++;
            }
            if (visited[next].degree==-1) visited[next].degree=graph.nodesLst[next].neighbors->size();
            if (visited[next].last_visited_walk!=walker) {
                visited[next].visited_walks++;
                visited[next].last_visited_walk=walker;
            }
            visited[next].visited_by(-1);

            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }
        if (anchor_visited==true) {
            anchor_visits++;
            int seen=0;
            for(int i=0;i<walk_log.size();i++) {
                if(target_set.find(walk_log[i])!=target_set.end())
                    seen = 1;
            }
            if (seen>0) coap_anchor++;

        }
        walk_log.clear();
    }
    printf("# anchor:\t%d\n",anchor);
    printf("# anchor degree:\t%d\n",graph.nodesLst[anchor].neighbors->size());
    printf("# anchor visits:\t%d\n",anchor_visits);
    printf("travel from anchor and set: %d / %d = %f\n",coap_anchor,target_set.size(),(float)coap_anchor/(float)target_set.size());

}



void randomWalkSuperSpaceTransitionsSets(Graph graph, int num_walker, int walk_len, int jump_len, int seed, char* file) {
    graph.calc();
    cout<<"Graph no nodes: "<<graph.noNodes<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";

    int jumping_walkers=0;
    int set_no=0;
    vector<int> set_size;

    readCoreFiles(graph, set_no, set_size,file);

    vector<int> set_visits;
    vector<int> set_transition;
    vector<int> set_self_transition;
    vector< vector <int> > jump;


    // 	static_cast<int>(static_cast<int> pow(static_cast<double>static_cast<double>


    int jumpHistorySize=static_cast<int>(pow(static_cast<double>(set_no-1),jump_len+1));
    int jumpInitializeSize=static_cast<int>(pow(static_cast<double>(set_no-1),jump_len));
    int totalJumpHistory=0;
    vector <int> jumpHistory(jumpHistorySize,0);//=new int[jumpHistorySize];
    vector <int> jumpInitialize(jumpInitializeSize,0);//=new int[jumpInitializeSize];

    for (int i=0;i<set_no;i++)
    {
        set_visits.push_back(0);
        set_transition.push_back(0);
        set_self_transition.push_back(0);

        jump.push_back(vector <int>());
        for (int i=0;i<set_no;i++)
        {
            jump.back().push_back(0);
        }
    }

    cout<<"set no: "<<set_no<<"\n";
    cout<<"jump size: "<<jump.size()<<"\n";
    cout<<"jump[0] size: "<<jump[0].size()<<"\n";
    cout<<"set_size size: "<<set_size.size()<<"\n";
    cout<<"set_transition size: "<<set_transition.size()<<"\n";
    cout<<"set_visits size: "<<set_visits.size()<<"\n";

    int initial_point=0;
    srand(seed);
    for (int walker=0;walker<num_walker;walker++)
    {
        int step=0;
        int lastKnownStep=-1;
        int knownNodes=0;

        bool jumped=false;
        int last_stat=-1;
        int this_stat=-1;


        FixedSizeFIFOQ fQ (jump_len);


        do
        {
            initial_point=rand()%graph.nodesLst.size();
        }while ((graph.nodesLst[initial_point].neighbors->size()==0));
        step++;
        int next=initial_point;

        while (knownNodes<jump_len)
        {
            set_visits[graph.nodesLst[next].getRegion()]++;
            if(graph.nodesLst[next].getRegion()!=(set_no-1))
            {
                knownNodes++;
                fQ.push(next);
                if ((knownNodes==1)&&(step>1)) jump[set_no-1][graph.nodesLst[next].getRegion()]++;
                lastKnownStep=step;
            }
            step++;

            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }

        for (int n=step; n<walk_len+1; n++)
        {
            set_visits[graph.nodesLst[next].getRegion()]++;

            if(graph.nodesLst[next].getRegion()!=(set_no-1))
            {
                last_stat=graph.nodesLst[fQ.last()].getRegion();
                this_stat = graph.nodesLst[next].getRegion();
                if ((this_stat!=-1)&&(last_stat!=-1))
                {
                    set_transition[last_stat]++;
                    jump[last_stat][this_stat]++;
                    if(fQ.last()==next)
                    {
                        set_self_transition[last_stat]++;
                    }
                }
                else
                {
                    cout<<"Error: Node beints to Unknown but was not marked"<<endl;
                }

                int jumpIndex=0;
                for(int i=(jump_len-1);i>=0;i--)
                {
                    if(graph.nodesLst[fQ.getElement(i)].getRegion()==-1) cout<<"error region\n";
                    jumpIndex+=static_cast<int>(pow(static_cast<double>(set_no-1),(i+1)))*graph.nodesLst[fQ.getElement(i)].getRegion();
                }
                jumpIndex+=this_stat;
                if(jumpIndex>jumpHistory.size()) cout<<"error jumpHistory\n";
                if(jumpIndex/(set_no-1)>jumpInitialize.size()) cout<<"error jumpInitialize "<<jumpIndex/(set_no-1)<<" [] "<<jumpInitialize.size()<<"\n";
                jumpInitialize[jumpIndex/(set_no-1)]++;
                jumpHistory[jumpIndex]++;
                totalJumpHistory++;
                //new known node seen, so now trans mitrix should be updated.
                knownNodes++;
                fQ.push(next);
            }
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
            if (next>=graph.nodesLst.size()) cout << "Error finding next snow ball node in " <<next<<"\t"<<graph.nodesLst[next].getRegion()<<endl;

        }
        if (jumped==true) jumping_walkers++;
        if ((graph.nodesLst[next].getRegion()==(set_no-1))&&(last_stat!=-1)) jump[last_stat][set_no-1]++;
    }

    cout<<"\n# Walker Lenght: "<<walk_len;
    cout<<"\n# Transitioning Walkers: "<<jumping_walkers<<" / "<<num_walker<<"\n";

    cout<<"\n# Visits\n";
    for (int i=0;i<set_no;i++)
    {
        cout<<"#set("<<i<<"):\t"<<set_visits[i]<<"\n";
    }

    cout<<"\n# Transition\n";
    for (int i=0;i<set_no;i++)
    {
        cout<<"\t# set("<<i<<")";
    }
    cout<<"\n";
    for (int i=0;i<set_no;i++)
    {
        cout<<"#set("<<i<<")\t";
        for (int j=0;j<set_no;j++)
        {
            cout<<jump[i][j]<<"\t";
        }
        cout<<"\n";
    }

    cout<<"\n3d View Boxes\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\n";
    for (int i=0;i<(set_no-1);i++)
    {
        for (int j=0;j<(set_no-1);j++)
        {
            if (set_transition[i]>0)
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\n";
            }
            else
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<0<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<0<<"\n";
            }
        }
        cout<<endl;
        for (int j=0;j<(set_no-1);j++)
        {
            if (set_transition[i]>0)
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\n";
            }
            else
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<0<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<0<<"\n";
            }
        }
        cout<<endl;
    }

    cout<<"\nNodeXL\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\n";
    for (int i=0;i<set_no;i++)
    {
        for (int j=0;j<set_no;j++)
        {
            if (set_transition[i]>0)
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\n";
            }
            else
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<0<<"\n";
            }
        }
    }


    cout<<"\nNodeXL no self loop\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\n";
    for (int i=0;i<set_no;i++)
    {
        for (int j=0;j<set_no;j++)
        {
            if (i==j)
            {
                if (set_transition[i]>0)
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]-set_self_transition[i]<<"\t"<<((jump[i][j]-set_self_transition[i])*1.0)/((set_transition[i]-set_self_transition[i])*1.0)<<"\n";
                }
                else
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]-set_self_transition[i]<<"\t"<<0<<"\n";
                }
            }
            else
            {
                if (set_transition[i]>0)
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<(jump[i][j]*1.0)/((set_transition[i]-set_self_transition[i])*1.0)<<"\n";
                }
                else
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<0<<"\n";
                }
            }
        }
    }

    cout<<"\n# Multistep step super space transition\n";
    cout<<"# total transitions: "<<totalJumpHistory<<"\n";
    vector<int> indexArray(jump_len+1,0);//=new int[jump_len+1];
    //	for (int i=0;i<(jump_len+1);i++) indexArray[i]=0;

    // for (int i=0;i<jumpHistorySize;i++)	cout<<i<<"\t"<<jumpHistory[i]<<"\t"<<jumpHistory[i]*1.0/(totalJumpHistory*1.0)<<endl;
    cout<<"index: route\tcount\tprob\ttotalProb\n";
    for (int i=0;i<jumpHistorySize;i++)
    {
        int index=i;
        int jumpInitilializeIndex=i/(set_no-1);
        int jumpedFrom=index / (int)static_cast<int>(pow(static_cast<double>(set_no-1),jump_len));
        cout<<index<<": ";
        for (int j=jump_len;j>0;j--)
        {
            cout << index / (int)static_cast<int>(pow(static_cast<double>(set_no-1),j))<<" => ";
            index = index % (int)static_cast<int>(pow(static_cast<double>(set_no-1),j));
        }
        cout<<index<<"\t"<<jumpHistory[i]<<"\t"<<jumpHistory[i]/(set_transition[jumpedFrom]*1.0)<<"\t"<<jumpHistory[i]/(jumpInitialize[jumpInitilializeIndex]*1.0)<<endl;
    }
    exit(-1);
}

void extendRegionCore(Graph graph,int num_walker,int walk_len ,int fileType, char *inCoreFile, char *dumpdir,char *nettype,
                      bool justLCC, int betweenRegions) {
    unsigned begin_t_2=clock();

    int numChecks = num_walker;
    int numChecksHardLimit = num_walker*100;
    int minDegreeForMapping = 0;

    graph.calc();
    cout<<"# Graph Nodes: "<<graph.noNodes<<endl;
    cout<<"# Graph Edges: "<<graph.noEdges<<endl;
    char fname[512];

    int partCount=0;
    vector<int> coreSize;

    if (fileType==0) {
        readCoreFiles(graph, partCount, coreSize, inCoreFile);
    } else {
        readCoreFile(graph, partCount, coreSize, inCoreFile);
    }
    cout<<"# No Regions: "<<partCount<<endl;

    vector<int> connectedCoreSize=vector<int>(partCount,0);
    vector<int> partSize=vector<int>(partCount,0);
    vector < vector<int> > partMember = vector< vector<int> >(partCount,vector<int>());

    vector<double> coreEdgeCount=vector<double>(partCount,0);
    vector<double> connectedCoreEdgeCount=vector<double>(partCount,0);
    vector<double> connectedCoreLooseEdgeCount=vector<double>(partCount,0);

    vector<unsigned int> sizeOfPart(partCount,0);

    // Begin find the LCC
    if (justLCC) {
        vector<int> centreCandidateId(partCount,-1);
        vector<int> centreCandidateDeg(partCount,-1);
        for(int i=0;i<graph.nodesLst.size();i++) {
            int degree=graph.nodesLst[i].neighbors->size();
            int region=graph.nodesLst[i].getRegion();
            if (region>=partCount) cout<<"!! WTF!!"<<endl;
            if (degree==0) continue;
            sizeOfPart[region]++;
            //			if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
            if (centreCandidateDeg[region]<degree) {
                //				centreCandidateFound[region]==true;
                centreCandidateId[region]=i;
                centreCandidateDeg[region]=degree;
            }
        }
        for (unsigned int region=0;region<partCount;region++) {
            cout<<"R"<<region<<"\t"<<centreCandidateId[region]<<"\t"<<centreCandidateDeg[region]<<"\tsize:"<<sizeOfPart[region]<<endl;
        }

        //		for (unsigned int region=0;region<(partCount-1);region++) {
        //			int ccNo=0;
        //			int connectedNodesCntSum=0;
        //			int connectedNodesCntMax=0;
        //			double connectedNodesLooseEdgesMax=0;
        //			while (centreCandidateId[region]!=-1) {
        //				cout<<"====> CC_No: "<<ccNo<<endl;
        //				double thisCCLooseEdgeCount;
        //				RegionBFS (centreCandidateId[region],graph,false,thisCCLooseEdgeCount);
        //				centreCandidateId[region]=-1;
        //				centreCandidateDeg[region]=-1;
        //				ccNo++;
        //				int inThisCC=0;
        ////				int inThisCCSumDegree=0;
        //				for(unsigned int i=0;i<graph.nodesLst.size();i++) {
        ////					int i=partMember[region][j];
        //					if(graph.nodesLst[i].neighbors->size()==0) continue;
        //					if(graph.nodesLst[i].getRegion()==region) {
        //						if(graph.nodesLst[i].hop==-1) {
        //							if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
        //								centreCandidateId[region]=i; // remove if only one CC is needed.
        //								centreCandidateDeg[region]=graph.nodesLst[i].neighbors->size();
        //							}
        //						} else {
        //							graph.nodesLst[i].setRegion(-1);
        //							inThisCC++;
        ////							inThisCCSumDegree+=graph.nodesLst[i].neighbors->size();
        //						}
        //					}
        //				}
        //				connectedNodesCntSum += inThisCC;
        //				if (connectedNodesCntMax<inThisCC) {
        //					connectedNodesCntMax		= inThisCC;
        //					connectedNodesLooseEdgesMax = thisCCLooseEdgeCount;
        //				}
        //				if ((partSize[region]-connectedNodesCntSum)<connectedNodesCntMax) {
        //					centreCandidateId[region]=-1;
        //				}
        //				if (ccNo>1000) break;
        //			}
        //			connectedCoreSize[region]=connectedNodesCntMax;
        //			connectedCoreLooseEdgeCount[region]=connectedNodesLooseEdgesMax;
        //			cout <<"$R"<<region<<"$\t"<<coreSize[region]<<"\t"<<(double)(connectedCoreSize[region])/(double)(coreSize[region])<<"\t";
        //			cout <<"\t sumDegree: "<<coreEdgeCount[region]<<"\t-> halfEdges: "<<connectedCoreLooseEdgeCount[region]<<endl;
        //			cout.flush();
        //		}

        for (unsigned int region=0;region<(partCount-1);region++) {
            for(unsigned int i=0;i<graph.nodesLst.size();i++)
                graph.nodesLst[i].hop=-1;
            RegionBFS (centreCandidateId[region],graph,false,connectedCoreLooseEdgeCount[region]);
            for(unsigned int i=0;i<graph.nodesLst.size();i++) {
                if (graph.nodesLst[i].getRegion()==region) {
                    coreEdgeCount[region]+=graph.nodesLst[i].neighbors->size();
                    if (graph.nodesLst[i].hop==-1) {
                        graph.nodesLst[i].setRegion(partCount-1);
                    } else {
                        connectedCoreSize[region]++;
                        connectedCoreEdgeCount[region]+=graph.nodesLst[i].neighbors->size();
                    }
                }
            }
            cout <<"$R"<<region<<"$\t"<<coreSize[region]<<"\t"<<(double)(connectedCoreSize[region])/(double)(coreSize[region])<<"\t";
            cout <<"\t sumDegree: "<<coreEdgeCount[region]<<"\t-> halfEdges: "<<connectedCoreLooseEdgeCount[region]<<endl;
            cout.flush();
        }
        unsigned end_t_2=clock();
        printf("LCC measured in %f secs.\n",double(diffclock(end_t_2,begin_t_2)));
    } else {
        for (unsigned int id=0; id<graph.nodesLst.size();id++) {
            int region = graph.nodesLst[id].getRegion();
            for (unsigned int nIndex=0;nIndex<graph.nodesLst[id].neighbors->size();nIndex++) {
                int neighbor = graph.nodesLst[id].neighbors->at(nIndex);
                int targetRegion = graph.nodesLst[neighbor].getRegion();
                //				if (targetRegion == (partCount-1)) connectedCoreLooseEdgeCount[region]++;
                if (targetRegion != region) connectedCoreLooseEdgeCount[region]++;
            }
        }
    }
    // End find the LCC

    ofstream oFile;
    sprintf(fname,"%s/%s_nw_%d",dumpdir,nettype,numChecks);
    if (justLCC) strcat(fname,"_LCC");
    if (walk_len>0) strcat(fname,"_wl");
    if (betweenRegions==1) { 			/* n-closest */
        strcat(fname,"_btw");
    } else if (betweenRegions==2) { 	/* enregy saving */
        strcat(fname,"_engy");
    } else if (betweenRegions==3) { 	/* x*(1/N) */
        strcat(fname,"_times");
    } else if (betweenRegions==4) { 	/* thresh */
        strcat(fname,"_thrsh");
    } else if (betweenRegions==5) { 	/* thresh */
        strcat(fname,"_1x2");
    }
    strcat(fname,"_parts.txt");
    std::string fileName=std::string(fname);

    cout<<fileName<<endl;

    oFile.open(fileName.c_str(),ios::out);
    oFile<<"#node_id\t#deg\t#CLS\t#conf\t:";
    for (int k=0;k<partCount-1;k++) {
        oFile<<"\t"<<k;
    }
    oFile<<endl;

    for  (int id=0; id<graph.nodesLst.size();id++) {
        if (graph.nodesLst[id].neighbors->size()==0)
            continue;

        oFile<<id<<"\t"<<graph.nodesLst[id].neighbors->size()<<"\t";
        oFile.flush();
        if (graph.nodesLst[id].neighbors->size()<=minDegreeForMapping) {
            oFile<<-1;
            oFile<<"\t"<<-1<<"\t:";
            oFile<<"\t("<<0<<")\t:";
            for (int k=0;k<partCount-1;k++) {
                oFile<<"\t0(-1)[0]";
            }
            oFile<<endl;
            continue;
        } else if (graph.nodesLst[id].getRegion()!=(partCount-1)) { /* Node in one core */
            oFile<<graph.nodesLst[id].getRegion();
            oFile<<"\t"<<1<<"\t:";
            oFile<<"\t("<<0<<")\t:";
            for (int k=0;k<partCount-1;k++) {
                if (k==graph.nodesLst[id].getRegion()) oFile<<"\t1(0)";
                else oFile<<"\t0(-1)[0]";
            }
            oFile<<endl;
            partSize[graph.nodesLst[id].getRegion()]++;
            partMember[graph.nodesLst[id].getRegion()].push_back(id);
            continue;
        }

        vector <double> avgDistance=vector<double>(partCount-1,0);
        vector <double> hitCoreCount=vector<double>(partCount-1,0);
        vector <double> hitCoreNorm=vector<double>(partCount-1,0);
        vector <int> hitCoreCountAux=vector<int>(partCount-1,0);
        int totExp=0;
        if (walk_len>0) {
            totExp=0;
            for (int i=0;(i<numChecks)&&(totExp<numChecksHardLimit);i++) {
                totExp ++;
                int len=0;
                int next=id;
                while ((graph.nodesLst[next].getRegion()==(partCount-1)) and (len<walk_len)) {
                    next=nextNodeIdSnowBall(graph.nodesLst[next]);
                    len++;
                }
                if (graph.nodesLst[next].getRegion()!=(partCount-1)) {
                    /* drop value of each step */
                    hitCoreCountAux[graph.nodesLst[next].getRegion()]++;
                    hitCoreCount[graph.nodesLst[next].getRegion()]++; /* fixed maximum length */
                    // beints[graph.nodesLst[next].getRegion()]+=(double(walk_len-1)*len)/double(walk_len); /* drop point at each step */
                    hitCoreNorm[graph.nodesLst[next].getRegion()]+=(1.0/graph.nodesLst[next].neighbors->size()); /* normalize by degree */
                    avgDistance[graph.nodesLst[next].getRegion()]+=len;
                } else i--;
            }
        } else {
            for (int i=0;i<numChecks;i++) {
                int next=id;
                int len=0;
                while (graph.nodesLst[next].getRegion()==(partCount-1)) {
                    next=nextNodeIdSnowBall(graph.nodesLst[next]);
                    len++;
                }
                hitCoreCountAux[graph.nodesLst[next].getRegion()]++;
                hitCoreCount[graph.nodesLst[next].getRegion()]++; /* fixed maximum length */
                // beints[graph.nodesLst[next].getRegion()]+=(double(walk_len-1)*len)/double(walk_len); /* drop point at each step */
                hitCoreNorm[graph.nodesLst[next].getRegion()]+=(1.0/graph.nodesLst[next].neighbors->size()); /* normalize by degree */
                avgDistance[graph.nodesLst[next].getRegion()]+=len;
            }
        }
        double sumBeint=0;
        for (int i=0;i<hitCoreCount.size();i++) {
            if (avgDistance[i]>0) avgDistance[i]/=hitCoreCount[i];
            else avgDistance[i]=-1;

            /* How to normalize */
            //			hitCoreCount[i]=hitCoreCount[i]/(connectedCoreLooseEdgeCount[i]);   /* normalize by total number of stick out edges */
            //			hitCoreCount[i]=hitCoreNorm[i]/coreSize[i];                         /* normalize by the degree of core's node */
            /* End How to normalize */

            sumBeint+=hitCoreCount[i];
        }

        if (sumBeint>0) {
            for (int i=0;i<hitCoreCount.size();i++) {
                hitCoreCount[i]/=sumBeint;
            }
            /* */

            vector<double> beInSetConfidence(partCount-1,-1);
            vector<int> beInSetIndex(partCount-1,-1);
            beInSetConfidence[0]=hitCoreCount[0];
            beInSetIndex[0]=0;
            for (int i=1;i<hitCoreCount.size();i++) {
                int index=i;
                for(int j=0;j<i;j++) {
                    if(hitCoreCount[i]>beInSetConfidence[j]) {
                        index=j;
                        break;
                    }
                }
                for(int j=i;j>index;j--) {
                    beInSetConfidence[j]=beInSetConfidence[j-1];
                    beInSetIndex[j]=beInSetIndex[j-1];
                }
                beInSetConfidence[index]=hitCoreCount[i];
                beInSetIndex[index]=i;
            }
            double ownerDistance=avgDistance[beInSetIndex[0]];

            /* if no between is set then pick the max */
            vector<int> owners;
            owners.push_back(beInSetIndex[0]);
            if (betweenRegions==1) {
                for (int i=1;i<hitCoreCount.size();i++) {
                    if ((beInSetConfidence[i]*2)>=beInSetConfidence[0])
                        owners.push_back(beInSetIndex[i]);
                }
                sort(owners.begin(),owners.end());
            } else if (betweenRegions==2) {
                sumBeint=beInSetConfidence[0];
                for (int i=1;i<hitCoreCount.size();i++) {
                    if (sumBeint<=0.8) {
                        owners.push_back(beInSetIndex[i]);
                        sumBeint+=beInSetConfidence[i];
                    }
                    else break;
                }
            } else if (betweenRegions==3) {
                double thresh=2.0;
                if (beInSetConfidence[0] < (thresh/hitCoreCount.size())) owners[0]=-1;
            } else if (betweenRegions==4) {
                double thresh=0.5;
                if (beInSetConfidence[0] < thresh) owners[0]=-1;
            } else if (betweenRegions==5) {
                double times=2;
                if (beInSetConfidence[0] < (beInSetConfidence[1]*times)) owners[0]=-1;
            }

            /* To file */
            for (int i=0;i<owners.size()-1;i++) {
                oFile<<owners[i]<<"-";
            }
            oFile<<owners.back();
            oFile<<"\t"<<beInSetConfidence[0]<<"\t:";
            oFile<<"\t("<<ownerDistance<<")\t:";
            for (int i=0;i<hitCoreCount.size();i++) {
                oFile<<"\t"<<hitCoreCount[i]<<"("<<avgDistance[i]<<")["<<hitCoreCountAux[i]<<"]";
            }
            oFile<<endl;
            oFile.flush();
            graph.nodesLst[id].setRegion(owners.back());
            partSize[graph.nodesLst[id].getRegion()]++;
            partMember[graph.nodesLst[id].getRegion()].push_back(id);
        } else {
            /* To file */
            oFile<<-1;
            oFile<<"\t"<<-1<<"\t:";
            oFile<<"\t("<<-1<<")\t:";
            for (int i=0;i<hitCoreCount.size();i++) {
                oFile<<"\t"<<hitCoreCount[i]<<"("<<avgDistance[i]<<")["<<hitCoreCountAux[i]<<"]";
            }
            oFile<<endl;
            oFile.flush();
        }
    }
    oFile.close();

    unsigned end_t_2=clock();
    printf("partitioned in %f secs.\n",double(diffclock(end_t_2,begin_t_2)));


    vector<double> partDeg(partCount,0);
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
            cout<<"Shit... integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcRegion=graph.nodesLst[i].getRegion();

        partDeg[srcRegion]+=graph.nodesLst[i].neighbors->size();
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
    for (int region=0;region<partCount;region++) {
        for(unsigned int i=0;i<graph.nodesLst.size();i++)
            graph.nodesLst[i].hop=-1;
        int ccNo=0;
        //			int disconnectedNodesCnt=0;
        //			int disconnectedNodesDegrees=0;
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
            cout<<"# Number of Counted CC's: "<<ccNo<<endl;
            cout<<"CC's average degree: "<<connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0)<<" - separate nodes average degree: "
               <<(partDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0)<<endl;
        }

        LLC_Percentage[region]=(connectedNodesCntMax*1.0)/(partSize[region]);
        inLCC_avgDeg[region]=connectedNodesSumDegreesMax/(connectedNodesCntMax*1.0);
        outLCC_avgDeg[region]=(partDeg[region]-connectedNodesSumDegreesMax)/((partSize[region]-connectedNodesCntMax)*1.0);
    }

    //	for (int region=0;region<partCount;region++) {
    //		for(unsigned int i=0;i<graph.nodesLst.size();i++)
    //			graph.nodesLst[i].hop=-1;
    //		int ccNo=0;
    //		int disconnectedNodesCnt=0;
    //		int disconnectedNodesDegrees=0;
    //		int connectedNodesCnt=0;
    //		int connectedNodesDegrees=0;
    //		while (centreCandidateId[region]!=-1) {
    //			double temp;
    //			RegionBFS (centreCandidateId[region],graph,false,temp);
    //			centreCandidateId[region]=-1;
    //			centreCandidateDeg[region]=-1;
    //			ccNo++;
    //			int inThisCC=0;
    //			for(unsigned int j=0;j<partMember[region].size();j++) {
    //				int i=partMember[region][j];
    //				if(graph.nodesLst[i].neighbors->size()==0) continue;
    //				if(graph.nodesLst[i].getRegion()==region) {
    //					if(graph.nodesLst[i].hop==-1) {
    //						if (ccNo==1) {
    //							disconnectedNodesCnt++;
    //							disconnectedNodesDegrees+=graph.nodesLst[i].neighbors->size();
    //						}
    //						if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
    //							//	centreCandidateId[region]=i; // remove if only the LCC is needed.
    //							centreCandidateDeg[region]=graph.nodesLst[i].neighbors->size();
    //						}
    //					} else {
    //						graph.nodesLst[i].setRegion(-1);
    //						inThisCC++;
    //						if (ccNo==1) {
    //							connectedNodesCnt++;
    //							connectedNodesDegrees+=graph.nodesLst[i].neighbors->size();
    //						}
    //					}
    //				}
    //			}
    //		}
    //		LLC_Percentage[region]=(connectedNodesCnt*1.0)/(connectedNodesCnt+disconnectedNodesCnt);
    //		inLCC_avgDeg[region]=connectedNodesDegrees/(connectedNodesCnt*1.0);
    //		outLCC_avgDeg[region]=disconnectedNodesDegrees/(disconnectedNodesCnt*1.0);
    //	}

    //	ofstream oFile;
    //	char fname[512];
    sprintf(fname,"%s/%s_nw_%d",dumpdir,nettype,numChecks);
    if (justLCC) strcat(fname,"_LCC");
    if (walk_len>0) strcat(fname,"_wl");
    strcat(fname,"_regionProp.txt");

    oFile.open((const char*)fname,ios::out);

    oFile<<"#Region"<<"\t&";
    oFile<<"\t"<<"Core Size"<<"\t&";
    oFile<<"\t"<<"Core Avg.Deg"<<"\t&";
    oFile<<"\t"<<"CoreCC Size"<<"\t&";
    oFile<<"\t"<<"CoreCC Avg.Deg"<<"\t&";
    oFile<<"\t"<<"CoreCC OutDeg"<<"\t&";
    oFile<<"\t"<<"CoreCC\%"<<"\t&";

    oFile<<"\t"<<"\%Nodes"<<"\t&";
    oFile<<"\t"<<"\%Edges"<<"\t&";
    oFile<<"\t"<<"\%Int Links"<<"\t&";
    oFile<<"\t"<<"\%Ext Links"<<"\t&";
    oFile<<"\t"<<"Int. Bias"<<"\t&";
    oFile<<"\t"<<"Ext. Bias"<<"\t&";

    oFile<<"\t"<<"AvgDeg."<<"\t&";
    oFile<<"\t"<<"Modularity"<<"\t&";

    oFile<<"\t"<<"RegionCC\%"<<"\t\\\\";
    oFile<<endl;
    for (int dst=0;dst<(partCount-1);dst++) {
        int dstRegion=dst;

        oFile<<"$R"<<dstRegion<<"$\t&";

        oFile<<"\t"<<coreSize[dstRegion]<<"\t&"; 									//core size
        oFile<<"\t"<<coreEdgeCount[dstRegion]/coreSize[dstRegion]<<"\t&";           //core avg. degree

        oFile<<"\t"<<connectedCoreSize[dstRegion]<<"\t&"; 							//connected core size
        oFile<<"\t"<<connectedCoreEdgeCount[dstRegion]/connectedCoreSize[dstRegion]<<"\t&"; 				//connected core avg. degree
        oFile<<"\t"<<connectedCoreLooseEdgeCount[dstRegion]<<"\t&";                 //connected core loose edges
        oFile<<"\t"<<(connectedCoreSize[dstRegion]*100.0)/coreSize[dstRegion]<<"\t&";                       //core LCC


        oFile<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&"; 				//region size
        double regionEdgePerc=(partDeg[dstRegion]*100.0)/(graphDeg*1.0);
        oFile<<"\t"<<regionEdgePerc<<"\t&"; 										//region half edges

        double internal=regionDegIntra[dstRegion]*100.0/partDeg[dstRegion];
        oFile<<"\t"<<internal<<"\t&";												// region internal connectivity
        double external=((partDeg[dstRegion]-regionDegIntra[dstRegion])*100.0)/(partDeg[dstRegion]);
        oFile<<"\t"<<external<<"\t&";												// region external connectivity

        oFile<<"\t"<<(internal-regionEdgePerc)/regionEdgePerc<<"\t&"; 				// region internal connectivity bias
        oFile<<"\t"<<(external-(100-regionEdgePerc))/(100-regionEdgePerc)<<"\t&"; 	// region external connectivity bias


        oFile<<"\t"<< partDeg[dstRegion]/partSize[dstRegion] <<"\t&";
        double module= (regionDegIntra[dstRegion]/partDeg[dstRegion]) - (partDeg[dstRegion]/graphDeg);
        oFile<<"\t"<<module<<"\t&";

        oFile<<"\t"<<LLC_Percentage[dstRegion] <<"\t\\\\";
        oFile<<endl;
    }
    oFile<<"#End Stats"<<endl;
    oFile.close();
    return;
}


void extendRegionCoreInRounds(Graph graph,int num_walker,int walk_len ,int fileType, char *inCoreFile, char *dumpdir,char *nettype,
                              bool justLCC, int betweenRegions) {
    int numChecks = num_walker;
    int numChecksHardLimit = num_walker*100;
    bool inRounds=false;
    int minDegreeForMapping = 0;

    graph.calc();
    cout<<"# Graph Nodes: "<<graph.noNodes<<"\n";
    cout<<"# Graph Edges: "<<graph.noEdges<<"\n";
    char fname[1024];

    int partCount=0;
    vector<int> coreSize;

    if (fileType==0) {
        readCoreFiles(graph, partCount, coreSize, inCoreFile);
    } else {
        readCoreFile(graph, partCount, coreSize, inCoreFile);
    }
    cout<<"# No Regions: "<<partCount<<endl;

    vector<int> connectedCoreSize=vector<int>(partCount,0);
    vector<int> partSize=vector<int>(partCount,0);
    vector < vector<int> > partMember = vector< vector<int> >(partCount,vector<int>());

    vector<double> coreEdgeCount=vector<double>(partCount,0);
    vector<double> connectedCoreEdgeCount=vector<double>(partCount,0);
    vector<double> connectedCoreLooseEdgeCount=vector<double>(partCount,0);

    // Begin find the LCC
    if (justLCC) {
        vector<int> centreCandidateId(partCount,-1);
        vector<int> centreCandidateDeg(partCount,-1);
        for(int i=0;i<graph.nodesLst.size();i++) {
            if (graph.nodesLst[i].neighbors->size()==0) continue;
            int srcRegion=graph.nodesLst[i].getRegion();
            if ((centreCandidateId[srcRegion]==-1)||(centreCandidateDeg[srcRegion]<graph.nodesLst[i].neighbors->size())) {
                centreCandidateId[srcRegion]=i;
                centreCandidateDeg[srcRegion]=graph.nodesLst[i].neighbors->size();
            }
        }
        for (int region=0;region<(partCount-1);region++) {
            for(int i=0;i<graph.nodesLst.size();i++)	graph.nodesLst[i].hop=-1;

            RegionBFS (centreCandidateId[region],graph,false,connectedCoreLooseEdgeCount[region]);
            for(int i=0;i<graph.nodesLst.size();i++) {
                if (graph.nodesLst[i].getRegion()==region) {
                    coreEdgeCount[region]+=graph.nodesLst[i].neighbors->size();
                    if (graph.nodesLst[i].hop==-1) {
                        graph.nodesLst[i].setRegion(partCount-1);
                    } else {
                        connectedCoreSize[region]++;
                        connectedCoreEdgeCount[region]+=graph.nodesLst[i].neighbors->size();
                    }
                }
            }
            cout <<"$R"<<region<<"$\t"<<coreSize[region]<<"\t"<<(double)(connectedCoreSize[region])/(double)(coreSize[region])<<"\t";
            cout <<"\t sumDegree: "<<coreEdgeCount[region]<<"\t-> halfEdges: "<<connectedCoreLooseEdgeCount[region]<<endl;
        }
    } else {
        for (int region=0;region<(partCount-1);region++) {
            for (unsigned int id=0; id<graph.nodesLst.size();id++) {
                for (unsigned int nIndex=0;nIndex<graph.nodesLst[id].neighbors->size();nIndex++) {
                    int neighbor = graph.nodesLst[id].neighbors->at(nIndex);
                    int targetRegion = graph.nodesLst[neighbor].getRegion();
                    if (targetRegion!=region) connectedCoreLooseEdgeCount[region]++;
                }
            }
        }
    }
    // End find the LCC

    if (!inRounds) {
        ofstream ofile;
        sprintf(fname,"%s/%s_nw_%d",dumpdir,nettype,numChecks);
        if (justLCC) strcat(fname,"_LCC");
        if (walk_len>0) strcat(fname,"_wl");
        if (betweenRegions==1) { 			/* n-closest */
            strcat(fname,"_btw");
        } else if (betweenRegions==2) { 	/* enregy saving */
            strcat(fname,"_engy");
        } else if (betweenRegions==3) { 	/* x*(1/N) */
            strcat(fname,"_times");
        } else if (betweenRegions==4) { 	/* thresh */
            strcat(fname,"_thrsh");
        } else if (betweenRegions==5) { 	/* thresh */
            strcat(fname,"_1x2");
        }
        strcat(fname,"_parts.txt");

        ofile.open((const char*)fname,ios::out);
        ofile<<"#node_id\t#deg\t#CLS\t#conf\t:";
        for (int k=0;k<partCount-1;k++) {
            ofile<<"\t"<<k;
        }
        ofile<<endl;

        for  (int id=0; id<graph.nodesLst.size();id++) {
            ofile<<id<<"\t"<<graph.nodesLst[id].neighbors->size()<<"\t";
            ofile.flush();
            if (graph.nodesLst[id].neighbors->size()<=minDegreeForMapping) {
                ofile<<-1;
                ofile<<"\t"<<-1<<"\t:";
                ofile<<"\t("<<0<<")\t:";
                for (int k=0;k<partCount-1;k++) {
                    ofile<<"\t0(-1)";
                }
                ofile<<endl;
                continue;
            } else if (graph.nodesLst[id].getRegion()!=(partCount-1)) { /* Node in one core */
                ofile<<graph.nodesLst[id].getRegion();
                ofile<<"\t"<<1<<"\t:";
                ofile<<"\t("<<0<<")\t:";
                for (int k=0;k<partCount-1;k++) {
                    if (k==graph.nodesLst[id].getRegion()) ofile<<"\t1(0)";
                    else ofile<<"\t0(-1)";
                }
                ofile<<endl;
                partSize[graph.nodesLst[id].getRegion()]++;
                partMember[graph.nodesLst[id].getRegion()].push_back(id);
                continue;
            }

            vector <double> avgDistance=vector<double>(partCount-1,0);
            vector <double> beints=vector<double>(partCount-1,0);
            int totExp=0;
            if (walk_len>0) {
                totExp=0;
                for (int i=0;(i<numChecks)&&(totExp<numChecksHardLimit);i++) {
                    totExp ++;
                    int len=0;
                    int next=id;
                    while ((graph.nodesLst[next].getRegion()==(partCount-1)) and (len<walk_len)) {
                        next=nextNodeIdSnowBall(graph.nodesLst[next]);
                        len++;
                    }
                    if (graph.nodesLst[next].getRegion()!=(partCount-1)) {
                        beints[graph.nodesLst[next].getRegion()]++; /* fixed maximum length */
                        // beints[graph.nodesLst[next].getRegion()]+=(double(walk_len-1)*len)/double(walk_len); /* drop point at each step */
                        avgDistance[graph.nodesLst[next].getRegion()]+=len;
                    } else i--;
                }
            } else {
                for (int i=0;i<numChecks;i++) {
                    int next=id;
                    int len=0;
                    while (graph.nodesLst[next].getRegion()==(partCount-1)) {
                        next=nextNodeIdSnowBall(graph.nodesLst[next]);
                        len++;
                    }
                    beints[graph.nodesLst[next].getRegion()]++;
                    avgDistance[graph.nodesLst[next].getRegion()]+=len;
                }
            }
            double sumBeint=0;
            for (int i=0;i<beints.size();i++) {
                if (avgDistance[i]>0) avgDistance[i]/=beints[i];
                else avgDistance[i]=-1;
                beints[i]=beints[i]/(numChecks*connectedCoreLooseEdgeCount[i]);
                sumBeint+=beints[i];
            }

            if (sumBeint>0) {
                for (int i=0;i<beints.size();i++) {
                    beints[i]/=sumBeint;
                }
                //////////////////////////////// /* */

                vector<double> beInSetConfidence(partCount-1,-1);
                vector<int> beInSetIndex(partCount-1,-1);
                beInSetConfidence[0]=beints[0];
                beInSetIndex[0]=0;
                for (int i=1;i<beints.size();i++) {
                    int index=i;
                    for(int j=0;j<i;j++) {
                        if(beints[i]>beInSetConfidence[j]) {
                            index=j;
                            break;
                        }
                    }
                    for(int j=i;j>index;j--) {
                        beInSetConfidence[j]=beInSetConfidence[j-1];
                        beInSetIndex[j]=beInSetIndex[j-1];
                    }
                    beInSetConfidence[index]=beints[i];
                    beInSetIndex[index]=i;
                }
                double ownerDistance=avgDistance[beInSetIndex[0]];


                /* if no between is set then pick the max */
                vector<int> owners;
                owners.push_back(beInSetIndex[0]);
                if (betweenRegions==1) {
                    for (int i=1;i<beints.size();i++) {
                        if ((beInSetConfidence[i]*2)>=beInSetConfidence[0])
                            owners.push_back(beInSetIndex[i]);
                    }
                    sort(owners.begin(),owners.end());
                } else if (betweenRegions==2) {
                    sumBeint=beInSetConfidence[0];
                    for (int i=1;i<beints.size();i++) {
                        if (sumBeint<=0.8) {
                            owners.push_back(beInSetIndex[i]);
                            sumBeint+=beInSetConfidence[i];
                        }
                        else break;
                    }
                } else if (betweenRegions==3) {
                    double thresh=2.0;
                    if (beInSetConfidence[0] < (thresh/beints.size())) owners[0]=-1;
                } else if (betweenRegions==4) {
                    double thresh=0.5;
                    if (beInSetConfidence[0] < thresh) owners[0]=-1;
                } else if (betweenRegions==5) {
                    double times=2;
                    if (beInSetConfidence[0] < (beInSetConfidence[1]*times)) owners[0]=-1;
                }

                /* To file */
                for (int i=0;i<owners.size()-1;i++) {
                    ofile<<owners[i]<<"-";
                }
                ofile<<owners.back();
                ofile<<"\t"<<beInSetConfidence[0]<<"\t:";
                ofile<<"\t("<<ownerDistance<<")\t:";
                for (int i=0;i<beints.size();i++) {
                    ofile<<"\t"<<beints[i]<<"("<<avgDistance[i]<<")";
                }
                ofile<<endl;
                ofile.flush();
                graph.nodesLst[id].setRegion(owners.back());
                partSize[graph.nodesLst[id].getRegion()]++;
                partMember[graph.nodesLst[id].getRegion()].push_back(id);
            } else {
                /* To file */
                ofile<<-1;
                ofile<<"\t"<<-1<<"\t:";
                ofile<<"\t("<<-1<<")\t:";
                for (int i=0;i<beints.size();i++) {
                    ofile<<"\t"<<beints[i]<<"("<<avgDistance[i]<<")";
                }
                ofile<<endl;
                ofile.flush();
            }
        }
        ofile.close();
    } else {
        ofstream ofile;
        if (walk_len>0) {
            if (justLCC) sprintf(fname,"%s/%s_LCC_wl_grad_parts.txt",dumpdir,nettype);
            else sprintf(fname,"%s/%s_wl_grad_parts.txt",dumpdir,nettype);
        }
        else {
            if (justLCC) sprintf(fname,"%s/%s_LCC_grad_parts.txt",dumpdir,nettype);
            else sprintf(fname,"%s/%s_grad_parts.txt",dumpdir,nettype);
        }
        ofile.open((const char*)fname,ios::out);
        ofile<<"#node_id\t#deg\t#CLS\t#conf\t#thresh\t#round\n";

        for  (int id=0; id<graph.nodesLst.size();id++) {
            if (graph.nodesLst[id].neighbors->size()==0) continue;
            if (graph.nodesLst[id].getRegion()!=(partCount-1)) {
                ofile<<id<<"\t"<<graph.nodesLst[id].neighbors->size()<<"\t"<<graph.nodesLst[id].getRegion()<<"\t100"<<"\t100\t0"<<endl;
                continue;
            }
        }

        bool remaining=false;
        bool addedThisRound=false;
        int round=0;
        for (double threshold=0.95;threshold>0.7;threshold=threshold-0.05) {
            do {
                remaining=false;
                addedThisRound=false;
                round++;
                map <int, int> mappingConf;
                mappingConf.clear();
                for (int id=0; id<graph.nodesLst.size();id++) {
                    if (graph.nodesLst[id].neighbors->size()<=minDegreeForMapping) continue;
                    if (graph.nodesLst[id].getRegion()!=(partCount-1)) continue;
                    vector <double> beints=vector<double>(partCount-1,0);
                    if (walk_len>0) {
                        for (int i=0;i<numChecks;i++) {
                            int len=0;
                            int next=id;
                            while ((graph.nodesLst[next].getRegion()==(partCount-1)) and (len<walk_len)) {
                                next=nextNodeIdSnowBall(graph.nodesLst[next]);
                                len++;
                            }
                            if (graph.nodesLst[next].getRegion()!=(partCount-1))
                                beints[graph.nodesLst[next].getRegion()]++;
                            else
                                i--;
                        }
                    }
                    else {
                        for (int i=0;i<numChecks;i++) {

                            int next=id;
                            while (graph.nodesLst[next].getRegion()==(partCount-1)) {
                                next=nextNodeIdSnowBall(graph.nodesLst[next]);
                            }
                            beints[graph.nodesLst[next].getRegion()]++;
                        }
                    }

                    int thisNodeSet=-1;
                    double maxBeint=-1;
                    for (int i=0;i<beints.size();i++) {
                        beints[i]/=double(numChecks);
                        if (beints[i]>maxBeint) {
                            maxBeint=beints[i];
                            thisNodeSet=i;
                        }
                    }
                    if ( maxBeint> threshold ) {
                        ofile<<id<<"\t"<<graph.nodesLst[id].neighbors->size()<<"\t"<<thisNodeSet<<"\t"<<maxBeint*100<<"\t"<<threshold<<"\t"<<round<<"\t:";
                        for (int i=0;i<beints.size();i++) {
                            ofile<<"\t"<<beints[i];
                        }
                        ofile<<endl;

                        mappingConf[id]=thisNodeSet;
                        addedThisRound=true;
                    }
                    else {
                        remaining=true;
                    }
                }
                for( map<int,int>::iterator ii=mappingConf.begin(); ii!=mappingConf.end(); ++ii) {
                    graph.nodesLst[ii->first].setRegion(ii->second);
                }
            } while (remaining&&addedThisRound);
        }
        for (int id=0; id<graph.nodesLst.size();id++) {
            if (graph.nodesLst[id].neighbors->size()==0) continue;
            if (graph.nodesLst[id].getRegion()!=(partCount-1)) continue;

            vector <double> beints=vector<double>(partCount-1,0);
            for (int i=0;i<numChecks;i++) {
                int next=id;
                while (graph.nodesLst[next].getRegion()==(partCount-1)) {
                    next=nextNodeIdSnowBall(graph.nodesLst[next]);
                }
                beints[graph.nodesLst[next].getRegion()]++;
            }
            int thisNodeSet=-1;
            double maxBeint=-1;
            for (int i=0;i<beints.size();i++) {
                beints[i]/=double(numChecks);
                if (beints[i]>maxBeint) {
                    maxBeint=beints[i];
                    thisNodeSet=i;
                }
            }
            ofile<<id<<"\t"<<graph.nodesLst[id].neighbors->size()<<"\t"<<"*"<<thisNodeSet<<"*"<<"\t"<<maxBeint*100<<"\t"<<"middle"<<"\t"<<round<<"\t:";
            for (int i=0;i<beints.size();i++) {
                ofile<<"\t"<<beints[i];
            }
            ofile<<endl;
            ofile.flush();
        }
        ofile.close();
    }



    vector<double> partDeg(partCount,0);
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
            cout<<"Shit... integer is not enough"<<endl;
            cout<<i<<" after "<<i-1<<endl;
            exit(-1);
        }
        if (graph.nodesLst[i].neighbors->size()==0) continue;
        int srcRegion=graph.nodesLst[i].getRegion();

        partDeg[srcRegion]+=graph.nodesLst[i].neighbors->size();
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
    for (int region=0;region<partCount;region++) {
        for(unsigned int i=0;i<graph.nodesLst.size();i++)
            graph.nodesLst[i].hop=-1;
        int ccNo=0;
        int disconnectedNodesCnt=0;
        int disconnectedNodesDegrees=0;
        int connectedNodesCnt=0;
        int connectedNodesDegrees=0;
        while (centreCandidateId[region]!=-1) {
            double temp;
            RegionBFS (centreCandidateId[region],graph,false,temp);
            centreCandidateId[region]=-1;
            centreCandidateDeg[region]=-1;
            ccNo++;
            int inThisCC=0;
            for(unsigned int j=0;j<partMember[region].size();j++) {
                int i=partMember[region][j];
                if(graph.nodesLst[i].neighbors->size()==0) continue;
                if(graph.nodesLst[i].getRegion()==region) {
                    if(graph.nodesLst[i].hop==-1) {
                        if (ccNo==1) {
                            disconnectedNodesCnt++;
                            disconnectedNodesDegrees+=graph.nodesLst[i].neighbors->size();
                        }
                        if ((centreCandidateId[region]==-1)||(centreCandidateDeg[region]<graph.nodesLst[i].neighbors->size())) {
                            //	centreCandidateId[region]=i; // remove if only the LCC is needed.
                            centreCandidateDeg[region]=graph.nodesLst[i].neighbors->size();
                        }
                    } else {
                        graph.nodesLst[i].setRegion(-1);
                        inThisCC++;
                        if (ccNo==1) {
                            connectedNodesCnt++;
                            connectedNodesDegrees+=graph.nodesLst[i].neighbors->size();
                        }
                    }
                }
            }
        }
        LLC_Percentage[region]=(connectedNodesCnt*1.0)/(connectedNodesCnt+disconnectedNodesCnt);
        inLCC_avgDeg[region]=connectedNodesDegrees/(connectedNodesCnt*1.0);
        outLCC_avgDeg[region]=disconnectedNodesDegrees/(disconnectedNodesCnt*1.0);
    }




    ofstream ofile;
    sprintf(fname,"%s/%s_nw_%d_regionProp.txt",dumpdir,nettype,numChecks);
    ofile.open((const char*)fname,ios::out);

    ofile<<"#Region"<<"\t&";
    ofile<<"\t"<<"Core Size"<<"\t&";
    ofile<<"\t"<<"Core Avg.Deg"<<"\t&";
    ofile<<"\t"<<"CoreCC Size"<<"\t&";
    ofile<<"\t"<<"CoreCC Avg.Deg"<<"\t&";
    ofile<<"\t"<<"CoreCC OutDeg"<<"\t&";
    ofile<<"\t"<<"CoreCC\%"<<"\t&";

    ofile<<"\t"<<"\%Nodes"<<"\t&";
    ofile<<"\t"<<"\%Edges"<<"\t&";
    ofile<<"\t"<<"\%Int Links"<<"\t&";
    ofile<<"\t"<<"\%Ext Links"<<"\t&";
    ofile<<"\t"<<"Int. Bias"<<"\t&";
    ofile<<"\t"<<"Ext. Bias"<<"\t&";

    ofile<<"\t"<<"AvgDeg."<<"\t&";
    ofile<<"\t"<<"Modularity"<<"\t&";

    ofile<<"\t"<<"RegionCC\%"<<"\t\\\\";
    ofile<<endl;
    for (int dst=0;dst<(partCount-1);dst++) {
        int dstRegion=dst;

        ofile<<"$R"<<dstRegion<<"$\t&";

        ofile<<"\t"<<coreSize[dstRegion]<<"\t&"; 									//core size
        ofile<<"\t"<<coreEdgeCount[dstRegion]/coreSize[dstRegion]<<"\t&";           //core avg. degree

        ofile<<"\t"<<connectedCoreSize[dstRegion]<<"\t&"; 							//connected core size
        ofile<<"\t"<<connectedCoreEdgeCount[dstRegion]/connectedCoreSize[dstRegion]<<"\t&"; 				//connected core avg. degree
        ofile<<"\t"<<connectedCoreLooseEdgeCount[dstRegion]<<"\t&";                 //connected core loose edges
        ofile<<"\t"<<(connectedCoreSize[dstRegion]*100.0)/coreSize[dstRegion]<<"\t&";                       //core LCC


        ofile<<"\t"<<partSize[dstRegion]*100.0/graph.noNodes<<"\t&"; 				//region size
        double regionEdgePerc=(partDeg[dstRegion]*100.0)/(graphDeg*1.0);
        ofile<<"\t"<<regionEdgePerc<<"\t&"; 										//region half edges

        double internal=regionDegIntra[dstRegion]*100.0/partDeg[dstRegion];
        ofile<<"\t"<<internal<<"\t&";												// region internal connectivity
        double external=((partDeg[dstRegion]-regionDegIntra[dstRegion])*100.0)/(partDeg[dstRegion]);
        ofile<<"\t"<<external<<"\t&";												// region external connectivity

        ofile<<"\t"<<(internal-regionEdgePerc)/regionEdgePerc<<"\t&"; 				// region internal connectivity bias
        ofile<<"\t"<<(external-(100-regionEdgePerc))/(100-regionEdgePerc)<<"\t&"; 	// region external connectivity bias


        ofile<<"\t"<< partDeg[dstRegion]/partSize[dstRegion] <<"\t&";
        double module= (regionDegIntra[dstRegion]/partDeg[dstRegion]) - (partDeg[dstRegion]/graphDeg);
        ofile<<"\t"<<module<<"\t&";

        ofile<<"\t"<<LLC_Percentage[dstRegion] <<"\t\\\\";
        ofile<<endl;
    }
    ofile<<"#End Stats"<<endl;
    ofile.close();
    return;
}

void partRWTransitionCounter(Graph graph, char* dumpDir, char* nettype, int num_walker, int walkLen, int seed, char* regionFile, int regionFileType, char* outFile) {
    graph.calc();
    cout<<"# Graph Nodes: "<<graph.noNodes<<"\n";
    cout<<"# Graph Edges: "<<graph.noEdges<<"\n";

    walkLen = graph.noNodes;

    int allKnownNodes=0;
    int partCount=0;
    vector<int> partSize;

    if (regionFileType==0) { /* Core file */
        cout<<"# Reading info file"<<endl;
        readCoreFiles(graph, partCount, partSize, regionFile);
        partCount--;
        for (int i=0;i<partCount;i++) {
            allKnownNodes+=partSize[i];
        }
    } else if (regionFileType==1) { /* Part file */
        cout<<"# Reading part file"<<endl;
        readCoreFile(graph, partCount, partSize, regionFile);
        for (int i=0;i<partCount;i++) {
            allKnownNodes+=partSize[i];
        }
    } else {
        printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");
        cout<<"regionFileType: "<<regionFileType<<endl;
    }

    vector < int > coreName = vector < int >(allKnownNodes,-1);
    vector < vector <double> > distSumMatrix = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < vector <double> > distSqSumMatrix = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < vector <double> > distStdDevMatrix = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < vector <double> > distStdDevFluctMatrix = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < vector <double> > tranMatrix = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < vector <double> > tranMatrixNormal = vector < vector <double> > ( allKnownNodes, vector <double> (allKnownNodes,0));
    vector < double > tranArrayNormal = vector <double> (allKnownNodes,0);

    for (size_t i=0;i<graph.nodesLst.size();i++) {
        if (graph.nodesLst[i].auxID>=0) {
            coreName[graph.nodesLst[i].auxID]=i;
        }
    }

    cout<<"indexes found for "<< allKnownNodes <<" nodes"<<endl;
    for (int walker=0;walker<num_walker;walker++) {
        float totalJumpCnt=0;
        unsigned int initial_point;
        do {
            initial_point=rand()%graph.nodesLst.size();
        } while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next = initial_point;
        int prvKnownIndex = -1;
        int newKnownIndex = -1;
        int prvKnownStep = -1;

        for (int step=0;step<walkLen;step++) {
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
            if (graph.nodesLst[next].auxID>=0) {
                newKnownIndex = graph.nodesLst[next].auxID;
                if (prvKnownIndex>=0) {
                    if (prvKnownIndex != newKnownIndex) {
                        totalJumpCnt++;

                        int deg = graph.nodesLst[next].neighbors->size();
                        int n = tranMatrix[prvKnownIndex][newKnownIndex] + 1;
                        int dist = step-prvKnownStep;

                        tranMatrixNormal[prvKnownIndex][newKnownIndex] += 1.0/deg;
                        tranArrayNormal[prvKnownIndex] += 1.0/deg;

                        tranMatrix[prvKnownIndex][newKnownIndex] = n;

                        distSumMatrix[prvKnownIndex][newKnownIndex] += dist;
                        distSqSumMatrix[prvKnownIndex][newKnownIndex] += pow(dist,2);

                        if (n>2) {
                            double mean   = distSumMatrix[prvKnownIndex][newKnownIndex] / n;
                            double stdDev = sqrt((distSqSumMatrix[prvKnownIndex][newKnownIndex] / n) - pow(mean, 2));
                            if (n>3) {
                                double oldStdDev = distStdDevMatrix[prvKnownIndex][newKnownIndex];
                                distStdDevFluctMatrix[prvKnownIndex][newKnownIndex] = stdDev - oldStdDev;
                            }
                            distStdDevMatrix[prvKnownIndex][newKnownIndex] = stdDev;
                        }
                    }
                    prvKnownStep = step;
                }
                prvKnownIndex = newKnownIndex;
            }
        }
        cout<<"Walk "<<walker<<" ended: "<<totalJumpCnt<<endl;
    }

    float maxJumps = 0;
    for (int i=0;i<allKnownNodes;i++){
        if (tranArrayNormal[i]>maxJumps) {
            maxJumps=tranArrayNormal[i];
        }
    }

    ofstream ofile;
    char fname[1024];
    sprintf(fname,"%s/%s_jumpCount.txt",dumpDir,nettype);
    cout<<fname<<endl;
    ofile.open(outFile,ios::out);
    for (int i=0;i<allKnownNodes;i++) {
        for (int j=0;j<allKnownNodes;j++) {
            if (tranArrayNormal[i]>0) {
                ofile<<i<<"\t"<<j
                    <<"\t"<<tranMatrixNormal[i][j]/tranArrayNormal[i]
                    <<"\t"<<tranMatrixNormal[i][j]
                    <<"\t"<<(tranMatrixNormal[i][j]*maxJumps/tranArrayNormal[i])+(tranMatrixNormal[j][i]*maxJumps/tranArrayNormal[j])
                    <<"\t"<<tranMatrix[i][j]
                    <<"\t"<<distSumMatrix[i][j]
                    <<"\t"<<distSqSumMatrix[i][j]
                    <<"\t"<<distStdDevMatrix[i][j]
                    <<"\t"<<distStdDevFluctMatrix[i][j]<<endl;
            }
        }
    }
    ofile.close();

    sprintf(fname,"%s/%s_index.txt",dumpDir,nettype);
    ofile.open((const char*)fname,ios::out);
    for (int i=0;i<allKnownNodes;i++){
        ofile<<i<<"\t"<<coreName[i]<<endl;
    }
    ofile.close();
}

void randomWalkTransitionsSets(Graph graph, float num_walker_p, int jump_len, int seed, char* regionFile, int regionFileType) {
    int numJumps = 40;
    graph.calc();
    cout<<"# Graph Nodes: "<<graph.noNodes<<"\n";
    cout<<"# Graph Edges: "<<graph.noEdges<<"\n";

    //    int jumping_walkers=0;
    //    int allKnownNodes=0;
    int partCount=0;
    vector<int> partSize;

    if (regionFileType==0) {
        cout<<"# Reading info file"<<endl;
        readCoreFiles(graph, partCount, partSize, regionFile);
    } else if (regionFileType==1) {
        cout<<"# Reading part file"<<endl;
        readPartFile(graph, partCount, partSize, regionFile);
    } else {
        printf("Unexpected option : RWdistFile or nodesPartFile should be provided\n");
    }

    vector< vector < vector <int> > > jump;
    cout<<"part count: "<<partCount<<"\n";
    cout<<"partSize size: "<<partSize.size()<<"\n";

    for  (int k=0; k<numJumps;k++) {
        jump.push_back( vector < vector <int> > () );
    }
    for (int i=0;i<(partCount-1);i++) {
        for  (int k=0; k<numJumps;k++) {
            jump[k].push_back(vector <int>(partCount,0));
        }
        int setSumDegree=0;
        for (int n=0;n<graph.nodesLst.size();n++) {
            if ((graph.nodesLst[n].getRegion()==i) && (graph.nodesLst[n].neighbors->size()!=0)) {
                setSumDegree+=graph.nodesLst[n].neighbors->size();
            }
        }

        srand(seed);
        int num_walker=num_walker_p*partSize[i];

        for (int walker=0;walker<num_walker;walker++) {

            // int initial_point=-1;
            // do {
            //      initial_point=rand()%graph.nodesLst.size();
            // } while ((graph.nodesLst[initial_point].neighbors->size()==0) || (graph.nodesLst[initial_point].getRegion()!=i));

            int initial_point=-1;
            int cutDegree=(rand()%setSumDegree)+1;

            int counter=0;
            for (int n=0;n<graph.nodesLst.size();n++) {
                if ((graph.nodesLst[n].getRegion()!=i) || (graph.nodesLst[n].neighbors->size()==0)) {
                    continue;
                }
                counter += graph.nodesLst[n].neighbors->size();
                if (counter>=cutDegree) {
                    initial_point=n;
                    break;
                }
            }
            if (initial_point==-1) {
                cout<<"Initial node selection does not work"<<endl;
                exit(-1);
            }
            int next=initial_point;

            // cout <<setSumDegree <<"\t"<<cutDegree<<"\t"<<next<<"\t"<<graph.nodesLst[next].getRegion()<<endl;

            for (int k=0;k<(jump_len*numJumps);k++) {
                next=nextNodeIdSnowBall(graph.nodesLst[next]);
                if (next>=graph.nodesLst.size()) {
                    cout << "Error finding next snow ball node in " <<next<<"\t"<<graph.nodesLst[next].getRegion()<<endl;
                    exit(-1);
                }
                if ((k%jump_len)==(jump_len-1)) {
                    jump[k/jump_len][i][graph.nodesLst[next].getRegion()]++;
                }

                // cout <<">>\t"<<next<<"\t";
            }
            // cout<<endl;
        }
    }

    cout<<"\n# Jump Length: "<<jump_len;
    cout<<"\n# Transitions\n";
    for (int k=0;k<numJumps;k++) {
        cout<<"\n>> Length: "<<jump_len*(k+1)<<"\n";
        for (int j=0;j<partCount;j++)  {
            cout<<"\t#set("<<j<<")";
        }
        cout<<"\t|";
        for (int j=0;j<partCount;j++)  {
            cout<<"\t#set("<<j<<")";
        }
        cout<<"\n";
        for (int i=0;i<(partCount-1);i++) {
            cout<<"#set("<<i<<")\t";
            for (int j=0;j<partCount;j++) {
                cout<<jump[k][i][j]<<"\t";
            }
            cout<<"|\t";
            for (int j=0;j<partCount;j++) {
                cout<<(jump[k][i][j]*1.0)/((num_walker_p*partSize[i])-jump[k][i].back())<<"\t";
            }
            cout<<"\n";
        }
    }
    return;
}


// open some files containing set of node and their degrees. it computes RW distance of transitions
void randomWalkDistanceSets(Graph graph, int num_walker, int walk_len, int seed, char* file) {
    graph.calc();
    cout<<"Graph no nodes: "<<graph.noNodes<<"\n";
    cout<<"Graph no edges: "<<graph.noEdges<<"\n";


    int jump_len=1;
    int jumping_walkers=0;
    int set_no=0;
    int allKnownNodes=0;
    vector<int> set_size;

    readCoreFiles(graph, set_no, set_size,file);

    vector <int> set_visits;
    vector <int> set_transition;
    vector <int> set_self_transition;

    vector <double> set_self_dist;
    vector <double> set_diam;
    vector <double> set_diam_measure_cnt;

    vector < vector <int> > jump;
    vector < vector <double> > dist;
    vector < vector <double> > distQ;

    int jumpHistorySize=static_cast<int>(pow(static_cast<double>(set_no-1),jump_len+1));
    int jumpInitializeSize=static_cast<int>(pow(static_cast<double>(set_no-1),jump_len));
    int totalJumpHistory=0;
    vector <int> jumpHistory(jumpHistorySize,0);//=new int[jumpHistorySize];
    vector <int> jumpInitialize(jumpInitializeSize,0);//=new int[jumpInitializeSize];

    for (int i=0;i<set_no;i++) {
        set_visits.push_back(0);
        set_transition.push_back(0);
        set_self_transition.push_back(0);
        set_self_dist.push_back(0);
        set_diam.push_back(0);
        set_diam_measure_cnt.push_back(0);
        jump.push_back(vector <int>());
        dist.push_back(vector <double>());
        distQ.push_back(vector <double>());
        for (int i=0;i<set_no;i++) {
            jump.back().push_back(0);
            dist.back().push_back(0);
            distQ.back().push_back(0);
        }
    }

    cout<<"set no: "<<set_no<<"\n";
    cout<<"jump size: "<<jump.size()<<"\n";
    cout<<"jump[0] size: "<<jump[0].size()<<"\n";
    cout<<"set_size size: "<<set_size.size()<<"\n";
    cout<<"set_transition size: "<<set_transition.size()<<"\n";
    cout<<"set_visits size: "<<set_visits.size()<<"\n";

    int initial_point=0;
    srand(seed);
    for (int walker=0;walker<num_walker;walker++) {
        int step=0;
        int lastKnownStep=-1;
        int fstThisSet=-1;
        int knownNodes=0;

        bool jumped=false;
        int last_stat=-1;
        int this_stat=-1;


        FixedSizeFIFOQ fQ (jump_len);
        FixedSizeFIFOQ fQStep (jump_len);


        do {
            initial_point=rand()%graph.nodesLst.size();
        } while ((graph.nodesLst[initial_point].neighbors->size()==0));
        step++;
        int next=initial_point;

        while ((knownNodes<jump_len)&&(step<walk_len+1)) {
            set_visits[graph.nodesLst[next].getRegion()]++;
            if(graph.nodesLst[next].getRegion()!=(set_no-1)) {
                knownNodes++;
                fQ.push(next);
                fQStep.push(step);
                if ((knownNodes==1)&&(step>1)) jump[set_no-1][graph.nodesLst[next].getRegion()]++;
                lastKnownStep=step;
                fstThisSet=step;
            }
            step++;

            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }

        for (int n=step; n<walk_len+1; n++) {
            set_visits[graph.nodesLst[next].getRegion()]++;

            if(graph.nodesLst[next].getRegion()!=(set_no-1)) {
                last_stat=graph.nodesLst[fQ.last()].getRegion();
                lastKnownStep=fQStep.last();
                this_stat = graph.nodesLst[next].getRegion();
                if((this_stat!=last_stat)&&(this_stat!=set_no-1)) {
                    jumped=true;
                    if(fstThisSet!=-1) {
                        set_diam[last_stat]+=lastKnownStep-fstThisSet;
                        set_diam_measure_cnt[last_stat]++;
                        fstThisSet=n;
                    }
                    else
                        fstThisSet=n;
                }
                if ((this_stat!=-1)&&(last_stat!=-1)) {
                    set_transition[last_stat]++;
                    jump[last_stat][this_stat]++;
                    dist[last_stat][this_stat]=dist[last_stat][this_stat]+(n-lastKnownStep);
                    distQ[last_stat][this_stat]+=(n-lastKnownStep)*(n-lastKnownStep);
                    if(fQ.last()==next) {
                        set_self_transition[last_stat]++;
                        set_self_dist[last_stat]+=(n-lastKnownStep);
                    }
                } else {
                    cout<<"Error: Node beints to Unknown but was not marked"<<endl;
                }

                int jumpIndex=0;
                for(int i=(jump_len-1);i>=0;i--) {
                    if(graph.nodesLst[fQ.getElement(i)].getRegion()==-1) cout<<"error region\n";
                    jumpIndex+=static_cast<int>(pow(static_cast<double>(set_no-1),(i+1)))*graph.nodesLst[fQ.getElement(i)].getRegion();
                }
                jumpIndex+=this_stat;
                if(jumpIndex>jumpHistory.size()) cout<<"error jumpHistory\n";
                if(jumpIndex/(set_no-1)>jumpInitialize.size()) cout<<"error jumpInitialize "<<jumpIndex/(set_no-1)<<" [] "<<jumpInitialize.size()<<"\n";
                jumpInitialize[jumpIndex/(set_no-1)]++;
                jumpHistory[jumpIndex]++;
                totalJumpHistory++;
                //new known node seen, so now trans mitrix should be updated.
                knownNodes++;
                fQ.push(next);
                fQStep.push(n);
            }
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
            if (next>=graph.nodesLst.size()) cout << "Error finding next snow ball node in " <<next<<"\t"<<graph.nodesLst[next].getRegion()<<endl;

        }
        if (jumped==true) jumping_walkers++;
    }

    cout<<"\n# Walker Lenght: "<<walk_len;
    cout<<"\n# Transitioning Walkers: "<<jumping_walkers<<" / "<<num_walker<<"\n";

    cout<<"\n# Visits\t #Diam\n";
    for (int i=0;i<set_no;i++) {
        if(set_diam_measure_cnt[i]>0)
            cout<<"#set("<<i<<"):\t"<<set_visits[i]<<"\t"<<set_diam[i]/set_diam_measure_cnt[i]<<"\n";
        else
            cout<<"#set("<<i<<"):\t"<<set_visits[i]<<"\t"<<"Nan"<<"\n";
        allKnownNodes+=set_size[i];
    }
    allKnownNodes-=set_size[set_no-1];

    cout<<"\n# Transition\n";
    for (int i=0;i<set_no;i++) {
        cout<<"\t# set("<<i<<")";
    }
    cout<<"\n";
    for (int i=0;i<set_no;i++) {
        cout<<"#set("<<i<<")\t";
        for (int j=0;j<set_no;j++) {
            cout<<jump[i][j]<<"\t";
            if (jump[i][j]!=0) dist[i][j]=dist[i][j]/jump[i][j];
            else dist[i][j]=0;
        }
        cout<<"\n";
    }

    cout<<"\n3d View Boxes\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\tAvgDist\n";
    for (int i=0;i<(set_no-1);i++) {
        for (int j=0;j<(set_no-1);j++) {
            if (set_transition[i]>0) {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\t"<<dist[i][j]<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\t"<<dist[i][j]<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
            } else {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<0<<"\t"<<0<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<0<<"\t"<<0<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
            }
        }
        cout<<endl;
        for (int j=0;j<(set_no-1);j++)
        {
            if (set_transition[i]>0)
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\t"<<dist[i][j]<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\t"<<dist[i][j]<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
            }
            else
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<0<<"\t"<<0<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<0<<"\t"<<0<<"\t"<<set_size[j]<<"\t"<<allKnownNodes<<"\n";
            }
        }
        cout<<endl;
    }

    cout<<"#n3d View Boxes === No node level self transitions\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\tAvgDist\n";
    for (int i=0;i<(set_no-1);i++)
    {
        for (int j=0;j<(set_no-1);j++)
        {
            double meanDist=0;
            double meanProb=0;
            if (i==j)
            {
                if ((jump[i][j]-set_self_transition[i])>0) meanDist=((dist[i][j]*jump[i][j])-(set_self_dist[i]))/((jump[i][j]-set_self_transition[i])*1.0);
                if ((set_transition[i]-set_self_transition[i])>0) meanProb=((jump[i][j]-set_self_transition[i])*1.0)/((set_transition[i]-set_self_transition[i])*1.0);
            }
            else
            {
                meanDist=dist[i][j];
                if ((set_transition[i]-set_self_transition[i])>0) meanProb=((jump[i][j])*1.0)/((set_transition[i]-set_self_transition[i])*1.0);
            }

            int others = (i==j)? set_size[j]-1:set_size[j];

            cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<meanProb<<"\t"<<meanDist<<"\t"<<others<<"\t"<<allKnownNodes-1<<"\n";
            cout<<"set("<<i<<")\tset("<<j<<")\t"<<i<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<meanProb<<"\t"<<meanDist<<"\t"<<others<<"\t"<<allKnownNodes-1<<"\n";
        }
        cout<<endl;

        for (int j=0;j<(set_no-1);j++)
        {
            double meanDist=0;
            double meanProb=0;
            if (i==j)
            {
                if ((jump[i][j]-set_self_transition[i])>0) meanDist=((dist[i][j]*jump[i][j])-(set_self_dist[i]))/((jump[i][j]-set_self_transition[i])*1.0);
                if ((set_transition[i]-set_self_transition[i])>0) meanProb=((jump[i][j]-set_self_transition[i])*1.0)/((set_transition[i]-set_self_transition[i])*1.0);
            }
            else
            {
                meanDist=dist[i][j];
                if ((set_transition[i]-set_self_transition[i])>0) meanProb=((jump[i][j])*1.0)/((set_transition[i]-set_self_transition[i])*1.0);
            }
            int others = (i==j)? set_size[j]-1:set_size[j];

            cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j<<"\t"<<jump[i][j]<<"\t"<<meanProb<<"\t"<<meanDist<<"\t"<<others<<"\t"<<allKnownNodes-1<<"\n";
            cout<<"set("<<i<<")\tset("<<j<<")\t"<<i+1<<"\t"<<j+1<<"\t"<<jump[i][j]<<"\t"<<meanProb<<"\t"<<meanDist<<"\t"<<others<<"\t"<<allKnownNodes-1	<<"\n";
        }
        cout<<endl;
    }


    cout<<"\nNodeXL\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\tAvgDistance\n";
    for (int i=0;i<set_no;i++)
    {
        for (int j=0;j<set_no;j++)
        {
            if (set_transition[i]>0)
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<jump[i][j]*1.0/set_transition[i]*1.0<<"\t"<<dist[i][j]<<"\n";
            }
            else
            {
                cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<0<<"\t"<<0<<"\n";
            }
        }
    }


    cout<<"\nNodeXL no self loop\n";
    cout<<"#set\t#set\t#Transition\tTransitionProb\n";
    for (int i=0;i<set_no;i++)
    {
        for (int j=0;j<set_no;j++)
        {
            if (i==j)
            {
                if (set_transition[i]>0)
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]-set_self_transition[i]<<"\t"<<((jump[i][j]-set_self_transition[i])*100.0)/((set_transition[i]-set_self_transition[i])*1.0)<<"\n";
                }
                else
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]-set_self_transition[i]<<"\t"<<0<<"\n";
                }
            }
            else
            {
                if (set_transition[i]>0)
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<(jump[i][j]*100.0)/((set_transition[i]-set_self_transition[i])*1.0)<<"\n";
                }
                else
                {
                    cout<<"set("<<i<<")\tset("<<j<<")\t"<<jump[i][j]<<"\t"<<0<<"\n";
                }
            }
        }
    }

    cout<<"\n# Multistep step super space transition\n";
    cout<<"# total transitions: "<<totalJumpHistory<<"\n";
    vector<int> indexArray(jump_len+1,0);

    cout<<"index: route\tcount\tprob\ttotalProb\n";
    for (int i=0;i<jumpHistorySize;i++)
    {
        int index=i;
        int jumpInitilializeIndex=i/(set_no-1);
        int jumpedFrom=index / (int)static_cast<int>(pow(static_cast<double>(set_no-1),jump_len));
        cout<<index<<": ";
        for (int j=jump_len;j>0;j--)
        {
            cout << index / (int)static_cast<int>(pow(static_cast<double>(set_no-1),j))<<" => ";
            index = index % (int)static_cast<int>(pow(static_cast<double>(set_no-1),j));
        }
        cout<<index<<"\t"<<jumpHistory[i]<<"\t"<<jumpHistory[i]/(set_transition[jumpedFrom]*1.0)<<"\t"<<jumpHistory[i]/(jumpInitialize[jumpInitilializeIndex]*1.0)<<endl;
    }
    exit(-1);
}

// open some files containing set of node and their degrees. it computes RW distance of nodes in each group from anchor
// (highest degree node) of the both groups
// void randomWalkTransProb(Graph graph, int num_walker, int walk_len, int seed, char* file)
// {
// }


// Captures random walk topology of the top X high degree nodes and transition between them.
// Can be used to validate the clustering done over high degree nodes.
void randomWalkTopolgyHigh(Graph graph, int num_walker, int walk_len, char *dumpdir, char *nettype, int seed, char* file)
{
    vector<int> node_visits;
    vector<int> node_class;
    vector<int> node_id;

    //vector< vector <int> > class_nodes;
    vector< vector <int> > coap;
    vector< vector <float> > distance;

    string str;

    ifstream infile;
    infile.open(file);
    int currentNodeId=0;
    if(!infile)
    {
        cout << "Cannot open set file: "<<file<<".\n";
        exit(-1);
    }
    else
        //	cout << "#node\tid\tdegree\tclass\n";
    {
        // set_size.push_back(0);
        vector <string> tkns;
        while(!infile.eof())
        {
            str.clear();
            getline(infile,str);
            tkns.clear();
            Tokenize (str,tkns," ");
            //			Tokenize (str,tkns,"\t");
            if (tkns.size()>=2)
            {
                node_visits.push_back(0);
                int current=atoi(tkns[0].c_str());
                graph.nodesLst[current].setRegion(currentNodeId);
                node_id.push_back(current);
                //				cout << "#node("<<set_no<<")\t"<<graph.nodesLst[current].id<<"\t"<<graph.nodesLst[current].neighbors->size()<<"\t"<<tkns[2]<<"\n";
                node_class.push_back(atoi(tkns[3].c_str()));
                currentNodeId++;
                //				for (int x=0;x<tkns.size();x++)
                //				{
                //					cout<<x<<": "<<tkns[x]<<" - ";
                //				}
                //				exit(-1);
            }
        }
    }
    infile.close();
    //int nodeCount=currentNodeId+1;

    vector <int> tmp_vec_int(node_id.size(),0);
    vector <float> tmp_vec_flt(node_id.size(),0);
    for (int i=0;i<node_id.size();i++)
    {
        coap.push_back(tmp_vec_int);
        distance.push_back(tmp_vec_flt);
    }

    //cout<<tmp_vec_int.size()<<endl;
    //cout<<coap.size()<<endl;

    int initial_point=0;
    // vector <shWalkVNode> visited;
    srand(seed);

    for (int walker=0;walker<num_walker;walker++)
    {
        int last_step=0;
        int last_stat=-1;
        int this_stat=-1;
        do
        {
            initial_point=rand()%graph.nodesLst.size();
        }while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next=initial_point;
        for (int n=0; n<walk_len; n++)
        {
            this_stat = graph.nodesLst[next].getRegion();
            if (this_stat!=-1)
            {
                if (last_stat!=-1)
                {
                    node_visits[last_stat]++;
                    coap[last_stat][this_stat]++;
                    distance[last_stat][this_stat]+=n-last_step;
                }
                last_stat=this_stat;
                last_step=n;
            }
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }
    }
    for (int i=0;i<node_id.size();i++)
    {
        for (int j=0;j<node_id.size();j++)
        {
            if (coap[i][j]>0) distance[i][j]=distance[i][j]/(1.0*coap[i][j]);
            else distance[i][j]=0;
        }
    }

    cout<<"#node\t#CLS\t#node\t#CLS\t#Transition\t#Distance\t#Prob\n";
    for (int i=0;i<node_id.size();i++)
    {
        for (int j=0;j<node_id.size();j++)
        {
            cout<<i<<"\t"<<node_class[i]<<"\t"<<j<<"\t"<<node_class[j]<<"\t"<<coap[i][j]<<"\t";
            if (distance[i][j]>0) cout<<distance[i][j];
            else cout<<walk_len+1;
            cout<<"\t"<<((coap[i][j]*1.0/node_visits[i]*1.0)+(coap[j][i]*1.0/node_visits[j]*1.0))/2<<"\n";
        }
    }

    if ((strcmp(dumpdir,"-1")!=0)&&(strcmp(nettype,"-1")!=0))
    {
        char fname[128];
        ofstream ofile;
        sprintf(fname,"%s/%s_jumpCount.txt",dumpdir,nettype);
        //string fileName=fname;
        ofile.open((const char*)fname,ios::out);
        ofile<<"#node_no\t#node_id\t#CLS\t#node_no\t#node_id\t#CLS\t#Transition\t#MeanTrans\t#Distance\t#Prob\n";
        for (int i=0;i<node_id.size();i++)
        {
            for (int j=0;j<node_id.size();j++)
            {
                if (i==j) ofile<<i<<"\t"<<node_id[i]<<"\t"<<node_class[i]<<"\t"<<j<<"\t"<<node_id[j]<<"\t"<<node_class[j]<<"\t"<<0<<"\t"<<"0"<<"\t";
                else ofile<<i<<"\t"<<node_id[i]<<"\t"<<node_class[i]<<"\t"<<j<<"\t"<<node_id[j]<<"\t"<<node_class[j]<<"\t"<<coap[i][j]<<"\t"<<(coap[i][j]+coap[j][i])/2<<"\t";
                if (distance[i][j]>0) ofile<<distance[i][j];
                else ofile<<walk_len+1;
                ofile<<"\t"<<((coap[i][j]*1.0/node_visits[i]*1.0)+(coap[j][i]*1.0/node_visits[j]*1.0))/2<<"\n";
            }
        }
    }
    else
    {
        printf("Can not open %s/%s_jumpCount.txt\n",dumpdir,nettype);
    }
}


// Logs number of visits and number of visited walks for RW with discovery rate higher than thresh
void randomWalkAndProcessDiscThresh(Graph graph, int num_walker, int walk_len, char* dumpdir, char* nettype, int seed, float min_thresh,float max_thresh)
{
    int initial_point=0;
    int this_walker_visited_nodes=0;
    int min_visited_nodes=min_thresh*walk_len;
    int max_visited_nodes=max_thresh*walk_len;
    vector <VisitedNode_old> visited;
    vector <int> walk_log;
    int eligible_walks=0;
    // printf("thresh: %f\n",min_thresh);
    printf("min nodes: %d\n",min_visited_nodes);
    printf("max nodes: %d\n",max_visited_nodes);
    // getchar();
    srand(seed);
    for (int walker=0;walker<num_walker;walker++)
    {
        walk_log.clear();
        this_walker_visited_nodes=0;
        do
        {
            initial_point=rand()%graph.nodesLst.size();
        }while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next=initial_point;
        for (int n=0; n<walk_len; n++)
        {
            walk_log.push_back(next);
            int last=visited.size();
            while(visited.size()<=next)
            {
                visited.push_back(VisitedNode_old(last,-1));
                last++;
            }
            if (visited[next].degree==-1) visited[next].degree=graph.nodesLst[next].neighbors->size();
            if (visited[next].last_visited_walk!=walker)
            {
                visited[next].visited_walks++;
                visited[next].last_visited_walk=walker;
                this_walker_visited_nodes++;
            }

            if (visited[next].auxVar==-1) visited[next].auxVar=n;
            else if (visited[next].auxVar>n) visited[next].auxVar=n;
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }
        if((min_visited_nodes<=this_walker_visited_nodes)&&(this_walker_visited_nodes<=max_visited_nodes))
        {
            eligible_walks++;
            for(int i=0;i<walk_log.size();i++)
            {
                next=walk_log[i];
                visited[next].num_visit++;
            }
        }
        else
        {
            for(int i=0;i<walk_log.size();i++)
            {
                next=walk_log[i];
                if (visited[next].last_visited_walk==walker)
                {
                    visited[next].visited_walks--;
                    visited[next].last_visited_walk=-1;
                }
            }
        }
    }

    printf("dumping evaluations\n");

    char fname[128];
    sprintf(fname,"%s/%s_eval_wl%d_nw%d_sd%d_minTrsh%d_maxTrsh%d.txt",dumpdir,nettype,walk_len,num_walker,seed,min_visited_nodes,max_visited_nodes);
    // 	else sprintf(fname,"%s/%s_eval_wl%d_nw%d_sd%d.txt",dumpdir,nettype,walk_len,num_walker,seed);
    FILE *oFile = fopen(fname, "w");
    fprintf(oFile,"# NODES:%d\n",graph.nodesLst.size());
    fprintf(oFile,"# Connected Component Size:%d\n",graph.noNodes);
    fprintf(oFile,"# EDGES:%d\n",graph.noEdges);
    fprintf(oFile,"# Number of eligible walkers: %d\n",eligible_walks);
    fprintf(oFile,"#id\tdegree\tnvw\tnv\n");

    for (int i=0; i<visited.size(); i++)
    {
        if (visited[i].degree>0)
        {
            // 			int nvw_g_trsh=0;
            // 			int min = visited[i].num_visit_per_walk[0];
            // 			int max = visited[i].num_visit_per_walk[0];
            // 			float avg =visited[i].num_visit_per_walk[0];
            // 			for (int j=1;j<visited[i].num_visit_per_walk.size();j++)
            // 			{
            // 				if (visited[i].num_visit_per_walk[j]>=thresh) nvw_g_trsh++;
            // 				if (min > visited[i].num_visit_per_walk[j]) min = visited[i].num_visit_per_walk[j];
            // 				if (max < visited[i].num_visit_per_walk[j]) max = visited[i].num_visit_per_walk[j];
            // 				avg+=visited[i].num_visit_per_walk[j];
            // 			}
            // 			avg=avg/(float)visited[i].num_visit_per_walk.size();
            // 			float stddev=0;
            // 			for (int j=0;j<visited[i].num_visit_per_walk.size();j++)
            // 			{
            // 				stddev=stddev+(float)((visited[i].num_visit_per_walk[j]-avg)*(visited[i].num_visit_per_walk[j]-avg));
            // 			}
            // 			stddev=stddev/(float)visited[i].num_visit_per_walk.size();
            // 			if(thresh>1) fprintf(oFile,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\n",visited[i].id,visited[i].degree,visited[i].visited_walks,visited[i].num_visit,nvw_g_trsh,max,min,stddev);
            fprintf(oFile,"%d\t%d\t%d\t%d\n",visited[i].id,visited[i].degree,visited[i].visited_walks,visited[i].num_visit);
        }
    }
    fclose(oFile);
}


// Logs the discovery rate of different RWs
void randomWalkDiscoveredNodes(Graph graph, int num_walker, int walk_len, char* dumpdir, char* nettype, int seed) {
    //	char fname[128];
    //	sprintf(fname,"%s/%s_walk_disc_wl%d_nw%d_sd%d.txt",dumpdir,nettype,walk_len,num_walker,seed);
    //	FILE *oFile = fopen(fname, "w");
    //	fprintf(oFile,"#walk_id\tVisited\n");

    //	int initial_point=0;
    //	vector <VisitedNode> visited;
    //	srand(seed);
    //	for (int walker=0;walker<num_walker;walker++) {
    //		int this_walker_visited_nodes=0;
    //		do {
    //			initial_point=rand()%graph.nodesLst.size();
    //		}while (graph.nodesLst[initial_point].neighbors->size()==0);
    //		int next=initial_point;
    //		for (int n=0; n<walk_len; n++) {
    //			int last=visited.size();
    //			while(visited.size()<=next) {
    //				visited.push_back(VisitedNode(last,-1));
    //				last++;
    //			}
    //			if (visited[next].degree==-1) visited[next].degree=graph.nodesLst[next].neighbors->size();
    //			if (visited[next].last_visited_walk!=walker) {
    //				visited[next].visited_walks++;
    //				visited[next].last_visited_walk=walker;
    //				this_walker_visited_nodes++;
    //			}
    //			visited[next].num_visit++;

    //			if (visited[next].auxVar==-1) visited[next].auxVar=n;
    //			else if (visited[next].hop>n) visited[next].auxVar=n;
    //			next=nextNodeIdSnowBall(graph.nodesLst[next]);
    //		}
    //		fprintf(oFile,"%d\t%d\n", walker,this_walker_visited_nodes);
    //	}
    //	fclose(oFile);
}


// open a file containing set of node and their degrees. it computes RW Discovery rate from nodes in that set
// note that this number should be normalized by number of walkers used.
void randomWalkDiscoveryRate(Graph graph, int num_walker, int walk_len, int seed, char* file0) {
    vector <int> source_set;
    source_set.clear();
    string str;
    ifstream infile;
    infile.open(file0);
    if(!infile) {
        cout << "Cannot open infile_name: "<<file0<<".\n";
        exit(-1);
    }
    vector <string> tkns;
    while(!infile.eof())
    {
        str.clear();
        getline(infile,str);
        tkns.clear();
        Tokenize (str,tkns,"\t");
        if (tkns.size()>0)
        {
            int current=atoi(tkns[0].c_str());
            source_set.push_back(current);
        }
    }
    infile.close();

    printf("# Initial set size: %d \n",source_set.size());

    random_shuffle(source_set.begin(),source_set.end());

    source_set.erase (source_set.begin()+5,source_set.end());

    int initial_point=0;
    vector <VisitedNode_old> visited;
    srand(seed);

    for (int walker=0;walker<num_walker;walker++)
    {
        // bool set_visited[2]={false,false};
        // int last_step=0;
        //tmp_target_set[0]=target_set[0];
        //tmp_target_set[1]=target_set[1];
        do
        {
            int initial_point_idx=rand()%source_set.size();
            initial_point=source_set[initial_point_idx];
        }while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next=initial_point;
        for (int n=0; n<walk_len; n++)
        {
            int last=visited.size();
            while(visited.size()<=next)
            {
                visited.push_back(VisitedNode_old(last,-1));
                last++;
            }
            if (visited[next].degree==-1) visited[next].degree=graph.nodesLst[next].neighbors->size();
            if (visited[next].last_visited_walk!=walker)
            {
                visited[next].visited_walks++;
                visited[next].last_visited_walk=walker;
                // visited[next].num_visit_per_walk.push_back(1);
            }
            else
            {
                // visited[next].num_visit_per_walk.back()++;
            }
            visited[next].num_visit++;

            if (visited[next].auxVar==-1) visited[next].auxVar=n;
            else if (visited[next].auxVar>n) visited[next].auxVar=n;
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }
    }

    vector<int> no_nodes_dicovered;
    for (int i=0;i<walk_len;i++) no_nodes_dicovered.push_back(0);

    for (int i=0;i<visited.size();i++) {
        if(visited[i].auxVar!=-1) no_nodes_dicovered[visited[i].auxVar]++;
    }

    int cummulative=0;
    printf("# num walker:%d \n",num_walker);
    printf("# walk length: %d \n",walk_len);
    printf("# Source set size: %d \n",source_set.size());
    printf("#step\t#no nodes\n");
    for (int i=0;i<walk_len;i++)
    {
        cummulative=cummulative+no_nodes_dicovered[i];
        printf("%d\t%d\n",i,cummulative);
    }
}

// Prints RW traces in the file (generataed for Daniel but extended for nvw(k) vs nvw)
void randomWalkAndLog(Graph graph, int num_walker, int walk_len ,char* dumpdir, char* nettype, int seed) {
    graph.calc();
    printf("# NODES:%d\n",graph.nodesLst.size());
    printf("# Connected Component Size:%d\n",graph.noNodes);
    printf("# EDGES:%d\n",graph.noEdges);

    int initial_point=0;
    vector <VisitedNode_old> visited;
    srand(seed);
    for (int walker=0;walker<num_walker;walker++) {
        do {
            initial_point=rand()%graph.nodesLst.size();

        } while (graph.nodesLst[initial_point].neighbors->size()==0);
        int next=initial_point;
        for (int n=0; n<walk_len; n++) {
            int last=visited.size();
            while(visited.size()<=next) {
                visited.push_back(VisitedNode_old(last,-1));
                last++;
            }
            if (visited[next].degree==-1) visited[next].degree=graph.nodesLst[next].neighbors->size();
            if (visited[next].last_visited_walk!=walker) {
                visited[next].visited_walks++;
                visited[next].last_visited_walk=walker;
                //				visited[next].num_visit_per_walk.push_back(1);
            }
            else
            {
                //				visited[next].num_visit_per_walk.back()++;
            }
            visited[next].num_visit++;
            next=nextNodeIdSnowBall(graph.nodesLst[next]);
        }
    }

    //===============Evaluate Estimation of number of visits========================
    for(int node_id=0;node_id<visited.size();node_id++) {
        visited[node_id].est_num_visit=0;
        //		visited[node_id].var_est_num_visit=0;
        //		visited[node_id].var_est_num_visit_2=0;
    }
    double budget=walk_len*num_walker;
    for(int node_id=0;node_id<visited.size();node_id++)
    {
        if(visited[node_id].num_visit>0) {
            double p=visited[node_id].num_visit*1.0/(budget*1.0);
            //			visited[node_id].var_num_visit+=visited[node_id].num_visit*static_cast<int>(pow(static_cast<double>(1-p),2));
            //			visited[node_id].var_num_visit+=(budget-visited[node_id].num_visit)*static_cast<int>(pow(static_cast<double>(p),2));
            //			visited[node_id].var_num_visit=sqrt(visited[node_id].var_num_visit);

            double credit=(visited[node_id].num_visit*1.0)/(visited[node_id].degree*1.0);
            for(int nei=0;nei<graph.nodesLst[node_id].neighbors->size();nei++)
            {
                int nei_id=graph.nodesLst[node_id].neighbors->at(nei);
                int last=visited.size();
                while(visited.size()<=nei_id) {
                    visited.push_back(VisitedNode_old(last,-1));
                    last++;
                }
                if (visited[nei_id].degree==-1) visited[nei_id].degree=graph.nodesLst[nei_id].neighbors->size();
                visited[nei_id].est_num_visit+=credit;
            }
        }
    }
    printf("dumping evaluations\n");
    char fname[128];
    sprintf(fname,"%s/%s_eval_wl%d_nw%d_sd%d.txt",dumpdir,nettype,walk_len,num_walker,seed);
    FILE *oFile = fopen(fname, "w");
    fprintf(oFile,"# NODES:%d\n",graph.nodesLst.size());
    fprintf(oFile,"# Connected Component Size:%d\n",graph.noNodes);
    fprintf(oFile,"# EDGES:%d\n",(int)graph.noEdges);
    fprintf(oFile,"#id\tdegree\tvis_wa\tno_vis\test_no_vis");
    //	fprintf(oFile,"\tVisits per Walker List");
    fprintf(oFile,"\n");
    for (unsigned int i=0; i<visited.size(); i++)
    {
        if (visited[i].degree>0)
        {
            fprintf(oFile,"%d\t%d\t%d\t%d\t%f",visited[i].id,visited[i].degree,visited[i].visited_walks,visited[i].num_visit,visited[i].est_num_visit);
            fprintf(oFile,"\n");
        }
    }
    fclose(oFile);
}

void randomWalkEscapeRate(Graph graph, int partIndex, double nwPortion,  int walk_len, int simulationRounds, char* dumpdir, char* nettype, int seed)
{
    graph.calc();
    vector <int> escaped(walk_len+1,0);
    graph.calc();
    // vector <shWalkVNode> visited;
    // number of walkers is set to 20% of nodes
    int num_walker=nwPortion*partIndex;
    srand(seed);
    for (int round=0; round<simulationRounds; round++)
    {
        vector <int> initial_points;
        for (int walker=0;walker<num_walker;walker++)
        {
            int initial_point;
            do
            {
                initial_point=rand()%graph.nodesLst.size();
            }while ((graph.nodesLst[initial_point].neighbors->size()==0)||(initial_point>partIndex));
            initial_points.push_back(initial_point);
        }
        cout<<"initial_points initialized\n";
        for(int cur_wl_pos=1;cur_wl_pos<=walk_len;cur_wl_pos++)
        {
            for (int walker=0;walker<num_walker;walker++)
            {
                int next=initial_points[walker];
                if (next==-1)
                {
                    escaped[cur_wl_pos]++;
                }
                else
                {
                    next=nextNodeIdSnowBall(graph.nodesLst[next]);
                    if (next<partIndex) { initial_points[walker]=next; }
                    else { initial_points[walker]=-1; }
                }
            }
        }
    }
    char fname[128];
    sprintf(fname,"%s/%s_escaped_nwP%f_wl%d_sim%d_sd%d.txt",dumpdir,nettype,nwPortion,walk_len,simulationRounds,seed);
    FILE *oFile = fopen(fname, "w");
    fprintf(oFile,"# NODES:%d\n",graph.nodesLst.size());
    fprintf(oFile,"# Connected Component Size:%d\n",graph.noNodes);
    fprintf(oFile,"# EDGES:%d\n",(int)graph.noEdges);
    fprintf(oFile,"# number of walkers: %d\n",num_walker);
    fprintf(oFile,"#wlen\t#escaped\n");
    for (unsigned int i=0;i<escaped.size();i++) {
        fprintf(oFile,"%d\t%f\n",i,escaped[i]/(simulationRounds*num_walker*1.0));
    }
    fclose(oFile);
    printf("dumped evaluations\n");
}

// perform random walks with different length at each 10 steps stores results in files
void randomWalkAndProcess(Graph graph, int maxWL, float nwPortion, char* dumpdir, char* nettype, int seed, bool excludeInit) {
    int batch = 10;
    //	float z = 2.718;
    graph.calc();
    printf("# NODES:%d\n",graph.nodesLst.size());
    printf("# Connected Component Size:%d\n",graph.noNodes);
    printf("# EDGES:%f\n",graph.noEdges);

    vector <int> wls;
    unsigned begin_t=clock();
    for (int i=0;i<=(maxWL/10);i++) wls.push_back(10*i);	//int wls[]={10,20,50,100,200,500,1000};
    int max_wl_ptr=wls.size();								//int max_wl_ptr=sizeof wls/sizeof(int);
    graph.calc();
    vector <unsigned int> initial_points;
    vector <VisitedNode> visited;
    for (unsigned int i=0;i<graph.nodesLst.size();i++) {
        visited.push_back(VisitedNode(i, graph.nodesLst[i].neighbors->size(), batch));
    }

    int num_walker=( (int) (nwPortion*graph.noNodes/batch) )*batch;
    int perBatchWalkCount=num_walker/batch;
    srand(seed);
    for (int walker=0;walker<num_walker;walker++) {
        unsigned int initial_point;
        do {
            initial_point=rand()%graph.nodesLst.size();
        } while (graph.nodesLst[initial_point].neighbors->size()==0);
        if(excludeInit) {
            for(int i=0;i<10;i++) {
                initial_point=nextNodeIdSnowBall(graph.nodesLst[initial_point]);
            }
        }
        initial_points.push_back(initial_point);
    }
    unsigned end_t=clock();
    printf("Starting Points initialized for %d(%d) walks in %f secs.\n",num_walker,perBatchWalkCount,double(diffclock(end_t,begin_t)));

    for(int cur_wl_ptr=1;cur_wl_ptr<max_wl_ptr;cur_wl_ptr++) {
        int walk_len=wls[cur_wl_ptr];
        begin_t=clock();
        for (int b = 0; b < batch; b++) {
            unsigned begin_t1=clock();
            for (int w=0;w<perBatchWalkCount;w++) {
                int walker=w+(b*perBatchWalkCount);
                unsigned int next=initial_points[walker];
                for (int n=wls[cur_wl_ptr-1]; n<walk_len; n++) {
                    visited[next].visited_by(b);
                    next=nextNodeIdSnowBall(graph.nodesLst[next]);
                }
                initial_points[walker]=next;
            }
            unsigned end_t1=clock();
            if (b==0) printf("Batch %d Random Walk performed in %f secs.\n",b,double(diffclock(end_t1,begin_t1)));
            /* =============== END Evaluate Estimation of number of visits======================== */
        }

        unsigned begin_t1=clock();
        /* ===============Evaluate Estimation of number of visits======================== */
        for(unsigned int node_id=0;node_id<visited.size();node_id++) {
            std::fill(visited[node_id].est_num_visit_array.begin(), visited[node_id].est_num_visit_array.end(), 0);
        }
        for(unsigned int node_id=0;node_id<visited.size();node_id++) {
            vector<float> credit = vector<float>(batch,0);
            for (int b=0;b<batch;b++) {
                credit[b]=(visited[node_id].num_visit_array[b]*batch)/(visited[node_id].degree);
            }
            for(unsigned int nei=0;nei<graph.nodesLst[node_id].neighbors->size();nei++) {
                int nei_id=graph.nodesLst[node_id].neighbors->at(nei);
                for (int b=0;b<batch;b++) {
                    visited[nei_id].est_num_visit_array[b]+=credit[b];
                }
            }

            //			if(visited[node_id].num_visit_array[b]>0) {
            //				double credit=(visited[node_id].num_visit_array[b]*batch)/(visited[node_id].degree);
            //				for(unsigned int nei=0;nei<graph.nodesLst[node_id].neighbors->size();nei++) {
            //					int nei_id=graph.nodesLst[node_id].neighbors->at(nei);
            //					visited[nei_id].est_num_visit_array[b]+=credit;
            //				}
            //			}
        }
        unsigned end_t1=clock();
        printf("Estimate visit calculated for in %f secs.\n",double(diffclock(end_t1,begin_t1)));

        end_t=clock();
        printf("Random Walk with lengths performed in %f secs.\n",walk_len,double(diffclock(end_t,begin_t)));

        /* ====== Calculate the variance based on batches ====== */
        begin_t=clock();
        for(unsigned int node_id=0;node_id<visited.size();node_id++) {
            visited[node_id].est_num_visit=0;
            visited[node_id].est_num_visit_var=0;

            double sum=0;
            double sumSq=0;

            double sumInv=0;
            double sumInvSq=0;
            for (int b=0;b<batch;b++) {
                sum += visited[node_id].est_num_visit_array[b];
                sumSq += visited[node_id].est_num_visit_array[b]*visited[node_id].est_num_visit_array[b];

                sumInv += 1.0/visited[node_id].est_num_visit_array[b];
                sumInvSq += 1.0/(visited[node_id].est_num_visit_array[b]*visited[node_id].est_num_visit_array[b]);
            }
            double mean=sum/batch;
            double variance=(sumSq/batch)-mean*mean;

            double invMean=sumInv/batch;
            double invVariance=(sumInvSq/batch)-invMean*invMean;

            visited[node_id].est_num_visit          = mean;
            visited[node_id].est_num_visit_var		= variance;

            visited[node_id].meanInvNumVisit        = invMean;
            visited[node_id].varInvNumVisit		    = invVariance;
        }
        end_t=clock();
        printf("Calculated E[Visit] and conf in %f secs.\n",double(diffclock(end_t,begin_t)));
        /* ====== END Calculate the variance based on batches ====== */

        begin_t=clock();
        char fname[1024];
        sprintf(fname,"%s/%s_eval_nwP%f_wl%d_sd%d.txt",dumpdir,nettype,nwPortion,walk_len,seed);
        FILE *oFile = fopen(fname, "w");
        //		fprintf(oFile,"# NODES:%d\n",graph.nodesLst.size());
        //		fprintf(oFile,"# Connected Component Size:%d\n",graph.noNodes);
        //		fprintf(oFile,"# EDGES:%f\n",graph.noEdges);
        //		fprintf(oFile,"# number of walkers: %d\n",num_walker);
        //		fprintf(oFile,"#id\tdegree\tnv\tE[nv]\tE[nv].stdDev\tE[nv].conf\n");
        for (unsigned int i=0; i<visited.size(); i++) {
            if (visited[i].est_num_visit>0) {
                fprintf(oFile,"%d\t%d\t",visited[i].id,visited[i].degree);
                fprintf(oFile,"%d\t",(int)visited[i].num_visit);
                fprintf(oFile,"%f\t",visited[i].est_num_visit);
                fprintf(oFile,"%f\t",visited[i].est_num_visit_var);
                fprintf(oFile,"%e\t",visited[i].meanInvNumVisit);
                fprintf(oFile,"%e\t",visited[i].varInvNumVisit);
                fprintf(oFile,"\n");
            }
        }
        fclose(oFile);
        end_t=clock();
        printf("Dumped evisits in %f secs.\n",double(diffclock(end_t,begin_t)));
        cout.flush();
    }
    cout<<"visited vector size: "<<visited.size()<<"\n";
    printf("dumped evaluations\n");
}

// Page Rank
void pageRank(Graph graph, int minWL, int maxWL, int stepWL, char* dumpdir, char* netType, int walkType) {
    graph.calc();
    vector <int> wls;
    unsigned begin_t,end_t=clock();
    wls.push_back(0);
    for (int i=minWL;i<=maxWL;i+=stepWL) wls.push_back(i);	//int wls[]={10,20,50,100,200,500,1000};
    int max_wl_ptr=wls.size();								//int max_wl_ptr=sizeof wls/sizeof(int);

    vector <VisitedNode> visited;
    for (unsigned int i=0;i<graph.nodesLst.size();i++) {
        visited.push_back(VisitedNode(i, graph.nodesLst[i].neighbors->size()));
        visited[i].est_num_visit=0.0;
    }
    if (walkType==3) {
        for (unsigned int i=0;i<graph.nodesLst.size();i++) {
            unsigned int initNode = rand()%graph.nodesLst.size();
            visited[initNode].est_num_visit++;
        }
    }

    cout<<visited.size()<<endl;

    begin_t = clock();

    for(int cur_wl_ptr=1;cur_wl_ptr<max_wl_ptr;cur_wl_ptr++) {
        int walk_len=wls[cur_wl_ptr];
        for (int n=wls[cur_wl_ptr-1]; n<wls[cur_wl_ptr]; n++) {
            for(unsigned int node_id=0;node_id<visited.size();node_id++) {
                if(visited[node_id].degree>0) {
                    double credit= 0;
                    if (walkType==2) {
                        if (n==0)	credit=(1.0+visited[node_id].est_num_visit)/(visited[node_id].degree*1.0);
                        else		credit=(visited[node_id].est_num_visit)/(visited[node_id].degree*1.0);
                    } else if (walkType==1) {
                        credit=(1.0+visited[node_id].est_num_visit)/(visited[node_id].degree*1.0);
                    } else if (walkType==3) {
                        credit=(visited[node_id].est_num_visit)/(visited[node_id].degree*1.0);
                    }

                    for(unsigned int nei=0;nei<graph.nodesLst[node_id].neighbors->size();nei++) {
                        int nei_id=graph.nodesLst[node_id].neighbors->at(nei);
                        visited[nei_id].num_visit+=credit;
                    }
                }
            }
            for(unsigned int node_id=0;node_id<visited.size();node_id++) {
                visited[node_id].est_num_visit_var += visited[node_id].est_num_visit-visited[node_id].num_visit;

                visited[node_id].est_num_visit=visited[node_id].num_visit;
                visited[node_id].num_visit=0;
            }
        }

        end_t=clock();
        printf("Page Rank for %d steps calculated in %f secs.\n",walk_len,double(diffclock(end_t,begin_t)));

        unsigned begin_dump=clock();
        char fname[128];
        sprintf(fname,"%s/%s_pageRank%d_wl%d.txt",dumpdir,netType,walkType,walk_len);
        FILE *oFile = fopen(fname, "w");
        for (unsigned int i=0; i<visited.size(); i++) {
            if (visited[i].est_num_visit>0) {
                fprintf(oFile,"%d\t%d\t",visited[i].id,visited[i].degree);
                fprintf(oFile,"%f\t",visited[i].est_num_visit_var);
                fprintf(oFile,"%f\t",visited[i].est_num_visit);
                fprintf(oFile,"\n");
            }
        }
        fclose(oFile);
        unsigned end_dump=clock();
        printf("Dumped page rank in %f secs.\n",double(diffclock(end_dump,begin_dump)));
        cout.flush();
    }
    cout<<"visited vector size: "<<visited.size()<<"\n";
    printf("dumped evaluations\n");
}

// EVALUATE MIXING TIME OF A GRAPH
void evalMixing(Graph graph, float nwPortion, char* dumpdir, char* nettype, int seed)
{
    graph.calc();
    vector <double> maxErrorVec;
    vector <double> avgErrorVec;
    vector <double> maxRelErrorVec;
    vector <double> avgRelErrorVec;

    vector <double> _90ErrorVec;
    vector <double> _75ErrorVec;
    vector <double> _50ErrorVec;
    vector <double> _25ErrorVec;
    vector <double> _10ErrorVec;

    map<int, int> degFreq;
    for(unsigned int node_id=0;node_id<graph.nodesLst.size();node_id++)
    {
        int k = graph.nodesLst[node_id].neighbors->size();//graph.nodesLst[next].neighbors->size();

        map<int, int>::iterator lb = degFreq.lower_bound(k);
        if(lb != degFreq.end() && !(degFreq.key_comp()(k, lb->first)))
            (*lb).second++;
        else
        {
            int v=1;
            degFreq.insert(lb, map<int, int>::value_type(k, v));
        }
    }


    vector <int> wls;
    for (int i=1;i<10;i++) wls.push_back(i);
    for (int i=1;i<10;i++) wls.push_back(10*i);	//int wls[]={10,20,50,100,200,500,1000};
    for (int i=1;i<=10;i++) wls.push_back(100*i);	//int wls[]={10,20,50,100,200,500,1000};
    // wls.push_back(100);
    int max_wl_ptr=wls.size();				//int max_wl_ptr=sizeof wls/sizeof(int);
    graph.calc();
    vector <int> initial_points;
    int num_walker=nwPortion*graph.noNodes;

    srand(seed);

    for (int walker=0;walker<num_walker;walker++)
    {
        int initial_point=0;
        //		do
        //		{
        //			initial_point=rand()%graph.nodesLst.size();
        //		}while (graph.nodesLst[initial_point].neighbors->size()==0);
        initial_points.push_back(initial_point);
    }
    cout<<"initial_points initialized\n";
    for(int cur_wl_ptr=0;cur_wl_ptr<max_wl_ptr;cur_wl_ptr++)
    {
        map<int, int> freq;
        for (int walker=0;walker<num_walker;walker++)
        {
            int next=initial_points[walker];
            int n=0;
            if (cur_wl_ptr>0)
            {
                n=wls[cur_wl_ptr-1];
            }
            for (; n<wls[cur_wl_ptr]; n++)
            {
                next=nextNodeIdSnowBall(graph.nodesLst[next]);
            }
            initial_points[walker]=next;

            //			int k = next;//graph.nodesLst[next].neighbors->size();
            int k = graph.nodesLst[next].neighbors->size();
            map<int, int>::iterator lb = freq.lower_bound(k);
            if(lb != freq.end() && !(freq.key_comp()(k, lb->first)))
            {
                // key already exists
                // update lb->second if you care to
                (*lb).second++;
            }
            else
            {
                // the key does not exist in the map
                // add it to the map
                int v=1;
                freq.insert(lb, map<int, int>::value_type(k, v));
            }
        }

        double sumError=0;
        double maxError=0;
        double maxRelError=0;
        double sumRelError=0;
        vector<double> errorVecTmp;
        for(unsigned int node_id=0;node_id<graph.nodesLst.size();node_id++)
        {
            //			int k = node_id;
            int k = graph.nodesLst[node_id].neighbors->size();
            double f = 0;
            map<int, int>::iterator lb = freq.lower_bound(k);
            if(lb != freq.end() && !(freq.key_comp()(k, lb->first)))
            {
                f=(*lb).second;
            }

            double deg 		= graph.nodesLst[node_id].neighbors->size();
            //			double error	= (deg/graph.noEdges)-(f/num_walker);
            double error	= (degFreq[deg]*deg/graph.noEdges)-(f/num_walker);
            if (f==0) continue;

            errorVecTmp.push_back(abs(error));
            //			errorVecTmp.push_back(abs(error/(deg/graph.noEdges)));

            if (abs(error)>maxError) maxError=abs(error);

            if (abs(error/(deg/graph.noEdges))>maxRelError) maxRelError=abs(error/(deg/graph.noEdges));
            sumRelError	+=abs(error/(deg/graph.noEdges));

        }
        sort(errorVecTmp.begin(), errorVecTmp.end());

        avgErrorVec.push_back(sumError/graph.nodesLst.size());
        maxErrorVec.push_back(maxError);
        avgRelErrorVec.push_back(sumRelError/graph.nodesLst.size());
        maxRelErrorVec.push_back(maxRelError);

        _90ErrorVec.push_back(errorVecTmp[errorVecTmp.size()*0.9]);
        _75ErrorVec.push_back(errorVecTmp[errorVecTmp.size()*0.75]);
        _50ErrorVec.push_back(errorVecTmp[errorVecTmp.size()*0.5]);
        _25ErrorVec.push_back(errorVecTmp[errorVecTmp.size()*0.25]);
        _10ErrorVec.push_back(errorVecTmp[errorVecTmp.size()*0.1]);
        errorVecTmp.clear();

    }

    char fname[128];
    sprintf(fname,"%s/%s_mixErrorDist_nwP%f_sd%d.txt",dumpdir,nettype,nwPortion,seed);
    FILE *oFile = fopen(fname, "w");
    fprintf(oFile,"# NODES:%d\n",graph.nodesLst.size());
    fprintf(oFile,"# Connected Component Size:%d\n",graph.noNodes);
    fprintf(oFile,"# EDGES:%d\n",graph.noEdges);
    fprintf(oFile,"# number of walkers: %d\n",num_walker);
    fprintf(oFile,"#wl\tmaxErr\t_10Err\t_25Err\t_50Err\t_75Err\t_90Err\tavgErr\tmaxRelErr\tavgRelErr\n");

    for(int wl_ptr=0;wl_ptr<max_wl_ptr;wl_ptr++)
    {
        fprintf(oFile,"%d\t%f\t",wls[wl_ptr],maxErrorVec[wl_ptr]);
        fprintf(oFile,"%f\t%f\t%f\t%f\t%f\t",_10ErrorVec[wl_ptr],_25ErrorVec[wl_ptr],_50ErrorVec[wl_ptr],_75ErrorVec[wl_ptr],_90ErrorVec[wl_ptr]);
        fprintf(oFile,"%f\t%f\t%f\n",avgErrorVec[wl_ptr],maxErrorVec[wl_ptr],avgErrorVec[wl_ptr]);
    }
    fclose(oFile);
    printf("dumped evaluations\n");
}



vector < vector <WalkedNode> > aggregateWalkers_visibles(int num_walker,int walk_len,int walker_idx)
{
    float alpha=0.5;

    int maxNoEntrance;

    char fileName[128];
    int vertex;
    int deg;
    int noEdges=0,noNodes=0;
    vector <WalkedNode> visited;
    int loc;
    int latestTransition=0;
    vector <int> convVec;
    char *line=NULL;
    size_t linesize;
    vector < vector <int> > noVisited;
    vector < vector <int> > noVisible;
    vector < vector <int> > noNewVis;
    vector < vector <int> > noNewInvis;
    //	vector <float>

    for (int n=0; n<num_walker; n++)
    {
        maxNoEntrance=0;

        noVisited.push_back(vector <int>());
        noVisible.push_back(vector <int>());
        noNewVis.push_back(vector <int>());
        noNewInvis.push_back(vector <int>());

        printf("evaluating walk %d of %d: walk %d\n",n+1,num_walker,n+walker_idx);
        sprintf(fileName,"rawDump_L%d_%d.txt",150000,n+walker_idx);
        FILE *fh = fopen(fileName, "r");
        if (getline(&line, &linesize, fh)>0)
            sscanf(line, "#%d %d", &noEdges,&noNodes);
        loc = 0; //walk_len;
        while (getline(&line, &linesize, fh)>0)
        {
            if ( sscanf(line, "%d\t%d", &vertex, &deg)==2 )
            {
                int last=visited.size();;
                while(visited.size()<=vertex)
                {
                    visited.push_back(WalkedNode(last,0));
                    last++;
                }
                if(visited[vertex].visitedWalks.size()>0)
                {
                    if ((visited[vertex].visitedWalks.back()!=n))
                    {
                        visited[vertex].visitedWalks.push_back(n);
                    }
                }
                else
                {
                    visited[vertex].id=vertex;
                    visited[vertex].degree=deg;//push_back(deg);

                    visited[vertex].visitedWalks.push_back(n);
                }
                visited[vertex].stat[n].noVisits++;

            }
            loc++;
        }

        if (loc!=walk_len) printf("walk log was not long enough\n");
        fclose(fh);


        for (int nid=0; nid<visited.size(); nid++)
        {
            if (visited[nid].visitedWalks.size()>0)
            {
                if (visited[nid].visitedWalks.back()==n)
                {
                    visited[nid].stat[n].visibility=(visited[nid].stat[n].noVisits*1.0/loc)*((noEdges*2.0)/(visited[nid].degree*1.0));
                    // visited[nid].visibility.push_back((visited[nid].noVisits.back()*noEdges*2.0)/(loc*visited[nid].degree*1.0));
                }
            }
        }
    }


    // for (int j=0;j<num_walker;j++)
    // {
    // 	convVec.push_back(0);
    // }
    for (int j=0;j<visited.size() ; j++)
    {
        for (int k=0;k<visited[j].visitedWalks.size() ; k++)
        {
            if (visited[j].stat[visited[j].visitedWalks[k]].visibility>1)
            {
                visited[j].vis++;
                // if (latestTransition<visited[j].lastTransition[k])
                // 	latestTransition=visited[j].lastTransition[k];
                // if (convVec[visited[j].visitedWalks[k]]<visited[j].lastTransition[k])
                // 	convVec[visited[j].visitedWalks[k]]=visited[j].lastTransition[k];
            }
        }
    }
    //quickSort (visited,0,visited.size());
    vector < vector <WalkedNode> > res=classifyVisitedNodes(visited,num_walker);
    return (res);
}

void quickSort(vector <WalkedNode> &arr, int left, int right)
{
    int i = left, j = right;
    WalkedNode tmp(0,0);//=new WalkedNode(0,0);
    int pivot = arr[(left + right) / 2].vis;//itedWalks.size();
    /* partition */
    while (i <= j)
    {
        while (arr[i].vis > pivot)
            i++;
        while (arr[j].vis < pivot)
            j--;
        if (i <= j)
        {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
    /* recursion */
    if (left < j)
        quickSort(arr, left, j);
    if (i < right)
        quickSort(arr, i, right);
}

vector < vector <WalkedNode> > classifyVisitedNodes (vector <WalkedNode> arr,int num_walker) {
    vector < vector <WalkedNode> > res;
    for (int i=0; i<num_walker; i++) {
        res.push_back(vector <WalkedNode>());
    }
    //printf("got here 2\n");
    for (unsigned int i=0; i<arr.size(); i++) {
        if (arr[i].vis>num_walker) {
            cout<<"WHAT??!!!"<<endl;
            getchar();
        }
        if (arr[i].vis>0) {
            res[arr[i].vis-1].push_back(arr[i]);
        }
    }
    return res;
}

