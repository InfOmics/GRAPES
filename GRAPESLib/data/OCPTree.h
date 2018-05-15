/*
 * OCPTree.h
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

#ifndef OCPTREE_H_
#define OCPTREE_H_

#include <iostream>
#include <fstream>
#include <map>
#include <queue>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "OCPTreeNode.h"
#include "OCPTreeListeners.h"

namespace GRAPESLib{

class OCPQueueNode{
public:
	OCPTreeNode* f_node;
	OCPTreeNode* s_node;
	OCPQueueNode* next;
	OCPQueueNode(){
		f_node = NULL;
		s_node = NULL;
		next = NULL;
	};
	OCPQueueNode(OCPTreeNode* _f_node, OCPTreeNode* _s_node){
		this->f_node=_f_node;
		this->s_node=_s_node;
		this->next=NULL;
	};
};

class OCPTree{
public:
	OCPTreeNode* root;

	OCPTree(){
		this->root=new OCPTreeNode((node_label_t)NULL, NULL);
	}
	~OCPTree(){};


	void write(FILE* fd, OCPTreeNode* n){
		n->write(fd);
		OCPTreeNode* c = n->first_child;
		while(c!=NULL){
			write(fd,c);
			c=c->next;
		}
	}
	void write(FILE* fd){
		write(fd, root);

	}

	void read(FILE* fd, OCPTreeNode& n){
		n.read(fd);
		for(int i=0; i<n.child_count; i++){
			OCPTreeNode* c= new OCPTreeNode((node_label_t)NULL,NULL);
			n.add_child_on_tail(c);
			read(fd, *c);
		}
	}
	void read(FILE* fd){
		root = new OCPTreeNode((int)NULL,NULL);
		read(fd, *root);
	}




	void match(OCPTree &g, OCPTreeMatchingListener& mlistener){
		//se due nodi stanno alla stessa posizione delle due code
		//vuol dire che sono mecciati
		//this deve essere la query

		OCPTreeNode* nt;
		OCPTreeNode* ng;
		OCPTreeNode* ct;
		OCPTreeNode* cg;

		OCPQueueNode* current_qn;
		OCPQueueNode* tail_qn;

		OCPQueueNode* delete_queue_node;

		current_qn=new OCPQueueNode(this->root, g.root);
		tail_qn=current_qn;

		bool goon=true;
		while(current_qn!=NULL && goon){
			nt=current_qn->f_node;
			ng=current_qn->s_node;

			if(nt->is_special){
				goon = mlistener.matched_nodes(*nt,*ng);
			}

			ct=nt->first_child;
			cg=ng->first_child;
			while(ct!=NULL && goon){
				while(cg!=NULL && cg->label<ct->label)
					cg=cg->next;
				if(cg==NULL || cg->label!=ct->label){
					goon = mlistener.unmatched_node(*ct);
				}
				else{
					tail_qn->next=new OCPQueueNode(ct, cg);
					tail_qn=tail_qn->next;
				}
				ct=ct->next;
			}
			delete_queue_node = current_qn;
			current_qn=current_qn->next;
			delete delete_queue_node;
		}
	}

	void merge(OCPTree& g, OCPTreeMergingListener& mlistener){
		//se due nodi stanno alla stessa posizione delle due code
		//vuol dire che sono mecciati
		//this deve essere la query

		OCPTreeNode* nt;
		OCPTreeNode* ng;
		OCPTreeNode* ct;
		OCPTreeNode* cg;

		OCPQueueNode* current_qn;
		OCPQueueNode* tail_qn;

		OCPQueueNode* delete_queue_node;

		current_qn=new OCPQueueNode(this->root, g.root);
		tail_qn=current_qn;

		while(current_qn!=NULL){
			nt=current_qn->f_node;
			ng=current_qn->s_node;

			mlistener.matched_nodes(*nt,*ng);

			ct=nt->first_child;
			cg=ng->first_child;
			while(ct!=NULL){
				if(cg!=NULL && cg->label==ct->label){
					tail_qn->next=new OCPQueueNode(ct, cg);
					tail_qn=tail_qn->next;
					cg=cg->next;
				}
				else{
					while(cg!=NULL && cg->label<ct->label){
						mlistener.shifted_node(*nt,*cg);
						cg=cg->next;
					}
					if(cg==NULL || cg->label!=ct->label){
						mlistener.unmatched_node(*nt,*ct);
					}
					else{
						tail_qn->next=new OCPQueueNode(ct, cg);
						tail_qn=tail_qn->next;
						cg=cg->next;
					}
				}
				ct=ct->next;
			}
			while(cg!=NULL){
				mlistener.shifted_node(*nt,*cg);
				cg=cg->next;
			}
			delete_queue_node = current_qn;
			current_qn=current_qn->next;
			delete delete_queue_node;
		}
	}

	void print_xml(std::ostream& s){
		OCPTreeNode* n = this->root;
		print_xml(s,n,0);
	}
	void print_xml(std::ostream& s,OCPTreeNode* n, int level){
		if(n!=NULL){
			for(int i=0;i<level;i++)s<<"\t";
			s<<"<node label=\""<<n->label<<"\" child_count=\""<<n->child_count<<"\" isspecial=\""<<n->is_special<<"\">\n";

			for(int i=0;i<level;i++)s<<"\t";
			s<<"<OCPTNGraphsInfos>\n";
			for(OCPTNGraphsInfos::iterator s_IT = n->gsinfos.begin(); s_IT!=n->gsinfos.end(); s_IT++){
				for(int i=0;i<level+1;i++)s<<"\t";
				s<<"<element id=\""<<(s_IT->first)<<"\" path_occurences=\""<<s_IT->second.path_occurrence<<"\">\n";

				for(int i=0;i<level+2;i++)s<<"\t";
				s<<"<bitset size=\""<<s_IT->second.from_nodes.cardinality()<<"\">\n";

				for(int i=0;i<level+3;i++)s<<"\t";
				for(size_t i=0;i<s_IT->second.from_nodes.size();i++)
					s<<s_IT->second.from_nodes.get(i);
				s<<"\n";

				for(int i=0;i<level+3;i++)s<<"\t";
				for(sbitset::iterator oIT = s_IT->second.from_nodes.first_ones();  oIT != s_IT->second.from_nodes.end(); oIT.next_ones())
					s<<oIT.first<<" ";
				s<<"\n";

				for(int i=0;i<level+2;i++)s<<"\t";
				s<<"</bitset>\n";

				for(int i=0;i<level+1;i++)s<<"\t";
				s<<"</element>\n";
			}
			for(int i=0;i<level;i++)s<<"\t";
			s<<"</OCPTNGraphsInfos>\n";

			OCPTreeNode* c = n->first_child;
			while(c!=NULL){
				print_xml(s,c,level+1);
				c = c->next;
			}

			for(int i=0;i<level;i++)s<<"\t";
			s<<"</node>\n";
		}
	}
};

}
#endif /* OCPTREE_H_ */
