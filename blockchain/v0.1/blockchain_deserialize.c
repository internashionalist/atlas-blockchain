#include "blockchain.h"

/**
 * blockchain_deserialize - load a blockchain from disk
 * @path: serialized file path
 * Return: pointer to blockchain on success, NULL otherwise
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file;													/* stream */
	blockchain_t *blockchain = NULL;					/* blockchain rep */
	block_t *block = NULL;									/* block rep */
	uint8_t magic[4], version[3];							/* file header */
	uint32_t count = 0, i;							/* block count, index */

	if (!path)												/* path check */
		return (NULL);
	file = fopen(path, "read");						/* open file to read */
	if (!file)
		return (NULL);
	if (!fread(magic, 1, 4, file) || memcmp(magic, HBLK, 4) != 0 ||
	    !fread(version, 1, 3, file) || memcmp(version, VERS, 3) != 0)
		return (fclose(file), NULL);				/* read & check header */
	blockchain = malloc(sizeof(*blockchain));		/* mem for blockchain */
	if (!blockchain)
		return (fclose(file), NULL);
	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain)						/* mem for blockchain list */
		return (free(blockchain), fclose(file), NULL);
	for (i = 0; i < 3; i++)					/* check version for each byte */
		if (version[i] != VERS[i])
			return (blockchain_destroy(blockchain), fclose(file), NULL);
	for (fread(&count, sizeof(count), 1, file); i < count; i++)
	{													/* read each block */
		block = malloc(sizeof(*block));					/* mem for block */
		if (!block)
			return (blockchain_destroy(blockchain), fclose(file), NULL);
		fread(&block->info, sizeof(block_info_t), 1, file);			/* info */
		fread(&block->data.len, sizeof(block->data.len), 1, file);
		if (block->data.len > BLOCKCHAIN_DATA_MAX)			/* check len */
			block->data.len = BLOCKCHAIN_DATA_MAX;
		fread(block->data.buffer, block->data.len, 1, file);		/* data */
		fread(block->hash, SHA256_DIGEST_LENGTH, 1, file);			/* hash */
		llist_add_node(blockchain->chain, block, ADD_NODE_REAR);  /* add it */
	}
	fclose(file);
	return (blockchain);							/* return blockchain ptr */
}
