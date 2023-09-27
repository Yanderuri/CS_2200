/**
 * Name: Vy Mai
 * GTID: 903681630
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

arraylist_t *create_arraylist(uint capacity)
{
    arraylist_t *arraylist;
    if ((arraylist = malloc(sizeof(arraylist_t))) == NULL)
    {
        return NULL;
    }
    arraylist->capacity = capacity;
    arraylist->size = 0;
    if ((arraylist->backing_array = calloc(capacity, sizeof(char *))) == NULL)
    {
        return NULL;
    }
    return arraylist;
}

void add_at_index(arraylist_t *arraylist, char *data, int index)
{
    if (index < 0)
    {
        return;
    }
    if (index >= arraylist->capacity || arraylist->size == arraylist->capacity)
    {
        resize(arraylist);
    }
    if (index >= arraylist->size && index < arraylist->capacity)
    {
        arraylist->backing_array[index] = data;
        arraylist->size++;
        return;
    }
    for (int i = arraylist->size; i > index; i--)
    {
        arraylist->backing_array[i] = arraylist->backing_array[i - 1];
    }
    arraylist->backing_array[index] = data;
    arraylist->size++;
}

void append(arraylist_t *arraylist, char *data)
{
    if (arraylist->size == arraylist->capacity)
    {
        resize(arraylist);
    }
    arraylist->backing_array[arraylist->size] = data;
    arraylist->size++;
}

char *remove_from_index(arraylist_t *arraylist, int index)
{
    if (index >= arraylist->size || index < 0)
    {
        return NULL;
    }
    char *removed = arraylist->backing_array[index];
    arraylist->backing_array[index] = NULL;
    for (int i = index; i < arraylist->size; i++)
    {
        arraylist->backing_array[i] = arraylist->backing_array[i + 1];
    }
    arraylist->size--;
    return removed;
}

void resize(arraylist_t *arraylist)
{
    arraylist->capacity *= 2;
    char **new_backing_array = calloc(arraylist->capacity, sizeof(char *));
    if (new_backing_array == NULL)
    {
        arraylist->capacity /= 2;
        return;
    }
    for (int i = 0; i < arraylist->size; i++)
    {
        new_backing_array[i] = arraylist->backing_array[i];
    }
    free(arraylist->backing_array);
    arraylist->backing_array = new_backing_array;
}

void destroy(arraylist_t *arraylist)
{
    int i = 0;
    for (int i = 0; i < arraylist->size; i++)
    {
        arraylist->backing_array[i] = NULL;
    }
    free(arraylist);
}