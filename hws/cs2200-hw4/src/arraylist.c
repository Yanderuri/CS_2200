/**
 * Name: <your name>
 * GTID: <your GTID>
 */

/*  PART 2: A CS-2200 C implementation of the arraylist data structure.
    Implement an array list.
    The methods that are required are all described in the header file. Description for the methods can be found there.

    Hint 1: Review documentation/ man page for malloc, calloc, and realloc.
    Hint 2: Review how an arraylist works.
    Hint 3: You can use GDB if your implentation causes segmentation faults.
*/

#include "arraylist.h"

/* Student code goes below this point */

arraylist_t *create_arraylist(uint capacity){
    arraylist_t *arraylist;
    if ((arraylist = malloc(sizeof(arraylist_t))) == NULL){
        return NULL;
    }
    arraylist->capacity = capacity;
    arraylist->size = 0;
    if ((arraylist->backing_array = calloc(capacity, sizeof(char *))) == NULL){
        return NULL;
    }
    return arraylist;
}

void add_at_index(arraylist_t *arraylist, uint index, char *data){
    if (index > arraylist->size){
        return;
    }
    if (arraylist->size == arraylist->capacity){
        resize(arraylist);
    }
    for(int i = arraylist->size; i > index; i--){
        arraylist->backing_array[i] = arraylist->backing_array[i-1];
    }
    arraylist->backing_array[index] = data;
    arraylist->size++;
}

void append(arraylist_t *arraylist, char *data){
    if (arraylist->size == arraylist->capacity){
        resize(arraylist);
    }
    arraylist->backing_array[arraylist->size] = data;
    arraylist->size++;
}

char* remove_from_index(arraylist_t * arraylist, uint index){
    if (index > arraylist->size || index < 0){
        return NULL;
    }
    char *removed = arraylist->backing_array[index];
    for(int i = index; i < arraylist->size; i++){
        arraylist->backing_array[i] = arraylist->backing_array[i+1];
    }
    arraylist->size--;
    return removed;
}

void resize(arraylist_t *arraylist){
    arraylist->capacity *= 2;
    char **new_backing_array = realloc(arraylist->backing_array, arraylist->capacity * sizeof(char *));
    if (new_backing_array == NULL){
        return;
    }
    free(arraylist->backing_array); // is this needed.
    arraylist->backing_array = new_backing_array;
}

void destroy_arraylist(arraylist_t *arraylist){
    free(arraylist->backing_array);
    free(arraylist);
}