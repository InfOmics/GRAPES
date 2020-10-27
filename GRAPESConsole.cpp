/*
Copyright (c) 2013 by Rosalba Giugno

GRAPES is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



#include "Options.h"

#include "Graph.h"
#include "LabelMap.h"
#include "sbitset.h"
#include "OCPTreeNode.h"
#include "OCPTreeListeners.h"
#include "OCPTree.h"
#include "GraphPathListener.h"
#include "GraphVisit.h"
#include "GRAPESIndex.h"
#include "GraphReaders.h"
#include "BuildManager.h"
#include "BuilderThread.h"
#include "BuildRunner.h"
#include "MatchingManager.h"
#include "MatchingThread.h"
#include "MatchingRunner.h"

#include "AttributeComparator.h"
#include "VF2GraphReaders.h"
#include "argedit.h"


#include "Graph.h"
#include "MatchListener.h"
#include "vf2_mono_state.h"
#include "vf2_sub_state.h"
#include "match.h"


#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>




using namespace GRAPESLib;

u_size_t MAXDEPTH = 4;
thread_id_t NTHREADS = 1;
VERBOSE_TYPE verbose_type = VERBOSE_TYPE_FULL;
MPRINT_OPTIONS mprint_opt = MPRINT_OPT_NO;


using namespace GRAPESLib;

void usage(char* argv[]);
void print_mark();

void graph_build(const std::string& input_network_file, bool direct);
void graph_find(const std::string& input_network_file, const std::string& query_graph_file, bool direct);
void graph_find_vf2(const std::string& input_network_file, const std::string& query_graph_file, bool direct);


int main(int argc, char* argv[]){
	if(argc<5){
		std::cout<<"\nToo few parameters.\n"<<std::endl;
		usage(argv);
	}


	if(atoi(argv[1]) <= 0) usage(argv);
	NTHREADS = (thread_id_t)atoi(argv[1]);


	bool direct;
	std::string cmd = argv[3];
	if(cmd == "-gfu"){
		direct = false;
	}
	else if(cmd == "-gfd"){
		direct = true;
	}
	else{
		usage(argv);
	}


	cmd = argv[2];
	if(cmd=="-b" && (argc==5 || argc==7)){
		if(argc==7){
			cmd = argv[5];
			if(cmd != "-lp"){
				usage(argv);
			}
			if(atoi(argv[6]) <= 0) usage(argv);
			MAXDEPTH = atoi(argv[6]);
		}

		graph_build(argv[4], direct);
	}

	else if(cmd=="-f" && (argc==7 || argc==9)){
		cmd = argv[6];
		if(cmd == "-no"){
		}
		else if(cmd == "-console"){
			mprint_opt = MPRINT_OPT_CONSOLE;
		}
		else if(cmd == "-file"){
			mprint_opt = MPRINT_OPT_FILE;
		}
		else{
			usage(argv);
		}

		if(argc==9){
			cmd = argv[7];
			if(cmd != "-lp"){
				usage(argv);
			}
			if(atoi(argv[8]) <= 0) usage(argv);
			MAXDEPTH = (u_size_t)atoi(argv[8]);
		}

		graph_find(argv[4], argv[5], direct);
	}


	else if(cmd=="-v" && (argc==7 || argc==9)){
		cmd = argv[6];
		if(cmd == "-no"){
		}
		else if(cmd == "-console"){
			mprint_opt = MPRINT_OPT_CONSOLE;
		}
		else if(cmd == "-file"){
			mprint_opt = MPRINT_OPT_FILE;
		}
		else{
			usage(argv);
		}

		if(argc==9){
			cmd = argv[7];
			if(cmd != "-lp"){
				usage(argv);
			}
			if(atoi(argv[8]) <= 0) usage(argv);
			MAXDEPTH = (u_size_t)atoi(argv[8]);
		}

		graph_find_vf2(argv[4], argv[5], direct);
	}


	else{
		usage(argv);
	}

	exit(0);


};
void usage(char* argv[]){
		print_mark();
		std::cout<<"\nUsage:"<<std::endl;
		std::cout<<"\tBuild database index \t: "<<argv[0]<<" nthreads -b -[gfu|gfd] db_file [-lp lp]"<<std::endl;
		std::cout<<"\tQuery on database \t: "<<argv[0]<<" nthreads -f -[gfu|gfd] db_file query_file -[no|console|file] [-lp lp]"<<std::endl;
		std::cout<<"\n"<<std::endl;
		exit(1);
};
void print_mark(){
	std::cout<<"\n\t========\n";
	std::cout<<"\t GRAPES version 2.9.mp.vf2\n";
	std::cout<<"\t========\n\n";
};


















void graph_build(const std::string& input_network_file, bool direct){
print_mark();
std::cout<<"\t========BUILD========\n\n";
std::cout<<"Number of threads: "<<NTHREADS<<"\n";
std::cout<<"Max LP depth: "<<MAXDEPTH<<"\n";
std::cout<<"Input database file: "<<input_network_file<<"\n";




double read_t, build_t, save_t, total_t;
TIMEHANDLE start=start_time();
TIMEHANDLE start_p=start_time();

	//** read database **//
	std::ifstream is;
	is.open(input_network_file.c_str(), std::ios::in);
	LabelMap labelMap;

	GraphReader_gff temp_greader(labelMap,is);
	temp_greader.direct = direct;//				std::cout<<mt->id<<" - "<<job->coco.first<<"\n";

	std::queue<GRAPESLib::Graph> temp_queue;
	bool temp_read = true;
	int temp_i = 0;
	while(temp_read){
		GRAPESLib::Graph* g = new GRAPESLib::Graph(temp_i);
		temp_read = temp_greader.readGraph(*g);
		if(temp_read){
			temp_queue.push(*g);
			temp_i++;
		}
	}
	GraphsQueueReader greader(labelMap, temp_queue);


