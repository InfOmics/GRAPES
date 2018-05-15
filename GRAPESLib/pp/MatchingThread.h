/*
 * MatchingThread.h
 *
 *  Created on: Dec 2, 2010
 *      Author: bovi
 */
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

#ifndef MATCHINGTHREAD_H_
#define MATCHINGTHREAD_H_

#include "size_t.h"
#include "typedefs.h"

#include "Options.h"

#include <iostream>
#include <stack>
#include <stdio.h>
#include "math.h"

#include "timer.h"

#include "Graph.h"
#include "AttributeComparator.h"
#include "MatchListener.h"
#include "vf2_mono_state.h"
#include "vf2_sub_state.h"
#include "match.h"


namespace GRAPESLib{



class InitMatchingThread{
public:
	thread_id_t id;
	MatchingManager& mman;
	node_id_t query_size;
	AttributeComparator& edgeComparator;

	thread_id_t nthreads;

	InitMatchingThread(
				thread_id_t _id,
				MatchingManager& _mman,
				node_id_t _query_size,
				AttributeComparator& _edgeComparator,
				thread_id_t _nthreads)
		: id(_id),
		  mman(_mman),
		  query_size(_query_size),
		  edgeComparator(_edgeComparator),
		  nthreads(_nthreads){
	}

	void fillJob(InitMatchingJob* job){
		node_id_t* masked_in_degree =  (node_id_t*)calloc(job->graph.n, sizeof(node_id_t));
		node_id_t* masked_out_degree = (node_id_t*)calloc(job->graph.n, sizeof(node_id_t));

//		u_lsize_t *dens = (u_lsize_t)calloc(job->graph.nof_nodes, sizeof(u_lsize_t));
//		bool densdone = false;

		//** global mask **//
		sbitset target_mask;

		for(node_cands_t::iterator nIT = mman.gncands[job->g_id].begin();  nIT!=mman.gncands[job->g_id].end(); nIT++){
			target_mask |= nIT->second;
		}

		//** compute degrees of induced subgraph **//
		node_id_t n, ne;
		for(node_id_t n=0; n<job->graph.n; n++){
			if(target_mask.get(n)){
				for(node_id_t i=0; i<job->graph.out_count[n]; i++){
					ne = job->graph.out[n][i];
					if(target_mask.get(ne)){
						masked_out_degree[n]++;
					}
				}
				for(node_id_t i=0; i<job->graph.in_count[n]; i++){
					ne = job->graph.in[n][i];
					if(target_mask.get(ne)){
						masked_in_degree[n]++;
					}
				}
			}
		}

		//** fix node candidates **//
		for(node_id_t q=0; q< mman.query.n; q++){
			for(sbitset::iterator IT =  mman.gncands[job->g_id][q].first_ones(); IT!= mman.gncands[job->g_id][q].end(); IT.next_ones()){
				if(		masked_in_degree[IT.first] < mman.query.in_count[q]
						||	masked_out_degree[IT.first] < mman.query.out_count[q]){
					if(target_mask.get(IT.first)){
						 mman.gncands[job->g_id][q].set(IT.first, false);
					}
				}
			}
			if(  mman.gncands[job->g_id][q].is_empty() )
				return;
		}

		node_id_t ra, qb, rb;
		bool notfound;
		bool nextlevel = true;

		//** static domains reduction **//
		for(node_id_t qa=0; qa<mman.query.n; qa++){

			for(sbitset::iterator qaIT= mman.gncands[job->g_id][qa].first_ones(); qaIT!= mman.gncands[job->g_id][qa].end(); qaIT.next_ones()){

				if(		masked_in_degree[qaIT.first] < mman.query.in_count[qa]
					||	masked_out_degree[qaIT.first] < mman.query.out_count[qa]){
						mman.gncands[job->g_id][qa].set(qaIT.first, false);
				}
				else{
					ra = qaIT.first;
					for(node_id_t i_qb=0; i_qb<mman.query.out_count[qa]; i_qb++){
						qb = mman.query.out[qa][i_qb];
						notfound = true;

						for(node_id_t i_rb=0; i_rb<job->graph.out_count[ra]; i_rb++){
							rb = job->graph.out[ra][i_rb];
							if( 	mman.gncands[job->g_id][qb].get(rb)
									//TODO
//									&&
//									edgeComparator.compare(mman.query.adj_attrs[qa][i_qb], job->graph.out_adj_attrs[ra][i_rb])
							){
								notfound = false;
								break;
							}
						}

						if(notfound){
							nextlevel = true;
							mman.gncands[job->g_id][qa].set(ra, false);
							break;
						}
					}
				}
			}

			if(mman.gncands[job->g_id][qa].is_empty())
				return;
		}


		//** static domain reduction until convergence **//
		while(nextlevel){

			nextlevel = false;

			for(int qa=0; qa<mman.query.n; qa++){

				for(sbitset::iterator qaIT= mman.gncands[job->g_id][qa].first_ones(); qaIT!= mman.gncands[job->g_id][qa].end(); qaIT.next_ones()){
					ra = qaIT.first;
					for(node_id_t i_qb=0; i_qb<mman.query.out_count[qa]; i_qb++){
						qb = mman.query.out[qa][i_qb];
						notfound = true;

						for(node_id_t i_rb=0; i_rb<job->graph.out_count[ra]; i_rb++){
							rb = job->graph.out[ra][i_rb];
							if( 	mman.gncands[job->g_id][qb].get(rb)
									//TODO
//									&&
//									edgeComparator.compare(mman.query.adj_attrs[qa][i_qb], job->graph.out_adj_attrs[ra][i_rb])
									){
								notfound = false;
								break;
							}
						}

						if(notfound){
							nextlevel = true;
							mman.gncands[job->g_id][qa].set(ra, false);
							break;
						}
					}
				}

				if(mman.gncands[job->g_id][qa].is_empty())
					return;
			}

		}

		//** calculate global mask **//
		target_mask.clear(job->graph.n);
		for(node_cands_t::iterator nIT = mman.gncands[job->g_id].begin();  nIT!=mman.gncands[job->g_id].end(); nIT++){
			target_mask |= nIT->second;
		}


		//** retrieve connected components and add job to queue**//
		bool *visited = (bool*)calloc(job->graph.n, sizeof(bool));
		u_size_t cardinality;
		node_id_t gn=0;
		while(gn<job->graph.n){
			if(!visited[gn] && target_mask.get(gn)){
				sbitset* coco = new sbitset();

				visited[gn] = true;
				coco->set(gn, true);

				std::stack<node_id_t> bfsqueue;
				bfsqueue.push(gn);

				while(!bfsqueue.empty()){

					n = bfsqueue.top();
					bfsqueue.pop();

					for(node_id_t i=0; i<job->graph.out_count[n]; i++){
						ne = job->graph.out[n][i];
						if(!visited[ne] && target_mask.get(ne)){
								bfsqueue.push(ne);
								visited[ne] = true;
								coco->set(ne, true);
//							}
						}
					}
					for(node_id_t i=0; i<job->graph.in_count[n]; i++){
						ne = job->graph.in[n][i];
						if(!visited[ne] && target_mask.get(ne)){
								bfsqueue.push(ne);
								visited[ne] = true;
								coco->set(ne, true);
//							}
						}
					}
				}

				cardinality = coco->cardinality();

				//** decompose connected components into sub-tasks **//
				if(cardinality >= query_size){

					mman.number_of_cocos[id] += 1;

					//TODO
					//push job (connected component)
					//job->cocos.push_front(match_job_t(*coco));

					//search the max dom node d inside the coco
					//get the diameter D of d
					//consider only targets of dom(d) inside the coco
					//for each target
					//  make a partition with just one core node
					//  extend the frontier of the partition by a bfs of length D


					u_size_t max_card_value = 0;
					node_id_t max_card_node = -1;

					u_size_t min_card_value = 0;
					node_id_t min_card_node = -1;

					u_size_t c_card;
					for(node_id_t qa=0; qa<mman.query.NodeCount(); qa++){

						c_card = 0;
						for(sbitset::iterator qaIT= mman.gncands[job->g_id][qa].first_ones(); qaIT!= mman.gncands[job->g_id][qa].end(); qaIT.next_ones()){
							ra = qaIT.first;
							if(coco->get(ra)){
								c_card++;
							}
						}

						if(c_card <= nthreads){
							if(max_card_value == 0 ||
								c_card > max_card_value ||
								( c_card == max_card_value && mman.query.EdgeCount(qa) > mman.query.EdgeCount(max_card_node) )
							){
								max_card_value = c_card;
								max_card_node =qa;
							}
						}
						else{
							if(min_card_value == 0 ||
								c_card < min_card_value ||
								( c_card == min_card_value && mman.query.EdgeCount(qa) > mman.query.EdgeCount(min_card_node) )
							){
								min_card_value = c_card;
								min_card_node =qa;
							}
						}

					}

					if(max_card_value == 1 || max_card_value == 0){
						if(min_card_node != -1){
							max_card_value = min_card_value;
							max_card_node = min_card_node;
						}
					}
					//consider only targets of dom(d) inside the coco
					//for each target
					//  make a partition with just one core node
					for(sbitset::iterator qaIT= mman.gncands[job->g_id][max_card_node].first_ones()
							; qaIT!= mman.gncands[job->g_id][max_card_node].end(); qaIT.next_ones()){
						ra = qaIT.first;
						if(coco->get(ra)){
							job->cocos.push_front(match_job_t(ra, max_card_node));
						}
					}

				}
				delete coco;
			}
			gn++;
		}

		free(visited);
		free(masked_in_degree);
		free(masked_out_degree);
//		free(dens);
	}


