#include <windows.h>
#include <lm.h>
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>


#pragma comment(lib, "Netapi32.lib")
#define ID_START_BUTTON 101
#define ID_CHECKBOX     102


std::wstring programName = L"Bulk User Builder ";
std::string baseName = "user";
int _index = 0;
int minIndex = 0;
int maxUsersNum = 100000;
bool isNewUserAdmin = false;
bool start = false;


std::wstring converter(const std::string str, int num) {
    std::stringstream ss;
    ss << str << num;
    std::string combined = ss.str();
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide_combined = converter.from_bytes(combined);
    return wide_combined;
}

bool isUserRunSoftwareUsAdmin() {
    bool status = false;
    HANDLE tokenHandle = nullptr;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
        TOKEN_ELEVATION elevation;
        DWORD returnLength = 0;

        if (GetTokenInformation(tokenHandle, TokenElevation, &elevation, sizeof(elevation), &returnLength)) {
            status = elevation.TokenIsElevated;
        }
        CloseHandle(tokenHandle);
    }
    return status;
}

int GetTotalUsers() {
    DWORD totalEntries = 0;
    LPUSER_INFO_0 userInfo;
    DWORD entriesRead = 0, resumeHandle = 0;
    NET_API_STATUS status;

    do {
        status = NetUserEnum(
            nullptr,
            0,
            FILTER_NORMAL_ACCOUNT,
            (LPBYTE*)&userInfo,
            MAX_PREFERRED_LENGTH,
            &entriesRead,
            &totalEntries,
            &resumeHandle
        );

        if (status == NERR_Success || status == ERROR_MORE_DATA) {
            NetApiBufferFree(userInfo);
        } else {
            return -1;
        }
    } while (status == ERROR_MORE_DATA);

    return totalEntries;
}

bool doesUserExist(const std::wstring& userName) {
    USER_INFO_0* pUserInfo = nullptr;


    NET_API_STATUS status = NetUserGetInfo(NULL, userName.c_str(), 0, (LPBYTE*)&pUserInfo);
    if (status == NERR_Success) {
        NetApiBufferFree(pUserInfo);
        return true;
    }
    return false;
}



int createnewuser(std::string baseName, int userId, bool isUserAdmin) {
    DWORD dwError = 0;

    std::wcout.imbue(std::locale(""));
    std::wstring userName = converter(baseName, userId+minIndex);

    while (doesUserExist(userName)) {
        minIndex++;
        userName = converter(baseName, userId+minIndex);
        std::wcout << userName << std::endl;
    }

    LPCWSTR groupName;
    USER_INFO_1 userInfo = { 0 };
    userInfo.usri1_name = const_cast<LPWSTR>(userName.c_str());
    userInfo.usri1_password = (LPWSTR)L"User";
    userInfo.usri1_priv = USER_PRIV_USER;
    userInfo.usri1_comment = (LPWSTR)L"Created via Windows API";
    userInfo.usri1_flags = UF_NORMAL_ACCOUNT;

    NET_API_STATUS nStatus = NetUserAdd(NULL, 1, (LPBYTE)&userInfo, &dwError);

    if (nStatus == NERR_Success) {
        if (isUserAdmin) {
            groupName = L"Администраторы";
        } else {
            groupName = L"Administrators";
        }

        LOCALGROUP_MEMBERS_INFO_3 lmi;
        lmi.lgrmi3_domainandname = const_cast<LPWSTR>(userName.c_str());

        NET_API_STATUS groupStatus = NetLocalGroupAddMembers(NULL, groupName, 3, (LPBYTE)&lmi, 1);
        if (!groupStatus == NERR_Success) {
            return 1;
        }
    } else {
        return 1;
    }

    return 0;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RunWindow(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASSW wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        programName.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 600,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hCheckbox, hButton, hStaticText, hIndexTitleBox;
    static bool isAdminChecked = false;
    static HWND hIndexBox;


    switch (uMsg) {
        case WM_CREATE:

            hStaticText = CreateWindowW(
                L"STATIC",
                L"Welcome to the Bulk User Creation Tool for Windows.\nEnter the number of users (maximum 100000) then check or uncheck and whether to make these users administrators or not and then click start. Standard password is 'User' This program is only for the English version of Windows.",
                WS_VISIBLE | WS_CHILD,
                0, 0, 400, 600,
                hwnd, NULL, NULL, NULL
            );

            hCheckbox = CreateWindowW(
                L"BUTTON",
                L"Add administrator user",
                WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                10, 400, 180, 20,
                hwnd, (HMENU)ID_CHECKBOX, NULL, NULL
            );

            hIndexTitleBox = CreateWindowW(
                L"STATIC",
                L"Enter the number of users:",
                WS_VISIBLE | WS_CHILD,
                10, 380, 360, 20,
                hwnd, NULL, NULL, NULL
                );

            hIndexBox = CreateWindowW(
                L"EDIT",
                L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
                10, 400, 360, 20,
                hwnd, NULL, NULL, NULL
                );


            hButton = CreateWindowW(
                L"BUTTON",
                L"Start",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                150, 495, 80, 30,
                hwnd, (HMENU)ID_START_BUTTON, NULL, NULL
            );
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_CHECKBOX:
                    if (SendMessageW(hCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        isNewUserAdmin = false;
                        SendMessageW(hCheckbox, BM_SETCHECK, BST_UNCHECKED, 0);

                    } else {
                        isNewUserAdmin = true;
                        SendMessageW(hCheckbox, BM_SETCHECK, BST_CHECKED, 0);
                    }
                    break;

                case ID_START_BUTTON:
                    wchar_t buffer[10] = {0};
                    GetWindowTextW(hIndexBox, buffer, 10);
                    _index = _wtoi(buffer);

                    if (_index <= 0) {
                        MessageBoxW(hwnd, L"Please enter a valid value.", programName.c_str(), MB_OK | MB_ICONERROR);
                        break;
                    } else if (_index > maxUsersNum) {
                        MessageBoxW(hwnd, L"Too large number please enter a number up to 100.000", programName.c_str(), MB_OK | MB_ICONERROR);
                        break;
                    }


                    start = true;
                    PostQuitMessage(0);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    if (!isUserRunSoftwareUsAdmin()) {
        MessageBoxW(
                NULL,
                L"Start software us admin or in this computer your user must be admin",
                programName.c_str(),
                MB_ICONERROR | MB_OK );
        return 0;
    }
    if (GetTotalUsers() > 999) {
        MessageBoxW(
                NULL,
                L"There are already too many users in your system",
                programName.c_str(),
                MB_ICONERROR | MB_OK );
        return 0;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    RunWindow(hInstance);

    if (start) {
        MessageBoxW(
            NULL,
            L"The program has been launched press the OK button to continue",
            programName.c_str(),
            MB_ICONINFORMATION | MB_OK );
        for (int i = 0;i<_index;i++) {
            int status = createnewuser(baseName, i, isNewUserAdmin);
            if (status != 0) {
                if (GetTotalUsers() > maxUsersNum) {
                    MessageBoxW(
                        NULL,
                        L"Some error occurred Try restarting the program.",
                        programName.c_str(),
                        MB_ICONERROR | MB_OK );
                    return 0;
                }
            }
        }
        MessageBoxW(
                NULL,
                L"Done",
                programName.c_str(),
                MB_ICONINFORMATION | MB_OK );
    }
    return 0;
}
