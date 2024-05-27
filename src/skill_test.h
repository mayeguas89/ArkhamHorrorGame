#pragma once

#include "chaos_bag.h"

#include <tuple>

template<typename T>
struct SkillTest
{
  std::pair<bool, int8_t> operator()(const Skill& skillTested,
                                     const Player& player,
                                     ChaosBag& chaosBag,
                                     const std::vector<std::shared_ptr<Card>>& commitedCards)
  {
    auto [token, effect] = chaosBag.GetToken();

    // Check autofail
    if (token == ChaosToken::Token::kAutoFail)
      return {false, 0};

    // Get result from the skill tested from the players and from the commited cards
    auto result = T::GetSkillValue(player.GetSkill());
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += T::GetSkillValue(card->GetSkill()); });
    auto comparisonValue = T::GetSkillValue(skillTested) < 0 ? 0 : T::GetSkillValue(skillTested);

    // Add the effect of the token
    result += effect;

    // Compare agains the skill
    return {result >= comparisonValue, result - comparisonValue};
  }
};
