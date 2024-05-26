#include "card.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace ::testing;

TEST(CardFactory, CreateExistingInvestigatorCard)
{
  ASSERT_THAT(CardFactory::CreateCard<Investigator>("Roland Banks"), NotNull());
}

TEST(CardFactory, CreateNonExistingInvestigatorCard)
{
  ASSERT_THAT(CardFactory::CreateCard<Investigator>(""), IsNull());
}

TEST(CardFactory, CheckExistingInvestigatorCard)
{
  auto card = CardFactory::CreateCard<Investigator>("Roland Banks");
  Investigator expectedCard{"Roland Banks", Faction::kGuardian, {3, 3, 4, 2}, 9, 5};
  EXPECT_EQ(*card, expectedCard);
}

TEST(CardFactory, CreateExistingAssetCard)
{
  ASSERT_THAT(CardFactory::CreateCard<Asset>("Roland's .38 Special"), NotNull());
}

TEST(CardFactory, CreateNonExistingAssetCard)
{
  ASSERT_THAT(CardFactory::CreateCard<Asset>(""), IsNull());
}

TEST(CardFactory, CheckExistingAssetCard_Rolands38Special)
{
  auto card = CardFactory::CreateCard<Asset>("Roland's .38 Special");

  uint8_t cost = 3;
  uint8_t uses = 4;
  uint8_t activationCost = 1;
  uint8_t additionalDamage = 1;

  Skill cardSkill;
  cardSkill.combat = 1;
  cardSkill.agility = 1;
  cardSkill.wild = 1;
  Skill activationEffectSkill;
  activationEffectSkill.combat = 1;
  Skill activationOpionalEffectSkill;
  activationOpionalEffectSkill.combat = 3;

  Asset expectedCard{"Roland's .38 Special", Faction::kNeutral, cost, cardSkill, Slot::kHand, uses};
  std::shared_ptr<Effect> activationEffect = std::make_shared<LocationOptionalFightEffect>(
    activationEffectSkill,
    activationCost,
    additionalDamage,
    LocationOptionalEffect{activationEffectSkill,
                           activationCost,
                           activationOpionalEffectSkill,
                           LocationOptionalEffectCondition::kUndiscoveredClues});
  expectedCard.RegisterActivationEffect(activationEffect);
  EXPECT_EQ(*card, expectedCard);
}

TEST(CardFactory, CheckExistingAssetCard_45Automatic)
{
  auto card = CardFactory::CreateCard<Asset>(".45 Automatic");

  uint8_t cost = 4;
  uint8_t uses = 4;
  uint8_t activationCost = 1;
  uint8_t additionalDamage = 1;

  Skill cardSkill;
  cardSkill.agility = 1;
  Skill activationEffectSkill;
  activationEffectSkill.combat = 1;

  Asset expectedCard{".45 Automatic", Faction::kGuardian, cost, cardSkill, Slot::kHand, uses};
  std::shared_ptr<Effect> activationEffect =
    std::make_shared<FightEffect>(activationEffectSkill, activationCost, additionalDamage);
  expectedCard.RegisterActivationEffect(activationEffect);
  EXPECT_EQ(*card, expectedCard);
}

TEST(CardFactory, CheckExistingAssetCard_PhysicalTraining)
{
  auto card = CardFactory::CreateCard<Asset>("Physical Training");

  uint8_t cost = 2;
  uint8_t activationCost = 1;

  Skill cardSkill;
  cardSkill.combat = 1;
  cardSkill.willpower = 1;

  Skill triggerEffectSkillWillpower;
  triggerEffectSkillWillpower.willpower = 1;

  Skill triggerEffectSkillCombat;
  triggerEffectSkillCombat.combat = 1;

  Asset expectedCard{"Physical Training", Faction::kGuardian, cost, cardSkill, Slot::kInvalid};
  auto triggerEffectWillpower = std::make_shared<Effect>(triggerEffectSkillWillpower, activationCost);
  auto triggerEffectCombat = std::make_shared<Effect>(triggerEffectSkillCombat, activationCost);
  expectedCard.RegisterTriggerEffect(triggerEffectWillpower);
  expectedCard.RegisterTriggerEffect(triggerEffectCombat);
  EXPECT_EQ(*card, expectedCard);
}
