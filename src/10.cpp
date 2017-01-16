#include <fstream>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

using namespace std::string_literals;

std::vector<std::string> readFile(const std::string &fn) {
  std::vector<std::string> res;
  std::ifstream file(fn);
  std::string line;
  while (std::getline(file, line)) {
    res.push_back(line);
  }
  return res;
}

class Token {
public:
  Token() = default;
  Token(int owner) : owner_(owner) {}
  void owner(int owner) { owner_ = owner; }
  int owner() const { return owner_; }

private:
  int owner_;
};

std::tuple<int, int, int>
tokensWithSameOwner(const std::unordered_map<int, Token> &tokens) {
  std::unordered_map<int, int> owners;
  for (auto token : tokens) {
    auto ret = owners.insert({token.second.owner(), token.first});
    if (ret.second == false) {
      return std::make_tuple(ret.first->second, token.first,
                             token.second.owner());
    }
  }
  return std::make_tuple(0, 0, 0);
}

class Bot {
public:
  void loDest(int d) { loDest_ = d; }
  void hiDest(int d) { hiDest_ = d; }
  boost::optional<int> loDest() const { return loDest_; }
  boost::optional<int> hiDest() const { return hiDest_; }
  void loToOutput(bool b) { loToOutput_ = b; }
  void hiToOutput(bool b) { hiToOutput_ = b; }
  bool loToOutput() const { return loToOutput_; }
  bool hiToOutput() const { return hiToOutput_; }
  void clearLo() {
    loDest_ = {};
    loToOutput_ = false;
  }
  void clearHi() {
    hiDest_ = {};
    hiToOutput_ = false;
  }

private:
  boost::optional<int> loDest_;
  boost::optional<int> hiDest_;
  bool loToOutput_ = false;
  bool hiToOutput_ = false;
};

void printLoHi(int lo, int hi, int loTokenVal, int hiTokenVal, int owner) {
  if (loTokenVal == lo && hiTokenVal == hi) {
    std::cout << "Comparison bot is: " << owner << std::endl;
  }
}

class Factory {
public:
  using TokenValue = int;
  using BotNumber = int;
  using TokenList = std::unordered_map<TokenValue, Token>;
  using BotList = std::unordered_map<BotNumber, Bot>;

  void processInstructions(const std::vector<std::string> &lines) {
    std::for_each(lines.begin(), lines.end(), [this](const auto &instruction) {
      prepareInstruction(instruction);
    });
  }

  void run(int lo, int hi) {
    bool isSwappable = true;
    while (isSwappable) {
      int t1, t2, owner;
      std::tie(t1, t2, owner) = tokensWithSameOwner(tokens_);
      auto loTokenVal = std::min(t1, t2);
      auto hiTokenVal = std::max(t1, t2);
      printLoHi(lo, hi, loTokenVal, hiTokenVal, owner);
      if (loTokenVal == hiTokenVal) {
        isSwappable = false;
        continue;
      }
      auto bot = bots_[owner];
      updateOwner(bot.loDest(), bot.loToOutput(), loTokenVal);
      updateOwner(bot.hiDest(), bot.hiToOutput(), hiTokenVal);
      bot.clearLo();
      bot.clearHi();
    }
  }

  int valueOfOutput(int output) {
    auto p =
        std::find_if(output_.begin(), output_.end(),
                     [output](const auto &o) { return o.first == output; });
    return p->second;
  }

private:
  void updateOwner(const boost::optional<int> &dest, bool destIsOutput,
                   int tokenVal) {
    if (!dest) {
      return;
    }
    if (destIsOutput) {
      output_.push_back(std::make_pair(dest.get(), tokenVal));
      tokens_.erase(tokenVal);
    } else {
      tokens_[tokenVal].owner(dest.get());
    }
  }

  void prepareInstruction(const std::string &instruction) {
    std::vector<std::string> split;
    boost::split(split, instruction, boost::is_any_of(" "));
    if (split[0] == "value"s) {
      createOwnedToken(split);
    } else if (split[0] == "bot"s) {
      createBotRules(split);
    }
  }

  void createOwnedToken(const std::vector<std::string> &split) {
    constexpr int tokenValIndex = 1;
    constexpr int tokenOwnerIndex = 5;
    tokens_.emplace(std::make_pair(
        boost::lexical_cast<int>(split[tokenValIndex]),
        Token(boost::lexical_cast<int>(split[tokenOwnerIndex]))));
  }

  void createBotRules(const std::vector<std::string> &split) {
    constexpr int botNumIndex = 1;
    constexpr int loDestTypeIndex = 5;
    constexpr int loDestIndex = 6;
    constexpr int hiDestTypeIndex = 10;
    constexpr int hiDestIndex = 11;

    auto botNum = boost::lexical_cast<int>(split[botNumIndex]);
    bots_[botNum].loDest(boost::lexical_cast<int>(split[loDestIndex]));
    bots_[botNum].loToOutput(split[loDestTypeIndex] == "output"s);

    bots_[botNum].hiDest(boost::lexical_cast<int>(split[hiDestIndex]));
    bots_[botNum].hiToOutput(split[hiDestTypeIndex] == "output"s);
  }

  TokenList tokens_;
  BotList bots_;
  std::vector<std::pair<int, int>> output_;
};

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cerr << "Provide input file on command line." << std::endl;
    return 1;
  }
  auto lines = readFile(argv[1]);
  auto f = Factory();
  f.processInstructions(lines);
  f.run(17, 61);
  std::cout << "output 0: " << f.valueOfOutput(0) << "\n";
  std::cout << "output 1: " << f.valueOfOutput(1) << "\n";
  std::cout << "output 2: " << f.valueOfOutput(2) << "\n";
  return 0;
}
