#include "transaction.h"

/**
* find_matching_unspent -		finds matching unspent tx output for an input
* @all_unspent:					list of all current unspent transaction outputs
* @tx_input:					transaction input to match
*
* Return:						pointer to matching unspent output
*								or NULL on failure/not found
*/
static unspent_tx_out_t *find_matching_unspent(
	llist_t *all_unspent,
	tx_in_t const *tx_input)
{
	unspent_tx_out_t *potential;						/* potential match */
	int count, idx;										/* loop variables */

	if (!all_unspent || !tx_input)						/* input checks */
		return (NULL);

	count = llist_size(all_unspent);					/* get list size */
	if (count < 0)										/* empty list */
		return (NULL);

	for (idx = 0; idx < count; idx++)					/* iterate list */
	{
		potential = llist_get_node_at(all_unspent, idx); /* get potential match */
		if (!potential)
			return (NULL);
														/* compare to input */
		if (!memcmp(
				potential->block_hash, tx_input->block_hash, SHA256_DIGEST_LENGTH) &&
			!memcmp(
				potential->tx_id, tx_input->tx_id, SHA256_DIGEST_LENGTH) &&
			!memcmp(
				potential->out.hash, tx_input->tx_out_hash, SHA256_DIGEST_LENGTH))
			return (potential);							/* found a match! */
	}

	return (NULL);										/* no match found */
}

/**
* process_inputs -				processes transaction inputs for validity
* @transaction:					pointer to transaction
* @all_unspent:					list of all unspent transaction outputs
* @total_in:					pointer to total input amount accumulator
*
* Return:						1 on success, 0 on failure
*/
static int process_inputs(
	transaction_t const *transaction,

	llist_t *all_unspent,
	uint64_t *total_in)
{
	int idx, count;										/* loop variables */

	count = llist_size(transaction->inputs);			/* get list size */
	if (count < 0)
		return (0);

	for (idx = 0; idx < count; idx++)					/* iterate inputs */
	{
		tx_in_t *curr_in;								/* current input */
		unspent_tx_out_t *unspent;						/* matching unspent output */
		EC_KEY *pub_key;								/* from unspent output */

		curr_in = llist_get_node_at(transaction->inputs, idx); /* get input */
		if (!curr_in)
			return (0);

		unspent = find_matching_unspent(all_unspent, curr_in); /* find match */
		if (!unspent)
			return (0);

		pub_key = ec_from_pub(unspent->out.pub);		/* get pub key */
		if (!pub_key)
			return (0);
		if (!ec_verify(pub_key, transaction->id,		/* verify signature */
				SHA256_DIGEST_LENGTH, &curr_in->sig))
		{
			EC_KEY_free(pub_key);
			return (0);
		}
		EC_KEY_free(pub_key);

		if (*total_in > UINT64_MAX - unspent->out.amount) /* check overflow */
			return (0);
		*total_in += unspent->out.amount;				/* accumulate input amount */
	}

	return (1);											/* success! */
}

/**
* process_outputs -				processes transaction outputs for validity
* @transaction:					pointer to transaction
* @total_out:					pointer to total output amount accumulator
*
* Return:						1 on success, 0 on failure
*/
static int process_outputs(
	transaction_t const *transaction,
	uint64_t *total_out)
{
	int idx, count;										/* loop variables */

	count = llist_size(transaction->outputs);			/* get list size */
	if (count < 0)
		return (0);

	for (idx = 0; idx < count; idx++)					/* iterate outputs */
	{
		tx_out_t *out;

		out = llist_get_node_at(transaction->outputs, idx); /* get output */
		if (!out)
			return (0);
		if (*total_out > UINT64_MAX - out->amount)		/* check overflow */
			return (0);
		*total_out += out->amount;						/* accumulate output amount */
	}

	return (1);											/* success! */
}

/**
* transaction_is_valid -		validates a transaction
* @transaction:					pointer to transaction
* @all_unspent:					list of all unspent transaction outputs
*
* Return:						1 on success, 0 on failure
*/
int transaction_is_valid(
	transaction_t const *transaction,
	llist_t *all_unspent)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];				/* computed hash buffer */
	uint64_t total_in = 0, total_out = 0;				/* total amounts */

	if (!transaction || !all_unspent)					/* input checks */
		return (0);
														/* verify transaction ID */
	if (!transaction_hash(transaction, hash_buf) ||
		memcmp(hash_buf, transaction->id, SHA256_DIGEST_LENGTH))
		return (0);
														/* process inputs/outputs */
	if (!process_inputs(transaction, all_unspent, &total_in) ||
		!process_outputs(transaction, &total_out))
		return (0);

	return (total_in == total_out);						/* verify amounts match */
}
