#pragma once

#define PROGRAM_NAME "omen-rgb-cli"
#define PROGRAM_VERSION "1.0.0"

#define CMD_ZONES      "zones"
#define CMD_ALL        "all"
#define CMD_BRIGHTNESS "brightness"
#define CMD_ANIMATION  "animation"
#define CMD_READ       "read"
#define CMD_PRIDE      "pride"
#define CMD_TRANS      "trans"
#define CMD_BI         "bi"
#define CMD_PAN        "pan"
#define CMD_ACE        "ace"
#define CMD_LESBIAN    "lesbian"
#define CMD_GAY        "gay"
#define CMD_NONBINARY  "nonbinary"
#define CMD_GENDERFLUID "genderfluid"
#define CMD_AGENDER    "agender"
#define CMD_DEMIGIRL   "demigirl"
#define CMD_DEMIBOY    "demiboy"
#define CMD_ARO        "aro"
#define CMD_DEMI       "demi"
#define CMD_USA        "usa"
#define CMD_UK         "uk"
#define CMD_FRANCE     "france"
#define CMD_GERMANY    "germany"
#define CMD_ITALY      "italy"
#define CMD_SPAIN      "spain"
#define CMD_CANADA     "canada"
#define CMD_JAPAN      "japan"
#define CMD_KOREA      "korea"
#define CMD_BRAZIL     "brazil"
#define CMD_AUSTRALIA  "australia"
#define CMD_INDIA      "india"
#define CMD_CHINA      "china"
#define CMD_RUSSIA     "russia"
#define CMD_MEXICO     "mexico"
#define CMD_SUNSET     "sunset"
#define CMD_OCEAN      "ocean"
#define CMD_FOREST     "forest"
#define CMD_FIRE       "fire"
#define CMD_ICE        "ice"
#define CMD_NEON       "neon"
#define CMD_PASTEL     "pastel"
#define CMD_MIDNIGHT   "midnight"
#define CMD_AURORA     "aurora"
#define CMD_RAINBOW    "rainbow"
#define CMD_PRESETS       "presets"
#define CMD_PRIDE_PRESETS "pride-presets"
#define CMD_COUNTRY_PRESETS "country-presets"
#define CMD_THEME_PRESETS "theme-presets"
#define CMD_EXAMPLES   "examples"
#define CMD_HELP       "help"
#define CMD_EXIT       "exit"
#define CMD_VERSION    "version"

#define ZONES_TEXT "0, 1, 2, 3"

#define ANIMATION_MODES_TEXT "static, breathing, rainbow, wave, pulse, chase, sparkle, candle, aurora, disco"

#define EXAMPLE_COMMANDS_TEXT \
PROGRAM_NAME " " CMD_ZONES " 0 FF0000\n" \
PROGRAM_NAME " " CMD_ALL " #FFFFFF\n" \
PROGRAM_NAME " " CMD_BRIGHTNESS " 50\n" \
PROGRAM_NAME " " CMD_ANIMATION " breathing 3\n"

#define USAGE_TEXT \
"HP OMEN RGB CLI - Available commands:\n\n" \
"Basic Commands:\n" \
CMD_ZONES " <zone_number> <hex_color>   - Set a single zone color (" ZONES_TEXT ", RRGGBB)\n" \
CMD_ALL " <hex_color>                   - Set all zones to the same color\n" \
CMD_BRIGHTNESS " <0-100>                - Set keyboard brightness\n" \
CMD_ANIMATION " <mode> <speed>          - Set animation mode and speed (" ANIMATION_MODES_TEXT ")\n" \
CMD_READ " <option>                     - Read current setting (brightness, animation, zone0-3, all)\n" \
"\nPresets:\n" \
CMD_PRESETS "                           - Browse all available flags and themes\n" \
CMD_PRIDE_PRESETS "                     - Browse pride flag options\n" \
CMD_COUNTRY_PRESETS "                   - Browse country flag options\n" \
CMD_THEME_PRESETS "                     - Browse color theme options\n" \
"\nOther:\n" \
CMD_EXAMPLES "                          - Show example commands\n" \
CMD_HELP "                              - Show this help page\n" \
CMD_VERSION "                           - Show the software version\n" \
"Usage: " PROGRAM_NAME " <command> [args...]\n"

#define RGB_HEX uint32_t
#define ZONE_ID uint8_t

#define HEX_TO_UPPER(value) "0x" << std::hex << std::uppercase << (DWORD64)value << std::dec << std::nouppercase
#define RGB_HEX_TO_UPPER(value) "#" << std::hex << std::uppercase << value << std::dec << std::nouppercase


// ## CMD SUCCESS / ERR ##

#define MSG_OK_ZONE(zone, color) \
    "[OK] Zone " << static_cast<int>(zone) << " color set to " << utils::rgbHexToUpper(color) << " successfully!"

#define MSG_OK_ALL(color) \
    "[OK] All zones color set to " << utils::rgbHexToUpper(color) << " successfully!"

#define MSG_OK_BRIGHTNESS(value) \
    "[OK] Brightness set to " << static_cast<int>(value) << "% successfully!"

#define MSG_OK_ANIMATION(mode, speed) \
    "[OK] Animation mode '" << (mode) << "' set with speed " << (speed) << " successfully!"

#define MSG_OK_READ(option, value) \
    "[OK] " << (option) << ": " << (value)

#define MSG_ERR(text) \
    "[ERROR] " << (text)

// ## FS Paths ##

#define ZONE_BASE_PATH "/sys/devices/platform/omen-rgb-keyboard/rgb_zones/zone"
#define BRIGHTNESS_PATH "/sys/devices/platform/omen-rgb-keyboard/rgb_zones/brightness"
#define ANIMATION_MODE_PATH "/sys/devices/platform/omen-rgb-keyboard/rgb_zones/animation_mode"
#define ANIMATION_SPEED_PATH "/sys/devices/platform/omen-rgb-keyboard/rgb_zones/animation_speed"
#define ALL_ZONES_PATH "/sys/devices/platform/omen-rgb-keyboard/rgb_zones/all"
