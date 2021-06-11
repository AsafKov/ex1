#include <stdlib.h>
#include <stdbool.h>

typedef struct node_t {
    int x;
    struct node_t *next;
} *Node;

typedef enum { SUCCESS=0,
    MEMORY_ERROR,
    UNSORTED_LIST,
    NULL_ARGUMENT,
} ErrorCode;

int getListLength(Node list);
bool isListSorted(Node list);
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code);
bool createNextLink(Node node);
bool insertSpare(Node merged_out, Node list, const int *length);
void freeList(Node list);
ErrorCode mergeLists(Node merged_out, Node list1, Node list2, int *list1_length, int *list2_length);

/**
 * Merging two linked lists of sorted number into one sorted linked list
 * @param list1 - A given sorted linked list
 * @param list2  - A given sorted linked list
 * @param error_code - Upon finish, used to indicate whether the process was successful or how it failed
 * @return Node (linked list of numbers), NULL on failure
 */
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code){
    Node merged_out = NULL;
    if (list1 == NULL || list2 == NULL || error_code == NULL){
        *error_code = NULL_ARGUMENT;
        return merged_out;
    }

    int list1_length = getListLength(list1);
    int list2_length = getListLength(list2);

    // Stop process if either list is unsorted
    if(!isListSorted(list1) || isListSorted(list2)){
        *error_code = UNSORTED_LIST;
        return merged_out;
    }

    merged_out = malloc(sizeof(struct node_t));
    if(merged_out == NULL) {
        *error_code = MEMORY_ERROR;
        return merged_out;
    }

    *error_code = mergeLists(merged_out, list1, list2, &list1_length, &list2_length);
    if(*error_code != SUCCESS){
        freeList(merged_out);
        return NULL;
    }
    *error_code = SUCCESS;
    return merged_out;
}

ErrorCode mergeLists(Node merged_out, Node list1, Node list2, int *list1_length, int *list2_length){
    // Set a new pointer to the start of the list for iteration
    Node current_node = merged_out;
    Node dummy_list1 = list1, dummy_list2 = list2;
    while(*list1_length != 0 && *list2_length != 0){
        if(dummy_list1 -> x < dummy_list2 -> x){
            current_node -> x = dummy_list1 -> x;
            dummy_list1 = dummy_list1->next;
            *list1_length -= 1;
        } else {
            current_node -> x = dummy_list2 -> x;
            dummy_list2 = dummy_list2->next;
            *list2_length -= 1;
        }
        if(!createNextLink(current_node)) {
            return MEMORY_ERROR;
        }
        current_node = current_node->next;
    }

    // Since we don't know which list was fully inserted, we'll try to add the spares from both
    if(!insertSpare(merged_out, dummy_list1, list1_length) || !insertSpare(merged_out, dummy_list2, list2_length)){
        // Memory allocation failure has occurred while trying to finish the merging
        return MEMORY_ERROR;
    }
    return SUCCESS;
}

/**
 * Insert whatever spares are left from the lists after the initial merging
 * @param merged_out - Current node of the merged list
 * @param list - The list that may have spare nodes
 * @param length - Amount of nodes left to insert
 * @return true if successful, false if failed due to memory error
 */
bool insertSpare(Node merged_out, Node list, const int *length){
    while(*length != 0){
        merged_out->x = list->x;
        length -= 1;
        if(*length == 0)
            return true;
        if(!createNextLink(merged_out)) {
            return false;
        }
        merged_out = merged_out->next;
        list = list->next;
    }
    return true;
}

/**
 * De-allocate memory of a linked list
 * @param list - list to de-allocate
 */
void freeList(Node list){
    Node next_node = list;
    while(next_node != NULL){
        next_node = next_node->next;
        free(list);
        list = next_node;
    }
}

/**
 * Called whenever we need to allocate memory for a new link
 * @param node - the node linking to the new node
 * @return true if successful, false for memory allocation failure
 */
bool createNextLink(Node node){
    if(node == NULL){
        return false;
    }
    node->next = malloc(sizeof(struct node_t));
    if(node->next == NULL)
        return false;
    return true;
}
