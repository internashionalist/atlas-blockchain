#include "transaction.h"

/**
 * unspent_tx_out_create -	creates an unspent transaction output
 * @block_hash:				block hash containing transaction
 * @tx_id:					transaction ID
 * @out:					transaction output
 *
 * Return:					pointer to new unspent transaction output
 *							or NULL on failure
 */
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH],
	tx_out_t const *out)
