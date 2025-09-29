#include "blockchain.h"

/**
 * block_hash -		generates the SHA256 hash of a block
 * @block:			block pointer
 * @hash_buf:		output buffer
 * Return:			pointer to hash_buf or NULL on failure
 */
uint8_t *block_hash(
	block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	if (!block || !hash_buf)						/* check for NULL */
		return (NULL);

	SHA256((unsigned char *)&block->info,			/* hash info and data */
	sizeof(block->info) + block->data.len, hash_buf);

	return (hash_buf);								/* return hash buffer */
}
