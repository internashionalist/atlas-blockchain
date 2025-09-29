#include "blockchain.h"

/**
 * block_hash -		computes the SHA256 hash of a block's info and data
 * @block:			block pointer
 * @hash_buf:		output buffer
 * Return:			pointer to hash_buf or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX context;							/* SHA256 context */

	if (!block || !hash_buf)					/* validate inputs */
		return (NULL);

	if (SHA256_Init(&context) != 1)				/* init context */
		return (NULL);

	if (SHA256_Update(&context, &block->info,	/* hash block info */
		sizeof(block->info)) != 1)
		return (NULL);

	if (SHA256_Update(&context, &block->data,	/* hash block data */
		sizeof(block->data)) != 1)
		return (NULL);

	if (SHA256_Final(hash_buf, &context) != 1)	/* finalize hash */
		return (NULL);

	return (hash_buf);							/* return hash buffer */
}
