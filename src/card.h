#pragma once

#include "effect.h"
#include "helpers.h"
#include "skill.h"
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

enum class Faction : uint8_t
{
  kGuardian = 0,
  kSeeker,
  kRogue,
  kMystic,
  kSurvivor,
  kNeutral,
  kInvalid
};

enum class Slot : uint8_t
{
  kAccesory,
  kBody,
  kAlly,
  kHand,
  kArcane,
  kBothHands,
  kInvalid
};

enum class Action : uint8_t
{
  kAction,
  kInvalid
};

class Card
{
public:
  Card(const std::string& name, const Faction faction): mName{name}, mFaction{faction} {}
  virtual const Skill& GetSkill() const = 0;

protected:
  std::string mName;
  std::string mTraits;
  Faction mFaction;

private:
  friend struct fmt::formatter<Card>;
};
template<>
struct fmt::formatter<Card>: fmt::formatter<std::string>
{
  auto format(const Card& card, format_context& ctx) const -> decltype(ctx.out())
  {
    return fmt::format_to(ctx.out(),
                          "'name': {}, 'traits': {}, 'faction': {}",
                          card.mName,
                          card.mTraits,
                          StringToEnum::GetString<Faction>(card.mFaction));
  }
};

inline void PrintTo(const Card& card, ::std::ostream* os)
{
  *os << fmt::format("{}", card);
}

class Investigator: public Card
{
public:
  Investigator(const std::string& name,
               const Faction faction,
               const Skill& skill,
               const uint8_t health,
               const uint8_t sanity):
    Card{name, faction},
    mSkill{skill},
    mHealth{health},
    mSanity{sanity},
    mElderSign{nullptr}
  {}

  void RegisterElderSign(const std::shared_ptr<Effect>& effect)
  {
    mElderSign = std::move(effect);
  }

  const Skill& GetSkill() const override
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
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(investigator));
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

class Asset: public Card
{
public:
  Asset(const std::string& name,
        const Faction faction,
        const uint8_t cost,
        const Skill& skill,
        const Slot slot,
        std::optional<uint8_t> uses = std::nullopt):
    Card{name, faction},
    mCost{cost},
    mSkill{skill},
    mSlot{slot},
    mUses{uses}
  {}
  friend bool operator==(const Asset& rhs, const Asset& lhs);

  const Skill& GetSkill() const override
  {
    return mSkill;
  }

  void RegisterActivationEffect(const std::shared_ptr<Effect>& effect)
  {
    mActivationEffects.push_back(std::move(effect));
  }

  void RegisterTriggerEffect(const std::shared_ptr<Effect>& effect)
  {
    mTriggerEffects.push_back(std::move(effect));
  }

  const std::vector<std::shared_ptr<Effect>>& GetActivationEffect() const
  {
    return mActivationEffects;
  }

  const std::vector<std::shared_ptr<Effect>>& GetTriggerEffect() const
  {
    return mTriggerEffects;
  }

private:
  Skill mSkill;                 // Habilidades para skill tests
  uint8_t mCost;                // Recursos para jugarla
  Slot mSlot;                   // Espacio que ocupa
  std::optional<uint8_t> mUses; // Usos de la carta

  std::vector<std::shared_ptr<Effect>> mActivationEffects;
  std::vector<std::shared_ptr<Effect>> mTriggerEffects;

  friend struct fmt::formatter<Asset>;
};

static inline bool operator==(const Asset& rhs, const Asset& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill, rhs.mCost, rhs.mSlot, rhs.mUses)
           == std::tie(lhs.mName, lhs.mFaction, lhs.mSkill, lhs.mCost, rhs.mSlot, rhs.mUses)
         && CompareVectorOfSharedPtr(lhs.mActivationEffects, rhs.mActivationEffects)
         && CompareVectorOfSharedPtr(lhs.mTriggerEffects, rhs.mTriggerEffects);
}

