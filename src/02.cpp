#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <boost/optional.hpp>

using boi = boost::optional<int>;
using Keypad = std::vector<std::vector<boi>>;

class Person {
public:
  Person(const Keypad &k) : keys(k) {
    for (row = 0; row < static_cast<int>(keys.size()); ++row) {
      for (col = 0; col < static_cast<int>(keys[row].size()); ++col) {
        if (keys[row][col] && keys[row][col] == 5) {
          std::cout << "5 is at [" << row << "][" << col << "]\n";
          break;
        }
      }
      if (col < static_cast<int>(keys[row].size())) {
        break;
      }
    }
  }

  int findButton(const std::string &moves) {
    std::for_each(moves.begin(), moves.end(),
                  [this](const auto &direction) { move(direction); });
    return *keys[row][col];
  }

  void move(const char &c) {
    boi newRow;
    boi newCol;
    switch (c) {
    case 'U':
      newRow = std::max(0, row - 1);
      break;
    case 'D':
      newRow = std::min(static_cast<int>(keys.size()) - 1, row + 1);
      break;
    case 'L':
      newCol = std::max(0, col - 1);
      break;
    case 'R':
      newCol = std::min(static_cast<int>(keys[row].size()) - 1, col + 1);
      break;
    }
    if (newRow && keys[*newRow][col]) {
      row = *newRow;
    } else if (newCol && keys[row][*newCol]) {
      col = *newCol;
    }
  }

private:
  Keypad keys;
  int row;
  int col;
};

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }

  boost::optional<int> NaN;
  // clang-format off
  Keypad keypad1{
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
  };
  Keypad keypad2{
    {NaN, NaN,   1, NaN, NaN},
    {NaN,   2,   3,   4, NaN},
    {  5,   6,   7,   8,   9},
    {NaN, 0xA, 0xB, 0xC, NaN},
    {NaN, NaN, 0xD, NaN, NaN}
  };
  // clang-format on

  std::string moves;
  std::ifstream in(argv[1]);
  Person p(keypad2);
  while (std::getline(in, moves)) {
    boost::trim(moves);
    std::cout << std::hex << std::uppercase << p.findButton(moves);
  }
  std::cout << "\n";
  return 0;
}
