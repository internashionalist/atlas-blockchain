#include "blockchain.h"

static int read_field(FILE *file, void *buf, size_t size, int swap);
static int read_transaction(
	transaction_t *tx, FILE *file, int swap);
static int read_block(
	FILE *file, block_t *block, int swap);
static int read_header(
	FILE *file, uint32_t *blocks, uint32_t *unspent, int *swap);

/**
 * read_field -					reads bytes and optionally swap endianness
 * @file:						source stream
 * @buf:						destination buffer
 * @size:						number of bytes to copy
 * @swap:						swap flag
 *
 * Return:						1 on success, otherwise 0
 */
static int read_field(
	FILE *file,
	void *buf,
	size_t size,
	int swap)
{
	if (!file || !buf || !size)						/* check inputs */
		return (0);
	if (!(fread(buf, 1, size, file) != size))		/* read bytes */
		return (0);
	if (swap && size > 1)							/* swap if needed */
		_swap_endian(buf, size);
	return (1);
}

/**
 * read_transaction -			rebuilds a transaction from a stream
 * @tx:							transaction to populate
 * @file:						source stream
 * @swap:						swap flag for numeric fields
 *
 * Return:						1 on success, otherwise 0
 */
int read_transaction(transaction_t *tx, FILE *file, int swap)
{
	uint32_t in_count, out_count;
	tx_in_t *in;
	tx_out_t *out;
													/* read header */
	if (fread(tx->id, 1, SHA256_DIGEST_LENGTH, file) != SHA256_DIGEST_LENGTH ||
		!read_field(file, &in_count, sizeof(in_count), swap) ||
		!read_field(file, &out_count, sizeof(out_count), swap))
		return (0);
	while (in_count--)								/* read inputs */
	{
		in = calloc(1, sizeof(*in));
		if (!in ||
			fread(in->block_hash, 1, SHA256_DIGEST_LENGTH, file) !=
				SHA256_DIGEST_LENGTH ||
			fread(in->tx_id, 1, SHA256_DIGEST_LENGTH, file) !=
				SHA256_DIGEST_LENGTH ||
			fread(in->tx_out_hash, 1, SHA256_DIGEST_LENGTH, file) !=
				SHA256_DIGEST_LENGTH ||
			fread(in->sig.sig, 1, SIG_MAX_LEN, file) != SIG_MAX_LEN ||
			fread(&in->sig.len, 1, 1, file) != 1 ||
			llist_add_node(tx->inputs, in, ADD_NODE_REAR) == -1)
			return (free(in), 0);
	}
	while (out_count--)								/* read outputs */
	{
		out = calloc(1, sizeof(*out));
		if (!out ||
			!read_field(file, &out->amount, sizeof(out->amount), swap) ||
			fread(out->pub, 1, EC_PUB_LEN, file) != EC_PUB_LEN ||
			fread(out->hash, 1, SHA256_DIGEST_LENGTH, file) !=
				SHA256_DIGEST_LENGTH ||
			llist_add_node(tx->outputs, out, ADD_NODE_REAR) == -1)
			return (free(out), 0);
	}
	return (1);
}

/**
 * read_block -					rebuild a block and its transactions
 * @file:						source stream
 * @block:						block to populate
 * @swap:						swap flag for numeric fields
 *
 * Return:						1 on success, otherwise 0
 */
int read_block(
	FILE *file,
	block_t *block,
	int swap)
{
	uint32_t data_len, i;
	int32_t marker;
	transaction_t *tx;
													/* read block info */
	if (!file || !block ||
		!read_field(file, &block->info.index, sizeof(block->info.index), swap) ||
		!read_field(file, &block->info.difficulty,
			sizeof(block->info.difficulty), swap) ||
		!read_field(file, &block->info.timestamp,
			sizeof(block->info.timestamp), swap) ||
		!read_field(file, &block->info.nonce,
			sizeof(block->info.nonce), swap) ||
		fread(block->info.prev_hash, 1, SHA256_DIGEST_LENGTH, file) !=
			SHA256_DIGEST_LENGTH ||
		!read_field(file, &block->data.len, sizeof(block->data.len), swap))
		return (0);
	data_len = block->data.len;
	if (data_len > BLOCKCHAIN_DATA_MAX ||			/* read block data */
		(data_len && fread(block->data.buffer, 1, data_len, file) != data_len) ||
		fread(block->hash, 1, SHA256_DIGEST_LENGTH, file) !=
			SHA256_DIGEST_LENGTH ||
		!read_field(file, &marker, sizeof(marker), swap) || marker < -1)
		return (0);
	if (marker < 0)									/* no transactions */
		return (marker == -1);
	(block->transactions = llist_create(MT_SUPPORT_FALSE));	/* init tx list */
	if (!block->transactions)
		return (0);
	for (i = 0; i < (uint32_t)marker; ++i)			/* read tx list */
	{
		tx = calloc(1, sizeof(*tx));				/* allocate tx */
		if (!tx)
			return (0);
		tx->inputs = llist_create(MT_SUPPORT_FALSE); /* init input list */
		tx->outputs = llist_create(MT_SUPPORT_FALSE); /* init output list */
		if (!tx->inputs || !tx->outputs || !read_transaction(tx, file, swap) ||
			llist_add_node(block->transactions, tx, ADD_NODE_REAR) == -1)
			return (transaction_destroy(tx), 0);	/* read/add tx */
	}
	return (1);
}

