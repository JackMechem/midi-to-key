# Midi to Key

> A small, simple, and minimal program for using any midi controller as a macro pad on Linux (Wayland).

<details>

<summary>Table of Contents</summary>

#### Table of Contents

- [Support](#support)
  - [Linux](#support-linux)
- [Dependencies](#dependencies)
- [Compiling and Installing](#comp-and-installing)
  - [Install Dependencies](#cai-1)
    - [Arch Linux](#cai-1-1)
  - [Clone and Compile](#cai-2)
    - [Clone](#cai-2-1)
    - [Build Directory](#cai-2-2)
    - [Build](#cai-2-3)
    - [Install](#cai-2-4)
- [Usage](#usage)
  - [Help Page](#help-page)
  - [Listing and Listening](#listing-io)
    - [Listing all IO](#listing-io-1)
    - [Listening to a Port](#listing-io-2)
  - [Running](#running)
    - [Default Location](#running-default)
    - [Custom Location](#running-custom)
- [Configuration](#configuration)
  - [Default Location](#configuration-default-path)
  - [Config Section](#configuration-config)
  - [Mapping Section](#configuration-mapping)
    - [Overall Syntax](#configuration-mapping-syntax)
    - [Fields](#configuration-mapping-fields)
    - [Key Codes](#configuration-mapping-key-codes)
  - [Example Configuration File](#configuration-example)

</details>

## Support <a name="support"></a>

##### Linux <a name="support-linux"></a>

- Full support for Wayland and X11

## Dependencies <a name="dependencies"></a>

- [rtmidi](https://archlinux.org/packages/extra/x86_64/rtmidi/)
- [toml++ (tomlplusplus)](https://archlinux.org/packages/extra/x86_64/tomlplusplus/)
- [cmake](https://archlinux.org/packages/extra/x86_64/cmake/)
- [gcc](https://archlinux.org/packages/core/x86_64/gcc/)

## Compiling and Installing <a name="comp-and-installing"></a>

### 1. Install Dependencies <a name="cai-1"></a>

##### 1.1 Arch Linux <a name="cai-1-1"></a>

```
sudo pacman -S rtmidi tomlplusplus gcc cmake
```

### 2. Clone the Project and Compile <a name="cai-2"></a>

##### 2.1 Clone the Project <a name="cai-2-1"></a>

```
git clone https://github.com/JackMechem/midi-to-key.git

cd midi-to-key
```

##### 2.2 CD Or Make The Build Directory If It Doesn't Exist <a name="cai-2-2"></a>

```
mkdir build
cd build
```

##### 2.3 Build The Project <a name="cai-2-3"></a>

```
cmake ..        # Must be in the midi-to-key/build directory!
cmake --build .
```

##### 2.4 Install The Project <a name="cai-2-4"></a>

```
sudo cmake --install .
```

##### See the [usage](#usage) and [configuration](#configuration) sections to configure.

## Usage <a name="usage"></a>

### Help Page <a name="help-page"></a>

##### To show this help page run `midi-to-key -h`

```
Usage:
  Help and List:
    midi-to-key {--help|-h} | Shows Help Page
    midi-to-key {--list-io|-lio} | Lists midi inputs and outputs
    midi-to-key {--input-port|ip} <Port-Number> {--listen|-ln} | Listens to specified input port and displays midi note registered - Note: The {--input-port|-ip} flag must be typed before the {--listen|-ln} flag

    Examples:
      midi-to-key -lio | Lists io
      midi-to-key -h | Shows help page
      midi-to-key -ip 2 -ln | Lists input from specified port #2


  Running The Program:
    midi-to-key run [{--config|-c} </path/to/config>] - Note: Default config is $HOME/.config/midi-to-key/config.toml
```

## Listing Midi IO and Listen On Port <a name="listing-io"></a>

### Listing all inputs and outputs <a name="listing-io-1"></a>

The command below lists all midi inputs and output devices and their ports. <br />
**Keep note of the port of the input device you want to use as this is used in the next step and the configuration file!**

```
midi-to-key --list-io
# OR
midi-to-key -lio
```

##### Example output:

```
There are 2 MIDI input sources available.
  Input Port #1: Midi Through:Midi Through Port-0 14:0
  Input Port #2: Arduino Micro:Arduino Micro MIDI 1 16:0

There are 3 MIDI output ports available.
  Output Port #1: Midi Through:Midi Through Port-0 14:0
  Output Port #2: Arduino Micro:Arduino Micro MIDI 1 16:0
```

### Listening to a midi port <a name="listing-io-2"></a>

The command below listens to any inputs coming from the port given (in this example, port #2). <br />
**NOTE: This command must be typed in this order; the port is first.** <br />
Keep note of Byte 0 and Byte 1 for all of the buttons you want to map! <br /> Byte 0 is used to tell if a button is pressed down or released and Byte 1 translates to the midi note being played. <br />
**This will be used in the configuration file later!**

```
midi-to-key -ip 2 -ln               # Replace the 2 with what ever port you are using
OR
midi-to-key --input-port 2 --listen # Replace the 2 with what ever port you are using
```

## Running the Program <a name="running"></a>

### Running With The Default Configuration Path <a name="running-default"></a>

The default configuration path is listed below.
**The configuration file is NOT created automatically!!<br />
See the [configuration](#configuration) section to create a configuration file**

```
~/.config/midi-to-key/config.toml
```

##### The command below runs the program with the default configuration path

```
midi-to-key run
```

### Running With a Custom Configuration Path <a name="running-custom"></a>

##### The command below run the program with a custom configuration path

```
midi-to-key run -c /path/to/config.toml
# OR
midi-to-key run --config /path/to/config.toml
```

## Configuration <a name="configuration"></a>

> This project uses [toml](https://toml.io/) for its configuration file.

#### Default Configuration File Path <a name="configuration-default-path"></a>

```
~/.config/midi-to-key/config.toml
```

### Config Section <a name="configuration-config"></a>

The config section only contains the input port.<br />
To find the input port, see the [listing io](#listing-io-1) section.

```
[config]
inputPort = 2 # The port of your input device
```

### Mapping Section <a name="configuration-mapping"></a>

The mapping section is composed of an array of tables named mapping. <br />

#### Overall Syntax <a name="configuration-mapping-syntax"></a>

```
[[mapping]]            # Every mapping entry is written exactly like this
name = "Bottom Left"   # Name of the mapping that shows up in console
byte0 = 144            # Byte 0 (button position) of the button or event
byte1 = 37             # Byte 1 (note) of the button or event
type = "command"       # Type of event either "command" or "key"
command = "kitty"      # command executed when the event is called

[[mapping]]
name = "Bottom Middle"
byte0 = 144
byte1 = 38
type = "key"
key = [2, 3, 4]        # Key codes that will be simulated when the event is called
```

#### Fields <a name="configuration-mapping-fields"></a>

| Field (\* required)                          | Type                        | Description                                                                                                                                                                                                                                                                                                                                  |
| -------------------------------------------- | --------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| name\*                                       | integer                     | Name of the mapping. This is not important but is recomended as it shows up in the console.                                                                                                                                                                                                                                                  |
| byte0\*                                      | integer                     | Value of byte0 at the desired state of the button where the event will trigger. In the case of a midi button, this value translates to the state of the button, down or up.                                                                                                                                                                  |
| byte1\*                                      | integer                     | Value of byte1 at the desired state of the button where the event will trigger. In the case of a midi button, this value translates to the midi note being played.                                                                                                                                                                           |
| type\*                                       | string ("command" or "key") | Type of event that will be triggered. This is either a command or a simulated key press. The only two values that can go in this field are "command" or "key".                                                                                                                                                                               |
| command (\* only when the type is "command") | string                      | If the event type is set to "command", this field is required. This can be used to launch programs or run any other command. When executed, the `&` symbol is added to the end.                                                                                                                                                              |
| key (\* only when the type is "key")         | Array of integers           | If the event type is set to "key", this field is required. All key codes in the array will be simulated when the event is triggered. To find out what keys translate to what key codes, [see this article (the section on identifying keycodes in console)](https://wiki.archlinux.org/title/Keyboard_input#Identifying_keycodes_in_console) |

#### Key Codes <a name="configuration-mapping-key-codes"></a>

**This program uses linux input event codes!**<br />
To find out what key codes translate to what keys, [see this article (the section on identifying keycodes in console).](https://wiki.archlinux.org/title/Keyboard_input#Identifying_keycodes_in_console) <br/>
You can also run the command below:

```
sudo showkey --keycodes
```

All key codes can also be found in [this header file](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h) located in the linux kernel source.

### Example Configuration <a name="configuration-example"></a>

An example configuration file can be found in the src/ directory of the project source, or below

```
[config]
inputPort = 2          # Port of the midi input device

[[mapping]]            # Every mapping entry is written exactly like this
name = "Bottom Left"   # Name of the mapping that shows up in console
byte0 = 144            # Byte 0 (button position) of the button or event
byte1 = 37             # Byte 1 (note) of the button or event
type = "command"       # Type of event either "command" or "key"
command = "kitty"      # command executed when the event is called

[[mapping]]
name = "Bottom Middle"
byte0 = 144
byte1 = 38
type = "key"
key = [2, 3, 4]        # Key codes that will be simulated when the event is called

[[mapping]]
name = "Bottom Right"
byte0 = 144
byte1 = 39
type = "command"
command = "thunar"
```

---

---

---

> Disclaimer: I am not very experienced in c++. There will be mistakes and there will be poorly written code. I am always learning and will maintain this project to the best of my ability!
