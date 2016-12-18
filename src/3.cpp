#include <fstream>
#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

int isTriangle(std::vector<int> t) {
  if (t.empty()) {
    return 0;
  }
  std::sort(t.begin(), t.end());
  std::cout << "Triange: " << t[0] << "," << t[1] << "," << t[2] << "\n";
  return ((t[0] + t[1]) > t[2]) ? 1 : 0;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }

  unsigned numTriangles = 0;
  std::string sizes;
  std::ifstream in(argv[1]);
  while (std::getline(in, sizes)) {
    int num;
    std::vector<int> triangle;
    std::istringstream iss(sizes);
    while (iss >> num) {
      triangle.push_back(num);
    }
    numTriangles += isTriangle(triangle);
  }
  std::cout << numTriangles << "\n";
  return 0;
}
