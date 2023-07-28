#include <windows.h>
#include <thread>

#define CURSOR_IDLE_TIME 3000 // 3 seconds

int main() {
    // Get the current position of the cursor
    POINT current_position;
    GetCursorPos(&current_position);
    POINT last_position = current_position;

    // Load the cursor from a file
    HCURSOR defaultCursor = LoadCursorFromFile("cursors/default.cur");
    HCURSOR changedCursor = LoadCursorFromFile("cursors/cloak.cur");

    if (!defaultCursor || !changedCursor) {
        MessageBox(NULL, "Failed to load cursors.", "Error", MB_OK);
        return 0;
    }

    // Change the default cursor
    HCURSOR copyDefault = CopyCursor(defaultCursor);
    SetSystemCursor(copyDefault, 32512);

    DWORD cursorLastMoved = GetTickCount();

    // Run the timer in another thread
    std::thread([&]() {
        while (true) {
            Sleep(100); // wait for 100 milliseconds
            GetCursorPos(&current_position); // get the current position
            if (current_position.x != last_position.x || current_position.y != last_position.y) {
                DestroyCursor(copyDefault); // destroy old copy
                copyDefault = CopyCursor(defaultCursor); // make new copy
                SetSystemCursor(copyDefault, 32512);

                last_position = current_position;

                cursorLastMoved = GetTickCount();
            } else {
                if (GetTickCount() - cursorLastMoved > CURSOR_IDLE_TIME) {
                    DestroyCursor(copyDefault); // destroy old copy
                    copyDefault = CopyCursor(changedCursor); // make new copy
                    SetSystemCursor(copyDefault, 32512);
                }
            }
        }
    }).detach();

    // Keep the application running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    DestroyCursor(copyDefault);
    DestroyCursor(defaultCursor);
    DestroyCursor(changedCursor);

    return 0;
}
