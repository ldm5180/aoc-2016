#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <range/v3/all.hpp>

using namespace ranges::v3;

std::vector<std::string> readFile(const std::string &fn) {
  std::vector<std::string> res;
  std::ifstream file(fn);
  std::string line;
  while (std::getline(file, line)) {
    res.push_back(line);
  }
  return res;
}

std::vector<std::string> matchList(const std::regex &re,
                                   const std::string &line) {
  std::vector<std::string> list;
  std::smatch sm;
  std::sregex_iterator next(line.begin(), line.end(), re);
  while (next != std::sregex_iterator()) {
    list.push_back(next->str(1));
    ++next;
  }
  return list;
}

bool isIpv7Tls(const std::string &line) {
  std::regex abba("(.)(?!\1)(.)\\2\\1");
  auto isBracket = [](auto c) { return c == '[' || c == ']'; };
  if (!matchList(abba, yield_from(view::split(line, isBracket) | view::drop(1) |
                                  view::stride(2) | view::join('|')))
           .empty()) {
    return false; // any hypernet match is a disqualification.
  }

  return !matchList(abba, yield_from(view::split(line, isBracket) |
                                     view::stride(2) | view::join('|')))
              .empty();
}

bool isIpv7Ssl(const std::string &line) {
  std::regex aba("(?=((.)(?!\\2).\\2))");
  auto splitOnBrackets =
      view::split(line, [](const auto &c) { return c == '[' || c == ']'; });
  auto hypernets = matchList(aba, splitOnBrackets | view::drop(1) |
                                      view::stride(2) | view::join('|'));
  decltype(hypernets) invertedHypernets;
  std::transform(hypernets.begin(), hypernets.end(),
                 std::back_inserter(invertedHypernets), [](const auto &h) {
                   return std::string{h[1], h[0], h[1]};
                 });
  auto addrs =
      matchList(aba, splitOnBrackets | view::stride(2) | view::join('|'));

  std::sort(addrs.begin(), addrs.end());
  std::sort(invertedHypernets.begin(), invertedHypernets.end());
  for (const auto &a : addrs) {
    auto lb =
        std::lower_bound(invertedHypernets.begin(), invertedHypernets.end(), a);
    if ((lb != invertedHypernets.end()) && (*lb == a)) {
      return true;
    }
  }
  return false;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }
  auto lines = readFile(argv[1]);
  auto numIpv7Tls =
      std::count_if(lines.begin(), lines.end(),
                    [](const auto &line) { return isIpv7Tls(line); });
  auto numIpv7Ssl =
      std::count_if(lines.begin(), lines.end(),
                    [](const auto &line) { return isIpv7Ssl(line); });
  std::cout << "Number of IPv7 TLS addresses: " << numIpv7Tls << "\n";
  std::cout << "Number of IPv7 SSL addresses: " << numIpv7Ssl << "\n";
  return 0;
}
