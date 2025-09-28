#ifndef _BLOCKCHAIN_H
#define _BLOCKCHAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <llist.h>
#include <openssl/sha.h>

#include "provided/endianness.h"

#define BLOCKCHAIN_DATA_MAX	1024
#define HLBTNN_HASH \
	"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d\x0c" \
	"\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03"

/* BLOCKCHAIN STRUCTURES */

/**
 * struct block_info_s -	metadata in a block
 * @index:					position of block in chain (starting at 0)
 * @difficulty:				Proof-of-Work difficulty
 * @timestamp:				time block created
 * @nonce:					nonce used for mining
 * @prev_hash:				hash of previous block (all 0s for GENESIS block)
 *
 * notes:	integers (> 1 byte) stored in little-endian format
 */
typedef struct block_info_s
{
	uint32_t index;
	uint32_t difficulty;
	uint64_t timestamp;
	uint64_t nonce;
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

/**
 * struct block_data_s -	data stored in a block
 * @buffer:					raw block data
 * @len:					number of bytes stored in buffer
 *
 * notes:	don't null-terminate buffer
 *			len must be <= BLOCKCHAIN_DATA_MAX
 */
typedef struct block_data_s
{
	int8_t buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t len;
} block_data_t;

/**
 * struct block_s -			represents a block in the blockchain
 * @info:					block metadata (from block_info_s)
 * @data:					block payload (data length <= BLOCKCHAIN_DATA_MAX)
 * @hash:					block hash (SHA256 of info + data)
 */
typedef struct block_s
{
	block_info_t info;
	block_data_t data;
	uint8_t hash[SHA256_DIGEST_LENGTH];
} block_t;

/**
 * struct blockchain_s -	container for the blockchain itself
 * @chain:					linked list of all blocks
 */
typedef struct blockchain_s
{
	llist_t *chain;
} blockchain_t;



/* FUNCTION PROTOTYPES */

blockchain_t *blockchain_create(
	void);
block_t *block_create(
	block_t const *prev, int8_t const *data, uint32_t data_len);
void block_destroy(
	block_t *block);
void blockchain_destroy(
	blockchain_t *blockchain);
uint8_t *block_hash(
	block_t const *block, uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
int blockchain_serialize(
	blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(
	char const *path);
int block_is_valid(block_t const *block, block_t const *prev_block);

#endif /* _BLOCKCHAIN_H */
