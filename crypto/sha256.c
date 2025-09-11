#include "hblk_crypto.h"

/**
 * sha256 -		computes SHA-256 hash of a byte sequence
 * @s:			pointer to input bytes (NULL if len == 0)
 * @len:		number of bytes to hash from s
 * @digest:		output buffer (32 bytes), or NULL
 *
 * Return:		digest on success or NULL on failure
 */
uint8_t *sha256(int8_t const *s, size_t len,
				uint8_t digest[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX ctxt;				/* SHA-256 context */

	if (digest == NULL)				/* no output buffer */
		return (NULL);

	if (!SHA256_Init(&ctxt))		/* initialize context */
		return (NULL);

	if (len > 0)					/* if input data */
		if (!SHA256_Update(&ctxt, s, len))	/* hash input data */
			return (NULL);

	if (!SHA256_Final(digest, &ctxt))	/* finalize hash computation */
		return (NULL);

	return (digest);				/* return output buffer */
}
