#pragma once
#include <random>
#include <iostream>

#include "openssl/include/openssl/aes.h"
#include "openssl/include/openssl/sha.h"

namespace skc {
    template<class _Ty>
    using clean_type = typename std::remove_const_t<std::remove_reference_t<_Ty>>;

    template <int _size, typename T>
    class skCrypter_CUSTOM
    {
    public:
        __forceinline constexpr skCrypter_CUSTOM(T* data)
        {
            //this->EncryptionKey = 3;
            this->EncryptionKey = this->GenerateRandomInteger(INT_MIN, INT_MAX);
            this->KeyEncryptionKey = 5;
            this->EncryptKey();
            crypt(data);
        }

        __forceinline T* get()
        {
            return _storage;
        }

        __forceinline int size() // (w)char count
        {
            return _size;
        }

        __forceinline int key()
        {
            return this->EncryptionKey;
        }

        __forceinline T* encrypt()
        {
            if (!isEncrypted())
            {
                crypt(_storage);
            }

            return _storage;
        }

        __forceinline T* decrypt()
        {
            if (isEncrypted())
            {
                decrypt(_storage);
            }

            return _storage;
        }

        __forceinline bool isEncrypted()
        {
            return _storage[_size - 1] != 0;
        }

        __forceinline void clear() // set full storage to 0
        {
            for (int i = 0; i < _size; i++)
            {
                _storage[i] = 0;
            }
        }

        __forceinline operator T* ()
        {
            decrypt();

            return _storage;
        }

    private:
        int EncryptionKey;
        int KeyEncryptionKey;
        bool bIsKeyEncoded;

        __forceinline bool IsKeyEncoded()
        {
            return bIsKeyEncoded;
        }

        __forceinline void DecryptKey()
        {
            if (IsKeyEncoded())
            {
                EncryptionKey -= KeyEncryptionKey;
                this->bIsKeyEncoded = false;
            }
        }

        __forceinline void EncryptKey()
        {
            if (!IsKeyEncoded())
            {
                EncryptionKey += KeyEncryptionKey;
                this->bIsKeyEncoded = true;
            }
        }

        __forceinline constexpr void crypt(T* data)
        {
            for (int i = 0; i < _size; i++)
            {
                DecryptKey();
                _storage[i] = data[i] ^ (this->EncryptionKey + i % (1 + this->EncryptionKey));
                EncryptKey();
            }
        }

        __forceinline constexpr void decrypt(T* data)
        {
            for (int i = 0; i < _size; i++)
            {
                DecryptKey();
                _storage[i] = data[i] ^ (this->EncryptionKey + i % (1 + this->EncryptionKey));
                EncryptKey();
            }
        }

        __forceinline int GenerateRandomInteger(int min, int max) {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(min, max);

            return distribution(eng);
        }

        T _storage[_size]{};
    };

    template <int _size, char _key1, typename T>
    class skCrypter_AES {
    public:
        __forceinline constexpr skCrypter_AES(T* data) {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(0, 255);
            for (int i = 0; i < AES_BLOCK_SIZE; ++i) {
                encryptionKey[i] = distribution(eng);
            }

            AES_set_encrypt_key(encryptionKey, 128, &aesKey);
            AES_set_decrypt_key(encryptionKey, 128, &aesKey);

            crypt(data);
        }

        __forceinline T* get() {
            return _storage;
        }

        __forceinline int size() {
            return _size;
        }

        __forceinline int key() {
            return *encryptionKey;
        }


        __forceinline bool isEncrypted() {
            return _storage[_size - 1] != 0;
        }

        __forceinline T* encrypt() {
            if (!isEncrypted()) {
                crypt(_storage);
            }
            return _storage;
        }

        __forceinline T* decrypt() {
            if (isEncrypted()) {
                decrypt(_storage);
            }
            return _storage;
        }


        __forceinline void clear() {
            for (int i = 0; i < _size; i++) {
                _storage[i] = 0;
            }
        }

        __forceinline operator T* () {
            decrypt();
            return _storage;
        }

    private:
        AES_KEY aesKey;
        int EncryptionKey;
        unsigned char encryptionKey[16]; // Assuming 128-bit AES key
        T _storage[_size]{};

        __forceinline int GenerateRandomInteger(int min, int max) {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(min, max);
            return distribution(eng);
        }

        __forceinline void crypt(T* data) {
            for (int i = 0; i < _size; i += AES_BLOCK_SIZE) {
                AES_encrypt((const unsigned char*)(data + i), (unsigned char*)(_storage + i), &aesKey);
            }
        }

