
#include "transaction/transaction.h"
#include "blockchain.h"

int genesis_checker(block_t const *block);

/**
 * genesis_checker -			checks if a block is the genesis block
 * @block:						block to compare
 *
 * Return:						0 if it's Phil Collins, otherwise -1
 */
int genesis_checker(block_t const *block)
{
	uint8_t const zeros[SHA256_DIGEST_LENGTH] = {0};	/* all-zero array */

	if (block->info.index != GENESIS_INDEX ||		/* check genesis fields */
		block->info.difficulty != 0 ||
		block->info.nonce != 0 ||
		block->info.timestamp != GENESIS_TIMESTAMP ||
		memcmp(block->info.prev_hash, zeros, SHA256_DIGEST_LENGTH) != 0)
		return (-1);

	if (block->data.len != GENESIS_DATA_LEN ||		/* check genesis data */
		memcmp(block->data.buffer, "Holberton School", GENESIS_DATA_LEN) != 0)
		return (-1);

	if (memcmp(block->hash, HLBTN_HASH, SHA256_DIGEST_LENGTH) != 0)
		return (-1);								/* check genesis hash */

	return (0);										/* PHIL COLLINS PLAYS */
}

/**
 * validate_prev - ]				validates previous block linkage
 * @block:							block being verified
 * @prev_block:						expected previous block
 * @prev_hash:						buffer to hold previous hash
 *
 * Return:							0 on success, -1 otherwise
 */
static int validate_prev(
	block_t const *block,
	block_t const *prev_block,
	uint8_t prev_hash[SHA256_DIGEST_LENGTH])
{
													/* check previous block */
	if (!prev_block ||
		block->info.index != prev_block->info.index + 1 ||
		!block_hash(prev_block, prev_hash) ||
		memcmp(prev_hash, prev_block->hash, SHA256_DIGEST_LENGTH) != 0 ||
		memcmp(prev_hash, block->info.prev_hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);
	return (0);
}

/**
 * validate_transactions -				ensures block transactions are valid
 * @block:								block being verified
 * @all_unspent:						list of current unspent outputs
 *
 * Return:								0 on success, -1 otherwise
 */
static int validate_transactions(
	block_t const *block,
	llist_t *all_unspent)
{
	int tx_count, idx;								/* loop variables */
	transaction_t *coinbase, *transaction;			/* transaction pointers */

	if (!block->transactions)
		return (-1);
	tx_count = llist_size(block->transactions);		/* get transaction count */
	if (tx_count <= 0)
		return (-1);

	coinbase = llist_get_node_at(block->transactions, 0); /* get coinbase tx */
	if (!coinbase || !coinbase_is_valid(coinbase, block->info.index))
		return (-1);

	if (tx_count == 1)								/* only coinbase present */
		return (0);
	if (!all_unspent)								/* no unspent outputs */
		return (-1);
	for (idx = 1; idx < tx_count; idx++)			/* validate transactions */
	{
		transaction = llist_get_node_at(block->transactions, idx);
		if (!transaction || !transaction_is_valid(transaction, all_unspent))
			return (-1);
	}
	return (0);
}

/**
 * block_is_valid -				validates a block against previous block
 * @block:						block to validate
 * @prev_block:					previous block in chain (NULL if genesis)
 * @all_unspent:				list of all currently unspent outputs
 *
 * Return:						0 if block is valid, otherwise -1
 */
int block_is_valid(
	block_t const *block,
	block_t const *prev_block,
	llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];			/* computed block hash */
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];	/* previous block hash */

	if (!block || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);

	if (block->info.index == GENESIS_INDEX)		/* check for genesis block */
		return (genesis_checker(block) == 0 ? 0 : -1);

	if (validate_prev(block, prev_block, prev_hash) != 0 || /* validate prev */
		validate_transactions(block, all_unspent) != 0)		/* validate txs */
		return (-1);

	if (!block_hash(block, hash) ||				/* compute block hash */
		memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0) /* comp hashes */
		return (-1);

	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (-1);							/* check hash vs difficulty */

	return (0);
}
