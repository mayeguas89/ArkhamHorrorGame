#pragma once
#include "effect.h"
#include "faction.h"
#include "helpers.h"
#include "skill.h"
#include "slot.h"
#include "spdlog/spdlog.h"
#include "string_to_enum.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Card
{
public:
  Card(const std::string& name, const Faction faction, const Skill& skill):
    mName{name},
    mFaction{faction},
    mTestSkill{skill}
  {
    mEffects.insert({EffectType::kActivate, {}});
    mEffects.insert({EffectType::kTrigger, {}});
    mEffects.insert({EffectType::kPasive, {}});
    mEffects.insert({EffectType::kDiscard, {}});
  }

  virtual const Skill& GetSkill() const
  {
    return mTestSkill;
  }

  void RegisterActivationEffect(const std::shared_ptr<Effect>& effect)
  {
    mEffects[EffectType::kActivate].push_back(std::move(effect));
  }

  void RegisterTriggerEffect(const std::shared_ptr<Effect>& effect)
  {
    mEffects[EffectType::kTrigger].push_back(std::move(effect));
  }

  void RegisterDiscardEffect(const std::shared_ptr<Effect>& effect)
  {
    mEffects[EffectType::kDiscard].push_back(std::move(effect));
  }

  void RegisterPasiveEffect(const std::shared_ptr<Effect>& effect)
  {
    mEffects[EffectType::kPasive].push_back(std::move(effect));
  }

  const std::vector<std::shared_ptr<Effect>>& GetActivationEffect() const
  {
    return mEffects.at(EffectType::kActivate);
  }

  const std::vector<std::shared_ptr<Effect>>& GetTriggerEffect() const
  {
    return mEffects.at(EffectType::kTrigger);
  }

protected:
  std::string mName;
  std::string mTraits;
  Faction mFaction;
  Skill mTestSkill;

  std::unordered_map<EffectType, std::vector<std::shared_ptr<Effect>>> mEffects;

private:
  friend struct fmt::formatter<Card>;
};
template<>
struct fmt::formatter<Card>: fmt::formatter<std::string>
{
  auto format(const Card& card, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(),
                             "'name': {}, 'traits': {}, 'faction': {}, 'test_skill': {}",
                             card.mName,
                             card.mTraits,
                             StringToEnum::GetString<Faction>(card.mFaction),
                             card.mTestSkill);
    it = fmt::format_to(it, "{{'activationEffects':[");
    std::for_each(card.mEffects.at(EffectType::kActivate).begin(),
                  card.mEffects.at(EffectType::kActivate).end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}, {{'triggerEffects':[");
    std::for_each(card.mEffects.at(EffectType::kTrigger).begin(),
                  card.mEffects.at(EffectType::kTrigger).end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}, {{'passiveEffects':[");
    std::for_each(card.mEffects.at(EffectType::kPasive).begin(),
                  card.mEffects.at(EffectType::kPasive).end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}, {{'discardEffects':[");
    std::for_each(card.mEffects.at(EffectType::kDiscard).begin(),
                  card.mEffects.at(EffectType::kDiscard).end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}");
    return it;
  }
};

inline void PrintTo(const Card& card, ::std::ostream* os)
{
  *os << fmt::format("{}", card);
}
