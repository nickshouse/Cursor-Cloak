bool keepRunning = true;
#include <windows.h>
#include <thread>

#define ID_TRAY_APP_ICON    5000
#define ID_TRAY_EXIT        3000
#define WM_SYSICON          (WM_USER + 1)

DWORD cursorIdleTime = 3000; // Set this dynamically as per your needs // 3 seconds

NOTIFYICONDATAW nid = {0};

void AddTrayIcon(HWND hWnd) {
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_SYSICON;
    nid.hIcon = LoadIconW(NULL, MAKEINTRESOURCEW(32512));  // 32512 is the resource ID for the application icon
    wcscpy_s(nid.szTip, L"Cursor Cloak");
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    Shell_NotifyIconW(NIM_ADD, &nid);

    // free icon handle
    if(nid.hIcon && DestroyIcon(nid.hIcon)) {
        nid.hIcon = NULL;
    }
}

void DeleteTrayIcon(HWND hWnd) {
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            AddTrayIcon(hWnd);
        } break;

        case WM_DESTROY: {
            keepRunning = false;
            DeleteTrayIcon(hWnd);
            PostQuitMessage(0);
        } break;

        case WM_SYSICON: {
            switch(lParam) {
                case WM_RBUTTONDOWN: {
                    POINT curPoint;
                    GetCursorPos(&curPoint);
                    HMENU hPopMenu = CreatePopupMenu();
                    InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"E&xit");
                    SetMenuDefaultItem(hPopMenu, ID_TRAY_EXIT, FALSE);
                    SetForegroundWindow(hWnd);
                    TrackPopupMenu(hPopMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
                    PostMessage(hWnd, WM_NULL, 0, 0);
                } break;
            }
        } break;

        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case ID_TRAY_EXIT: {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                } break;
            }
        } break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

int main() {
    // create a hidden window to handle system tray icon messages
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"cloaking_app";
    RegisterClassExW(&wc);
    HWND hWnd = CreateWindowExW(0, wc.lpszClassName, L"cloaking_app", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    MessageBoxW(NULL, L"The application is running.", L"Info", MB_OK);

    // Get the current position of the cursor
    POINT current_position;
    GetCursorPos(&current_position);
    POINT last_position = current_position;

    // Load the cursors from files
    HCURSOR defaultCursor = LoadCursorFromFileW(L"cursors/aero_arrow.cur");
    HCURSOR changedCursor = LoadCursorFromFileW(L"cursors/cloak.cur");
    HCURSOR defaultLinkCursor = LoadCursorFromFileW(L"cursors/aero_link.cur");
    HCURSOR defaultTextCursor = LoadCursorFromFileW(L"cursors/beam_i.cur");

    if (!defaultCursor || !changedCursor || !defaultLinkCursor || !defaultTextCursor) {
        MessageBoxW(NULL, L"Failed to load cursors.", L"Error", MB_OK);
        return 0;
    }

    // Change the default cursors
    HCURSOR copyDefault = CopyCursor(defaultCursor);
    SetSystemCursor(copyDefault, 32512);  // 32512 is IDC_ARROW
    HCURSOR copyDefaultLink = CopyCursor(defaultLinkCursor);
    SetSystemCursor(copyDefaultLink, 32649);  // 32649 is IDC_HAND
    HCURSOR copyDefaultText = CopyCursor(defaultTextCursor);
    SetSystemCursor(copyDefaultText, 32513);  // 32513 is IDC_IBEAM

    DWORD cursorLastMoved = GetTickCount();

    // Run the timer in another thread
    std::thread([&]() {
        while (keepRunning) {
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
                if (GetTickCount() - cursorLastMoved > cursorIdleTime) {
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
