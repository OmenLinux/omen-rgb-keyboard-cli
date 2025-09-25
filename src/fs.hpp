#pragma once
#include <string>
#include <fstream>
#include <iostream>

namespace omen::fs {
  inline bool writeSysfs(const std::string& path, const std::string& value) {
      std::ofstream file(path);
      if (!file) {
          std::cerr << "Failed to open sysfs path for writing: " << path << "\n";
          return false;
      }

      file << value;
      if (!file) {
          std::cerr << "Failed to write value to sysfs path: " << path << "\n";
          return false;
      }

      return true;
  }

  inline std::string readSysfs(const std::string& path) {
      std::ifstream file(path);
      if (!file) {
          throw std::runtime_error("Failed to open sysfs path for reading: " + path);
      }

      std::string value;
      std::getline(file, value);
      return value;
  }

  inline bool sysfsExists(const std::string& path) {
      std::ifstream file(path);
      return file.good();
  }
}
