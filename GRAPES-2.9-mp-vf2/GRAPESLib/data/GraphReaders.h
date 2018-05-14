/*
 * textdb_driver.h
 *
 *  Created on: Nov 2, 2010
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

#ifndef GRAPHREADERS_H_
#define GRAPHREADERS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "LabelMap.h"
#include <queue>

namespace GRAPESLib{

class GraphReader{
public:
	LabelMap& _labelMap;
	GraphReader(LabelMap& labelMap) : _labelMap(labelMap){}

	virtual bool readGraph(Graph& g) = 0;
	bool readGraph(Graph& g, graph_id_t id){
		if(readGraph(g)){
			g.id = id;
			return true;
		}
		return false;
	}
	virtual bool gotoGraph(graph_id_t id) =0;
};

class GraphsQueueReader : public GraphReader{
public:
	std::queue<Graph>& _queue;
	graph_id_t c_id;

	GraphsQueueReader(LabelMap& labelMap, std::queue<Graph>& queue)
			: GraphReader(labelMap), _queue(queue){
		c_id=0;
	}
	virtual bool readGraph(Graph& g){
		if(!_queue.empty()){
			g = _queue.front();
			_queue.pop();
			c_id++;
			return true;
		}
		return false;
	}
	virtual bool gotoGraph(graph_id_t id){
		while(c_id<id && !_queue.empty()){
			_queue.pop();
			c_id++;
		}
		if(!_queue.empty()){
			return true;
		}
		return false;
	}
};


class GraphReader_gff : public GraphReader{
public:
	enum State {
		AttendGraph,
		LoadNumNodes,
		LoadNodes,
		LoadNumEdges,
		LoadEdges,
		End
	};
	enum EState{
		FirstEdge,
		SecondEdge
	};


	std::ifstream& in;
	graph_id_t c_id;

	bool direct;

	GraphReader_gff(LabelMap& labelMap, std::ifstream& _in)
			: GraphReader(labelMap), in(_in){
		c_id = 0;
		direct = false;
	}

	virtual bool gotoGraph(graph_id_t id){
		if(!in.is_open() || in.eof() || in.bad())
			return false;

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		u_size_t itemcount=0, i=0;
		node_id_t firste, seconde;

		while(c_id<id && !in.eof() && state!=End){
			in>>s;
			if(s=="")
				return false;
			switch (state){
				case AttendGraph:{
					if(s[0]=='#'){
						state=LoadNumNodes;
					}
					else{
						return false;
					}
					break;
				}
				case LoadNumNodes:{
					itemcount=atoi(s.c_str());
					if(itemcount==0)
						return false;
					i=0;
					state=LoadNodes;
					break;
				}
				case LoadNodes:{
					i++;
					if(i>=itemcount)
						state=LoadNumEdges;

					break;
				}
				case LoadNumEdges:{
					//itemcount=stringto<int>(s);
					itemcount=atoi(s.c_str()) *2;
					i=0;
					state=LoadEdges;
					break;
				}
				case LoadEdges:{
					i++;
					if(i>=itemcount){
						c_id++;
						if(c_id!=id)
							state=AttendGraph;
						else
							state=End;
					}
					break;
				}
			}
		}
		if(c_id==id && !in.eof())
			return true;
		return false;
	}

	virtual bool readGraph(Graph& g){
		if(!in.is_open() || in.eof() || in.bad()){
//			if(!in.is_open())
//				std::cout<<"@GraphReader_gff.readGraph(): error: !in.isOpen()\n";
//			if(in.eof())
//				std::cout<<"@GraphReader_gff.readGraph(): error: in.eof()\n";
//			if(in.bad())
//				std::cout<<"@GraphReader_gff.readGraph(): error: in.bad()\n";
			return false;
		}

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		u_size_t itemcount=0, i=0;
		node_id_t firste, seconde;

		while(!in.eof() && state!=End){
			in>>s;
//std::cout<<"s:"<<s<<"\n";
			if(s=="")
				return false;
			switch (state){
				case AttendGraph:{
					if(s[0]=='#'){
						state=LoadNumNodes;
					}
					else{
						return false;
					}
					break;
				}
				case LoadNumNodes:{
					itemcount=atoi(s.c_str());
					if(itemcount==0)
						return false;
					g.setSize((size_t)itemcount);
					i=0;
					state=LoadNodes;
					break;
				}
				case LoadNodes:{
					g.insertNode(i, _labelMap.getLabel(s));
					i++;
					if(i>=itemcount)
						state=LoadNumEdges;

					break;
				}
				case LoadNumEdges:{
					itemcount=atoi(s.c_str());
					i=0;
					state=LoadEdges;
					break;
				}
				case LoadEdges:{
					if(i>=itemcount){
						state=End;
					}
					else{
						if(estate==FirstEdge){
							firste=atoi(s.c_str());
							estate=SecondEdge;
						}
						else{
							seconde=atoi(s.c_str());
							g.insertEdge(firste, seconde);
							if(!direct)
								g.insertEdge(seconde, firste);
							estate=FirstEdge;
							i++;
						}
						if(i>=itemcount){
							state=End;
						}
					}
					break;
				}
			}
		}
		g.direct = direct;
		c_id++;
		return true;
	}
};


//std::string
//intToString(int i){
//	std::stringstream ss;
//	ss<< i;
//	return ss.str();
//};

}

#endif /* TEXTDB_DRIVER_H_ */
