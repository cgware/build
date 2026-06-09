#ifndef PROJ_GRAPH_H
#define PROJ_GRAPH_H

#include "proj.h"

int proj_graph_transitive_deps(const proj_t *proj, list_node_t target, arr_t *deps, alloc_t alloc);
int proj_graph_toposort_packages(const proj_t *proj, arr_t *order, alloc_t alloc);
int proj_graph_toposort_targets(const proj_t *proj, arr_t *order, alloc_t alloc);

#endif
