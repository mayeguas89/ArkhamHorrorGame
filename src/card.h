#pragma once

#include "effect.h"
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

// SLOTS
enum class Slot : uint8_t
{
  kAccesory,
  kBody,
  kAlly,
  kHand,
  kArcane,
  kInvalid
};
struct Skill
{
  uint8_t willpower = 0;
  uint8_t intellect = 0;
  uint8_t combat = 0;
  uint8_t agility = 0;
  uint8_t wild = 0;
};

class Card
{
public:
  Card(const std::string& name, const Faction faction): mName{name}, mFaction{faction} {}

protected:
  std::string mName;
  std::string mTraits;
  Faction mFaction;

private:
};

static inline bool operator==(const Skill& rhs, const Skill& lhs)
{
  return std::tie(rhs.willpower, rhs.intellect, rhs.combat, rhs.agility, rhs.wild)
         == std::tie(lhs.willpower, lhs.intellect, lhs.combat, lhs.agility, lhs.wild);
}

class Investigator: public Card
{
public:
  Investigator(const std::string& name,
               const Faction faction,
               const uint8_t willpower,
               const uint8_t intellect,
               const uint8_t combat,
               const uint8_t agility,
               const uint8_t health,
               const uint8_t sanity):
    Card{name, faction},
    mSkill{.willpower = willpower, .intellect = intellect, .combat = combat, .agility = agility},
    mHealth{health},
    mSanity{sanity}
  {}

  void RegisterElderSign(const Effect& effect) {}

  friend bool operator==(const Investigator& rhs, const Investigator& lhs);

private:
  Effect mElderSign;
  Skill mSkill;
  uint8_t mHealth;
  uint8_t mSanity;
};

static inline bool operator==(const Investigator& rhs, const Investigator& lhs)
{
  return std::tie(rhs.mName, rhs.mFaction, rhs.mSkill, rhs.mHealth, rhs.mSanity)
         == std::tie(lhs.mName, lhs.mFaction, rhs.mSkill, lhs.mHealth, lhs.mSanity);
}

class Asset: public Card
{
public:
  Asset(const std::string& name, const Faction faction, const uint8_t cost, const Skill& skill):
    Card{name, faction},
    mCost{cost},
    mSkill{skill}
  {}

private:
  Skill mSkill;
  uint8_t mCost;
};

class CardDB
{
public:
  static inline const std::string kCardFilePath = "data/cards.json";

  struct InvestigatorCardDBData
  {
    std::string name;
    std::string faction;
    std::string traits;
    std::string subname;
    uint8_t skill_willpower;
    uint8_t skill_intellect;
    uint8_t skill_combat;
    uint8_t skill_agility;
    uint8_t health;
    uint8_t sanity;
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
                                                       dbData.skill_willpower,
                                                       dbData.skill_intellect,
                                                       dbData.skill_combat,
                                                       dbData.skill_agility,
                                                       dbData.health,
                                                       dbData.sanity);
    return investigator;
  }

private:
  CardDB()
  {
    ReadFile();
  }

  ~CardDB() noexcept = default;

  static inline const std::string kInvestigatorsKey{"investigators"};

  void ReadFile();

  std::unordered_map<std::string, InvestigatorCardDBData> mInvestigators;
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
