#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "resource/resource.h"
#include <commctrl.h>

// Game state
int playerX = 250;     // Player's position
int playerWidth = 50;  // Player's width
int playerY = 300;   
int blockX, blockY;    // Falling block's position
int blockSize = 30;    // Block size
int playerHeight = 20;
int score = 0;         // Player's score
int isRunning = 1;

HWND hStatusBar;


void ResetGame() {
    playerX = 250;
    playerY = 300;
    blockX = rand() % 470;
    blockY = 0;
    score = 0;
    InvalidateRect(NULL, NULL, TRUE);
}

#define NEW_GAM 132
#define EXIT 128
#define ABOUT 256
#define HELP_TOPICS 260

// Function to handle window messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            // Create the status bar
            hStatusBar = CreateWindowEx(
                0, STATUSCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                0, 0, 0, 0,
                hwnd, (HMENU)1, GetModuleHandle(NULL), NULL
            );
            HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
            if (hIcon) {
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            }

            return 0;
        case WM_SIZE: {
            // Adjust the status bar size
            SendMessage(hStatusBar, WM_SIZE, 0, 0);
        
            // Get the height of the status bar
            RECT rcStatus;
            GetWindowRect(hStatusBar, &rcStatus);
            int statusBarHeight = rcStatus.bottom - rcStatus.top;
        
            // Adjust the client area to account for the status bar
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            rcClient.bottom -= statusBarHeight;
            SetWindowPos(hStatusBar, NULL, 0, rcClient.bottom, rcClient.right, statusBarHeight, SWP_NOZORDER);
        
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_ACTIVATE: {
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_MOVE: {
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Set background color to gray
            HBRUSH grayBrush = CreateSolidBrush(RGB(128, 128, 128));
            FillRect(hdc, &ps.rcPaint, grayBrush);
            DeleteObject(grayBrush);

            // Draw falling block in red
            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
            RECT blockRect = {blockX, blockY, blockX + blockSize, blockY + blockSize};
            FillRect(hdc, &blockRect, redBrush);
            DeleteObject(redBrush);

            // Draw the outline of the falling block in black
            HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
            FrameRect(hdc, &blockRect, blackBrush);
            DeleteObject(blackBrush); // Clean up the brush after use

            // Draw player block
            HBRUSH brownBrush = CreateSolidBrush(RGB(139, 69, 19));
            RECT playerRect = {playerX, playerY, playerX + playerWidth, playerY + playerHeight};
            FillRect(hdc, &playerRect, brownBrush);
            DeleteObject(brownBrush);

            // Draw player block outline in black
            HBRUSH blackBrushOutline = CreateSolidBrush(RGB(0, 0, 0));
            FrameRect(hdc, &playerRect, blackBrushOutline);
            DeleteObject(blackBrushOutline); // Clean up the brush after use

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case EXIT:
                    PostQuitMessage(0);
                    break;
                case ABOUT:
                    MessageBox(hwnd, "the finest of scratch game ever\nmade by boinkwer to catch", "abaut catch ze block", MB_OK | MB_ICONINFORMATION);
                    break;
                case NEW_GAM:
                    ResetGame();
                    break;
                case HELP_TOPICS:
                    {
                        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(INT_HELPCHM), RT_RCDATA);
                        if (hResInfo) {
                            HGLOBAL hRes = LoadResource(NULL, hResInfo);
                            if (hRes) {
                                DWORD resSize = SizeofResource(NULL, hResInfo);
                                LPVOID pResData = LockResource(hRes);

                                if (pResData) {
                                    TCHAR tempPath[MAX_PATH];
                                    GetTempPath(MAX_PATH, tempPath);
                                    TCHAR tempFile[MAX_PATH];
                                    GetTempFileName(tempPath, TEXT("CHM"), 0, tempFile);

                                    HANDLE hFile = CreateFile(tempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
                                    if (hFile != INVALID_HANDLE_VALUE) {
                                        DWORD bytesWritten;
                                        WriteFile(hFile, pResData, resSize, &bytesWritten, NULL);
                                        CloseHandle(hFile);

                                        // Rename .tmp to .chm
                                        TCHAR newFilePath[MAX_PATH];
                                        wsprintf(newFilePath, TEXT("%s.chm"), tempFile);
                                        if (MoveFile(tempFile, newFilePath)) {
                                            // Open the CHM file
                                            ShellExecute(hwnd, TEXT("open"), newFilePath, NULL, NULL, SW_SHOWNORMAL);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;

            }
            break;
        case WM_DESTROY:
            isRunning = 0;  // Set the flag to 0 when the window is destroyed
            PostQuitMessage(0);
            return 0;
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Main game loop and initialization
int main() {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Set up window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "CatchGame";
    wc.hCursor = LoadCursor(wc.hInstance, MAKEINTRESOURCE(IDI_CURSOR1)); // Load custom cursor
    RegisterClass(&wc);

    HMENU menu = CreateMenu();
    HMENU gam = CreateMenu();
    HMENU help = CreateMenu();
    

    AppendMenu(menu, MF_POPUP, (UINT_PTR)gam, "gam");
    AppendMenu(menu, MF_POPUP, (UINT_PTR)help, "help");

    AppendMenu(gam, MF_STRING, NEW_GAM, "new gam");
    AppendMenu(gam, MF_SEPARATOR, 0, NULL);  // Divider
    AppendMenu(gam, MF_STRING, EXIT, "exitearino");
    AppendMenu(help, MF_STRING, HELP_TOPICS, "help topicals...");
    AppendMenu(help, MF_STRING, ABOUT, "abaut");

    // Create window
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "catch ze block",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        NULL, menu, wc.hInstance, NULL);

    // Initialize the falling block
    blockX = rand() % 470;
    blockY = 0;

    // Show the window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Game loop
    MSG msg;
    while (isRunning) {
        // Process all pending messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                isRunning = 0;  // Set the flag to 0 when WM_QUIT is received
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Update game state
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            if (playerX > 0) {
                playerX -= 5;  // Move left
            }
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            if (playerX < 450) {
                playerX += 5;  // Move right
            }
        }

        blockY += 4.5;  // Make the block fall
        if (blockY > 400) {
            blockY = 0;  // Reset block position when it falls off the screen
            blockX = rand() % 470;  // Randomize x position of falling block
        }

        // Check for collision (catching the block)
        if (blockY + blockSize >= 350 && blockX >= playerX && blockX <= playerX + playerWidth) {
            score++;  // Increase score if caught
            blockY = -blockSize;  // Move block off-screen
            blockX = rand() % 470;
        }

        // Update the status bar with the score
        char scoreText[50];
        sprintf(scoreText, "score: %d", score);
        SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)scoreText);

        RECT gameArea;
        GetClientRect(hwnd, &gameArea);
        RECT rcStatus;
        GetWindowRect(hStatusBar, &rcStatus);
        gameArea.bottom -= (rcStatus.bottom - rcStatus.top);
        InvalidateRect(hwnd, &gameArea, FALSE);
        Sleep(10);
    }

    return 0;
}