	static void* run(void* argsptr){
		InitMatchingThread* mt = (InitMatchingThread*)argsptr;
		InitMatchingJob* job;
		while(mt->mman.syncGetAInitJob(mt->id, &job)){
			mt->fillJob((InitMatchingJob*)job);
			mt->mman.syncFinishInitJob(mt->id, job);
		}
		pthread_exit(NULL);
	}
};




bool my_visitor(int n, node_id ni1[], node_id ni2[], void* usr_data){
	((MatchListener*)usr_data)->match(n, ni1,ni2);
	return false;
}

class MatchingThread{

public:
	MatchingManager& mman;
	thread_id_t id;
	MatchListener& mlistener;
	ARGEdit& equery;
	bool first_match;
	AttributeComparator& edgeComparator;//TODO NOT USED

	MPRINT_OPTIONS mprint_opt;

	MatchingThread(	MatchingManager& _mman,
					thread_id_t _id,
					std::ostream& outstream,
					ARGEdit& _query,
					MatchListener& _mlistener,
					AttributeComparator& _edgeComparator,
					MPRINT_OPTIONS _mprint_opt)
			: mman(_mman),
			  id(_id),
			  equery(_query),
			  mlistener(_mlistener),
			  edgeComparator(_edgeComparator),
			  mprint_opt(_mprint_opt){
		first_match = true;
	}

