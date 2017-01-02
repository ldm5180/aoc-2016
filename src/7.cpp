#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <range/v3/all.hpp>

using namespace ranges::v3;

std::vector<std::string> readFile(const std::string &fn) {
  std::vector<std::string> res;
  std::ifstream file(fn);
  std::string line;
  while (std::getline(file, line)) {
    boost::trim(line);
    if (!line.empty()) {
      res.push_back(line);
    }
  }
  return res;
}

bool isMatch(const std::regex &re, const std::string &line) {
  std::smatch sm;
  auto searchLoc = line.cbegin();
  while (std::regex_search(searchLoc, line.cend(), sm, re)) {
    if (sm[1] != sm[2]) {
      return true;
    }
    searchLoc = sm[0].second;
  }
  return false;
}

bool isIpv7Tls(const std::string &line) {
  std::regex abba("(.)(?!\1)(.)\\2\\1");

  auto splitOnBrackets =
      view::split(line, [](const auto &c) { return c == '[' || c == ']'; });
  auto hypernets =
      splitOnBrackets | view::drop(1) | view::stride(2) | view::join('|');
  auto addrs = splitOnBrackets | view::stride(2) | view::join('|');

  std::string hypernetMatchStr = ranges::yield_from(hypernets);
  if (isMatch(abba, hypernetMatchStr)) {
    return false; // any hypernet match is a disqualification.
  }

  std::string addrMatchStr = ranges::yield_from(addrs);
  return isMatch(abba, addrMatchStr);
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
  std::cout << "Number of IPv7 TLS addresses: " << numIpv7Tls << "\n";
  return 0;
}
