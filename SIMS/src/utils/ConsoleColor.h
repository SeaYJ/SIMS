#pragma once
#ifndef CONSOLE_COLOR_H
#define CONSOLE_COLOR_H

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // !_WIN32

class ConsoleColor {
public:
    // 枚举定义了一系列可用的控制台颜色
    enum Color {
        NONE = 0,        // 无颜色
        BLACK,           // 黑色
        DARK_RED,        // 暗红色
        DARK_GREEN,      // 暗绿色
        DARK_YELLOW,     // 暗黄色
        DARK_BLUE,       // 暗蓝色
        DARK_MAGENTA,    // 暗洋红色
        DARK_CYAN,       // 暗青色
        GRAY,            // 灰色
        DARK_GRAY,       // 暗灰色
        RED,             // 红色
        GREEN,           // 绿色
        YELLOW,          // 黄色
        BLUE,            // 蓝色
        MAGENTA,         // 洋红色
        CYAN,            // 青色
        WHITE            // 白色
    };

    // 构造函数，初始化前景色和背景色
    ConsoleColor(Color fg = NONE, Color bg = NONE);

    // 设置控制台前景色和背景色
    void setColor(Color fg, Color bg);

    // 保存当前控制台颜色
    void saveCurrentColor();

    // 应用设置的颜色到控制台流
    void applyColor(std::ostream& os);

    // 恢复之前保存的控制台颜色
    void resetColor(std::ostream& os);

    // 工具函数：输出指定颜色的内容，输出完成后恢复控制台颜色
    static void printColored(Color fg, Color bg, const std::string& text);

private:
    Color fg_color; // 前景色
    Color bg_color; // 背景色

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi; // Windows下保存的控制台颜色信息

    // Windows 平台下，生成前景色和背景色的控制台属性
    WORD generateAttributes(Color fg, Color bg, WORD attributes);
#else
    // Unix 平台下应用控制台颜色
    void applyUnixColor(std::ostream& os, Color fg, Color bg);
#endif // !_WIN32
};

#endif // !CONSOLE_COLOR_H
