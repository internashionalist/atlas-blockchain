#include "blockchain.h"

/**
 * block_mine -		hashes block until hash meets difficulty, then stores
 *					winning hash in block
 * @block:			block to mine pointer
 *
 * Return:			none
 */
void block_mine(block_t *block)
{
	if (!block)										/* NULL block */
		return;

	while (1)										/* infinite loop */
	{
		block_hash(block, block->hash);				/* hash block */
		if (hash_matches_difficulty(
			block->hash, block->info.difficulty))	/* compare hash to diff */
			break;									/* until match found */
		block->info.nonce++;					/* increment nonce & retry */
	}
}
