/*
 * BuildManager.h
 *
 *  Created on: Nov 7, 2010
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

#ifndef BUILDRUNNER_H_
#define BUILDRUNNER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "OCPTree.h"
#include "GraphReaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "BuildManager.h"
#include "BuilderThread.h"

namespace GRAPESLib{

class BuildRunner{
	BuildManager& _bman;
	DFSGraphVisitor& _gvisitor;
	u_size_t _n_threads;


public:

	BuildRunner(BuildManager& bman,
				DFSGraphVisitor& gvisitor,
				thread_id_t n_threads = 4)
			: _bman(bman),
			  _gvisitor(gvisitor){
		_n_threads = n_threads;
	}

	void run(){
		pthread_t builderPThreads[_n_threads];

		int rc;
		for(thread_id_t i=0;i<_n_threads;i++){
			BuilderThread* bt = new BuilderThread(i, _bman, _gvisitor.clone());
			rc = pthread_create(&builderPThreads[i], NULL, BuilderThread::run, (void*)bt);
			if(rc){
				printf("ERROR; return code from pthread_create() is %d\n", rc);
				exit(-1);
			}
		}

		for(thread_id_t i=0;i<_n_threads;i++){
			rc = pthread_join(builderPThreads[i], NULL);
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
		}

		_bman.getFinalOCPtree();
	}
};

}

#endif /* BUILDMANAGER_H_ */
