#include "llist.h"

#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * struct llist_node_s - Internal node structure
 * @data: Pointer to node payload
 * @next: Pointer to next node
 */
typedef struct llist_node_s
{
        void *data;
        struct llist_node_s *next;
} llist_node_s;

/**
 * struct __list - Concrete list definition hidden behind the opaque typedef
 * @head: Pointer to first node
 * @tail: Pointer to last node
 * @size: Number of nodes currently stored
 * @mt:   Whether multithread protection is enabled
 * @lock: Mutex protecting the list when @mt is non-zero
 */
struct __list
{
        llist_node_s *head;
        llist_node_s *tail;
        size_t size;
        int mt;
        pthread_mutex_t lock;
};

int llist_errno = LLIST_SUCCESS;

/**
 * lock_list - Acquire list mutex when MT support is requested
 */
static void lock_list(llist_t *list)
{
        if (list && list->mt)
                pthread_mutex_lock(&list->lock);
}

/**
 * unlock_list - Release list mutex when MT support is requested
 */
static void unlock_list(llist_t *list)
{
        if (list && list->mt)
                pthread_mutex_unlock(&list->lock);
}

llist_t *llist_create(unsigned int flags)
{
        llist_t *list;
        int rc;

        list = calloc(1, sizeof(*list));
        if (!list)
        {
                llist_errno = LLIST_MALLOC_ERROR;
                return (NULL);
        }

        list->mt = ((flags & MT_SUPPORT_TRUE) == MT_SUPPORT_TRUE);
        if (list->mt)
        {
                rc = pthread_mutex_init(&list->lock, NULL);
                if (rc != 0)
                {
                        free(list);
                        llist_errno = LLIST_MULTITHREAD_ISSUE;
                        return (NULL);
                }
        }

        llist_errno = LLIST_SUCCESS;
        return (list);
}

static void free_nodes(llist_t *list, int destroy_nodes, node_dtor_t destructor)
{
        llist_node_s *node;

        while (list->head)
        {
                node = list->head;
                list->head = node->next;
                if (destroy_nodes)
                {
                        if (destructor)
                                destructor(node->data);
                        else
                                free(node->data);
                }
                free(node);
        }
        list->tail = NULL;
        list->size = 0;
}

int llist_destroy(llist_t *list, int destroy_nodes, node_dtor_t destructor)
{
        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        free_nodes(list, destroy_nodes, destructor);
        unlock_list(list);

        if (list->mt)
                pthread_mutex_destroy(&list->lock);
        free(list);

        llist_errno = LLIST_SUCCESS;
        return (0);
}

int llist_add_node(llist_t *list, llist_node_t node, int flags)
{
        llist_node_s *new_node;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        new_node = malloc(sizeof(*new_node));
        if (!new_node)
        {
                llist_errno = LLIST_MALLOC_ERROR;
                return (-1);
        }
        new_node->data = node;
        new_node->next = NULL;

        lock_list(list);
        if ((flags & ADD_NODE_FRONT) == ADD_NODE_FRONT)
        {
                new_node->next = list->head;
                list->head = new_node;
                if (!list->tail)
                        list->tail = new_node;
        }
        else
        {
                if (list->tail)
                        list->tail->next = new_node;
                else
                        list->head = new_node;
                list->tail = new_node;
        }
        list->size++;
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (0);
}

int llist_insert_node(llist_t *list, llist_node_t node,
        node_ident_t identifier, void *arg, int flags)
{
        llist_node_s *new_node, *iter, *prev = NULL;

        if (!list || !identifier)
        {
            llist_errno = LLIST_NULL_ARGUMENT;
            return (-1);
        }

        new_node = malloc(sizeof(*new_node));
        if (!new_node)
        {
                llist_errno = LLIST_MALLOC_ERROR;
                return (-1);
        }
        new_node->data = node;
        new_node->next = NULL;

        lock_list(list);
        iter = list->head;
        while (iter && !identifier(iter->data, arg))
        {
                prev = iter;
                iter = iter->next;
        }

        if (!iter)
        {
                unlock_list(list);
                free(new_node);
                llist_errno = LLIST_NODE_NOT_FOUND;
                return (-1);
        }

        if ((flags & ADD_NODE_BEFORE) == ADD_NODE_BEFORE)
        {
                if (!prev)
                {
                        new_node->next = list->head;
                        list->head = new_node;
                }
                else
                {
                        new_node->next = prev->next;
                        prev->next = new_node;
                }
        }
        else
        {
                new_node->next = iter->next;
                iter->next = new_node;
                if (list->tail == iter)
                        list->tail = new_node;
        }

        list->size++;
        unlock_list(list);
        llist_errno = LLIST_SUCCESS;
        return (0);
}

int llist_remove_node(llist_t *list, node_ident_t identifier, void *arg,
        int destroy_node, node_dtor_t destructor)
{
        llist_node_s *iter, *prev = NULL;

        if (!list || !identifier)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        iter = list->head;
        while (iter && !identifier(iter->data, arg))
        {
                prev = iter;
                iter = iter->next;
        }

        if (!iter)
        {
                unlock_list(list);
                llist_errno = LLIST_NODE_NOT_FOUND;
                return (-1);
        }

        if (!prev)
                list->head = iter->next;
        else
                prev->next = iter->next;
        if (list->tail == iter)
                list->tail = prev;
        list->size--;
        unlock_list(list);

        if (destroy_node)
        {
                if (destructor)
                        destructor(iter->data);
                else
                        free(iter->data);
        }
        free(iter);

        llist_errno = LLIST_SUCCESS;
        return (0);
}

