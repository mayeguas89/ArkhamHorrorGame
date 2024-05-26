#pragma once

#include <array>
#include <string>

enum class Faction : uint8_t;
enum class Slot : uint8_t;
enum class LocationOptionalEffectCondition : uint8_t;

class StringToEnum
{
public:
  template<typename T>
  static inline T Get(const std::string& name)
  {
    return T::kInvalid;
  }

  template<>
  static inline Slot Get(const std::string& name)
  {
    return Get<Slot, kSlotSize>(name, kSlotNames);
  }

  template<>
  static inline Faction Get(const std::string& name)
  {
    return Get<Faction, kFactionSize>(name, kFactionNames);
  }

  template<>
  static inline LocationOptionalEffectCondition Get(const std::string& name)
  {
    return Get<LocationOptionalEffectCondition, kLocationOptionalEffectConditionSize>(
      name,
      kLocationOptionalEffectConditionNames);
  }

private:
  static inline constexpr uint8_t kFactionSize = 6;
  static inline const std::array<std::string, static_cast<size_t>(kFactionSize)> kFactionNames =
    {"guardian", "seeker", "rogue", "mystic", "survivor", "neutral"};
  static inline constexpr uint8_t kSlotSize = 5;
  static inline const std::array<std::string, static_cast<size_t>(kSlotSize)> kSlotNames = {"accessory",
                                                                                            "body",
                                                                                            "ally",
                                                                                            "hand",
                                                                                            "arcane"};
  static inline constexpr uint8_t kLocationOptionalEffectConditionSize = 1;
  static inline const std::array<std::string, static_cast<size_t>(kLocationOptionalEffectConditionSize)>
    kLocationOptionalEffectConditionNames = {"undiscovered_clues"};

  template<typename T>
  static inline std::string GetTypeName()
  {
    return typeid(T).name();
  }

  template<>
  static inline std::string GetTypeName<Faction>()
  {
    return "Faction";
  }

  template<>
  static inline std::string GetTypeName<Slot>()
  {
    return "Slot";
  }

  template<>
  static inline std::string GetTypeName<LocationOptionalEffectCondition>()
  {
    return "LocationOptionalEffectCondition";
  }

  template<typename T, int Size>
  static inline T Get(const std::string& name, const std::array<std::string, Size>& nameList)
  {
    auto it = std::find(nameList.begin(), nameList.end(), name);
    if (it == nameList.end())
    {
      throw std::runtime_error(fmt::format("'{}' is not registered as {}", name, GetTypeName<T>()));
    }
    auto d = std::distance(nameList.begin(), it);
    return static_cast<T>(d);
  }
};