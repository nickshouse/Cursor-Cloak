#include <windows.h>                    // Provides access to Windows APIs.
#include <thread>                       // Provides multi-threading functionality.

#define IDI_APP_ICON 101                // Application icon.
#define ID_TRAY_APP_ICON 5000           // Tray icon.
#define ID_TRAY_EXIT 3000               // Tray exit command.
#define WM_SYSICON (WM_USER + 1)        // System tray message.

bool keepRunning = true;                // Controls the main loop of the application.
DWORD cursorIdleTime = 3000;            // Time (ms) for the cursor to be considered idle.
NOTIFYICONDATAW nid = {0};              // Contains the notification icon data.

// Function to add the system tray icon.
void AddTrayIcon(HWND hWnd, HINSTANCE hInstance) {
    nid.cbSize = sizeof(NOTIFYICONDATAW);        // Size of the structure.
    nid.hWnd = hWnd;                             // Window handle for the taskbar status area icon.
    nid.uID = ID_TRAY_APP_ICON;                  // Application-defined identifier of the taskbar icon.
    nid.uVersion = NOTIFYICON_VERSION;           // Version of the notification icon.
    nid.uCallbackMessage = WM_SYSICON;           // App-defined message identifier sent to the window procedure.
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));   // Load the application icon from the embedded resources.

    // If the icon failed to load, show an error message and return.
    if (!nid.hIcon) {
        MessageBoxW(NULL, L"Failed to load icon.", L"Error", MB_OK);
        return;
    }

    wcscpy_s(nid.szTip, L"Cursor Cloak");               // Tooltip text for the icon.
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;      // Flags to show that nid.uID, nid.uVersion, and nid.szTip are valid.
    Shell_NotifyIconW(NIM_ADD, &nid);                   // Add the notification icon to the taskbar status area.
}

// Function to delete the system tray icon.
void DeleteTrayIcon(HWND hWnd) {
    Shell_NotifyIconW(NIM_DELETE, &nid);                // Delete the notification icon from the taskbar status area.

    // If the icon handle is valid, destroy the icon and set the handle to NULL.
    if(nid.hIcon && DestroyIcon(nid.hIcon)) {
        nid.hIcon = NULL;
    }
}

// Window procedure function to handle messages for the hidden window.
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            AddTrayIcon(hWnd, ((LPCREATESTRUCT)lParam)->hInstance);   // Add the tray icon.
        } break;
        case WM_DESTROY: {
            keepRunning = false;                        // Stop the application loop.
            DeleteTrayIcon(hWnd);                       // Delete the tray icon.
            PostQuitMessage(0);                         // Post a quit message to the message queue of the calling thread.
        } break;
        case WM_SYSICON: {
            switch(lParam) {
                case WM_RBUTTONUP: {
                    POINT curPoint;                     // Current cursor position.
                    GetCursorPos(&curPoint);            // Get the current cursor position.
                    HMENU hPopMenu = CreatePopupMenu(); // Create a pop-up menu.
                    InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"E&xit");  // Insert an exit item into the menu.
                    SetMenuDefaultItem(hPopMenu, ID_TRAY_EXIT, FALSE);    // Set the exit item as the default item in the menu.
                    SetForegroundWindow(hWnd);          // Set the window to the foreground.
                    // Display the pop-up menu at the current cursor position, with the specified alignment and no animation.
                    TrackPopupMenu(hPopMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
                    PostMessage(hWnd, WM_NULL, 0, 0);   // Post a null message to the window.
                } break;
            }
        } break;
        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case ID_TRAY_EXIT: {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);  // Send a close message to the window.
                } break;
            }
        } break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);    // Call the default window procedure for unhandled messages.
    }
    return 0;  // The message was processed.
}

