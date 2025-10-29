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
{
	unspent_tx_out_t *unspent;					/* unspent tx output */

	if (!block_hash || !tx_id || !out)			/* validate inputs */
		return (NULL);

	unspent = malloc(sizeof(*unspent));			/* allocate memory */
	if (!unspent)
		return (NULL);

												/* init memory & set fields */
	memcpy(unspent->block_hash, block_hash, SHA256_DIGEST_LENGTH);
	memcpy(unspent->tx_id, tx_id, SHA256_DIGEST_LENGTH);
	memcpy(&unspent->out, out, sizeof(unspent->out));

	return (unspent);							/* return unspent tx output */
}
