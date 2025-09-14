#include "hblk_crypto.h"

/*
 * ec_sign -	signs a message with an EC private key
 * @key:		EC key pair with private key
 * @msg:		bytes to sign
 * @msglen:		number of bytes in @msg
 * @sig:		output (DER-encoded signature buffer + length)
 *
 * Return:		sig->sig on success or NULL on failure
 */
uint8_t *ec_sign(
	EC_KEY const *key, uint8_t const *msg, size_t msglen, sig_t *sig)
{
	uint8_t digest[SHA256_DIGEST_LENGTH];			/* message digest */
	unsigned int der_sig_len = 0;					/* DER-encoded sig len */
	int max_len;									/* max sig length */

	if (!key || !msg || !sig)						/* input checks */
		return (NULL);

	if (!SHA256(msg, msglen, digest))				/* hash message */
		return (NULL);

	max_len = ECDSA_size(key);						/* make sure sig fits */
	if (max_len <= 0 || max_len > SIG_MAX_LEN)		/* inside buffer */
		return (NULL);

	if (ECDSA_sign(0, digest, SHA256_DIGEST_LENGTH,	/* sign the digest */
		sig->sig, &der_sig_len, (EC_KEY *)key) != 1)
		return (NULL);

	sig->len = (uint8_t)der_sig_len;				/* store DER sig length */
	return (sig->sig);								/* return sig pointer */
}
