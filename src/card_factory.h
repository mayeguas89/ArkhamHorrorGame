#pragma once

#include "card_db.h"

#include <memory>

class CardFactory
{
public:
  template<typename T>
  static inline std::shared_ptr<T> CreateCard(const std::string& name)
  {
    return std::move(CardDB::Instance().GetCard<T>(name));
  }

private:
};
