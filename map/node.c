#include "headers/node.h"

struct node_t{
    MapDataElement data;
    MapKeyElement key;
    struct node_t *next;
};

Node createEmptyNode(){
    Node node = malloc(sizeof(*node));
    if(node == NULL){
        return NULL;
    }
    node->next = NULL;
    node->data = NULL;
    node->key = NULL;
    return node;
}

MapKeyElement getKey(Node node){
    return node->key;
}

void setKey(Node node, MapKeyElement key){
    node->key = key;
}

MapDataElement getData(Node node){
    return node->data;
}

void setData(Node node, freeMapDataElements freeData, MapDataElement data){
    freeData(node->data);
    node->data = data;
}

Node getNext(Node node){
    return node->next;
}

void setNext(Node setTo, Node nextNode){
    setTo->next = nextNode;
}

