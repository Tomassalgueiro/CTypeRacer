# CTypeRacer

A terminal-based typing game written in C. It fetches random quotes from the web, provides colored visual feedback and persists player statistics across sessions.

---

## Features

* **Dynamic Quote Retrieval:** Fetches random quotes using the DummyJSON Quotes API via `libcurl` and parses them with `cJSON`.
* **Real-time Terminal Feedback:** Runs in raw terminal mode (`termios`). Correct keystrokes appear in green, typos are highlighted in underlined red, and pending characters are dimmed.
* **Responsive Multi-line Display:** Queries terminal window dimensions using `ioctl` to compute 2D cursor placement, preventing screen flickering and duplicate text on long quotes.
* **Persistent Statistics:** Saves session metrics (games played, total time elapsed, total characters typed, and best WPM) to binary storage located at `~/.cache/CTypeRacer/stats.bin`.

---

## Dependencies

* C Compiler (`gcc` or `clang` supporting C99/C11)
* `make`
* `libcurl`
* `cJSON`

### Installing Dependencies

#### Debian / Ubuntu
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libcjson-dev
```

#### Arch Linux
```bash
sudo pacman -S base-devel curl cjson
```

#### Fedora / RHEL
```bash
sudo dnf install gcc make libcurl-devel cjson-devel
```

---

## Build and Run

1. Clone the repository and navigate into the project directory:
   ```bash
   git clone git@github.com:Tomassalgueiro/CTypeRacer.git 
   cd CTypeRacer
   ```

2. Compile the project:
   ```bash
   make all
   ```

3. Run the executable:
   ```bash
   ./typeracer
   ```

To clean intermediate object files and build artifacts:
```bash
make clean
```

To perform a clean rebuild:
```bash
make re
```

---

## How to Play

* **[1] Start Game:** Fetches a quote and enters raw input mode.
  * The timer starts as soon as you type the first character.
  * Green characters indicate correct entries; underlined red characters mark mistakes.
  * `Backspace` allows correcting mistakes.
  * `Ctrl+C` exits the active run early.
  * Upon finishing the quote, your elapsed time and net WPM are calculated and stored.
* **[2] Stats:** View cumulative career statistics:
  * Total games played
  * Personal best WPM
  * Total characters typed
  * Cumulative time spent typing
* **[3] Exit:** Saves data and cleanly closes the application.
