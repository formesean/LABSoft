#include "LAB_Encryptor.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/pwdbased.h>

typedef unsigned char byte;

void LAB_Encryptor:: 
add_iv_to_ciphertext (const CryptoPP::SecByteBlock& iv, 
                            std::string&            ciphertext)
{
  ciphertext += std::string (iv.begin(), iv.end());
}

CryptoPP::SecByteBlock LAB_Encryptor:: 
get_iv_from_ciphertext (const std::string& ciphertext)
{
  // IV is 128 bits in AES CBC
  std::string iv (ciphertext.end () - CryptoPP::AES::BLOCKSIZE, ciphertext.end ());

  return (CryptoPP::SecByteBlock (reinterpret_cast<const byte*>(&iv[0]), iv.size ()));
}

std::string LAB_Encryptor:: 
get_ciphertext_from_ciphertext_with_iv (const std::string& ciphertext)
{
  // IV is 128 bits in AES CBC
  return (std::string (ciphertext.begin (), ciphertext.end () - CryptoPP::AES::BLOCKSIZE));
}

void LAB_Encryptor:: 
derive_key_from_password (const std::string&            password,
                                CryptoPP::SecByteBlock& key)
{
  CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf;

  pbkdf.DeriveKey (
    key,                          // derived
    key.size(),                   // derived length
    0x00,                         // purpose (unused for PKCS5_PBKDF2_HMAC)
    (const byte*)password.data(), // secret
    password.size(),              // secret length
    nullptr,                      // salt
    0,                            // salt length
    1000                          // iterations
  );
}

std::string LAB_Encryptor:: 
encrypt_string (const std::string& plaintext,
                const std::string& password)
{
  // 1. create random number generator object
  CryptoPP::AutoSeededRandomPool prng;

  // 2. create two SecByteBlock containers for key and initialization vector (iv)
  CryptoPP::SecByteBlock key  (CryptoPP::AES::DEFAULT_KEYLENGTH);
  CryptoPP::SecByteBlock iv   (CryptoPP::AES::BLOCKSIZE); // IV is 128 bits in AES CBC

  // 3. derive key from the password 
  derive_key_from_password (password, key);

  // 4. generate iv random data
  prng.GenerateBlock (iv, iv.size ());

  // 5. create CBC mode AES cipher object
  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;

  // 6. set key of the cipher object
  e.SetKeyWithIV (key, key.size (), iv);

  // 7. do encryption
  std::string ciphertext;

  CryptoPP::StringSource s (
    plaintext,
    true,
    new CryptoPP::StreamTransformationFilter (
      e,
      new CryptoPP::StringSink (ciphertext)
    )
  );

  // 8. add iv to ciphertext
  add_iv_to_ciphertext (iv, ciphertext);

  return (ciphertext);
}

std::string LAB_Encryptor:: 
decrypt_string (const std::string& ciphertext,
                    const std::string& password)
{
  // 1.
  std::string actual_ciphertext = get_ciphertext_from_ciphertext_with_iv (ciphertext);

  // 2. 
  CryptoPP::SecByteBlock key  (CryptoPP::AES::DEFAULT_KEYLENGTH);
  CryptoPP::SecByteBlock iv   (CryptoPP::AES::BLOCKSIZE); // 128 bit in AES CBC

  // 3. 
  derive_key_from_password (password, key);

  // 4.
  iv = get_iv_from_ciphertext (ciphertext);

  // 5.
  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;

  // 6.
  d.SetKeyWithIV (key, key.size (), iv);

  // 7.
  std::string plaintext;
  CryptoPP::StringSource s (
    actual_ciphertext,
    true,
    new CryptoPP::StreamTransformationFilter (
      d,
      new CryptoPP::StringSink (plaintext)
    )
  );

  return (plaintext);
}