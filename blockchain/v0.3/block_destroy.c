#include "blockchain.h"

/**
 * block_destroy -		frees and destroys a block
 * @block:				block to free
 *
 * Return:				void
 */
void block_destroy(
	block_t *block)
{
	if (!block)								/* null block */
		return;
	if (block->transactions)				/* free transactions list */
		llist_destroy(
			block->transactions, 1, (node_dtor_t)transaction_destroy);
	free(block);							/* free block */
	block = NULL;							/* nullify block */
}
