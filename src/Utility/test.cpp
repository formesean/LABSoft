#include "LAB_Encryptor.h"

#include <string>
#include <iostream>

int main ()
{
  std::string plaintext = "the quick brown fox";
  std::string password = "qweasdzxc";

  std::string ciphertext = LAB_Encryptor::encrypt_string 
                            (plaintext, password);
  
  std::cout << "plaintext: " << plaintext << "\n";
  std::cout << "password: " << password << "\n";
  std::cout << "ciphertext: " << ciphertext << "\n";

  std::cout << "********************" << "\n";

  std::string recovered = LAB_Encryptor::decrypt_string (ciphertext, password);

  std::cout << "recovered: " << recovered << "\n";

  std::getchar ();
}