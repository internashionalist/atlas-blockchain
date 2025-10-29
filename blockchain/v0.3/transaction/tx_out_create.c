#include "transaction.h"

/**
 * tx_out_create -	creates a transaction output
 * @amount:			amount to transfer
 * @pub:			recipient public key
 *
 * Return:			pointer to created transaction output
 *					or NULL on failure
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *output;						/* new transaction output */

	if (pub == NULL || amount == 0)			/* validate inputs */
		return (NULL);
	output = malloc(sizeof(*output));		/* allocate memory */
	if (!output)
		return (NULL);
	memset(output, 0, sizeof(*output));		/* initialize memory */
	output->amount = amount;				/* set amount */
	memcpy(output->pub, pub, EC_PUB_LEN);	/* set recipient pub key */
	if (!SHA256(
		(uint8_t *)output, sizeof(uint32_t) + EC_PUB_LEN, output->hash))
	{
		free(output);						/* compute hash*/
		return (NULL);
	}
	return (output);						/* return new tx output */
}
