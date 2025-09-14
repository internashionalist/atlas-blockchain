#include "hblk_crypto.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <openssl/pem.h>

/**
 * ec_save -	saves an EC key pair to files in a specified folder
 * @key:		EC key pair to save
 * @folder:		destination directory path
 *
 * Return:		1 on success, 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char priv_path[PATH_MAX], pub_path[PATH_MAX];	/* file paths */
	FILE *priv_fp = NULL, *pub_fp = NULL;		/* file pointers */

	if (!key || !folder)						/* check inputs */
		return (0);

	if (mkdir(folder, 0755) == -1 && errno != EEXIST)	/* create folder */
		return (0);
												/* create file paths */
	snprintf(priv_path, sizeof(priv_path), "%s/%s", folder, PRI_FILENAME);
	snprintf(pub_path, sizeof(pub_path), "%s/%s", folder, PUB_FILENAME);

	priv_fp = fopen(priv_path, "w");			/* open private key file */
	if (!priv_fp || !PEM_write_ECPrivateKey(
		priv_fp, key, NULL, NULL, 0, NULL, NULL) || (fclose(priv_fp) != 0))
		goto fail;								/* write private key */
	priv_fp = NULL;								/* avoid double close */

	pub_fp = fopen(pub_path, "w");				/* open public key file */
	if (!pub_fp || !PEM_write_EC_PUBKEY(pub_fp, key) || (fclose(pub_fp) != 0))
		goto fail;								/* write public key */
	pub_fp = NULL;								/* avoid double close */

	return (1);									/* success */

fail:											/* failure cleanup */
	if (priv_fp)
		fclose(priv_fp);
	if (pub_fp)
		fclose(pub_fp);
	return (0);
}