	static void* run(void* argsptr){
			MatchingThread* mt = (MatchingThread*)argsptr;

			MatchingJob* job;

			QueryGraph query(&(mt->equery));
			VF2DSAttrComparator* attrComp = new VF2DSAttrComparator();
			query.SetNodeComparator(attrComp);

			sbitset* domains = new sbitset[query.NodeCount()];

//			graph_id_t p_gid = graph_id_t_MAX;
//			node_id_t p_pnodeid = node_id_t_MAX;


			while(mt->mman.syncGetAMatchJob(mt->id, &job)){

//				if(p_gid == graph_id_t_MAX ||  (job->coco.first != p_gid) || (job->coco.second.second != p_pnodeid)){
//
//					if(p_gid != graph_id_t_MAX){
//						delete[] domains;
//						domains = new sbitset[query.NodeCount()];
//					}
//
//
//
//					for(int i=0; i<query.NodeCount(); i++){
//						domains[i].warp(mt->mman.gncands[job->coco.first][i]);
//					}
//				}

				for(int i=0; i<query.NodeCount(); i++){
					domains[i].warp(mt->mman.gncands[job->coco.first][i]);
				}

				attrComp->domains = domains;
				//attrComp->coco_mask = &(job->coco.second);
//
//				p_gid = job->coco.first;
//				p_pnodeid = job->coco.second.second;
//
				mt->mlistener.matchcount = 0;
				mt->mlistener.gid = job->coco.first;
//
//
//				std::cout<<"("<<job->coco.second.second<<","<<job->coco.second.first<<"):"<<domains[job->coco.second.second].get(job->coco.second.first)<<"\n";
//				//query node: job->coco.second.second
//				//target node: job->coco.second.first
//
				State* s0 = new VF2MonoState(&(query), (mt->mman.graphs[job->coco.first]));
				matchFrom(s0, my_visitor, &(mt->mlistener),   job->coco.second.second, job->coco.second.first );
				delete s0;
//
//
				mt->mman.number_of_matches[mt->id] += mt->mlistener.matchcount;
				if(mt->mlistener.matchcount > 0)
					mt->mman.matching_graphs[mt->id].insert(job->coco.first);
			}

			delete[] domains;



//			sbitset* domains = new sbitset[query.n];
//
//			while(mt->mman.syncGetAMatchJob(mt->id, &job)){
//
////				delete[] domains;
////				domains = new sbitset[query.n];
//				for(int i=0; i<query.n; i++){
//					domains[i].warp(mt->mman.gncands[job->coco.first][i]);
//				}
//
//				attrComp->domains = domains;
//				attrComp->coco_mask = &(job->coco.second);
//
//
//				mt->mlistener.matchcount = 0;
//				mt->mlistener.gid = job->coco.first;
//
//
//				State* s0 = new VF2MonoState(&(query), (mt->mman.graphs[job->coco.first]));
//				match(s0, my_visitor, &(mt->mlistener) );
//
//
//				mt->mman.number_of_matches[mt->id] += mt->mlistener.matchcount;
//					if(mt->mlistener.matchcount > 0)
//						mt->mman.matching_graphs[mt->id].insert(job->coco.first);
//
//
//			}
//
//			delete[] domains;


			pthread_exit(NULL);
	}
};

}

#endif /* MATCHINGTHREAD_H_ */
