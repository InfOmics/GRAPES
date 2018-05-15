/*
 * MatchingRunner.h
 *
 *  Created on: Dec 2, 2010
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

#ifndef MATCHINGRUNNER_H_
#define MATCHINGRUNNER_H_

#include "size_t.h"
#include "typedefs.h"

#include "Options.h"

#include "MatchingManager.h"
#include "MatchingThread.h"

#include "AttributeComparator.h"
#include "MatchListener.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

namespace GRAPESLib{

class MatchRunner{

	MatchingManager& _mman;
	thread_id_t _n_threads;
	node_id_t _query_size;

	MPRINT_OPTIONS mprint_opt;

public:

	MatchRunner(	MatchingManager& mman,
					thread_id_t n_threads,
					node_id_t query_size,
					MPRINT_OPTIONS _mprint_opt)
				: 	_mman(mman), mprint_opt(_mprint_opt){
		_n_threads = n_threads;
		_query_size = query_size;
	}

	void runInitPhase(){
		pthread_t bmatchingPThreads[_n_threads];
		int rc;
		for(thread_id_t i=0;i<_n_threads;i++){
			InitMatchingThread* mt = new InitMatchingThread(i, _mman, _query_size, *(_mman.edgeComparator.clone()), _n_threads);
			rc = pthread_create(&bmatchingPThreads[i], NULL, InitMatchingThread::run, (void*)mt);
			if(rc){
				printf("ERROR; return code from pthread_create() is %d\n", rc);
				exit(-1);
			}
		}
		for(thread_id_t i=0;i<_n_threads;i++){
			rc = pthread_join(bmatchingPThreads[i], NULL);
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
		}
	}

	void runMatch(	std::map<graph_id_t, ReferenceGraph*>& graphs,
					ARGEdit& query){

		pthread_t matchingPThreads[_n_threads];
		std::ofstream outs[_n_threads];
		std::string pname = "thread";

		if(mprint_opt == MPRINT_OPT_FILE){
			for(thread_id_t i=0;i<_n_threads;i++)
				outs[i].open(((pname+((char)('0'+i)))+".matches").c_str(),std::ios::out);
		}


		int rc;
		for(thread_id_t i=0;i<_n_threads;i++){

			MatchListener* mlistener;
			if(mprint_opt == MPRINT_OPT_NO)
				mlistener = new EmptyMatchListener();
			else if(mprint_opt == MPRINT_OPT_CONSOLE)
				mlistener = new ConsoleMatchListener();
			else
				mlistener = new FileMatchListener(outs[i]);


			MatchingThread* mt = new MatchingThread(_mman,
													i,
													outs[i],
													query,
													*(mlistener),
													*(_mman.edgeComparator.clone()),
													mprint_opt);

			rc = pthread_create(&matchingPThreads[i], NULL, MatchingThread::run, (void*)mt);
			if(rc){
				printf("ERROR; return code from pthread_create() is %d\n", rc);
				exit(-1);
			}
		}
		for(int i=0;i<_n_threads;i++){
			rc = pthread_join(matchingPThreads[i], NULL);
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
		}

		if(mprint_opt == MPRINT_OPT_FILE){
			for(int i=0;i<_n_threads;i++){
				outs[i].flush();
				outs[i].close();
			}
		}
	}
};

}


#endif /* MATCHINGRUNNER_H_ */
