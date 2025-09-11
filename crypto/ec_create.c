#include "hblk_crypto.h"

/**
 * ec_create -	creates a new EC key pair
 *
 * Return:		pointer to a new EC_KEY or NULL on failure
 */
EC_KEY *ec_create(void)
{
	EC_KEY *key = NULL;					/* init new key */

	key = EC_KEY_new_by_curve_name(NID_secp256k1);	/* use secp256k1 */
	if (!key)										/* to create key pair */
		return (NULL);

	if (!EC_KEY_generate_key(key) || !EC_KEY_check_key(key))
	{
		EC_KEY_free(key);
		return (NULL);
	}

	return (key);						/* return pointer to new key pair */
}
