#include "blockchain.h"

/**
 * blockchain_destroy -	frees and nullifies a blockchain
 * @blockchain:			pointer to blockchain to annihilate
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	if (blockchain->chain)
		llist_destroy(blockchain->chain, 1,
			(node_dtor_t)block_destroy);		/* destroy blocks */
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 1, free); /* destroy unspent */
	free(blockchain);							/* free blockchain */
	blockchain = NULL;							/* nullify blockchain */
}
