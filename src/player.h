#pragma once

#include "card.h"
#include "deck.h"
#include "skill.h"

#include <memory>
#include <queue>
#include <string>
#include <vector>

class Player
{
public:
  virtual ~Player() = default;
  virtual const Skill& GetSkill() const = 0;
  virtual uint8_t GetDamagePool() const = 0;
  virtual uint8_t GetHorrorPool() const = 0;
  virtual void DrawCard() = 0;
};

class PlayerImpl: public Player
{
public:
  PlayerImpl(const std::string& name, const std::string& investigatorName)
  {
    mInvestigator = CardFactory::CreateCard<Investigator>(investigatorName);
  }

  const Skill& GetSkill() const override
  {
    return mInvestigator->GetSkill();
  }

  uint8_t GetDamagePool() const override
  {
    return mDamagePool;
  }

  uint8_t GetHorrorPool() const override
  {
    return mHorrorPool;
  }

  void DrawCard() override
  {
    auto card = mDeck.DrawCard();
    mHand.AddCard(std::move(card));
  }

private:
  std::shared_ptr<Card> mInvestigator;
  DeckImpl<std::vector<std::shared_ptr<Card>>> mHand;
  DeckImpl<std::queue<std::shared_ptr<Card>>> mDeck;
  uint8_t mDamagePool{0};
  uint8_t mHorrorPool{0};
};
