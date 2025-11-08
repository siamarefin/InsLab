from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA256
from Crypto.Random import get_random_bytes
import time
import os

# ---------- Helper functions ----------

def generate_keys():
    # AES key (256 bits)
    aes_key = get_random_bytes(32)
    with open("aes.key", "wb") as f:
        f.write(aes_key)

    # RSA key pair (2048 bits)
    rsa_key = RSA.generate(2048)
    private_key = rsa_key.export_key()
    public_key = rsa_key.publickey().export_key()

    with open("rsa_private.pem", "wb") as f:
        f.write(private_key)
    with open("rsa_public.pem", "wb") as f:
        f.write(public_key)

    print("✅ Keys generated and saved to files: aes.key, rsa_private.pem, rsa_public.pem\n")


def aes_encrypt():
    key = open("aes.key", "rb").read()
    cipher = AES.new(key, AES.MODE_EAX)
    plaintext = input("Enter text to encrypt: ").encode()
    start = time.perf_counter()
    ciphertext, tag = cipher.encrypt_and_digest(plaintext)
    end = time.perf_counter()
    with open("aes_encrypted.bin", "wb") as f:
        [f.write(x) for x in (cipher.nonce, tag, ciphertext)]
    print(f"✅ AES encrypted file saved as aes_encrypted.bin")
    print(f"⏱️ Time taken: {(end - start)*1000:.3f} ms\n")


def aes_decrypt():
    key = open("aes.key", "rb").read()
    with open("aes_encrypted.bin", "rb") as f:
        nonce, tag, ciphertext = [f.read(x) for x in (16, 16, -1)]
    cipher = AES.new(key, AES.MODE_EAX, nonce=nonce)
    start = time.perf_counter()
    plaintext = cipher.decrypt_and_verify(ciphertext, tag)
    end = time.perf_counter()
    print("✅ Decrypted message:", plaintext.decode())
    print(f"⏱️ Time taken: {(end - start)*1000:.3f} ms\n")


def rsa_encrypt():
    public_key = RSA.import_key(open("rsa_public.pem").read())
    cipher_rsa = PKCS1_OAEP.new(public_key)
    message = input("Enter text to encrypt: ").encode()
    start = time.perf_counter()
    ciphertext = cipher_rsa.encrypt(message)
    end = time.perf_counter()
    with open("rsa_encrypted.bin", "wb") as f:
        f.write(ciphertext)
    print(f"✅ RSA encrypted file saved as rsa_encrypted.bin")
    print(f"⏱️ Time taken: {(end - start)*1000:.3f} ms\n")


def rsa_decrypt():
    private_key = RSA.import_key(open("rsa_private.pem").read())
    cipher_rsa = PKCS1_OAEP.new(private_key)
    ciphertext = open("rsa_encrypted.bin", "rb").read()
    start = time.perf_counter()
    message = cipher_rsa.decrypt(ciphertext)
    end = time.perf_counter()
    print("✅ Decrypted message:", message.decode())
    print(f"⏱️ Time taken: {(end - start)*1000:.3f} ms\n")


def sha256_hash():
    filename = input("Enter file name to hash: ")
    if not os.path.exists(filename):
        print("❌ File not found.\n")
        return
    with open(filename, "rb") as f:
        data = f.read()
    start = time.perf_counter()
    hash_value = SHA256.new(data).hexdigest()
    end = time.perf_counter()
    print(f"🔹 SHA-256 hash: {hash_value}")
    print(f"⏱️ Time taken: {(end - start)*1000:.3f} ms\n")

import matplotlib.pyplot as plt

def timing_experiments():
    print("\n=== Timing Experiment ===")

    # ---------- AES Timing ----------
    from Crypto.Cipher import AES
    from Crypto.Random import get_random_bytes
    import time

    aes_key = get_random_bytes(32)  # 256-bit key
    aes_times = []
    sizes = [16, 64, 128, 256, 512]  # in bytes

    for size in sizes:
        data = get_random_bytes(size)
        cipher = AES.new(aes_key, AES.MODE_EAX)
        start = time.perf_counter()
        ciphertext, tag = cipher.encrypt_and_digest(data)
        end = time.perf_counter()
        elapsed = (end - start) * 1000  # ms
        aes_times.append(elapsed)
        print(f"AES {size*8:>4} bits plaintext -> {elapsed:.3f} ms")

    plt.figure()
    plt.plot([s * 8 for s in sizes], aes_times, marker="o")
    plt.title("AES Encryption Time vs Data Size")
    plt.xlabel("Data Size (bits)")
    plt.ylabel("Time (ms)")
    plt.grid(True)
    plt.savefig("aes_time.png")
    print("✅ AES timing graph saved as aes_time.png")

    # ---------- RSA Timing ----------
    from Crypto.PublicKey import RSA
    from Crypto.Cipher import PKCS1_OAEP

    rsa_sizes = [512, 1024, 2048, 3072, 4096]
    rsa_enc_times = []
    rsa_dec_times = []

    message = b"This is a test message."

    for bits in rsa_sizes:
        rsa_key = RSA.generate(bits)
        public_key = rsa_key.publickey()
        cipher_rsa_enc = PKCS1_OAEP.new(public_key)
        cipher_rsa_dec = PKCS1_OAEP.new(rsa_key)

        # Encryption time
        start = time.perf_counter()
        ciphertext = cipher_rsa_enc.encrypt(message)
        end = time.perf_counter()
        enc_time = (end - start) * 1000

        # Decryption time
        start = time.perf_counter()
        plaintext = cipher_rsa_dec.decrypt(ciphertext)
        end = time.perf_counter()
        dec_time = (end - start) * 1000

        rsa_enc_times.append(enc_time)
        rsa_dec_times.append(dec_time)
        print(f"RSA {bits} bits -> Enc: {enc_time:.3f} ms, Dec: {dec_time:.3f} ms")

    plt.figure()
    plt.plot(rsa_sizes, rsa_enc_times, marker="o", label="Encryption")
    plt.plot(rsa_sizes, rsa_dec_times, marker="s", label="Decryption")
    plt.title("RSA Encryption/Decryption Time vs Key Size")
    plt.xlabel("Key Size (bits)")
    plt.ylabel("Time (ms)")
    plt.legend()
    plt.grid(True)
    plt.savefig("rsa_time.png")
    print("✅ RSA timing graph saved as rsa_time.png\n")


# ---------- Main Menu ----------
def main():
    while True:
        print("=== Crypto Menu ===")
        print("1. Generate AES & RSA keys")
        print("2. AES Encrypt")
        print("3. AES Decrypt")
        print("4. RSA Encrypt")
        print("5. RSA Decrypt")
        print("6. SHA-256 Hash")
        print("7. Timing Experiments")
        print("8. Exit")

        choice = input("Choose option: ")

        if choice == "1":
            generate_keys()
        elif choice == "2":
            aes_encrypt()
        elif choice == "3":
            aes_decrypt()
        elif choice == "4":
            rsa_encrypt()
        elif choice == "5":
            rsa_decrypt()
        elif choice == "6":
            sha256_hash()
        elif choice == "7":
            timing_experiments()
        elif choice == "8":
            print("Exiting...")
            break
        else:
            print("Invalid choice.\n")

if __name__ == "__main__":
    main()

