#pragma once

#include "effect.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Card
{
public:
  enum class Faction : uint8_t
  {
    kGuardian = 0,
    kSeeker,
    kRogue,
    kMystic,
    kSurvivor,
    kNeutral
  };

  Card(const std::string& name, const Faction faction): mName{name}, mFaction{faction} {}

protected:
  std::string mName;
  std::string mTraits;
  Faction mFaction;

private:
};

static inline constexpr uint8_t kFactionNumber = 6;

static inline const std::array<std::string, static_cast<size_t>(kFactionNumber)> kFactionNames =
  {"guardian", "seeker", "rogue", "mystic", "survivor", "neutral"};

Card::Faction StringToFaction(const std::string& faction);

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
    mWillpower{willpower},
    mIntellect{intellect},
    mCombat{combat},
    mAgility{agility},
    mHealth{health},
    mSanity{sanity}
  {}

  void RegisterElderSign(const Effect& effect) {}

  friend bool operator==(const Investigator& rhs, const Investigator& lhs);

private:
  Effect mElderSign;
  uint8_t mWillpower;
  uint8_t mIntellect;
  uint8_t mCombat;
  uint8_t mAgility;
  uint8_t mHealth;
  uint8_t mSanity;
};

static inline bool operator==(const Investigator& rhs, const Investigator& lhs)
{
  return std::tie(rhs.mName,
                  rhs.mFaction,
                  rhs.mWillpower,
                  rhs.mIntellect,
                  rhs.mCombat,
                  rhs.mAgility,
                  rhs.mAgility,
                  rhs.mHealth,
                  rhs.mSanity)
         == std::tie(lhs.mName,
                     lhs.mFaction,
                     lhs.mWillpower,
                     lhs.mIntellect,
                     lhs.mCombat,
                     lhs.mAgility,
                     lhs.mAgility,
                     lhs.mHealth,
                     lhs.mSanity);
}

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
                                                       StringToFaction(dbData.faction),
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
