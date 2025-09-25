# omen-rgb-cli

Command line tool for controlling HP OMEN RGB keyboard lighting.

## Prerequisites

This CLI requires the [omen-rgb-keyboard](https://github.com/alessandromrc/omen-rgb-keyboard) kernel driver to be installed first.

```bash
git clone https://github.com/alessandromrc/omen-rgb-keyboard.git
cd omen-rgb-keyboard
sudo make install
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./omen-rgb-cli <command> [args...]
```

### Basic commands

- `zones <zone> <color>` - Set zone color (0-3, hex color)
- `all <color>` - Set all zones to same color
- `brightness <0-100>` - Set brightness
- `animation <mode> <speed>` - Set animation (static, breathing, rainbow, wave, etc.)
- `read <option>` - Read current settings

### Presets

- `presets` - Browse all presets
- `pride-presets` - Pride flags
- `country-presets` - Country flags  
- `theme-presets` - Color themes

### Examples

```bash
./omen-rgb-cli zones 0 FF0000
./omen-rgb-cli all #FFFFFF
./omen-rgb-cli brightness 75
./omen-rgb-cli pride
./omen-rgb-cli usa
./omen-rgb-cli cyberpunk
```

## Requirements

- HP OMEN RGB keyboard
- [omen-rgb-keyboard kernel driver](https://github.com/alessandromrc/omen-rgb-keyboard) installed
- Linux with sysfs support
- sudo access for hardware control

## Notes

The tool writes to `/sys/devices/platform/omen-rgb-keyboard/rgb_zones/` so you'll need sudo to change colors.
