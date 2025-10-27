#include "blockchain.h"

/**
 * hash_matches_difficulty -	checks if has meets difficulty requirement
 * @hash:						pointer to hash buffer to inspect
 * @difficulty:					number of leading zero bits required
 *
 * Return:						1 if the hash meets the difficulty,
 *								0 if not or error
 */
int hash_matches_difficulty(
	uint8_t const hash[SHA256_DIGEST_LENGTH], uint32_t difficulty)
{
	uint32_t i;										/* loop iterator */
	uint32_t total_bits;							/* total bits in hash */
	uint8_t curr_byte;								/* inspected byte */
	uint8_t curr_bit;								/* inspected bit */

	if (!hash)										/* NULL hash */
		return (0);

	total_bits = SHA256_DIGEST_LENGTH * 8;			/* bytes to bits */

	if (difficulty > total_bits)					/* difficulty too high */
		return (0);

	for (i = 0; i < difficulty; i++)				/* loop through bits */
	{
		curr_byte = hash[i / 8];					/* hash byte */
		curr_bit = 0x80 >> (i % 8);					/* target bit */
		if ((curr_byte & curr_bit) != 0)			/* check bit */
			return (0);
	}

	return (1);										/* all bits zero */
}
