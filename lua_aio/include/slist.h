/*
 * =====================================================================================
 *
 *       Filename:  slist.h
 *        Created:  05/19/14 22:22:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SLIST__
#define  __SLIST__

typedef struct slist_node_s slist_node_t;
struct slist_node_s { slist_node_t * next; void * ptr; };

typedef struct slist_s slist_t;
struct slist_s { slist_node_t * head; slist_node_t * last; };

slist_t slist_create();
void slist_append(slist_t* list, void* ptr);
void slist_delete(slist_t list);
slist_node_t* slist_first(slist_t list);
slist_node_t* slist_next(slist_node_t* list);

#endif   /* ----- #ifndef __SLIST__  ----- */
