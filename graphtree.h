#ifndef _GRAPHTREE_H_
#define _GRAPHTREE_H_

// graphtree.h
// Header-only c library to visualize DFS(depth-first search) easily with graphviz!
// License CC0 by https://github.com/toduq

#include <stdio.h>
#include <stdarg.h>

#define GRAPHTREE_MAX_DEPTH 100

typedef struct graphtree {
  int depth;
  int current_id;
  int stack[GRAPHTREE_MAX_DEPTH];
  FILE *fp;
} graphtree_t;

void graphtree_init(graphtree_t *graph, char *filename) {
  graph->depth = 0;
  graph->current_id = 0;
  graph->stack[0] = 0;
  graph->fp = fopen(filename, "w");
  fputs("digraph G {\n", graph->fp);
  fputs("  rankdir=TB;\n", graph->fp);
}

// [enter, exit] is set-function for BEFORE labeling
int graphtree_enter(graphtree_t *graph) {
  graph->stack[graph->depth] = graph->current_id;
  if(graph->depth > 0) {
    fprintf(graph->fp, "  %d -> %d;\n", graph->stack[graph->depth-1], graph->stack[graph->depth]);
  }
  graph->depth++;
  graph->current_id++;
  return graph->current_id - 1;
}

int graphtree_exit(graphtree_t *graph, char *format, ...) {
  graph->depth--;
  fprintf(graph->fp, "  %d [label = \"", graph->stack[graph->depth]);
  va_list arg;
  va_start(arg, format);
  vfprintf(graph->fp, format, arg);
  va_end(arg);
  fprintf(graph->fp, "\"];\n");
  return graph->stack[graph->depth];
}

// [down, up] is also set-function for AFTER labeling
int graphtree_down(graphtree_t *graph, char *format, ...) {
  graph->stack[graph->depth] = graph->current_id;
  fprintf(graph->fp, "  %d [label = \"", graph->stack[graph->depth]);
  va_list arg;
  va_start(arg, format);
  vfprintf(graph->fp, format, arg);
  va_end(arg);
  fprintf(graph->fp, "\"];\n");
  if(graph->depth > 0) {
    fprintf(graph->fp, "  %d -> %d;\n", graph->stack[graph->depth-1], graph->stack[graph->depth]);
  }
  graph->depth++;
  graph->current_id++;
  return graph->current_id - 1;
}

int graphtree_up(graphtree_t *graph) {
  graph->depth--;
  return graph->stack[graph->depth];
}

void graphtree_save(graphtree_t *graph) {
  fputs("}", graph->fp);
  fclose(graph->fp);
}

#endif
