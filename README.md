# koiinit

A tiny C++ project initializer that is basically a mini Cargo.

It creates a simple starter project using either Make, CMake, or Meson.

## Usage

Create a Makefile project:

```bash
koiinit myproject
```

Create a CMake project:

```bash
koiinit myproject --cmake
```

Create a CMake project and initialize a empty git repo:
```bash
koiinit myproject --cmake --git
```

Create a Meson project:

```bash
koiinit myproject --meson
```

Create a Meson project and initialize a empty git repo:

```bash
koiinit --meson --git
```

## Example
```bash
koiinit virus 
```
```text
Created project 'virus' using Make.
```
What it makes:
```text
virus
├── .gitignore
├── .clang-format
├── Makefile
├── README.md
└── src
    └── main.cpp
```

Or with Meson:
```bash
koiinit virus --meson
```
```text
Created project 'virus' using Meson.
```

```text
virus
├── .gitignore
├── .clang-format
├── meson.build
├── README.md
└── src
    └── main.cpp
```

## Build and install

```bash
make -j8
```

```bash
sudo make install
```
