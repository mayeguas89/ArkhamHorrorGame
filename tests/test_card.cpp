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
  Investigator expectedCard{"Roland Banks", Faction::kGuardian, 3, 3, 4, 2, 9, 5};
  EXPECT_EQ(*card, expectedCard);
}
