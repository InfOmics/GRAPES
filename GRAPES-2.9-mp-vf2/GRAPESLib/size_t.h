/*
 * gmsize_t.h
 *
 *  Created on: Dec 17, 2011
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

#ifndef GMSIZE_T_H_
#define GMSIZE_T_H_

#include <stdint.h>
#include "limits.h"



typedef uint32_t 	u_size_t;
typedef int32_t 	s_size_t;

typedef uint64_t 	u_lsize_t;
typedef int64_t 	s_lsize_t;

typedef uint16_t 	u_ssize_t;
typedef int16_t 	s_ssize_t;




typedef u_size_t	node_id_t;
typedef u_size_t	graph_id_t;
typedef u_size_t	node_label_t;
typedef u_size_t	u_path_id_t;
typedef int 		thread_id_t;

#define graph_id_t_MAX UINT_MAX
#define node_id_t_MAX UINT_MAX

#endif /* GMSIZE_T_H_ */
