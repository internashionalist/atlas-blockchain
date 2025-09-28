#include "blockchain.h"

/**
 * blockchain_destroy -	frees and nullifies a blockchain
 * @blockchain:			pointer to blockchain to annihilate
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	llist_destroy(blockchain->chain, 1, NULL);	/* destroy list and blocks */
	free(blockchain);							/* free blockchain struct */
	blockchain = NULL;							/* nullify pointer */
}
