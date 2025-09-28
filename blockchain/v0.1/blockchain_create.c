#include "blockchain.h"

/**
 * blockchain_create -	creates and initializes a blockchain
 *						with the genesis block
 *
 * Return:				pointer to the new blockchain
 *						or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain = NULL;			/* new blockchain container */
	block_t *genesis = NULL;					/* gen block representation */
	block_data_t gen_data = {"Holberton School", 16};	/* gen block data */
	block_info_t gen_info = {0, 0, 1537578000, 0, {0}};		/* gen block info */


	blockchain = malloc(sizeof(*blockchain));	/* allocate memory - chain */
	if (!blockchain)
		return (NULL);

	blockchain->chain = llist_create(MT_SUPPORT_TRUE);	/* create list */
	if (!blockchain->chain)
	{
		free(blockchain);
		return (NULL);
	}

	genesis = malloc(sizeof(*genesis));		/* allocate memory - gen block */
	if (!genesis)
	{
		llist_destroy(blockchain->chain, 0, NULL);	/* must destroy list */
		free(blockchain);							/* before freeing chain */
		return (NULL);
	}

	genesis->info = gen_info;						/* set genesis info */
	genesis->data = gen_data;						/* set genesis data */
	memcpy(genesis->hash, HLBTN_HASH, sizeof(genesis->hash)); /* copy genesis */
	if (llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) != 0)
	{												/* add genesis to chain */
		free(genesis);
		llist_destroy(blockchain->chain, 0, NULL);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);						/* ptr to new blockchain */
}
