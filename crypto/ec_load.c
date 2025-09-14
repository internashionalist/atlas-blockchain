#include "hblk_crypto.h"

/**
 * ec_load -	loads an EC key pair from a folder
 * @folder:		path to folder containing key files
 *
 * Return:		pointer to EC_KEY on success, or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	char priv_path[PATH_MAX], pub_path[PATH_MAX];	/* file paths */
	FILE *priv_fp = NULL, *pub_fp = NULL;			/* file pointers */
	EC_KEY *key = NULL, *pub_key = NULL;			/* loaded keys */

	if (!folder)
		return (NULL);
													/* build file paths */
	snprintf(priv_path, sizeof(priv_path), "%s/%s", folder, PRI_FILENAME);
	snprintf(pub_path, sizeof(pub_path), "%s/%s", folder, PUB_FILENAME);
	priv_fp = fopen(priv_path, "r");				/* open private key */
	if (!priv_fp)
		goto fail;

	key = PEM_read_ECPrivateKey(priv_fp, NULL, NULL, NULL);
	if (!key)										/* read private key */
		goto fail;
	fclose(priv_fp), priv_fp = NULL;				/* close it up */

	pub_fp = fopen(pub_path, "r");					/* open public key */
	if (!pub_fp)
		goto fail;

	pub_key = PEM_read_EC_PUBKEY(pub_fp, NULL, NULL, NULL);
	if (!pub_key)									/* read public key */
		goto fail;
	fclose(pub_fp), pub_fp = NULL;					/* close that up */
	EC_KEY_free(pub_key);							/* free unneeded key */
	return (key);									/* return private key */

fail:												/* failure cleanup */
	if (priv_fp)
		fclose(priv_fp);
	if (pub_fp)
		fclose(pub_fp);
	if (pub_key)
		EC_KEY_free(pub_key);
	if (key)
		EC_KEY_free(key);
	return (NULL);
}
