#ifndef LAB_ENCRYPTOR_H
#define LAB_ENCRYPTOR_H

#include <string>

#include <cryptopp/secblock.h>

// LAB_Encryptor uses the Crypto++ library. Encryption used is 
// Advanced Encryption Standard (AES), CBC Mode, 128-bit key size.

class LAB_Encryptor
{
  private:
    static void add_iv_to_ciphertext 
      (const CryptoPP::SecByteBlock& iv, std::string& ciphertext);

    static CryptoPP::SecByteBlock get_iv_from_ciphertext 
      (const std::string& ciphertext);

    static std::string get_ciphertext_from_ciphertext_with_iv 
      (const std::string& ciphertext);

    static void derive_key_from_password 
      (const std::string& password, CryptoPP::SecByteBlock& key_block);

  public:
    static std::string encrypt_string (const std::string& plaintext, const std::string& password);
    static std::string decrypt_string (const std::string& ciphertext, const std::string& password);
};

#endif