read_t = end_time(start_p);
start_p=start_time();


	//** build index **//
	OCPTree index;
	DefaultBuildManager bman(index, greader, MAXDEPTH, NTHREADS);
	AllPathListener plistener;
	DFSGraphVisitor gvisitor(plistener);
	BuildRunner buildrun(bman, gvisitor, NTHREADS);
	buildrun.run();


build_t = end_time(start_p);
start_p=start_time();

	//** save index on file **//
	GRAPESIndex grapes_index(labelMap, index);
//	std::ofstream os;
//	os.open((input_network_file+".index.grapes").c_str(), std::ios::out);
//	os<<grapes_index;
//	os.flush();
//	os.close();
	FILE *fd;
	fd = fopen((input_network_file+".index.grapes").c_str(), "wb");
	grapes_index.write(fd);
	fflush(fd);
	fclose(fd);


save_t = end_time(start_p);
total_t = end_time(start);


std::cout<<"\n";
std::cout<<"Number of graphs inside the database: "<<temp_i<<"\n";
std::cout<<"\n";
std::cout<<"Time for read database from file: \t"<<read_t<<"\n";
std::cout<<"Time for build database index: \t"<<build_t<<"\n";
std::cout<<"Time for save index on file: \t"<<save_t<<"\n";
std::cout<<"Total time: \t"<<total_t<<"\n";
std::cout<<"\n\n";

};



void print(VF2Graph* g){
	std::cout<<g->n<<"\n";
	for(int i=0; i<g->n; i++){
		std::cout<<"("<<i<<") o["<<g->out_count[i]<<"]{";
		for(int j=0; j<g->out_count[i];j++)
			std::cout<<g->out[i][j]<<",";
		std::cout<<"}\n";
		std::cout<<"\t i["<<g->in_count[i]<<"]{";
		for(int j=0; j<g->in_count[i];j++)
			std::cout<<g->in[i][j]<<",";
		std::cout<<"}\n";
	}
};



