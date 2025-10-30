#include "transaction.h"

/**
 * tx_in_create -	allocates and initializes a transaction input structure
 * @unspent:		unspent transaction output being referenced
 *
 * Return:			pointer to new transaction input or NULL on failure
 */
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent)
{
	tx_in_t *tx_in;								/* transaction input */

	if (!unspent)
		return (NULL);

	tx_in = calloc(1, sizeof(*tx_in));			/* allocate memory */
	if (!tx_in)
		return (NULL);
												/* init/set fields */
	memcpy(tx_in->block_hash, unspent->block_hash, SHA256_DIGEST_LENGTH);
	memcpy(tx_in->tx_id, unspent->tx_id, SHA256_DIGEST_LENGTH);
	memcpy(tx_in->tx_out_hash, unspent->out.hash, SHA256_DIGEST_LENGTH);

	memset(&tx_in->sig, 0, sizeof(tx_in->sig));	/* init signature */

	return (tx_in);								/* new transaction input */
}
