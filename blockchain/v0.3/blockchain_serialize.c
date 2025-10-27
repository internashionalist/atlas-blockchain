#include "blockchain.h"

int write_field(FILE *file, void const *buf, size_t size, int swap);
int write_block(FILE *file, block_t const *block, uint32_t len, int swap);

/**
 * write_field -			writes raw bytes to file and
 *							swaps endianness if needed
 * @file:					file stream to write to
 * @buf:					pointer to data
 * @size:					number of bytes to write
 * @swap:					swap flag
 * Return:					1 on success, 0 on failure
 */
int write_field(FILE *file, void const *buf, size_t size, int swap)
{
	uint64_t tmp;									/* temp storage */

	if (swap && size > 1)							/* if swap needed */
	{
		memcpy(&tmp, buf, size);					/* copy data to temp */
		_swap_endian(&tmp, size);					/* swap endianness */
		return (fwrite(&tmp, size, 1, file) == 1);	/* write swapped data */
	}
	return (fwrite(buf, 1, size, file) == size);	/* otherwise write as is */
}

/**
 * write_block -			writes one block to file
 * @file:					file stream to write to
 * @block:					block pointer
 * @len:					length of block data
 * @swap:					endianness flag
 * Return:					1 on success, 0 on failure
 */
int write_block(
	FILE *file, block_t const *block, uint32_t len, int swap)
{
	if (!write_field(file, &block->info.index,			/* write block info */
			sizeof(block->info.index), swap) ||
	    !write_field(file, &block->info.difficulty,
			sizeof(block->info.difficulty), swap) ||
	    !write_field(file, &block->info.timestamp,
			sizeof(block->info.timestamp), swap) ||
	    !write_field(file, &block->info.nonce,
			sizeof(block->info.nonce), swap) ||
	    !write_field(file, block->info.prev_hash,
			SHA256_DIGEST_LENGTH, 0) ||
	    !write_field(file, &len, sizeof(len), swap) ||
			(len && !write_field(file, block->data.buffer, len, 0)) ||
			!write_field(file, block->hash, SHA256_DIGEST_LENGTH, 0))
		return (0);

	return (1);
}

/**
 * blockchain_serialize -	writes a blockchain to file
 * @blockchain:				blockchain pointer
 * @path:					file path to write to
 * Return:					0 on success, otherwise -1
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *file;											/* file pointer */
	uint8_t endian;										/* endianness */
	uint32_t count, i, len;				/* number of blocks, index, length */
	int swap;										/* endianness swap flag */
	block_t const *block;							/* curr block pointer */

	if (!blockchain || !path)
		return (-1);
	count = (uint32_t)llist_size(blockchain->chain);	/* get block count */
	if (!count)
		return (-1);
	file = fopen(path, "wb");						/* open file to write */
	if (!file)
		return (-1);
	endian = _get_endianness();					/* get system endianness */
	swap = (endian == 2);								/* set swap flag */
	if (!write_field(file, HBLK, 4, 0) ||				/* write header */
	    !write_field(file, VERS, 3, 0) ||
	    !write_field(file, &endian, 1, 0) ||
	    !write_field(file, &count, sizeof(count), swap))
		goto fail;
	for (i = 0; i < count; i++)						/* iterate over blocks */
	{
		block = llist_get_node_at(blockchain->chain, i);	/* get block */
		if (!block)
			goto fail;
		len = block->data.len;							/* get data length */
		if (len > BLOCKCHAIN_DATA_MAX)						/* check length */
			len = BLOCKCHAIN_DATA_MAX;
		if (!write_block(file, block, len, swap))			/* write block */
			goto fail;
	}
	fclose(file);
	return (0);												/* success */
fail:
	fclose(file);
	return (-1);
}
