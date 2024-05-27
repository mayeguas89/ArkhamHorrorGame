#pragma once

#include "chaos_bag.h"

#include <gmock/gmock.h>

class Mock_ChaosBag: public ChaosBag
{
public:
  MOCK_METHOD(ChaosToken, GetToken, (), (override));
  MOCK_METHOD(void, AddToken, (const ChaosToken&), (override));
};