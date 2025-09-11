# Blockchain – Crypto

## Resources
#### Read or Watch
- [OpenSSL crypto library](https://www.openssl.org/docs/manmaster/man7/crypto.html)
- [SHA: Secure Hashing Algorithm](https://en.wikipedia.org/wiki/SHA-2)
- [Elliptic Curve Cryptography (ECC)](https://en.wikipedia.org/wiki/Elliptic-curve_cryptography)
- [ECDSA](https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm)
- How a blockchain works (intro articles/videos of your choice)

## Learning Objectives
At the end of this project, you are expected to be able to [explain to anyone](https://fs.blog/feynman-learning-technique/), **without the help of Google**:

### General
- How a blockchain achieves tamper-resistance (“unbreakable” in practice)
- What a hash algorithm is and what **SHA** stands for
- How hash algorithms apply to blockchains
- What asymmetric cryptography is
- How asymmetric cryptography applies to cryptocurrencies
- What **ECC** (Elliptic Curve Cryptography) is
- What **ECDSA** is
- What a digital signature is and how it applies to cryptocurrencies

## Requirements
### General
- Allowed editors: `vi`, `vim`, `emacs`
- All your files will be compiled on **Ubuntu 20.04 LTS**
- Your C programs and functions will be compiled with **gcc 9.\*** using the flags:  
  `-Wall -Werror -Wextra -pedantic -Wno-deprecated-declarations` and linker flags `-lssl -lcrypto`
- All your files should end with a new line
- A `README.md` file at the root of the project is **mandatory**
- Your code should use the **Betty** style (checked with `betty-style.pl` and `betty-doc.pl`)
- You are allowed to use the standard C library
- The prototypes of all your functions must be declared in the header **`crypto/hblk_crypto.h`**
- All header files must be **include-guarded**

## More Info
### Dependencies