#include "../../include/bcrypt/BCrypt.hpp"
#include <windows.h>
#include <string>
#include <fstream>
#include <codecvt>
#include <locale>
#include "json.hpp"

// Global variables
HINSTANCE hInst;
HWND hEdit, hButton1, hDropdown1, hDropdown2, hCopybutton, hEdit1, hEdit2, hEdit3, hSubmitbutton,
hDropdown3, hDropdown4, hDeletebutton, hDropdown5, hDropdown6, hPassword, hUpdatebutton;

int parentWidth;
int parentHeight;
int xPos;
int yPos;

std::string line;
const char* c = line.c_str();

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Function to copy text to the clipboard
void CopyToClipboard(const char* text) {
    // Open the clipboard
    if (OpenClipboard(NULL)) {
        // Empty the clipboard before setting new data 
        EmptyClipboard();

        // Allocate global memory for the text to be copied
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, strlen(text) + 1);
        if (hGlobal) {
            // Lock the global memory to get a pointer to the data
            char* pText = (char*)GlobalLock(hGlobal);
            if (pText) {
                // Copy the text to the allocated memory
                strcpy_s(pText, strlen(text) + 1, text);  // +1 for the null-terminator

                // Set the clipboard data with the allocated global memory
                SetClipboardData(CF_TEXT, hGlobal);
            }
            GlobalUnlock(hGlobal);
        }

        // Close the clipboard
        CloseClipboard();
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Ratcave";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, "Window registration failed!", "Error", MB_ICONERROR);
        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindowEx(
        0,
        "Ratcave",
        "Ratcave",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd) {
        MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hEdit = CreateWindow(
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD,
            0, 0, 200, 25,
            hWnd,
            NULL,
            hInst,
            NULL
        );

        hButton1 = CreateWindow(
            "BUTTON",
            "Login",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            0, 0, 75, 25,
            hWnd,
            (HMENU)1,
            hInst,
            NULL
        );
        // Center the button in the parent window
        RECT clientRect;
        GetClientRect(hWnd, &clientRect); // Get the dimensions of the parent window

        parentWidth = clientRect.right - clientRect.left;
        parentHeight = clientRect.bottom - clientRect.top;

        xPos = (parentWidth - 285) / 2;
        yPos = (parentHeight - 25) / 2;

        MoveWindow(hEdit, xPos, yPos, 200, 25, TRUE);
        MoveWindow(hButton1, xPos + 210, yPos, 75, 25, TRUE);
        break;

    case WM_SIZE:
        // center it agian if window is resized
        GetClientRect(hWnd, &clientRect);

        parentWidth = clientRect.right - clientRect.left;
        parentHeight = clientRect.bottom - clientRect.top;

        xPos = (parentWidth - 285) / 2;
        yPos = (parentHeight - 25) / 2;

        MoveWindow(hEdit, xPos, yPos, 200, 25, TRUE);
        MoveWindow(hButton1, xPos + 210, yPos, 75, 25, TRUE);
        break;

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);  // Make the text background transparent
        return (LRESULT)GetStockObject(NULL_BRUSH); // Remove default background
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Button clicked

            wchar_t buffer[100]; // Wide-character buffer
            GetWindowTextW(hEdit, buffer, 100);
            std::wstring inputW(buffer); // Convert to std::wstring

            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string input = converter.to_bytes(inputW);

            std::string hash = "$2a$12$sh1dAADYPiK.9MVYfz3pteR3ioWKMrodR7fKmRWTAnt8x1P4WMCTq"; // this is the hash for "password"

            if (!(BCrypt::validatePassword(input, hash))) { // Compare with wide string literal
                MessageBox(hWnd, "You've been ratted.", "Incorrect Password", MB_OK | MB_ICONERROR); // Unicode strings for message box
            }
            else {

                // Maximize the window and disable minimize & resize options
                ShowWindow(hWnd, SW_MAXIMIZE);
                LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
                style &= ~(WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX); // Disable minimize, maximize, and resizing
                SetWindowLongPtr(hWnd, GWL_STYLE, style);
                SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

                GetClientRect(hWnd, &clientRect);

                parentWidth = clientRect.right - clientRect.left;
                parentHeight = clientRect.bottom - clientRect.top;

                xPos = parentWidth - 155;

                HWND hLabel1 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Platform:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned, no border)
                    clientRect.left + 20, clientRect.top + 80,  // Position above dropdown
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                hDropdown1 = CreateWindow(
                    "COMBOBOX",
                    "",
                    CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
                    clientRect.left+20, clientRect.top+100, xPos/2, 300,
                    hWnd,
                    (HMENU)4,
                    hInst,
                    NULL);

                HWND hLabel2 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Username:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 2 + 20, clientRect.top + 80,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hDropdown2 = CreateWindow(
                    "COMBOBOX",
                    "",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                    clientRect.left + 20+xPos/2+20, clientRect.top + 100, xPos/2, 300,
                    hWnd,
                    (HMENU)5,
                    hInst,
                    NULL);

                hCopybutton = CreateWindow(
                    "BUTTON",
                    "Copy",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20+20, clientRect.top + 100, 75, 25,
                    hWnd,
                    (HMENU)2,
                    hInst,
                    NULL);

                HWND hLabel6 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "KILL A RAT:\nPlatform",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned, no border)
                    clientRect.left + 20, clientRect.top + 420,  // Position above dropdown
                    100, 40,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                hDropdown3 = CreateWindow(
                    "COMBOBOX",
                    "",
                    CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
                    clientRect.left + 20, clientRect.top + 460, xPos / 2, 300,
                    hWnd,
                    (HMENU)4,
                    hInst,
                    NULL);

                HWND hLabel7 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Username:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 2 + 20, clientRect.top + 440,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hDropdown4 = CreateWindow(
                    "COMBOBOX",
                    "",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                    clientRect.left + 20 + xPos / 2 + 20, clientRect.top + 460, xPos / 2, 300,
                    hWnd,
                    (HMENU)5,
                    hInst,
                    NULL);

                hDeletebutton = CreateWindow(
                    "BUTTON",
                    "Delete",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20 + 20, clientRect.top + 460, 75, 25,
                    hWnd,
                    (HMENU)2,
                    hInst,
                    NULL);

                xPos = parentWidth - 175;

                HWND hLabel3 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "ADD A RAT:\nPlatform:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20, clientRect.top + 340,  // Position (above dropdown)
                    100, 40,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hEdit1 = CreateWindow(
                    "EDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    clientRect.left + 20, clientRect.top + 380, xPos / 3, 25,
                    hWnd,
                    NULL,
                    hInst,
                    NULL);
                
                HWND hLabel4 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Username:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 3 + 20, clientRect.top + 360,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hEdit2 = CreateWindow(
                    "EDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    clientRect.left + 20+xPos/3+20, clientRect.top + 380, xPos / 3, 25,
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                HWND hLabel5 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Password:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 3 + 20 + xPos / 3 + 20, clientRect.top + 360,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hEdit3 = CreateWindow(
                    "EDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    clientRect.left + 20 + xPos / 3 + 20+xPos/3+20, clientRect.top + 380, xPos / 3, 25,
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                hSubmitbutton = CreateWindow(
                    "BUTTON",
                    "Add",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20 + 20+20, clientRect.top + 380, 75, 25,
                    hWnd, 
                    (HMENU)6,
                    hInst,
                    NULL);

                HWND hLabel8 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "FIX A RAT:\nPlatform",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned, no border)
                    clientRect.left + 20, clientRect.top + 500,  // Position above dropdown
                    100, 40,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                hDropdown5 = CreateWindow(
                    "COMBOBOX",
                    "",
                    CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
                    clientRect.left + 20, clientRect.top + 540, xPos / 3, 200,
                    hWnd,
                    (HMENU)4,
                    hInst,
                    NULL);

                HWND hLabel9 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "Username:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 2 + 20, clientRect.top + 520,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hDropdown6 = CreateWindow(
                    "COMBOBOX",
                    "",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                    clientRect.left + 20 + xPos / 3 + 20, clientRect.top + 540, xPos / 3, 200,
                    hWnd,
                    (HMENU)5,
                    hInst,
                    NULL);

                HWND hLabel10 = CreateWindow(
                    "STATIC",             // Window class for static text
                    "New Password:",          // Text to display
                    WS_CHILD | WS_VISIBLE | SS_LEFT, // Styles (visible, left-aligned)
                    clientRect.left + 20 + xPos / 3 + 20 + xPos / 3 + 20, clientRect.top + 520,  // Position (above dropdown)
                    100, 20,              // Width and height
                    hWnd,
                    NULL,
                    hInst,
                    NULL
                );

                hPassword = CreateWindow(
                    "EDIT",
                    "",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    clientRect.left + 20 + xPos / 3 + 20 + xPos / 3 + 20, clientRect.top + 540, xPos / 3, 25,
                    hWnd,
                    NULL,
                    hInst,
                    NULL);

                hUpdatebutton = CreateWindow(
                    "BUTTON",
                    "Update",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20 + 20 + 20, clientRect.top + 540, 75, 25,
                    hWnd,
                    (HMENU)2,
                    hInst,
                    NULL);

                TCHAR Planets[9][10] =
                {
                    TEXT("Mercury"), TEXT("Venus"), TEXT("Terra"), TEXT("Mars"),
                    TEXT("Jupiter"), TEXT("Saturn"), TEXT("Uranus"), TEXT("Neptune"),
                    TEXT("Pluto??")
                };

                TCHAR A[16];
                int  k = 0;

                memset(&A, 0, sizeof(A));
                for (k = 0; k <= 8; k += 1)
                {
                    strcpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Planets[k]);

                    // Add string to combobox.
                    SendMessage(hDropdown1, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
                }

                SendMessage(hDropdown2, CB_ADDSTRING, 0, (LPARAM)"Choice A");
                SendMessage(hDropdown2, CB_ADDSTRING, 0, (LPARAM)"Choice B");
                SendMessage(hDropdown2, CB_ADDSTRING, 0, (LPARAM)"Choice C");

                DestroyWindow(hEdit);  // Destroy the text box
                DestroyWindow(hButton1); // Destroy the button
            }
        }

        if (LOWORD(wParam) == 2)
        {
            const std::string fileName = "example.txt";
            std::ofstream outFile(fileName, std::ios::app);

            if (outFile) {
                // Write data to the file
                outFile << "randi" << std::endl;

                // Close the file after writing
                outFile.close();
            }
            //DestroyWindow(hButton2);

            /*hButton3 = CreateWindow(
                "BUTTON",  // Predefined class; Unicode assumed 
                "copy",      // Button text 
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                xPos + 105,         // x position 
                yPos,         // y position 
                75,        // Button width
                25,        // Button height
                hWnd,     // Parent window
                (HMENU)3,       // No menu.
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL);                   // Additional parameters*/
        }

        if (LOWORD(wParam) == 3)
        {
            const std::string fileName = "example.txt";
            std::ifstream inFile(fileName);

            if (inFile) {
                // File exists, read and print its contents
                getline(inFile, line);
                inFile.close();
            }
            CopyToClipboard(c);

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}