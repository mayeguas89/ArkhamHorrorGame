#pragma once

#include "card.h"

class Asset: public Card
{
public:
  Asset(const std::string& name,
        const Faction faction,
        const Skill& skill,
        const uint8_t cost,
        const Slot slot,
        std::optional<uint8_t> uses = std::nullopt,
        std::optional<uint8_t> health = std::nullopt,
        std::optional<uint8_t> sanity = std::nullopt):
    Card{name, faction, skill},
    mCost{cost},
    mSlot{slot},
    mUses{uses},
    mHealth{health},
    mSanity{sanity}
  {}
  friend bool operator==(const Asset& rhs, const Asset& lhs);

private:
  uint8_t mCost;                // Recursos para jugarla
  Slot mSlot;                   // Espacio que ocupa
  std::optional<uint8_t> mUses; // Usos de la carta
  std::optional<uint8_t> mHealth;
  std::optional<uint8_t> mSanity;

  friend struct fmt::formatter<Asset>;
};

static inline bool operator==(const Asset& rhs, const Asset& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mTestSkill, rhs.mCost, rhs.mSlot)
           == std::tie(lhs.mName, lhs.mFaction, lhs.mTestSkill, lhs.mCost, lhs.mSlot)
         && lhs.mUses.value_or(0) == rhs.mUses.value_or(0) && lhs.mSanity.value_or(0) == rhs.mSanity.value_or(0)
         && lhs.mHealth.value_or(0) == rhs.mHealth.value_or(0)
         && CompareVectorOfSharedPtr(lhs.mEffects.at(EffectType::kActivate),
                                     rhs.mEffects.at(EffectType::kActivate))
         && CompareVectorOfSharedPtr(lhs.mEffects.at(EffectType::kTrigger), rhs.mEffects.at(EffectType::kTrigger))
         && CompareVectorOfSharedPtr(lhs.mEffects.at(EffectType::kDiscard), rhs.mEffects.at(EffectType::kDiscard))
         && CompareVectorOfSharedPtr(lhs.mEffects.at(EffectType::kPasive), rhs.mEffects.at(EffectType::kPasive));
}

template<>
struct fmt::formatter<Asset>: fmt::formatter<std::string>
{
  auto format(const Asset& asset, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(asset));
    it = fmt::format_to(it,
                        "{{'cost': {}, 'slot': {}, 'uses': {}, 'health': {}, 'sanity': {}}}",
                        asset.mCost,
                        asset.mSlot == Slot::kInvalid ? "invalid" : StringToEnum::GetString<Slot>(asset.mSlot),
                        asset.mUses.value_or(0),
                        asset.mHealth.value_or(0),
                        asset.mSanity.value_or(0));
    return it;
  }
};

inline void PrintTo(const Asset& asset, ::std::ostream* os)
{
  *os << fmt::format("{}", asset);
}
