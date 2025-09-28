#ifndef _LLIST_H_
# define _LLIST_H_

/*****************************************************************************/
/********************************** DEFINES **********************************/
/*****************************************************************************/

# define ADD_NODE_FRONT		(1 << 0)
# define ADD_NODE_REAR		(~ADD_NODE_FRONT)

# define ADD_NODE_BEFORE	(1 << 0)
# define ADD_NODE_AFTER		(~ADD_NODE_BEFORE)

# define SORT_LIST_ASC		(1 << 0)
# define SORT_LIST_DESC		(~SORT_LIST_ASC)

# define MT_SUPPORT_TRUE	(1 << 0)
# define MT_SUPPORT_FALSE	(~MT_SUPPORT_TRUE)

/*****************************************************************************/
/*********************************** TYPES ***********************************/
/*****************************************************************************/

/**
 * enum _LLIST - All the different functions errors
 *
 * @LLIST_SUCCESS:           Success
 * @LLIST_NODE_NOT_FOUND:     No matching node found
 * @LLIST_NULL_ARGUMENT:      NULL argument
 * @LLIST_OUT_OF_RANGE:       Index out of range
 * @LLIST_MALLOC_ERROR:       Memory allocation error
 * @LLIST_MULTITHREAD_ISSUE:  Multithreading error
 */
typedef enum _LLIST
{
	LLIST_SUCCESS = 0,
	LLIST_NODE_NOT_FOUND,
	LLIST_NULL_ARGUMENT,
	LLIST_OUT_OF_RANGE,
	LLIST_MALLOC_ERROR,
	LLIST_MULTITHREAD_ISSUE
} E_LLIST;

extern int llist_errno;

/* Opaque linked list structure */
typedef struct __list llist_t;
typedef void *llist_node_t;

/* function prototypes */
typedef int (*node_func_t)(llist_node_t node, unsigned int idx, void *arg);
typedef void (*node_dtor_t)(llist_node_t node);

/**
 * node_cmp_t - Function to compare two nodes
 *
 * @first:  Pointer to the first node
 * @second: Pointer to the second node
 * @arg:    Extra user-defined parameter
 *
 * Return: An integer less than, equal to, or greater than zero if first,
 *         respectively, is less than, equal, or greater than second
 */
typedef int (*node_cmp_t)(llist_node_t first, llist_node_t second, void *arg);

/**
 * node_ident_t - Function to identify a node
 *
 * @node: Pointer to the node to identify
 * @arg:  Extra user-defined parameter
 *
 * Return: Any non-zero value if @node is positively identified, 0 otherwise
 */
typedef int (*node_ident_t)(llist_node_t node, void *arg);

/*****************************************************************************/
/********************************* FUNCTIONS *********************************/
/*****************************************************************************/

llist_t *llist_create(unsigned int flags);
int llist_destroy(llist_t *list, int destroy_nodes, node_dtor_t destructor);
int llist_add_node(llist_t *list, llist_node_t node, int flags);
int llist_insert_node(llist_t *list, llist_node_t node,
        node_ident_t identifier, void *arg, int flags);
int llist_remove_node(llist_t *list, node_ident_t identifier, void *arg,
        int destroy_node, node_dtor_t destructor);
llist_node_t llist_find_node(llist_t *list, node_ident_t identifier,
        void *arg);
llist_node_t llist_get_node_at(llist_t *list, unsigned int index);
int llist_for_each(llist_t *list, node_func_t action, void *arg);
llist_node_t llist_get_head(llist_t *list);
llist_node_t llist_get_tail(llist_t *list);
llist_node_t llist_pop(llist_t *list);
int llist_size(llist_t *list);
int llist_is_empty(llist_t *list);
int llist_append(llist_t *first, llist_t *second);
int llist_reverse(llist_t *list);
int llist_sort(llist_t *list, node_cmp_t cmp_func, void *arg, int flags);

#endif /* ! _LLIST_H_ */
