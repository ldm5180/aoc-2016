#include <fstream>
#include <iostream>
#include <memory>

#include <boost/algorithm/string/trim.hpp>

#include "range/v3/all.hpp"

using namespace ranges::v3;
using IfstreamPtr = std::unique_ptr<std::ifstream>;

std::vector<std::string> readFile(const std::string &fn) {
  std::vector<std::string> res;
  std::ifstream file(fn);
  std::string line;
  while (std::getline(file, line)) {
    res.push_back(view::all(line));
  }
  return res;
}

template <typename T> char mostCommonLastChar(T &lines) {
  std::array<unsigned, 26> histogram;
  histogram.fill(0);
  ranges::for_each(lines, [&histogram](auto &line) {
    boost::trim(line);
    ++histogram[line.back() - 'a'];
    line.pop_back();
  });
  return std::distance(histogram.begin(),
                       std::min_element(histogram.begin(), histogram.end())) +
         'a';
}

std::string repeatedMsg(std::vector<std::string> lines) {
  std::string mostCommon(lines.front().size(), '\0');
  auto linesRange = view::all(lines) | view::remove_if([](const auto &line) {
                      return line.empty();
                    });
  std::generate(mostCommon.rbegin(), mostCommon.rend(),
                [&linesRange]() { return mostCommonLastChar(linesRange); });
  return mostCommon;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }
  auto lines = readFile(argv[1]);
  std::cout << repeatedMsg(lines) << "\n";
  return 0;
}
