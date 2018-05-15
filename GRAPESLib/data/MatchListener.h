/*
 * MatchListener.h
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

#ifndef MATCHLISTENER_H_
#define MATCHLISTENER_H_

namespace GRAPESLib{

class MatchListener{
public:
	int gid;
	long matchcount;
	MatchListener(){
		matchcount = 0;
		gid = 0;
	}
	virtual ~MatchListener(){};
	virtual void match(int n, node_id ni1[], node_id ni2[])=0;
};


class EmptyMatchListener : public MatchListener {
public:
	EmptyMatchListener() : MatchListener(){
	}
	virtual void match(int n, node_id ni1[], node_id ni2[]){
		matchcount++;
	};
};

class ConsoleMatchListener : public MatchListener {
public:
	ConsoleMatchListener() : MatchListener(){
	}
	//virtual void match(int n, int* qIDs, int* rIDs){
	virtual void match(int n, node_id ni1[], node_id ni2[]){
		matchcount++;
		std::cout<< gid<<"{";
		for(int i=0; i<n; i++){
			std::cout<< "("<< ni1[i] <<","<< ni2[i] <<")";
		}
		std::cout<< "}\n";
	}
};

class FileMatchListener : public MatchListener {
	std::ofstream& os;
public:
	FileMatchListener(std::ofstream& _os) : MatchListener(), os(_os){
	}
	//virtual void match(int n, int* qIDs, int* rIDs){
	virtual void match(int n, node_id ni1[], node_id ni2[]){
		matchcount++;
		os<< gid<<"{";
		for(int i=0; i<n; i++){
			os<< "("<< ni1[i] <<","<< ni2[i] <<")";
		}
		os<< "}\n";
	}
};

}


#endif /* MATCHLISTENER_H_ */
