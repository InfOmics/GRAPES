/*
 * BuilderThread.h
 *
 *  Created on: Nov 9, 2010
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

#ifndef BUILDERTHREAD_H_
#define BUILDERTHREAD_H_

#include <map>
#include <set>

#include "size_t.h"
#include "typedefs.h"

#include "Graph.h"
#include "OCPTree.h"
#include "BuildManager.h"
#include "GraphVisit.h"
#include "GraphPathListener.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "timer.h"


namespace GRAPESLib{

class BuilderThread{
public:
	thread_id_t _id;
	BuildManager& _buildman;
	DFSGraphVisitor& _gvisitor;

	BuilderThread(thread_id_t id, BuildManager& buildman, DFSGraphVisitor& gvisitor)
		: _id(id), _buildman(buildman), _gvisitor(gvisitor){
	}

	static void* run(void* argsptr){
		BuilderThread* bt = (BuilderThread*)argsptr;
		BuildJob* job;

		while(bt->_buildman.syncGetAJob(bt->_id, &job)){

			bt->_gvisitor.plistener.index_tree = &(job->_tree);
			bt->_gvisitor.plistener.graph_id = (job->_graph).id;
			for(node_id_set_t::iterator n_IT = job->_n_ids.begin(); n_IT!= job->_n_ids.end(); n_IT++){
				bt->_gvisitor.run(job->_graph, job->_graph.nodes[*n_IT], bt->_buildman._lp);
			}
			bt->_buildman.syncFinishJob(bt->_id, job);
		}

		pthread_exit(NULL);
	}
};

}

#endif /* BUILDERTHREAD_H_ */
