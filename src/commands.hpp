#pragma once
#include "definitions.hpp"
#include "enums.hpp"
#include "fs.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

using namespace omen::enums;

namespace omen::rgb::commands
{

    inline bool checkArgs(const std::vector<std::string> &args, size_t expected,
                          const std::string &usage)
    {

        if (args.size() != expected)
        {
            std::cerr << "Usage: " << usage << "\n";
            return false;
        }
        return true;
    }

    inline void cmdZones(const std::vector<std::string> &args)
    {
        if (!checkArgs(args, 3, std::string(CMD_ZONES) + " <zone_number> <hex_color>"))
            return;

        ZONE_ID zone = utils::stringToUint8(args[1]);
        RGB_HEX color = utils::hexStringToRGB(utils::sanitizeHexString(args[2]));

        std::string path = std::string(ZONE_BASE_PATH) + (zone < 10 ? "0" : "") + std::to_string(zone);

        if (!omen::fs::writeSysfs(path, utils::rgbHexToUpper(color, false)))
        {
            std::cerr << MSG_ERR("Could not set zone color.") << "\n";
        }
        else
        {
            std::cout << MSG_OK_ZONE(zone, color) << "\n";
        }
    }

    inline void cmdAll(const std::vector<std::string> &args)
    {
        if (!checkArgs(args, 2, std::string(CMD_ALL) + " <hex_color>"))
            return;

        RGB_HEX color = utils::hexStringToRGB(utils::sanitizeHexString(args[1]));
        std::string colorStr = utils::rgbHexToUpper(color, false);

        bool allSuccess = true;
        for (int zone = 0; zone < 4; ++zone)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (zone < 10 ? "0" : "") + std::to_string(zone);
            if (!omen::fs::writeSysfs(path, colorStr))
            {
                allSuccess = false;
                break;
            }
        }

