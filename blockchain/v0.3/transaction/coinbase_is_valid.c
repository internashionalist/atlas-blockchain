#include "transaction.h"

int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
    uint8_t hash_buf[SHA256_DIGEST_LENGTH];
    uint8_t zero_hash[SHA256_DIGEST_LENGTH] = {0};
    uint8_t zero_sig[SIG_MAX_LEN] = {0};
    tx_in_t *input;
    tx_out_t *output;

    if (!coinbase)
        return (0);

    if (!transaction_hash(coinbase, hash_buf) ||
        memcmp(hash_buf, coinbase->id, SHA256_DIGEST_LENGTH) != 0)
        return (0);

    if (llist_size(coinbase->inputs) != 1 ||
        llist_size(coinbase->outputs) != 1)
        return (0);

    input = llist_get_node_at(coinbase->inputs, 0);
    output = llist_get_node_at(coinbase->outputs, 0);
    if (!input || !output)
        return (0);

    if (memcmp(input->tx_out_hash, &block_index, sizeof(block_index)) != 0)
        return (0);

    if (memcmp(input->block_hash, zero_hash, SHA256_DIGEST_LENGTH) != 0 ||
        memcmp(input->tx_id, zero_hash, SHA256_DIGEST_LENGTH) != 0 ||
        input->sig.len != 0 ||
        memcmp(input->sig.sig, zero_sig, SIG_MAX_LEN) != 0)
        return (0);

    if (output->amount != COINBASE_AMOUNT)
        return (0);

    return (1);
}
