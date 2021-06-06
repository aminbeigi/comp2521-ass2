/**
 * Name: Amin Ghasembeigi
 * ZID: z5555555
 */

// Centrality Measures ADT interface
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "FloydWarshall.h"
#include "Graph.h"

static void removeNonAdjacentEdges(Graph g, double** values);

/**
 * Finds  the  edge  betweenness  centrality  for each edge in the given
 * graph and returns the results in a  EdgeValues  structure.  The  edge
 * betweenness centrality of a non-existant edge should be set to -1.0.
 */
EdgeValues edgeBetweennessCentrality(Graph g) {
    int nV = GraphNumVertices(g);
    ShortestPaths sps = FloydWarshall(g);
    
    // allocate memory for values[][]
    double **values = malloc(nV * sizeof(double*));
    for (int i = 0; i < nV; i++) {
        values[i] = malloc(nV * sizeof(double));
    }

    // populate values[][]
    for (int i = 0; i < nV; ++i) {
        for (int j = 0; j < nV; ++j) {
            if (sps.next[i][j] == -1) {
                // edge doesn't exist
                values[i][j] = -1;

            } else {
                // reconstruct shortest path and increment edge in values[][] along the way
                int src = i;
                int dest = j;

                while (src != dest) {
                    int tmpSrc = src;
                    src = sps.next[src][dest];
                    ++values[tmpSrc][src];
                }
            }
        }
    }

    removeNonAdjacentEdges(g, values);
    EdgeValues e = {.numNodes = nV, .values = (double**)values};
    freeShortestPaths(sps);
    return e;
}

/**
 * Prints  the  values in the given EdgeValues structure to stdout. This
 * function is purely for debugging purposes and will NOT be marked.
 */
void showEdgeValues(EdgeValues evs) {

}

/**
 * Frees all memory associated with the given EdgeValues  structure.  We
 * will call this function during testing, so you must implement it.
 */
void freeEdgeValues(EdgeValues evs) {
    for (int i = 0; i < evs.numNodes; ++i) {
        free(evs.values[i]);
    }
    free(evs.values);
}

// helper functions

/**
 * remove non-adjacent items from values[][]
 * used in the edge betweenness centrality algorithm to clean up values[][] before returning
 */
static void removeNonAdjacentEdges(Graph g, double** values) {
    for (int u = 0; u < GraphNumVertices(g); ++u) {
        for (int v = 0; v < GraphNumVertices(g); ++v) {
            if (values[u][v] != -1 && !GraphIsAdjacent(g, u, v)) {
                // set non-adjacent edges that are not -1 to -1
                values[u][v] = -1;
            }
        }
    }
}
