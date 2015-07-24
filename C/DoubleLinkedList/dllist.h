/*
        Josh A. Bosley
        Double linked list header.
        1-30-2014
*/

#ifndef DLLIST_H_INCLUDED
#define DLLIST_H_INCLUDED

#include <stdlib.h>

// Elements
typedef struct N_El
{
    void *data;
    struct N_El *pr;
    struct N_El *nx;

}   N_El;

// List
typedef struct N_list
{
    int   size;
    int (*match)(const void *k1, const void *k2);
    void (*destroy)( void *data);
    N_El *head;
    N_El *tail;

} N_list;

// Accessors
void create_list (N_list *list, void(*destroy)(void *data));
void destroy_list(N_list *list);
int  insert_next (N_list *list, N_El *el, const void *data);
int  insert_prev (N_list *list, N_El *el, const void *data);
int  remove_data (N_list *list, N_El *el, void **data     );

// Macros
#define list_size(list)  ((list)->size)
#define list_head(list)  ((list)->head)
#define list_tail(list)  ((list)->tail)
#define list_is_head(el) ((el)->pr != NULL ? 0 : 1)
#define list_is_tail(el) ((el)->nx != NULL ? 0 : 1)
#define list_data(el)    ((el)->data)
#define list_nx(el)      ((el)->nx)
#define list_pr(el)      ((el)->pr)

#endif // DLLIST_H_INCLUDED
