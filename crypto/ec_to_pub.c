#include "hblk_crypto.h"

/**
 * ec_to_pub -	extracts the public key from an EC key pair
 * @key:		pointer to EC key pair
 * @pub:		output buffer (65 bytes) for public key
 *
 * Return:		pointer to pub or NULL on failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	const EC_GROUP *group;					/* elliptic curve group */
	const EC_POINT *point;					/* public key point on curve */
	size_t out_len;							/* output length */

	if (!key || !pub)						/* NULL checks */
		return (NULL);

	group = EC_KEY_get0_group(key);			/* get group & public key point */
	point = EC_KEY_get0_public_key(key);	/* from EC key pair */
	if (!group || !point)
		return (NULL);

	out_len = EC_POINT_point2oct(group, point,	/* convert point to octets */
		POINT_CONVERSION_UNCOMPRESSED, pub, EC_PUB_LEN, NULL);

	if (out_len != EC_PUB_LEN)				/* check output length */
		return (NULL);

	return (pub);							/* return output buffer pointer */
}
