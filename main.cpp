#include <shobjidl.h>
#include <fstream>

#include "patcher.h"
#include "utils.h"
#include "logger.h"

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

    string strpath;
    if (argc == 1)
    {
        HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(result))
        {
            Info("Please open an executable for minecraft. (Minecraft.Windows.exe)")
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
                Info("Selected {}.",strpath)
                pItem->Release();
            }
            else
            {
                Error("Open file failed!")
            }
            openFile->Release();
            CoUninitialize();
        }
        if (strpath.empty())
            return FAIL_CANNOT_OPEN_FILE;
    }
    else if (argc == 2)
    {
        strpath = argv[1];
    }
    else
    {
        Error("Wrong parameter!")
    }

    // Open file;

    if (!MCPatcher::open(strpath))
    {
        Error("Can't read executable file!")
        return FAIL_CANNOT_READ_FILE;
    }
    else
    {
        std::ofstream ofs(strpath + ".bak",ios::binary);
        ofs << MCPatcher::getImage().rdbuf();
        if (ofs.good())
        {
            Info("Backup created to: {}.bak",strpath)
        }
        else
        {
            Error("Fail to create backup!")
            return FAIL_BACKUP;
        }
        ofs.close();
    }

    // Select platform;

    auto platform = Platform::Win10;
    auto patches = MCPatcher::patches[platform];
    if (!patches.empty())
    {
        Error("There are no patches available for the current platform.")
        return FAIL_CURRENT_PLATFORM_NO_PATCH;
    }

    // Do patch;

    Info("Looking for bytes...")
    if (MCPatcher::tryApply(platform))
    {
        Info("Patch successfully.")
    }
    else
    {
        Error("Failed, if it is the latest version, please send issue.")
        return FAIL_CANNOT_FIND_BYTE;
    }

    MCPatcher::close();

    return 0;
}