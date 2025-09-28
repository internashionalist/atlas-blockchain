#include "blockchain.h"

/**
 * block_create -	creates a new block following a previous block
 * @prev:			pointer to previous block in chain
 * @data:			pointer to data to duplicate in new block
 * @data_len:		number of bytes of data to duplicate
 *
 * Return:			pointer to new block or NULL on failure
 */
block_t *block_create(
	block_t const *prev, int8_t const *data, uint32_t data_len)
{
	block_t *block;										/* new block rep */
	block_data_t block_data = {{0}, 0};					/* new block data */
	block_info_t block_info = {0, 0, time(NULL), 0, {0}}; /* new block info */

	block = malloc(sizeof(*block));					/* mem for new block */
	if (!block)
		return (NULL);

	if (prev != NULL)								/* previous block exists */
	{
		block_info.index = prev->info.index + 1;	/* set new block position */
		block_info.difficulty = prev->info.difficulty; /* copy difficulty */
		memcpy(block_info.prev_hash, prev->hash,
			sizeof(block_info.prev_hash));			/* copy previous hash */
	}

	if (data != NULL && data_len > 0)				/* data is valid */
	{
		if (data_len > BLOCKCHAIN_DATA_MAX)			/* cap data length */
			data_len = BLOCKCHAIN_DATA_MAX;
		memcpy(block_data.buffer, data, data_len);	/* copy block data */
		block_data.len = data_len;					/* set data length */
	}

	block->info = block_info;						/* set new block info */
	block->data = block_data;						/* set new block data */
	memset(block->hash, 0, sizeof(block->hash));	/* zero new block hash */

	return (block);									/* ptr to new block rep */
}
