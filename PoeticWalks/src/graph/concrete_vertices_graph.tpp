#ifndef CONCRETE_VERTICES_GRAPH_TPP
#define CONCRETE_VERTICES_GRAPH_TPP

#include <stdexcept>

#include "concrete_vertices_graph.h"

template<typename T>
Vertex<T>::Vertex(T vertex):
    label{vertex} {}

template<typename T>
int Vertex<T>::setTarget(Vertex<T> *target, int weight) {
    if (weight < 0) throw std::domain_error("Edge weight cannot be negative.");
    
    int previousWeight = 0;
    if (targets.find(target) == targets.end() && weight == 0) {
        return previousWeight;
    }

    if (targets.find(target) != targets.end()) {
        previousWeight = targets[target];
    }

    if (weight == 0) {
        targets.erase(target);
        (*target).sources.erase(this);
        return previousWeight;
    }

    targets[target] = weight;
    (*target).sources[this] = weight;
    return previousWeight;
}

template<typename T>
std::unordered_map<T, int> Vertex<T>::getSources() {
    std::unordered_map<T, int> output;
    for (auto& [key, value]: sources) {
        output[(*key).label] = value;
    }
    return output;
}

template<typename T>
std::unordered_map<T, int> Vertex<T>::getTargets() {
    std::unordered_map<T, int> output;
    for (auto& [key, value]: targets) {
        output[(*key).label] = value;
    }
    return output;
}

template<typename T>
ConcreteVerticesGraph<T>::ConcreteVerticesGraph():
    vertices_{} {}

template<typename T>
ConcreteVerticesGraph<T>::~ConcreteVerticesGraph() {
    for (int i = 0; i < vertices_.size(); i++) {
        delete vertices_[i];
    }
}

template<typename T>
bool ConcreteVerticesGraph<T>::add(T vertex) {
    if (findVertexIndex(vertex) != -1) return false;

    vertices_.push_back(new Vertex<T>(vertex));
    return true; 
}

template<typename T>
int ConcreteVerticesGraph<T>::set(T source, T target, int weight) {
    if (weight < 0) throw std::domain_error("Edge weight cannot be negative.");
    
    Vertex<T> *sourcePointer;
    Vertex<T> *targetPointer;

    int vertexIndex;

    vertexIndex = findVertexIndex(source);
    if (vertexIndex == -1) {
        sourcePointer = new Vertex<T>(source);
        vertices_.push_back(sourcePointer);
    }
    else {
        sourcePointer = vertices_[vertexIndex];
    }

    vertexIndex = findVertexIndex(target);
    if (vertexIndex == -1) {
        targetPointer = new Vertex<T>(target);
        vertices_.push_back(targetPointer);
    }
    else {
        targetPointer = vertices_[vertexIndex];
    }

    return (*sourcePointer).setTarget(targetPointer, weight);
}

template<typename T>
bool ConcreteVerticesGraph<T>::remove(T vertex) {
    int vertexIndex;
    vertexIndex = findVertexIndex(vertex);
    if (vertexIndex == -1) return false;

    std::unordered_map<T, int> sourcesMap = (*vertices_[vertexIndex]).getSources();
    std::unordered_map<T, int> targetsMap = (*vertices_[vertexIndex]).getTargets();
    for (int i = 0 ; i < vertices_.size(); i++) {
        T vertexLabel = (*vertices_[i]).label;
        if (sourcesMap.find(vertexLabel) != sourcesMap.end()) {
            (*vertices_[i]).setTarget(vertices_[vertexIndex], 0);
        }
        if (targetsMap.find(vertexLabel) != targetsMap.end()) {
            (*vertices_[vertexIndex]).setTarget(vertices_[i], 0);
        }
    }

    delete vertices_[vertexIndex];
    vertices_.erase(vertices_.begin() + vertexIndex);
    return true;
}
template<typename T>
std::unordered_set<T> ConcreteVerticesGraph<T>::vertices() {
    std::unordered_set<T> output;
    for (int i = 0 ; i < vertices_.size(); i++) {
        output.insert((*vertices_[i]).label);
    }
    return output;
}

template<typename T>
std::unordered_map<T, int> ConcreteVerticesGraph<T>::sources(T target) {
    int vertexIndex;
    if (vertexIndex = findVertexIndex(target) == -1) return std::unordered_map<T, int>();

    return (*vertices_[vertexIndex]).getSources();
}

template<typename T>
std::unordered_map<T, int> ConcreteVerticesGraph<T>::targets(T source) {
    int vertexIndex;
    if (vertexIndex = findVertexIndex(source) == -1) return std::unordered_map<T, int>();

    return (*vertices_[vertexIndex]).getTargets();
}

template<typename T>
int ConcreteVerticesGraph<T>::findVertexIndex(T vertex) {
    for (int i = 0 ; i < vertices_.size(); i++) {
        if ((*vertices_[i]).label == vertex) return i;
    }
    return -1;
}

#endif