#include <openssl/md5.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <range/v3/view.hpp>

using Digest = std::array<unsigned char, 16>;

bool isValidHash(const Digest &digest) {
  return (digest[0] == 0) && (digest[1] == 0) && ((digest[2] & 0xF0) == 0);
}

Digest generateHash(const std::string &input, unsigned index) {
  Digest digest;
  std::string key = input + std::to_string(index);
  MD5(reinterpret_cast<const unsigned char *>(key.data()), key.length(),
      digest.data());
  return digest;
}

int hashToChar(const Digest &d) { return d[2] & 0x0F; }

struct HashFinder {
  HashFinder(const std::string &input) : input_(input) {}
  Digest nextHash() {
    Digest digest;
    for (;;) {
      digest = generateHash(input_, index++);
      if (isValidHash(digest)) {
        break;
      }
    }
    return digest;
  }

private:
  std::string input_;
  unsigned index = 0;
};

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide puzzle input on command line." << std::endl;
    return 1;
  }

  HashFinder hf(argv[1]);
  std::array<int, 8> password;
  std::generate(password.begin(), password.end(),
                [&hf]() { return hashToChar(hf.nextHash()); });
  std::for_each(password.begin(), password.end(),
                [](auto c) { std::cout << std::hex << c; });
  std::cout << "\n";

  return 0;
}
