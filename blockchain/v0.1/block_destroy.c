#include "blockchain.h"

/**
 * block_destroy -		frees and destroys a block
 * @block:				block to free
 *
 * Return:				void
 */
void block_destroy(block_t *block)
{
	if (!block)
		return;
	free(block);
	block = NULL;
}
