#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "../v0.3/blockchain.h"

tx_out_t *tx_out_create(
	uint32_t amount, uint8_t const pub[EC_PUB_LEN]);
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH], uint8_t tx_id[
		SHA256_DIGEST_LENGTH], tx_out_t const *out);
tx_in_t *tx_in_create(
	unspent_tx_out_t const *unspent);
uint8_t *transaction_hash(
	transaction_t const *transaction, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);

#endif /* TRANSACTION_H */
