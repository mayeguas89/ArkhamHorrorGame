#pragma once
#include "asset.h"
#include "card.h"
#include "investigator.h"

#include <variant>

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
    std::unordered_map<std::string, std::variant<int8_t, std::string>> modifications;
    // Target For modifications
    std::string target;
  };

  struct AssetCardDBData: public CardDBData
  {
    AssetCardDBData()
    {
      effects.insert({EffectType::kActivate, {}});
      effects.insert({EffectType::kTrigger, {}});
      effects.insert({EffectType::kPasive, {}});
      effects.insert({EffectType::kDiscard, {}});
    }
    uint8_t cost{0};                                                   // Coste de la carta
    Skill skill;                                                       // Habilidades para skill tests
    std::optional<std::string> slot;                                   // Espacio que ocupa si lo tiene
    std::optional<uint8_t> uses{std::nullopt};                         // Usos si tiene
    std::unordered_map<EffectType, std::vector<ActionDBData>> effects; // Efectos
    uint8_t health{0};
    uint8_t sanity{0};
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
                                         dbData.skill,
                                         dbData.cost,
                                         dbData.slot.has_value() ? StringToEnum::Get<Slot>(dbData.slot.value()) :
                                                                   Slot::kInvalid,
                                         dbData.uses,
                                         dbData.health,
                                         dbData.sanity);

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
              static_cast<uint8_t>(std::get<int8_t>(action.modifications.find("damage")->second)),
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
            if (action.modifications.find("damage") != action.modifications.end())
            {
              uint8_t damage = static_cast<uint8_t>(std::get<int8_t>(action.modifications.find("damage")->second));
              if (action.modifications.find("condition") == action.modifications.end())
              {
                std::shared_ptr<Effect> effect =
                  std::make_shared<FightEffectWithAdditionalDamage>(mainSkill, action.expend.value_or(0), damage);
                register_function(effect);
              }
              else
              {
                std::shared_ptr<Condition> condition;
                if (std::get<std::string>(action.modifications.find("condition")->second) == "only_enemy_engaged")
                {
                  condition = std::make_shared<OnlyEnemyEngaged>();
                }
                std::shared_ptr<Effect> effect =
                  std::make_shared<FightEffectWithAdditionalDamageWithCondition>(mainSkill,
                                                                                 action.expend.value_or(0),
                                                                                 damage,
                                                                                 condition);
                register_function(effect);
              }
            }
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
                                                                                    std::get<int8_t>(it->second));
            register_function(effect);
          }
        }
        else if (action.action == "make_damage")
        {
          if (auto it = action.modifications.find("damage"); it != action.modifications.end())
          {
            auto effect = std::make_shared<MakeDamageAtTargetAtCurrentLocation>(
              mainSkill,
              action.expend.value_or(0),
              static_cast<uint8_t>(std::get<int8_t>(it->second)));
            register_function(effect);
          }
        }
        else if (action.action == "recive_damage")
        {
          if (auto it = action.modifications.find("damage"); it != action.modifications.end())
          {
            if (action.target == "attacking_enemy")
            {
              auto effect =
                std::make_shared<MakeDamageToAttacker>(mainSkill,
                                                       action.expend.value_or(0),
                                                       static_cast<uint8_t>(std::get<int8_t>(it->second)));
              register_function(effect);
            }
          }
        }
      }
    };

    register_effect(dbData.effects.at(EffectType::kActivate),
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterActivationEffect(effect); });

    register_effect(dbData.effects.at(EffectType::kTrigger),
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterTriggerEffect(effect); });

    register_effect(dbData.effects.at(EffectType::kDiscard),
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterDiscardEffect(effect); });

    register_effect(dbData.effects.at(EffectType::kPasive),
                    [&asset](const std::shared_ptr<Effect>& effect) { asset->RegisterPasiveEffect(effect); });

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