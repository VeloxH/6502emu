#include <iostream>
#include <yaml-cpp/yaml.h>

#include "emu-config.hpp"

EmuConfig *config;

EmuConfig::EmuConfig(FilePath file_name) {
  // singleton pattern 🤓
  if (config) {
    delete config;
  }

  this->config_file_name = file_name;
  YAML::Node config_root = YAML::LoadFile(file_name);

  for (auto ram_mapping : config_root["ram"]) {
    auto start = ram_mapping["start"].as<StartAddress>();
    auto size = ram_mapping["size"].as<Size>();

    this->ram.push_back({start, size});
  }

  for (auto rom_mapping : config_root["roms"]) {
    auto file_name = rom_mapping["file"].as<FilePath>();
    auto start = rom_mapping["start"].as<StartAddress>();
    auto read_only = true;
    if (rom_mapping["ro"].IsDefined()) {
      read_only = rom_mapping["ro"].as<bool>();
    }

    this->roms.push_back({file_name, start, read_only});
  }

  auto cpu_opts = config_root["cpu"];
  if (cpu_opts.IsDefined()) {
    if (cpu_opts["init_pc"].IsDefined()) {
      if (cpu_opts["init_pc"].as<std::string>() == "reset") {
        this->init_reset = true;
      } else {
        this->init_reset = false;
        this->init_pc = cpu_opts["init_pc"].as<Word>();
      }
    }

    if (cpu_opts["clock_speed"].IsDefined()) {
      this->clock_speed = cpu_opts["clock_speed"].as<uint64_t>();
    }
  }

  auto plugin_opts = config_root["plugins"];
  if (plugin_opts.IsDefined()) {
    if (plugin_opts["enumerate"].IsDefined()) {
      this->enumerate_plugins = plugin_opts["enumerate"].as<bool>();
    }

    if (plugin_opts["config"].IsDefined()) {
      for (YAML::Node plugin_node : plugin_opts["config"]) {
        std::string file_name = plugin_node["file"].as<FileName>();
        Word start_address = 0;
        if (plugin_node["disable"].IsDefined())
          start_address = plugin_node["address"].as<Word>();

        bool plugin_disable = false;
        if (plugin_node["disable"].IsDefined())
          plugin_disable = plugin_node["disable"].as<bool>();

        this->plugin_configs.push_back({file_name, start_address, plugin_disable});
      }
    }
  }
}
