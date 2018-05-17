# GRAPES

>A SMP querying system for efficient parallel subgraph isomorphism in databases of graphs.

Version: **2.9.mp.vf2, April, 2013**

<hr />

### Description
GRAPES is a querying system for parallel searching in databases of graphs, and single target graph, using symmetric multiprocessing (SMP) architectures. It implements a parallel version of well established graph searching algorithms providing efficient solutions for graphs indexing and matching.


GRAPES is developed in C++ under GNU\Linux using POSIX Threads programming and no further dependencies out of standard GNU C++ library. It can also be compiled to run under Windows and MAC OSx systems.


The GRAPES method consists of three main phases: indexing, filtering, and matching. 
In the indexing phase, features are extracted from target graphs and a database index is built off-line. A feature is a labeled path presents in a graph. GRAPES extracts all paths up to a fixed length (lp) and stores them in a compact trie structure. Moreover, starting nodes of such paths are also stored. Once the index is built, the system is able to find subgraph isomorphims between a query and all the target graphs. The filtering phase allows to a priori discard such database graphs which do not contain the query's features. It is also able to preliminarily recognize unmatching nodes, and entire regions, of database graphs. This useful behavior allows GRAPES to dial both with large database or single target graphs. Finally, an exact subgraph matching algorithm is run in parallel. 
See the related scientific paper for more details.

<hr />

### License

GRAPES is distributed under the MIT license. This means that it is free for both academic and commercial use. 
Note however that some third party components in GRAPES require that you reference certain works in scientific publications. 
You are free to link or use GRAPES inside source code of your own program. If do so, please reference (cite) GRAPES and this website. 
We appreciate bug fixes and would be happy to collaborate for improvements. <br>
[MIT License](https://raw.githubusercontent.com/GiugnoLab/GRAPES/master/LICENSE)

<hr />

### Citation
    "Giugno, R., Bonnici, V., Bombieri, N., Pulvirenti, A., Ferro, A., & Shasha, D. (2013). Grapes:
    A software for parallel searching on biological graphs targeting multi-core architectures.
    PloS one, 8(10), e76911."
   <span style="color:blue">[Online version](http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0076911) </span> <br>
   [Cited by](https://scholar.google.it/scholar?cites=4839872088400060775&as_sdt=2005&sciodt=0,5&hl=it) <br>
   ###### BibTeX format:
   
    "@article{giugno2013grapes,
    title={Grapes: A software for parallel searching on biological graphs targeting multi-core architectures},
    author={Giugno, Rosalba and Bonnici, Vincenzo and Bombieri, Nicola and Pulvirenti, Alfredo and Ferro, Alfredo and Shasha, Dennis},
    journal={PloS one},
    volume={8},
    number={10},
    pages={e76911},
    year={2013},
    publisher={Public Library of Science}
     }"
    
<hr />

### Usage

GRAPES is developed in C++ under GNU\Linux using POSIX Threads programming and no further dependencies out of standard GNU C++ library. It works on Unix and Mac OS X systems with G++ installed, and it can be compiled under Windows using Gygwin.

The executable *grapes* allows both to build the database index and to run a query.

##### Build Source Code

Executables are available only after building source code on your system.
 ```
 cd GRAPES-2.9.mp.vf2
 make clean 
 make -B
 ```
At the and of the build process,the executable grapes will be available.

##### Database Index Construction

Build the index of the given database of graphs.
```
./grapes NTHREADS -b -[gfu|gfd] db_file [-lp lp]
```
    

| Attribute | Description |
|-----------------------|-------------|
|**NTHREADS** |        number of parallel threads|
|**-[gfu  &#124; gfd]**| input file format <br> ***-gfu*** : undirected graphs file format; <br>   ***-gfd***  : directed graphs file format |
|**db_file** | textual graphs database file|
|**[-lp lp]**| OPTIONAL, specify feature paths length, namely the depth of the DFS which extract paths. lp must be greather than 1, eg -lp 3. Default value -lp 4.|


The indexing phase ever produces the *db_file.index.grapes* file in which the database index is stored.


Example: build the database index of the PCM dataset (undirected graphs) by 6 parallel threads
```
./grapes 6 -b -gfu pcms/db/pcms_all.200.gfu
```
The command creates the file *pcms/db/pcms_all.200.gfu.index.grapes* which will be automatically used on querying.

##### Querying
```
./grapes NTHREADS -f -[gfu|gfd] db_file query_file -[no|console|file] [-lp lp]
```
| Attribute | Description |
|-----------------------|-------------|
|**NTHREADS** |        number of parallel threads|
|**-[gfu  &#124; gfd]**| input file format <br> ***-gfu*** : undirected graphs file format; <br>   ***-gfd***  : directed graphs file format |
|**db_file** | textual graphs database file|
|**query_file** | textual query graph file. It must contain just one graph |
|**-[no &#124; console &#124; file]** | print found matches (sub-isomorphisms) <br> ***-no*** :do not print; <br> ***-console*** :print matches on screen; <br> ***-file*** :print matches on files. Each thread prints found matches in a different file
|**[-lp lp]** | OPTIONAL ONLY IF the database index was built with a lp value different from 4,specify feature paths length, namely the depth of the DFS which extract paths. lp must be greater than 1, eg -lp 3. Default value -lp 4.|

Example: search a query graphs (undirected) inside the PCM dataset (undirected) by 6 parallel threads and print matches on screen
```
>./grapes 6 -b -gfu pcms/db/pcms_all.200.gfu pcms/queries/query_32_1ARO.cm.L.gfu -console
```
<span style="color:red">ATTENTION:</span> before run a query, the database index must have been computed by the command *grapes -b* and the resultant *.index.grapes* file must be maintained in the same directory of the database textual file.

<hr />

### Formats

##### Input Formats

Graphs are stored in text files containing one or more items.
The current input format allows the description of undirected -gfu or directed -gfd graphs with labels on nodes.
>\#[graph_name] <br>
[number of nodes] <br>
[label_of_first_node] <br> 
[label_of_second_node] <br>
... <br>
[label_of_last_node] <br> 
[number of edges] <br>
[node id] [node id] <br>
[node id] [node id] <br>
...

GRAPES assigns ids to nodes following the order in which they are written in the input file, starting from 0.

<span style="color:red">ATTENTION:</span> *[graph_name]* and labels can not contain blank characters (spaces).
Labels are case sensitive.

Inside a graphs database file, graphs IDs are assigned following the order in which they are written into the input file.

An example of input file is the following one:

>#my_graph <br>
4 <br>
A <br>
B <br>
C <br>
Br <br>
5 <br>
0 1 <br>
2 1 <br>
2 3 <br>
0 3 <br>
0 2

Since GRAPES does not allow multigraphs, it ignores all duplicated edges without reporting any error.
In the case of undirected graphs, 1 0 is a duplicate of the edge 0 1.

##### Output Format

GRAPES outputs found matches in the following format:
```
[DB Graph ID]:{([query node id], [target node id]), ...}
```
Example
>0:{(0,10),(2,11),(1,12)}

This is the match found between a query with 3 nodes and a target graph. The query node with ID equals to 0 was matched to the target node with ID 10, and so on.

<hr />

### Dataset 
[Dataset repository](https://github.com/GiugnoLab/Dataset-GRAPES)

<hr />
