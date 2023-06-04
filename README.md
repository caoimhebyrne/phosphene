## phosphene

A demo OS for the Raspberry Pi 4B (and partially the Raspberry Pi 3).

### Building

Make sure you have the `aarch64-elf` toolchain.
On macOS, you can install it using [Homebrew](https://brew.sh):

```bash
$ brew install aarch64-elf-gcc
```

Then, you can run `Scripts/setup.sh` and `Scripts/build.sh` to make a `kernel8.img`

### Running in QEMU

At the moment, QEMU doesn't have support for the RPi4, so we will use the 3B machine.

> **Warning**
> When I add interrupt support, the RPi3 won't be supported as it doesn't use the GIC-400 interrupt controller.

```bash
$ qemu-system-aarch64 -M raspi3b -serial stdio -kernel Build/kernel8.img
```

#### Where did the name `phosphene` come from?

[Here.](https://open.spotify.com/track/0bST5HtiAmqbsEBO50cD4R)

#### Where did the name for the stdlib, `Fluorescent` come from?

You guessed it! [Here.](https://open.spotify.com/track/6jeiwnfLGkmPgY8SLybfIa)

### Credits

- [rpi4os.com](https://www.rpi4os.com/): Getting started with bootstrapping (`src/boot/boot.S` & `src/linker.ld`)
- [OSDev Wiki](https://wiki.osdev.org): Providing values for the different raspberry pi board types
