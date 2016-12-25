#include <openssl/md5.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <range/v3/all.hpp>

using namespace ranges::v3;
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

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide puzzle input on command line." << std::endl;
    return 1;
  }

  std::string input(argv[1]);
  auto password =
      view::ints(0) |
      view::transform([&input](int i) { return generateHash(input, i); }) |
      view::remove_if([](const Digest &d) { return !isValidHash(d); }) |
      view::transform([](const Digest &d) { return hashToChar(d); }) |
      view::take(8);
  ranges::for_each(password, [](auto c) { std::cout << std::hex << c; });
  std::cout << "\n";
  return 0;
}