template<>
struct fmt::formatter<Asset>: fmt::formatter<std::string>
{
  auto format(const Asset& asset, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(asset));
    it = fmt::format_to(it,
                        "{{'skill': {}, 'cost': {}, 'slot': {}, 'uses': {}, ",
                        asset.mSkill,
                        asset.mCost,
                        StringToEnum::GetString<Slot>(asset.mSlot),
                        asset.mUses.value_or(0));
    it = fmt::format_to(it, "{{'activationEffects':[");
    std::for_each(asset.mActivationEffects.begin(),
                  asset.mActivationEffects.end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}, {{'triggerEffects':[");
    std::for_each(asset.mTriggerEffects.begin(),
                  asset.mTriggerEffects.end(),
                  [&it](const auto& effect) { it = fmt::format_to(it, "{{{}}}, ", *effect); });
    it = fmt::format_to(it, "]}}");
    return it;
  }
};

inline void PrintTo(const Asset& asset, ::std::ostream* os)
{
  *os << fmt::format("{}", asset);
}

class Hability: public Card
{
public:
  Hability(const std::string& name, const Faction faction, const Skill& skill): Card{name, faction}, mSkill{skill}
  {}

  virtual const Skill& GetSkill() const override
  {
    return mSkill;
  }

  friend bool operator==(const Hability& rhs, const Hability& lhs);

private:
  Skill mSkill;
  friend struct fmt::formatter<Hability>;
};

static inline bool operator==(const Hability& rhs, const Hability& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill) == std::tie(lhs.mName, lhs.mFaction, lhs.mSkill);
}

template<>
struct fmt::formatter<Hability>: fmt::formatter<std::string>
{
  auto format(const Hability& hability, format_context& ctx) const -> decltype(ctx.out())
  {
    auto it = fmt::format_to(ctx.out(), "'card': {{{}}}\n", static_cast<const Card&>(hability));
    it = fmt::format_to(it, "{{'skill': {}}}", hability.mSkill);
    return it;
  }
};

inline void PrintTo(const Hability& hability, ::std::ostream* os)
{
  *os << fmt::format("{}", hability);
}

class CardDB
{
public:
  static inline const std::string kCardFilePath = "data/cards.json";

  struct CardDBData
  {
    std::string name;
    std::string faction;
    std::string traits;
  };

  struct InvestigatorCardDBData: public CardDBData
  {
    std::string subname;
    Skill skill;
    uint8_t health{0};
    uint8_t sanity{0};
  };

  struct ActionDBData
  {
    // Si gasta recursos asignados, si no tiene asignados entonces son resources del jugador
    std::optional<uint8_t> expend{std::nullopt};
    // Accion a la que afecta : lucha / skill_test
    std::string action;
    struct SkillEffectsDBData
    {
      Skill skill; // Valor que añade a las skills
      struct OptionalEffectDBData
      {
        Skill skill;           // Valor que añade a las skills
        std::string what;      // Que afecta: "lugar"
        std::string condition; // Condicion de que afecta: "undiscovered_clues"
      };
      // Modificacion de la skill si se cumple la condicion (estar en un lugar sin pistas descubiertas)
      std::optional<OptionalEffectDBData> optional_effect;
    };
    SkillEffectsDBData skill_effect;
    // Efecto extra sobre la accion: para la lucha puede ser +1 de daño
    std::unordered_map<std::string, int8_t> modifications;
  };

  struct AssetCardDBData: public CardDBData
  {
    uint8_t cost{0};                            // Coste de la carta
    Skill skill;                                // Habilidades para skill tests
    std::optional<std::string> slot;            // Espacio que ocupa si lo tiene
    std::optional<uint8_t> uses{std::nullopt};  // Usos si tiene
    std::vector<ActionDBData> activate_actions; // Efecto de activacion si tiene
    std::vector<ActionDBData> trigger_actions;  // Efecto de trigger si tiene
  };

