#pragma once

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