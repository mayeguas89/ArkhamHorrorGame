#pragma once

#include "card.h"
#include "skill.h"

#include <memory>
#include <string>

class Player
{
public:
  virtual ~Player() = default;
  virtual const Skill& GetSkill() const = 0;
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

private:
  std::shared_ptr<Card> mInvestigator;
  uint8_t mDamagePool{0};
  uint8_t mHorrorPool{0};
};
