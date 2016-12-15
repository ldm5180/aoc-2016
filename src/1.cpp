#include <cstdlib>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

class Person {
  enum class Heading : int { North = 0, East, South, West };

public:
  Person() : heading_(Heading::North), position_(std::make_pair(0, 0)) {}
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
      position_.first += std::atoi(&movement[1]);
    } else if (heading() == Heading::South) {
      position_.first -= std::atoi(&movement[1]);
    } else if (heading() == Heading::East) {
      position_.second += std::atoi(&movement[1]);
    } else if (heading() == Heading::West) {
      position_.second -= std::atoi(&movement[1]);
    }
  }
  int distance() const {
    return std::abs(position_.first) + std::abs(position_.second);
  }
  std::string position() const {
    return std::to_string(position_.first) + "," +
           std::to_string(position_.second);
  }
  Heading heading() const {
    return static_cast<Heading>(static_cast<int>(heading_) % 4);
  }

private:
  Heading heading_;
  std::pair<int, int> position_;
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
            << " --- Position: " << p.position() << "\n";
  while (std::getline(in, movement, ',')) {
    boost::trim(movement);
    p.turn(movement);
    p.move(movement);
    std::cout << "Move: " << movement
              << " --- Heading: " << static_cast<int>(p.heading())
              << " --- Position: " << p.position() << "\n";
  }

  std::cout << p.distance() << std::endl;
  return 0;
}
