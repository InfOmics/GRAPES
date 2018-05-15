/*
 * LabelMap.h
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

#ifndef LABELMAP_H_
#define LABELMAP_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"

namespace GRAPESLib{
//using namespace GGSXLib;

class LabelMap: public std::map<std::string, node_label_t>{
public:
	node_label_t getLabel(std::string s){
//		std::cout<<"LabelMap.getLabel("<<s<<")\n";

		LabelMap::iterator IT = this->find(s);
		if(IT == this->end()){
			node_label_t label = (node_label_t)this->size();
			this->insert(std::pair<std::string,node_label_t>(s,label));
			return label;
		}
		else{
			return IT->second;
		}
	}

//	friend std::istream& operator>>(std::istream& is, LabelMap& m){
//		size_t size;
//		is.read((char*)&size,sizeof(std::size_t));
//		for(int i=0;i<size;i++){
//			std::string s_label;
//			node_label_t n_label=0;
//			is>> s_label;
//			is.seekg(((int)is.tellg())+1);;
//			is.read((char*)&n_label, sizeof(node_label_t));
//			m.insert(std::pair<std::string, node_label_t>(s_label,n_label));
//		}
//		return is;
//	}
//	friend std::ostream& operator<<(std::ostream& os, LabelMap& m){
//		std::size_t size = m.size();
//		os.write((char*)&(size), sizeof(std::size_t));
//		for(LabelMap::iterator IT = m.begin(); IT!=m.end(); IT++){
//			os<< IT->first;
//			os<<std::endl;
//			os.write((char*)&(IT->second), sizeof(node_label_t));
//		}
//		return os;
//	}


	void write(FILE *fd){
		u_size_t msize = (u_size_t)(size());
		fwrite(&msize, sizeof(u_size_t), 1, fd);

//		std::cout<<"LabelMap.size = "<<msize<<"\n";
		char c = '\n';

		for(LabelMap::iterator IT = begin(); IT!=end(); IT++){
			IT->first.length();

			fprintf(fd, "%s", IT->first.c_str());
			fwrite(&c, sizeof(char), 1, fd);

			fwrite(&(IT->second), sizeof(node_label_t), 1, fd);

//			std::cout<<IT->first<<" => "<<IT->second<<"\n";
		}
	}


	void read(FILE* fd){
		u_size_t size = 0;
		fread(&size, sizeof(u_size_t), 1, fd);

//		std::cout<<"LabelMap.size = "<<size<<"\n";

		char str[256];

		for(u_size_t i=0; i<size; i++){
			fscanf(fd, "%s", str);
			std::string s_label(str);

			fseek(fd, ftell(fd) + 1, SEEK_SET);

			node_label_t n_label = 0;
			fread(&n_label, sizeof(node_label_t), 1, fd);

//			std::cout<<s_label<<" => "<<n_label<<"\n";

			insert(std::pair<std::string, node_label_t>(s_label,n_label));
		}
	}
};

}

#endif /* LABELMAP_H_ */
