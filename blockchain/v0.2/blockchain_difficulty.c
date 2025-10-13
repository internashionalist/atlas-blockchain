#include "blockchain.h"

/**
 * blockchain_difficulty -	computes difficulty to assign to next block
 * @blockchain:				blockchain container to inspect
 *
 * Return:					difficulty value for next block or 0 on error
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	llist_t *chain;								/* chain of blocks */
	block_t *latest_blk;						/* most recent block */
	block_t *ref_block;                         /* block to reference */
	uint32_t latest_index;						/* index of latest block */
	uint32_t difficulty;						/* difficulty to return */

	if (!blockchain)
		return (0);
	chain = blockchain->chain;					/* get chain pointer */
	if (!chain)
		return (0);
	latest_blk = llist_get_tail(chain);			/* locate latest block */
	if (!latest_blk)
		return (0);
	latest_index = latest_blk->info.index;		/* get latest index */
	if (latest_index % DIFFICULTY_ADJUSTMENT_INTERVAL != 0)
		return (latest_blk->info.difficulty);	/* no adjustment needed */
	if (latest_index < DIFFICULTY_ADJUSTMENT_INTERVAL)
		return (latest_blk->info.difficulty);	/* too few blocks */
	ref_block = llist_get_node_at(chain,
		latest_index - DIFFICULTY_ADJUSTMENT_INTERVAL);	/* locate ref block */
	if (!ref_block)
		return (latest_blk->info.difficulty);
	difficulty = latest_blk->info.difficulty;	/* current difficulty */
	if (ACTUAL_BLOCK_INTERVAL(latest_blk, ref_block)
		< EXPECTED_BLOCK_INTERVAL() / 2)		/* blocks mined too quickly */
		difficulty++;
	else if (ACTUAL_BLOCK_INTERVAL(latest_blk, ref_block)	/* too slowly */
		> EXPECTED_BLOCK_INTERVAL() * 2 && difficulty > 0)
		difficulty--;
	return (difficulty);						/* jusssssst right */
}
