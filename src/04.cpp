#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

struct Frequency {
  char c = 0;
  unsigned num = 0;
};

using Histogram = std::array<Frequency, std::numeric_limits<char>::max()>;
using Checksum = std::string;

Histogram HistogramFactory(const std::string &s) {
  Histogram h;
  char c = 0;
  std::generate(h.begin(), h.end(), [&c]() { return Frequency{c++, 0}; });
  std::for_each(s.begin(), s.end(), [&h](auto c) { ++h[c].num; });
  h['-'].num = 0; // '-' is ignored as a character for counting.
  return h;
}

Checksum ChecksumFactory(const Histogram &h) {
  Checksum check = std::accumulate(h.rbegin(), h.rend(), std::string(),
                                   [](const auto &accum, const auto f) {
                                     if (f.num > 0) {
                                       return accum + f.c;
                                     }
                                     return accum;
                                   });
  return check.substr(0, 5);
}

std::string decrypt(const std::string &enc, int rotate) {
  std::string dec;
  std::transform(enc.begin(), enc.end(), std::back_inserter(dec),
                 [rotate](auto c) {
                   if (c == '-') {
                     return ' ';
                   }
                   c = std::toupper(c);
                   c += rotate % 26;
                   if (c > 'Z') {
                     c -= 26;
                   }
                   return c;
                 });
  return dec;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }

  int sectorSum = 0;
  std::string line;
  std::ifstream in(argv[1]);
  while (std::getline(in, line)) {
    std::vector<std::string> sl;
    boost::trim(line);
    if (line.empty()) {
      continue;
    }
    std::string enc(line.begin(), line.begin() + line.find_last_of("-") + 1);
    std::string sectorStr(line.begin() + enc.size(),
                          line.begin() + line.find_last_of("["));
    std::string checksum(
        line.begin() + enc.size() + sectorStr.size() + 1,
        std::min(line.begin() + enc.size() + sectorStr.size() + 1 + 5,
                 line.begin() + line.find_last_of("]")));

    auto hist = HistogramFactory(enc);
    std::sort(hist.begin(), hist.end(), [](const auto &lhs, const auto &rhs) {
      if ((lhs.num == rhs.num) && (lhs.num > 0)) {
        return lhs.c > rhs.c;
      }
      return lhs.num < rhs.num;
    });
    auto check = ChecksumFactory(hist);
    if (check == checksum) {
      auto sector = boost::lexical_cast<int>(sectorStr);
      sectorSum += sector;
      std::cout << decrypt(enc, sector) << " " << sector << "\n";
    }
  }

  std::cout << "Sum of sectors: " << sectorSum << "\n";
  return 0;
}
