/*
 * OCPTreeListeners.h
 *
 *  Created on: Nov 7, 2010
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

#ifndef OCPTREELISTENERS_H_
#define OCPTREELISTENERS_H_

#include <set>
#include <map>

#include "size_t.h"
#include "typedefs.h"

#include "OCPTreeNode.h"

namespace GRAPESLib{

class OCPTreeLoaderListener{
public:
	virtual void loaded_node(OCPTreeNode &n)=0;
};

class OCPTreeMatchingListener{
public:
	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b)=0;
	virtual bool unmatched_node(OCPTreeNode &a) =0;
};

class OCPTreeMergingListener{
public:
	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b)=0;
	virtual bool unmatched_node(OCPTreeNode &a, OCPTreeNode &b)=0;
	virtual bool shifted_node(OCPTreeNode &a, OCPTreeNode &b)=0;
};



class DefaultOCPTLoaderListener : public OCPTreeLoaderListener{
public:
	DefaultOCPTLoaderListener(){}
	virtual void loaded_node(OCPTreeNode &n){}
};

class DefaultOCPTMatchingListener : public OCPTreeMatchingListener{
public:
	filtering_graph_set_t& _graphs;
	graph_node_cands_t& _gncands;

	DefaultOCPTMatchingListener(filtering_graph_set_t &graphs, graph_node_cands_t& gncands)
			: _graphs(graphs), _gncands(gncands){
	}

	bool prune_rule_1(	OCPTreeNode &a,
						OCPTreeNode &b){
		size_t pre_size= _graphs.size();
		size_t occ = a.gsinfos[0].path_occurrence;
		for(OCPTNGraphsInfos::iterator IT = b.gsinfos.begin(); IT!=b.gsinfos.end(); IT++){
			if((IT->second).path_occurrence >= occ)
				_graphs.insert(IT->first);
		}
		return pre_size!=_graphs.size();
	}
	bool prune_rule_1(	OCPTreeNode &a,
						OCPTreeNode &b,
						filtering_graph_set_t::iterator& fgs_IT){
		OCPTNGraphsInfos::iterator gis_IT = b.gsinfos.find(*fgs_IT);
		if(gis_IT==b.gsinfos.end()){
			return false;
		}
		else{
			if(gis_IT->second.path_occurrence < a.gsinfos[0].path_occurrence){
				return false;
			}
		}
		return true;
	}

	bool prune_rule_2(	OCPTreeNode &a,
						OCPTreeNode &b,
						graph_node_cands_t::iterator& gncands_IT){

		bool ret = true;
		sbitset* a_from_nodes=&(a.gsinfos[0].from_nodes);
		//TODO? i<... && ret
//		for(size_t i=0;i<a_from_nodes->size(); i++){
		//for(sbitset::iterator sIT = a_from_nodes->begin(); sIT!=a_from_nodes->end(); sIT++){


		for(sbitset::iterator sIT = a_from_nodes->first_ones(); sIT!=a_from_nodes->end(); sIT.next_ones()){
//			if(a_from_nodes->get(i)){
				sbitset& ncands = (gncands_IT->second)[ sIT.first ];

//				if(ncands.size() > 16448){
//					std::cout<<">ncands.size("<<ncands.size()<<")\n";
//				}
//
//				if(b.gsinfos[gncands_IT->first].from_nodes.size() > 16431){
//					std::cout<<"###########################\n";
//					std::cout<<"node("<<gncands_IT->first<<") size("<<b.gsinfos[gncands_IT->first].from_nodes.size()<<")\n";
//				}

				if(ncands.at_least_one() > 0){
//					std::cout<<"AND\n";
					ncands &= b.gsinfos[gncands_IT->first].from_nodes;
				}
				else{
//					std::cout<<"OR\n";
					ncands |= b.gsinfos[gncands_IT->first].from_nodes;
				}
				ret &= ncands.at_least_one();


//				if(ncands.size() > 16448){
//					std::cout<<"<ncands.size("<<ncands.size()<<")\n";
//				}
//			}


		}
		return ret;
	}

	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b){
		if(_graphs.size()==0){
			prune_rule_1(a,b);
			filtering_graph_set_t::iterator fgs_IT = _graphs.begin();
			while(fgs_IT!=_graphs.end()){
				std::pair<graph_node_cands_t::iterator,bool> gncands_IT_i
					= _gncands.insert(std::pair<graph_id_t, node_cands_t>(*fgs_IT,node_cands_t()));
				graph_node_cands_t::iterator gncands_IT = gncands_IT_i.first;
				if(!prune_rule_2(a,b,gncands_IT)){
					_graphs.erase(fgs_IT++);
					_gncands.erase(gncands_IT);
				}
				else{
					fgs_IT++;
				}
			}
		}
		else{
			filtering_graph_set_t::iterator fgs_IT = _graphs.begin();
			bool erase = false;
			while(fgs_IT!=_graphs.end()){
				std::pair<graph_node_cands_t::iterator,bool> gncands_IT_i
					= _gncands.insert(std::pair<graph_id_t, node_cands_t>(*fgs_IT,node_cands_t()));
				graph_node_cands_t::iterator gncands_IT = gncands_IT_i.first;

				erase = !prune_rule_1(a,b,fgs_IT);
				if(erase){
					_graphs.erase(fgs_IT++);
					_gncands.erase(gncands_IT);
				}
				else{
					erase = !prune_rule_2(a,b,gncands_IT);
					if(erase){
						_graphs.erase(fgs_IT++);
						_gncands.erase(gncands_IT);
					}
					else
						fgs_IT++;
				}
			}
		}
		if(_graphs.size()==0)
			return false;
		return true;
	}
	virtual bool unmatched_node(OCPTreeNode &a){
		_graphs.clear();
		return false;
	}
};


class DefaultOCPTMergingListener : public OCPTreeMergingListener{
public:
	DefaultOCPTMergingListener() : OCPTreeMergingListener(){
	}
	~DefaultOCPTMergingListener(){
	}

	void copy_node(OCPTreeNode& a, OCPTreeNode& b){
		a.gsinfos = OCPTNGraphsInfos(b.gsinfos);
		a.is_special = b.is_special;
		OCPTreeNode* cb = b.first_child;
		OCPTreeNode* ca;
		while(cb!=NULL){
			ca = a.add_child_on_tail(cb->label);
			copy_node(*ca,*cb);
			cb = cb->next;
		}
	}

	void merge_nodes(OCPTreeNode& a, OCPTreeNode& b){
		//DA REIMPLEMENTARE CON GLI ITERATORI IN MODO DA OTTIMIZZARE
		a.is_special |= b.is_special;
		OCPTNGraphsInfos::iterator a_IT;
		OCPTNGraphsInfos::iterator b_IT;
		for(a_IT=a.gsinfos.begin(); a_IT!=a.gsinfos.end();a_IT++){
			b_IT = b.gsinfos.find(a_IT->first);
			if(b_IT != b.gsinfos.end()){
				a_IT->second.path_occurrence += b_IT->second.path_occurrence;
				a_IT->second.from_nodes |= b_IT->second.from_nodes;
			}
		}
		for(b_IT=b.gsinfos.begin(); b_IT!=b.gsinfos.end();b_IT++){
			a_IT = a.gsinfos.find(b_IT->first);
			if(a_IT == a.gsinfos.end()){
				a.gsinfos.insert(std::pair<graph_id_t, OCPTNGraphInfo>(b_IT->first,OCPTNGraphInfo(b_IT->second)));
			}
		}
	}

	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b){
		merge_nodes(a,b);
	}
	virtual bool unmatched_node(OCPTreeNode &a, OCPTreeNode &b){
		//parent(b), node(b)
	}
	virtual bool shifted_node(OCPTreeNode &a, OCPTreeNode &b){
		//parent(a), node(b)
		OCPTreeNode* n = a.add_child_on_tail(b.label);
		copy_node(*n, b);
	}
};

}

#endif /* OCPTREELISTENERS_H_ */
