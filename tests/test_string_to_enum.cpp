#include "card.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "string_to_enum.h"

using namespace ::testing;

TEST(StringToEnum, FactionExpectedValues)
{
  EXPECT_EQ(StringToEnum::Get<Faction>("guardian"), Faction::kGuardian);
  EXPECT_EQ(StringToEnum::Get<Faction>("seeker"), Faction::kSeeker);
  EXPECT_EQ(StringToEnum::Get<Faction>("rogue"), Faction::kRogue);
  EXPECT_EQ(StringToEnum::Get<Faction>("mystic"), Faction::kMystic);
  EXPECT_EQ(StringToEnum::Get<Faction>("survivor"), Faction::kSurvivor);
  EXPECT_EQ(StringToEnum::Get<Faction>("neutral"), Faction::kNeutral);
}

TEST(StringToEnum, ThrowsIfInvalidFaction)
{
  try
  {
    StringToEnum::Get<Faction>("");
    FAIL() << "Expected std::runtime_error";
  }
  catch (const std::runtime_error& e)
  {
    ASSERT_STREQ(e.what(), "'' is not registered as Faction");
  }
  catch (...)
  {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(StringToEnum, SlotExpectedValues)
{
  EXPECT_EQ(StringToEnum::Get<Slot>("accessory"), Slot::kAccesory);
  EXPECT_EQ(StringToEnum::Get<Slot>("body"), Slot::kBody);
  EXPECT_EQ(StringToEnum::Get<Slot>("ally"), Slot::kAlly);
  EXPECT_EQ(StringToEnum::Get<Slot>("hand"), Slot::kHand);
  EXPECT_EQ(StringToEnum::Get<Slot>("arcane"), Slot::kArcane);
}

TEST(StringToEnum, ThrowsIfInvalidSlot)
{
  try
  {
    StringToEnum::Get<Slot>("");
    FAIL() << "Expected std::runtime_error";
  }
  catch (const std::runtime_error& e)
  {
    ASSERT_STREQ(e.what(), "'' is not registered as Slot");
  }
  catch (...)
  {
    FAIL() << "Expected std::runtime_error";
  }
}
