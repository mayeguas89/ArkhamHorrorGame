#pragma once

#include "skill.h"

#include <tuple>

// Base class for all Effects
class Effect
{
public:
  Effect() noexcept = default;
  Effect(const Skill& skill, const uint8_t activationCost): mSkill{skill}, mActivationCost{activationCost} {}
  virtual ~Effect() = default;

  const Skill& GetSkill() const
  {
    return mSkill;
  }

  friend bool operator==(const Effect& rhs, const Effect& lhs);

protected:
  Skill mSkill; // Skill which the effect changes
  uint8_t mActivationCost{0};
};

static inline bool operator==(const Effect& lhs, const Effect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost) == std::tie(rhs.mSkill, rhs.mActivationCost);
}

// Base class for FightEffect
class FightEffect: public Effect
{
public:
  FightEffect() noexcept = default;
  FightEffect(const Skill& skill, const uint8_t activationCost, const uint8_t additionalDamage):
    Effect{skill, activationCost},
    mAdditionalDamage{additionalDamage}
  {}

  uint8_t GetAdditionalDamage() const
  {
    return mAdditionalDamage;
  }
  friend bool operator==(const FightEffect& rhs, const FightEffect& lhs);

protected:
  uint8_t mAdditionalDamage{0};
};

static inline bool operator==(const FightEffect& lhs, const FightEffect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAdditionalDamage)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAdditionalDamage);
}

// Base class for OptionalEffect
class OptionalEffect: public Effect
{
public:
  OptionalEffect() noexcept = default;
  OptionalEffect(const Skill& skill, const uint8_t activationCost, const Skill& optionalSkill):
    Effect{skill, activationCost},
    mOptionalSkill{optionalSkill}
  {}

  const Skill& GetOptionalSkill() const
  {
    return mOptionalSkill;
  }

  friend bool operator==(const OptionalEffect& rhs, const OptionalEffect& lhs);

protected:
  Skill mOptionalSkill;
};

static inline bool operator==(const OptionalEffect& lhs, const OptionalEffect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mOptionalSkill)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mOptionalSkill);
}

enum class LocationOptionalEffectCondition : uint8_t
{
  kUndiscoveredClues = 0,
  kInvalid
};

// Base class for LocationOptionalEffect
class LocationOptionalEffect: public OptionalEffect
{
public:
  LocationOptionalEffect() noexcept = default;
  LocationOptionalEffect(const Skill& skill,
                         const uint8_t activationCost,
                         const Skill& optionalSkill,
                         const LocationOptionalEffectCondition condition):
    OptionalEffect{skill, activationCost, optionalSkill},
    mCondition{condition}
  {}

  LocationOptionalEffectCondition GetCondition() const
  {
    return mCondition;
  }
  friend bool operator==(const LocationOptionalEffect& rhs, const LocationOptionalEffect& lhs);

private:
  LocationOptionalEffectCondition mCondition{LocationOptionalEffectCondition::kInvalid};
};

static inline bool operator==(const LocationOptionalEffect& lhs, const LocationOptionalEffect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mOptionalSkill, lhs.mCondition)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mOptionalSkill, rhs.mCondition);
}
class LocationOptionalFightEffect: public FightEffect
{
public:
  LocationOptionalFightEffect(const Skill& skill,
                              const uint8_t activationCost,
                              const uint8_t additionalDamage,
                              const LocationOptionalEffect& locationOptionalEffect):
    FightEffect{skill, activationCost, additionalDamage},
    mLocationOptionalEffect{locationOptionalEffect}
  {}

  const LocationOptionalEffect& GetLocationOptionalEffect() const
  {
    return mLocationOptionalEffect;
  }
  friend bool operator==(const LocationOptionalFightEffect& rhs, const LocationOptionalFightEffect& lhs);

private:
  LocationOptionalEffect mLocationOptionalEffect;
};
static inline bool operator==(const LocationOptionalFightEffect& lhs, const LocationOptionalFightEffect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mLocationOptionalEffect)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mLocationOptionalEffect);
}