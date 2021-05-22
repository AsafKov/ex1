#include "node.h"
#include <stdlib.h>
#include "map.h"

struct node_t{
    MapDataElement data;
    MapKeyElement key;
    struct node_t *next;
};

Node createList(){
    Node node = malloc(sizeof(*node));
    if(node == NULL){
        return NULL;
    }
    node->next = NULL;
    node->data = NULL;
    node->key = NULL;
    return node;
}