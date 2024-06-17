#include <windows.h>
#include <iostream>
#include <string>
#include <ShlObj.h>
#include <unordered_map>  
#include <filesystem>     


std::string GetDefaultAppForProtocol(const std::string& protocol) {
    HKEY hKey;
    std::string regPath = "Software\\Classes\\" + protocol + "\\shell\\open\\command";
    char appName[256];
    DWORD bufferSize = sizeof(appName);
    DWORD dwType = 0;

    if (RegOpenKeyExA(HKEY_CLASSES_ROOT, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, nullptr, NULL, &dwType, (LPBYTE)appName, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            std::string appString(appName);
            if (appString.find("Bloxstrap.exe") != std::string::npos) {
                return "Bloxstrap";
            }
            else if (appString.find("Roblox") != std::string::npos) {
                return "Roblox";
            }
            return appString;
        }
        RegCloseKey(hKey);
    }

    if (RegOpenKeyExA(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, nullptr, NULL, &dwType, (LPBYTE)appName, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            std::string appString(appName);
            if (appString.find("Bloxstrap.exe") != std::string::npos) {
                return "Bloxstrap";
            }
            else if (appString.find("Roblox") != std::string::npos) {
                return "Roblox";
            }
            return appString;
        }
        RegCloseKey(hKey);
    }

    return "Unknown";
}

std::string GetUserAppDataPath(const std::string& defaultApp) {
    char appDataPath[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath) != S_OK) {
        return "";
    }

    std::string fullPath = std::string(appDataPath) + "\\" + defaultApp + "\\Versions\\version-2cca5ed32b534b2a\\content\\fonts";

    DWORD attributes = GetFileAttributesA(fullPath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return ""; 
    }

    return fullPath;
}

void RenameFontFiles(const std::string& basePath) {
    std::unordered_map<std::string, std::string> fileMappings = {
        {"SourceSansPro-Regular.ttf", "Montserrat-Regular.ttf"},
        {"SourceSansPro-Semibold.ttf", "Montserrat-Medium.ttf"},
        {"SourceSansPro-Bold.ttf", "Montserrat-Bold.ttf"}
    };

    for (const auto& mapping : fileMappings) {
        const std::string& sourceName = mapping.first;
        const std::string& targetName = mapping.second;

        std::filesystem::path sourcePath = basePath + "\\" + sourceName;
        std::filesystem::path targetPath = basePath + "\\" + targetName;

        if (std::filesystem::exists(sourcePath)) {
            if (std::filesystem::exists(targetPath)) {
                std::filesystem::remove(targetPath);
                std::cout << "Removed existing file: " << targetName << std::endl;
            }

            try {
                std::filesystem::rename(sourcePath, targetPath);
                std::cout << "File renamed: " << sourceName << " -> " << targetName << std::endl;
            }
            catch (const std::filesystem::filesystem_error& ex) {
                std::cerr << "Error renaming file: " << ex.what() << std::endl;
            }
        }
        else {
            std::cout << "File " << sourceName << " does not exist." << std::endl;
        }
    }
}


int main() {
    std::string protocol = "ROBLOX";
    std::string defaultApp = GetDefaultAppForProtocol(protocol);
    std::cout << "The default apps is " << defaultApp << std::endl;
    std::cout << "Check if the directory exists... " << std::endl;
    std::string result = GetUserAppDataPath(defaultApp);
    if (result == "")
    {
        std::cout << "Uhh... look like we didn't found the folder. Please ask dev about it. " << std::endl;
    }
    else 
    {
        std::cout << "Found Folder! " << result << std::endl;
        std::cout << "Rename files now. " << std::endl;
        RenameFontFiles(result);
    }
    return 0;
}
