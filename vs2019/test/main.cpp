#include "../../include/bcrypt/BCrypt.hpp"
#include <windows.h>
#include <string>
#include <fstream>
#include <codecvt>
#include <locale>
#include "json.hpp"

using json = nlohmann::json;

// Global variables
HINSTANCE hInst;
HWND hEdit, hButton1, hDropdown1, hDropdown2, hCopybutton, hEdit1, hEdit2, hEdit3, hSubmitbutton,
hDropdown3, hDropdown4, hDeletebutton, hDropdown5, hDropdown6, hPassword, hUpdatebutton;

int parentWidth;
int parentHeight;
int xPos;
int yPos;

char platform[256]; // Adjust size as needed
char username[256];
char password[256];

std::string line;
const char* c = line.c_str();

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// Function to clear password
void secureClear(void* ptr, size_t size) {
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (size--) *p++ = 0;
}

    // Function to copy text to the clipboard
void CopyToClipboard(const std::string& text) {
    if (OpenClipboard(NULL)) {
        EmptyClipboard();

        // Allocate global memory for the text
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (hGlobal) {
            // Lock the memory and copy the text
            char* pText = static_cast<char*>(GlobalLock(hGlobal));
            if (pText) {
                strcpy_s(pText, text.size() + 1, text.c_str());  // Copy string to memory
                GlobalUnlock(hGlobal);

                // Set clipboard data
                SetClipboardData(CF_TEXT, hGlobal);
            }
        }

        CloseClipboard();
    }
}


void getPlatform(HWND hComboBox) {
    std::ifstream inFile("vault.json");
    if (!inFile) {
        MessageBox(NULL, "Failed to open vault.json", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (inFile.peek() == std::ifstream::traits_type::eof()) {
        EnableWindow(hComboBox, FALSE);  // Disable dropdown if empty
        return;
    }
    else {
        json jsonObj = json::parse(inFile);  // Read the JSON
        inFile.close();

        // Clear existing items in the ComboBox
        SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);

        // Iterate through JSON object and add keys (platforms) to the ComboBox
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)it.key().c_str());
        }
    }
}

