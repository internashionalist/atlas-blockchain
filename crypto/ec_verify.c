#include "hblk_crypto.h"

/**
 * ec_verify -	verifies a message signature with an EC public key
 * @key:		EC key pair with public key
 * @msg:		bytes that were signed
 * @msglen:		number of bytes in msg
 * @sig:		signature to verify
 *
 * Return:		1 if signature is valid or otherwise 0
 */
int ec_verify(
	EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t const *sig)
{
	uint8_t digest[SHA256_DIGEST_LENGTH];		/* SHA-256 digest of message */

	if (!key || !msg || !sig || sig->len == 0)	/* input checks */
		return (0);

	if (SHA256(msg, msglen, digest) == NULL)	/* hash message first */
		return (0);

	if (ECDSA_verify(0, digest, SHA256_DIGEST_LENGTH, sig->sig, sig->len,
		(EC_KEY *)key) != 1)					/* verify signature */
		return (0);

	return (1);									/* signature is valid */
}
