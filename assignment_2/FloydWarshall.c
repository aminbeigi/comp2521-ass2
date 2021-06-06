/**
 * Name: Amin Ghasembeigi
 * ZID: z5555555
 */

// Floyd Warshall ADT interface
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "FloydWarshall.h"
#include "Graph.h"

static bool validDistances(int d1, int d2);

/**
 * Finds all shortest paths between all pairs of nodes.
 * 
 * The  function  returns  a 'ShortestPaths' structure with the required
 * information:
 * - the number of vertices in the graph
 * - distance matrix
 * - matrix of intermediates (see description above)
 */
ShortestPaths FloydWarshall(Graph g) {
    int nV = GraphNumVertices(g);

    // allocate memory for dist[][] and next[][]
    int **dist = malloc(nV * sizeof(int*));
    int **next = malloc(nV * sizeof(int*));
    for (int i = 0; i < nV; i++) {
        dist[i] = malloc(nV * sizeof(int));
        next[i] = malloc(nV * sizeof(int));
    }

    // initailise dist[][] and next[][]
    for (int i = 0; i < nV; ++i) {
        for (int j = 0; j < nV; ++j) {
            if (i == j) {
                // distance to self is 0
                dist[i][j] = 0;
            } else {
                dist[i][j] = INFINITY;
            }
            next[i][j] = -1;
        }
    }
    
    // copy distances from given graph and outLinks for next values
    for (int u = 0; u < nV; ++u) {
        AdjList outLink = GraphOutIncident(g, u);
        while (outLink != NULL) {
            dist[u][outLink->v] = outLink->weight;
            next[u][outLink->v] = outLink->v;
            outLink = outLink->next;
        }
    }
    
    // Floyd-Warshall algorithm
    // find shortest path between all pairs of verticies
    for (int k = 0; k < nV; ++k) {
        for (int i = 0; i < nV; ++i) {
            for (int j = 0; j < nV; ++j) {
                if (validDistances(dist[i][k], dist[k][j]) && dist[i][j] > dist[i][k] + dist[k][j]) {
                    // dist[i][k] and dist[k][j] are not INFINITY
                    // update distance with the new smaller weight
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    ShortestPaths sps = {.numNodes = nV, .dist = (int**)dist, .next = (int**)next};
    return sps;
}

/**
 * This  function  is  for  you to print out the ShortestPaths structure
 * while you are debugging/testing your implementation. 
 * 
 * We will not call this function during testing, so you may  print  out
 * the  given  ShortestPaths  structure in whatever format you want. You
 * may choose not to implement this function.
 */
void showShortestPaths(ShortestPaths sps) {
    for (int i = 0; i < sps.numNodes; ++i) {
        for (int j = 0; j < sps.numNodes; ++j) {
            if (sps.dist[i][j] == INFINITY) {
                printf("INF ");

            } else { 
                printf("%d ", sps.dist[i][j]);
            }
        }
        printf("\n");
    }
}

/**
 * Frees  all  memory associated with the given ShortestPaths structure.
 * We will call this function during testing, so you must implement it.
 */
void freeShortestPaths(ShortestPaths sps) {
    for (int i = 0; i < sps.numNodes; ++i) {
        free(sps.dist[i]);
        free(sps.next[i]);
    }
    free(sps.dist);
    free(sps.next);
}

// helper funcitons

/**
 * returns true if both dist1 and dist2 are not INFINITY
 * otherwise false
 * used to prevent interger overflow in the Floyd-Warshall algorithm
 */
static bool validDistances(int dist1, int dist2) {
    return dist1 != INFINITY && dist2 != INFINITY;
}
