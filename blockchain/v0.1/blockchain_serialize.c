#include "blockchain.h"

/**
 * blockchain_serialize - Serialize a blockchain
 * @chain: Pointer to the blockchain to serialize
 * @buf: Buffer where the serialized data is stored
 * @len: Pointer to a variable where the length of the serialized data is stored
 *
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *chain, uint8_t **buf, size_t *len)
{

