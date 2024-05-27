#pragma once

#include "chaos_bag.h"
#include "player.h"

#include <memory>
#include <vector>

class Game
{
public:
  Game& Instance()
  {
    static Game instance;
    return instance;
  }

  void LoadScenario()
  {
    FillChaosBag();
  }

  void AddPlayer(const std::unique_ptr<Player> player)
  {
    mPlayers.push_back(std::move(player));
  }

protected:
  void FillChaosBag()
  {
    // Hardcoded with first scenario
    mChaosBag.AddToken({ChaosToken::Token::kSkull, 0});
    mChaosBag.AddToken({ChaosToken::Token::kSkull, 0});
    mChaosBag.AddToken({ChaosToken::Token::kCultist, -1});
    mChaosBag.AddToken({ChaosToken::Token::kElderSign, 1});
    mChaosBag.AddToken({ChaosToken::Token::kTable, -2});
    mChaosBag.AddToken({ChaosToken::Token::kValue, 1});
    mChaosBag.AddToken({ChaosToken::Token::kValue, 0});
    mChaosBag.AddToken({ChaosToken::Token::kValue, 0});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -1});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -1});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -1});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -2});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -2});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -3});
    mChaosBag.AddToken({ChaosToken::Token::kValue, -4});
    mChaosBag.AddToken({ChaosToken::Token::kAutoFail, 0});
  }

  Game() noexcept = default;

  ~Game() noexcept = default;

  std::vector<std::unique_ptr<Player>> mPlayers;
  ChaosBagImpl mChaosBag;
};