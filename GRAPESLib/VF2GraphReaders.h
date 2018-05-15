/*
 * RIGraphReaders.cpp
 *
 *  Created on: Feb 11, 2013
 *      Author: vbonnici
 */
/*
Copyright (c) 2013 by Rosalba Giugno

RI is provided under the terms of The MIT License (MIT):

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

#ifndef RIGRAPHREADERS_H
#define RIGRAPHREADERS_H



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include "LabelMap.h"
#include "typedefs.h"

namespace GRAPESLib{

#define STR_READ_LENGTH 256

typedef short vf2_count_type;
typedef unsigned short vf2_node_id;
const vf2_node_id NULL_NODE=0xFFFF;



class VF2GraphReader{
public:
	LabelMap& _labelMap;
	VF2GraphReader(LabelMap& labelMap) : _labelMap(labelMap){}

	virtual ~VF2GraphReader(){}

	virtual ARGEdit* readGraph() = 0;
	virtual VF2Graph* readSGraph() = 0;

	virtual bool open(const std::string& fname) = 0;
	virtual bool close() = 0;
};

class VF2GraphReader_gfu : public VF2GraphReader{
public:

	FILE* fd;
	graph_id_t c_id;

	bool direct;

	VF2GraphReader_gfu(LabelMap& labelMap)
			: VF2GraphReader(labelMap){
		c_id = 0;
		direct = false;
		fd = NULL;
	}
	~VF2GraphReader_gfu(){}

	virtual bool open(const std::string& fname){
		fd = fopen(fname.c_str(), "r");
		if (fd==NULL){
			printf("ERROR: Cannot open input file %s\n", fname.c_str());
			exit(1);
		}
		return true;
	}
	virtual bool close(){
		fclose(fd);
		return true;
	}

	struct gr_neighs_t{
	public:
		int nid;
		gr_neighs_t *next;
	};

	virtual ARGEdit* readGraph(){

		if(!feof(fd)){

			ARGEdit* ed = new ARGEdit();

			char str[STR_READ_LENGTH];
			int i,j;

			node_id_t nof_nodes = 0;
			int nof_edges = 0;

			if (fscanf(fd,"%s",str) != 1){	//#graphname
				if(!feof(fd))
				std::cout<<"error reading graphname\n";
				return NULL;
			}
			if (fscanf(fd,"%d",&(nof_nodes)) != 1){//nof nodes
				std::cout<<"error reading nof nodes\n";
				return NULL;
			}

			int* nn = new int[nof_nodes];

			//node labels
			char *label = new char[STR_READ_LENGTH];
			for(i=0; i<nof_nodes; i++){
				if (fscanf(fd,"%s",label) != 1){
					return NULL;
				}
//				node_id_t a = i;
//				ed->InsertNode(&a);
				nn[i] = i;
				ed->InsertNode(&(nn[i]));
			}

			int temp = 0;
			if (fscanf(fd,"%d",&temp) != 1){//number of edges
				std::cout<<"error reading nof edges\n";
				return NULL;
			}
			int es = 0, et = 0;
			for(i=0; i<temp; i++){
				if (fscanf(fd,"%d",&es) != 1){//source node
					std::cout<<"error reading source node\n";
					return NULL;
				}
				if (fscanf(fd,"%d",&et) != 1){//target node
					std::cout<<"error reading target node\n";
					return NULL;
				}

				ed->InsertEdge(es, et, NULL);
				if(!direct)
					ed->InsertEdge(et, es, NULL);
			}

			return ed;
		}
		return NULL;
	}

	void sort_edges(VF2Graph* g){
		for(int i=0;i<g->n;i++){
			if(g->out_count[i]>1){
				quicksort_edges(g->out[i], 0, g->out_count[i] -1);
			}
			if(g->in_count[i]>1){
				quicksort_edges(g->in[i], 0, g->in_count[i] -1);
			}
		}
	}
	void quicksort_edges(vf2_node_id* adj_list, int p, int r){
		if(p<r){
			int q=quicksort_edges_partition(adj_list, p, r);
			quicksort_edges(adj_list, p, q-1);
			quicksort_edges(adj_list, q+1, r);
		}
	}
	int quicksort_edges_partition(vf2_node_id* adj_list, int p, int r){
		vf2_node_id ltmp;
		vf2_node_id target = adj_list[r];
		int i = p-1;
		for(int j=p; j<r; j++){
			//if(adj_list[j] < target || (adj_list[j]==target  && (edgeComparator.compareint(adj_attrs[j], attr)<=0))){
			if(adj_list[j] < target){
				i++;
				ltmp = adj_list[i];
				adj_list[i] = adj_list[j];
				adj_list[j] = ltmp;
			}
		}
		ltmp = adj_list[i+1];
		adj_list[i+1] = adj_list[r];
		adj_list[r] = ltmp;
		return i+1;
	}

	virtual VF2Graph* readSGraph(){

			if(!feof(fd)){
				VF2Graph* graph = new VF2Graph();
//				int n;              /* number of nodes  */
//				void* *attr;        /* node attributes  */
//				count_type  *in_count;  /* number of 'in' edges for each node */
//				node_id **in;       /* nodes connected by 'in' edges to each node */
//				void* **in_attr;    /* Edge attributes for 'in' edges */
//				count_type  *out_count; /* number of 'out edges for each node */
//				node_id **out;      /* nodes connected by 'out' edges to each node */
//				void* **out_attr;   /* Edge attributes for 'out' edges */


				char str[STR_READ_LENGTH];
				int i,j;

				if (fscanf(fd,"%s",str) != 1){	//#graphname
					return NULL;
				}
				if (fscanf(fd,"%d",&(graph->n)) != 1){//nof nodes
					return NULL;
				}

				graph->attr = new void*[graph->n];
				int* nn = new int[graph->n];

				//node labels
				char *label = new char[STR_READ_LENGTH];
				for(i=0; i<graph->n; i++){
					if (fscanf(fd,"%s",label) != 1){
						return NULL;
					}
					nn[i] = i;
					graph->attr[i] = &(nn[i]);
				}

				graph->out_count = new vf2_count_type[graph->n];
				graph->in_count = new vf2_count_type[graph->n];
				for(int i=0; i<graph->n; i++){
					graph->out_count[i] = 0;
					graph->in_count[i] = 0;
				}

				gr_neighs_t **ns_o = (gr_neighs_t**)malloc(graph->n * sizeof(gr_neighs_t));
				gr_neighs_t **ns_i = (gr_neighs_t**)malloc(graph->n * sizeof(gr_neighs_t));
				for(i=0; i<graph->n; i++){
					ns_o[i] = NULL;
					ns_i[i] = NULL;
				}
				int temp = 0;
				if (fscanf(fd,"%d",&temp) != 1){//number of edges
					return NULL;
				}

				int es = 0, et = 0;
				for(i=0; i<temp; i++){
					if (fscanf(fd,"%d",&es) != 1){//source node
						return NULL;
					}
					if (fscanf(fd,"%d",&et) != 1){//target node
						return NULL;
					}

					graph->out_count[es]++;
					graph->in_count[et]++;

					if(ns_o[es] == NULL){
						ns_o[es] = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
						ns_o[es]->nid = et;
						ns_o[es]->next = NULL;
					}
					else{
						gr_neighs_t* n = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
						n->nid = et;
						n->next = (struct gr_neighs_t*)ns_o[es];
						ns_o[es] = n;
					}

					if(!direct){
					graph->out_count[et]++;
					graph->in_count[es]++;

					if(ns_o[et] == NULL){
						ns_o[et] = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
						ns_o[et]->nid = es;
						ns_o[et]->next = NULL;
					}
					else{
						gr_neighs_t* n = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
						n->nid = es;
						n->next = (struct gr_neighs_t*)ns_o[et];
						ns_o[et] = n;
					}
					}

				}


				graph->in_attr = new void**[graph->n];
				graph->out_attr = new void**[graph->n];

				graph->in = new vf2_node_id*[graph->n];
				graph->out = new vf2_node_id*[graph->n];

				int* ink = (int*)calloc(graph->n, sizeof(int));
				for (i=0; i<graph->n; i++){
					graph->in[i] = new vf2_node_id[ graph->in_count[i] ];
					graph->in_attr[i] = new void*[graph->in_count[i]];
				}
				for (i=0; i<graph->n; i++){
					// reading degree and successors of vertex i
					graph->out[i] = new vf2_node_id[ graph->out_count[i] ];
					graph->out_attr[i] = new void*[graph->out_count[i]];

					gr_neighs_t *n = ns_o[i];
					for (j=0; j<graph->out_count[i]; j++){
						graph->out[i][j] = n->nid;

						graph->out_attr[i][j] = NULL;

						graph->in[n->nid][ink[n->nid]] = i;

						graph->in_attr[n->nid][ink[n->nid]] = NULL;

						ink[n->nid]++;

						n = n->next;
					}
				}

				free(ink);

				for(int i=0; i<graph->n; i++){
					if(ns_o[i] != NULL){
						gr_neighs_t *p = NULL;
						gr_neighs_t *n = ns_o[i];
						for (j=0; j<graph->out_count[i]; j++){
							if(p!=NULL)
								free(p);
							p = n;
							n = n->next;
						}
						if(p!=NULL)
						free(p);
					}

					if(ns_i[i] != NULL){
						gr_neighs_t *p = NULL;
						gr_neighs_t *n = ns_i[i];
						for (j=0; j<graph->out_count[i]; j++){
							if(p!=NULL)
								free(p);
							p = n;
							n = n->next;
						}
						if(p!=NULL)
						free(p);
					}
		//			free(ns_o);
		//			free(ns_i);
				}

				sort_edges(graph);

				return graph;
			}
			return NULL;
	}
};

}

#endif
