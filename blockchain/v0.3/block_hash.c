#include "blockchain.h"

/**
 * add_transaction_hash -	helper to add transaction hash to SHA256 context
 * @node:					node containing transaction
 * @idx:					index of node in list
 * @arg:					pointer to SHA256_CTX
 *
 * Return:					0 on success, -1 on failure
 */
static int add_transaction_hash(
	llist_node_t node,
	unsigned int idx,
	void *arg)
{
	SHA256_CTX *sha = arg;							/* SHA256 context */
	transaction_t const *transaction = node;		/* transaction node */
	uint8_t tx_hash[SHA256_DIGEST_LENGTH];			/* transaction hash */

	(void)idx;										/* unused parameter */
													/* get transaction hash */
	if (!transaction_hash(transaction, tx_hash) ||
		!SHA256_Update(sha, tx_hash, SHA256_DIGEST_LENGTH))
		return (-1);
	return (0);
}

/**
 * block_hash -				generates the SHA256 hash of a block
 * @block:					block pointer
 * @hash_buf:				output buffer
 *
 * Return:					pointer to hash_buf or NULL on failure
 */
uint8_t *block_hash(
	block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctx;									/* SHA256 context */
	int tx_count = 0;								/* transaction count */

	if (!block || !hash_buf)						/* check for NULL */
		return (NULL);

	if (!SHA256_Init(&ctx))							/* initialize SHA256 */
		return (NULL);
													/* hash block info */
	if (!SHA256_Update(&ctx, &block->info, sizeof(block->info)))
		return (NULL);
													/* hash block data */
	if (block->data.len && !SHA256_Update(&ctx,
		block->data.buffer, block->data.len))
		return (NULL);

	if (block->transactions)						/* hash transactions */
	{
		tx_count = llist_size(block->transactions);	/* get tx count */
		if (tx_count < 0)
			return (NULL);
													/* hash each tx */
		if (tx_count &&
			llist_for_each(block->transactions,
				add_transaction_hash, &ctx) != 0)
			return (NULL);
	}

	if (!SHA256_Final(hash_buf, &ctx))				/* finalize hash */
		return (NULL);

	return (hash_buf);								/* return hash buffer */
}
