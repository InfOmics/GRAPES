/*
 * Graph.h
 *
 *  Created on: Dec 26, 2010
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

#ifndef GRAPH_H_
#define GRAPH_H_

#include <set>
#include <list>
#include <map>
#include <iostream>

#include "size_t.h"
#include "typedefs.h"

namespace GRAPESLib{


class GNode{
public:
	node_id_t id;
	node_label_t label;
	node_id_set_t out_neighbors;
	node_id_set_t in_neighbors;

	GNode(){
		this->id = -1;
		this->label = -1;
	}
	GNode(node_id_t id, node_label_t label){
		this->id = id;
		this->label = label;
	}
};


class Graph{
public:

	std::string name;
	graph_id_t id;
	bool direct;
	node_id_t nodes_count;
	GNode* nodes;


	Graph(){
		this->id  = -1;
		this->direct = false;
		this->nodes = NULL;
		this->nodes_count = 0;
	}

	Graph(graph_id_t id){
		this->id  = id;
		this->direct = false;
		this->nodes = NULL;
		this->nodes_count = 0;
	}

	Graph(graph_id_t id, bool direct, node_id_t size){
		this->id  = id;
		this->direct = direct;
		this->nodes_count = size;
		this->nodes = new GNode[nodes_count];
	}

	void setSize(node_id_t size){
		this->nodes_count = size;

		if(this->nodes != NULL){
			delete nodes;
		}
		this->nodes = new GNode[nodes_count];
	}

	node_id_t size(){
		return this->nodes_count;
	}

	void insertNode(node_id_t id, node_label_t label){
		if(id < this->nodes_count){
			nodes[id].id = id;
			nodes[id].label = label;
//			label_nodes_map[label].insert(id);
		}
	}
	void insertEdge(node_id_t from, node_id_t to){
		if(from < this->nodes_count  &&  to < this->nodes_count){
			nodes[from].out_neighbors.insert(to);
		}
	}


	void fillInAdiacsFull(){
		for(int i=0; i<nodes_count; i++){
			for(node_id_set_t::iterator IT = nodes[i].out_neighbors.begin(); IT!=nodes[i].out_neighbors.end(); i++){
				nodes[(*IT)].in_neighbors.insert(i);
			}
		}
	}
	void fillInAdiacsExclusive(){
		for(int i=0; i<nodes_count; i++){
			for(node_id_set_t::iterator IT = nodes[i].out_neighbors.begin(); IT!=nodes[i].out_neighbors.end(); i++){
				if(nodes[(*IT)].out_neighbors.find(i) == nodes[(*IT)].out_neighbors.end())
					nodes[(*IT)].in_neighbors.insert(i);
			}
		}
	}

	void print(std::ostream& os){
		os<<"graph_id:"<<id<<"\n";
		os<<"\tnodes_count="<<nodes_count<<"\n";
		for(node_id_t i=0;i<nodes_count;i++){
			os<<"\tnode["<<i<<"] id="<<nodes[i].id<<" label="<<nodes[i].label<<"\n";
			os<<"\t\tout[";
			for(node_id_set_t::iterator IT=nodes[i].out_neighbors.begin(); IT!=nodes[i].out_neighbors.end();IT++)
				os<<*IT<<",";
			os<<"]\n";
		}
	}
};

}


#endif /* GRAPH_H_ */
