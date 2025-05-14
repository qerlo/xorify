# xorify

**xorify** is a simple command-line tool to encrypt and decrypt files using the XOR cipher. It takes a key and applies the XOR operation to each byte of the file, producing an encrypted (or decrypted) output.

## Features

- Encrypts or decrypts any type of file using a user-provided key.
- Generate a key of a specified length.
- Cross-platform: Works on **Linux** and **Windows** (only via MSYS2, Windows terminal needs to be fixed).
- Simple and lightweight — written in C.

## Build

To build `xorify`, simply run:

```bash
gcc xorify.c -o xorify
```

## Examples
Generate a key of length 100 bytes:
```bash
xorify -gk 100
```
If you want to store it in a file:
```bash
xorify -gk 100 > key_file
```
Encrypt a file:
```bash
xorify key_file file_to_encrypt > encrypted 
```
Decrypt a file:
```bash
xorify key_file encrypted > decrypted
```
