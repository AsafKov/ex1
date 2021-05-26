#ifndef EX1_LINKEDLIST_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#define EX1_LINKEDLIST_H

typedef struct node_t *Node;

Node createEmptyNode();

MapKeyElement getKey(Node node);

void setKey(Node node, MapKeyElement key);

MapDataElement getData(Node node);

void setData(Node node, freeMapDataElements freeData, MapDataElement data);

Node getNext(Node node);

void setNext(Node setTo, Node nextNode);

#endif //EX1_LINKEDLIST_H
