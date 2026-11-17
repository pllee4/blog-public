/*
 * composite_loader.cpp17.cpp
 *
 * Created on: Nov 05, 2026 21:05
 * Description:
 *
 * Copyright (c) 2026 Pin Loon Lee (pllee4)
 */

#include <concepts>
#include <iostream>
#include <tuple>
#include <type_traits>
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

class SeasonManager {
 public:
  Season GetCurrentSeason() const;
  void Update(WorldData&) const {
    // world_data.forest = ... some updates
    // world_data.river = ... some updates
  }
  void OnLoaded(const WorldData& world_data) {
    season = world_data.season;
    std::cout << "SeasonManager::OnLoaded" << std::endl;
  }

  int season;
};

class CityManager {
 public:
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
    std::cout << "CityManager::OnLoaded" << std::endl;
  }

 private:
  bool power_shortage_;
  bool virus_outbreak_;
};

template <typename T, typename = void>
inline constexpr bool kHasOnLoaded = false;
template <typename T>
inline constexpr bool kHasOnLoaded<T, std::void_t<decltype(&T::OnLoaded)>> =
    std::is_same_v<decltype(&T::OnLoaded), void (T::*)(const WorldData&)>;

template <typename... Ts>
class WorldDataLoader {
  static_assert((kHasOnLoaded<Ts> && ...),
                "each listener needs exactly: void OnLoaded(const WorldData&)");
 public:
  // do not use const Ts& as we do not want temporaries
  explicit WorldDataLoader(Ts&... ts) : listeners_{ts...} {}
  WorldDataLoader(const WorldDataLoader&) = delete;
  WorldDataLoader& operator=(const WorldDataLoader&) = delete;

  // ProWorldDataLoader{season_manager, city_manager}.Load(world_data);
  void Load(const WorldData& world_data) {
    std::apply([&world_data](
                   auto&... listener) { (listener.OnLoaded(world_data), ...); },
               listeners_);
  }

  // using ProWorldDataLoader = WorldDataLoader<SeasonManager, CityManager>;
  // ProWorldDataLoader::Load(world_data, season_manager, city_manager);
  static void Load(const WorldData& world_data, Ts&... listeners) {
    (listeners.OnLoaded(world_data), ...);
  }

 private:
  std::tuple<Ts&...> listeners_;
};

using BasicWorldDataLoader = WorldDataLoader<SeasonManager>;
using ProWorldDataLoader = WorldDataLoader<SeasonManager, CityManager>;

struct App {
  App() = default;

  // explicitly deleted WorldDataLoader avoid default copying of WorldDataLoader
  // by reference which would refer to the old manager
  // App(const App& other)
  //     : season_manager(other.season_manager),  // copy app1.season_manager
  //     into
  //                                              // app2.season_manager
  //       loader(other.loader)                   // copy app1.loader
  // {}

  App(const App& other)
      : season_manager(other.season_manager), loader{season_manager} {}
  App& operator=(const App& other) {
    season_manager =
        other.season_manager;  // retriever already refers to this->season
    return *this;
  }

  SeasonManager season_manager;
  WorldDataLoader<SeasonManager> loader{season_manager};
};

int main() {
  SeasonManager season_manager;
  CityManager city_manager;
  WorldData world_data;
  //   ProWorldDataLoader::Load(world_data, season_manager, city_manager);
  //   BasicWorldDataLoader::Load(world_data, season_manager);

  WorldDataLoader loader{season_manager, city_manager};
  loader.Load(world_data);

  world_data.season = 1;
  App app1;
  std::cout << app1.season_manager.season << std::endl;
  App app2 = app1;               // defaulted copy
  app2.loader.Load(world_data);  // loads into app 1 if using default copy
  std::cout << app1.season_manager.season << std::endl;
  std::cout << app2.season_manager.season << std::endl;
}