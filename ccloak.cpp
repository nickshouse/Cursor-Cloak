bool keepRunning = true;  // Declare and initialize a boolean variable that controls the main loop of the application

#include <windows.h>  // Include Windows header file for Windows specific functionality
#include <thread>     // Include thread header file for multi-threading functionality

#define IDI_APP_ICON 101
#define ID_TRAY_APP_ICON    5000  // Define identifier for the application icon in the system tray
#define ID_TRAY_EXIT        3000  // Define identifier for the exit command in the system tray menu
#define WM_SYSICON          (WM_USER + 1) // Define custom message identifier for system tray icon

DWORD cursorIdleTime = 3000; // Declare and initialize DWORD for the cursor idle time, in milliseconds

NOTIFYICONDATAW nid = {0};  // Declare and initialize a structure that contains information for the system tray icon

void AddTrayIcon(HWND hWnd) {  // Function to add the system tray icon
    nid.cbSize = sizeof(NOTIFYICONDATAW);  // Set the size of the structure
    nid.hWnd = hWnd;  // Set the handle to the window that receives notification messages from the taskbar
    nid.uID = ID_TRAY_APP_ICON;  // Set the application-defined identifier of the taskbar icon
    nid.uVersion = NOTIFYICON_VERSION;  // Set the version of the notification icon
    nid.uCallbackMessage = WM_SYSICON;  // Set the application-defined message identifier sent to the window procedure

    // Load the icon from the file
    nid.hIcon = (HICON)LoadImageW(NULL, L"ccloak.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    if (!nid.hIcon) {
        MessageBoxW(NULL, L"Failed to load icon.", L"Error", MB_OK);
        return;
    }

    wcscpy_s(nid.szTip, L"Cursor Cloak");  // Copy the tooltip text to the structure
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;  // Set the flags to include message, icon, and tooltip
    Shell_NotifyIconW(NIM_ADD, &nid);  // Add the icon to the taskbar
}




void DeleteTrayIcon(HWND hWnd) {  // Function to delete the system tray icon
    Shell_NotifyIconW(NIM_DELETE, &nid); // Delete the icon from the taskbar
    if(nid.hIcon && DestroyIcon(nid.hIcon)) { // Check if icon handle is valid and destroy it
        nid.hIcon = NULL; // Set icon handle to NULL after destruction
    }
}


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {  // Window procedure to handle messages for the hidden window
    switch (msg) {  // Switch statement to handle different messages
        case WM_CREATE: {  // Message for window creation
            AddTrayIcon(hWnd); // Call function to add the tray icon
        } break;

        case WM_DESTROY: {  // Message for window destruction
            keepRunning = false; // Set control variable to false to stop the application loop
            DeleteTrayIcon(hWnd); // Call function to delete the tray icon
            PostQuitMessage(0); // Post a quit message to end the application
        } break;

        case WM_SYSICON: {  // Custom message for system tray icon
            switch(lParam) {  // Switch statement for the lParam value (notification message)
                case WM_RBUTTONDOWN: {  // Right mouse button down on the system tray icon
                    POINT curPoint; // Declare a structure to hold the current cursor position
                    GetCursorPos(&curPoint); // Get the current cursor position
                    HMENU hPopMenu = CreatePopupMenu(); // Create a pop-up menu
                    InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"E&xit"); // Insert an exit item into the menu
                    SetMenuDefaultItem(hPopMenu, ID_TRAY_EXIT, FALSE); // Set the exit item as the default item in the menu
                    SetForegroundWindow(hWnd); // Set the window to the foreground
                    TrackPopupMenu(hPopMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL); // Display the pop-up menu at the current cursor position
                    PostMessage(hWnd, WM_NULL, 0, 0); // Post a null message to the window (this is often used to make the menu disappear when clicking elsewhere)
                } break;
            }
        } break;

        case WM_COMMAND: {  // Message for command input (like menu commands)
            switch(LOWORD(wParam)) {  // Switch statement for the low-order word of wParam (which contains the control ID)
                case ID_TRAY_EXIT: {  // Exit command from the system tray menu
                    SendMessage(hWnd, WM_CLOSE, 0, 0); // Send a close message to the window
                } break;
            }
        } break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam); // Call the default window procedure for unhandled messages
    }

    return 0; // Return 0 to indicate the message was processed
}

