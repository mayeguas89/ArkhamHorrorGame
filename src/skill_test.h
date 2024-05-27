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
    auto [comparisonValue, result] = skill(skillTested, player, commitedCards);

    // Add wild commited cards
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += card->GetSkill().wild; });
    // Add the effect of the token
    result += effect;

    // Compare agains the skill
    return {result >= comparisonValue, result - comparisonValue};
  }

private:
  T skill;
};

struct Willpower
{
  std::pair<int8_t, int8_t> operator()(const Skill& skillTested,
                                       const Player& player,
                                       const std::vector<std::shared_ptr<Card>>& commitedCards)
  {
    auto result = player.GetSkill().willpower;
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += card->GetSkill().willpower; });

    return {skillTested.willpower < 0 ? 0 : skillTested.willpower, result};
  }
};

struct Combat
{
  std::pair<int8_t, int8_t> operator()(const Skill& skillTested,
                                       const Player& player,
                                       const std::vector<std::shared_ptr<Card>>& commitedCards)
  {
    auto result = player.GetSkill().combat;
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += card->GetSkill().combat; });
    return {skillTested.combat < 0 ? 0 : skillTested.combat, result};
  }
};

struct Intellect
{
  std::pair<int8_t, int8_t> operator()(const Skill& skillTested,
                                       const Player& player,
                                       const std::vector<std::shared_ptr<Card>>& commitedCards)
  {
    auto result = player.GetSkill().intellect;
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += card->GetSkill().intellect; });
    return {skillTested.intellect < 0 ? 0 : skillTested.intellect, result};
  }
};

struct Agility
{
  std::pair<int8_t, int8_t> operator()(const Skill& skillTested,
                                       const Player& player,
                                       const std::vector<std::shared_ptr<Card>>& commitedCards)
  {
    auto result = player.GetSkill().agility;
    std::for_each(commitedCards.begin(),
                  commitedCards.end(),
                  [&result](const auto& card) { result += card->GetSkill().agility; });
    return {skillTested.agility < 0 ? 0 : skillTested.agility, result};
  }
};