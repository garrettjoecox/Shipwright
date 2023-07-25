# SoH Redux

## What is this
I asked myself, if we could re-do it all over again but with the gained knowledge, what would we do differently? This is the result of that question. I have not fully developed my thoughts and plan for this fork long term, some of the changes here may upstreamed, some may not. This project will be executed in phases listed below.

### Phase 1: Strip
The first phase is to strip the CPP side of the project down to the bare minimum required to run the game with a few notable exceptions:
- Full LUS support, including the ability to configure graphics, audio, and input settings.
- Dynamic aspect ratio support (Opposed to the original static 4:3)
- Interpolation
- Saving/Loading files to/from JSON

### Phase 2: Pull Decomp Changes
Ship of Harkinian originally forked from decomp almost 2 years ago and has fallen behind in terms of codebase documentation. We don't need to/can't reach exact parity here but we want to get it as close as possible, and do this more often going forward.

### Phase 3: Experiment
The third phase is to experiment with and establish patterns for adding functionality to the game. Notable projects will include:
- A new UI system (Likely still using ImGui, but streamlined)
- Developing a pattern for interacting with the source code while touching as little of it as possible
- Coming up with "mod" based organization patterns for the codebase
- New architecture for randomizer, which accounts for no duplicated data, auto tracking what's obtainable, multiworld support, etc.

### Phase 4: Rebuild
The fourth phase is to aim for parity with the original project, but with a more organized codebase and a more streamlined development process. The following 4 mods will be developed using the new architecture:
- Debugging mod
- Cheats mod
- Enhancements mod
- Randomizer mod

Notably, these mods should exist in silos, and should be able to be enabled/disabled at independent of each other.

## Project Goals
- **Modularity**: The codebase should be organized in a way that makes it easy to add new features and mods.
- **Maintainability**: The codebase should be easy to maintain, pull requests should be easy to review and merge.
- **Untouched Source**: The original source code should be left untouched as much as possible, this ensures bugs do not slip into the vanilla experience. All vanilla code paths should remain intact, which will also make updating the project with new decomp changes easier.

## Potential Patterns

```cpp
// Original Source example
void En_CowCheckEmptyBottle(EnCow* this, PlayState* play) {
    if (Inventory_HasEmptyBottle(play)) {
        this->actionFunc = En_CowGiveMilk;
        this->actor.textId = 0x4010;
    } else {
        this->actionFunc = En_CowReturnToIdle;
        this->actor.textId = 0x4011;
    }
}
void En_CowGiveMilk(EnCow* this, PlayState* play) {
    Message_CloseTextbox(play);
    this->actionFunc = En_CowIdle;
    GiveItemIdFromActor(&this->actor, play, GI_MILK, 10000.0f, 100.0f);
}

// Overwritten by randomizer mod
void Randomizer_CowGiveItem(EnCow* this, PlayState* play) {
  RandomizerCheck rc = Randomizer_GetRandomizerCheckFromActor(this->actor.params, play->sceneId);

  if (rc.isShuffled()) {
    if (rc.isObtainable() && !rc.isObtained()) {
      this->actionFunc = En_CowIdle;
      Randomzier_ObtainRandomizerCheck(rc);
    } else {
      this->actionFunc = En_CowReturnToIdle;
      this->actor.textId = 0x4011;
    }
  } else {
    this->actionFunc = En_CowCheckEmptyBottle;
  }
}

void registerRandomizerMod() {
  registerFunctionModifier(En_CowCheckEmptyBottle, Randomizer_CowGiveItem, MODIFIER_REPLACE); // MODIFIER_BEFORE, MODIFIER_AFTER
}
```

