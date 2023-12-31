#include "graph.h"
#include "path.h"
#include "stack.h"
#include "vertices.h"

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define OPTIONS "i:o:dh"

uint32_t min = -1;
Path *best_path = NULL;

void dfshelper(uint32_t vertex, Graph *g, Path *p) {
    //printf("graph vertices %u\n", graph_vertices(g));
    //printf("path vertices %u\n", path_vertices(p));
    if (graph_vertices(g) == path_vertices(p)) {
        //printf("inside 1st helper\n");
        //printf("g, vertex is %u\n, weight is %u\n", vertex, graph_get_weight(g, vertex, 0));
        if (graph_get_weight(g, vertex, 0) != 0) {
            // printf("inside 2nd helper\n");
            path_add(p, 0, g);
            if (min < 0 || path_distance(p) < min) {
                //printf("inside 3rd helper\n");
                min = path_distance(p);
                if (best_path == NULL) {
                    //printf("best path isnull\n");
                    best_path = path_create(graph_vertices(g) + 1);
                }
                path_copy(best_path, p);
            }
            path_remove(p, g);
        }
    }
}

void dfs(uint32_t vertex, Graph *g, Path *p) {
    dfshelper(vertex, g, p);
    graph_visit_vertex(g, vertex);
    for (uint32_t i = 0; i < graph_vertices(g); i++) {
        if (graph_get_weight(g, vertex, i) != 0) {
            //printf("inside dfs 1st\n");
            if (!graph_visited(g, i)) {
                //printf("adding dfs\n");
                path_add(p, i, g);
                dfs(i, g, p);
                //printf("removing path, i is %u\n", i);
                path_remove(p, g);
            }
        }
    }
    graph_unvisit_vertex(g, vertex);
}

int main(int argc, char **argv) {

    int opt = 0;
    bool help = false;
    //int arguments = 0;
    bool directed = false;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            //printf("got infile %s\n", optarg);
            if (infile == NULL) {
                printf("infile broke");
            }
            assert(infile != NULL);
            //arguments = 1;
            //printf("arguments is %u\n", arguments);
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                printf("outfile broke");
            }
            assert(outfile != NULL);
            //arguments = 1;
            break;
        case 'd':
            directed = true;
            //arguments = 1;
            break;
        case 'h':
            help = true;
            //printf("help is true\noptarg is %s\n", optarg);
            //arguments = 1;
            break;
        }
    }

    // if (arguments == 0) {
    //    fprintf(stderr,
    //     "After running ./tsp you can include any of these flags\n-i <inputfile> :  Sets the "
    ///    "file to read from (input file). Requires a filename as an argument. The default "
    // "file to read from is stdin\n-o <outputfile> : Sets the file to write to (output "
    // "file). Requires a filename as an argument. The default file to write to is "
    //      "stdout\n-d : Treats all graphs as directed. The default is undirected graphs.\n-h "
    //        ": Prints this help 	message.");
    // return 0;
    //}
    if (help == true) {
        printf("After running ./tsp you can include any of these flags\n-i <inputfile> :  Sets the "
               "file to read from (input file). Requires a filename as an argument. The default "
               "file to read from is stdin\n-o <outputfile> : Sets the file to write to (output "
               "file). Requires a filename as an argument. The default file to write to is "
               "stdout\n-d : Treats all graphs as directed. The default is undirected graphs.\n-h "
               ": Prints this help 	message.");
        return 0;
    }

    uint32_t num_vertices = 0;
    //printf("num vertices is %u\n", num_vertices);
    if (fscanf(infile, "%u\n", &num_vertices) != 1) {
        fprintf(stderr, "tsp: error reading number of vertices\n");
        exit(1);
    }
    //printf("num vertices is %u\n", num_vertices);

    Graph *g = graph_create(num_vertices, directed);
    char vertex[100];
    for (uint32_t i = 0; i < num_vertices; i++) {
        //got fgets from discord help (ben)
        if (fgets(vertex, sizeof(vertex), infile) == NULL) {
            fprintf(stderr, "tsp: error reading in vertex name\n");
            exit(1);
        } else {
            vertex[strcspn(vertex, "\n")] = '\0';
            graph_add_vertex(g, vertex, i);
        }
    }
    uint32_t num_edges = 0;
    if (fscanf(infile, "%u\n", &num_edges) != 1) {
        fprintf(stderr, "tsp: error reading in num edges");
        exit(1);
    }
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t weight = 0;
    for (uint32_t i = 0; i < num_edges; i++) {
        if (fscanf(infile, "%u %u %u", &start, &end, &weight) != 3) {
            fprintf(stderr, "tsp: error reading adjacency list");
            exit(1);
        } else {
            graph_add_edge(g, start, end, weight);
        }
    }

    //graph_print(g);

    Path *path = path_create(graph_vertices(g) + 1);
    path_add(path, 0, g);
    //path_add(path, 1, g);
    //path_add(path, 2, g);
    //path_add(path, 3, g);
    //path_remove(path, g);
    //path_print(path, stdout, g);
    //printf("\n");
    //Path *path2 = path_create(graph_vertices(g) + 1);
    //path_add(path2, 5, g);
    //path_add(path2, 6, g);
    //path_add(path2, 7, g);
    //path_print(path2, stdout, g);
    //path_copy(path2, path);
    //path_print(path2, stdout, g);
    //run dfs
    dfs(0, g, path);
    //path_print(best_path, outfile, g);

    if (best_path == NULL || path_vertices(best_path) != graph_vertices(g) + 1) {
        fprintf(outfile, "No path found! Alissa is lost!\n");
    } else {
        path_print(best_path, outfile, g);
    }

    if (infile != stdin) {
        fclose(infile);
    }

    if (outfile != stdout) {
        fclose(outfile);
    }
    graph_free(&g);
    path_free(&best_path);
    path_free(&path);

    return 0;
}
