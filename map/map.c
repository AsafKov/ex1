#include "node.c"
#include <stdlib.h>
#include <stdio.h>

#define NULL_ARGUMENT_INDICATOR (-1)

static MapResult reassignValue(Map map, MapKeyElement keyElement, MapDataElement dataElement);
MapResult addNewValues(Map map, MapKeyElement keyElement, MapDataElement dataElement);
static MapResult initializeNode(Map map, Node node, MapDataElement data, MapKeyElement key);
static Node createList();

struct Map_t {
    copyMapDataElements copyDataFunction;
    copyMapKeyElements copyMapKeyFunction;
    freeMapDataElements freeMapDataFunction;
    freeMapKeyElements freeMapKeyFunction;
    compareMapKeyElements compareMapKeyFunction;
    Node elements;
    Node pointer;
    int size;
};

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
    if(copyDataElement == NULL || compareKeyElements == NULL || freeDataElement == NULL
       || copyKeyElement == NULL || freeKeyElement == NULL) {
        return NULL;
    }
    Map map = malloc(sizeof(*map));
    if(map == NULL){
        return NULL;
    }
    map->copyDataFunction = copyDataElement;
    map->copyMapKeyFunction = copyKeyElement;
    map->freeMapDataFunction = freeDataElement;
    map->freeMapKeyFunction = freeKeyElement;
    map->compareMapKeyFunction = compareKeyElements;
    map->elements = NULL;

    map->size = 0;
    return map;
}

void mapDestroy(Map map){
    if(map == NULL) return;
    mapClear(map);
    free(map);
}

MapResult mapClear(Map map){
    if(map == NULL){
        return MAP_NULL_ARGUMENT;
    }
    map->pointer = NULL;
    Node dummy;
    while(map->elements != NULL){
        map->freeMapKeyFunction(map->elements->key);
        map->freeMapDataFunction(map->elements->data);
        dummy = map->elements;
        map->elements = dummy->next;
        free(dummy);
    }
    map->size = 0;
    return MAP_SUCCESS;
}

MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(map == NULL || keyElement == NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(!mapContains(map, keyElement)){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    Node dummy = map->elements;
    Node prev_node = NULL;
    while(map->compareMapKeyFunction(dummy->key, keyElement) != 0){
        prev_node = dummy;
        dummy = dummy->next;
    }
    if(prev_node != NULL){
        prev_node->next = dummy->next;
    } else {
        map->elements = dummy->next;
    }
    map->freeMapKeyFunction(dummy->key);
    map->freeMapDataFunction(dummy->data);
    free(dummy);
    map->size--;
    return MAP_SUCCESS;
}

Map mapCopy(Map map){
    if(map == NULL){
        return NULL;
    }
    Map map_copy = mapCreate(map->copyDataFunction, map->copyMapKeyFunction,
                             map->freeMapDataFunction, map->freeMapKeyFunction,
                             map->compareMapKeyFunction);
    if(map_copy == NULL){
        return NULL;
    }

    map_copy->elements = createList();
    if(map_copy->elements == NULL){
        free(map_copy);
        return NULL;
    }
    if(map->size == 0){
        return map_copy;
    }
    Node copy_dummy = map_copy->elements;
    Node original_dummy = map->elements;
    for(int i=1; i<map->size; i++){
        copy_dummy->next = createList();
        if(copy_dummy == NULL){
            mapDestroy(map_copy);
            return NULL;
        }
        copy_dummy = copy_dummy->next;
    }
    copy_dummy = map_copy->elements;
    for(int i=0; i<map->size; i++){
        if(initializeNode(map, copy_dummy, original_dummy->data, original_dummy->key) != MAP_SUCCESS){
            mapDestroy(map_copy);
            return NULL;
        }
        copy_dummy = copy_dummy->next;
        original_dummy = original_dummy->next;
    }
    map_copy->size = map->size;

    return map_copy;
}

int mapGetSize(Map map){
    if(map != NULL) {
        return map->size;
    }
    return NULL_ARGUMENT_INDICATOR;
}

bool mapContains(Map map, MapKeyElement element){
    if(map == NULL || map->size == 0 || element == NULL){
        return false;
    }
    Node dummy = map->elements;
    if(dummy == NULL) {
        return false;
    }

    int compareResult = map->compareMapKeyFunction(dummy->key, element);
    while(dummy->next != NULL && compareResult < 0){
        dummy = dummy->next;
        compareResult = map->compareMapKeyFunction(dummy->key, element);
    }
    return compareResult == 0;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    if(map == NULL || keyElement == NULL || dataElement == NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(map->elements == NULL){
        return addNewValues(map, keyElement, dataElement);
    }
    if(reassignValue(map, keyElement, dataElement) == MAP_SUCCESS){
        return MAP_SUCCESS;
    }
    return addNewValues(map, keyElement, dataElement);
}

/**
 * Add new key-data pair
 * @param map
 * @param keyElement
 * @param dataElement
 * @return
 */
MapResult addNewValues(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    MapResult result;
    if(map->elements == NULL){
        map->elements = createList();
        result = initializeNode(map, map->elements, dataElement, keyElement);
        if(result == MAP_SUCCESS){
            map->size++;
        } else {
            free(map->elements);
            map->elements = NULL;
        }
        return result;
    }
    Node dummy = map->elements;
    Node previous_node = dummy;
    int compareResult = map->compareMapKeyFunction(keyElement, dummy->key);
    Node newNode = createList();
    if(initializeNode(map, newNode, dataElement, keyElement) != MAP_SUCCESS){
        return MAP_OUT_OF_MEMORY;
    }
    if(compareResult < 0){
        newNode->next = dummy;
        map->elements = newNode;
    } else {
        while(dummy->next != NULL && compareResult > 0){
            previous_node = dummy;
            dummy = dummy->next;
            compareResult = map->compareMapKeyFunction(keyElement, dummy->key);
        }

        if(compareResult > 0){
            dummy->next = newNode;
        } else {
            newNode->next = dummy;
            previous_node->next = newNode;
        }
    }
    map->size++;
    return MAP_SUCCESS;
}

static MapResult initializeNode(Map map, Node node, MapDataElement data, MapKeyElement key){
    if(node == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    MapKeyElement temp_key = map->copyMapKeyFunction(key);
    if(temp_key == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    MapDataElement  temp_data = map->copyDataFunction(data);
    if(temp_data == NULL){
        map->freeMapKeyFunction(temp_key);
        return MAP_OUT_OF_MEMORY;
    }
    node->key = temp_key;
    node->data = temp_data;
    return MAP_SUCCESS;
}

static Node createList(){
    Node node = malloc(sizeof(*node));
    if(node == NULL){
        return NULL;
    }
    node->next = NULL;
    node->data = NULL;
    node->key = NULL;
    return node;
}

/**
 * If key exists, reassign data associated with it
 * @param map - The map to which we reassign the data
 * @param keyElement - The key which we need to reassign
 * @param dataElement - The new data
 * @return MAP_ITEM_DOES_NOT_EXIST if map doesn't contain key, MAP_SUCCESS if data was updated successfully
 */
static MapResult reassignValue(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    if(!mapContains(map, keyElement)){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    Node dummy = map->elements;

    while(dummy->next != NULL && map->compareMapKeyFunction(dummy->key, keyElement)){
        dummy = dummy->next;
    }
    MapDataElement temp_data = map->copyDataFunction(dataElement);
    if(temp_data == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    map->freeMapDataFunction(dummy->data);
    dummy->data = temp_data;
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map){
    if(map == NULL || map->size == 0)
        return NULL;
    map->pointer = map->elements;
    return map->copyMapKeyFunction((MapKeyElement)map->pointer->key);
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(map == NULL || map->size == 0 || !mapContains(map, keyElement)) {
        return NULL;
    }
    Node dummy = map->elements;
    int compareResult = map->compareMapKeyFunction(keyElement, dummy->key);
    while(dummy->next != NULL && compareResult != 0){
        dummy = dummy->next;
        compareResult = map->compareMapKeyFunction(keyElement, dummy->key);
    }
    if(compareResult == 0)
        return (MapDataElement)dummy->data;
    return NULL;
}

MapKeyElement mapGetNext(Map map){
    if(map == NULL || map->pointer == NULL || map->pointer->next == NULL){
        return NULL;
    }
    map->pointer = map->pointer->next;
    return map->copyMapKeyFunction(map->pointer->key);
}
