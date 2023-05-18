#include <shobjidl.h>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

#include "patcher.h"
#include "utils.h"

constexpr unsigned int FAIL_CANNOT_OPEN_FILE = (0x1001);
constexpr unsigned int FAIL_CANNOT_READ_FILE = (0x1002);
constexpr unsigned int FAIL_CANNOT_FIND_BYTE = (0x1003);
constexpr unsigned int FAIL_CURRENT_PLATFORM_NO_PATCH = (0x1004);
constexpr unsigned int FAIL_BACKUP = (0x1005);

#define VERSION "1.2.0"

int main(int argc, char *argv[]) {

    // Welcome message.

    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
    spdlog::info("MCPatcher v{}, Repository: github.com/Redbeanw44602/MCPatcher [MIT]", VERSION);

    // Add known patch(es);

    MCPatcher patcher;

    auto generalPatch = MCPatcher::compile(
            "48 8B 42 08 48 8B 88 80 01 00 00 48 85 C9 74 07 E8 ?? ?? ?? 00 "
            "EB 04 0F B6 42 10 84 C0 74 ?? B0 01(00) 48 8B 4C 24 ?? 48 33 CC");

    patcher.registerPatch(Platform::Win10, "General_Patch_V2", generalPatch);

    // Ask for binary file;

    string strpath;
    if (argc == 1) {
        HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(result)) {
            spdlog::info("Please open an executable for minecraft. (Minecraft.Windows.exe)");
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
            if (SUCCEEDED(result)) {
                PWSTR pPath;
                pItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
                std::wstringstream path;
                path << pPath;
                strpath = wchar2string(path.str().c_str());
                spdlog::info("Selected {}.",strpath);
                pItem->Release();
            } else {
                spdlog::error("Open file failed!");
            }
            openFile->Release();
            CoUninitialize();
        }
        if (strpath.empty())
            return FAIL_CANNOT_OPEN_FILE;
    } else if (argc == 2) {
        strpath = argv[1];
    } else {
        spdlog::error("Wrong parameter!");
    }

    // Open file;

    if (!patcher.target(strpath)) {
        spdlog::error("Can't read executable file!");
        return FAIL_CANNOT_READ_FILE;
    } else {
        std::ofstream ofs(strpath + ".bak", std::ios::binary);
        ofs << patcher.getImage().rdbuf();
        if (ofs.good()) {
            spdlog::info("Backup created to: {}.bak",strpath);
        } else {
            spdlog::error("Fail to create backup!");
            return FAIL_BACKUP;
        }
        ofs.close();
    }

    // Select platform;

    auto platform = Platform::Win10;
    auto patches = patcher.getPatches(platform);
    if (patches.empty()) {
        spdlog::error("There are no patches available for the current platform.");
        return FAIL_CURRENT_PLATFORM_NO_PATCH;
    }

    // Do patch;

    spdlog::info("Looking for bytes...");
    if (patcher.apply(platform)) {
        spdlog::info("Patch successfully.");
    } else {
        spdlog::error("Failed, if it is the latest version, please send issue.");
        return FAIL_CANNOT_FIND_BYTE;
    }

    return 0;
}