int main() { // Main function for the application
    // create a hidden window to handle system tray icon messages
    WNDCLASSEXW wc = {0};  // Declare and initialize a structure that contains the class attributes for the hidden window
    wc.cbSize = sizeof(WNDCLASSEXW);  // Set the size of the structure
    wc.lpfnWndProc = WindowProcedure;  // Set the pointer to the window procedure
    wc.hInstance = GetModuleHandle(NULL); // Get the handle to the current instance of the application
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); // Load the application icon with a specific resource ID
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); // Load the application small icon with a specific resource ID
    wc.lpszClassName = L"cloaking_app";  // Set the class name
    RegisterClassExW(&wc); // Register the class for the hidden window
    HWND hWnd = CreateWindowExW(0, wc.lpszClassName, L"cloaking_app", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL); // Create a hidden window with the specified class

    MessageBoxW(NULL, L"The application is running.", L"Info", MB_OK); // Show a message box to inform the user that the application is running

    // Get the current position of the cursor
    POINT current_position;  // Declare a structure to hold the current cursor position
    GetCursorPos(&current_position); // Get the current cursor position
    POINT last_position = current_position; // Store the current cursor position as the last known position

    // Load the cursors from files
    HCURSOR defaultCursor = LoadCursorFromFileW(L"cursors/aero_arrow.cur"); // Load the default cursor from a file
    HCURSOR changedCursor = LoadCursorFromFileW(L"cursors/cloak.cur"); // Load the changed cursor (cloak) from a file
    HCURSOR defaultLinkCursor = LoadCursorFromFileW(L"cursors/aero_link.cur"); // Load the default link cursor from a file
    HCURSOR defaultTextCursor = LoadCursorFromFileW(L"cursors/beam_i.cur"); // Load the default text cursor from a file
    
    if (!defaultCursor || !changedCursor || !defaultLinkCursor || !defaultTextCursor) {  // Check if any of the cursors have not been loaded successfully
        MessageBoxW(NULL, L"Failed to load cursors.", L"Error", MB_OK);  // Show a message box with an error if a cursor failed to load
        return 0;  // Return 0 to indicate the error
    }

    HCURSOR copyDefault = CopyCursor(defaultCursor);  // Copy the default cursor
    SetSystemCursor(copyDefault, 32512);  // Set the system cursor to the copied default cursor, IDC_ARROW is the ID for the standard arrow cursor

    HCURSOR copyDefaultLink = CopyCursor(defaultLinkCursor);  // Copy the default link cursor
    SetSystemCursor(copyDefaultLink, 32649);  // Set the system link cursor to the copied default cursor, IDC_HAND is the ID for the hand cursor

    HCURSOR copyDefaultText = CopyCursor(defaultTextCursor);  // Copy the default text cursor
    SetSystemCursor(copyDefaultText, 32513);  // Set the system text cursor to the copied default cursor, IDC_IBEAM is the ID for the I-beam cursor

    DWORD cursorLastMoved = GetTickCount();  // Get the current tick count (system time in milliseconds) and save it

    std::thread([&]() {  // Create a new thread to run a timer
        while (keepRunning) {  // Continue running while keepRunning is true
            Sleep(100);  // Pause execution for 100 milliseconds
            GetCursorPos(&current_position);  // Get the current cursor position
            if (current_position.x != last_position.x || current_position.y != last_position.y) {  // Check if the cursor has moved
                DestroyCursor(copyDefault);  // If the cursor has moved, destroy the old copy of the default cursor
                copyDefault = CopyCursor(defaultCursor);  // Copy the default cursor again
                SetSystemCursor(copyDefault, 32512);  // Set the system cursor to the new copy

                DestroyCursor(copyDefaultLink);  // Destroy the old copy of the link cursor
                copyDefaultLink = CopyCursor(defaultLinkCursor);  // Copy the link cursor again
                SetSystemCursor(copyDefaultLink, 32649);  // Set the system link cursor to the new copy

                DestroyCursor(copyDefaultText);  // Destroy the old copy of the text cursor
                copyDefaultText = CopyCursor(defaultTextCursor);  // Copy the text cursor again
                SetSystemCursor(copyDefaultText, 32513);  // Set the system text cursor to the new copy

                last_position = current_position;  // Update the last position to the current position
                cursorLastMoved = GetTickCount();  // Update the last moved time to the current tick count
            } else {
                if (GetTickCount() - cursorLastMoved > cursorIdleTime) {  // If the cursor has not moved, check if it has been idle for longer than cursorIdleTime
                    DestroyCursor(copyDefault);  // If the cursor has been idle, destroy the old copy of the default cursor
                    copyDefault = CopyCursor(changedCursor);  // Copy the changed cursor
                    SetSystemCursor(copyDefault, 32512);  // Set the system cursor to the new copy

                    DestroyCursor(copyDefaultLink);  // Destroy the old copy of the link cursor
                    copyDefaultLink = CopyCursor(changedCursor);  // Copy the changed cursor
                    SetSystemCursor(copyDefaultLink, 32649);  // Set the system link cursor to the new copy

                    DestroyCursor(copyDefaultText);  // Destroy the old copy of the text cursor
                    copyDefaultText = CopyCursor(changedCursor);  // Copy the changed cursor
                    SetSystemCursor(copyDefaultText, 32513);  // Set the system text cursor to the new copy
                }
            }
        }
    }).detach();  // Detach the thread to run independently

    MSG msg;  // Create a variable to store message data
    while (GetMessage(&msg, NULL, 0, 0)) {  // Loop to keep the application running
        TranslateMessage(&msg);  // Translate virtual-key messages into character messages
        DispatchMessage(&msg);  // Dispatches a message to a window procedure
    }

    DestroyCursor(copyDefault);  // Destroy the copied default cursor
    DestroyCursor(defaultCursor);  // Destroy the original default cursor
    DestroyCursor(changedCursor);  // Destroy the changed cursor
    DestroyCursor(copyDefaultLink);  // Destroy the copied link cursor
    DestroyCursor(defaultLinkCursor);  // Destroy the original link cursor
    DestroyCursor(copyDefaultText);  // Destroy the copied text cursor
    DestroyCursor(defaultTextCursor);  // Destroy the original text cursor

    return 0;  // Return 0 to indicate the end of the function
}
