#ifndef VFLIBRARY_H
#define VFLIBRARY_H

//#include <textdb_driver_vf.h>
#include <iostream>
#include <string>

#include <argraph.h>
#include <argedit.h>

using namespace std;

//namespace VFlibrary{

typedef ARGraph<string,void> MyARGraph;

class VStringDestroyer: public AttrDestroyer{
public:
	virtual void destroy(void *p){
		delete p;
	};
};
class VStringComparator: public AttrComparator{
public:
	virtual bool compatible(void* pa, void* pb){
		string* a=(string*)pa;
		string* b=(string*)pb;
//		std::cout<<*a<<":"<<*b<<"\n";
		//string c="a";
		//return !(a->compare(*b));
		return (*a)==(*b);
	};
};


class VIntComparator: public AttrComparator{
public:
	virtual bool compatible(void* attr1, void* attr2){
		int* a=(int*)attr1;
		int* b=(int*)attr2;
		return !(((*a)-(*b))==0);
	};
};

//}

#endif