llist_node_t llist_find_node(llist_t *list, node_ident_t identifier, void *arg)
{
        llist_node_s *iter;

        if (!list || !identifier)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (NULL);
        }

        lock_list(list);
        iter = list->head;
        while (iter && !identifier(iter->data, arg))
                iter = iter->next;
        unlock_list(list);

        if (!iter)
        {
                llist_errno = LLIST_NODE_NOT_FOUND;
                return (NULL);
        }

        llist_errno = LLIST_SUCCESS;
        return (iter->data);
}

llist_node_t llist_get_node_at(llist_t *list, unsigned int index)
{
        llist_node_s *iter;
        unsigned int idx = 0;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (NULL);
        }

        lock_list(list);
        if (index >= list->size)
        {
                unlock_list(list);
                llist_errno = LLIST_OUT_OF_RANGE;
                return (NULL);
        }
        iter = list->head;
        while (iter && idx < index)
        {
                iter = iter->next;
                idx++;
        }
        unlock_list(list);

        if (!iter)
        {
                llist_errno = LLIST_OUT_OF_RANGE;
                return (NULL);
        }

        llist_errno = LLIST_SUCCESS;
        return (iter->data);
}

int llist_for_each(llist_t *list, node_func_t action, void *arg)
{
        llist_node_s *iter;
        unsigned int idx = 0;
        int rc = 0;

        if (!list || !action)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        iter = list->head;
        while (iter)
        {
                rc = action(iter->data, idx, arg);
                if (rc)
                        break;
                iter = iter->next;
                idx++;
        }
        unlock_list(list);

        if (rc)
        {
                llist_errno = LLIST_SUCCESS;
                return (rc);
        }

        llist_errno = LLIST_SUCCESS;
        return (0);
}

llist_node_t llist_get_head(llist_t *list)
{
        llist_node_t data = NULL;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (NULL);
        }

        lock_list(list);
        if (list->head)
                data = list->head->data;
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (data);
}

llist_node_t llist_get_tail(llist_t *list)
{
        llist_node_t data = NULL;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (NULL);
        }

        lock_list(list);
        if (list->tail)
                data = list->tail->data;
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (data);
}

llist_node_t llist_pop(llist_t *list)
{
        llist_node_s *node;
        void *data;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (NULL);
        }

        lock_list(list);
        node = list->head;
        if (!node)
        {
                unlock_list(list);
                llist_errno = LLIST_OUT_OF_RANGE;
                return (NULL);
        }

        list->head = node->next;
        if (!list->head)
                list->tail = NULL;
        list->size--;
        unlock_list(list);

        data = node->data;
        free(node);
        llist_errno = LLIST_SUCCESS;
        return (data);
}

int llist_size(llist_t *list)
{
        int size;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        size = (int)list->size;
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (size);
}

int llist_is_empty(llist_t *list)
{
        int empty;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (1);
        }

        lock_list(list);
        empty = (list->size == 0);
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (empty);
}

int llist_append(llist_t *first, llist_t *second)
{
        if (!first || !second)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(first);
        lock_list(second);

        if (!second->head)
        {
                unlock_list(second);
                unlock_list(first);
                llist_errno = LLIST_SUCCESS;
                return (0);
        }

        if (first->tail)
                first->tail->next = second->head;
        else
                first->head = second->head;

        first->tail = second->tail;
        first->size += second->size;

        second->head = NULL;
        second->tail = NULL;
        second->size = 0;

        unlock_list(second);
        unlock_list(first);

        llist_errno = LLIST_SUCCESS;
        return (0);
}

int llist_reverse(llist_t *list)
{
        llist_node_s *prev = NULL, *cur, *next;

        if (!list)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        cur = list->head;
        list->tail = list->head;
        while (cur)
        {
                next = cur->next;
                cur->next = prev;
                prev = cur;
                cur = next;
        }
        list->head = prev;
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (0);
}

int llist_sort(llist_t *list, node_cmp_t cmp_func, void *arg, int flags)
{
        llist_node_s *sorted = NULL, *cur, *next, **probe;
        int asc = ((flags & SORT_LIST_ASC) == SORT_LIST_ASC);

        if (!list || !cmp_func)
        {
                llist_errno = LLIST_NULL_ARGUMENT;
                return (-1);
        }

        lock_list(list);
        cur = list->head;
        list->tail = NULL;
        while (cur)
        {
                next = cur->next;
                probe = &sorted;
                while (*probe)
                {
                        int cmp = cmp_func((*probe)->data, cur->data, arg);
                        if ((asc && cmp > 0) || (!asc && cmp < 0))
                                break;
                        probe = &(*probe)->next;
                }
                cur->next = *probe;
                *probe = cur;
                cur = next;
        }
        list->head = sorted;
        list->tail = sorted;
        if (list->tail)
        {
                while (list->tail->next)
                        list->tail = list->tail->next;
        }
        unlock_list(list);

        llist_errno = LLIST_SUCCESS;
        return (0);
}
