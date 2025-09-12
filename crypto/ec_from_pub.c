#include "hblk_crypto.h"

/**
 * ec_from_pub -	creates an EC_KEY from a 65-byte public key
 * @pub:			public key buffer
 *
 * Return:			newly-created EC_KEY* or NULL on failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key = NULL;							/* init new key */
	const EC_GROUP *grp = NULL;					/* init group */
	EC_POINT *point = NULL;						/* init point */

	key = EC_KEY_new_by_curve_name(EC_CURVE);	/* create new key */
	if (key == NULL)
		return (NULL);

	if (pub == NULL)							/* if no key buffer */
	{
		EC_KEY_free(key);
		return (NULL);
	}
	grp = EC_KEY_get0_group(key);				/* get the group */
	if (grp == NULL)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	point = EC_POINT_new(grp);					/* create new point */
	if (point == NULL)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	if (EC_POINT_oct2point(grp, point, pub, EC_PUB_LEN, NULL) != 1 ||
		EC_KEY_set_public_key(key, point) != 1)
	{
		EC_POINT_free(point);					/* convert to point */
		EC_KEY_free(key);						/* set as public key */
		return (NULL);
	}

	EC_POINT_free(point);						/* can now free point */
	point = NULL;								/* avoid double-free */

	return (key);								/* return the new key */
}
