#include "blockchain.h"

/**
 * block_hash - Compute the SHA-256 hash of a block
 * @block: Pointer to the block to hash
 * @hash_buf: Buffer where the resulting hash is stored
 *
 * Return: Pointer to @hash_buf on success, NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX context;

	if (!block || !hash_buf)
		return (NULL);

	if (SHA256_Init(&context) != 1)
		return (NULL);

	if (SHA256_Update(&context, &block->info, sizeof(block->info)) != 1)
		return (NULL);

	if (SHA256_Update(&context, &block->data.len,
		sizeof(block->data.len)) != 1)
		return (NULL);

	if (block->data.len > 0)
	{
		size_t data_len = block->data.len;

		if (data_len > BLOCKCHAIN_DATA_MAX)
			data_len = BLOCKCHAIN_DATA_MAX;
		if (SHA256_Update(&context, block->data.buffer, data_len) != 1)
			return (NULL);
	}

	if (SHA256_Final(hash_buf, &context) != 1)
		return (NULL);

	return (hash_buf);
}