void graph_find(const std::string& input_network_file, const std::string& query_graph_file, bool direct){
print_mark();
std::cout<<"\t========QUERY========\n\n";
std::cout<<"Number of threads: "<<NTHREADS<<"\n";
std::cout<<"Max LP depth: "<<MAXDEPTH<<"\n";
std::cout<<"Input database file: "<<input_network_file<<"\n";
std::cout<<"Query file: "<<query_graph_file<<"\n";
std::cout<<"\n";

double sinit_t, qbuild_t, filtering_t, dbload_t, balancing_t, match_t, total_t;
TIMEHANDLE start=start_time();
TIMEHANDLE start_p=start_time();

//	std::ifstream is;
//	is.open((input_network_file+".index.grapes").c_str(), std::ios::in);
//	LabelMap labelMap; OCPTree index_tree;
//	GRAPESIndex index(labelMap, index_tree);
//	is>>index;
//	is.close();

	FILE* fd;
	fd = fopen((input_network_file+".index.grapes").c_str(), "rb");
	LabelMap labelMap; OCPTree index_tree;
	GRAPESIndex index(labelMap, index_tree);
	index.read(fd);
	fclose(fd);




sinit_t = end_time(start_p);
start_p=start_time();

	std::ifstream is;
	is.open(query_graph_file.c_str(), std::ios::in);
	OCPTree query;

	GraphReader_gff greader(index._labelMap,is);
	greader.direct = direct;

	DefaultBuildManager bman(query, greader, MAXDEPTH, NTHREADS);
	OnePathListener plistener;
	DFSGraphVisitor gvisitor(plistener);
	BuildRunner buildrun(bman, gvisitor, 1);
	buildrun.run();
	is.close();

qbuild_t = end_time(start_p);
start_p=start_time();

	filtering_graph_set_t fgset;
	graph_node_cands_t gncands;
	query.match(index._ocpTree, *(new DefaultOCPTMatchingListener(fgset, gncands)));

filtering_t = end_time(start_p);
start_p=start_time();

	VF2GraphReader_gfu  s_q_reader(labelMap);
	s_q_reader.direct = direct;
	s_q_reader.open(query_graph_file);
	ARGEdit* squery;
	if((squery = s_q_reader.readGraph()) == NULL){
		std::cout<<"Cannot open query file for matching\n";
		exit(1);
	}
	s_q_reader.close();


	std::map<graph_id_t, ReferenceGraph*> rgraphs;
	VF2GraphReader_gfu  s_r_reader(labelMap);
	s_r_reader.direct = direct;
	s_r_reader.open(input_network_file);
	graph_id_t srid = 0;
	bool sreaded = true;
	do{
		//ARGEdit* rgraph = s_r_reader.readGraph();
		VF2Graph* rgraph = s_r_reader.readSGraph();
		sreaded = (rgraph != NULL);
		if(sreaded){
			//rgraphs.insert(std::pair<graph_id_t, ReferenceGraph* >(srid, new ReferenceGraph(rgraph)));
			rgraphs.insert(std::pair<graph_id_t, ReferenceGraph* >(srid, rgraph));

//			print((rgraphs.find(srid))->second);

			srid++;
		}
	}while(sreaded);

dbload_t = end_time(start_p);
start_p=start_time();

	QueryGraph aqg(squery);
	MatchingManager mman(aqg, rgraphs, fgset, gncands, *(new DefaultAttrComparator()), NTHREADS);
	MatchRunner mrunner(mman, NTHREADS, squery->NodeCount(), mprint_opt);
	mrunner.runInitPhase();
	std::map<int, std::list<g_match_task_t> > assign;

	mman.createBalancedRun();

balancing_t = end_time(start_p);
start_p = start_time();

	mrunner.runMatch(rgraphs, *squery);

match_t = end_time(start_p);
total_t = end_time(start);


std::set<graph_id_t> candidates;
for(std::list<g_match_task_t>::iterator IT = mman.coco_units.begin(); IT!=mman.coco_units.end(); IT++)
	candidates.insert((*IT).first);

int nof_cocos = 0;
for(thread_id_t i=0; i<NTHREADS; i++){
	nof_cocos += mman.number_of_cocos[i];
}

long nof_matches = 0;
for(thread_id_t i=0; i<NTHREADS; i++)
	nof_matches += mman.number_of_matches[i];

std::set<graph_id_t> matching_graphs;
for(thread_id_t i=0; i<NTHREADS; i++)
	matching_graphs.insert(mman.matching_graphs[i].begin(), mman.matching_graphs[i].end());


std::cout<<"\n";
std::cout<<"Number of candidate graphs: "<<candidates.size()<<"\n";
std::cout<<"Number of connected components by filtering: "<< nof_cocos <<"\n";
//std::cout<<"Number of matching jobs: "<< mman.coco_units.size()<<"\n";
std::cout<<"Number of matching graphs: "<<matching_graphs.size()<<"\n";
std::cout<<"Number of found matches: "<<nof_matches<<"\n";
std::cout<<"\n";
std::cout<<"DB's index load time: \t"<<sinit_t<<"\n";
std::cout<<"Query indexing time: \t"<<qbuild_t<<"\n";
std::cout<<"Filtering time: \t"<<filtering_t<<"\n";
std::cout<<"DB load time: \t"<<dbload_t<<"\n";
std::cout<<"DB's decomposing time: \t"<<balancing_t<<"\n";
std::cout<<"Matching time: \t"<<match_t<<"\n";
std::cout<<"Total time: \t"<<total_t<<"\n";
std::cout<<"\n\n";


//std::cout<<"candidate graphs:\n";
//for(std::set<graph_id_t>::iterator IT = candidates.begin(); IT!= candidates.end(); IT++)
//	std::cout<<(*IT)<<"\t";
//std::cout<<"\n";
//
//std::cout<<"node candidates:\n";
//for(graph_node_cands_t::iterator gIT = mman.gncands.begin(); gIT!=mman.gncands.end(); gIT++){
//	std::cout<<"g id: "<<gIT->first<<"\n";
//
//	for(node_cands_t::iterator qIT = gIT->second.begin(); qIT!=gIT->second.end(); qIT++){
//		std::cout<<"qn["<<qIT->first<<"]\t["<<qIT->second.cardinality()<<"]{";
//		for(sbitset::iterator sIT = qIT->second.first_ones(); sIT != qIT->second.end(); sIT.next_ones()){
//			std::cout<<sIT.first<<",";
//		}
//		std::cout<<"}\n";
//	}
//}


//for(u_size_t i=0; i< rgraphs.size(); i++){
//	std::cout<<"g["<<i<<"]\n";
//	for(u_size_t j=0; j<rgraphs.at(i)->nof_nodes; j++){
//		if(mman.nodemm[i][j] != 0){
//			std::cout<<j<<"\t"<<mman.nodemm[i][j]<<"\n";
//		}
//	}
//	std::cout<<"\n";
//}


};


