#include <windows.h>
#include <thread>

#define CURSOR_IDLE_TIME 3000 // 3 seconds

int main() {
    // Get the current position of the cursor
    POINT current_position;
    GetCursorPos(&current_position);
    POINT last_position = current_position;

    // Load the cursors from files
    HCURSOR defaultCursor = LoadCursorFromFile("cursors/aero_arrow.cur");
    HCURSOR changedCursor = LoadCursorFromFile("cursors/cloak.cur");
    HCURSOR defaultLinkCursor = LoadCursorFromFile("cursors/aero_link.cur");
    HCURSOR defaultTextCursor = LoadCursorFromFile("cursors/beam_i.cur");

    if (!defaultCursor || !changedCursor || !defaultLinkCursor || !defaultTextCursor) {
        MessageBox(NULL, "Failed to load cursors.", "Error", MB_OK);
        return 0;
    }

    // Change the default cursors
    HCURSOR copyDefault = CopyCursor(defaultCursor);
    SetSystemCursor(copyDefault, 32512);  // 32512 is OCR_NORMAL
    HCURSOR copyDefaultLink = CopyCursor(defaultLinkCursor);
    SetSystemCursor(copyDefaultLink, 32649);  // 32649 is OCR_HAND
    HCURSOR copyDefaultText = CopyCursor(defaultTextCursor);
    SetSystemCursor(copyDefaultText, 32513);  // 32513 is OCR_IBEAM

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

                DestroyCursor(copyDefaultLink); // destroy old copy
                copyDefaultLink = CopyCursor(defaultLinkCursor); // make new copy
                SetSystemCursor(copyDefaultLink, 32649);

                DestroyCursor(copyDefaultText); // destroy old copy
                copyDefaultText = CopyCursor(defaultTextCursor); // make new copy
                SetSystemCursor(copyDefaultText, 32513);

                last_position = current_position;
                cursorLastMoved = GetTickCount();
            } else {
                if (GetTickCount() - cursorLastMoved > CURSOR_IDLE_TIME) {
                    DestroyCursor(copyDefault); // destroy old copy
                    copyDefault = CopyCursor(changedCursor); // make new copy
                    SetSystemCursor(copyDefault, 32512);

                    DestroyCursor(copyDefaultLink); // destroy old copy
                    copyDefaultLink = CopyCursor(changedCursor); // make new copy using cloak cursor
                    SetSystemCursor(copyDefaultLink, 32649);

                    DestroyCursor(copyDefaultText); // destroy old copy
                    copyDefaultText = CopyCursor(changedCursor); // make new copy using cloak cursor
                    SetSystemCursor(copyDefaultText, 32513);
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
    DestroyCursor(copyDefaultLink);
    DestroyCursor(defaultLinkCursor);
    DestroyCursor(copyDefaultText);
    DestroyCursor(defaultTextCursor);

    return 0;
}
