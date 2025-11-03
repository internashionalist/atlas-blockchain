#include "transaction.h"

/**
 * coinbase_is_valid -		checks whether a coinbase transaction is valid
 * @coinbase:				pointer to transaction being checked
 * @block_index:			index of block containing the transaction
 *
 * Return:					1 if valid, 0 otherwise
 */
int coinbase_is_valid(
	transaction_t const *coinbase,
	uint32_t block_index)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];		   /* computed hash buffer */
	tx_in_t *input;								   /* transaction input */
	tx_out_t *output;							   /* transaction output */
	uint8_t zero_hash[SHA256_DIGEST_LENGTH] = {0}; /* zeroed hash */
	uint8_t zero_sig[SIG_MAX_LEN] = {0};		   /* zeroed signature */

	if (!coinbase) /* no transaction */
		return (0);
	/* verify hash & ID */
	if (!transaction_hash(coinbase, hash_buf) ||
		memcmp(hash_buf, coinbase->id, SHA256_DIGEST_LENGTH) != 0)
		return (0);
	/* check input/output counts */
	if (llist_size(coinbase->inputs) != 1 ||
		llist_size(coinbase->outputs) != 1)
		return (0);
	/* get input/output */
	input = llist_get_node_at(coinbase->inputs, 0);
	output = llist_get_node_at(coinbase->outputs, 0);
	if (!input || !output)
		return (0);
	/* check input against block index */
	if (memcmp(input->tx_out_hash, &block_index, sizeof(block_index)) != 0)
		return (0);
	/* check that other input fields are zeroed */
	if (memcmp(input->block_hash, zero_hash, SHA256_DIGEST_LENGTH) != 0 ||
		memcmp(input->tx_id, zero_hash, SHA256_DIGEST_LENGTH) != 0 ||
		input->sig.len != 0 ||
		memcmp(input->sig.sig, zero_sig, SIG_MAX_LEN) != 0)
		return (0);

	if (output->amount != COINBASE_AMOUNT) /* check output amount */
		return (0);

	return (1); /* verified */
}
