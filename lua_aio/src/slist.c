
#include "slist.h"

slist_t slist_create()
{
    slist_t list;
    list.head = NULL;
    list.last = NULL;
    return list;
}

void slist_append(slist_t* list, void* ptr)
{
    assert( list != NULL );
    slist_node_t * new = (slist_node_t*)malloc(sizeof(slist_node_t));
    new->ptr = ptr;
    if( list->last == NULL )
    {
        assert( list->head == NULL );
        list->head = new;
    }
    else
    {
        list->last->next = new;
    }

    list->last = new;
}

void slist_delete(slist_t list)
{
    slist_node_t * tmp;
    slist_node_t * cur;

    tmp = list.head;

    while( tmp != NULL )
    {
        cur = tmp;
        tmp = tmp->next;
        free(cur);
    }
}

slist_node_t* slist_first(slist_t list)
{
    return list.head;
}

slist_node_t* slist_next(slist_node_t* list)
{
    assert( list != NULL );
    return list->next;
}
