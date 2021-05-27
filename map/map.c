#include "headers/node.h"
#include <stdlib.h>
#include <stdio.h>

#define NULL_ARGUMENT_INDICATOR (-1)

static MapResult reassignValue(Map map, MapKeyElement keyElement, MapDataElement dataElement);
MapResult addNewValues(Map map, MapKeyElement keyElement, MapDataElement dataElement);
static MapResult initializeNode(Map map, Node node, MapDataElement data, MapKeyElement key);

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
        map->freeMapKeyFunction(getKey(map->elements));
        map->freeMapDataFunction(getData((map->elements)));
        dummy = map->elements;
        map->elements = getNext(dummy);
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
    while(map->compareMapKeyFunction(getKey(dummy), keyElement) != 0){
        prev_node = dummy;
        dummy = getNext(dummy);
    }
    if(prev_node != NULL){
        setNext(prev_node, getNext(dummy));
    } else {
        map->elements = getNext(dummy);
    }
    map->freeMapDataFunction(getData(dummy));
    map->freeMapKeyFunction(getKey(dummy));
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

    if(mapGetSize(map) == 0){
        return map_copy;
    }
    map_copy->elements = createEmptyNode();
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
        setNext(copy_dummy, createEmptyNode());
        if(getNext(copy_dummy) == NULL){
            mapDestroy(map_copy);
            return NULL;
        }
        copy_dummy = getNext(copy_dummy);
    }
    copy_dummy = map_copy->elements;
    for(int i=0; i<map->size; i++){
        if(initializeNode(map, copy_dummy, getData(original_dummy), getKey(original_dummy)) != MAP_SUCCESS){
            mapDestroy(map_copy);
            return NULL;
        }
        copy_dummy = getNext(copy_dummy);
        original_dummy = getNext(original_dummy);
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

    int compareResult = map->compareMapKeyFunction(getKey(dummy), element);
    while(getNext(dummy) != NULL && compareResult < 0){
        dummy = getNext(dummy);
        compareResult = map->compareMapKeyFunction(getKey(dummy), element);
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
        map->elements = createEmptyNode();
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
    int compareResult = map->compareMapKeyFunction(keyElement, getKey(dummy));
    Node newNode = createEmptyNode();
    if(initializeNode(map, newNode, dataElement, keyElement) != MAP_SUCCESS){
        return MAP_OUT_OF_MEMORY;
    }
    if(compareResult < 0){
        setNext(newNode, dummy);
        map->elements = newNode;
    } else {
        while(getNext(dummy) != NULL && compareResult > 0){
            previous_node = dummy;
            dummy = getNext(dummy);
            compareResult = map->compareMapKeyFunction(keyElement, getKey(dummy));
        }

        if(compareResult > 0){
            setNext(dummy, newNode);
        } else {
            setNext(newNode, dummy);
            setNext(previous_node, newNode);
        }
    }
    map->size++;
    return MAP_SUCCESS;
}

static MapResult initializeNode(Map map, Node node, MapDataElement data, MapKeyElement key){
    if(node == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    MapKeyElement new_key = map->copyMapKeyFunction(key);
    if(new_key == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    MapDataElement new_data = map->copyDataFunction(data);
    if(new_data == NULL){
        map->freeMapKeyFunction(new_data);
        return MAP_OUT_OF_MEMORY;
    }
    setKey(node, new_key);
    setData(node, map->freeMapDataFunction, new_data);
    return MAP_SUCCESS;
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

    while(getNext(dummy) != NULL && map->compareMapKeyFunction(getKey(dummy), keyElement)){
        dummy = getNext(dummy);
    }
    MapDataElement temp_data = map->copyDataFunction(dataElement);
    if(temp_data == NULL){
        return MAP_OUT_OF_MEMORY;
    }
    map->freeMapDataFunction(getData(dummy));
    setData(dummy, map->freeMapDataFunction ,temp_data);
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map){
    if(map == NULL || map->size == 0)
        return NULL;
    map->pointer = map->elements;
    return map->copyMapKeyFunction((MapKeyElement)getKey(map->pointer));
}

MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(map == NULL || map->size == 0 || !mapContains(map, keyElement)) {
        return NULL;
    }
    Node dummy = map->elements;
    int compareResult = map->compareMapKeyFunction(keyElement, getKey(dummy));
    while(getNext(dummy) != NULL && compareResult != 0){
        dummy = getNext(dummy);
        compareResult = map->compareMapKeyFunction(keyElement, getKey(dummy));
    }
    if(compareResult == 0) {
        return (MapDataElement) getData(dummy);
    }
    return NULL;
}

MapKeyElement mapGetNext(Map map){
    if(map == NULL || map->pointer == NULL || getNext(map->pointer) == NULL){
        return NULL;
    }
    map->pointer = getNext(map->pointer);
    return map->copyMapKeyFunction(getKey(map->pointer));
}