  static inline CardDB& Instance()
  {
    static CardDB instance;
    return instance;
  }

  template<typename T>
  std::shared_ptr<T> GetCard(const std::string&)
  {
    return nullptr;
  }

  template<>
  std::shared_ptr<Investigator> GetCard(const std::string& name)
  {
    const auto& it = mInvestigators.find(name);
    if (it == mInvestigators.end())
      return nullptr;

    const auto& [_, dbData] = *it;
    auto investigator = std::make_shared<Investigator>(name,
                                                       StringToEnum::Get<Faction>(dbData.faction),
                                                       dbData.skill,
                                                       dbData.health,
                                                       dbData.sanity);
    return investigator;
  }

  template<>
  std::shared_ptr<Asset> GetCard(const std::string& name)
  {
    const auto& it = mAssets.find(name);
    if (it == mAssets.end())
      return nullptr;

    const auto& [_, dbData] = *it;

    auto asset = std::make_shared<Asset>(name,
                                         StringToEnum::Get<Faction>(dbData.faction),
                                         dbData.cost,
                                         dbData.skill,
                                         dbData.slot.has_value() ? StringToEnum::Get<Slot>(dbData.slot.value()) :
                                                                   Slot::kInvalid,
                                         dbData.uses);

    auto register_effect = [&](const std::vector<ActionDBData>& actions,
                               std::function<void(const std::shared_ptr<Effect>&)> register_function)
    {
      for (const auto& action: actions)
      {
        Skill mainSkill = action.skill_effect.skill;
        if (action.action == "fight")
        {
          if (action.skill_effect.optional_effect.has_value()
              && action.skill_effect.optional_effect->what == "location")
          {
            Skill optionalSkill = action.skill_effect.optional_effect->skill;
            std::shared_ptr<Effect> effect = std::make_shared<LocationOptionalFightEffectWithAdditionalDamage>(
              mainSkill,
              action.expend.value_or(0),
              action.modifications.find("damage")->second,
              LocationOptionalEffect{mainSkill,
                                     action.expend.value_or(0),
                                     optionalSkill,
                                     StringToEnum::Get<LocationOptionalEffectCondition>(
                                       action.skill_effect.optional_effect->condition)});
            register_function(effect);
          }
          // No optional effect
          else
          {
            std::shared_ptr<Effect> effect =
              std::make_shared<FightEffectWithAdditionalDamage>(mainSkill,
                                                                action.expend.value_or(0),
                                                                action.modifications.find("damage")->second);
            register_function(effect);
          }
        }
        else if (action.action == "skill_test")
        {
          auto effect = std::make_shared<Effect>(mainSkill, action.expend.value_or(0));
          register_function(effect);
        }
        else if (action.action == "investigate")
        {
          if (auto it = action.modifications.find("shroud"); it != action.modifications.end())
          {
            auto effect = std::make_shared<InvestigateEffectWithShroudModification>(mainSkill,
                                                                                    action.expend.value_or(0),
                                                                                    it->second);
            register_function(effect);
          }
        }
      }
    };

    register_effect(dbData.activate_actions,
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterActivationEffect(effect); });

    register_effect(dbData.trigger_actions,
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterTriggerEffect(effect); });

    return asset;
  }

private:
  CardDB()
  {
    ReadFile();
  }

  ~CardDB() noexcept = default;

  static inline const std::string kInvestigatorsKey{"investigators"};
  static inline const std::string kAssetsKey{"assets"};

  void ReadFile();

  std::unordered_map<std::string, InvestigatorCardDBData> mInvestigators;
  std::unordered_map<std::string, AssetCardDBData> mAssets;
};

class CardFactory
{
public:
  template<typename T>
  static inline std::shared_ptr<T> CreateCard(const std::string& name)
  {
    return std::move(CardDB::Instance().GetCard<T>(name));
  }

private:
};
