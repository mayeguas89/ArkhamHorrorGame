#pragma once

#include "chaos_token.h"

#include <random>
#include <vector>

class ChaosBag
{
public:
  virtual ~ChaosBag() = default;
  virtual void AddToken(const ChaosToken& token) = 0;
  virtual ChaosToken GetToken() = 0;
};

class ChaosBagImpl: public ChaosBag
{
public:
  ChaosBagImpl()
  {
    std::random_device rd;
    mGenerator = std::mt19937(rd());
  }

  virtual void AddToken(const ChaosToken& token) override
  {
    mTokens.push_back(token);
  }

  virtual ChaosToken GetToken() override
  {
    std::shuffle(mTokens.begin(), mTokens.end(), mGenerator);
    std::uniform_int_distribution<size_t> dist(0, mTokens.size());
    size_t r = dist(mGenerator);
    return mTokens.at(r);
  }

private:
  std::vector<ChaosToken> mTokens;
  std::mt19937 mGenerator;
};