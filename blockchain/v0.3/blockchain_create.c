#include "blockchain.h"

/**
 * blockchain_cleanup -			helper to free blockchain on failure
 * @blockchain:					blockchain to free
 *
 * Return:						void
 */
static void blockchain_cleanup(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	if (blockchain->chain)
		llist_destroy(blockchain->chain, 0, NULL);
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 0, NULL);
	free(blockchain);
}

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
	memset(blockchain, 0, sizeof(*blockchain));	/* ensure clean slate */
	blockchain->unspent = llist_create(MT_SUPPORT_FALSE); /* create unspent */
	blockchain->chain = llist_create(MT_SUPPORT_TRUE);	/* create list */
	if (!blockchain->unspent || !blockchain->chain)
	{
		blockchain_cleanup(blockchain);
		return (NULL);
	}
	genesis = malloc(sizeof(*genesis));		/* allocate memory - gen block */
	if (!genesis)
	{
		blockchain_cleanup(blockchain);
		return (NULL);
	}
	genesis->info = gen_info;						/* set genesis info */
	genesis->data = gen_data;						/* set genesis data */
	genesis->transactions = NULL;					/* no transactions */
	memcpy(genesis->hash, HLBTN_HASH, sizeof(genesis->hash)); /* copy genesis */
	if (llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) != 0)
	{												/* add genesis to chain */
		free(genesis);
		blockchain_cleanup(blockchain);
		return (NULL);
	}
	return (blockchain);						/* ptr to new blockchain */
}
