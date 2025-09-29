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
	SHA256_CTX context;							/* SHA256 context */
	size_t data_len;							/* length of block data */

	if (!block || !hash_buf)					/* validate inputs */
		return (NULL);

	if (SHA256_Init(&context) != 1)				/* init context */
		return (NULL);

	if (SHA256_Update(&context, &block->info,	/* hash block info */
		sizeof(block->info)) != 1)
		return (NULL);

	data_len = block->data.len;					/* get data length */
	if (data_len > BLOCKCHAIN_DATA_MAX)			/* cap data length */
		data_len = BLOCKCHAIN_DATA_MAX;
	if (data_len &&								/* update context */
	    SHA256_Update(&context, block->data.buffer, data_len) != 1)
		return (NULL);

	if (SHA256_Final(hash_buf, &context) != 1)	/* finalize hash */
		return (NULL);

	return (hash_buf);							/* return hash buffer */
}
