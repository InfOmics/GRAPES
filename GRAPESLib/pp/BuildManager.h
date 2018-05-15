/*
 * BuildingEnv.h
 *
 *  Created on: Nov 9, 2010
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

#ifndef BUILDMANAGER_H_
#define BUILDMANAGER_H_

#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <pthread.h>
#include <iostream>
#include <fstream>

#include <math.h>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "OCPTree.h"
#include "OCPTreeListeners.h"
#include "GraphReaders.h"
#include "timer.h"

namespace GRAPESLib{

class BuildGraphSupp{
public:
	label_nodes_map_t label_nodes_map;
	buildjob_nodesunit_t build_units;

	BuildGraphSupp(Graph& g){
		for(node_id_t i=0; i<g.nodes_count; i++)
			label_nodes_map[g.nodes[i].label].insert(g.nodes[i].id);
	}
};

class BuildJob{
public:
	OCPTree& _tree;
	Graph& _graph;
	BuildGraphSupp& _bgsupp;
	node_id_set_t& _n_ids;
	bool destroy_graph_onreturn;

	BuildJob(	OCPTree& tree,
				Graph& graph,
				BuildGraphSupp& bgsupp,
				node_id_set_t& n_ids,
				bool destroy)
			:	_tree(tree), _graph(graph), _bgsupp(bgsupp), _n_ids(n_ids){
		destroy_graph_onreturn = destroy;
	}
};

class BuildManager{
public:
	OCPTree& _globalIndex;
	GraphReader& _greader;
	u_size_t _lp;
	thread_id_t _nthreads;
	pthread_mutex_t _getajob_sync;
	pthread_mutex_t _finishjob_sync;


	BuildManager(	OCPTree& index,
					GraphReader& greader,
					u_size_t lp,
					thread_id_t nthreads)

			: _globalIndex(index),
			  _greader(greader){
		_lp=lp;
		_nthreads = nthreads;
		_getajob_sync = PTHREAD_MUTEX_INITIALIZER;
		_finishjob_sync = PTHREAD_MUTEX_INITIALIZER;

	}

	bool syncGetAJob(thread_id_t thread, BuildJob** buildJob){
		pthread_mutex_lock(&_getajob_sync);
		bool get = getAJob(thread, buildJob);
		pthread_mutex_unlock(&_getajob_sync);
		return get;
	}
	void syncFinishJob(thread_id_t thread, BuildJob* buildJob){
		pthread_mutex_lock(&_finishjob_sync);
		finishJob(thread, buildJob);
		pthread_mutex_unlock(&_finishjob_sync);
	}

	virtual bool getAJob(thread_id_t thread, BuildJob** buildJob) = 0;
	virtual void finishJob(thread_id_t thread, BuildJob* buildJob) = 0;
	virtual OCPTree& getFinalOCPtree() = 0;
};

/*
 * ************************************************************************
 * DEFAULT
 * ************************************************************************
 */

class DefaultBuildManager : public BuildManager{
public:

	std::map<thread_id_t,OCPTree> _threads_ocptrees;
	Graph* c_graph;
	BuildGraphSupp* c_bgsupp;
	graph_id_t c_graph_id;

	buildjob_nodesunit_t* c_nodes_map;
	buildjob_nodesunit_t::iterator c_nodesmap_IT;


	std::map<graph_id_t,std::pair<size_t,size_t> > g_labels_count;

	DefaultBuildManager(OCPTree& index, GraphReader& greader, u_size_t lp, thread_id_t nthreads)
			:BuildManager(index,greader,lp, nthreads){
		c_graph_id = -1;

		c_graph = NULL;
		c_bgsupp = NULL;
	}

	void getNodesSets(Graph& g, BuildGraphSupp& bgsupp){
//std::cout<<"@BuildManager.getNodes()\n";
		u_size_t unit_size = (u_size_t)(ceil(g.nodes_count/ (u_size_t)_nthreads));
		if(unit_size * _nthreads < g.nodes_count) unit_size++; //cannot use double

		thread_id_t n_beans = bgsupp.label_nodes_map.size();
		std::pair<thread_id_t,node_label_t> beans[n_beans];
		thread_id_t i=0;
		for(label_nodes_map_t::iterator it = bgsupp.label_nodes_map.begin(); it!=bgsupp.label_nodes_map.end(); it++){
			beans[i] = std::pair<thread_id_t,node_label_t>(it->second.size(), it->first);

			i++;
		}
		std::sort(beans, beans + n_beans);

		bgsupp.build_units.clear();

		thread_id_t c_bean = n_beans-1;
		thread_id_t c_thread = 0;
		while(c_bean >= 0){

			while(bgsupp.build_units[c_thread].size() > unit_size){
				c_thread++;
				c_thread%=_nthreads;
			}

			if((bgsupp.build_units[c_thread].size() + beans[c_bean].first) >= unit_size){
				for(node_id_set_t::iterator
							IT = bgsupp.label_nodes_map[beans[c_bean].second].begin();
							IT != bgsupp.label_nodes_map[beans[c_bean].second].end();
							IT++){
					bgsupp.build_units[c_thread].insert(*IT);
					if(bgsupp.build_units[c_thread].size() >= unit_size){
						c_thread++;
						c_thread%=_nthreads;
					}

				}
			}
			else{
				bgsupp.build_units[c_thread].insert(
						bgsupp.label_nodes_map[beans[c_bean].second].begin(),
						bgsupp.label_nodes_map[beans[c_bean].second].end());
			}
			c_bean--;
		}
	}


	bool nextGraph(){

		c_graph_id++;
		//Graph g(c_graph_id);
		c_graph = new Graph(c_graph_id);
		if(_greader.readGraph(*c_graph)){

			c_bgsupp = new BuildGraphSupp(*c_graph);

			getNodesSets(*c_graph, *c_bgsupp);
			c_nodes_map = &(c_bgsupp->build_units);
			c_nodesmap_IT = c_nodes_map->begin();
			g_labels_count[c_graph_id].first = c_bgsupp->label_nodes_map.size();
			g_labels_count[c_graph_id].second = 0;
			return true;
		}
		else{
			c_graph_id--;
			delete c_graph;
			return false;
		}
	}

	bool getAJob(thread_id_t thread, BuildJob** buildJob){
		bool get = false;

		if(c_graph_id == -1){
			get = nextGraph();
		}
		else if(c_nodesmap_IT == c_nodes_map->end()){
			get = nextGraph();
		}
		else{
			get = true;
		}

		if(get){
			bool destroy = g_labels_count[c_graph_id].first == g_labels_count[c_graph_id].second;
			*buildJob = new BuildJob(_threads_ocptrees[thread], *c_graph, *c_bgsupp, c_nodesmap_IT->second, destroy);
			(g_labels_count[c_graph_id].first)++;
			c_nodesmap_IT++;
		}
		return get;
	}

	virtual void finishJob(thread_id_t thread, BuildJob* buildJob){
		if(buildJob->destroy_graph_onreturn){
			g_labels_count.erase(g_labels_count.find(buildJob->_graph.id));
			delete &(buildJob->_graph);
			delete &(buildJob->_bgsupp);
		}
	}

	virtual OCPTree& getFinalOCPtree(){
		DefaultOCPTMergingListener mlistener;
		for(std::map<thread_id_t, OCPTree>::iterator IT = _threads_ocptrees.begin(); IT!=_threads_ocptrees.end();IT++){
			_globalIndex.merge(IT->second, mlistener);
		}
		return _globalIndex;
	}
};

}


#endif /* BUILDINGENV_H_ */
