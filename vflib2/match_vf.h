#ifndef MATCHVF_H
#define MATCHVF_H

#include <iostream>

#include "VFlibrary.h"

#include <vf2_mono_state.h>
#include <vf2_sub_state.h>
#include <match.h>

#include "timer.h"

#include <fstream>

enum MatchType {MT_ISO, MT_SUB, MT_MONO};

long matchcount = 0;

class my_visitor_data_t{
public:
	std::ostream& os;
	int gid;
	my_visitor_data_t(std::ostream& _os, int _gid) : os(_os), gid(_gid){
	}
};

bool my_visitor3(int n, node_id ni1[], node_id ni2[], void* usr_data){
	matchcount++;
	my_visitor_data_t* vis = (my_visitor_data_t*)usr_data;
	vis->os<<vis->gid<<":{";
	for(int i=0;i<n;i++){
		vis->os<<"("<<ni1[i]<<","<<ni2[i]<<")";
		if(i!=n-1){
			vis->os<<",";
		}
	}
	vis->os<<"}\n";
	return false;
};

bool my_visitor(int n, node_id ni1[], node_id ni2[], void* usr_data){
	matchcount++;
	std::cout<<"{";
	for(int i=0;i<n;i++){
		std::cout<<"("<<ni1[i]<<","<<ni2[i]<<")";
		if(i!=n-1)
			std::cout<<",";
	}
	std::cout<<"}\n";
	return false;
};

bool my_visitor2(int n, node_id ni1[], node_id ni2[], void* usr_data){
	matchcount++;
	return false;
};

void matchvf(	MyARGraph* g1,
				MyARGraph* g2,
				int gid,
				std::ostream& os,
				MatchType mtype){
	State* s0;

	switch(mtype){
		case MT_ISO:
			s0 = new VF2SubState(g1,g2);
			if(g1->NodeCount() != g2->NodeCount())
				return;
			break;
		case MT_SUB:
			s0 = new VF2SubState(g1,g2);
			break;
		case MT_MONO:
			s0 = new VF2MonoState(g1,g2);
			break;
	}


	//match(s0, my_visitor, new my_visitor_data_t(os,gid));
	match(s0, my_visitor2, new my_visitor_data_t(os,gid));
	//match(s0, my_visitor3, new my_visitor_data_t(os,gid));
};

#endif
