![alt text](https://github.com/nickshouse/Cursor-Cloak/blob/main/ccloak.png?raw=true)

# Mouse Cursor Hider (Windows)

This C++ program is designed to hide your mouse cursor after it has been idle for a certain period of time. It accomplishes this by replacing the most commonly seen cursor icons (the default arrow, text select I-beam, and link select hand) with a "cloak" icon that is effectively invisible, hence achieving the effect of hiding the cursor.

The program now includes a system tray icon. You can exit the program by right-clicking the system tray icon and selecting "Exit".

## Program Functionality

Here's how the program works:

1. At startup, the program creates a system tray icon and loads three standard mouse cursor icons (default arrow, text select I-beam, and link select hand) from their corresponding .cur files, as well as a "cloak" cursor intended to be invisible.

2. It then continuously monitors the position of the mouse cursor. As long as the mouse is moving, it displays the appropriate cursor icon for the current context (arrow, I-beam, or hand).

3. If the program detects that the mouse has been idle for a certain amount of time (currently set to 3 seconds), it replaces the current cursor icon with the "cloak" icon, effectively making the cursor disappear.

4. As soon as the mouse is moved again, the program immediately restores the appropriate standard cursor icon.

5. The program keeps running until it is exited from the system tray.

## How to Use

1. Compile the provided C++ source code file. You can do this using a C++ compiler such as `g++`. Here is an example of how to do this from the command line:

```sh
g++ cloak.cpp -o cloak -luser32 -mwindows
```

This command tells `g++` to compile the source file `main.cpp` and output (`-o`) the resulting executable to a file called `cloak`. The `-luser32` flag links the User32 library, which the program requires. The -mwindows flag prevents the Command Prompt from appearing when running the program.

2. Ensure the .cur files for the standard and "cloak" cursors are located in a subdirectory named "cursors".

3. Run the compiled executable. The mouse cursor will now automatically disappear after 3 seconds of inactivity, and reappear as soon as the mouse is moved.

```sh
./cloak
```

**Please Note:** This program uses Windows-specific API calls and is intended for use on Windows operating systems.

## Customization

You can customize the "cloak" cursor and the idle timeout period by providing a different "cloak" .cur file and modifying the `cursorIdleTime` variable in the source code, respectively. Please note that any changes will require recompilation of the program.
