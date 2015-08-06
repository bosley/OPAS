#include <string.h>
#include <stdlib.h>
#include "dllist.h"

void create_list (N_list *list, void(*destroy)( void *data))
{
    list->size      = 0;
    list->destroy   = destroy;
    list->head      = NULL;
    list->tail      = NULL;
    return;
}

void destroy_list(N_list *list)
{
    void *data;
    while(list_size(list) > 0)
    {
        if ( remove_data(list, list_tail(list),(void**)&data) == 0
            && list->destroy != NULL)
        {
            list->destroy(data);

        }
    }
    memset(list, 0, sizeof(N_list));
    return;
}

// MERGE INSER NEXT AND INSERT PREV. PASS INT TO DETERMINE WHICH.

int  insert_next (N_list *list, N_El *el, const void *data)
{
    // Don't allow null elements
    if (el == NULL && list_size(list) != 0)
        return -1;

    // Allocate a place for the new node, and insert.
    N_El *node;
    if ((node = (N_El *)malloc(sizeof(N_El))) == NULL)
        return -1;

    node->data = (void *)data;

    // Empty list
    if (list_size(list) == 0)
    {
        list->head      = node;
        list->tail      = node;
        list->head->pr  = NULL;
        list->head->nx  = NULL;
    }

    else
    {
        node->nx = el->nx;
        node->pr = el->pr;

        if (el->nx == NULL)
            list->tail = node;
        else
            el->nx->pr = node;
    }

    list->size++;

    return 0;
}

int  insert_prev (N_list *list, N_El *el, const void *data)
{
    // Don't allow null elements
    if (el == NULL && list_size(list) != 0)
        return -1;

    // Allocate a place for the new node, and insert.
    N_El *node;
    if ((node = (N_El *)malloc(sizeof(N_El))) == NULL)
        return -1;

    node->data = (void *)data;

    // Empty list
    if (list_size(list) == 0)
    {
        list->head      = node;
        list->tail      = node;
        list->head->pr  = NULL;
        list->head->nx  = NULL;
    }

    else
    {
        node->nx = el;
        node->pr = el->pr;

        if (el->pr == NULL)
            list->head = node;
        else
            el->pr = node;
    }

    list->size++;

    return 0;
}

int  remove_data (N_list *list, N_El *el, void **data     )
{
    if (el == NULL || list_size(list) == 0)
        return -1;

    *data = el->data;

    if (el == list->head)
    {
        list->head = el->nx;

        if (list->head == NULL)
            list->tail== NULL;
        else
            el->nx->pr = NULL;
    }
    else
    {
        el->pr->nx = el->nx;

        if (el->nx == NULL)
            list->tail = el->nx;
        else
            el->nx->pr = el->pr;
    }

    free(el);
    list->size--;
    return 0;
}