<h1 align="center">Frogger</h1>

<p align="center">
  <em>University Project</em>
</p>

<p align="center">
  <strong>Built with:</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=white" alt="C">
  <img src="https://img.shields.io/badge/Make-FFD43B?style=for-the-badge&logo=gnu&logoColor=white" alt="Make">
</p>

---

## Table of Contents

- [About the Project](#about-the-project)
- [Requirements](#requirements)
- [Usage](#usage)

## About the Project

Frogger is a group project developed by [Me](https://github.com/alesmag) and [Anna Chiara Mameli](https://github.com/Pandanna) for the University of Cagliari. The game is a mockup of the original Frogger, where a frog needs to avoid obstacles and enemies trying to reach its homes. In this game, the frog can shoot projectiles
to defeat enemies (who can also shoot). The game features simple graphics to adapt to any system, sound and other effects. 

The project is implemented in C. It focuses on efficient memory management and has both processing and threading versions.

Disclaimer: this project has been developed mainly for Linux x86/x64 systems. 

## Requirements

Frogger requires the following dependencies:

- **Make**
- **GCC (GNU Compiler Collection)**
- **Ncurses library**

### Installation

#### Ubuntu/Debian:
```sh
sudo apt update && sudo apt install -y build-essential gcc libncurses-dev libncursesw-dev
```

#### Fedora:
```sh
sudo dnf install -y make gcc ncurses-devel
```

#### macOS (using Homebrew):
```sh
brew install make gcc ncurses
```

## Usage

### Building the Project
To compile the project, run the following command:
```sh
make
```
This will generate the executable file in the project directory.

### Running the Game
Once compiled, you can start the game by executing:
```sh
./frogger.out
```

### Cleaning the Build
To remove compiled files and reset the build directory, run:
```sh
make clean
```
