#ifndef ELGAMAL_H
#define ELGAMAL_H

#include "../fast_exponentiation/fast_exp.h"
#include "../fast_exponentiation/mul_mod.h"
#include "../mul_inverse/mul_inv.h"

#include <cassert>
#include <random>
#include <utility>

#define DEFAULT_MODULUS 4611686018427377339
#if !defined(MODULUS)
#define MODULUS DEFAULT_MODULUS
#endif
constexpr IntegerImpl prime_modulus{MODULUS};

#define DEFAULT_GENERATOR 4
#if !defined(GENERATOR)
#define GENERATOR DEFAULT_GENERATOR
#endif
constexpr IntegerImpl generator{GENERATOR};

namespace elgamal {

// Generate private (secret) key.
IntegerImpl generate_private_key()
{
    std::random_device rnd;
    std::uniform_int_distribution<IntegerImpl> dist(1, prime_modulus - 1);
    return dist(rnd);
}

// Generate public key.
IntegerImpl generate_public_key(IntegerImpl private_key)
{
    return fast_exp<IntegerImpl, IntegerImpl, prime_modulus>(generator, private_key);
}

// Encrypt message.
std::pair<IntegerImpl, IntegerImpl> encrypt_message(IntegerImpl message, IntegerImpl receiver_public_key, IntegerImpl testing_ephemeral_key)
{
    assert(1 < message && message < prime_modulus);
    IntegerImpl ephemeral_key;
    if (testing_ephemeral_key) {
        ephemeral_key = testing_ephemeral_key;
    } else {
        ephemeral_key = generate_private_key();
    }
    IntegerImpl cipher_text_1 = generate_public_key(ephemeral_key);
    IntegerImpl shared_secret = fast_exp<IntegerImpl, IntegerImpl, prime_modulus>(receiver_public_key, ephemeral_key);
    IntegerImpl cipher_text_2 = mul_mod<IntegerImpl, prime_modulus>(message, shared_secret);
    return {cipher_text_1, cipher_text_2};
}

// Decrypt cipher text.
IntegerImpl decrypt_cipher(std::pair<IntegerImpl, IntegerImpl> cipher_text, IntegerImpl receiver_private_key)
{
    IntegerImpl shared_secret = fast_exp<IntegerImpl, IntegerImpl, prime_modulus>(cipher_text.first, receiver_private_key);
    return mul_mod<IntegerImpl, prime_modulus>(cipher_text.second, mul_inv<IntegerImpl, prime_modulus>(shared_secret));
}

} // namespace elgamal

#endif // ELGAMAL_H
