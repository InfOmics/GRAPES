/*
 * MSTLGraphVisit.h
 *
 *  Created on: Nov 3, 2010
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

#ifndef MSTLGPATHLISTENER_H_
#define MSTLGPATHLISTENER_H_

#include <set>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "OCPTree.h"

namespace GRAPESLib{

class GraphPathListener {
public:
	OCPTree* index_tree;
	node_id_t start_vertex_id;
	OCPTreeNode* current_node;
	graph_id_t graph_id;

public:
	GraphPathListener(){
		index_tree=NULL;
		start_vertex_id=-1;
		current_node=NULL;
		graph_id=-1;
	}
	GraphPathListener(OCPTree* index_tree, graph_id_t graph_id){
		this->index_tree = index_tree;
		this->graph_id = graph_id;
		this->start_vertex_id = -1;
		current_node=NULL;
	}
	GraphPathListener (const GraphPathListener& l){
		this->index_tree = l.index_tree;
		this->graph_id = l.graph_id;
		this->start_vertex_id = l.start_vertex_id;
		current_node=NULL;
	}

	virtual GraphPathListener& clone()=0;

	virtual void start_vertex(GNode& n)=0;
	virtual void discover_vertex(GNode& n)=0;
	virtual void finish_vertex(GNode& n)=0;
};

class EmptyPathListener :  public GraphPathListener{
	virtual GraphPathListener& clone(){
		return *(new EmptyPathListener());
	}
	virtual void start_vertex(GNode& n){
	};
	virtual void discover_vertex(GNode& n){};
	virtual void finish_vertex(GNode& n){};
};


class AllPathListener: public GraphPathListener{
public:
	AllPathListener()
			: GraphPathListener(){
	}
	AllPathListener(OCPTree* index_tree, graph_id_t graph_id)
			: GraphPathListener(index_tree, graph_id){
	}

	virtual AllPathListener& clone(){
		return *(new AllPathListener());
	}

	virtual void start_vertex(GNode& n){
		node_label_t label = n.label;
		current_node=index_tree->root->add_child(label);
		start_vertex_id = n.id;
		current_node->gsinfos[graph_id].path_occurrence++;
		current_node->gsinfos[graph_id].from_nodes.set(start_vertex_id,true);
		current_node->is_special=true;
	}
	virtual void discover_vertex(GNode& n){
		node_label_t label = n.label;
		current_node=current_node->add_child(label);
		current_node->gsinfos[graph_id].path_occurrence ++;
		current_node->gsinfos[graph_id].from_nodes.set(start_vertex_id,true);
		current_node->is_special=true;
	}
	virtual void finish_vertex(GNode& n){
		current_node=current_node->parent;
	}
};

class OnePathListener: public GraphPathListener{
protected:
	std::set<OCPTreeNode*> v_set;
	std::set<OCPTreeNode*>::iterator v_set_it;
public:

	virtual GraphPathListener& clone(){
		return *(new OnePathListener());
	}

	virtual void start_vertex(GNode& n){
		v_set.clear();
		node_label_t label = n.label;
		current_node=index_tree->root->add_child(label);
		start_vertex_id = n.id;
	}
	virtual void discover_vertex(GNode& n){
		v_set.insert(current_node);
		node_label_t label = n.label;
		current_node=current_node->add_child(label);
	}
	virtual void finish_vertex(GNode& n){
		v_set_it=v_set.find(current_node);
		if(v_set_it==v_set.end()){
			current_node->is_special=true;
			current_node->gsinfos[graph_id].path_occurrence ++;
			current_node->gsinfos[graph_id].from_nodes.set(start_vertex_id,true);
		}
		else
			v_set.erase(v_set_it);
		current_node=current_node->parent;
	}
};

}


#endif /* MSTLGRAPHVISIT_H_ */
