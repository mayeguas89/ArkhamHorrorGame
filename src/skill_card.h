#pragma once

#include "card.h"

class SkillCard: public Card
{
public:
  SkillCard(const std::string& name, const Faction faction, const Skill& skill): Card{name, faction, skill} {}

  friend bool operator==(const SkillCard& rhs, const SkillCard& lhs);

private:
  friend struct fmt::formatter<SkillCard>;
};

static inline bool operator==(const SkillCard& rhs, const SkillCard& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mTestSkill) == std::tie(lhs.mName, lhs.mFaction, lhs.mTestSkill);
}

template<>
struct fmt::formatter<SkillCard>: fmt::formatter<std::string>
{
  auto format(const SkillCard& skillCard, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(skillCard));
    return it;
  }
};

inline void PrintTo(const SkillCard& skillCard, ::std::ostream* os)
{
  *os << fmt::format("{}", skillCard);
}