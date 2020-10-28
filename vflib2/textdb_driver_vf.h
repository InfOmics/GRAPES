#ifndef TEXTDBDRIVERVF_H
#define TEXTDBDRIVERVF_H

#include <argraph.h>
#include <argedit.h>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>

#include "VFlibrary.h"

enum GraphFileType {GFT_GFF, GFT_VFU, GFT_VFL, GFT_FSG, GFT_GFD};

using namespace std;

bool read_graph_vf_gff(std::ifstream& in, ARGEdit& ed){
	enum State {
		AttendGraph,
		LoadNumNodes,
		LoadNodes,
		LoadNumEdges,
		LoadEdges,
		End
	} state = AttendGraph;
	enum EState{
		FirstEdge,
		SecondEdge
	} estate=FirstEdge;
	
	std::map<int, std::set<int> > already;

	string s;
	int itemcount=0, i=0;
	int firste, seconde;
	while(!in.eof() && state!=End){
		in>>s;
		//std::cout<<"-"<<s<<"\n";
		if(s=="")
			return false;
		switch (state){
			case AttendGraph:{
				if(s[0]=='#'){
//std::cout<<"graph name:"<<s<<"\n";
					//_graph.name=s;
					state=LoadNumNodes;
				}
				else{
					return false;
				}
				break;
			}
			case LoadNumNodes:{
				//itemcount=stringto<int>(s);
				itemcount=atoi(s.c_str());
				//assert(itemcount);
				if(itemcount==0)
					return false;
				//std::cout<<"nodes count:"<<itemcount<<"\n";
				//_graph.set_num_nodes(itemcount);
				i=0;
				state=LoadNodes;
				break;
			}
			case LoadNodes:{
				//nodelabel in s
				//std::cout<<i<<":"<<s<<"\n";
				ed.InsertNode(new string(s));
				i++;
				if(i>=itemcount)
					state=LoadNumEdges;
				
				break;
			}
			case LoadNumEdges:{
				//itemcount=stringto<int>(s);
				itemcount=atoi(s.c_str());
				//assert(itemcount);
				//if(itemcount==0)
				//	return false;
				//std::cout<<"edges count:"<<itemcount<<"\n";
				i=0;
				state=LoadEdges;
				break;
			}
			case LoadEdges:{
				if(estate==FirstEdge){
					firste=atoi(s.c_str());
					//std::cout<<firste<<"/";
					estate=SecondEdge;
				}
				else{
					seconde=atoi(s.c_str());
					//std::cout<<seconde<<"\n";

					if(already[firste].find(seconde) ==  already[firste].end()){
						already[firste].insert(seconde);
						ed.InsertEdge(firste, seconde, NULL);
					}
					if(already[seconde].find(firste) ==  already[seconde].end()){
						already[seconde].insert(firste);
						ed.InsertEdge(seconde, firste, NULL);
					}
					estate=FirstEdge;
					i++;
					//std::cout<<"i:"<<i<<"\n";
				}
				if(i>=itemcount){
					//std::cout<<"end\n";
					state=End;
				}
				break;
			}
		}
	}

	return true;
};


bool read_graph_vf_gfd(std::ifstream& in, ARGEdit& ed){
	enum State {
		AttendGraph,
		LoadNumNodes,
		LoadNodes,
		LoadNumEdges,
		LoadEdges,
		End
	} state = AttendGraph;
	enum EState{
		FirstEdge,
		SecondEdge
	} estate=FirstEdge;

	std::map<int, std::set<int> > already;

	string s;
	int itemcount=0, i=0;
	int firste, seconde;
	while(!in.eof() && state!=End){
		in>>s;
		//std::cout<<"-"<<s<<"\n";
		if(s=="")
			return false;
		switch (state){
			case AttendGraph:{
				if(s[0]=='#'){
//std::cout<<"graph name:"<<s<<"\n";
					//_graph.name=s;
					state=LoadNumNodes;
				}
				else{
					return false;
				}
				break;
			}
			case LoadNumNodes:{
				//itemcount=stringto<int>(s);
				itemcount=atoi(s.c_str());
				//assert(itemcount);
				if(itemcount==0)
					return false;
				//std::cout<<"nodes count:"<<itemcount<<"\n";
				//_graph.set_num_nodes(itemcount);
				i=0;
				state=LoadNodes;
				break;
			}
			case LoadNodes:{
				//nodelabel in s
				//std::cout<<i<<":"<<s<<"\n";
				ed.InsertNode(new string(s));
				i++;
				if(i>=itemcount)
					state=LoadNumEdges;

				break;
			}
			case LoadNumEdges:{
				//itemcount=stringto<int>(s);
				itemcount=atoi(s.c_str());
				//assert(itemcount);
				//if(itemcount==0)
				//	return false;
				//std::cout<<"edges count:"<<itemcount<<"\n";
				i=0;
				state=LoadEdges;
				break;
			}
			case LoadEdges:{
				if(estate==FirstEdge){
					firste=atoi(s.c_str());
					//std::cout<<firste<<"/";
					estate=SecondEdge;
				}
				else{
					seconde=atoi(s.c_str());
					//std::cout<<seconde<<"\n";

					if(already[firste].find(seconde) ==  already[firste].end()){
						already[firste].insert(seconde);
						ed.InsertEdge(firste, seconde, NULL);
					}
//					if(already[seconde].find(firste) ==  already[seconde].end()){
//						already[seconde].insert(firste);
//						ed.InsertEdge(seconde, firste, NULL);
//					}
					estate=FirstEdge;
					i++;
					//std::cout<<"i:"<<i<<"\n";
				}
				if(i>=itemcount){
					//std::cout<<"end\n";
					state=End;
				}
				break;
			}
		}
	}

	return true;
};



