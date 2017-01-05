#include <fstream>
#include <iostream>
#include <vector>

#include "range/v3/all.hpp"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

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

class Screen {
public:
  Screen(int width = 50, int height = 6)
      : width_(width), height_(height), screen_(width * height, false) {}

  int numberOfLitPixels(const std::vector<std::string> &lines);
  std::string dump() const;

private:
  void applyLine(const std::string &line);
  void applyRect(int width, int height);
  void applyRowRotation(int row, int amount);
  void applyColRotation(int col, int amount);

  int width_;
  int height_;
  std::vector<bool> screen_;
};

int Screen::numberOfLitPixels(const std::vector<std::string> &lines) {
  ranges::for_each(lines, [this](const auto &line) { applyLine(line); });
  return ranges::count_if(screen_, [](auto b) { return b; });
}

std::string Screen::dump() const {
  std::string out;
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      out += (screen_[i * width_ + j] == true ? '#' : ' ');
    }
    out += "\n";
  }
  return out;
}

void Screen::applyLine(const std::string &line) {
  std::vector<std::string> splitLine;
  boost::split(splitLine, line, boost::is_any_of(" "));
  if (splitLine[0] == "rect") {
    boost::split(splitLine, splitLine[1], boost::is_any_of("x"));
    applyRect(boost::lexical_cast<int>(splitLine[0]),
              boost::lexical_cast<int>(splitLine[1]));
  } else if (splitLine[0] == "rotate") {
    auto rowOrColNum = boost::lexical_cast<int>(splitLine[2].substr(2));
    auto amount = boost::lexical_cast<int>(splitLine[4]);
    if (splitLine[1] == "row") {
      applyRowRotation(rowOrColNum, amount);
    } else if (splitLine[1] == "column") {
      applyColRotation(rowOrColNum, amount);
    }
  }
}

void Screen::applyRect(int width, int height) {
  for (int i = 0; i < height; ++i) {
    view::drop(screen_, i * width_) | view::take(width) |
        action::transform([](auto) { return true; });
  }
}

void Screen::applyRowRotation(int row, int amount) {
  auto r = view::drop(screen_, row * width_) | view::take(width_);
  ranges::rotate(r, r.begin() + width_ - amount);
}

void Screen::applyColRotation(int col, int amount) {
  auto r =
      view::drop(screen_, col) | view::stride(width_) | view::take(height_);
  ranges::rotate(r, r.begin() + height_ - amount);
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }
  auto lines = readFile(argv[1]);
  Screen screen;
  if (argc > 3) {
    screen = Screen(boost::lexical_cast<int>(argv[2]),
                    boost::lexical_cast<int>(argv[3]));
  }
  std::cout << screen.numberOfLitPixels(lines) << "\n\n";
  std::cout << screen.dump() << "\n";
  return 0;
}
