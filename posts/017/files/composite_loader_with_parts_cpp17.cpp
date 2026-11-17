/* 
 * composite_loader_with_parts_cpp17.cpp
 * 
 * Created on: Nov 14, 2026 12:24
 * Description: 
 * 
 * Copyright (c) 2026 Pin Loon Lee (pllee4)
 */ 

#include <iostream>
#include <tuple>
#include <vector>

enum class Season { SPRING, SUMMER, AUTUMN, WINTER };

struct Forest {};
struct River {};

struct City {};
struct RobotsCity {};
struct HumanCity {};
struct SuperHumanCity {};

struct SeedVault {};
struct OrbitalTether {};
struct AnomalyZone {};

struct MemoryReservoir {};

// different component of the system would all information
struct WorldData {
  Forest forest;  // legacy field
  River river;    // legacy field

  HumanCity human_city;
  RobotsCity robots_city;
  SuperHumanCity super_human_city;

  OrbitalTether orbital_tether;  // space elevator anchor points

  AnomalyZone anomaly_zone;  // areas where physics gets unreliable

  int season = 0;
};

// template <typename... Interested>
// struct Parts {
//   template <typename... Changed>
//   static constexpr bool kAnyOf = (std::is_same_v<Changed, Interested> ||
//                                   ...);  // BUG
// };

template <typename... Interested>
struct Parts {
  template <typename Part>
  static constexpr bool kHas = (std::is_same_v<Part, Interested> || ...);

  template <typename... Changed>
  static constexpr bool kAnyOf = (kHas<Changed> || ...);
};

template <typename T, typename... Changed>
inline constexpr bool kInterestedIn =
    sizeof...(Changed) == 0 || T::Interests::template kAnyOf<Changed...>;

class SeasonManager {
 public:
  using Interests = Parts<Forest, River>;
  Season GetCurrentSeason() const;
  void Update(WorldData&) const {
    // world_data.forest = ... some updates
    // world_data.river = ... some updates
  }
  void OnLoaded(const WorldData&) {
    std::cout << "SeasonManager::onLoaded" << std::endl;
  }
};

class CityManager {
 public:
  using Interests = Parts<HumanCity, RobotsCity, SuperHumanCity>;
  void Update(WorldData&) const {
    if (power_shortage_) {
      // world_data.human_city =
      // world_data.robots_city =
      // world_data.super_human_city =
    }
    if (virus_outbreak_) {
      // world_data.human_city =
    }
  }

  void OnLoaded(const WorldData&) {
    std::cout << "CityManager::onLoaded" << std::endl;
  }

 private:
  bool power_shortage_;
  bool virus_outbreak_;
};

template <typename T>
inline constexpr bool kIsParts = false;
template <typename... Ps>
inline constexpr bool kIsParts<Parts<Ps...>> = true;

template <typename T, typename = void>
inline constexpr bool kHasInterests = false;
template <typename T>
inline constexpr bool kHasInterests<T, std::void_t<typename T::Interests>> =
    kIsParts<typename T::Interests>;

template <typename T, typename = void>
inline constexpr bool kHasOnLoaded = false;
template <typename T>
inline constexpr bool kHasOnLoaded<T, std::void_t<decltype(&T::OnLoaded)>> =
    std::is_same_v<decltype(&T::OnLoaded), void (T::*)(const WorldData&)>;

template <typename T>
inline constexpr bool kLoadable = kHasOnLoaded<T> && kHasInterests<T>;

template <typename... Ts>
struct WorldDataLoader {
  static_assert((kHasOnLoaded<Ts> && ...),
                "each listener needs exactly: void OnLoaded(const WorldData&)");
  static_assert((kHasInterests<Ts> && ...),
                "each listener needs: using Interests = Parts<...>");

  // do not use const Ts& as we do not want temporaries
  explicit WorldDataLoader(Ts&... ts) : listeners_{ts...} {}
  WorldDataLoader(const WorldDataLoader&) = delete;
  WorldDataLoader& operator=(const WorldDataLoader&) = delete;

  template <typename... Changed>
  void Load(const WorldData& world_data) {
    std::apply(
        [&world_data](auto&... listener) {
          (
              [&world_data, &listener] {
                using Listener = std::remove_cv_t<std::remove_reference_t<decltype(listener)>>;
                if constexpr (kInterestedIn<Listener, Changed...>) {
                  listener.OnLoaded(world_data);
                }
              }(),
              ...);
        },
        listeners_);
  }

 private:
  std::tuple<Ts&...> listeners_;
};

using ProWorldDataLoader = WorldDataLoader<SeasonManager, CityManager>;

template <typename... Changed, typename... Ts>
void LoadAll(const WorldData& world_data, Ts&... managers) {
  static_assert((kLoadable<Ts> && ...),
                "each manager needs OnLoaded(const WorldData&) and Interests = Parts<...>");
  (
      [&world_data, &managers] {
        if constexpr (kInterestedIn<Ts, Changed...>) {
          managers.OnLoaded(world_data);
        }
      }(),
      ...);
}

int main() {
  SeasonManager season_manager;
  CityManager city_manager;
  WorldData world_data;

  // auto worldLoader = ProWorldDataLoader(season_manager, city_manager);
  WorldDataLoader world_loader{season_manager, city_manager};
  // world_loader.Load(world_data);             // all, cannot compile with the buggy version for the fold
  // world_loader.Load<HumanCity>(world_data);  // CityManager only

  // free function
  // LoadAll<HumanCity, River>(world_data, season_manager); // SeasonManager::onLoaded HumanCity == Forest, River == River
  // LoadAll<River, HumanCity>(world_data, season_manager); // no SeasonManager::onLoaded 	River == Forest, HumanCity == River for the BUG case
  LoadAll<River>(world_data, season_manager); // cannot compile with the buggy version for the fold
}
