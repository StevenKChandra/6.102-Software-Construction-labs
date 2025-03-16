#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_set>
#include <unordered_map>

/**
 * A mutable weighted directed graph with labeled vertices and edges.
 * 
 * Edges are directed and have a positive weight of type int.
 * 
 * PS2 instructions: this is a required ADT interface.
 * You MUST NOT change the specifications or add additional methods.
 * 
 * @param <L> type of vertex labels in this graph, must be immutable
 */
template<typename T>
class Graph {
public:
    ~Graph() = default;

    /**
     * Create an empty graph.
     * 
     * @param <L> type of vertex labels in the graph, must be immutable
     * @return a new empty weighted directed graph
     */

    /**
     * Add a vertex to this graph.
     * 
     * @param vertex label for the new vertex
     * @return true if this graph did not already include a vertex with the
     *         given label; otherwise false (and this graph is not modified)
     */

    virtual bool add(T vertex) = 0;

    /**
     * Add, change, or remove a weighted directed edge in this graph.
     * If weight is nonzero, add an edge or update the weight of that edge;
     * vertices with the given labels are added to the graph if they do not
     * already exist.
     * If weight is zero, remove the edge if it exists (the graph is not
     * otherwise modified).
     * 
     * @param source label of the source vertex
     * @param target label of the target vertex
     * @param weight nonnegative weight of the edge
     * @return the previous weight of the edge, or zero if there was no such
     *         edge
     */
    virtual int set(T source, T target, int weight) = 0;

    /**
     * Remove a vertex from this graph; any edges to or from the vertex are
     * also removed.
     * 
     * @param vertex label of the vertex to remove
     * @return true if this graph included a vertex with the given label;
     *         otherwise false (and this graph is not modified)
     */
    virtual bool remove(T vertex) = 0;

    /**
     * Get all the vertices in this graph.
     * 
     * @return the set of labels of vertices in this graph
     */
    virtual std::unordered_set<T> vertices() = 0;

    /**
     * Get the source vertices with directed edges to a target vertex and the
     * weights of those edges.
     * 
     * @param target a label
     * @return a map where the key set is the set of labels of vertices such
     *         that this graph includes an edge from that vertex to target, and
     *         the value for each key is the (nonzero) weight of the edge from
     *         the key to target
     */
    virtual std::unordered_map<T, int> sources(T target) = 0;

    /**
     * Get the target vertices with directed edges from a source vertex and the
     * weights of those edges.
     * 
     * @param source a label
     * @return a map where the key set is the set of labels of vertices such
     *         that this graph includes an edge from source to that vertex, and
     *         the value for each key is the (nonzero) weight of the edge from
     *         source to the key
     */
    virtual std::unordered_map<T, int> targets(T source) = 0;
};

#endif