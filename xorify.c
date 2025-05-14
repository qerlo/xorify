#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
	#include <fcntl.h>
	#include <io.h>
#endif

#define ERROR -1
#define HELP 0
#define XORIFY 1
#define GENKEY 2

#define USAGE "Usage: xorify [OPTION] <length> | <key_path> <file_path>\n\
\n\
Encrypt or decrypt a file using a simple method.\n\
\n\
Options:\n\
  -gk, --genkey    Generate key of the specified length (bytes)\n\
  --help           Display this help message and exit.\n\
\n\
Arguments:\n\
  <key_path>       The path to the key\n\
  <file_path>      The path to the file to encrypt or decrypt.\n\
\n\
Examples:\n\
  xorify key.bin my_document.txt\n\
  xorify --genkey 100\n"

int validate_arguments(int argc, char *argv[]);
int xorify(char *key_path, char *file_path);
int genkey(long length);

int main(int argc, char *argv[])
{
	#ifdef _WIN32
		fflush(stdout);
		if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
			fprintf(stderr, "Failed to set stdout to binary mode\n");
			return ERROR;
		}
	#endif
	
	srand(time(NULL));
	int res = validate_arguments(argc, argv);
	switch (res) {
		case XORIFY:
			return xorify(argv[1], argv[2]);
		case GENKEY:
			return genkey(atol(argv[2]));
		default:
			fprintf(stderr, USAGE);
	}
	return ERROR;
}

int validate_arguments(int argc, char *argv[])
{
	if (argc == 3) {
		if (strcmp(argv[1], "-gk") == 0 || strcmp(argv[1], "--genkey") == 0) {
			return GENKEY;
		} else {
			return XORIFY;
		}
	}
	
	return HELP;
}

int xorify(char *key_path, char *file_path)
{
	FILE *key_fp = fopen(key_path, "rb");
	if (key_fp == NULL) {
		fprintf(stderr, "An error occurred while reading the key\n");
		return ERROR;
	}
	FILE *file_fp = fopen(file_path, "rb");
	if (file_fp == NULL) {
		fclose(key_fp);
		fprintf(stderr, "An error occurred while reading the file\n");
		return ERROR;
	}
	fseek(key_fp, 0L, SEEK_END);
	fseek(file_fp, 0L, SEEK_END);
	long key_size = ftell(key_fp);
	long file_size = ftell(file_fp);
	if (key_size != file_size) {
		fclose(key_fp);
		fclose(file_fp);
		fprintf(stderr, "Can't encrypt the file because the key has a different size\n");
		return ERROR;
	}
	uint8_t key_buffer[8192];
	uint8_t file_buffer[8192];
	uint8_t encrypt_buffer[8192];
	size_t key_bytes_read = 0;
	size_t file_bytes_read = 0;
	fseek(key_fp, 0L, SEEK_SET);
	fseek(file_fp, 0L, SEEK_SET);
	while (((key_bytes_read = fread(key_buffer, sizeof(uint8_t), sizeof(key_buffer), key_fp)) > 0) && 
			((file_bytes_read = fread(file_buffer, sizeof(uint8_t), sizeof(file_buffer), file_fp))) > 0) {
		if (key_bytes_read != file_bytes_read) {
			fclose(key_fp);
			fclose(file_fp);
			fprintf(stderr, "An error occured during encryption (in op)\n");
			return ERROR;
		}
		// here key_bytes_read and file_bytes_read can be used interchangeably
		for (size_t i = 0; i < key_bytes_read; i++) {
			encrypt_buffer[i] = key_buffer[i] ^ file_buffer[i];
		}
		int written = fwrite(encrypt_buffer, sizeof(uint8_t), key_bytes_read, stdout);
		fflush(stdout);
		if (written != key_bytes_read) {
			fclose(key_fp);
			fclose(file_fp);
			fprintf(stderr, "An error occured during encryption (out op)\n");
			return ERROR;
		}
	}
	fclose(key_fp);
	fclose(file_fp);
	return 0;
}

int genkey(long length) 
{
	if (length <= 0) {
		fprintf(stderr, "Invalid key length\n");
		return ERROR;
	}
	uint8_t b = 0;
	for (int i = 0; i < length; i++) {
		b = rand();
		int written = fwrite(&b, sizeof(uint8_t), 1, stdout);
		if (written <= 0) {
			fprintf(stderr, "Something went wrong during key generation\n");
			return ERROR;
		}
	}
	fflush(stdout);
	return 0;
}