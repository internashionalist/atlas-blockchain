#include <stdlib.h>
#include <string.h>
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
	tx_out_t *output;

	if (pub == NULL || amount == 0)
		return (NULL);
	output = malloc(sizeof(*output));
	if (!output)
		return (NULL);
	memset(output, 0, sizeof(*output));
	output->amount = amount;
	memcpy(output->pub, pub, EC_PUB_LEN);
	if (!SHA256(
		(uint8_t *)output, sizeof(uint32_t) + EC_PUB_LEN, output->hash))
	{
		free(output);
		return (NULL);
	}
	return (output);
}