        if (!allSuccess)
        {
            std::cerr << MSG_ERR("Could not set all zones color.") << "\n";
        }
        else
        {
            std::cout << MSG_OK_ALL(color) << "\n";
        }
    }

    inline void cmdBrightness(const std::vector<std::string> &args)
    {
        if (!checkArgs(args, 2, std::string(CMD_BRIGHTNESS) + " <0-100>"))
            return;

        uint8_t brightness = utils::stringToUint8(args[1]);

        if (brightness > 100)
        {
            std::cerr << MSG_ERR("Brightness must be between 0 and 100.") << "\n";
            return;
        }

        if (!omen::fs::writeSysfs(BRIGHTNESS_PATH, std::to_string(brightness)))
        {
            std::cerr << MSG_ERR("Could not set brightness.") << "\n";
        }
        else
        {
            std::cout << MSG_OK_BRIGHTNESS(brightness) << "\n";
        }
    }

    inline void cmdAnimation(const std::vector<std::string> &args)
    {
        if (!checkArgs(args, 3, std::string(CMD_ANIMATION) + " <mode> <speed>"))
            return;

        std::string mode = utils::toLower(args[1]);
        uint8_t speed = utils::stringToUint8(args[2]);

        static const std::vector<std::string> validModes = {
            "static", "breathing", "rainbow", "wave", "pulse",
            "chase", "sparkle", "candle", "aurora", "disco"};

        bool validMode = false;
        for (const auto &validModeStr : validModes)
        {
            if (mode == validModeStr)
            {
                validMode = true;
                break;
            }
        }

        if (!validMode)
        {
            std::cerr << MSG_ERR("Invalid animation mode. Valid modes: " ANIMATION_MODES_TEXT) << "\n";
            return;
        }

        if (speed < 1 || speed > 10)
        {
            std::cerr << MSG_ERR("Speed must be between 1 and 10.") << "\n";
            return;
        }

        bool modeSuccess = omen::fs::writeSysfs(ANIMATION_MODE_PATH, mode);
        bool speedSuccess = omen::fs::writeSysfs(ANIMATION_SPEED_PATH, std::to_string(speed));

        if (!modeSuccess || !speedSuccess)
        {
            std::cerr << MSG_ERR("Could not set animation mode.") << "\n";
        }
        else
        {
            std::cout << MSG_OK_ANIMATION(mode, speed) << "\n";
        }
    }

    inline void cmdRead(const std::vector<std::string> &args)
    {
        if (!checkArgs(args, 2, std::string(CMD_READ) + " <option>"))
            return;

        std::string option = utils::toLower(args[1]);

        try
        {
            if (option == "brightness")
            {
                std::string value = omen::fs::readSysfs(BRIGHTNESS_PATH);
                std::cout << MSG_OK_READ("Brightness", value + "%") << "\n";
            }
            else if (option == "animation")
            {
                std::string mode = omen::fs::readSysfs(ANIMATION_MODE_PATH);
                std::string speed = omen::fs::readSysfs(ANIMATION_SPEED_PATH);
                std::cout << MSG_OK_READ("Animation", mode + " (speed: " + speed + ")") << "\n";
            }
            else if (option == "all")
            {
                for (int zone = 0; zone < 4; ++zone)
                {
                    std::string path = std::string(ZONE_BASE_PATH) + (zone < 10 ? "0" : "") + std::to_string(zone);
                    if (omen::fs::sysfsExists(path))
                    {
                        std::string value = omen::fs::readSysfs(path);
                        std::cout << MSG_OK_READ("Zone " + std::to_string(zone), "#" + value) << "\n";
                    }
                }
            }
            else if (option.substr(0, 4) == "zone" && option.length() == 5)
            {
                char zoneChar = option[4];
                if (zoneChar >= '0' && zoneChar <= '3')
                {
                    int zone = zoneChar - '0';
                    std::string path = std::string(ZONE_BASE_PATH) + (zone < 10 ? "0" : "") + std::to_string(zone);
                    if (omen::fs::sysfsExists(path))
                    {
                        std::string value = omen::fs::readSysfs(path);
                        std::cout << MSG_OK_READ("Zone " + std::to_string(zone), "#" + value) << "\n";
                    }
                    else
                    {
                        std::cerr << MSG_ERR("Zone " + std::to_string(zone) + " not available.") << "\n";
                    }
                }
                else
                {
                    std::cerr << MSG_ERR("Invalid zone number. Use zone0, zone1, zone2, or zone3.") << "\n";
                }
            }
            else
            {
                std::cerr << MSG_ERR("Invalid option. Valid options: brightness, animation, zone0-3, all.") << "\n";
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << MSG_ERR("Failed to read " + option + ": " + ex.what()) << "\n";
        }
    }

    inline void cmdExamples() { std::cout << "Example commands:\n"
                                          << EXAMPLE_COMMANDS_TEXT; }

    inline void cmdVersion() { std::cout << PROGRAM_VERSION << std::endl; }

    struct FlagData
    {
        std::string name;
        std::string emoji;
        std::vector<RGB_HEX> colors;
        std::string category;
    };

    static const std::unordered_map<std::string, FlagData> FLAG_DATABASE = {
        {"pride", {"Pride", "🌈", {0xFF0000, 0xFF8000, 0xFFFF00, 0x00FF00, 0x0000FF, 0x8000FF}, "pride"}},
        {"trans", {"Transgender", "⚧", {0x5BCEFA, 0xF5A9B8, 0xFFFFFF, 0xF5A9B8, 0x5BCEFA}, "pride"}},
        {"bi", {"Bisexual", "💖", {0xD60270, 0x9B4F96, 0x0038A8}, "pride"}},
        {"pan", {"Pansexual", "💗", {0xFF1B8D, 0xFFD700, 0x1BB3FF}, "pride"}},
        {"ace", {"Asexual", "🖤", {0x000000, 0xA3A3A3, 0xFFFFFF, 0x800080}, "pride"}},
        {"lesbian", {"Lesbian", "🧡", {0xD52D00, 0xFF9A56, 0xFFFFFF, 0xD362A4, 0xA30262}, "pride"}},
        {"gay", {"Gay", "💙", {0x078D70, 0x26CEAA, 0x98E8C1, 0xFFFFFF, 0x7BADE2, 0x5049CC, 0x3D1A78}, "pride"}},
        {"nonbinary", {"Non-binary", "💛", {0xFFF430, 0xFFFFFF, 0x9C59D1, 0x000000}, "pride"}},
        {"genderfluid", {"Genderfluid", "💜", {0xFF75A2, 0xFFFFFF, 0xBE18D6, 0x000000, 0x333EBD}, "pride"}},
        {"agender", {"Agender", "🤍", {0x000000, 0xBCC4C6, 0xFFFFFF, 0xB8F483, 0xFFFFFF, 0xBCC4C6, 0x000000}, "pride"}},
        {"demigirl", {"Demigirl", "💗", {0x7F7F7F, 0xC4C4C4, 0xFFB6C1, 0xFFFFFF, 0xFFB6C1, 0xC4C4C4, 0x7F7F7F}, "pride"}},
        {"demiboy", {"Demiboy", "💙", {0x7F7F7F, 0xC4C4C4, 0x9ACEEB, 0xFFFFFF, 0x9ACEEB, 0xC4C4C4, 0x7F7F7F}, "pride"}},
        {"aro", {"Aromantic", "🤍", {0x3DA542, 0xA7D379, 0xFFFFFF, 0xA9A9A9, 0x000000}, "pride"}},
        {"demi", {"Demisexual", "💜", {0x000000, 0x7F7F7F, 0xFFFFFF, 0x800080}, "pride"}},
        {"intersex", {"Intersex", "🟡", {0xFFD700, 0x7B68EE, 0xFFD700}, "pride"}},
        {"twospirit", {"Two-Spirit", "🟣", {0x800080, 0xFFFFFF, 0x000000}, "pride"}},

        {"usa", {"United States", "[US]", {0xB22234, 0xFFFFFF, 0x3C3B6E}, "country"}},
        {"uk", {"United Kingdom", "[UK]", {0x012169, 0xFFFFFF, 0xC8102E}, "country"}},
        {"france", {"France", "[FR]", {0x002395, 0xFFFFFF, 0xED2939}, "country"}},
        {"germany", {"Germany", "[DE]", {0x000000, 0xDD0000, 0xFFCE00}, "country"}},
        {"italy", {"Italy", "[IT]", {0x009246, 0xFFFFFF, 0xCE2B37}, "country"}},
        {"canada", {"Canada", "[CA]", {0xFF0000, 0xFFFFFF, 0xFF0000}, "country"}},
        {"japan", {"Japan", "[JP]", {0xFFFFFF, 0xBC002D, 0xFFFFFF}, "country"}},
        {"brazil", {"Brazil", "[BR]", {0x009639, 0xFFDF00, 0x002776}, "country"}},
        {"australia", {"Australia", "[AU]", {0x00008B, 0xFF0000, 0xFFFFFF}, "country"}},
        {"spain", {"Spain", "[ES]", {0xAA151B, 0xF1BF00, 0xAA151B}, "country"}},

        {"sunset", {"Sunset", "🌅", {0xFF6B6B, 0xFF8E53, 0xFF6B9D, 0xC44569, 0xF8B500}, "theme"}},
        {"ocean", {"Ocean", "🌊", {0x006994, 0x0099CC, 0x00CCFF, 0x66E0FF}, "theme"}},
        {"fire", {"Fire", "🔥", {0xFF4500, 0xFF6347, 0xFF7F50, 0xFFA500}, "theme"}},
        {"rainbow", {"Rainbow", "🌈", {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3}, "theme"}},
        {"aurora", {"Aurora", "🌌", {0x00FF9F, 0x00D4FF, 0x9D4EDD, 0x7209B7}, "theme"}},
        {"matrix", {"Matrix", "🟢", {0x00FF00, 0x00CC00, 0x009900, 0x006600}, "theme"}},
        {"cyberpunk", {"Cyberpunk", "🤖", {0xFF0080, 0x00FFFF, 0x8000FF, 0xFFFF00}, "theme"}},
        {"neon", {"Neon", "💡", {0xFF00FF, 0x00FFFF, 0xFFFF00, 0xFF0080}, "theme"}},
        {"galaxy", {"Galaxy", "🌌", {0x4B0082, 0x8A2BE2, 0x9370DB, 0xDA70D6}, "theme"}}};

    inline void cmdFlag(const std::string &flagName)
    {
        auto it = FLAG_DATABASE.find(utils::toLower(flagName));
        if (it == FLAG_DATABASE.end())
        {
            std::cout << "[ERROR] Unknown flag/theme: " << flagName << std::endl;
            return;
        }

        const FlagData &flag = it->second;
        std::cout << "Applying " << flag.emoji << " " << flag.name << " theme..." << std::endl;

        for (size_t i = 0; i < flag.colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (flag.colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, flag.colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }

        if (flag.category == "theme")
        {
            std::cout << "💡 Tip: Use 'animation <mode> <speed>' for dynamic effects!" << std::endl;
        }
    }

    inline void cmdAce()
    {
        std::vector<RGB_HEX> colors = {0x000000, 0x808080, 0xFFFFFF, 0x800080};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdLesbian()
    {
        std::vector<RGB_HEX> colors = {0xD52D00, 0xEF7627, 0xFF9A56, 0xFFFFFF, 0xD162A4, 0xB55690, 0xA30262};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdGay()
    {
        std::vector<RGB_HEX> colors = {0x078D70, 0x26CEAA, 0x98E8C1, 0xFFFFFF, 0x7BADE2, 0x5049CC, 0x3D1A78};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdNonbinary()
    {
        std::vector<RGB_HEX> colors = {0xFFF430, 0xFFFFFF, 0x9C59D1, 0x000000};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdGenderfluid()
    {
        std::vector<RGB_HEX> colors = {0xFF75A2, 0xFFFFFF, 0xBE18D6, 0x000000, 0x333EBD};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdAgender()
    {
        std::vector<RGB_HEX> colors = {0x000000, 0xBCC4C6, 0xFFFFFF, 0xB8F483, 0xFFFFFF, 0xBCC4C6, 0x000000};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdDemigirl()
    {
        std::vector<RGB_HEX> colors = {0x7F7F7F, 0xC4C4C4, 0xFFB7D5, 0xFFFFFF, 0xFFB7D5, 0xC4C4C4, 0x7F7F7F};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdDemiboy()
    {
        std::vector<RGB_HEX> colors = {0x7F7F7F, 0xC4C4C4, 0x9AD9EA, 0xFFFFFF, 0x9AD9EA, 0xC4C4C4, 0x7F7F7F};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdAro()
    {
        std::vector<RGB_HEX> colors = {0x3AA63F, 0xA8D47A, 0xFFFFFF, 0xABABAB, 0x000000};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdDemi()
    {
        std::vector<RGB_HEX> colors = {0x7F7F7F, 0xFFFFFF, 0x7F7F7F};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdUsa()
    {
        std::vector<RGB_HEX> colors = {0xB22234, 0xFFFFFF, 0x3C3B6E};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdUk()
    {
        std::vector<RGB_HEX> colors = {0x012169, 0xFFFFFF, 0xC8102E};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdFrance()
    {
        std::vector<RGB_HEX> colors = {0x002395, 0xFFFFFF, 0xED2939};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdGermany()
    {
        std::vector<RGB_HEX> colors = {0x000000, 0xDD0000, 0xFFCE00};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdItaly()
    {
        std::vector<RGB_HEX> colors = {0x009246, 0xFFFFFF, 0xCE2B37};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdCanada()
    {
        std::vector<RGB_HEX> colors = {0xFF0000, 0xFFFFFF, 0xFF0000};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdJapan()
    {
        std::vector<RGB_HEX> colors = {0xFFFFFF, 0xBC002D, 0xFFFFFF};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdBrazil()
    {
        std::vector<RGB_HEX> colors = {0x009639, 0xFFDF00, 0x002776};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdAustralia()
    {
        std::vector<RGB_HEX> colors = {0x00008B, 0xFF0000, 0xFFFFFF};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdSpain()
    {
        std::vector<RGB_HEX> colors = {0xAA151B, 0xF1BF00, 0xAA151B};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdIntersex()
    {
        std::vector<RGB_HEX> colors = {0xFFD700, 0x7B68EE, 0xFFD700};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdTwospirit()
    {
        std::vector<RGB_HEX> colors = {0x800080, 0xFFFFFF, 0x000000};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdSunset()
    {
        std::vector<RGB_HEX> colors = {0xFF6B6B, 0xFF8E53, 0xFF6B9D, 0xC44569, 0xF8B500};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdOcean()
    {
        std::vector<RGB_HEX> colors = {0x001F3F, 0x0074D9, 0x7FDBFF, 0x39CCCC, 0x3D9970};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdFire()
    {
        std::vector<RGB_HEX> colors = {0xFF0000, 0xFF4500, 0xFF8C00, 0xFFD700, 0xFFFF00};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdRainbow()
    {
        std::vector<RGB_HEX> colors = {0xFF0000, 0xFF7F00, 0xFFFF00, 0x00FF00, 0x0000FF, 0x4B0082, 0x9400D3};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
    }

    inline void cmdAurora()
    {
        std::vector<RGB_HEX> colors = {0x00FF9F, 0x00D4FF, 0x9D4EDD, 0x7209B7};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
        std::cout << "🌌 Aurora theme applied! Use 'animation aurora 5' for full effect." << std::endl;
    }

    inline void cmdMatrix()
    {
        std::vector<RGB_HEX> colors = {0x00FF00, 0x00CC00, 0x009900, 0x006600};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
        std::cout << "🟢 Matrix theme applied! Use 'animation wave 3' for full effect." << std::endl;
    }

    inline void cmdCyberpunk()
    {
        std::vector<RGB_HEX> colors = {0xFF0080, 0x00FFFF, 0x8000FF, 0xFFFF00};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
        std::cout << "🤖 Cyberpunk theme applied! Use 'animation disco 4' for full effect." << std::endl;
    }

    inline void cmdNeon()
    {
        std::vector<RGB_HEX> colors = {0xFF00FF, 0x00FFFF, 0xFFFF00, 0xFF0080};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
        std::cout << "💡 Neon theme applied! Use 'animation chase 6' for full effect." << std::endl;
    }

    inline void cmdGalaxy()
    {
        std::vector<RGB_HEX> colors = {0x4B0082, 0x8A2BE2, 0x9370DB, 0xDA70D6};
        for (size_t i = 0; i < colors.size() && i < 4; ++i)
        {
            std::string path = std::string(ZONE_BASE_PATH) + (i < 10 ? "0" : "") + std::to_string(i);
            std::ostringstream oss;
            oss << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << (colors[i] & 0xFFFFFF);
            if (omen::fs::writeSysfs(path, oss.str()))
            {
                std::cout << MSG_OK_ZONE(i, colors[i]) << std::endl;
            }
            else
            {
                std::cout << "[ERROR] Failed to set zone " << i << " color" << std::endl;
            }
        }
        std::cout << "🌌 Galaxy theme applied! Use 'animation aurora 2' for full effect." << std::endl;
    }

    inline void cmdPridePresets();
    inline void cmdCountryPresets();
    inline void cmdThemePresets();
    inline void cmdPresets()
    {
        std::cout << "\n🎨 PRESET CATEGORIES:\n";
        std::cout << "  1. Pride Flags\n";
        std::cout << "  2. Country Flags\n";
        std::cout << "  3. Color Themes\n";
        std::cout << "\nEnter your choice (1-3): ";

        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            cmdPridePresets();
            break;
        case 2:
            cmdCountryPresets();
            break;
        case 3:
            cmdThemePresets();
            break;
        default:
            std::cout << "Invalid choice! Please enter 1-3.\n";
            break;
        }
    }

    inline void cmdPridePresets()
    {
        std::cout << "\n🏳️‍🌈 PRIDE FLAGS:\n";

        std::vector<std::string> prideFlags;
        for (const auto &pair : FLAG_DATABASE)
        {
            if (pair.second.category == "pride")
            {
                prideFlags.push_back(pair.first);
            }
        }

        for (size_t i = 0; i < prideFlags.size(); ++i)
        {
            const auto &flag = FLAG_DATABASE.at(prideFlags[i]);
            std::cout << "  " << (i + 1) << ". " << prideFlags[i] << " - " << flag.emoji << " " << flag.name << std::endl;
        }

        std::cout << "\nEnter your choice (1-" << prideFlags.size() << "): ";

        int choice;
        std::cin >> choice;

        if (choice >= 1 && choice <= static_cast<int>(prideFlags.size()))
        {
            cmdFlag(prideFlags[choice - 1]);
        }
        else
        {
            std::cout << "Invalid choice! Please enter 1-" << prideFlags.size() << ".\n";
        }
    }

    inline void cmdCountryPresets()
    {
        std::cout << "\n🌍 COUNTRY FLAGS:\n";

        std::vector<std::string> countryFlags;
        for (const auto &pair : FLAG_DATABASE)
        {
            if (pair.second.category == "country")
            {
                countryFlags.push_back(pair.first);
            }
        }

        for (size_t i = 0; i < countryFlags.size(); ++i)
        {
            const auto &flag = FLAG_DATABASE.at(countryFlags[i]);
            std::cout << "  " << (i + 1) << ". " << countryFlags[i] << " - " << flag.emoji << " " << flag.name << std::endl;
        }

        std::cout << "\nEnter your choice (1-" << countryFlags.size() << "): ";

        int choice;
        std::cin >> choice;

        if (choice >= 1 && choice <= static_cast<int>(countryFlags.size()))
        {
            cmdFlag(countryFlags[choice - 1]);
        }
        else
        {
            std::cout << "Invalid choice! Please enter 1-" << countryFlags.size() << ".\n";
        }
    }

    inline void cmdThemePresets()
    {
        std::cout << "\n🎨 COLOR THEMES:\n";

        std::vector<std::string> themes;
        for (const auto &pair : FLAG_DATABASE)
        {
            if (pair.second.category == "theme")
            {
                themes.push_back(pair.first);
            }
        }

        for (size_t i = 0; i < themes.size(); ++i)
        {
            const auto &flag = FLAG_DATABASE.at(themes[i]);
            std::cout << "  " << (i + 1) << ". " << themes[i] << " - " << flag.emoji << " " << flag.name << std::endl;
        }

        std::cout << "\nEnter your choice (1-" << themes.size() << "): ";

        int choice;
        std::cin >> choice;

        if (choice >= 1 && choice <= static_cast<int>(themes.size()))
        {
            cmdFlag(themes[choice - 1]);
        }
        else
        {
            std::cout << "Invalid choice! Please enter 1-" << themes.size() << ".\n";
        }
    }

    inline void printUsage() { std::cout << USAGE_TEXT; }

    inline Command parseCommand(const std::string &cmd)
    {
        static const std::unordered_map<std::string, Command> cmdMap = {
            {CMD_HELP, Command::Help},
            {CMD_ZONES, Command::Zones},
            {CMD_ALL, Command::All},
            {CMD_BRIGHTNESS, Command::Brightness},
            {CMD_ANIMATION, Command::Animation},
            {CMD_READ, Command::Read},
            {CMD_PRIDE, Command::Pride},
            {CMD_TRANS, Command::Trans},
            {CMD_BI, Command::Bi},
            {CMD_PAN, Command::Pan},
            {CMD_ACE, Command::Ace},
            {CMD_LESBIAN, Command::Lesbian},
            {CMD_GAY, Command::Gay},
            {CMD_NONBINARY, Command::Nonbinary},
            {CMD_GENDERFLUID, Command::Genderfluid},
            {CMD_AGENDER, Command::Agender},
            {CMD_DEMIGIRL, Command::Demigirl},
            {CMD_DEMIBOY, Command::Demiboy},
            {CMD_ARO, Command::Aro},
            {CMD_DEMI, Command::Demi},
            {CMD_USA, Command::Usa},
            {CMD_UK, Command::Uk},
            {CMD_FRANCE, Command::France},
            {CMD_GERMANY, Command::Germany},
            {CMD_ITALY, Command::Italy},
            {CMD_SUNSET, Command::Sunset},
            {CMD_OCEAN, Command::Ocean},
            {CMD_FIRE, Command::Fire},
            {CMD_RAINBOW, Command::Rainbow},
            {CMD_PRESETS, Command::Presets},
            {CMD_PRIDE_PRESETS, Command::PridePresets},
            {CMD_COUNTRY_PRESETS, Command::CountryPresets},
            {CMD_THEME_PRESETS, Command::ThemePresets},
            {CMD_EXAMPLES, Command::Examples},
            {CMD_VERSION, Command::Version}};

        auto it = cmdMap.find(cmd);
        return it != cmdMap.end() ? it->second : Command::Unknown;
    }

    inline void execute(int argc, char *argv[])
    {
        if (argc < 2)
        {
            printUsage();
            return;
        }

        std::string cmdStr = utils::toLower(argv[1]);
        std::vector<std::string> args(argv + 1, argv + argc);

        static const std::unordered_map<std::string, std::function<void()>> commandMap = {
            {"help", printUsage},
            {"zones", [&args]()
             { cmdZones(args); }},
            {"all", [&args]()
             { cmdAll(args); }},
            {"brightness", [&args]()
             { cmdBrightness(args); }},
            {"animation", [&args]()
             { cmdAnimation(args); }},
            {"read", [&args]()
             { cmdRead(args); }},
            {"presets", cmdPresets},
            {"pride-presets", cmdPridePresets},
            {"country-presets", cmdCountryPresets},
            {"theme-presets", cmdThemePresets},
            {"examples", cmdExamples},
            {"version", cmdVersion}};

        auto flagIt = FLAG_DATABASE.find(cmdStr);
        if (flagIt != FLAG_DATABASE.end())
        {
            cmdFlag(cmdStr);
            return;
        }

        auto it = commandMap.find(cmdStr);
        if (it != commandMap.end())
        {
            it->second();
        }
        else
        {
            std::cerr << "Unknown command: " << argv[1] << "\n\n";
            printUsage();
        }
    }

}
