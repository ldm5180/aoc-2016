#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <boost/algorithm/string/trim.hpp>

using Position = std::pair<int, int>;
std::string positionToString(const Position &p) {
  return std::to_string(p.first) + "," + std::to_string(p.second);
}

int distance(const Position &p) {
  return std::abs(p.first) + std::abs(p.second);
}

class Person {
  enum class Heading : int { North = 0, East, South, West };

public:
  Person() : heading_(Heading::North), position_(std::make_pair(0, 0)) {
    past_.push_back(position_);
  }
  void turn(const std::string &movement) {
    if (movement[0] == 'L') {
      heading_ = static_cast<Heading>((static_cast<int>(heading_) - 1));
    } else {
      heading_ = static_cast<Heading>((static_cast<int>(heading_) + 1));
    }
    if (static_cast<int>(heading_) == -1) {
      heading_ = Heading::West;
    }
  }
  void move(const std::string &movement) {
    if (heading() == Heading::North) {
      std::generate_n(std::back_inserter(past_), std::atoi(&movement[1]),
                      [this]() {
                        ++position_.first;
                        return position_;
                      });
    } else if (heading() == Heading::South) {
      std::generate_n(std::back_inserter(past_), std::atoi(&movement[1]),
                      [this]() {
                        --position_.first;
                        return position_;
                      });
    } else if (heading() == Heading::East) {
      std::generate_n(std::back_inserter(past_), std::atoi(&movement[1]),
                      [this]() {
                        ++position_.second;
                        return position_;
                      });
    } else if (heading() == Heading::West) {
      std::generate_n(std::back_inserter(past_), std::atoi(&movement[1]),
                      [this]() {
                        --position_.second;
                        return position_;
                      });
    }
  }
  Heading heading() const {
    return static_cast<Heading>(static_cast<int>(heading_) % 4);
  }
  const Position &position() const { return position_; }
  Position firstDupPosition() {
    std::unordered_set<std::string> positions;
    auto dup =
        std::find_if(past_.begin(), past_.end(), [&positions](const auto &p) {
          auto result = positions.insert(positionToString(p));
          return !result.second;
        });
    return *dup;
  }

private:
  Heading heading_;
  Position position_;
  std::vector<Position> past_;
};

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }

  std::string movement;
  std::ifstream in(argv[1]);
  Person p;
  std::cout << " --- Heading: " << static_cast<int>(p.heading())
            << " --- Position: " << positionToString(p.position()) << "\n";
  while (std::getline(in, movement, ',')) {
    boost::trim(movement);
    p.turn(movement);
    p.move(movement);
    std::cout << "Move: " << movement
              << " --- Heading: " << static_cast<int>(p.heading())
              << " --- Position: " << positionToString(p.position()) << "\n";
  }

  std::cout << "Location HQ: " << positionToString(p.firstDupPosition())
            << "\n";
  std::cout << "Distance to HQ: " << distance(p.firstDupPosition()) << "\n";
  std::cout << "End: " << distance(p.position()) << "\n";
  return 0;
}
