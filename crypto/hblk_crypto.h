#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>

#define EC_PUB_LEN 65

uint8_t *sha256(
	int8_t const *s, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]);
EC_KEY *ec_create(
	void);
uint8_t *ec_to_pub(
	EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);

#endif /* HBLK_CRYPTO_H */
