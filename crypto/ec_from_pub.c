#include "hblk_crypto.h"

/**
 * ec_from_pub -	creates an EC_KEY from an uncompressed public key buffer
 * @pub:			65-byte uncompressed public key
 *
 * Return:			pointer to newly created EC_KEY on success, or NULL on failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{

}