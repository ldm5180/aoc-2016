#include <fstream>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

int isTriangle(std::vector<int> t) {
  std::sort(t.begin(), t.end());
  std::cout << "Triange: " << t[0] << "," << t[1] << "," << t[2] << "\n";
  return ((t[0] + t[1]) > t[2]) ? 1 : 0;
}

int numTriangles(const std::vector<int> &ts) {
  if (ts.empty()) {
    return 0;
  }

  int numTris = 0;
  for (unsigned i = 0; i < 3; ++i) {
    std::vector<int> aTri = {ts[0 + i], ts[3 + i], ts[6 + i]};
    numTris += isTriangle(aTri);
  }
  return numTris;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }

  unsigned numTris = 0;
  std::string sizes;
  std::ifstream in(argv[1]);
  std::vector<int> triangles;
  while (std::getline(in, sizes)) {
    int num;
    std::istringstream iss(sizes);
    while (iss >> num) {
      triangles.push_back(num);
    }
    if (triangles.size() == 9) {
      numTris += numTriangles(triangles);
      triangles.clear();
    }
  }
  std::cout << numTris << "\n";
  return 0;
}
