#include "headers/node.h"
#include "headers/map.h"
#include <stdlib.h>

struct node_t{
    MapDataElement data;
    MapKeyElement key;
    struct node_t *next;
};
