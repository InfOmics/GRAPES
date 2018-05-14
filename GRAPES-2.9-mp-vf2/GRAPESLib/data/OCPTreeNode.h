/*
 * OCPTreeNode.h
 *
 *  Created on: Dec 28, 2010
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

#ifndef OCPTREENODE_H_
#define OCPTREENODE_H_

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "sbitset.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>


namespace GRAPESLib{

class OCPTNGraphInfo{
public:
	u_path_id_t path_occurrence;
	sbitset from_nodes;

	OCPTNGraphInfo(){
		path_occurrence = 0;
	}//!!!do not use!!!fake init!!!

	OCPTNGraphInfo(Graph &g) : from_nodes(g.size()){
		path_occurrence = 0;
	}
	OCPTNGraphInfo(u_path_id_t _path_occurrence, sbitset _from_nodes)
		: from_nodes(_from_nodes){
		path_occurrence = _path_occurrence;
	}

	OCPTNGraphInfo(const OCPTNGraphInfo& c) : from_nodes(c.from_nodes){
		path_occurrence =c.path_occurrence;
	}

//	friend std::istream& operator>>(std::istream& is, OCPTNGraphInfo& gi){
//		is.read((char*)&(gi.path_occurrence), sizeof(int));
//		is>>gi.from_nodes;
//		return is;
//	}
//	friend std::ostream& operator<<(std::ostream& os, OCPTNGraphInfo& gi){
//		os.write((char*)&(gi.path_occurrence), sizeof(int));
//		os<<gi.from_nodes;
//		return os;
//	}

	void write(FILE *fd){
		fwrite(&path_occurrence, sizeof(u_path_id_t), 1, fd);
		from_nodes.write(fd);
	}

	void read(FILE* fd){
		fread(&path_occurrence, sizeof(u_path_id_t), 1, fd);
		from_nodes.read(fd);
	}
};

class OCPTNGraphsInfos : public std::map<graph_id_t, OCPTNGraphInfo>{
public:
//	friend std::istream& operator>>(std::istream& is, OCPTNGraphsInfos& gi){
//		size_t size = 0;
//		is.read((char*)&size, sizeof(size_t));
//		for(int i=0;i<size;i++){
//			graph_id_t graph_id=0;
//			OCPTNGraphInfo graph_info;
//			is.read((char*)&graph_id, sizeof(graph_id_t));
//			is>>graph_info;
//			gi.insert(std::pair<graph_id_t, OCPTNGraphInfo>(graph_id, graph_info));
//		}
//		return is;
//	}
//	friend std::ostream& operator<<(std::ostream& os, OCPTNGraphsInfos& gi){
//		size_t size = gi.size();
//		os.write((char*)&size, sizeof(size_t));
//		for(OCPTNGraphsInfos::iterator IT=gi.begin(); IT!=gi.end();IT++){
//			os.write((char*)&(IT->first), sizeof(graph_id_t));
//			os<<(IT->second);
//		}
//		return os;
//	}

	void write(FILE *fd){
		u_size_t msize = (u_size_t)size();
		fwrite(&msize, sizeof(u_size_t), 1, fd);
		for(OCPTNGraphsInfos::iterator IT=begin(); IT!=end();IT++){
			fwrite(&(IT->first), sizeof(graph_id_t),1, fd);
			(IT->second).write(fd);
		}

	}

	void read(FILE* fd){
		u_size_t size = 0;
		fread(&size, sizeof(u_size_t),1, fd);
		for(u_size_t i=0; i<size; i++){
			graph_id_t graph_id = 0;
			OCPTNGraphInfo graph_info;

			fread(&graph_id, sizeof(graph_id_t),1, fd);
			graph_info.read(fd);

			insert(std::pair<graph_id_t, OCPTNGraphInfo>(graph_id, graph_info));
		}
	}

	void print(std::ostream& os){
		for(OCPTNGraphsInfos::iterator IT=this->begin(); IT!=this->end(); IT++){
			os<<IT->first<<":: occ= "<<IT->second.path_occurrence<<"\n";
		}
	}
};

class OCPTreeNode{
public:
	node_label_t label;
	OCPTreeNode* parent;
	OCPTreeNode* first_child;
	OCPTreeNode* last_child;
	OCPTreeNode* next;
	node_label_t child_count;
	bool is_special;
	OCPTNGraphsInfos gsinfos;

	OCPTreeNode(node_label_t _label, OCPTreeNode* _parent){
		this->label=_label;
		this->parent=_parent;
		this->next=NULL;
		this->first_child=NULL;
		this->last_child=NULL;
		this->child_count=0;
		this->is_special=false;
	}

	OCPTreeNode* add_child(node_label_t _label){
		if(this->first_child){
			if((this->first_child)->label > _label){
				OCPTreeNode* n=this->first_child;
				this->first_child=new OCPTreeNode(_label, this);
				(this->first_child)->next=n;
				(this->child_count)++;
				return this->first_child;
			}
			else if((this->first_child)->label == _label){
				return this->first_child;
			}

			else if((this->last_child)->label < _label){
				(this->last_child)->next=new OCPTreeNode(_label, this);
				this->last_child=(this->last_child)->next;
				(this->child_count)++;
				return this->last_child;
			}
			else if((this->last_child)->label == _label){
				return this->last_child;
			}

			else{
				OCPTreeNode* c=this->first_child;
				while(c->next!=NULL && (c->next)->label<=_label)
					c=c->next;

				if(c->label == _label){
					return c;
				}
				else{
					OCPTreeNode* n=c->next;
					c->next=new OCPTreeNode(_label, this);
					(c->next)->next=n;
					if(c->next->next==NULL)
						this->last_child=c->next;
					(this->child_count)++;
					return c->next;
				}
			}
		}
		else{
			this->first_child=new OCPTreeNode(_label, this);;
			this->last_child=this->first_child;
			(this->child_count)++;
			return this->first_child;
		}
	};

	OCPTreeNode* add_child_on_tail(node_label_t _label){
		if(this->first_child){
			if((this->last_child)->label > _label){
				return add_child(_label);
			}
			else if((this->last_child)->label < _label){
				(this->last_child)->next=new OCPTreeNode(_label, this);
				this->last_child=(this->last_child)->next;
				(this->child_count)++;
				return this->last_child;
			}
			else if((this->last_child)->label == _label){
				return this->last_child;
			}
		}
		else{
			this->first_child=new OCPTreeNode(_label, this);
			this->last_child=this->first_child;
			(this->child_count)++;
			return this->first_child;
		}
		return NULL;
	}

	void add_child_on_tail(OCPTreeNode* c){
		c->parent = this;
		if(this->last_child){
			this->last_child->next = c;
			this->last_child = this->last_child->next;
		}
		else{
			this->first_child = c;
			this->last_child = c;
		}
	}


	OCPTreeNode* get_child_by_label(node_label_t _label){
		OCPTreeNode* n=NULL;
		if(this->first_child!=NULL){
			if(_label==this->first_child->label)
				n=this->first_child;
			else if(_label==this->last_child->label)
				n=this->last_child;
			else if((_label>this->first_child->label)&&(_label<this->last_child->label)){
				OCPTreeNode* c=this->first_child->next;
				while(c!=NULL){
					if(_label==c->label){
						n=c;
						c=NULL;
					}
					else c=c->next;
				}
			}
		}
		return n;
	};

	void print_path(){
		OCPTreeNode* n = this;
		while(n!=NULL){
			std::cout<<n->label<<".";
			n=n->parent;
		}
	}
	void print_path(std::ostream& os){
		OCPTreeNode* n = this;
		while(n!=NULL){
			os<<n->label;
			n=n->parent;
		}
	}

//	friend std::istream& operator>>(std::istream& is, OCPTreeNode& n){
//		is.read((char*)&(n.label), sizeof(node_label_t));
//		is.read((char*)&(n.child_count), sizeof(size_t));
//		is>>n.gsinfos;
//		return is;
//	}
//	friend std::ostream& operator<<(std::ostream& os, OCPTreeNode& n){
//		os.write((char*)&(n.label), sizeof(node_label_t));
//		os.write((char*)&(n.child_count), sizeof(size_t));
//		os<<n.gsinfos;
//		return os;
//	}

	void write(FILE *fd){
		fwrite(&label, sizeof(node_label_t), 1, fd);
		fwrite(&child_count, sizeof(node_label_t), 1, fd);
		gsinfos.write(fd);
	}


	void read(FILE* fd){
		fread(&label, sizeof(node_label_t), 1, fd);
		fread(&child_count, sizeof(node_label_t), 1, fd);
		gsinfos.read(fd);
	}
};

}


#endif /* OCPTREENODE_H_ */
