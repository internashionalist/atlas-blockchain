#include "transaction.h"

/**
 * transaction_destroy -		deallocates a transaction structure
 * @transaction:				transaction to deallocate
 *
 * Return:						none
 */
void transaction_destroy(transaction_t *transaction)
{
	if (!transaction)						/* nothing to destroy */
		return;

	if (transaction->inputs)				/* free inputs */
		llist_destroy(transaction->inputs, 1, free);

	if (transaction->outputs)				/* free outputs */
		llist_destroy(transaction->outputs, 1, free);

	free(transaction);						/* free structure */
}
