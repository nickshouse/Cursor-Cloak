
![alt text](https://github.com/nickshouse/Cursor-Cloak/blob/main/ccloak.png?raw=true)

# Overview

This C++ program is designed to hide your mouse cursor after it has been idle for a certain period of time. It accomplishes this by replacing the most commonly seen cursor icons (the default arrow, text select I-beam, and link select hand) with a "cloak" icon that is effectively invisible, hence achieving the effect of hiding the cursor.

The program also includes a system tray icon so you can exit the program by right-clicking the system tray icon and selecting "Exit".

## Program Functionality

Here's how the program works:

1. At startup, the program creates a system tray icon and loads three standard mouse cursor icons (default arrow, text select I-beam, and link select hand) from their corresponding .cur files, as well as a "cloak" cursor intended to be invisible.

2. It then continuously monitors the position of the mouse cursor. As long as the mouse is moving, it displays the appropriate cursor icon for the current context (arrow, I-beam, or hand).

3. If the program detects that the mouse has been idle for a certain amount of time (currently set to 3 seconds), it replaces the current cursor icon with the "cloak" icon, effectively making the cursor disappear.

4. As soon as the mouse is moved again, the program immediately restores the appropriate standard cursor icon.

5. The program keeps running until it is exited from the system tray.

## How to Use

Go to the Releases and download the zip. Extract it anywhere and then run the .exe inside the Cursor Cloak folder.

or

Compile the provided C++ source code file. You can do this using a C++ compiler such as `g++`. Here is an example of how to do this from the command line:

First, compile the resource file `ccloak.rc` into `ccloak.res` using the `windres` command. This command translates the resources in the resource file into a binary format that can be included in the final executable:

```sh
windres ccloak.rc -O coff -o ccloak.res
```

Next, compile the source file `ccloak.cpp` and output the resulting object file to `ccloak.o`:

```sh
g++ -c ccloak.cpp -o ccloak.o
```

Finally, link the object and resource files into the final executable `ccloak`, with static linking of required libraries:

```sh
cmd.exe /C "g++ ccloak.o ccloak.res -o ccloak.exe -luser32 -mwindows -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive"
```

The `-luser32` flag links the User32 library, which the program requires. The `-mwindows` flag prevents the Command Prompt from appearing when running the program. The `-static-libgcc` and `-static-libstdc++` flags statically link the GCC and Standard C++ library, making the resulting executable independent of these libraries on the user's system. The `-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive` part is used to statically link the `libwinpthread` library, which is necessary for multi-threading.

Ensure the .cur files for the standard and "cloak" cursors are located in a subdirectory named "cursors".

Run the compiled executable. The mouse cursor will now automatically disappear after 3 seconds of inactivity, and reappear as soon as the mouse is moved.

```sh
./ccloak
```

**Please Note:** This program uses Windows-specific API calls and is intended for use on Windows operating systems only.

## Customization

You can customize the "cloak" cursor and the idle timeout period by providing a different "cloak" .cur file and modifying the `cursorIdleTime` variable in the source code, respectively. Please note that any changes will require recompilation of the program.
