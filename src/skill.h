#pragma once

#include "spdlog/spdlog.h"

#include <cstdint>
#include <tuple>

struct Skill
{
  Skill() = default;
  Skill& operator=(const Skill& other) = default;
  Skill(const Skill& other) = default;
  Skill(const int8_t pWillpower,
        const int8_t pIntellect,
        const int8_t pCombat,
        const int8_t pAgility,
        const int8_t pWild = 0):
    willpower{pWillpower},
    intellect{pIntellect},
    combat{pCombat},
    agility{pAgility},
    wild{pWild}
  {}
  int8_t willpower = 0;
  int8_t intellect = 0;
  int8_t combat = 0;
  int8_t agility = 0;
  int8_t wild = 0;
};

static inline bool operator==(const Skill& lhs, const Skill& rhs)
{
  return std::tie(lhs.willpower, lhs.intellect, lhs.combat, lhs.agility, lhs.wild)
         == std::tie(rhs.willpower, rhs.intellect, rhs.combat, rhs.agility, rhs.wild);
}

template<>
struct fmt::formatter<Skill>: fmt::formatter<std::string>
{
  auto format(const Skill& skill, format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(),
                          "{{'agility' {}, 'combat': {}, 'intellect': {}, 'willpower': {}, 'wild': {}}}",
                          skill.agility,
                          skill.combat,
                          skill.intellect,
                          skill.willpower,
                          skill.wild);
  }
};

struct Willpower
{
  static inline int8_t GetSkillValue(const Skill& skill)
  {
    return skill.willpower + skill.wild;
  };
};

struct Combat
{
  static inline int8_t GetSkillValue(const Skill& skill)
  {
    return skill.combat + skill.wild;
  };
};

struct Intellect
{
  static inline int8_t GetSkillValue(const Skill& skill)
  {
    return skill.intellect + skill.wild;
  };
};

struct Agility
{
  static inline int8_t GetSkillValue(const Skill& skill)
  {
    return skill.agility + skill.wild;
  };
};