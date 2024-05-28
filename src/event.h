#pragma once

#include "card.h"

class Event: public Card
{
public:
  Event(const std::string& name, const Faction faction, const Skill& skill, const uint8_t cost, const Slot slot):
    Card{name, faction, skill},
    mCost{cost},
    mSlot{slot}
  {}
  friend bool operator==(const Event& rhs, const Event& lhs);

private:
  uint8_t mCost; // Recursos para jugarla
  Slot mSlot;    // Espacio que ocupa

  friend struct fmt::formatter<Event>;
};

static inline bool operator==(const Event& rhs, const Event& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mTestSkill, rhs.mCost, rhs.mSlot)
           == std::tie(lhs.mName, lhs.mFaction, lhs.mTestSkill, lhs.mCost, lhs.mSlot)
         && CompareVectorOfSharedPtr(lhs.mActivationEffects, rhs.mActivationEffects)
         && CompareVectorOfSharedPtr(lhs.mTriggerEffects, rhs.mTriggerEffects);
}

template<>
struct fmt::formatter<Event>: fmt::formatter<std::string>
{
  auto format(const Event& event, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(event));
    it = fmt::format_to(it, "{{'cost': {}, 'slot': {}}}", event.mCost, StringToEnum::GetString<Slot>(event.mSlot));
    return it;
  }
};

inline void PrintTo(const Event& event, ::std::ostream* os)
{
  *os << fmt::format("{}", event);
}
