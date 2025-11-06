#include "transaction.h"

int is_it_spent(
	unspent_tx_out_t const *unspent,
	llist_t *txs);
int append_outputs(
	llist_t *txs,
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	llist_t *updated);
int append_existing_unspent(
	llist_t *all_unspent,
	llist_t *transactions,
	llist_t *updated);

/**
 * is_it_spent -	checks if an unspent transaction output is spent
 * @unspent:		unspent transaction output to check
 * @txs:			list of transactions to check against
 *
 * Return:			1 if spent, 0 if not, -1 on failure
 */
int is_it_spent(
	unspent_tx_out_t const *unspent,
	llist_t *txs)
{
	int tx_idx, tx_count;							/* loop variables */

	tx_count = txs ? llist_size(txs) : 0;			/* get number of txs */
	if (tx_count < 0)
		return (-1);
	for (tx_idx = 0; tx_idx < tx_count; tx_idx++)	/* iterate through txs */
	{
		transaction_t *tx;							/* current tx */
		int in_idx, in_count;						/* loop variables */

		tx = llist_get_node_at(txs, tx_idx);		/* get current tx */
		if (!tx)
			return (-1);
													/* get number of inputs */
		in_count = tx->inputs ? llist_size(tx->inputs) : 0;
		if (in_count < 0)
			return (-1);
													/* iterate through inputs */
		for (in_idx = 0; in_idx < in_count; in_idx++)
		{
			tx_in_t *in;							/* current input */

			in = llist_get_node_at(tx->inputs, in_idx);
			if (!in)
				return (-1);
													/* check if spent */
			if (!memcmp(
					in->block_hash, unspent->block_hash, SHA256_DIGEST_LENGTH) &&
				!memcmp(
					in->tx_id, unspent->tx_id, SHA256_DIGEST_LENGTH) &&
				!memcmp(
					in->tx_out_hash, unspent->out.hash, SHA256_DIGEST_LENGTH))
				return (1);							/* spent */
		}
	}
	return (0);										/* unspent */
}

/**
 * append_outputs -				appends new unspent tx outs to updated list
 * @txs:						list of transactions
 * @block_hash:					block hash to use for unspent tx outs
 * @updated:					list to append unspent tx outs to
 *
 * Return:						0 on success, -1 on failure
 */
int append_outputs(
	llist_t *txs,
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	llist_t *updated)
{
	int tx_idx, tx_count;								/* loop variables */

	tx_count = txs ? llist_size(txs) : 0;				/* get number of txs */
	if (tx_count < 0)
		return (-1);
	for (tx_idx = 0; tx_idx < tx_count; tx_idx++)		/* iterate through txs */
	{
		transaction_t *tx;								/* current tx */
		int out_idx, out_count;							/* loop variables */

		tx = llist_get_node_at(txs, tx_idx);			/* get current tx */
		if (!tx)
			return (-1);
													/* get number of outputs */
		out_count = tx->outputs ? llist_size(tx->outputs) : 0;
		if (out_count < 0)
			return (-1);
													/* iterate through outputs */
		for (out_idx = 0; out_idx < out_count; out_idx++)
		{
			tx_out_t *out;							/* current output */
			unspent_tx_out_t *node;					/* new unspent tx out */

			out = llist_get_node_at(tx->outputs, out_idx);
			if (!out)
				return (-1);
													/* create unspent tx out */
			node = unspent_tx_out_create(block_hash, tx->id, out);

													/* append to updated list */
			if (!node || llist_add_node(updated, node, ADD_NODE_REAR) != 0)
			{
				free(node);
				return (-1);
			}
		}
	}
	return (0);										/* successful append */
}

/**
 * append_existing_unspent -	appends still unspent tx outs to updated list
 * @all_unspent:				list of all unspent tx outs
 * @transactions:				list of transactions to check against
 * @updated:					list to append unspent tx outs to
 *
 * Return:						0 on success, -1 on failure
 */
int append_existing_unspent(
	llist_t *all_unspent,
	llist_t *transactions,
	llist_t *updated)
{
	int un_idx, un_count;								/* loop variables */

												/* unspent tx outs count */
	un_count = all_unspent ? llist_size(all_unspent) : 0;
	if (un_count < 0)
		return (-1);
										/* iterate through unspent tx outs */
	for (un_idx = 0; un_idx < un_count; un_idx++)
	{
		unspent_tx_out_t *unspent;					/* current unspent tx out */
		unspent_tx_out_t *copy;						/* copy of unspent tx out */
		int spent;									/* spent status */

		unspent = llist_get_node_at(all_unspent, un_idx);
		if (!unspent)
			return (-1);

		spent = is_it_spent(unspent, transactions); /* check if spent */
		if (spent < 0)
			return (-1);
		if (spent)									/* skip if spent */
			continue;
													/* copy unspent tx out */
		copy = unspent_tx_out_create(
			unspent->block_hash, unspent->tx_id, &unspent->out);

													/* append to updated list */
		if (!copy || llist_add_node(updated, copy, ADD_NODE_REAR) != 0)
		{
			free(copy);
			return (-1);
		}
	}
	return (0);										/* successful append */
}

/**
 * update_unspent -			updates the list of unspent transaction outputs
 * @transactions:			list of new transactions
 * @block_hash:				block hash to use for new unspent tx outs
 * @all_unspent:			list of all unspent transaction outputs
 *
 * Return:					new list of unspent transaction outputs,
 *							or NULL on failure
 */
llist_t *update_unspent(
	llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	llist_t *all_unspent)
{
	llist_t *updated;							/* updated unspent list */

	if (!block_hash)							/* check for valid block hash */
		return (NULL);

	updated = llist_create(MT_SUPPORT_FALSE);	/* create new list */
	if (!updated)
		return (NULL);
												/* append still unspent tx outs */
	if (append_existing_unspent(all_unspent, transactions, updated) != 0)
		goto fail;
												/* append new unspent tx outs */
	if (append_outputs(transactions, block_hash, updated) != 0)
		goto fail;
												/* cleanup old list */
	if (all_unspent)
		llist_destroy(all_unspent, 1, free);

	return (updated);							/* updated list */

fail:
	llist_destroy(updated, 1, free);
	return (NULL);
}