int main() {
    WNDCLASSEXW wc = {0};                 // Window class attributes for the hidden window.
    wc.cbSize = sizeof(WNDCLASSEXW);      // Size of the structure.
    wc.lpfnWndProc = WindowProcedure;     // Window procedure for the window class.
    wc.hInstance = GetModuleHandle(NULL); // Instance handle for the current process.
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APP_ICON));    // Load the application icon from the embedded resources.
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APP_ICON));  // Load the small icon for the application from the embedded resources.
    wc.lpszClassName = L"cloaking_app";   // Name of the window class.
    RegisterClassExW(&wc);                // Register the window class.
    HWND hWnd = CreateWindowExW(0, wc.lpszClassName, L"cloaking_app", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);  // Create a hidden message-only window.

    MessageBoxW(NULL, L"The application is running.", L"Info", MB_OK);  // Inform the user that the application is running.

    POINT current_position;                     // Current cursor position.
    GetCursorPos(&current_position);            // Get the current cursor position.
    POINT last_position = current_position;     // Store the current cursor position as the last known position.

    // Load the cursors from files.
    HCURSOR defaultCursor = LoadCursorFromFileW(L"cursors/aero_arrow.cur");
    HCURSOR changedCursor = LoadCursorFromFileW(L"cursors/cloak.cur");
    HCURSOR defaultLinkCursor = LoadCursorFromFileW(L"cursors/aero_link.cur");
    HCURSOR defaultTextCursor = LoadCursorFromFileW(L"cursors/beam_i.cur");

    // If any of the cursors failed to load, show an error message and return 0.
    if (!defaultCursor || !changedCursor || !defaultLinkCursor || !defaultTextCursor) {
        MessageBoxW(NULL, L"Failed to load cursors.", L"Error", MB_OK);
        return 0;
    }

    // Copy the default cursor and set it as the system cursor.
    HCURSOR copyDefault = CopyCursor(defaultCursor);
    SetSystemCursor(copyDefault, 32512);

    // Copy the link cursor and set it as the system cursor.
    HCURSOR copyDefaultLink = CopyCursor(defaultLinkCursor);
    SetSystemCursor(copyDefaultLink, 32649);

    // Copy the text cursor and set it as the system cursor.
    HCURSOR copyDefaultText = CopyCursor(defaultTextCursor);
    SetSystemCursor(copyDefaultText, 32513);

    DWORD cursorLastMoved = GetTickCount();     // Get the current system time in milliseconds.

    // Create a new thread to run a timer.
    std::thread([&]() {
        while (keepRunning) {                   // Continue running while keepRunning is true.
            Sleep(100);                         // Pause execution for 100 milliseconds.
            GetCursorPos(&current_position);    // Get the current cursor position.
            // If the cursor has moved, update the cursors and the last moved time.
            if (current_position.x != last_position.x || current_position.y != last_position.y) {
                DestroyCursor(copyDefault);
                copyDefault = CopyCursor(defaultCursor);
                SetSystemCursor(copyDefault, 32512);

                DestroyCursor(copyDefaultLink);
                copyDefaultLink = CopyCursor(defaultLinkCursor);
                SetSystemCursor(copyDefaultLink, 32649);

                DestroyCursor(copyDefaultText);
                copyDefaultText = CopyCursor(defaultTextCursor);
                SetSystemCursor(copyDefaultText, 32513);

                last_position = current_position;
                cursorLastMoved = GetTickCount();
            } else {
                // If the cursor has been idle, update the cursors.
                if (GetTickCount() - cursorLastMoved > cursorIdleTime) {
                    DestroyCursor(copyDefault);
                    copyDefault = CopyCursor(changedCursor);
                    SetSystemCursor(copyDefault, 32512);

                    DestroyCursor(copyDefaultLink);
                    copyDefaultLink = CopyCursor(changedCursor);
                    SetSystemCursor(copyDefaultLink, 32649);

                    DestroyCursor(copyDefaultText);
                    copyDefaultText = CopyCursor(changedCursor);
                    SetSystemCursor(copyDefaultText, 32513);
                }
            }
        }
    }).detach();                    // Detach the thread to run independently.

    MSG msg;                        // Message data.
    // Loop to keep the application running.
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);     // Translate virtual-key messages into character messages.
        DispatchMessage(&msg);      // Dispatches a message to a window procedure.
    }

    // Destroy all cursors.
    DestroyCursor(copyDefault);
    DestroyCursor(defaultCursor);
    DestroyCursor(changedCursor);
    DestroyCursor(copyDefaultLink);
    DestroyCursor(defaultLinkCursor);
    DestroyCursor(copyDefaultText);
    DestroyCursor(defaultTextCursor);

    return 0;                       // End of the function.
}
