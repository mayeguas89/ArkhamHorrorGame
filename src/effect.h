#pragma once

#include "condition.h"
#include "skill.h"
#include "spdlog/spdlog.h"

#include <tuple>

enum class EffectType
{
  kActivate,
  kTrigger,
  kPasive,
  kDiscard,
  kInvalid
};

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

protected:
  Skill mSkill; // Skill which the effect changes
  uint8_t mActivationCost{0};
  friend bool operator==(const Effect& rhs, const Effect& lhs);
  friend struct fmt::formatter<Effect>;
};

static inline bool operator==(const Effect& lhs, const Effect& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost) == std::tie(rhs.mSkill, rhs.mActivationCost);
}

template<>
struct fmt::formatter<Effect>: fmt::formatter<std::string>
{
  auto format(const Effect& effect, format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(),
                          "{{'skill' {}, 'activationCost': {}}}",
                          effect.mSkill,
                          effect.mActivationCost);
  }
};

inline void PrintTo(const Effect& effect, std::ostream* os)
{
  *os << fmt::format("{}", effect);
}

// Base class for all Effects
class EffectWithCondition: public Effect
{
public:
  EffectWithCondition() noexcept = default;
  EffectWithCondition(const Skill& skill, const uint8_t activationCost, std::shared_ptr<Condition> condition):
    Effect{skill, activationCost},
    mCondition{condition}
  {}

protected:
  std::shared_ptr<Condition> mCondition;
  friend bool operator==(const EffectWithCondition& rhs, const EffectWithCondition& lhs);
};

static inline bool operator==(const EffectWithCondition& lhs, const EffectWithCondition& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mCondition)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mCondition);
}

// Base class for FightEffect
class FightEffectWithAdditionalDamage: public Effect
{
public:
  FightEffectWithAdditionalDamage() noexcept = default;
  FightEffectWithAdditionalDamage(const Skill& skill,
                                  const uint8_t activationCost,
                                  const uint8_t additionalDamage):
    Effect{skill, activationCost},
    mAdditionalDamage{additionalDamage}
  {}

  uint8_t GetAdditionalDamage() const
  {
    return mAdditionalDamage;
  }
  friend bool operator==(const FightEffectWithAdditionalDamage& rhs, const FightEffectWithAdditionalDamage& lhs);

protected:
  uint8_t mAdditionalDamage{0};
};

static inline bool operator==(const FightEffectWithAdditionalDamage& lhs,
                              const FightEffectWithAdditionalDamage& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAdditionalDamage)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAdditionalDamage);
}
// Base class for InvestigateEffectWithShroudModification
class InvestigateEffectWithShroudModification: public Effect
{
public:
  InvestigateEffectWithShroudModification() noexcept = default;
  InvestigateEffectWithShroudModification(const Skill& skill,
                                          const uint8_t activationCost,
                                          const int8_t shroudModification):
    Effect{skill, activationCost},
    mShroudModification{shroudModification}
  {}

  int8_t GetShroudModification() const
  {
    return mShroudModification;
  }
  friend bool operator==(const InvestigateEffectWithShroudModification& rhs,
                         const InvestigateEffectWithShroudModification& lhs);

protected:
  int8_t mShroudModification{0};
};

static inline bool operator==(const InvestigateEffectWithShroudModification& lhs,
                              const InvestigateEffectWithShroudModification& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mShroudModification)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mShroudModification);
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
class LocationOptionalFightEffectWithAdditionalDamage: public FightEffectWithAdditionalDamage
{
public:
  LocationOptionalFightEffectWithAdditionalDamage(const Skill& skill,
                                                  const uint8_t activationCost,
                                                  const uint8_t additionalDamage,
                                                  const LocationOptionalEffect& locationOptionalEffect):
    FightEffectWithAdditionalDamage{skill, activationCost, additionalDamage},
    mLocationOptionalEffect{locationOptionalEffect}
  {}

  const LocationOptionalEffect& GetLocationOptionalEffect() const
  {
    return mLocationOptionalEffect;
  }
  friend bool operator==(const LocationOptionalFightEffectWithAdditionalDamage& rhs,
                         const LocationOptionalFightEffectWithAdditionalDamage& lhs);

private:
  LocationOptionalEffect mLocationOptionalEffect;
};

static inline bool operator==(const LocationOptionalFightEffectWithAdditionalDamage& lhs,
                              const LocationOptionalFightEffectWithAdditionalDamage& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAdditionalDamage, lhs.mLocationOptionalEffect)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAdditionalDamage, rhs.mLocationOptionalEffect);
}

class MakeDamageAtTargetAtCurrentLocation: public Effect
{
public:
  MakeDamageAtTargetAtCurrentLocation(const Skill& skill, const uint8_t activationCost, uint8_t amount):
    Effect{skill, activationCost},
    mAmount{amount}
  {}

  uint8_t GetDamage() const
  {
    return mAmount;
  }

  friend bool operator==(const MakeDamageAtTargetAtCurrentLocation& rhs,
                         const MakeDamageAtTargetAtCurrentLocation& lhs);

private:
  uint8_t mAmount;
};

static inline bool operator==(const MakeDamageAtTargetAtCurrentLocation& lhs,
                              const MakeDamageAtTargetAtCurrentLocation& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAmount)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAmount);
}

class FightEffectWithAdditionalDamageWithCondition: public FightEffectWithAdditionalDamage
{
public:
  FightEffectWithAdditionalDamageWithCondition() noexcept = default;
  FightEffectWithAdditionalDamageWithCondition(const Skill& skill,
                                               const uint8_t activationCost,
                                               const uint8_t additionalDamage,
                                               const std::shared_ptr<Condition> condition):
    FightEffectWithAdditionalDamage{skill, activationCost, additionalDamage},
    mCondition{condition}
  {}

  friend bool operator==(const FightEffectWithAdditionalDamageWithCondition& rhs,
                         const FightEffectWithAdditionalDamageWithCondition& lhs);

protected:
  std::shared_ptr<Condition> mCondition;
};

static inline bool operator==(const FightEffectWithAdditionalDamageWithCondition& lhs,
                              const FightEffectWithAdditionalDamageWithCondition& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAdditionalDamage)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAdditionalDamage);
}

class MakeDamageToAttacker: public Effect
{
public:
  MakeDamageToAttacker(const Skill& skill, const uint8_t activationCost, uint8_t amount):
    Effect{skill, activationCost},
    mAmount{amount}
  {}

  uint8_t GetDamage() const
  {
    return mAmount;
  }

  friend bool operator==(const MakeDamageToAttacker& rhs, const MakeDamageToAttacker& lhs);

private:
  uint8_t mAmount;
};

static inline bool operator==(const MakeDamageToAttacker& lhs, const MakeDamageToAttacker& rhs)
{
  return std::tie(lhs.mSkill, lhs.mActivationCost, lhs.mAmount)
         == std::tie(rhs.mSkill, rhs.mActivationCost, rhs.mAmount);
}
