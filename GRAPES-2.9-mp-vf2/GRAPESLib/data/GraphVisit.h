/*
 * MstlGraphVisit.h
 *
 *  Created on: Nov 6, 2010
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

#ifndef MSTLGRAPHVISIT_H_
#define MSTLGRAPHVISIT_H_

#include <stack>

#include "size_t.h"
#include "typedefs.h"

#include "GraphPathListener.h"

namespace GRAPESLib{

class DFSGraphVisitor{
	class stack_item{
	public:
		GNode& node;
		node_id_set_t::iterator e_IT;
		bool out_IT;
		bool start;
		stack_item(GNode& n) : node(n){
			//e_IT = node.out_neighbors.begin();
			e_IT = node.out_neighbors.begin();
			out_IT = true;
			start = true;
		}
		bool next_neigh(bool directGraph, std::map<node_id_t,bool>& visited){
			if(out_IT){
				if(start){
					start = false;
					while((e_IT!=node.out_neighbors.end()) && visited[*e_IT]){
						e_IT++;
					}
				}
				else{
					do{
						e_IT++;
					}while((e_IT!=node.out_neighbors.end()) && visited[*e_IT]);
				}
				if(e_IT==node.out_neighbors.end()){
					e_IT=node.in_neighbors.begin();
					out_IT=false;
					start = true;
				}
				else{
					return true;
				}
			}
			return false;
		}
	};

public:
	GraphPathListener& plistener;

public:
	DFSGraphVisitor(GraphPathListener& _plistener)
			: plistener(_plistener){
	}

	DFSGraphVisitor& clone(){
		return *(new DFSGraphVisitor(plistener.clone()));
	}

	void run(Graph& g, u_size_t& maxdepth){
		for(node_id_t i = 0; i<g.nodes_count; i++)
			run(g, g.nodes[i], maxdepth);
	}


	u_lsize_t run(Graph& g, GNode& n, u_size_t& maxdepth){
		u_lsize_t visited_count = 0;
		std::stack<stack_item> stack;
		std::map<node_id_t,bool> visited;

		plistener.start_vertex(n);
		visited[n.id] = true;
		stack.push(stack_item(n));

		u_size_t depth = 1;
		stack_item* c_item;
		GNode* c;
		while(!stack.empty()){

			c_item = &(stack.top());
			if(depth+1<=maxdepth && c_item->next_neigh(g.direct, visited)){
				c = &(g.nodes[*(c_item->e_IT)]);

				plistener.discover_vertex(*c);

				visited[c->id] = true;
				stack.push(stack_item(*c));
				depth++;
			}
			else{
				plistener.finish_vertex(c_item->node);
				visited[c_item->node.id] = false;
				stack.pop();
				depth--;
			}
		}
		return visited_count;
	}

};

}


#endif /* MSTLGRAPHVISIT_H_ */
