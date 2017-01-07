#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

class Decompressor {
public:
  std::size_t outputSize(const std::vector<std::string> &lines);
  std::size_t lengthSimple(const std::string &line);
};

struct Marker {
  std::size_t start = std::string::npos;
  std::size_t stop = std::string::npos;
};

struct Expander {
  std::size_t length;
  std::size_t repeat;
  std::size_t expandedLength() const { return length * repeat; }
};

Expander expanderFactory(const std::string &line, Marker m) {
  Expander e;
  auto x = line.find_first_of('x', m.start);
  e.length = boost::lexical_cast<std::size_t>(
      line.substr(m.start + 1, x - (m.start + 1)));
  e.repeat =
      boost::lexical_cast<std::size_t>(line.substr(x + 1, m.stop - (x + 1)));
  return e;
}

Marker nextExpansion(const std::string &line, const std::size_t startLoc) {
  Marker m;
  m.start = line.find_first_of('(', startLoc);
  if (m.start == std::string::npos) {
    return m;
  }
  m.stop = line.find_first_of(')', m.start);
  return m;
}

std::size_t Decompressor::outputSize(const std::vector<std::string> &lines) {
  return std::accumulate(lines.begin(), lines.end(), 0U,
                         [this](const unsigned accum, const auto &line) {
                           return accum + lengthSimple(line);
                         });
}

std::size_t Decompressor::lengthSimple(const std::string &line) {
  std::size_t length = 0;
  std::size_t cursor = 0;
  for (auto m = nextExpansion(line, cursor);
       m.start != std::string::npos && m.stop != std::string::npos;
       m = nextExpansion(line, cursor)) {
    Expander ex = expanderFactory(line, m);
    length += (m.start - cursor) + ex.expandedLength();
    cursor = (m.stop + 1) + ex.length;
  }
  return length + (line.size() - cursor);
}

std::vector<std::string> readFile(const std::string &fn) {
  std::vector<std::string> res;
  std::ifstream file(fn);
  std::string line;
  while (std::getline(file, line)) {
    res.push_back(line);
  }
  return res;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }
  auto lines = readFile(argv[1]);
  Decompressor d;
  auto length = d.outputSize(lines);
  std::cout << length << "\n";
};
