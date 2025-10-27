#include "blockchain.h"

int read_field(FILE *file, void *buf, size_t size, int swap);
int read_block(FILE *file, block_t *block, int swap);
int read_header(FILE *file, uint32_t *count, int *swap);
int append_block(FILE *file, blockchain_t *blockchain, int swap);

/**
 * read_field -				reads raw bytes from file and
 *							swaps endianness if requested
 * @file:					file stream to read from
 * @buf:					pointer to data
 * @size:					number of bytes to write
 * @swap:					swap flag
 * Return:					1 on success, 0 on failure
 */
int read_field(FILE *file, void *buf, size_t size, int swap)
{
	if (fread(buf, 1, size, file) != size)			/* read data */
		return (0);
	if (swap && size > 1)							/* if swap needed */
		_swap_endian(buf, size);					/* swap endianness */
	return (1);
}

/**
 * read_block -				reads a single block from file
 * @file:					input stream
 * @block:					block pointer to fill
 * @swap:					swap flag
 * Return:					1 on success, otherwise 0
 */
int read_block(FILE *file, block_t *block, int swap)
{
	uint32_t len;									/* data length */

	if (!file || !block)							/* check inputs */
		return (0);
	if (!read_field(file, &block->info.index,		/* read block info */
			sizeof(block->info.index), swap) ||
	    !read_field(file, &block->info.difficulty,
			sizeof(block->info.difficulty), swap) ||
	    !read_field(file, &block->info.timestamp,
			sizeof(block->info.timestamp), swap) ||
	    !read_field(file, &block->info.nonce,
			sizeof(block->info.nonce), swap))
		return (0);
	if (fread(block->info.prev_hash, 1, SHA256_DIGEST_LENGTH, file) !=
	    SHA256_DIGEST_LENGTH)						/* read prev hash */
		return (0);
	if (!read_field(file, &block->data.len, sizeof(block->data.len), swap))
		return (0);									/* read data len */
	len = block->data.len;
	if (len > BLOCKCHAIN_DATA_MAX)					/* check data len */
		return (0);
	if (len && fread(block->data.buffer, 1, len, file) != len)
		return (0);									/* read data */
	if (fread(block->hash, 1, SHA256_DIGEST_LENGTH, file) !=
	    SHA256_DIGEST_LENGTH)						/* read block hash */
		return (0);
	return (1);
}

/**
 * read_header -			reads and validates the serialized file header
 * @file:					input stream
 * @count:					block count
 * @swap:					swap flag
 * Return:					1 on success, otherwise 0
 */
int read_header(FILE *file, uint32_t *count, int *swap)
{
	uint8_t magic[4], version[3], endian;				/* header fields */

	if (fread(magic, 1, sizeof(magic), file) != sizeof(magic) ||
	    memcmp(magic, HBLK, sizeof(magic)) != 0 ||
	    fread(version, 1, sizeof(version), file) != sizeof(version) ||
	    memcmp(version, VERS, sizeof(version)) != 0 ||
	    fread(&endian, 1, 1, file) != 1)				/* read header */
		return (0);
	if (endian != 1 && endian != 2)						/* check endianness */
		return (0);
	*swap = (_get_endianness() != endian);				/* set swap flag */
	if (!read_field(file, count, sizeof(*count), *swap) || *count == 0)
		return (0);										/* read block count */
	return (1);
}

/**
 * append_block -			reads next block from file
 *							and appends it to a blockchain
 * @file:					input stream
 * @blockchain:				blockchain container to append to
 * @swap:					swap flag
 * Return:					1 on success, otherwise 0
 */
int append_block(FILE *file, blockchain_t *blockchain, int swap)
{
	block_t *block;										/* new block */

	block = malloc(sizeof(*block));						/* mem for block */
	if (!block)
		return (0);
	memset(block, 0, sizeof(*block));					/* clear block */
	if (!read_block(file, block, swap))					/* read block info */
	{
		free(block);
		return (0);
	}
	if (llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == -1)
	{													/* append to chain */
		free(block);
		return (0);
	}
	return (1);
}

/**
 * blockchain_deserialize -	reads a blockchain from a serialized file
 * @path:					file path to read from
 *
 * Return:					pointer to blockchain on success
 *							otherwise NULL
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file = NULL;									/* stream */
	blockchain_t *blockchain = NULL;				/* blockchain container */
	uint32_t count = 0, i;								/* count, index */
	int swap;											/* swap flag */

	if (!path)											/* check inputs */
		return (NULL);
	file = fopen(path, "rb");							/* open file */
	if (!file)
		return (NULL);
	if (!read_header(file, &count, &swap))				/* read header */
		goto fail;
	blockchain = malloc(sizeof(*blockchain));		/* mem for blockchain */
	if (!blockchain)
		goto fail;
	memset(blockchain, 0, sizeof(*blockchain));			/* clear blockchain */
	blockchain->chain = llist_create(MT_SUPPORT_FALSE);	/* create ll */
	if (!blockchain->chain)
		goto fail;
	for (i = 0; i < count; i++)						/* read & append blocks */
		if (!append_block(file, blockchain, swap))
			goto fail;
	fclose(file);										/* close up shop */
	return (blockchain);						/* deserialized blockchain */
fail:
	if (file)
		fclose(file);										/* GO TO JAIL */
	if (blockchain)
		blockchain_destroy(blockchain);
	return (NULL);
}
