/*
 * typedefs.h
 *
 *  Created on: Apr 27, 2013
 *      Author: vbonnici
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

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_


#include <map>
#include <set>
#include <list>
#include "size_t.h"
#include "sbitset.h"

#include "argedit.h"
#include "argraph.h"

namespace GRAPESLib{

/*   GRAPHS   */
typedef ARGraph<node_id_t,void> VF2Graph;
typedef VF2Graph QueryGraph;
typedef VF2Graph ReferenceGraph;


/*	CONNECTED COMPONENTS	*/
typedef sbitset 							connected_component_t;
typedef std::pair<node_id_t, node_id_t> 	match_job_t;//s q node, t node....    t node,  p node
//typedef connected_component_t 				match_job_t;
typedef std::list< match_job_t > 			match_jobs_t;




/*	NODES STRUCTS	*/
typedef std::set<node_id_t> 		node_id_set_t;
typedef std::list<node_id_t> 		node_id_list_t;

typedef std::set<node_label_t> 		node_label_set_t;
typedef std::list<node_label_t> 	node_label_list_t;

typedef std::map<node_label_t, node_id_set_t> 		label_nodes_map_t;

/*	GRAPHS STRCUTS	*/
typedef std::set<graph_id_t> 		graph_id_set_t;
typedef std::list<graph_id_t> 		graph_id_list_t;


/*	NODES <=> GRAPHS	*/
typedef std::map<node_id_t, sbitset> 			node_cands_t;
typedef std::map<graph_id_t, node_cands_t>		graph_node_cands_t;


/*	FILTERING	*/
typedef graph_id_set_t 			filtering_graph_set_t;


/*	PARALLEL BUILD	*/
typedef std::map<u_size_t, node_id_set_t> 	buildjob_nodesunit_t;



}



#endif /* TYPEDEFS_H_ */
