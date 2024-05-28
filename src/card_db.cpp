#include "card_db.h"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

Skill ReadSkillObject(const nlohmann::json& object)
{
  Skill skill;
  if (auto it = object.find("combat"); it != object.end())
  {
    skill.combat = it.value();
  }
  if (auto it = object.find("agility"); it != object.end())
  {
    skill.agility = it.value();
  }
  if (auto it = object.find("willpower"); it != object.end())
  {
    skill.willpower = it.value();
  }
  if (auto it = object.find("intellect"); it != object.end())
  {
    skill.intellect = it.value();
  }
  if (auto it = object.find("wild"); it != object.end())
  {
    skill.wild = it.value();
  }
  return skill;
}

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
    investigator.skill.willpower = element.at("skill_willpower");
    investigator.skill.intellect = element.at("skill_intellect");
    investigator.skill.combat = element.at("skill_combat");
    investigator.skill.agility = element.at("skill_agility");
    investigator.health = element.at("health");
    investigator.sanity = element.at("sanity");
    investigators.insert({investigator.name, investigator});
  }
}

void ReadActionDBData(const nlohmann::json& elements, std::vector<CardDB::ActionDBData>& actions)
{
  for (const auto& sub_element: elements)
  {
    CardDB::ActionDBData activate_action;
    const auto activate_element = sub_element;
    if (auto activate_it = activate_element.find("expend"); activate_it != activate_element.end())
      activate_action.expend = activate_it.value();
    if (auto activate_it = activate_element.find("action"); activate_it != activate_element.end())
      activate_action.action = activate_it.value();

    if (auto activate_it = activate_element.find("skills"); activate_it != activate_element.end())
    {
      const auto skills_element = activate_it.value();
      if (auto skills_it = skills_element.find("combat"); skills_it != skills_element.end())
        activate_action.skill_effect.skill.combat = skills_it.value();
      if (auto skills_it = skills_element.find("willpower"); skills_it != skills_element.end())
        activate_action.skill_effect.skill.willpower = skills_it.value();
      if (auto skills_it = skills_element.find("optional"); skills_it != skills_element.end())
      {
        CardDB::ActionDBData::SkillEffectsDBData::OptionalEffectDBData optional_effect;
        auto optional_element = skills_it.value();
        if (auto optional_it = optional_element.find("combat"); optional_it != optional_element.end())
          optional_effect.skill.combat = optional_it.value();
        if (auto optional_it = optional_element.find("location"); optional_it != optional_element.end())
        {
          optional_effect.what = optional_it.key();
          optional_effect.condition = optional_it.value();
        }
        activate_action.skill_effect.optional_effect = optional_effect;
      }
    }

    if (auto activate_it = activate_element.find("modifications"); activate_it != activate_element.end())
    {
      const auto effect_array = activate_it.value();
      for (const auto& effect_element: effect_array)
      {
        for (const auto [key, value]: effect_element.items())
        {
          if (key == "condition")
            activate_action.modifications.insert({key, static_cast<std::string>(value)});
          else
            activate_action.modifications.insert({key, static_cast<int8_t>(value)});
        }
      }
    }

    if (auto action_it = activate_element.find("target"); action_it != activate_element.end())
    {
      activate_action.target = action_it.value();
    }

    actions.push_back(activate_action);
  }
}

void ReadAssetObject(const nlohmann::json& object,
                     std::unordered_map<std::string, CardDB::AssetCardDBData>& assets)
{
  for (const auto& element: object)
  {
    CardDB::AssetCardDBData asset;
    asset.name = element.at("name");
    asset.faction = element.at("faction");
    asset.traits = element.at("traits");
    asset.cost = element.at("cost");
    asset.skill = ReadSkillObject(element.at("test_skill"));
    if (auto it = element.find("slot"); it != element.end())
      asset.slot = element.at("slot");
    if (auto it = element.find("uses"); it != element.end())
      asset.uses = element.at("uses");
    if (auto it = element.find("health"); it != element.end())
      asset.health = element.at("health");
    if (auto it = element.find("sanity"); it != element.end())
      asset.sanity = element.at("sanity");

    if (const auto it = element.find("activate"); it != element.end())
    {
      ReadActionDBData(it.value(), asset.effects.at(EffectType::kActivate));
    }

    if (const auto it = element.find("trigger"); it != element.end())
    {
      ReadActionDBData(it.value(), asset.effects.at(EffectType::kTrigger));
    }

    if (const auto it = element.find("discard"); it != element.end())
    {
      ReadActionDBData(it.value(), asset.effects.at(EffectType::kDiscard));
    }

    if (const auto it = element.find("pasive"); it != element.end())
    {
      ReadActionDBData(it.value(), asset.effects.at(EffectType::kPasive));
    }

    assets.insert({asset.name, asset});
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
    else if (key == kAssetsKey)
    {
      ReadAssetObject(val, mAssets);
    }
  }
}