/*
 * AttributeComparator.h
 *
 *  Created on: Aug 3, 2012
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

#ifndef ATTRIBUTECOMPARATOR_H_
#define ATTRIBUTECOMPARATOR_H_

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "size_t.h"
#include "typedefs.h"

#include "argraph.h" //VF2


namespace GRAPESLib{




class AttributeComparator{
public:
	virtual ~AttributeComparator(){};
	virtual bool compare(void* attr1, void* attr2)=0;
	virtual int compareint(void* attr1, void* attr2)=0;

	virtual AttributeComparator* clone() = 0;
};





class DefaultAttrComparator: public AttributeComparator{
public:
	DefaultAttrComparator(){};
	virtual bool compare(void* attr1, void* attr2){
		return true;
	};
	virtual int compareint(void* attr1, void* attr2){
		return 0;
	};


	virtual AttributeComparator* clone(){
		return new DefaultAttrComparator();
	}
};


class StringAttrComparator: public AttributeComparator{
public:
	StringAttrComparator(){};
	virtual bool compare(void* attr1, void* attr2){
		std::string* a=(std::string*)attr1;
		std::string* b=(std::string*)attr2;
		return (a->compare(*b))==0;
	};
	virtual int compareint(void* attr1, void* attr2){
		std::string* a=(std::string*)attr1;
		std::string* b=(std::string*)attr2;
		return a->compare(*b);
	};

	virtual AttributeComparator* clone(){
		return new StringAttrComparator();
	}
};

class IntAttrComparator: public AttributeComparator{
public:
	IntAttrComparator(){};
	virtual bool compare(void* attr1, void* attr2){
		int* a=(int*)attr1;
		int* b=(int*)attr2;
		return ((*a)-(*b))==0;
	};
	virtual int compareint(void* attr1, void* attr2){
		int* a=(int*)attr1;
		int* b=(int*)attr2;
		return (*a)-(*b);
	};

	virtual AttributeComparator* clone(){
		return new IntAttrComparator();
	}
};



class VF2DSAttrComparator : public AttrComparator{
public:
	sbitset* domains;
//	sbitset* coco_mask;

	VF2DSAttrComparator(){}

//	VF2DSAttrComparator(sbitset* _domains, sbitset* _coco_mask)
//			: domains(_domains), coco_mask(_coco_mask){
//	}

	~VF2DSAttrComparator(){
	}

	virtual bool compatible(void* pa, void* pb){
//		std::cout<<"comp("<<(*((int*)pa))<<","<<(*((int*)pb))<<"):";
//		if(coco_mask->get(*((int*)pb)))
//			std::cout<<"mask[true] ";
//		else
//			std::cout<<"mask[false] ";
//		if(domains[*((int*)pa)].get(*((int*)pb)))
//			std::cout<<"ds[true]";
//		else
//			std::cout<<"ds[false]";
//		std::cout<<"\n";

		return (  domains[*((int*)pa)].get(*((int*)pb))  );
//		return (  domains[*((int*)pa)].get(*((int*)pb))  );
//		return (  coco_mask.get(*((int*)pb)));
//		return true;


//		GGSXARGraphPointQ* apa = (GGSXARGraphPointQ*)pa;
//		GGSXARGraphPoint* apb = (GGSXARGraphPoint*)pb;
//
//		if(apa->label == apb->label){
//			return (apa->cands).get(apb->id) == true;
//		}
//		return false;
	};
};

}


#endif /* ATTRIBUTECOMPARATOR_H_ */
