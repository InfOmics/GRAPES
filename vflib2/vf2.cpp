#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>



#include "textdb_driver_vf.h"
#include "VFlibrary.h"
#include "match_vf.h"
#include "timer.h"


int main(int argc, char* argv[]){

    double match_t=0.0, total_t;
    TIMEHANDLE start=start_time();
    TIMEHANDLE start_p=start_time();

    std::cout<<"query file:"<<argv[1]<<"\n";
    std::cout<<"reference file:"<<argv[2]<<"\n";

    std::ifstream ifs (argv[1], std::ifstream::in);
    MyARGraph* query = read_text_graph_vf(ifs, GraphFileType::GFT_GFF);
    std::cout<<query->NodeCount()<<"\n";
    ifs.close();
    query->SetNodeComparator(new VStringComparator());


    int matchinggraphs = 0;
    int pmatches = 0;
    int gid = 0;
    std::ifstream ifsdb (argv[2], std::ifstream::in);
    while(true){
        MyARGraph* reference = read_text_graph_vf(ifsdb, GraphFileType::GFT_GFF);
        if(reference != NULL){
            //std::cout<<reference->NodeCount()<<"\n";
            start_p=start_time();
            matchvf(query,reference, gid, std::cout, MatchType::MT_MONO);
            match_t += end_time(start_p);
            if(matchcount != pmatches){
                matchinggraphs++;
            }
            pmatches =matchcount;
            //std::cout<<matchcount<<"\n";
        }else{
            break;
        }
        gid++;
    }
    ifsdb.close();

    total_t =  end_time(start);

    std::cout<<"matching graphs:"<<matchinggraphs<<"\n";
    std::cout<<"total matches:"<<matchcount<<"\n";
    std::cout<<"match time:"<<match_t<<"\n";
    std::cout<<"total time:"<<total_t<<"\n";
};