Original README:
---
![Ship of Harkinian](docs/shiptitle.darkmode.png#gh-dark-mode-only)
![Ship of Harkinian](docs/shiptitle.lightmode.png#gh-light-mode-only)

## Website

Official Website: https://www.shipofharkinian.com/

## Discord

Official Discord: https://discord.com/invite/shipofharkinian

If you're having any trouble after reading through this `README`, feel free ask for help in the Support text channels. Please keep in mind that we do not condone piracy.

# Quick Start

The Ship does not include any copyrighted assets.  You are required to provide a supported copy of the game.

### 1. Verify your ROM dump
You can verify you have dumped a supported copy of the game by using the compatibility checker at https://ship.equipment/. If you'd prefer to manually validate your ROM dump, you can cross-reference its `sha1` hash with the hashes [here](docs/supportedHashes.json).

### 2. Download The Ship of Harkinian from [Discord](https://discord.com/invite/shipofharkinian)
The latest release is available in the most recent post in the `#downloads` channel.

### 3. Launch the Game!
#### Windows
* Extract the zip
* Launch `soh.exe`

#### Linux
* Place your supported copy of the game in the same folder as the appimage.
* Execute `soh.appimage`.  You may have to `chmod +x` the appimage via terminal.

#### macOS
* Run `soh.app`. When prompted, select your supported copy of the game.
* You should see a notification saying `Processing OTR`, then, once the process is complete, you should get a notification saying `OTR Successfully Generated`, then the game should start.

#### Nintendo Switch
* Run one of the PC releases to generate an `oot.otr` and/or `oot-mq.otr` file. After launching the game on PC, you will be able to find these files in the same directory as `soh.exe` or `soh.appimage`. On macOS, these files can be found in `/Users/<username>/Library/Application Support/com.shipofharkinian.soh/`
* Copy the files to your sd card
```
sdcard
└── switch
    └── soh
        ├── oot-mq.otr
        ├── oot.otr
        ├── soh.nro
        └── soh.otr
```
* Launch via Atmosphere's `Game+R` launcher method.

### 4. Play!

Congratulations, you are now sailing with the Ship of Harkinian! Have fun!

# Configuration

### Default keyboard configuration
| N64 | A | B | Z | Start | Analog stick | C buttons | D-Pad |
| - | - | - | - | - | - | - | - |
| Keyboard | X | C | Z | Space | WASD | Arrow keys | TFGH |

### Other shortcuts
| Keys | Action |
| - | - |
| F1 | Toggle menubar |
| F5 | Save state |
| F6 | Change state |
| F7 | Load state |
| F9 | Toggle Text-to-Speech (Windows and Mac only) |
| F11 | Fullscreen |
| Tab | Toggle Alternate assets |
| Ctrl+R | Reset |

### Graphics Backends
Currently, there are three rendering APIs supported: DirectX11 (Windows), OpenGL (all platforms), and Metal (MacOS). You can change which API to use in the `Settings` menu of the menubar, which requires a restart.  If you're having an issue with crashing, you can change the API in the `shipofharkinian.json` file by finding the line `gfxbackend:""` and changing the value to `sdl` for OpenGL. DirectX 11 is the default on Windows.

# Custom Assets

Custom assets are packed in `.otr` files. To use custom assets, place them in the `mods` folder.

If you're interested in creating and/or packing your own custom asset `.otr` files, check out the following tools:
* [**retro - OTR generator**](https://github.com/HarbourMasters64/retro)
* [**fast64 - Blender plugin**](https://github.com/HarbourMasters/fast64)

# Development
### Building

If you want to manually compile SoH, please consult the [building instructions](docs/BUILDING.md).

### Playtesting
If you want to playtest a continuous integration build, you can find them at the links below. Keep in mind that these are for playtesting only, and you will likely encounter bugs and possibly crashes. 

* [Windows](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-windows.zip)
* [macOS](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-mac.zip)
* [Linux (performance)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-performance.zip) _(requires `glibc 2.35` or newer, but will be more performant than the compatibility build.)_
* [Linux (compatibility)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-compatiblity.zip) _(compatible with most Linux distributions, but may not be as performant as the performance build.)_
* [Switch](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-switch.zip)
* [Wii U](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-wiiu.zip)

<a href="https://github.com/Kenix3/libultraship/">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="./docs/poweredbylus.darkmode.png">
    <img alt="Powered by libultraship" src="./docs/poweredbylus.lightmode.png">
  </picture>
</a>
