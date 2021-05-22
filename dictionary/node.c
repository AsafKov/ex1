#include "node.h"
#include "map.h"
#include <stdlib.h>

struct node_t{
    MapDataElement data;
    MapKeyElement key;
    struct node_t *next;
};
