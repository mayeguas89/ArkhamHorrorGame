#pragma once

#include "skill.h"
#include "spdlog/spdlog.h"

#include <memory>
#include <ostream>
#include <string>

class Investigator
{
public:
  Investigator(const std::string& name,
               const Faction faction,
               const Skill& skill,
               const uint8_t health,
               const uint8_t sanity):
    mName{name},
    mFaction{faction},
    mSkill{skill},
    mHealth{health},
    mSanity{sanity},
    mElderSign{nullptr}
  {}

  void RegisterElderSign(const std::shared_ptr<Effect>& effect)
  {
    mElderSign = std::move(effect);
  }

  const Skill& GetSkill() const
  {
    return mSkill;
  }

  uint8_t GetHealth() const
  {
    return mHealth;
  }

  uint8_t GetSanity() const
  {
    return mSanity;
  }

private:
  std::string mName;
  std::string mTraits;
  Faction mFaction;
  std::shared_ptr<Effect> mElderSign;
  Skill mSkill;
  uint8_t mHealth;
  uint8_t mSanity;

  friend struct fmt::formatter<Investigator>;
  friend bool operator==(const Investigator& rhs, const Investigator& lhs);
};

static inline bool operator==(const Investigator& rhs, const Investigator& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill, rhs.mHealth, rhs.mSanity)
         == std::tie(lhs.mName, lhs.mFaction, lhs.mSkill, lhs.mHealth, lhs.mSanity);
}
template<>
struct fmt::formatter<Investigator>: fmt::formatter<std::string>
{
  auto format(const Investigator& investigator, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(),
                             "'name': {}, 'traits': {}, 'faction': {}",
                             investigator.mName,
                             investigator.mTraits,
                             StringToEnum::GetString<Faction>(investigator.mFaction));
    return fmt::format_to(it,
                          "{{'skill': {}, 'health': {}, 'sanity': {}}}",
                          investigator.mSkill,
                          investigator.mHealth,
                          investigator.mSanity);
  }
};

inline void PrintTo(const Investigator& investigator, ::std::ostream* os)
{
  *os << fmt::format("{}", investigator);
}