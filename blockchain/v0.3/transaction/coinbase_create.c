#include "transaction.h"

/**
 * coinbase_create -		creates a coinbase transaction
 * @receiver:				key pair of receiver
 * @block_index:			index of block containing transaction
 *
 * Return:					pointer to created transaction
 *							or NULL on failure
 */
transaction_t *coinbase_create(
	EC_KEY const *receiver,
	uint32_t block_index)
{
    transaction_t *transaction;					/* new transaction */
    tx_in_t *input;								/* transaction input */
    tx_out_t *output;							/* transaction output */
    uint8_t receiver_pub[EC_PUB_LEN];			/* receiver's public key */

    if (!receiver)								/* no receiver */
        return (NULL);

    transaction = calloc(1, sizeof(*transaction));	/* allocate transaction */
    if (!transaction)
        return (NULL);
											/* create input & output lists */
    transaction->inputs = llist_create(MT_SUPPORT_FALSE);
    transaction->outputs = llist_create(MT_SUPPORT_FALSE);
    if (!transaction->inputs || !transaction->outputs)
        goto fail;

    input = calloc(1, sizeof(*input));			/* create input */
    if (!input)
        goto fail;
												/* set input values */
    memcpy(input->tx_out_hash, &block_index, sizeof(block_index));
												/* add input to transaction */
    if (!llist_add_node(transaction->inputs, input, ADD_NODE_REAR))
    {
        free(input);
        goto fail;
    }

    if (!ec_to_pub(receiver, receiver_pub))		/* get receiver public key */
        goto fail;

    output = tx_out_create(COINBASE_AMOUNT, receiver_pub);	/* create output */
    if (!output)
        goto fail;
												/* add output to transaction */
    if (!llist_add_node(transaction->outputs, output, ADD_NODE_REAR))
    {
        free(output);
        goto fail;
    }
												/* compute transaction hash */
    if (!transaction_hash(transaction, transaction->id))
        goto fail;

    return (transaction);						/* successful tx creation */

fail:											/* fail cleanup*/
    if (transaction)
    {
        if (transaction->inputs)
            llist_destroy(transaction->inputs, 1, free);
        if (transaction->outputs)
            llist_destroy(transaction->outputs, 1, free);
        free(transaction);
    }
    return (NULL);
}
