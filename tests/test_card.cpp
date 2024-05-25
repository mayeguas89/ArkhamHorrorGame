#include "card.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace ::testing;

TEST(StringToFaction, ExpectedValues)
{
  EXPECT_EQ(StringToFaction("guardian"), Card::Faction::kGuardian);
  EXPECT_EQ(StringToFaction("seeker"), Card::Faction::kSeeker);
  EXPECT_EQ(StringToFaction("rogue"), Card::Faction::kRogue);
  EXPECT_EQ(StringToFaction("mystic"), Card::Faction::kMystic);
  EXPECT_EQ(StringToFaction("survivor"), Card::Faction::kSurvivor);
  EXPECT_EQ(StringToFaction("neutral"), Card::Faction::kNeutral);
}

TEST(StringToFaction, ThrowsIfInvalidFaction)
{
  try
  {
    StringToFaction("");
    FAIL() << "Expected std::runtime_error";
  }
  catch (const std::runtime_error& e)
  {
    ASSERT_THAT(e.what(), "Faction '' is not registered");
  }
  catch (...)
  {
    FAIL() << "Expected std::runtime_error";
  }
}

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
  Investigator expectedCard{"Roland Banks", Card::Faction::kGuardian, 3, 3, 4, 2, 9, 5};
  EXPECT_EQ(*card, expectedCard);
}
