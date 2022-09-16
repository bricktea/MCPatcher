#include "windows.h"
#include <iostream>
#include <shobjidl.h>
#include <fstream>

#include "patcher.h"
#include "utils.h"

using std::endl;
using std::cout;

#define FAIL_CANNOT_OPEN_FILE (-1)
#define FAIL_CANNOT_READ_FILE (-2)
#define FAIL_CANNOT_FIND_BYTE (-3)
#define FAIL_CURRENT_PLATFORM_NO_PATCH (-4)
#define FAIL_BACKUP (-5)

int main() {

    // Add known patches;

    MCPatcher::registerPatch(
            Platform::Win10,
            "PV1193025-1403A4F20",
            {
                {
                    { 0x10, 0x84, 0xC0, 0x74, 0x15, 0xB0, /*O*/0x01, 0x48, 0x8B, 0x4C, 0x24, 0x30, 0x48, 0x33, 0xCC },
                    { 0x10, 0x84, 0xC0, 0x74, 0x15, 0xB0, /*N*/0x00, 0x48, 0x8B, 0x4C, 0x24, 0x30, 0x48, 0x33, 0xCC }
                },
                {
                    { 0x48, 0x83, 0xC3, 0x10, 0x48, 0x3B, 0xDF, 0x75, 0xEA, 0xB0, /*O*/0x01, 0x48, 0x8B, 0x7C, 0x24 },
                    { 0x48, 0x83, 0xC3, 0x10, 0x48, 0x3B, 0xDF, 0x75, 0xEA, 0xB0, /*N*/0x00, 0x48, 0x8B, 0x7C, 0x24 }
                }
            }
            );

    // Ask for binary file;

    HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    string strpath;
    if (SUCCEEDED(result))
    {
        cout << "[i] Please open an executable for minecraft. (Minecraft.Windows.exe)" << endl;
        IFileOpenDialog *openFile;
        CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&openFile));
        COMDLG_FILTERSPEC extNames[] =
                {
                        { L"Minecraft", L"*.exe" }
                };
        openFile->SetFileTypes(1,extNames);
        openFile->Show(nullptr);
        IShellItem *pItem;
        result = openFile->GetResult(&pItem);
        if (SUCCEEDED(result))
        {
            PWSTR pPath;
            pItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
            std::wstringstream path;
            path << pPath;
            strpath = wchar2string(path.str().c_str());
            cout << "[i] Selected " << strpath << "." << endl;
            pItem->Release();
        }
        else
        {
            cout << "[x] Open file failed!" << endl;
        }
        openFile->Release();
        CoUninitialize();
    }
    if (strpath.empty())
        return FAIL_CANNOT_OPEN_FILE;

    // Open file;

    if (!MCPatcher::open(strpath))
    {
        cout << "[x] Can't read executable file!" << endl;
        return FAIL_CANNOT_READ_FILE;
    }
    else
    {
        std::ofstream ofs(strpath + ".bak",ios::binary);
        ofs << MCPatcher::getImage().rdbuf();
        if (ofs.good())
            cout << "[i] Backup created to: " << strpath + ".bak" << endl;
        else
        {
            cout << "[x] Fail to create backup!" << endl;
            return FAIL_BACKUP;
        }
        ofs.close();
    }

    // Select platform;

    auto platform = Platform::Win10;
    auto patches = MCPatcher::patches[platform];
    if (!patches.empty())
    {
        cout << "[x] There are no patches available for the current platform." << endl;
        return FAIL_CURRENT_PLATFORM_NO_PATCH;
    }

    // Do patch;

    cout << "[i] Looking for bytes..." << endl;
    if (MCPatcher::tryApply(platform))
        cout << "[i] Patch successfully." << endl;
    else
    {
        cout << "[x] Failed, if it is the latest version, please send issue." << endl;
        return FAIL_CANNOT_FIND_BYTE;
    }

    MCPatcher::close();

    return 0;
}