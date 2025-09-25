#include "definitions.hpp"
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <vector>
#include <iomanip>

namespace omen::rgb::utils {
  inline std::vector<std::string> split(const std::string &s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string word;
    while (iss >> word)
      tokens.push_back(word);
    return tokens;
  }

  inline std::string toLower(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
  }

  inline bool isValidRGB(RGB_HEX color) {
    RGB_HEX r = (color >> 16) & 0xFF;
    RGB_HEX g = (color >> 8) & 0xFF;
    RGB_HEX b = color & 0xFF;

    return (r <= 0xFF) && (g <= 0xFF) && (b <= 0xFF);
  }

  inline bool isValidHexChar(char c) {
    return std::isxdigit(static_cast<unsigned char>(c));
  }

  inline RGB_HEX hexStringToRGB(const std::string &hex) {
    if (hex.size() != 6) {
      throw std::invalid_argument("Hex string must have exactly 6 characters");
    }

    RGB_HEX color = 0;

    for (size_t i = 0; i < 6; ++i) {
      char c = hex[i];
      if (!isValidHexChar(c)) {
        throw std::invalid_argument("Invalid character in hex string");
      }

      RGB_HEX value = 0;
      if (c >= '0' && c <= '9')
        value = c - '0';
      else if (c >= 'A' && c <= 'F')
        value = 10 + (c - 'A');
      else if (c >= 'a' && c <= 'f')
        value = 10 + (c - 'a');

      color <<= 4;
      color |= value;
    }

    if (!isValidRGB(color)) {
      throw std::runtime_error("Parsed color channel exceeds 255");
    }

    return color;
  }

  inline uint8_t stringToUint8(const std::string &str) {
    if (str.empty()) {
      throw std::invalid_argument("Empty string cannot be converted to uint8_t");
    }

    uint32_t value = 0;
    for (char c : str) {
      if (!std::isdigit(static_cast<unsigned char>(c))) {
        throw std::invalid_argument("Invalid character in string: " + str);
      }
      value = value * 10 + (c - '0');
      if (value > 255) {
        throw std::out_of_range("Value exceeds 255: " + str);
      }
    }

    return static_cast<uint8_t>(value);
  }

  inline std::string sanitizeHexString(const std::string& s) {
    if (s.empty()) {
        throw std::invalid_argument("Empty color string");
    }
    if (s[0] == '#') {
        if (s.size() != 7) {
            throw std::invalid_argument("Hex string with # must have 7 characters: " + s);
        }
        return s.substr(1);
    } else if (s.size() != 6) {
        throw std::invalid_argument("Hex string must have 6 characters: " + s);
    }
    return s;
  }

  inline std::string rgbHexToUpper(uint32_t value, bool withHash = true) {
    std::ostringstream oss;
    if (withHash) oss << "#";
    oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (value & 0xFFFFFF);
    return oss.str();
  }
}
