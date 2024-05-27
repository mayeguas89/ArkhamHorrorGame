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
};

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

  friend bool operator==(const Investigator& rhs, const Investigator& lhs);

private:
  std::shared_ptr<Effect> mElderSign;
  Skill mSkill;
  uint8_t mHealth;
  uint8_t mSanity;
};

static inline bool operator==(const Investigator& rhs, const Investigator& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill, rhs.mHealth, rhs.mSanity)
         == std::tie(lhs.mName, lhs.mFaction, lhs.mSkill, lhs.mHealth, lhs.mSanity);
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
};

static inline bool operator==(const Asset& rhs, const Asset& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill, rhs.mCost, rhs.mSlot, rhs.mUses)
           == std::tie(lhs.mName, lhs.mFaction, lhs.mSkill, lhs.mCost, rhs.mSlot, rhs.mUses)
         && CompareVectorOfSharedPtr(lhs.mActivationEffects, rhs.mActivationEffects)
         && CompareVectorOfSharedPtr(lhs.mTriggerEffects, rhs.mTriggerEffects);
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
    uint8_t effect{0};
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
            std::shared_ptr<Effect> effect = std::make_shared<LocationOptionalFightEffect>(
              mainSkill,
              action.expend.value_or(0),
              action.effect,
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
              std::make_shared<FightEffect>(mainSkill, action.expend.value_or(0), action.effect);
            register_function(effect);
          }
        }
        else if (action.action == "skill_test")
        {
          auto effect = std::make_shared<Effect>(mainSkill, action.expend.value_or(0));
          register_function(effect);
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
