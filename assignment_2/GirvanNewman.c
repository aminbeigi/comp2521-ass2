/**
 * Name: Amin Ghasembeigi
 * ZID: z5555555
 */

// Girvan-Newman Algorithm for community discovery
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "GirvanNewman.h"
#include "Graph.h"

#define FILLER -2 

// TODO: two components

static Dendrogram newDNode(int vertex);
static double getHighestEdgeBetweenness(EdgeValues e);
static void removeEdge(Graph g, EdgeValues e, double edge);
static int getSingleNodeComponent(Graph g, int* visitedComponents);
static bool noEdges(EdgeValues e);
static int getConnectedCount(Graph g);
static void dfsComponent(Graph g, int start, int id, int componentOf[]);

/**
 * Generates  a Dendrogram for the given graph g using the Girvan-Newman
 * algorithm.
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram GirvanNewman(Graph g) {
    Dendrogram root = malloc(sizeof(Dendrogram));
    Dendrogram curr = root;
    Dendrogram currLeft = root;
    int nV = GraphNumVertices(g);

    int* visitedComponents = malloc(nV * sizeof(int));
    for (int i = 0; i < nV; ++i) {
        visitedComponents[i] = -1;
    }

    while (true) {
        int numConnectedBefore = getConnectedCount(g);
        EdgeValues e = edgeBetweennessCentrality(g);
        removeEdge(g, e, getHighestEdgeBetweenness(e));
        int vertex = getSingleNodeComponent(g, visitedComponents);
        int numConnectedAfter = getConnectedCount(g);
        int numConnectedDiff = numConnectedAfter - numConnectedBefore;

        if (noEdges(e)){
            // add final level of dendogram
            if (vertex != -1) {
                // there is a vertex to add
                curr->left = newDNode(vertex);
            }
            int vertex = getSingleNodeComponent(g, visitedComponents);
            if (vertex != -1) {
                // there is another vertex to add
                curr->right = newDNode(vertex);
            }

            freeEdgeValues(e);
            break;

        } else if (vertex != -1) {
            // new single component was generated
            // create another level in dendogram
            curr->right = newDNode(FILLER);
            curr->left = newDNode(vertex);
            curr = curr->right;

        } else if (vertex == -1 && numConnectedDiff != 0) {
            // didn't find single component but there is new component
            // create another level in dendogram both left and right
            curr->left = currLeft;
            curr->right = newDNode(FILLER);

        } else if (vertex == -1 && abs(numConnectedDiff) == 2) {
            // didn't find single component
            // but found two isolated nodes
            curr->left = newDNode(FILLER);
            currLeft = curr->left;
            curr->right = newDNode(FILLER);
        }
    }

    free(visitedComponents);
    return root;
}

/**
 * Frees all memory associated with the given Dendrogram  structure.  We
 * will call this function during testing, so you must implement it.
 */
void freeDendrogram(Dendrogram d) {
    if (d == NULL) {
        return;
    }
    freeDendrogram(d->left);
    freeDendrogram(d->right);
    free(d);
}

// helper functions

/**
 * creates a new DNode
 */
static Dendrogram newDNode(int vertex) {
    Dendrogram node = malloc(sizeof(DNode));
    if (node == NULL) {
        fprintf(stderr, "Couldn't allocate memory");
        exit(EXIT_FAILURE);
    }
    node->vertex = vertex;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/**
 * returns the highest valued item inside values[][]
 * or -1 if there is no edge betweeness i.e. the graph has no edges
 */
static double getHighestEdgeBetweenness(EdgeValues e) {
    int highestEdgeBetweenness = -1;
    for (int u = 0; u < e.numNodes; ++u) {
        for (int v = 0; v < e.numNodes; ++v) {
            if (e.values[u][v] > highestEdgeBetweenness) {
                highestEdgeBetweenness = e.values[u][v];
            }
        }
    }
    return highestEdgeBetweenness;
}

/**
 * remove an edge from values[][]
 * used in the Girvan-Newman algorithm to remove the edge with the highest edge betweenness
 */
static void removeEdge(Graph g, EdgeValues e, double edge) {
    for (int i = 0; i < e.numNodes; ++i) {
        for (int j = 0; j < e.numNodes; ++j) {
            if (e.values[i][j] == edge) {
                e.values[i][j] = -1;
                GraphRemoveEdge(g, i, j);
            }
        }
    }
}

/**
 * returns a vertex that is a new component
 * otherwise -1 if the graph only contains one component
 * used in the Girvan-Newman algorithm for component checking
 */
static int getSingleNodeComponent(Graph g, int* visitedComponents) { 
    int nV = GraphNumVertices(g);
    for (int u = 0; u < nV; ++u) {
        if (GraphOutIncident(g, u) == NULL && GraphInIncident(g, u) == NULL && visitedComponents[u] == -1) {
            // a vertex without outlinks and inlinks is a new component
            visitedComponents[u] = 1; // mark vertex as visited
            return u;
        }
    }
    return -1;
}

/**
 * returns true if the graph has no edges
 * otherwise false
 */
static bool noEdges(EdgeValues e) {
    return getHighestEdgeBetweenness(e) == -1;
}

/**
 * return total count of components in graph
 * source: COMP2521 lecture (week 5)
 */
static int getConnectedCount(Graph g) {
    int componentOf[GraphNumVertices(g)];
    for (int i = 0; i < GraphNumVertices(g); i++) {
        componentOf[i] = -1;
    }
    int getConnectedCount = 0;
    for (int i = 0; i < GraphNumVertices(g); i++) {
        if (componentOf[i] == -1) {
            // current node has not been visited
            dfsComponent(g, i, getConnectedCount, componentOf);
            getConnectedCount++;
        }
    }
    return getConnectedCount;
}

/**
 * label each node with a component ID
 * source: COMP2521 lecture (week 5)
 */
static void dfsComponent(Graph g, int start, int id, int componentOf[]) {
    componentOf[start] = id;
    for (int i = 0; i < GraphNumVertices(g); i++) {
        if (GraphIsAdjacent(g, start, i) && componentOf[i] == -1) {
            dfsComponent(g, i, id, componentOf);
        }
    }
}