bool read_graph_vf_fsg(std::ifstream& in, ARGEdit& ed){
	enum State{
		Start,		//0
		AttendIns,	//1
		GraphMark,	//2
		GraphId,	//3
		Node,		//4
		NodeId,		//5
		NodeLabel,	//6
		Edge,		//7
		EdgeSource,	//8
		EdgeTarget,	//9
		EdgeLabel,	//10
		End		//11
	} state=Start;
	
	if(in.eof())
		return false;
	
	string s;
	int esource, etarget;
	while(!in.eof() && state!=End){
		in>>s;
		if(s==""){
			state=End;
			//return false;
		}
		switch (state) {
			case Start:{
				if(s[0]=='t')
					state=GraphMark;
				else if(s[0]=='v')
					state=NodeId;
				else if(s[0]=='e')
					state=EdgeSource;
				else if(s[0]=='#')
					state=GraphId;
				else
					return false;
				break;
			}
			case AttendIns:{
				if(s[0]=='v')
					state=NodeId;
				else if(s[0]=='e')
					state=EdgeSource;
				else if(s[0]=='t')
					state=End;
				else
					return false;
				break;
			}
			case GraphMark:{
				state=GraphId;
				break;
			}
			case GraphId:{
				//state=Node;
				state=AttendIns;
				break;
			}
			case Node:{
				state=NodeId;
				break;
			}
			case NodeId:{
				state=NodeLabel;
				break;
			}
			case NodeLabel:{
				//std::cout<<"node: "<<s<<"\n";
				ed.InsertNode(new string(s));
				state=AttendIns;
				break;
			}
			case Edge:{
				state=EdgeSource;
				break;
			}
			case EdgeSource:{
				esource=atoi(s.c_str());
				state=EdgeTarget;
				break;
			}
			case EdgeTarget:{
				etarget=atoi(s.c_str());
				state=EdgeLabel;
				break;
			}
			case EdgeLabel:{
				//std::cout<<"edge: "<<esource<<"-"<<etarget<<":"<<s<<"\n";
				ed.InsertEdge(esource, etarget, new string(s));
				//ed.InsertEdge(etarget, esource, new string(s));
				state=AttendIns;
				break;
			}
		}
		if(in.eof())
			state=End;
	}
	return true;
}

unsigned short vf_read_word_unlabeled(std::ifstream& in){
	unsigned char b1, b2;
	b1=in.get();
	b2=in.get();
	//std::cout<<(unsigned short)(b1 | b2)<<".";
	//return b1 | b2;
	return b1 | (b2<<8);
};

bool read_graph_vf_unlabeled(std::ifstream& in, ARGEdit& ed){
	int nodes;
	int edges;
	int target;
	int i, j;
	
	if(!in.good())return false;
	nodes=vf_read_word_unlabeled(in);
	
	for(int i=0;i<nodes;i++){
		if(!in.good())return false;
		ed.InsertNode(NULL);
		
		edges=vf_read_word_unlabeled(in);
		
		for(int j=0;j<edges;j++){
			if(!in.good())return false;
			target=vf_read_word_unlabeled(in);
			
			ed.InsertEdge(i, target, NULL);
			//ed.InsertEdge(target, i, NULL);
		}
	}
	return true;
};

