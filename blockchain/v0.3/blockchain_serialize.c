#include "blockchain.h"

int write_field(FILE *file, void const *buf, size_t size, int swap);
int write_tx(FILE *file, transaction_t const *tx, int swap);
int write_block(FILE *file, block_t const *block, int swap);
int write_unspent(FILE *file, llist_t *unspent, int swap);

/**
 * write_field -			writes a field to file with optional endianness swap
 * @file:					file stream to write to
 * @buf:					pointer to data buffer
 * @size:					size of data buffer
 * @swap:					whether to swap endianness
 *
 * Return:					1 on success, 0 on failure
 */
int write_field(
	FILE *file,
	void const *buf,
	size_t size,
	int swap)
{
	uint64_t tmp;								/* temp buffer for swapping */

	if (swap && size > 1)						/* swap endianness if needed */
	{
		memcpy(&tmp, buf, size);				/* copy buffer to temp */
		_swap_endian(&tmp, size);				/* swap temp */
		return (fwrite(&tmp, size, 1, file) == 1); /* write swapped temp */
	}
	return (fwrite(buf, 1, size, file) == size); /* otherwise write original */
}

/**
 * write_tx -				writes a transaction to file
 * @file:					file stream to write to
 * @tx:						pointer to transaction to write
 * @swap:					whether to swap endianness of fields
 *
 * Return:					1 on success, 0 on failure
 */
int write_tx(
	FILE *file,
	transaction_t const *tx,
	int swap)
{
	int in_count, out_count;
	uint32_t in, out;
	int idx;

	in_count = tx && tx->inputs ? llist_size(tx->inputs) : 0;
	out_count = tx && tx->outputs ? llist_size(tx->outputs) : 0;
	in = (uint32_t)in_count;
	out = (uint32_t)out_count;
										/* validate counts, write headers */
	if ((tx->inputs && in_count < 0) || (tx->outputs && out_count < 0) ||
		!write_field(file, tx->id, SHA256_DIGEST_LENGTH, 0) ||
		!write_field(file, &in, sizeof(in), swap) ||
		!write_field(file, &out, sizeof(out), swap))
		return (0);
	for (idx = 0; idx < in_count; idx++)		/* iterate/write inputs */
	{
		tx_in_t *input;

		input = llist_get_node_at(tx->inputs, idx);
		if (!input ||
			!write_field(file, input->block_hash, SHA256_DIGEST_LENGTH, 0) ||
			!write_field(file, input->tx_id, SHA256_DIGEST_LENGTH, 0) ||
			!write_field(file, input->tx_out_hash, SHA256_DIGEST_LENGTH, 0) ||
			!write_field(file, input->sig.sig, SIG_MAX_LEN, 0) ||
			!write_field(file, &input->sig.len, sizeof(input->sig.len), 0))
			return (0);
	}
	for (idx = 0; idx < out_count; idx++)		/* iterate/write outputs */
	{
		tx_out_t *output;

		output = llist_get_node_at(tx->outputs, idx);
		if (!output ||
			!write_field(file, &output->amount, sizeof(output->amount), swap) ||
			!write_field(file, output->pub, EC_PUB_LEN, 0) ||
			!write_field(file, output->hash, SHA256_DIGEST_LENGTH, 0))
			return (0);
	}
	return (1);
}

/**
 * write_block -				writes a block to file
 * @file:						file stream to write to
 * @block:						pointer to block to write
 * @swap:						whether to swap endianness of fields
 *
 * Return:						1 on success, 0 on failure
 */
