#include "card.h"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

void ReadInvestigatorsObject(const nlohmann::json& object,
                             std::unordered_map<std::string, CardDB::InvestigatorCardDBData>& investigators)
{
  for (const auto& element: object)
  {
    CardDB::InvestigatorCardDBData investigator;
    investigator.name = element.at("name");
    investigator.faction = element.at("faction");
    investigator.traits = element.at("traits");
    investigator.subname = element.at("subname");
    investigator.skill_willpower = element.at("skill_willpower");
    investigator.skill_intellect = element.at("skill_intellect");
    investigator.skill_combat = element.at("skill_combat");
    investigator.skill_agility = element.at("skill_agility");
    investigator.health = element.at("health");
    investigator.sanity = element.at("sanity");
    investigators.insert({investigator.name, investigator});
  }
}

void CardDB::ReadFile()
{
  using json = nlohmann::json;
  std::fstream cardsFile{kCardFilePath};
  json cardsData = json::parse(cardsFile);
  for (const auto& [key, val]: cardsData.items())
  {
    if (key == kInvestigatorsKey)
    {
      ReadInvestigatorsObject(val, mInvestigators);
    }
  }
}