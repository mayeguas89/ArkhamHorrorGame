#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mock/mock_chaos_bag.h"
#include "mock/mock_player.h"
#include "skill_test.h"

#include <string>

using namespace ::testing;

struct Test_SkillTest: Test
{
private:
  std::unique_ptr<NiceMock<Mock_ChaosBag>> mockChaosBagTmp{std::make_unique<NiceMock<Mock_ChaosBag>>()};
  std::unique_ptr<NiceMock<Mock_Player>> mockPlayerTmp{std::make_unique<NiceMock<Mock_Player>>()};

public:
  Mock_Player& mockPlayer{*mockPlayerTmp.get()};
  Mock_ChaosBag& mockChaosBag{*mockChaosBagTmp.get()};
};

struct Test_SkillTestWillpower: Test_SkillTest
{
  Skill skillTested{1, 0, 0, 0};
  Skill playerSkill{2, 0, 0, 0};
  SkillTest<Willpower> skillTest;
};

TEST_F(Test_SkillTestWillpower, WillpowerTestAutoFail)
{
  ChaosToken chaosToken{ChaosToken::Token::kAutoFail, 0};
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  auto [succeed, _] = skillTest(skillTested, mockPlayer, mockChaosBag, {});
  EXPECT_FALSE(succeed);
}

TEST_F(Test_SkillTestWillpower, WillpowerTestSucceed)
{
  ChaosToken chaosToken{ChaosToken::Token::kValue, 0};
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  EXPECT_CALL(mockPlayer, GetSkill()).WillOnce(ReturnRef(playerSkill));
  auto [succeed, difference] = skillTest(skillTested, mockPlayer, mockChaosBag, {});
  EXPECT_TRUE(succeed);
  EXPECT_EQ(difference, playerSkill.willpower - skillTested.willpower + chaosToken.effect);
}

TEST_F(Test_SkillTestWillpower, WillpowerTestFails)
{
  ChaosToken chaosToken{ChaosToken::Token::kValue, -2};
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  EXPECT_CALL(mockPlayer, GetSkill()).WillOnce(ReturnRef(playerSkill));
  auto [succeed, difference] = skillTest(skillTested, mockPlayer, mockChaosBag, {});
  EXPECT_FALSE(succeed);
  EXPECT_EQ(difference, playerSkill.willpower - skillTested.willpower + chaosToken.effect);
}

TEST_F(Test_SkillTestWillpower, WillpowerTestSucceedWithWillpowerCommitedCards)
{
  ChaosToken chaosToken{ChaosToken::Token::kValue, -2};
  std::shared_ptr<Card> asset =
    std::make_shared<Asset>("InventedCard", Faction::kInvalid, 0, Skill{1, 0, 0, 0}, Slot::kInvalid);
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  EXPECT_CALL(mockPlayer, GetSkill()).WillOnce(ReturnRef(playerSkill));
  auto [succeed, difference] = skillTest(skillTested, mockPlayer, mockChaosBag, {asset});
  EXPECT_TRUE(succeed);
  EXPECT_EQ(difference,
            playerSkill.willpower - skillTested.willpower + chaosToken.effect
              + Willpower::GetSkillValue(asset->GetSkill()));
}

TEST_F(Test_SkillTestWillpower, WillpowerTestSucceedWithWildCommitedCards)
{
  ChaosToken chaosToken{ChaosToken::Token::kValue, -2};
  std::shared_ptr<Card> asset =
    std::make_shared<Asset>("InventedCard", Faction::kInvalid, 0, Skill{0, 0, 0, 0, 1}, Slot::kInvalid);
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  EXPECT_CALL(mockPlayer, GetSkill()).WillOnce(ReturnRef(playerSkill));
  auto [succeed, difference] = skillTest(skillTested, mockPlayer, mockChaosBag, {asset});
  EXPECT_TRUE(succeed);
  EXPECT_EQ(difference,
            playerSkill.willpower - skillTested.willpower + chaosToken.effect
              + Willpower::GetSkillValue(asset->GetSkill()));
}

TEST_F(Test_SkillTestWillpower, WillpowerTestFailsWithCommitedCardsFromOtherSkills)
{
  ChaosToken chaosToken{ChaosToken::Token::kValue, -2};
  std::shared_ptr<Card> asset =
    std::make_shared<Asset>("InventedCard", Faction::kInvalid, 0, Skill{0, 1, 0, 0}, Slot::kInvalid);
  EXPECT_CALL(mockChaosBag, GetToken()).WillOnce(Return(chaosToken));
  EXPECT_CALL(mockPlayer, GetSkill()).WillOnce(ReturnRef(playerSkill));
  auto [succeed, difference] = skillTest(skillTested, mockPlayer, mockChaosBag, {asset});
  EXPECT_FALSE(succeed);
  EXPECT_EQ(difference,
            playerSkill.willpower - skillTested.willpower + chaosToken.effect
              + Willpower::GetSkillValue(asset->GetSkill()));
}
