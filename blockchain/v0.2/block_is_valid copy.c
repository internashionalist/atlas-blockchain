#include "blockchain.h"

int genesis_checker(block_t const *block);

/**
 * genesis_checker -	checks if a block is the genesis block
 * @block:				block to compare
 * Return:				0 if it's Phil Collins, otherwise -1
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
 * block_is_valid -		validates a block against previous block
 * @block:				block to validate
 * @prev_block:			previous block in chain (NULL if genesis)
 * Return:				0 if block is valid, otherwise -1
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];				/* current block hash */
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];		/* previous block hash */

	if (!block)										/* NULL block */
		return (-1);

	if (block->data.len > BLOCKCHAIN_DATA_MAX)		/* data len too long */
		return (-1);

	if (block->info.index == GENESIS_INDEX)			/* idx matches genesis */
	{
		if (genesis_checker(block) == 0)			/* and is valid genesis */
			return (0);
	}
	else											/* not genesis block */
	{
		if (!prev_block)							/* NULL prev block */
			return (-1);

		if (block->info.index != prev_block->info.index + 1)
			return (-1);							/* invalid index */

		if (!block_hash(prev_block, prev_hash) ||	/* invalid prev hash */
			memcmp(prev_hash, prev_block->hash, SHA256_DIGEST_LENGTH) != 0 ||
			memcmp(prev_hash, block->info.prev_hash, SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}

	if (!block_hash(block, hash) ||				/* invalid current block hash */
		memcmp(hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);

	return (0);							/* if all checks out, block is valid */
}
