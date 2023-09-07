#include <TlHelp32.h>
#include <Windows.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    std::string targetPath;
    {
        fs::path _targetPath;
        std::stringstream ss;
        for (int i = 1; i < argc; ++i) {
            ss << argv[i] << " ";
        }
        _targetPath = ss.str();
        if (!fs::is_directory(_targetPath)) {
            std::cerr << _targetPath << ":is not a valid folder" << std::endl;
            std::exit(-1);
        }
        targetPath = fs::absolute(_targetPath).string();
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "failed to create snapshot" << std::endl;
        std::exit(-1);
    }

    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);

    DWORD parentPid = -1;
    DWORD currentPid = GetCurrentProcessId();

    while (Process32Next(hSnap, &pe)) {
        if (pe.th32ProcessID == currentPid) {
            parentPid = pe.th32ParentProcessID;
            break;
        }
    }

    CloseHandle(hSnap);

    if (parentPid == -1) {
        std::cerr << "failed to get parent pid" << std::endl;
        std::exit(-1);
    }

    // std::cout << "parent pid: " << parentPid << std::endl;

    HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, false, parentPid);
    if (hParent == INVALID_HANDLE_VALUE) {
        std::cerr << "getting handle to process failed" << std::endl;
        std::exit(-1);
    }

    void* remoteAddr = VirtualAllocEx(hParent, nullptr, targetPath.size(),
                                      MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    size_t written = -1;
    WriteProcessMemory(hParent, remoteAddr, targetPath.c_str(),
                       targetPath.size(), &written);
    if (written != targetPath.size()) {
        std::cerr << "warn: " << written << " bytes written of "
                  << targetPath.size() << " bytes" << std::endl;
    }

    DWORD threadId;
    HANDLE hThread = CreateRemoteThread(
        hParent, NULL, 0, (LPTHREAD_START_ROUTINE)SetCurrentDirectoryA,
        remoteAddr, 0, &threadId);
    // std::cout << "created thread: " << threadId << std::endl;
    WaitForSingleObject(hThread, INFINITE);

    DWORD exit = INT_MAX;
    GetExitCodeThread(hThread, &exit);
    VirtualFreeEx(hThread, remoteAddr, targetPath.size(), MEM_RELEASE);

    if (exit == 0)
        std::cout << "[SetDirectory]thread failed with: " << exit << std::endl;

    // Get last error
    // hThread = CreateRemoteThread(hParent, NULL, 0,
    // (LPTHREAD_START_ROUTINE)GetLastError, NULL, 0, &threadId);
    // WaitForSingleObject(hThread, INFINITE);
    // GetExitCodeThread(hThread, &exit);
    //
    // if (exit != 0)
    //         std::cout << "GetLastError(): " << exit << std::endl;

    CloseHandle(hParent);

    return 0;
}