        __forceinline void decrypt(T* data) {
            for (int i = 0; i < _size; i += AES_BLOCK_SIZE) {
                AES_decrypt((const unsigned char*)(data + i), (unsigned char*)(_storage + i), &aesKey);
            }
        }
    };

    template <int _size, char _key1, typename T>
    class skCrypter_SHA {
    public:
        __forceinline constexpr skCrypter_SHA(T* data) {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(0, 255);
            for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
                encryptionKey[i] = distribution(eng);
            }

            SHA256(encryptionKey, SHA256_DIGEST_LENGTH, hash);

            encrypt(data);
        }

        __forceinline T* get() {
            return _storage;
        }

        __forceinline int size() {
            return _size;
        }

        __forceinline int key() {
            return *encryptionKey;
        }

        __forceinline T* encrypt() {
            encrypt(_storage);
            return _storage;
        }

        __forceinline T* decrypt() {
            decrypt(_storage);
            return _storage;
        }

        __forceinline bool IsEncrypted() {
            return this->bIsEncrypted;
        }

        __forceinline void clear() {
            for (int i = 0; i < _size; i++) {
                _storage[i] = 0;
            }
        }

        __forceinline operator T* () {
            decrypt();
            return _storage;
        }

    private:
        unsigned char encryptionKey[SHA256_DIGEST_LENGTH]{};
        unsigned char hash[SHA256_DIGEST_LENGTH]{};
        T _storage[_size]{};
        bool bIsEncrypted;

        __forceinline void encrypt(T* data) {
            if (!this->IsEncrypted())
            {
                SHA256_CTX ctx;
                SHA256_Init(&ctx);
                SHA256_Update(&ctx, data, _size);
                SHA256_Final(hash, &ctx);

                SHA256(encryptionKey, SHA256_DIGEST_LENGTH, hash);

                for (int i = 0; i < _size; ++i) {
                    _storage[i] = data[i] ^ hash[i % SHA256_DIGEST_LENGTH];
                }

                this->bIsEncrypted = true;
            }
        }

        __forceinline void decrypt(T* data) {
            if (this->IsEncrypted())
            {
                for (int i = 0; i < _size; ++i) {
                    data[i] = _storage[i] ^ hash[i % SHA256_DIGEST_LENGTH];
                }

                this->bIsEncrypted = false;
            }
        }

        __forceinline int GenerateRandomInteger(int min, int max) {
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(min, max);
            return distribution(eng);
        }
    };

    template<typename T>
    class SHAStringEncryption
    {
    public:
        SHAStringEncryption(T* _var) : var(_var)
        {
            var = _var;
            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_int_distribution<> distribution(0, 255);
            for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
                encryptionKey[i] = distribution(eng);
            }

            SHA256(encryptionKey, SHA256_DIGEST_LENGTH, hash);
            Encrypt();
        }

        ~SHAStringEncryption()
        {
            if (var != nullptr) {
                var = nullptr;
            }
        }

        T* get()
        {
            return var;
        }

        bool IsEncoded()
        {
            return this->IsEncrypted;
        }

        void clear()
        {
            if (var != nullptr) {
                var = nullptr;
            }
            IsEncrypted = false;
        }

        operator T* ()
        {
            Decrypt();
            return var;
        }

        T* decode()
        {
            Decrypt();
            return var;
        }

        T* encode()
        {
            Encrypt();
            return var;
        }

    private:
        T* var;
        bool IsEncrypted;
        unsigned char encryptionKey[SHA256_DIGEST_LENGTH]{};
        unsigned char hash[SHA256_DIGEST_LENGTH]{};
        int _size = sizeof(var) / sizeof(var[0]);

        void Encrypt()
        {
            if (!IsEncoded())
            {
                SHA256_CTX ctx;
                SHA256_Init(&ctx);
                SHA256_Update(&ctx, var, _size);
                SHA256_Final(hash, &ctx);

                SHA256(encryptionKey, SHA256_DIGEST_LENGTH, hash);

                for (int i = 0; i < _size; ++i) {
                    var[i] = var[i] ^ hash[i % SHA256_DIGEST_LENGTH];
                }

                IsEncrypted = true;
            }
        }

        void Decrypt()
        {
            if (IsEncoded())
            {
                for (int i = 0; i < _size; ++i) {
                    var[i] = var[i] ^ hash[i % SHA256_DIGEST_LENGTH];
                }

                IsEncrypted = false;
            }
        }
    };
}

#define SECURECRYPT(str) []() { \
            static auto crypted = skc::skCrypter_CUSTOM<sizeof(str) / sizeof(str[0]), skc::clean_type<decltype(str[0])>>((skc::clean_type<decltype(str[0])>*)str); \
                    return crypted; }()