void getUsername(HWND hComboBox,const std::string& selectedPlatform) {
    std::ifstream inFile("vault.json");
    if (!inFile) {
        MessageBox(NULL, "Failed to open vault.json", "Error", MB_OK | MB_ICONERROR);
        EnableWindow(hComboBox, FALSE);
        return;
    }

    if (inFile.peek() == std::ifstream::traits_type::eof()) {
        EnableWindow(hComboBox, FALSE);  // Disable dropdown if empty
        return;
    }
    else {
        json jsonObj = json::parse(inFile);
        inFile.close();

        SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);

        if (jsonObj[selectedPlatform].empty()) {
            EnableWindow(hComboBox, FALSE);
        }
        else {
            for (auto it = jsonObj[selectedPlatform].begin(); it != jsonObj[selectedPlatform].end(); ++it) {
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)it.key().c_str());
            }
        }
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
                    (HMENU)2,
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
                    (HMENU)3,
                    hInst,
                    NULL);

                hCopybutton = CreateWindow(
                    "BUTTON",
                    "Copy",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20+20, clientRect.top + 100, 75, 25,
                    hWnd,
                    (HMENU)4,
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
                    (HMENU)5,
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
                    (HMENU)6,
                    hInst,
                    NULL);

                hDeletebutton = CreateWindow(
                    "BUTTON",
                    "Delete",
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    clientRect.left + 20 + xPos + 20 + 20, clientRect.top + 460, 75, 25,
                    hWnd,
                    (HMENU)7,
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
                    (HMENU)8,
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
                    (HMENU)9,
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
                    (HMENU)10,
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
                    (HMENU)11,
                    hInst,
                    NULL);

                getPlatform(hDropdown1);
                EnableWindow(hDropdown2, FALSE);
                
                getPlatform(hDropdown3);
                EnableWindow(hDropdown4, FALSE);

                DestroyWindow(hEdit);  // Destroy the text box
                DestroyWindow(hButton1); // Destroy the button
            }
        }

        if (LOWORD(wParam) == 2 && HIWORD(wParam) == CBN_SELCHANGE) {
            EnableWindow(hDropdown2, TRUE);

            char selectedPlatform[256] = { 0 };
            int selectedIndex = SendMessage(hDropdown1, CB_GETCURSEL, 0, 0);

            // Retrieve the selected platform name from the dropdown
            SendMessage(hDropdown1, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedPlatform);
            selectedPlatform[255] = '\0';  // Null-terminate the string

            std::string platformStr = selectedPlatform;
            getUsername(hDropdown2, platformStr);
        }

        if (LOWORD(wParam) == 5 && HIWORD(wParam) == CBN_SELCHANGE) {
            EnableWindow(hDropdown4, TRUE);

            char selectedPlatform[256] = { 0 };
            int selectedIndex = SendMessage(hDropdown3, CB_GETCURSEL, 0, 0);

            // Retrieve the selected platform name from the dropdown
            SendMessage(hDropdown3, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedPlatform);
            selectedPlatform[255] = '\0';  // Null-terminate the string

            std::string platformStr = selectedPlatform;
            getUsername(hDropdown4, platformStr);
        }

        if (LOWORD(wParam) == 4) {
            char selectedPlatform[256] = { 0 };
            char selectedUsername[256] = { 0 };
            int selectedIndex = SendMessage(hDropdown1, CB_GETCURSEL, 0, 0);
            int selectedIndex1 = SendMessage(hDropdown2, CB_GETCURSEL, 0, 0);
            SendMessage(hDropdown1, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedPlatform);
            SendMessage(hDropdown2, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedUsername);

            std::string platformStr = selectedPlatform; // platformStr still has platform
            std::string usernameStr = selectedUsername; // platformStr still has platform

            std::ifstream innFile("vault.json");
            if (!innFile) {
                MessageBox(NULL, "Failed to open vault.json", "Error", MB_OK | MB_ICONERROR);
            }
            else {
                json jsonObjj = json::parse(innFile);  // Read the JSON

                std::string passwordd = jsonObjj[platformStr][usernameStr];
                CopyToClipboard(passwordd);
            }
        }

        if (LOWORD(wParam) == 8)
        {
            GetWindowText(hEdit1, platform, sizeof(platform));

            GetWindowText(hEdit2, username, sizeof(username));

            GetWindowText(hEdit3, password, sizeof(password));

            std::ifstream inFile("vault.json");

            json jsonObj;
            
            if (inFile.peek() == std::ifstream::traits_type::eof()) {
                jsonObj = json::object();  // Start with an empty JSON object
                inFile.close();
            }
            else {
                jsonObj = json::parse(inFile);
            }

            jsonObj[platform][username] = password;

            std::ofstream outFile("vault.json");

            if (!outFile) {
                MessageBox(hWnd, "File failed to open!", "Error", MB_OK | MB_ICONERROR);
            }
            else {
                outFile << jsonObj.dump();

                // Close the file
                outFile.close();

                EnableWindow(hDropdown1, TRUE);
                getPlatform(hDropdown1);
                EnableWindow(hDropdown2, FALSE);

                EnableWindow(hDropdown3, TRUE);
                getPlatform(hDropdown3);
                EnableWindow(hDropdown4, FALSE);
            }
        }

        if (LOWORD(wParam) == 7) {
            std::ifstream inFile("vault.json");
            if (inFile.peek() == std::ifstream::traits_type::eof()) {
                MessageBox(hWnd, "Empty file.", "Error", MB_OK | MB_ICONERROR);
                inFile.close();
            }
            else {
                json jsonObj = json::parse(inFile);
            }
        }

        if (LOWORD(wParam) == 67) {
            MessageBox(hWnd, "gawk", "Error", MB_OK | MB_ICONERROR);
        }

        if (LOWORD(wParam) == 23)
        {
            const std::string fileName = "example.txt";
            std::ifstream inFile(fileName);

            if (inFile) {
                // File exists, read and print its contents
                getline(inFile, line);
                inFile.close();
            }
            CopyToClipboard(c);
            
            /*const std::string fileName = "example.txt";
            std::ofstream outFile(fileName, std::ios::app);

            if (outFile) {
                // Write data to the file
                outFile << "randi" << std::endl;

                // Close the file after writing
                outFile.close();
            }
            //DestroyWindow(hButton2);

            hButton3 = CreateWindow(
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

        if (LOWORD(wParam) == 55)
        {
            
            const std::string fileName = "example.txt";
            std::ifstream inFile(fileName);

            if (inFile) {
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