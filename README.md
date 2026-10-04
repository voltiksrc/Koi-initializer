# koiinit

A tiny C++ project initializer that is basically a mini Cargo.

It creates a simple starter project using either Meson or CMake.

## Usage

Create a Meson project:

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

## Example
```bash
koiinit virus
```
```text
Created project 'virus' using Meson.
```
What it makes:
```text
virus
├── meson.build
├── README.md
└── src
    └── main.cpp
```
## Build and install

```bash
meson setup build
```

```bash
meson compile -C build
```

```bash
sudo meson install -C build
```
