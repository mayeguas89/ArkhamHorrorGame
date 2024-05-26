#pragma once
#include <memory>
#include <vector>

template<typename T>
inline bool CompareVectorOfSharedPtr(const std::vector<std::shared_ptr<T>>& lhs,
                                     const std::vector<std::shared_ptr<T>>& rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }

  for (size_t i = 0; i < lhs.size(); ++i)
  {
    if (*lhs.at(i) != *rhs.at(i))
    {
      return false;
    }
  }

  return true;
}