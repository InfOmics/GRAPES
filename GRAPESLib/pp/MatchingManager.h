/*
 * MatchingManager.h
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

#ifndef MATCHINGMANAGER_H_
#define MATCHINGMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <set>
#include <list>
#include <map>

#include "size_t.h"
#include "typedefs.h"

#include "AttributeComparator.h"

namespace GRAPESLib{

class InitMatchingJob{
public:
	graph_id_t g_id;
	ReferenceGraph& graph;//IN
	match_jobs_t cocos;//OUT
	InitMatchingJob(graph_id_t _g_id, ReferenceGraph& _graph)
			: g_id(_g_id), graph(_graph){
	}
};

class g_match_task_t{
public:
	graph_id_t first;
	match_job_t second;
	g_match_task_t(){
		first = -1;
	}
	g_match_task_t(graph_id_t gid, match_job_t& coco)
		: first(gid), second(coco){
	}
};

class MatchingJob{
public:
	g_match_task_t& coco;

	MatchingJob(g_match_task_t& _coco)
			: coco(_coco){
	}
};

class MatchingManager{
public:
	pthread_mutex_t getajob_sync;
	pthread_mutex_t finishjob_sync;

	QueryGraph& query;

	graph_node_cands_t& gncands;

	filtering_graph_set_t& fgset;
	std::map<graph_id_t, ReferenceGraph*>& graphs;
	filtering_graph_set_t::iterator graphs_IT;

	std::list<g_match_task_t> coco_units;
	std::list<g_match_task_t>::iterator coco_units_mIT;

	AttributeComparator& edgeComparator;

	u_size_t* number_of_matches;
	std::set<graph_id_t>* matching_graphs;
	u_size_t* number_of_cocos;


	MatchingManager(
					QueryGraph& _query,
					std::map<graph_id_t, ReferenceGraph*>& _graphs,
					filtering_graph_set_t& _fgset,
					graph_node_cands_t& _gncands,
					AttributeComparator& _edgeComparator,
					thread_id_t NTHREADS)
			: query(_query), graphs(_graphs), fgset(_fgset), gncands(_gncands), edgeComparator(_edgeComparator){

		getajob_sync = PTHREAD_MUTEX_INITIALIZER;
		finishjob_sync = PTHREAD_MUTEX_INITIALIZER;

		number_of_matches = (u_size_t*)calloc(NTHREADS, sizeof(u_size_t));
		matching_graphs = new std::set<graph_id_t>[NTHREADS];
		number_of_cocos = (u_size_t*)calloc(NTHREADS, sizeof(u_size_t));

		graphs_IT = fgset.begin();
	}

	~MatchingManager(){
		free(number_of_matches);
		delete [] matching_graphs;
		free(number_of_cocos);
	}

	bool getAInitJob(thread_id_t thread, InitMatchingJob** mjob){
		if(graphs_IT !=  fgset.end()){
			*mjob = new InitMatchingJob(*graphs_IT, *(graphs[*graphs_IT]));
			graphs_IT++;
			return true;
		}
		return false;
	}
	void finishInitJob(thread_id_t thread, InitMatchingJob* job){
		for(match_jobs_t::iterator IT = job->cocos.begin(); IT!=job->cocos.end(); IT++){
			coco_units.push_back(g_match_task_t(job->g_id, *IT));
		}
	}
	bool syncGetAInitJob(thread_id_t thread, InitMatchingJob** mjob){
		pthread_mutex_lock(&getajob_sync);
		bool get = getAInitJob(thread, mjob);
		pthread_mutex_unlock(&getajob_sync);
		return get;
	}
	void syncFinishInitJob(thread_id_t thread, InitMatchingJob* mjob){
		pthread_mutex_lock(&finishjob_sync);
		finishInitJob(thread, mjob);
		pthread_mutex_unlock(&finishjob_sync);
	}






	void createBalancedRun(){
		coco_units_mIT =  coco_units.begin();
	}




	bool getAMatchJob(int thread, MatchingJob** mjob){
		if(coco_units_mIT !=  coco_units.end()){
			*mjob = new MatchingJob(*coco_units_mIT);
			coco_units_mIT++;
			return true;
		}
		return false;
	}

	bool syncGetAMatchJob(int thread, MatchingJob** mjob){
		pthread_mutex_lock(&getajob_sync);
		bool get = getAMatchJob(thread, mjob);
		pthread_mutex_unlock(&getajob_sync);
		return get;
	}
};

}

#endif /* MATCHINGMANAGER_H_ */
