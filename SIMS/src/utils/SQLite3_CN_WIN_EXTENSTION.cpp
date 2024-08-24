#include "SQLite3_CN_WIN_EXTENSTION.h"

std::string GBKToUTF8(const std::string& gbkStr)
{
    // 将 GBK 转换为宽字符（UTF-16）
    int wcharLen = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), static_cast<int>(gbkStr.length()), nullptr, 0);
    if (wcharLen <= 0) {
        return ""; // 转换失败
    }

    std::wstring wstr(wcharLen, 0);
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), static_cast<int>(gbkStr.length()), &wstr[0], wcharLen);

    // 将宽字符（UTF-16）转换为 UTF-8
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) {
        return ""; // 转换失败
    }

    std::string utf8Str(utf8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &utf8Str[0], utf8Len, nullptr, nullptr);

    return utf8Str;
}

std::string UTF8ToGBK(const std::string& utf8Str)
{
    // 将 UTF-8 转换为宽字符（UTF-16）
    int wcharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), static_cast<int>(utf8Str.length()), nullptr, 0);
    if (wcharLen <= 0) {
        return ""; // 转换失败
    }

    std::wstring wstr(wcharLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), static_cast<int>(utf8Str.length()), &wstr[0], wcharLen);

    // 将宽字符（UTF-16）转换为 GBK
    int gbkLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
    if (gbkLen <= 0) {
        return ""; // 转换失败
    }

    std::string gbkStr(gbkLen, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.length()), &gbkStr[0], gbkLen, nullptr, nullptr);

    return gbkStr;
}