//bool my_visitor(int n, node_id ni1[], node_id ni2[], void* usr_data){
//	((MatchListener*)usr_data)->match(n, ni1,ni2);
//	return false;
//};


void graph_find_vf2(const std::string& input_network_file, const std::string& query_graph_file, bool direct){
print_mark();
std::cout<<"\t========QUERY========\n\n";
std::cout<<"Input database file: "<<input_network_file<<"\n";
std::cout<<"Query file: "<<query_graph_file<<"\n";
std::cout<<"\n";

double match_t=0.0, total_t;
TIMEHANDLE start=start_time();
TIMEHANDLE start_p=start_time();

	LabelMap labelMap;

	VF2GraphReader_gfu  s_q_reader(labelMap);
	s_q_reader.direct = direct;
	s_q_reader.open(query_graph_file);
	ARGEdit* squery;
	if((squery = s_q_reader.readGraph()) == NULL){
		std::cout<<"Cannot open query file for matching\n";
		exit(1);
	}
	s_q_reader.close();
	QueryGraph aqg(squery);
	VF2DSAttrComparator* attrComp = new VF2DSAttrComparator();
	aqg.SetNodeComparator(attrComp);

	MatchListener* mlistener;
	std::ofstream outs;
	if(mprint_opt == MPRINT_OPT_NO)
		mlistener = new EmptyMatchListener();
	else if(mprint_opt == MPRINT_OPT_CONSOLE)
		mlistener = new ConsoleMatchListener();
	else{
		outs.open("matches.log",std::ios::out);
		mlistener = new FileMatchListener(outs);
	}

	std::map<graph_id_t, ReferenceGraph*> rgraphs;
	VF2GraphReader_gfu  s_r_reader(labelMap);
	s_r_reader.direct = direct;
	s_r_reader.open(input_network_file);
	graph_id_t srid = 0;
	bool sreaded = true;
	do{
		VF2Graph* rgraph = s_r_reader.readSGraph();
		sreaded = (rgraph != NULL);
		if(sreaded){

			start_p=start_time();

			State* s0 = new VF2MonoState(&(aqg), rgraph);
			match(s0, my_visitor, mlistener);
			delete s0;

			match_t += end_time(start_p);

			srid++;
		}
	}while(sreaded);

total_t = end_time(start);


if(mprint_opt == MPRINT_OPT_NO){}
else if(mprint_opt == MPRINT_OPT_CONSOLE){}
else{
	outs.flush();
	outs.close();
}

//std::set<graph_id_t> candidates;
//for(std::list<g_match_task_t>::iterator IT = mman.coco_units.begin(); IT!=mman.coco_units.end(); IT++)
//	candidates.insert((*IT).first);

//int nof_cocos = 0;
//for(thread_id_t i=0; i<NTHREADS; i++){
//	nof_cocos += mman.number_of_cocos[i];
//}

long nof_matches = mlistener->matchcount;

std::cout<<"\n";
std::cout<<"Number of found matches: "<<nof_matches<<"\n";
std::cout<<"\n";
std::cout<<"Matching time: \t"<<match_t<<"\n";
std::cout<<"Total time: \t"<<total_t<<"\n";
std::cout<<"\n\n";


};
