#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <llist.h>

#include "blockchain.h"
#include "hblk_crypto.h"

/**
 * struct tx_out_s -			transaction output
 * @amount:						amount transferred
 * @pub:						recipient public key
 * @hash:						transaction output hash
 */
typedef struct tx_out_s
{
	uint32_t amount;
	uint8_t pub[EC_PUB_LEN];
	uint8_t hash[SHA256_DIGEST_LENGTH];
} tx_out_t;

/**
 * struct tx_in_s -				transaction input
 * @block_hash:					transaction block hash
 * @tx_id:						transaction ID
 * @tx_out_hash:				transaction output hash
 * @sig:						signature
 */
typedef struct tx_in_s
{
	uint8_t block_hash[SHA256_DIGEST_LENGTH];
	uint8_t tx_id[SHA256_DIGEST_LENGTH];
	uint8_t tx_out_hash[SHA256_DIGEST_LENGTH];
	sig_t sig;
} tx_in_t;

/**
 * struct unspent_tx_out_s -	unspent transaction output
 * @block_hash:					block hash
 * @tx_id:						transaction ID
 * @out:						transaction output
 */
typedef struct unspent_tx_out_s
{
	uint8_t block_hash[SHA256_DIGEST_LENGTH];
	uint8_t tx_id[SHA256_DIGEST_LENGTH];
	tx_out_t out;
} unspent_tx_out_t;

/**
 * struct transaction_s -		transaction
 * @inputs:						list of transaction inputs
 * @outputs:					list of transaction outputs
 * @id:							transaction ID
 */
typedef struct transaction_s
{
	llist_t *inputs;
	llist_t *outputs;
	uint8_t id[SHA256_DIGEST_LENGTH];
} transaction_t;

tx_out_t *tx_out_create(
	uint32_t amount,
	uint8_t const pub[EC_PUB_LEN]);
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH],
	tx_out_t const *out);
tx_in_t *tx_in_create(
	unspent_tx_out_t const *unspent);
uint8_t *transaction_hash(
	transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH]);

#endif /* TRANSACTION_H */