unsigned short vf_read_word_labeled(std::ifstream& in){
	unsigned char b1, b2;
	b1=in.get();
	b2=in.get();
	//std::cout<<(unsigned short)(b1 | (b2<<8))<<".";
	return b1 | (b2<<8);
};

bool read_graph_vf_labeled(std::ifstream& in, ARGEdit& ed){
	int nodes;
	int edges;
	int target;
	int *attribute;
	
	if(!in.good())return false;
	nodes=vf_read_word_labeled(in);
	
	for(int i=0;i<nodes;i++){
		if(!in.good())return false;
		attribute=new int(vf_read_word_labeled(in));
		ed.InsertNode(attribute);
	}
	
	for(int i=0;i<nodes;i++){
		if(!in.good())return false;
		edges=vf_read_word_labeled(in);
		
		for(int j=0;j<edges;j++){
			if(!in.good())return false;
			target=vf_read_word_labeled(in);
			attribute=new int(vf_read_word_labeled(in));
			ed.InsertEdge(i, target, attribute);
		}
	}
	return true;
};

MyARGraph* read_text_graph_vf(std::ifstream& in, GraphFileType _graphtype){
	ARGEdit* ed=new ARGEdit();
	
	if(_graphtype== GFT_GFF){
		if(in.is_open()){
			if(read_graph_vf_gff(in, *ed)){
				MyARGraph* g=new MyARGraph(ed);
				return g;
			}
		}
	}
	else if(_graphtype== GFT_VFU){
		if(in.is_open()){
			if(read_graph_vf_unlabeled(in, *ed)){
				MyARGraph* g=new MyARGraph(ed);
				return g;
			}
		}
	}
	else if(_graphtype== GFT_VFL){;
		if(in.is_open()){
			if(read_graph_vf_labeled(in, *ed)){
				MyARGraph* g=new MyARGraph(ed);
				return g;
			}
		}
	}
	else if(_graphtype== GFT_FSG){;
		if(in.is_open()){
			if(read_graph_vf_fsg(in, *ed)){
				MyARGraph* g=new MyARGraph(ed);
				return g;
			}
		}
	}
	else if(_graphtype== GFT_GFD){;
		if(in.is_open()){
			if(read_graph_vf_gfd(in, *ed)){
				MyARGraph* g=new MyARGraph(ed);
				return g;
			}
		}
	}
	
	return NULL;
};

void open_ifstream(std::ifstream& in, const std::string& input_file, GraphFileType _graphtype){
	switch(_graphtype){
	case GFT_GFF:
	case GFT_FSG:
	case GFT_GFD:
		in.open(input_file.c_str(), ios_base::in);
		break;
	case GFT_VFU:
	case GFT_VFL:
		in.open(input_file.c_str(), ios_base::in | ios_base::binary );
		break;
	}
};

MyARGraph* read_text_graph_vf(const std::string& input_file, GraphFileType _graphtype){
	std::ifstream in;
//	if(_graphtype== GFT_GFF){
//		in.open(input_file.c_str(), ios_base::in);
//	}
//	else if(_graphtype== GFT_VFU){
//		in.open(input_file.c_str(), ios_base::in | ios_base::binary );
//	}
//	else if(_graphtype== GFT_VFL){
//		in.open(input_file.c_str(), ios_base::in | ios_base::binary );
//	}
//	else if(_graphtype== GFT_FSG){
//		in.open(input_file.c_str(), ios_base::in);
//	}
	open_ifstream(in, input_file, _graphtype);
	
	return read_text_graph_vf(in, _graphtype);
};

//MyARGraph* read_text_db_vf(std::ifstream& in, GraphFileType _graphtype){
//	if(in.is_open()){
//		bool exists_graph=true;
//		if(!in.eof()){
//			ARGEdit ed;
//			exists_graph=read_text_graph_vf(in, ed, _graphtype);
//			if(exists_graph){
//				MyARGraph* g=new MyARGraph(&ed);
//				return g;
//			}
//		}
//	}
//	return NULL;
//};

//ARGEdit* read_texttemp_db_vf(const std::string& input_file){
//	std::ifstream in;
//	in.open(input_file.c_str(), ios_base::in);
//	if(in.is_open()){
//		bool exists_graph=true;
//		if(!in.eof()){
//			ARGEdit ed;
//			exists_graph=read_graph_vf_gff(in, ed);
//			if(exists_graph){
//				return &ed;
//			}
//		}
//	}
//	return NULL;
//};


#endif
