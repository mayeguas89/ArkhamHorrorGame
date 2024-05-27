#pragma once

#include "player.h"

#include <gmock/gmock.h>

class Mock_Player: public Player
{
public:
  MOCK_METHOD(const Skill&, GetSkill, (), (const override));
};