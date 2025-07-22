// main.cpp

#include <windows.h>
#include <thread>

// Define resource identifiers (same as in ccloak.rc)
#define IDI_APP_ICON 101      // Application icon
#define ID_TRAY_APP_ICON 5000 // Tray icon
#define ID_TRAY_EXIT 3000     // Tray exit command

// Define cursor constants if not already defined
#ifndef OCR_NORMAL
#define OCR_NORMAL 32512
#endif

#ifndef OCR_IBEAM
#define OCR_IBEAM 32513
#endif

#ifndef OCR_WAIT
#define OCR_WAIT 32514
#endif

#ifndef OCR_HAND
#define OCR_HAND 32649
#endif

#ifndef OCR_APPSTARTING
#define OCR_APPSTARTING 32650
#endif

#define WM_SYSICON (WM_USER + 1)  // System tray message

bool keepRunning = true;          // Controls the main loop of the application
DWORD cursorIdleTime = 3000;      // Time (ms) for the cursor to be considered idle
NOTIFYICONDATAW nid = {0};        // Contains the notification icon data

// Function to add the system tray icon
void AddTrayIcon(HWND hWnd, HINSTANCE hInstance) {
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uVersion = NOTIFYICON_VERSION;
    nid.uCallbackMessage = WM_SYSICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    if (!nid.hIcon) {
        MessageBoxW(NULL, L"Failed to load icon.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    wcscpy_s(nid.szTip, L"Cursor Cloak");
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    Shell_NotifyIconW(NIM_ADD, &nid);
}

// Function to delete the system tray icon
void DeleteTrayIcon() {
    Shell_NotifyIconW(NIM_DELETE, &nid);

    if (nid.hIcon) {
        DestroyIcon(nid.hIcon);
        nid.hIcon = NULL;
    }
}

// Window procedure to handle messages
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            AddTrayIcon(hWnd, ((LPCREATESTRUCT)lParam)->hInstance);
            break;

        case WM_DESTROY:
            keepRunning = false;
            DeleteTrayIcon();
            PostQuitMessage(0);
            break;

        case WM_SYSICON:
            if (lParam == WM_RBUTTONUP) {
                POINT curPoint;
                GetCursorPos(&curPoint);
                HMENU hPopMenu = CreatePopupMenu();
                InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"Exit");
                SetMenuDefaultItem(hPopMenu, ID_TRAY_EXIT, FALSE);
                SetForegroundWindow(hWnd);
                TrackPopupMenu(hPopMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
                DestroyMenu(hPopMenu);
                PostMessage(hWnd, WM_NULL, 0, 0);
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            }
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    // Define and register window class
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.lpszClassName = L"CursorCloakClass";
    RegisterClassExW(&wc);

    // Create a hidden window
    HWND hWnd = CreateWindowExW(0, wc.lpszClassName, L"Cursor Cloak", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBoxW(NULL, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Load cursors
    HCURSOR defaultCursor = LoadCursorFromFileW(L"cursors\\aero_arrow.cur");
    if (!defaultCursor) {
        MessageBoxW(NULL, L"Failed to load aero_arrow.cur", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR defaultLinkCursor = LoadCursorFromFileW(L"cursors\\aero_link.cur");
    if (!defaultLinkCursor) {
        MessageBoxW(NULL, L"Failed to load aero_link.cur", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR defaultTextCursor = LoadCursorFromFileW(L"cursors\\beam_i.cur");
    if (!defaultTextCursor) {
        MessageBoxW(NULL, L"Failed to load beam_i.cur", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR defaultBusyCursor = LoadCursorFromFileW(L"cursors\\aero_busy.ani");
    if (!defaultBusyCursor) {
        MessageBoxW(NULL, L"Failed to load aero_busy.ani", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR defaultAppStartingCursor = LoadCursorFromFileW(L"cursors\\aero_working.ani");
    if (!defaultAppStartingCursor) {
        MessageBoxW(NULL, L"Failed to load aero_working.ani", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR hiddenCursor = LoadCursorFromFileW(L"cursors\\cloak.cur");
    if (!hiddenCursor) {
        MessageBoxW(NULL, L"Failed to load cloak.cur", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    HCURSOR hiddenBusyCursor = LoadCursorFromFileW(L"cursors\\cloak.ani");
    if (!hiddenBusyCursor) {
        MessageBoxW(NULL, L"Failed to load cloak.ani", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Set system cursors to custom defaults
    SetSystemCursor(CopyCursor(defaultCursor), OCR_NORMAL);
    SetSystemCursor(CopyCursor(defaultLinkCursor), OCR_HAND);
    SetSystemCursor(CopyCursor(defaultTextCursor), OCR_IBEAM);
    SetSystemCursor(CopyCursor(defaultBusyCursor), OCR_WAIT);
    SetSystemCursor(CopyCursor(defaultAppStartingCursor), OCR_APPSTARTING);

    // Variables for cursor position tracking
    POINT lastPosition;
    GetCursorPos(&lastPosition);
    DWORD lastMovedTime = GetTickCount();

    // Thread to monitor cursor movement
    std::thread cursorThread([&]() {
        while (keepRunning) {
            Sleep(100);
            POINT currentPosition;
            GetCursorPos(&currentPosition);

            if (currentPosition.x != lastPosition.x || currentPosition.y != lastPosition.y) {
                // Cursor has moved; reset system cursors
                SetSystemCursor(CopyCursor(defaultCursor), OCR_NORMAL);
                SetSystemCursor(CopyCursor(defaultLinkCursor), OCR_HAND);
                SetSystemCursor(CopyCursor(defaultTextCursor), OCR_IBEAM);
                SetSystemCursor(CopyCursor(defaultBusyCursor), OCR_WAIT);
                SetSystemCursor(CopyCursor(defaultAppStartingCursor), OCR_APPSTARTING);

                lastPosition = currentPosition;
                lastMovedTime = GetTickCount();
            } else if (GetTickCount() - lastMovedTime > cursorIdleTime) {
                // Cursor is idle; set system cursors to hidden cursor
                SetSystemCursor(CopyCursor(hiddenCursor), OCR_NORMAL);
                SetSystemCursor(CopyCursor(hiddenCursor), OCR_HAND);
                SetSystemCursor(CopyCursor(hiddenCursor), OCR_IBEAM);
                SetSystemCursor(CopyCursor(hiddenBusyCursor), OCR_WAIT);
                SetSystemCursor(CopyCursor(hiddenBusyCursor), OCR_APPSTARTING);
            }
        }
    });

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    keepRunning = false;
    cursorThread.join();

    // Reset system cursors to default
    SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);

    // Destroy loaded cursors
    DestroyCursor(defaultCursor);
    DestroyCursor(defaultLinkCursor);
    DestroyCursor(defaultTextCursor);
    DestroyCursor(defaultBusyCursor);
    DestroyCursor(defaultAppStartingCursor);
    DestroyCursor(hiddenCursor);
    DestroyCursor(hiddenBusyCursor);

    return 0;
}
