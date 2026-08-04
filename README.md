# pin-level
An all-purpose pin-level modular emulator in C for macOS / Linux (Windows support coming soon!)

## Progress
Currently completing the very first computer emulation, the Apple II.

* Main CPU/RAM/ROM pipeline complete
* Keyboard integration complete
* Video currently supports normal (no inverse or flash yet) TEXT mode only, with more coming soon!

The simulation, without `--trace`, runs on average 19% faster than a real Apple II. This headroom will be used in updates in the future!

*A note on video:* The emulator currently uses a hack-y terminal printing loop. This will soon be upgraded to pixel-level fidelity!

## Upcoming Updates
* Replacing spaghetti logic in `main.c`'s `board_tick` with the authentic 74LS-series TTL chips that powered the real computer
* Adding pixel-level fidelity to graphics
* A startup customization menu that allows you to select a board revision, hardware configuration and software version
* A custom DSL to define which parts are wired together where
* Windows support

## Design Philosophy
* As this is a teaching tool, modularity is valued over speed! If you need speed, there are many great emulators out there!
* Kernel-style C is used en masse throughout the repository.

## Structure
Each part's `.c` file in `src/` has a matching `.h` file in `include/`, which details function prototypes and the top-level struct, called `struct <PART_NAME>`. This is what is passed around between functions. This includes all of the pins of each chip (excluding power/ground), defined as `bool *<PIN_NAME>` (or `const bool *<PIN_NAME>` if they are strictly an input of the chip). 

Each top-level struct also has a pointer to a struct inside it, declared as `struct <PART_NAME>_priv *priv`. The fields of this private struct are unknown to the header, and are defined with `static` in the source file. This allows each individual function in a source file to track variables inside a `struct` of its type passed to it, while they are hidden from any other files. This enforces a strict separation of layers.

Each source file has exactly three public functions:

* `struct <PART_NAME> *<PART_NAME>_create(void)`
* `void <PART_NAME>_free(struct <PART_NAME> *c)`
* `void <PART_NAME>_tick(struct <PART_NAME> *c)`

In rare exceptions, a function can take an extra parameter for special reasons.

The top-level logic, in `main.c`, follows a similar structure, but without private structs and normal pins. Instead, `main.c` defines a `struct board`, which contains all the chips in the motherboard and all of the connections between them. Such connections are called *nets*, and are represented by `bool *`s, just as pins are. They are created with `net_create()` and freed en masse with `net_free_all()`.

A net should never change its location—rather, the pins on chips should set their addresses to the address of the net. Thus two or more `bool *`s are pointing to the same memory location, and any side can change the value and have it readable by any other side.

Everything is finished off with a `sigint_handler()` and a keyboard polling loop in `main`.

## Note on AI usage
This code was written wholly and completely by the author of this repository. Anthropic's Claude was quite helpful for  debugging and research, but never to write any of the code in this repository.

## How to Run
First, ensure you have the following files in a `roms/` folder at the project root:

* `e000.bin`, loaded with Integer BASIC part 1
* `e800.bin`, loaded with Integer BASIC part 2
* `f000.bin`, loaded with Integer BASIC part 3
* `f800.bin`, loaded with original Monitor ROM

Then run `make` in the project root to create `emulator`, and run with `./emulator`!

To `RESET` simply type a backslash (`\`), and to exit the simulation type a pipe character (`|`).

Note that `Ctrl+C` is disabled to allow it to be parsed by the emulated machine, so if for any reason the normal exit path (`|`) isn't working, you should send `SIGINT` with `kill -2 <PID>` in another terminal window.

Please avoid at all costs sending any other destruction signals, **ESPECIALLY** not the unhandleable `SIGKILL`. The `SIGINT` handler restores your terminal to the state it was in before, because the emulator changes a potpourri of terminal settings that need to be restored. If you must send one of these signals, the easiest way to restore your settings is by closing the terminal window and opening a new one.

`make clean` cleans up the `build/` folder and the executable.

## Flags
The emulator currently supports the following **LONG-ONLY** flags:

* `--measure`: disables the default infinite-loop simulation and instead runs for exactly 14,318,180 ticks, which is how many ticks are completed per second on a physical Apple II. Upon completion, the emulator will then display the wall-clock time it took to complete those ticks, to the nearest millisecond. If the number displayed is under 1 second, the simulation is overperforming; if not, it is underperforming.
* `--trace`: outputs a full CPU assembly trace into `stderr`. It is highly recommended to redirect `stderr` into a file.
* `--write`: auto-types from a file called `write.txt`. This is quite useful for loading BASIC programs. Nothing is written in the first 50 million ticks to give a few seconds to set up.

These can of course be mixed and matched!

## Special Thanks
A few special thanks:

* to Steve Wozniak and everyone who worked at Apple that made such a revolutionary computer as the Apple II possible,
* to Jim Sather, for creating the **INCREDIBLE** book *Understanding the Apple II*, which made research significantly easier,
* and to Anthropic's Claude AI, who assisted with research and debugging throughout this project!

## Closing Note
Thank you so much for checking out my project! As a young developer, I am always looking for tips from the more experienced out there. Please open a GitHub issue to give any feedback you might have.

Thank you again, and have a nice day!
