#pragma once

#include <cstdint>
#include <tuple>
struct Skill
{
  Skill() = default;
  Skill& operator=(const Skill& other) = default;
  Skill(const Skill& other) = default;
  Skill(const uint8_t pWillpower,
        const uint8_t pIntellect,
        const uint8_t pCombat,
        const uint8_t pAgility,
        const uint8_t pWild = 0):
    willpower{pWillpower},
    intellect{pIntellect},
    combat{pCombat},
    agility{pAgility},
    wild{pWild}
  {}
  uint8_t willpower = 0;
  uint8_t intellect = 0;
  uint8_t combat = 0;
  uint8_t agility = 0;
  uint8_t wild = 0;
};

static inline bool operator==(const Skill& lhs, const Skill& rhs)
{
  return std::tie(lhs.willpower, lhs.intellect, lhs.combat, lhs.agility, lhs.wild)
         == std::tie(rhs.willpower, rhs.intellect, rhs.combat, rhs.agility, rhs.wild);
}