/**
 * read_header -				validate and read file header data
 * @file:						source stream
 * @blocks:						destination for block count
 * @unspent:					destination for unspent count
 * @swap:						destination for swap flag
 *
 * Return:						1 on success, otherwise 0
 */
int read_header(
	FILE *file,
	uint32_t *blocks,
	uint32_t *unspent,
	int *swap)
{
	uint8_t magic[4], version[3], endian;			/* header fields */
													/* read/validate header */
	if (fread(magic, 1, sizeof(magic), file) != sizeof(magic) ||
		memcmp(magic, HBLK, sizeof(magic)) ||
		fread(version, 1, sizeof(version), file) != sizeof(version) ||
		memcmp(version, VERS, sizeof(version)) ||
		fread(&endian, 1, 1, file) != 1 ||
		(endian != 1 && endian != 2))
		return (0);
	*swap = (_get_endianness() != endian);			/* swap if needed */
	if (!read_field(file, blocks, sizeof(*blocks), *swap) ||
		!read_field(file, unspent, sizeof(*unspent), *swap))
		return (0);
	return (*blocks != 0);
}

/**
 * blockchain_deserialize -		rebuild a blockchain from a file
 * @path:						path to serialized blockchain
 *
 * Return:						pointer to blockchain on success, otherwise NULL
 */
blockchain_t *blockchain_deserialize(
	char const *path)
{
	FILE *file = NULL;
	blockchain_t *blockchain = NULL;
	block_t *block;
	unspent_tx_out_t *entry;
	uint32_t blocks = 0, unspent = 0;
	int swap = 0;

	file = fopen(path, "rb");						/* open file */
	if (!path || !file)
		return (NULL);
	blockchain = calloc(1, sizeof(*blockchain));	/* allocate blockchain */
	if (!read_header(file, &blocks, &unspent, &swap) || !blockchain)
		return (fclose(file), NULL);				/* read header */
	blockchain->chain = llist_create(MT_SUPPORT_FALSE); /* create chain list */
	blockchain->unspent = llist_create(MT_SUPPORT_FALSE); /* unspent list */
	if (!blockchain->chain || !blockchain->unspent)
		return (fclose(file), blockchain_destroy(blockchain), NULL);
	while (blocks--)								/* read blocks */
	{
		block = calloc(1, sizeof(*block));
		if (!block || !read_block(file, block, swap) ||
			llist_add_node(blockchain->chain, block, ADD_NODE_REAR) == -1)
			return (block_destroy(block), fclose(file),
				blockchain_destroy(blockchain), NULL);
	}
	while (unspent--)								/* read unspent tx outs */
	{
		entry = calloc(1, sizeof(*entry));			/* read all fields */
		if (!entry || fread(entry->block_hash, 1, SHA256_DIGEST_LENGTH, file) !=
			SHA256_DIGEST_LENGTH || fread(
				entry->tx_id, 1, SHA256_DIGEST_LENGTH, file) != SHA256_DIGEST_LENGTH ||
			!read_field(file, &entry->out.amount, sizeof(entry->out.amount), swap) ||
			fread(entry->out.pub, 1, EC_PUB_LEN, file) != EC_PUB_LEN ||
			fread(entry->out.hash, 1, SHA256_DIGEST_LENGTH, file) !=
				SHA256_DIGEST_LENGTH ||				/* add to list */
			llist_add_node(blockchain->unspent, entry, ADD_NODE_REAR) == -1)
			return (free(entry), fclose(file), blockchain_destroy(blockchain), NULL);
	fclose(file);
	return (blockchain);							/* return rebuilt blockchain */
}
