#include "transaction.h"

/**
 * collect_sender_unspent -		gathers up sender's unspent outputs
 * @amount:						amount to gather
 * @total:						running total gathered so far
 * @sender_pub:					sender's public key
 * @all_unspent:				list of all unspent outputs
 *
 * Return:						pointer to list or NULL on failure
 */
static llist_t *collect_sender_unspent(
	uint32_t amount,
	uint32_t *total,
	uint8_t sender_pub[EC_PUB_LEN],
	llist_t *all_unspent)
{
	int idx, count;								/* loop variables */
	unspent_tx_out_t *unspent;					/* current unspent output */
	llist_t *selected;							/* selected outputs */

	count = llist_size(all_unspent);			/* get list size */
	if (count < 0)
		return (NULL);
	selected = llist_create(MT_SUPPORT_FALSE);	/* create selected list */
	if (!selected)
		return (NULL);
												/* gather unspent outputs */
	for (idx = 0; idx < count && *total < amount; idx++)
	{
		unspent = llist_get_node_at(all_unspent, idx);
		if (!unspent)
			goto fail;
												/* skip if not sender's */
		if (memcmp(unspent->out.pub, sender_pub, EC_PUB_LEN))
			continue;
												/* add to selected */
		if (unspent->out.amount > UINT32_MAX - *total ||
			llist_add_node(selected, unspent, ADD_NODE_REAR) == -1)
			goto fail;
		*total += unspent->out.amount;			/* update total */
	}
	if (*total < amount)						/* insufficient funds */
		goto fail;
	return (selected);							/* return selected list */
fail:											/* failure cleanup */
	llist_destroy(selected, 0, NULL);
	return (NULL);
}

/**
 * append_inputs -	adds inputs to transaction from selected unspent outputs
 * @transaction:	transaction being populated
 * @selected:		list of selected unspent outputs
 *
 * Return:			0 on success, -1 on failure
 */
static int append_inputs(
	transaction_t *transaction,
	llist_t *selected)
{
	int idx, count;							/* loop variables */
	unspent_tx_out_t *unspent;				/* current unspent output */
	tx_in_t *input;							/* created transaction input */

	count = llist_size(selected);			/* get list size */
	if (count < 0)
		return (-1);
	for (idx = 0; idx < count; idx++)		/* iterate selected list */
	{
		unspent = llist_get_node_at(selected, idx);	/* get unspent output */
		if (!unspent)
			return (-1);
		input = tx_in_create(unspent);		/* create transaction input */
		if (!input)
			return (-1);
											/* append to transaction */
		if (llist_add_node(transaction->inputs, input, ADD_NODE_REAR) == -1)
		{
			free(input);
			return (-1);
		}
	}
	return (0);								/* successful input append */
}

/**
 * append_outputs -		adds outputs to transaction
 * @transaction:		transaction being appended to
 * @amount:				amount to send
 * @total:				total value from sender
 * @receiver_pub:		receiver public key
 * @sender_pub:			sender public key
 *
 * Return:				0 on success, -1 on failure
 */
static int append_outputs(
	transaction_t *transaction,
	uint32_t amount,
	uint32_t total,
	uint8_t receiver_pub[EC_PUB_LEN],
	uint8_t sender_pub[EC_PUB_LEN])
{
	tx_out_t *output;							/* receiver output */

	output = tx_out_create(amount, receiver_pub); /* create receiver output */
	if (!output)
		return (-1);
												/* append to transaction */
	if (llist_add_node(transaction->outputs, output, ADD_NODE_REAR) == -1)
	{
		free(output);
		return (-1);
	}
	if (total > amount)							/* if needed */
	{
		tx_out_t *out;							/* change output */

		out = tx_out_create(total - amount, sender_pub);
		if (!out)
			return (-1);
												/* append change output */
		if (llist_add_node(transaction->outputs, out, ADD_NODE_REAR) == -1)
		{
			free(out);
			return (-1);
		}
	}
	return (0);									/* successful output append */
}

/**
 * sign_inputs -		signs each transaction input
 * @transaction:		transaction to sign inputs for
 * @sender:				sender key pair
 * @all_unspent:		list of current unspent outputs
 *
 * Return:				0 on success, -1 on failure
 */
static int sign_inputs(
	transaction_t *transaction,
	EC_KEY const *sender,
	llist_t *all_unspent)
{
	int idx, count;							/* loop variables */
	tx_in_t *input;							/* current transaction input */

	count = llist_size(transaction->inputs); /* get list size */
	if (count < 0)
		return (-1);
	for (idx = 0; idx < count; idx++)		/* iterate inputs */
	{
		input = llist_get_node_at(transaction->inputs, idx); /* get input */
		if (!input ||
			!tx_in_sign(input, transaction->id, sender, all_unspent))
			return (-1);					/* sign input */
	}
	return (0);								/* successful signing */
}

/**
 * transaction_create -		creates a new transaction
 * @sender:					sender EC key pair
 * @receiver:				receiver EC key pair
 * @amount:					amount to transfer
 * @all_unspent:			list of all unspent transaction outputs
 *
 * Return:					pointer to created transaction on success
 *							or NULL on failure
 */
transaction_t *transaction_create(
	EC_KEY const *sender,
	EC_KEY const *receiver,
	uint32_t amount,
	llist_t *all_unspent)
{
	uint8_t sender_pub[EC_PUB_LEN], receiver_pub[EC_PUB_LEN]; /* pub keys */
	uint32_t total = 0;										/* total value */
	llist_t *selected = NULL;					/* selected unspent outputs */
	transaction_t *transaction = NULL;				/* created transaction */

	if (!sender || !receiver || !all_unspent || amount == 0 ||	/* checks */
		!ec_to_pub(sender, sender_pub) ||
		!ec_to_pub(receiver, receiver_pub))
		return (NULL);
											/* get selected unspent outputs */
	selected = collect_sender_unspent(amount, &total, sender_pub, all_unspent);
	if (!selected)
		return (NULL);
	transaction = calloc(1, sizeof(*transaction));	/* create transaction */
	if (!transaction)
		goto fail;
	transaction->inputs = llist_create(MT_SUPPORT_FALSE);	/* input list */
	transaction->outputs = llist_create(MT_SUPPORT_FALSE);	/* output list */
	if (!transaction->inputs || !transaction->outputs)
		goto fail;
	if (append_inputs(transaction, selected) == -1 || /* + inputs/outputs */
		append_outputs(
			transaction, amount, total, receiver_pub, sender_pub) == -1)
		goto fail;
	if (!transaction_hash(transaction, transaction->id) ||	 /* hash txn */
		sign_inputs(transaction, sender, all_unspent) == -1) /* sign inputs */
		goto fail;
	llist_destroy(selected, 0, NULL);						/* cleanup list */
	return (transaction);							/* created transaction */

fail:							/* space-saving cleanup protocols for betty */
	if (selected)
		llist_destroy(selected, 0, NULL);
	if (transaction && transaction->inputs)
		llist_destroy(transaction->inputs, 1, free);
	if (transaction && transaction->outputs)
		llist_destroy(transaction->outputs, 1, free);
	free(transaction);
	return (NULL);
}
