# Welcome to DreamRV!

DreamRV (from dream RISC-V) is a virtual platform to test and run simple programs created in RISC-V assembly. Similar to the PICO-8, but very low level: perfect for learning assembly and how computers work!


 ## Features
* **Simple MMIO RGBA framebuffer and buttons:** forget about interrupts! all you need to do is read some memory addresses to display pixels to the screen, or get gamepad inputs

* **Debugging info:** not quite able to squish that bug? DreamRV offers a debug mode, where you can go cycle by cycle, watching how registers and memory values change

## Usage
### Creating programs
To use the platform, it's essential to know how MMIO virtual devices are mapped:
* **frame buffer**: **0x80000-0x83FFF**, row-first as RGBA values (e.g. 0xFF0000FF is red, 0xFFFFFFFF is white...)
* **keypad**: **0x84000-0x87007**, read as a byte per button in the following order: up, down, left, right, A, B, C, D. Note that the buttons are mapped by default to the keyboard as up, down, left, right, Q, W, E, R

**Direction polling loop example**
```
button_poll_loop:

lb t0, 0(s1)      # get up
lb t1, 1(s1)      # get down
lb t2, 2(s1)      # get left
lb t3, 3(s1)      # get right

# t0-3 = 0x01 if the button is down, 0x00 if not

j button_poll_loop
```

**Pixel plotting function example**
```
plot_pixel:       # sets the pixel in x=a0, y=a1 to color=a2

li t1, 0x80000    # framebuffer mmio base
li t2, 4          # 4 bytes per pixel

mul a0, a0, t2    # calc x offset (x * 4)

li t3, 512        # offset per line in bytes (128 * 4)

mul a1, a1, t3    # calc y offset (y * (x * 4))

add t1, t1, a1    # add offsets to base
add t1, t1, a0

sw a2, 0(t1)
ret
```

**Example program: a movable dot on the screen**
```
.section .text
.globl _start

li s1, 0x84000    # button map base

li s2, 64         # current x
li s3, 63         # previous x

li s4, 64         # current y
li s5, 63         # previous y

li s6, 0x80000    # famebuffer mmio base

li s0, 0
li s7, 127        # for bounds check

button_poll_loop:

lb t0, 0(s1)      # get up
beqz t0, not_up
addi s4, s4, -1
not_up:

lb t0, 1(s1)      # get down
beqz t0, not_down
addi s4, s4, 1
not_down:

lb t0, 2(s1)      # get left
beqz t0, not_left
addi s2, s2, -1
not_left:

lb t0, 3(s1)      # get right
beqz t0, not_right
addi s2, s2, 1
not_right:

# bounds check
ble s2, s7, not_x_over
mv s2, s7
not_x_over:
bge s2, s0, not_x_under
mv s2, s0
not_x_under:
ble s4, s7, not_y_over
mv s4, s7
not_y_over:
bge s4, s0, not_y_under
mv s4, s0
not_y_under:


# plot on change
bne s3, s2, changed 
bne s4, s5, changed
j button_poll_loop

changed:
mv a0, s3
mv a1, s5
li a2, 0 
call plot_pixel   # clear previous px

mv s3, s2         # copy new coords
mv s5, s4

mv a0, s2
mv a1, s4
li a2, 0xFFFFFFFF
call plot_pixel

j button_poll_loop

plot_pixel:     # sets the pixel in x=a0, y=a1 to color=a2

li t1, 0x80000    # framebuffer mmio base
li t2, 4          # 4 bytes per pixel

mul a0, a0, t2    # calc x offset (x * 4)

li t3, 512        # offset per line in bytes (128 * 4)

mul a1, a1, t3    # calc y offset (y * (x * 4))

add t1, t1, a1    # add offsets to base
add t1, t1, a0

sw a2, 0(t1)
ret
```

### Compiling programs
**In Windows**
To use the included script (buildRV32_win.bat), you need to have MSYS2 MINGW64 installed, with the binaries folder in your PATH
Inside the MSYS2 MINGW64 terminal, run: `pacman -S mingw-w64-x86_64-riscv64-elf-toolchain`
Finaly, run the script from CMD or Powershell: `.\buildRV32_win.bat yourAssemblyCode.s`

**In Linux**
For debian-based distros with APT run: 
```
sudo apt update
sudo apt install gcc-riscv64-unknown-elf
```
Then, give the script (buildRV32_linux.sh) execution permissions with `chmod u+x buildRV32_linux.sh` and run it: `./buildRV32_linux.sh yourAssemblyCode.s`. You'll get a .bin as output!

### Running programs
``` ./DreamRV programName.bin [-c clockFrequency] [-m memoryAddress] [-s] [-f]```
* **clockFrequency**: what the CPU simulation will run at when unlocked (positive integer)
* **memoryAddress**: on debug mode, the default memory page to show (positive integer, can be hex using 0x...)
* **-s**: screen only mode, without the CPU info.
* **-f**: force, jump directly to the simulation loop without loading the program binary

## Images
![DreamRV debug mode screenshot](assets/DreamRV-debug-mode.png "DreamRV debug mode")
![DreamRV screen mode screenshot](assets/DreamRV-screen.mode.png "DreamRV screen mode")
