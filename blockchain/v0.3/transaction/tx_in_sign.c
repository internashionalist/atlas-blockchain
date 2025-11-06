#include "transaction.h"

unspent_tx_out_t *find_unspent_match(
	llist_t *all_unspent,
	tx_in_t const *tx_input);

/**
 * find_unspent_match -		finds matching unspent tx output for an input
 * @all_unspent:			list of all current unspent transaction outputs
 * @tx_input:				transaction input to match
 *
 * Return:					pointer to matching unspent output
 *							or NULL on failure/not found
 */
unspent_tx_out_t *find_unspent_match(
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
											/* compare potential match to input */
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
 * tx_in_sign -			signs a transaction input with owner's private key
 * @tx_input:			pointer to transaction input to sign
 * @tx_id:				transaction ID being signed
 * @sender:				owner's EC key pair
 * @all_unspent:		list of unspent transaction outputs
 *
 * Return: pointer to signature on success, NULL on failure
 */
sig_t *tx_in_sign(
	tx_in_t *tx_input,
	uint8_t const tx_id[SHA256_DIGEST_LENGTH],
	EC_KEY const *sender,
	llist_t *all_unspent)
{
	unspent_tx_out_t *unspent;					/* matching unspent output */
	uint8_t pub[EC_PUB_LEN];					/* sender's public key */

	if (!tx_input || !tx_id || !sender || !all_unspent)
		return (NULL);
												/* find output match */
	unspent = find_unspent_match(all_unspent, tx_input);
	if (!unspent)
		return (NULL);
												/* verify ownership */
	if (!ec_to_pub(sender, pub))
		return (NULL);
												/* compare public keys */
	if (memcmp(pub, unspent->out.pub, EC_PUB_LEN) != 0)
		return (NULL);
												/* sign the input */
	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &tx_input->sig))
		return (NULL);

	return (&tx_input->sig);					/* return ptr to signature */
}
