#include "elgamal.h"

#include "../helper/args_parser.h"

#include <cassert>
#include <iostream>
#include <utility>

int main(int argc, char* argv[])
{
    // World
    // =====
    std::cout << "ElGamal PKC Public Parameters:\n";
    std::cout << "Modulus p    = " << prime_modulus << '\n';
    std::cout << "Generator g  = " << generator << '\n';

    // Receiver
    // ========
    std::cout << "\nStep 1. Receiver: Private Key Generation.\n";
    IntegerImpl private_key = elgamal::generate_private_key();
    // Overriding with a command-line argument for testing purposes.
    private_key = parse_int_args<IntegerImpl>(argc, argv, "a=", private_key);
    std::cout << "private key  = " << private_key << '\n';

    std::cout << "\nStep 2. Receiver: Public Key Generation.\n";
    const auto public_key = elgamal::generate_public_key(private_key);
    std::cout << "public key   = " << public_key << '\n';

    // Sender
    // ======
    std::cout << "\nStep 3. Sender: Message Encryption.\n";
    IntegerImpl message = parse_int_args<IntegerImpl>(argc, argv, "m=", /* default message */ 42);
    std::cout << "message      = " << message << '\n';
    IntegerImpl testing_ephemeral_key = parse_int_args<IntegerImpl>(argc, argv, "k=", /* default message */ 0);
    const auto cipher_text = elgamal::encrypt_message(message, public_key, testing_ephemeral_key);
    std::cout << "cipher text  = " << "{" << cipher_text.first << ", " << cipher_text.second << "}\n";

    // Receiver
    // ========
    std::cout << "\nStep 4. Receiver: Cipher Text Decryption.\n";
    const auto decrypted_message = elgamal::decrypt_cipher(cipher_text, private_key);
    std::cout << "message      = " << decrypted_message << '\n';

    assert(message == decrypted_message);

    return 0;
}
