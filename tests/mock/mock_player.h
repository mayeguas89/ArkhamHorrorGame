#pragma once

#include "player.h"

#include <gmock/gmock.h>

class Mock_Player: public Player
{
public:
  MOCK_METHOD(const Skill&, GetSkill, (), (const override));
  MOCK_METHOD(uint8_t, GetDamagePool, (), (const override));
  MOCK_METHOD(uint8_t, GetHorrorPool, (), (const override));
  MOCK_METHOD(void, DrawCard, (), (override));
};