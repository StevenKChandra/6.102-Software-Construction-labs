#ifndef GRAPH_POET_H
#define GRAPH_POET_H

#include <string>
#include <memory>

#include "graph.h"

class GraphPoet {
private:
    Graph<std::string> *graph;
    int fd;
    
public:
    GraphPoet() = delete;

    /**
     * Create a new poet with the graph from corpus (as described above).
     * 
     * @param graphImplementation pointer to a graph implementation (dependency injection)
     * @param poetFilePath a string path to the poem txt file
     * @param absolutePath whether the path given is relative or not
     * @throws runtime_error if the file given by the path cannot be found or read
     */
    GraphPoet(Graph<std::string> *graphImplementation, std::string poetFilePath, bool absolutePath = false);

    ~GraphPoet();

    /**
     * Generate a poem.
     * 
     * @param input string from which to create the poem
     * @return poem (as described above)
     */
    std::string poem(std::string input);

    GraphPoet& operator=(GraphPoet&& that);
};

#endif