#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <cstdlib>
#include <Windows.h>

/*
* 第三方库 CRYPTOPP 存在一些无关紧要的警告信息，
* 这里针对 GCC\Clang\MSVC 等编译器做一下屏蔽。
*/
#if defined(__GNUC__) || defined(__clang__) // GCC 或 Clang 编译器
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnoexcept"
#pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(_MSC_VER)                     // MSVC 编译器
#pragma warning(push)
#pragma warning(disable : 26439)
#pragma warning(disable : 26495) 
#endif
#include <cryptopp/sha.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/aes.h>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#if defined(__GNUC__) || defined(__clang__) // GCC 或 Clang 编译器
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)                     // MSVC 编译器
#pragma warning(pop)
#endif

extern "C" {
#include <sqlite3.h>
}

// SIMS 数据库
sqlite3* sims_db;

#include "./modules/University.h"
#include "./modules/Person.h"
#include "./modules/Student.h"
#include "./modules/ConsoleUI.h"
#include "./utils/ConsoleColor.h"

#ifdef _WIN32
/*
* 负责解决 SQLite3 数据库 TEXT 数据中中文字符编码问题。
*
* SQLite3 TEXT 编码为 UTF-8，
* Windows 平台的控制台字符编码为 GBK，
* 所以，在 Windows 平台会出现“中文乱码问题”。
*/
#include "./utils/SQLite3_CN_WIN_EXTENSTION.h"
#endif // !_WIN32

std::string getEncryptedFileName(const std::string& student_information);
void initUniversityData(sqlite3* db);
void initStudentData(sqlite3* db);
bool deleteStudentInfo(sqlite3* db, std::string id_number);
template <typename T>
bool alterStudentInfo(sqlite3* db, const std::string& id_number, const std::string& column, const T& value);
bool addStudentInfo(
    sqlite3* db,
    std::string id_number,
    std::string name,
    std::string gender,
    int age,
    std::string contact_info,
    std::string student_id,
    const University* const univ,
    University::College college_id,
    University::Major major_id
);

enum class Page {
    MAIN_MENU,
    LOGIN,
    ABOUT,
    EXIT,
    CONSOLE_MENUS,
    VIEW_STUINFO,
    ALTER_STUINFO,
    DELETE_STUINFO,
    ADD_STUINFO
};

Page mainMenuUI(ConsoleUI& main_menu_ui);
Page loginUI(ConsoleUI& login_ui);
Page aboutUI(ConsoleUI& about_ui);
Page exitUI(ConsoleUI& exit_ui);
Page consoleUI(ConsoleUI& console_ui);
Page viewStuInfoUI(ConsoleUI& view_stu_info_ui);
Page alterStuInfoUI(ConsoleUI& alter_stu_info_ui);
Page deleteStuInfoUI(ConsoleUI& delete_stu_info_ui);
Page addStuInfoUI(ConsoleUI& add_stu_info_ui);

const int NOT_FOUND = -1;
const int searchStuInlineUI();

/*
* 在 C++ 中，static 关键字对全局变量的影响和意义：
* 
* - 全局变量：全局变量是在所有文件范围内都可见的变量。如果一个变量在某个文件中定义为全局变量，
*   它可以在整个程序的其他源文件中通过 extern 关键字访问。这种全局变量是具有外部链接性的。
*
* - 使用 static 关键字修饰全局变量：如果你为全局变量加上 static 关键字，变量的作用域就被限制
*   在定义它的源文件中。这意味着该变量具有内部链接性，只能在定义它的文件中访问，其他文件无法
*   通过 extern 声明访问到它。
*/

using PageFunc = std::function<Page(ConsoleUI&)>;
static std::map<Page, PageFunc> page_map = {
    { Page::MAIN_MENU, mainMenuUI },
    { Page::LOGIN, loginUI },
    { Page::ABOUT, aboutUI },
    { Page::EXIT, exitUI },
    { Page::CONSOLE_MENUS, consoleUI },
    { Page::VIEW_STUINFO, viewStuInfoUI },
    { Page::ALTER_STUINFO, alterStuInfoUI },
    { Page::DELETE_STUINFO, deleteStuInfoUI },
    { Page::ADD_STUINFO, addStuInfoUI }
};
static auto& nextPage = page_map;   // 为了增加代码可读性

void openWebpage(const std::string& url);





template<typename T>
inline bool alterStudentInfo(sqlite3* db, const std::string& id_number, const std::string& column, const T& value)
{
    sqlite3_stmt* stmt;
    std::string sql = "UPDATE Student SET " + column + " = ? WHERE id_number = ?;";

    // 准备 SQL 语句
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // 绑定第一个参数
    if constexpr (std::is_same_v<T, std::string>) {
        rc = sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_STATIC);
    }
    else if constexpr (std::is_integral_v<T>) {
        rc = sqlite3_bind_int(stmt, 1, value);
    }

    // 绑定第二个参数
    rc = sqlite3_bind_text(stmt, 2, id_number.c_str(), -1, SQLITE_STATIC);

    // 执行 SQL 语句
    rc = sqlite3_step(stmt);

    // 清理
    sqlite3_finalize(stmt);
    return true;
}

#endif // !MAIN_H
