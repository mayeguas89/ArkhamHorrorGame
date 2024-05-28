#include "deck.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "skill_card.h"
using namespace ::testing;

template<typename T>
bool CompareCard(std::shared_ptr<Card>& rhs, std::shared_ptr<T> lhs)
{
  auto pRhs = dynamic_cast<T*>(rhs.get());
  EXPECT_THAT(pRhs, NotNull());
  return *pRhs == *lhs;
}

struct Test_BasicDeck: Test
{
  typedef std::vector<std::shared_ptr<Card>> CardVector;
  typedef std::queue<std::shared_ptr<Card>> CardQueue;
  DeckImpl<CardVector> deck;
  Test_BasicDeck()
  {
    EXPECT_TRUE(deck.Empty());
  }
};

TEST_F(Test_BasicDeck, AddCard)
{
  deck.AddCard(std::make_shared<SkillCard>("CardId0", Faction::kInvalid, Skill{}));
  ASSERT_FALSE(deck.Empty());
  ASSERT_EQ(deck.Size(), 1);
}

struct Test_DrawDeck: Test_BasicDeck
{
  Test_DrawDeck()
  {
    deck.AddCard(std::make_shared<SkillCard>("CardId0", Faction::kInvalid, Skill{}));
  }
};

TEST_F(Test_DrawDeck, DrawCard)
{
  auto card = deck.DrawCard();
  auto expectedCard = std::make_shared<SkillCard>("CardId0", Faction::kInvalid, Skill{});
  ASSERT_TRUE(CompareCard<SkillCard>(card, expectedCard));
}

