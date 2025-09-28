#include "blockchain.h"
#include "provided/_genesis.c"

/**
 * blockchain_create -	creates and initializes a blockchain
 *						with the genesis block
 *
 * Return:				pointer to the new blockchain
 *						or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;					/* new blockchain container */
	block_t *genesis;							/* gen block representation */

	blockchain = malloc(sizeof(*blockchain));	/* allocate memory - chain */
	if (!blockchain)
		return (NULL);

	blockchain->chain = llist_create(MT_SUPPORT_TRUE);	/* create list */
	if (!blockchain->chain)
	{
		free(blockchain);
		return (NULL);
	}

	genesis = malloc(sizeof(*genesis));		/* allocate memory - genblk */
	if (!genesis)
	{
		llist_destroy(blockchain->chain, 0, NULL);	/* must destroy list */
		free(blockchain);							/* before freeing chain */
		return (NULL);
	}
	memcpy(genesis, &_genesis, sizeof(*genesis));	/* copy genesis */

	if (llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) != 0)
	{
		free(genesis);							/* add genesis block to chain */
		llist_destroy(blockchain->chain, 0, NULL);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);						/* ptr to new blockchain */
}
