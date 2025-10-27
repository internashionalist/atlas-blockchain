#include "blockchain.h"

/**
 * blockchain_difficulty -	computes difficulty to assign to next block
 * @blockchain:			blockchain container to inspect
 *
 * Return:			difficulty value for next block or 0 on error
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	llist_t *chain;					/* chain of blocks */
	block_t *latest_blk;			/* most recent block */
	block_t *ref_block;				/* reference block */
	uint32_t latest_index;			/* index of latest block */
	uint32_t difficulty;			/* difficulty to return */
	uint32_t expected;				/* expected elapsed time */
	uint64_t actual;				/* real elapsed time */

	if (!blockchain)
		return (0);
	chain = blockchain->chain;					/* get chain pointer */
	if (!chain)
		return (0);
	latest_blk = llist_get_tail(chain);			/* locate latest block */
	if (!latest_blk)
		return (0);
	latest_index = latest_blk->info.index;		/* store its index */
	if (latest_index == 0 ||					/* genesis || invalid index */
		latest_index % DIFFICULTY_ADJUSTMENT_INTERVAL != 0 ||
		llist_size(chain) < DIFFICULTY_ADJUSTMENT_INTERVAL)
		return (latest_blk->info.difficulty);	/* no adjustment needed */
	ref_block = llist_get_node_at(chain,		/* get reference block */
		llist_size(chain) - DIFFICULTY_ADJUSTMENT_INTERVAL);
	if (!ref_block)								/* no reference block */
		return (latest_blk->info.difficulty);	/* no adjustment needed */
	difficulty = latest_blk->info.difficulty;	/* start at current diff */
	expected = EXPECTED_BLOCK_INTERVAL();		/* set expected time */
	if (latest_blk->info.timestamp <= ref_block->info.timestamp)
		actual = 0;								/* prevent underflow */
	else										/* compute actual time */
		actual = ACTUAL_BLOCK_INTERVAL(latest_blk, ref_block);
	if (actual * 2 < expected)					/* mining too quickly */
		difficulty++;							/* increase difficulty */
	else if (actual > expected * 2 && difficulty > 0)	/* too slowly */
		difficulty--;							/* decrease difficulty */
	return (difficulty);					/* end adjusted difficulty */
}
