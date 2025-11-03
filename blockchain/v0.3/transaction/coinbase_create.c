#include "transaction.h"

/**
 * create_input -				allocates, initializes, and adds input
 *								to a coinbase transaction
 * @transaction:				transaction being built
 * @block_index:				current block index
 *
 * Return:						0 on success, -1 on failure
 */
static int create_input(
	transaction_t *transaction,
	uint32_t block_index)
{
	tx_in_t *input;							/* new input */

	input = calloc(1, sizeof(*input));		/* create/allocate input */
	if (!input)
		return (-1);
											/* set input fields */
	memcpy(input->tx_out_hash, &block_index, sizeof(block_index));
											/* attach input to tx */
	if (llist_add_node(transaction->inputs, input, ADD_NODE_REAR) == -1)
	{
		free(input);
		return (-1);
	}

	return (0);								/* success! */
}
/**
 * create_output -				allocates, initializes, and adds output
 *								to a coinbase transaction
 * @transaction:				transaction being built
 * @receiver:					receiver's EC key pair
 *
 * Return:						0 on success, -1 on failure
 */
static int create_output(
	transaction_t *transaction,
	EC_KEY const *receiver)
{
	tx_out_t *output;						/* new output */
	uint8_t receiver_pub[EC_PUB_LEN];		/* receiver's public key */

	if (!ec_to_pub(receiver, receiver_pub))	/* get pub key */
		return (-1);
											/* create output */
	output = tx_out_create(COINBASE_AMOUNT, receiver_pub);
	if (!output)
		return (-1);
											/* attach output to tx */
	if (llist_add_node(transaction->outputs, output, ADD_NODE_REAR) == -1)
	{
		free(output);
		return (-1);
	}

	return (0);								/* success! */
}

/**
 * coinbase_create -			creates a coinbase transaction
 * @receiver:					receiver's EC key pair
 * @block_index:				current block index
 *
 * Return:						pointer to created transaction
 *								or NULL on failure
 */
transaction_t *coinbase_create(
	EC_KEY const *receiver,
	uint32_t block_index)
{
	transaction_t *transaction;				/* new coinbase transaction */

	if (!receiver)							/* no receiver */
		return (NULL);

	transaction = calloc(1, sizeof(*transaction));	/* create transaction */
	if (!transaction)
		return (NULL);
											/* create input & output lists */
	transaction->inputs = llist_create(MT_SUPPORT_FALSE);
	transaction->outputs = llist_create(MT_SUPPORT_FALSE);
											/* create input, output, & hash */
	if (!transaction->inputs || !transaction->outputs ||
		create_input(transaction, block_index) == -1 ||
		create_output(transaction, receiver) == -1 ||
		!transaction_hash(transaction, transaction->id))
	{
		llist_destroy(transaction->inputs, 1, free);
		llist_destroy(transaction->outputs, 1, free);
		free(transaction);
		return (NULL);
	}

	return (transaction);					/* shiny new coinbase tx */
}
