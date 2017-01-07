#include <algorithm>
#include <array>
#include <iostream>

#include <boost/optional.hpp>
#include <openssl/md5.h>
#include <range/v3/all.hpp>

using namespace ranges::v3;
using Digest = std::array<unsigned char, 16>;

bool isValidHash(const Digest &digest) {
  auto loc = digest[2] & 0x0F;
  return (digest[0] == 0) && (digest[1] == 0) && ((digest[2] & 0xF0) == 0) &&
         (loc < 8);
}

Digest generateHash(const std::string &input, unsigned index) {
  Digest digest;
  std::string key = input + std::to_string(index);
  MD5(reinterpret_cast<const unsigned char *>(key.data()), key.length(),
      digest.data());
  return digest;
}

std::tuple<int, int> hashToChar(const Digest &d) {
  return std::make_tuple(d[2] & 0x0F, (d[3] & 0xF0) >> 4);
}

template <typename T, typename U> bool isFound(T &container, const U &val) {
  if (container[std::get<0>(val)]) {
    return true;
  }
  container[std::get<0>(val)] = std::get<1>(val);
  return false;
}

template <typename T> bool isAllFound(const T &container) {
  return !std::all_of(container.begin(), container.end(),
                      [](auto &b) { return b; });
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide puzzle input on command line." << std::endl;
    return 1;
  }

  std::string input(argv[1]);
  std::vector<boost::optional<int>> positional(8);
  std::vector<std::tuple<int, int>> password = yield_from(
      view::ints(0) |
      view::transform([&input](auto i) { return generateHash(input, i); }) |
      view::remove_if([](const Digest &d) { return !isValidHash(d); }) |
      view::transform([](const Digest &d) { return hashToChar(d); }) |
      view::remove_if(
          [&positional](const auto &val) { return isFound(positional, val); }) |
      view::take_while(
          [&positional](const auto &) { return isAllFound(positional); }));
  ranges::for_each(positional, [](auto &c) { std::cout << std::hex << *c; });
  std::cout << "\n";
  return 0;
}