int write_block(
	FILE *file,
	block_t const *block,
	int swap)
{
	uint32_t data_len;							/* length of block data */
	int tx_count, idx = 0;						/* tx count, index */
	int32_t marker;								/* for tx count */

	data_len = block->data.len;
	tx_count = block->transactions ? llist_size(block->transactions) : -1;
	marker = block->transactions ? (int32_t)tx_count : -1;
											/* check counts, write fields */
	if ((block->transactions && tx_count < 0) || /* invalid tx count */
		data_len > BLOCKCHAIN_DATA_MAX ||		/* invalid data length */
		!write_field(file, &block->info.index,	/* block index */
			sizeof(block->info.index), swap) ||
		!write_field(file, &block->info.difficulty, /* block difficulty */
			sizeof(block->info.difficulty), swap) ||
		!write_field(file, &block->info.timestamp, /* block timestamp */
			sizeof(block->info.timestamp), swap) ||
		!write_field(file, &block->info.nonce,	/* block nonce */
			sizeof(block->info.nonce), swap) ||
		!write_field(file, block->info.prev_hash, /* block previous hash */
			SHA256_DIGEST_LENGTH, 0) ||
		!write_field(file, &data_len,			/* block data length */
			sizeof(data_len), swap) ||
		(!write_field(file, block->data.buffer,	/* block data */
			data_len, 0) && data_len) ||
		!write_field(file, block->hash,			/* block hash */
			SHA256_DIGEST_LENGTH, 0) ||
		!write_field(file, &marker,				/* tx count marker */
			sizeof(marker), swap))
		return (0);
	if (marker <= 0)							/* invalid marker */
		return (marker == 0 || marker == -1);
	for (idx = 0; idx < tx_count; idx++)		/* loop transactions */
	{
		transaction_t *tx;

		tx = llist_get_node_at(block->transactions, idx);
		if (!tx || !write_tx(file, tx, swap))	/* write tx */
			return (0);
	}
	return (1);
}

/**
 * write_unspent -			writes unspent transaction outputs to file
 * @file:					file stream to write to
 * @unspent:				list of unspent transaction outputs
 * @swap:					whether to swap endianness of fields
 *
 * Return:					1 on success, 0 on failure
 */
int write_unspent(
	FILE *file,
	llist_t *unspent,
	int swap)
{
	int idx, out_count;

	out_count = unspent ? llist_size(unspent) : 0;
	if (out_count < 0)
		return (0);
									/* write each unspent tx out */
	for (idx = 0; idx < out_count; idx++)
	{
		unspent_tx_out_t *entry;

		entry = llist_get_node_at(unspent, idx);
		if (!entry ||
			!write_field(
				file, entry->block_hash, SHA256_DIGEST_LENGTH, 0) ||
			!write_field(
				file, entry->tx_id, SHA256_DIGEST_LENGTH, 0) ||
			!write_field(
				file, &entry->out.amount, sizeof(entry->out.amount), swap) ||
			!write_field(
				file, entry->out.pub, EC_PUB_LEN, 0) ||
			!write_field(
				file, entry->out.hash, SHA256_DIGEST_LENGTH, 0))
			return (0);
	}
	return (1);
}

/**
 * blockchain_serialize -		serializes a blockchain to a file
 * @blockchain:					pointer to blockchain to serialize
 * @path:						path to file to write to
 *
 * Return:						0 on success, -1 on failure
 */
int blockchain_serialize(
	blockchain_t const *blockchain,
	char const *path)
{
	FILE *file;
	uint8_t endian;
	uint32_t block_count, unspent_count, idx;
	int swap, chain_size, unspent_size;

	if (!blockchain || !path)
		return (-1);
	chain_size = llist_size(blockchain->chain);
	unspent_size = blockchain->unspent ? llist_size(blockchain->unspent) : 0;
	if (chain_size <= 0 || unspent_size < 0)
		return (-1);
	block_count = (uint32_t)chain_size;
	unspent_count = (uint32_t)unspent_size;
	file = fopen(path, "wb");
	if (!file)
		return (-1);
	endian = _get_endianness();
	swap = (endian == 2);
	if (!write_field(file, HBLK, 4, 0) ||
		!write_field(file, VERS, 3, 0) ||
		!write_field(file, &endian, 1, 0) ||
		!write_field(file, &block_count, sizeof(block_count), swap) ||
		!write_field(file, &unspent_count, sizeof(unspent_count), swap))
		goto fail;
	for (idx = 0; idx < block_count; idx++)
	{
		block_t const *block = llist_get_node_at(blockchain->chain, idx);

		if (!block || !write_block(file, block, swap))
			goto fail;
	}
	if (!write_unspent(file, blockchain->unspent, swap))
		goto fail;
	fclose(file);
	return (0);
fail:
	fclose(file);
	return (-1);
}
