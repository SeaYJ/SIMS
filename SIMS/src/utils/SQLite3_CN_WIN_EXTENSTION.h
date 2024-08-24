#pragma once
#ifndef SQLITE3_CN_WIN_EXTENSTION_H
#define SQLITE3_CN_WIN_EXTENSTION_H

#include <string>
#include <Windows.h>

std::string GBKToUTF8(const std::string& gbkStr);

std::string UTF8ToGBK(const std::string& utf8Str);

#endif // !SQLITE3_CN_WIN_EXTENSTION_H
