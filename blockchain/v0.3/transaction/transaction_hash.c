#include "transaction.h"

/*
 * copy_inputs -	copies transaction input data to a buffer
 * @inputs:			list of transaction inputs
 * @count:			number of inputs to copy
 * @buffer:			buffer to copy data into
 * @offset:			offset into the buffer
 *
 * Return:			0 on success, -1 on failure
 */
static int copy_inputs(
	llist_t *inputs, int count, uint8_t *buffer, size_t *offset)
{
	unsigned int i;										/* index */
	tx_in_t *in;										/* tx input */

	if (!inputs)										/* if no list */
	{
		if (count == 0)									/* if no inputs */
			return (0);									/* success-ish */
		return (-1);
	}

	for (i = 0; i < (unsigned int)count; i++)			/* iterate inputs */
	{
		in = llist_get_node_at(inputs, i);				/* get input */
		if (!in)
			return (-1);
														/* copy input fields */
		memcpy(buffer + *offset, in->block_hash, SHA256_DIGEST_LENGTH);
		*offset += SHA256_DIGEST_LENGTH;				/* update */
		memcpy(buffer + *offset, in->tx_id, SHA256_DIGEST_LENGTH);
		*offset += SHA256_DIGEST_LENGTH;
		memcpy(buffer + *offset, in->tx_out_hash, SHA256_DIGEST_LENGTH);
		*offset += SHA256_DIGEST_LENGTH;
	}
	return (0);
}


/*
 * copy_outputs -	copies transaction output data to a buffer
 * @outputs:		list of transaction outputs
 * @count:			number of outputs to copy
 * @buffer:			buffer to copy data into
 * @offset:			offset into the buffer
 *
 * Return:			0 on success, -1 on failure
 */
static int copy_outputs(
	llist_t *outputs, int count, uint8_t *buffer, size_t *offset)
{
	unsigned int i;										/* index */
	tx_out_t *out;										/* tx output */

	if (!outputs)										/* if no list */
	{
		if (count == 0)
			return (0);
		return (-1);
	}

	for (i = 0; i < (unsigned int)count; i++)			/* iterate outputs */
	{
		out = llist_get_node_at(outputs, i);			/* get output */
		if (!out)
			return (-1);
														/* copy output hash */
		memcpy(buffer + *offset, out->hash, SHA256_DIGEST_LENGTH);
		*offset += SHA256_DIGEST_LENGTH;				/* update offset */
	}
	return (0);
}

/**
 * transaction_hash -	computes the ID hash of a transaction
 * @transaction:		pointer to transaction data
 * @hash_buf:			buffer in which to store the resulting hash
 *
 * Return:				pointer to hash buffer or NULL on failure
 */
uint8_t *transaction_hash(
	transaction_t const *transaction, uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *buffer = NULL;								/* data buffer */
	size_t total_len = 0, offset = 0;					/* lengths */
	int in_count, out_count;							/* counts */
	const int8_t *data;									/* data for hashing */

	if (!transaction || !hash_buf)						/* check inputs */
		return (NULL);
	in_count = 0;										/* count inputs */
	if (transaction->inputs)
		in_count = llist_size(transaction->inputs);
	out_count = 0;										/* count outputs */
	if (transaction->outputs)
		out_count = llist_size(transaction->outputs);
	if (in_count < 0 || out_count < 0)					/* check counts */
		return (NULL);
														/* get total length */
	total_len = (size_t)(in_count * 3 + out_count) * SHA256_DIGEST_LENGTH;
	if (total_len)										/* allocate buffer */
	{
		buffer = malloc(total_len);						/* data buffer */
		if (!buffer)
			return (NULL);								/* copy ins/outs */
		if (copy_inputs(transaction->inputs, in_count, buffer, &offset) ||
			copy_outputs(transaction->outputs, out_count, buffer, &offset))
		{
			free(buffer);
			return (NULL);
		}
	}
	data = total_len ? (int8_t const *)buffer : NULL;	/* data to hash */

	if (!sha256(data, total_len, hash_buf))				/* compute hash */
	{
		free(buffer);
		return (NULL);
	}
	free(buffer);
	return (hash_buf);									/* computed hash */
}
