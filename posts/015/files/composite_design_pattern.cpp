#include <concepts>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
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
};

class LegacyClass {
 public:
  void LoadWorldData(const WorldData& world_data) {
    forest_ = world_data.forest;
    river_ = world_data.river;
  }

  // falling leaves in autumn, river would be frozen in winter
  void UpdateForSeason(const Season& season);

  Forest GetForest() const { return forest_; }
  River GetRiver() const { return river_; }

  WorldData GetUpdatedWorldData() const {
    WorldData world_data;
    world_data.forest = forest_;
    world_data.river = river_;
    return world_data;
  }

 private:
  Forest forest_;
  River river_;
};

class BadExtendedLegacyClass {
 public:
  void LoadDatabase(const WorldData& world_data) {
    forest_ = world_data.forest;
    river_ = world_data.river;
    human_city_ = world_data.human_city;
    robots_city_ = world_data.robots_city;
    super_human_city_ = world_data.super_human_city;
  }

  void UpdateForSeason(
      const Season& season);  // legacy, affecting forest, river

  void UpdateForPowerOutage();    // newly added, affecting human_city,
                                  // robots_city, super_human_city
  void UpdateForVirusOutbreak();  // newly added, affecting human_city

  Forest GetForest() const;
  River GetRiver() const;

  WorldData GetUpdatedWorldData() const;

 private:
  Forest forest_;
  River river_;

  RobotsCity robots_city_;
  HumanCity human_city_;
  SuperHumanCity super_human_city_;
};

class SeasonManager {
 public:
  Season GetCurrentSeason() const;
  void Update(WorldData&) const {
    // world_data.forest = ... some updates
    // world_data.river = ... some updates
  }
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

 private:
  bool power_shortage_;
  bool virus_outbreak_;
};

class IUpdater {
 public:
  virtual ~IUpdater() = default;
  virtual void Update(WorldData& world_data) const = 0;
};

struct BadSeasonManager : IUpdater {
  // ... other function
  void Update(WorldData&) const override {}
};

struct BadCityManager : IUpdater {
  // ... other function
  void Update(WorldData&) const override {}
};

class BadWorldDataUpdater {
 public:
  void Add(std::shared_ptr<const IUpdater> updater) {
    updaters_.push_back(updater);
  }

  void Update(WorldData& world_data) const {
    for (const auto& updater : updaters_) {
      updater->Update(world_data);
    }
  }

 private:
  std::vector<std::shared_ptr<const IUpdater>> updaters_;
};

// template <typename T>
// concept Updateable = requires {
//   { &T::Update } -> std::same_as<void (T::*)(WorldData&) const>;
// };

// template <Updateable... Ts>

template <typename... Ts>
class WorldDataUpdater {
  static_assert(
      (std::is_same_v<decltype(&Ts::Update), void (Ts::*)(WorldData&) const> &&
       ...),
      "each modifier needs exactly: void Update(WorldData&) const");

 public:
  explicit WorldDataUpdater(Ts&... ts) : updaters_{ts...} {}

  void Update(WorldData& value) const {
    std::apply(
        [&value](const auto&... updater) { (updater.Update(value), ...); },
        updaters_);
  }

 private:
  std::tuple<const Ts&...> updaters_;
};

// decide new infomation should stay in which world
using BasicWorldDataUpdater = WorldDataUpdater<SeasonManager>;
using ProWorldDataUpdater = WorldDataUpdater<CityManager>;

int main() {
  SeasonManager season_manager;
  CityManager city_manager;

  WorldData world_data;
  WorldDataUpdater world_data_updater{season_manager, city_manager};

  BadWorldDataUpdater bad_world_data_updater;

  BadSeasonManager bad_season_manager;
  BadCityManager bad_city_manager;

  bad_world_data_updater.Add(
      std::make_shared<BadSeasonManager>(bad_season_manager));
  bad_world_data_updater.Add(
      std::make_shared<BadCityManager>(bad_city_manager));

  bad_world_data_updater.Update(world_data);
  // season_manager,power_manager, outbreak_manager could do their own
  // processing

  world_data_updater.Update(world_data);

  return 0;
}