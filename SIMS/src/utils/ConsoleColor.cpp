#include "ConsoleColor.h"

ConsoleColor::ConsoleColor(Color fg, Color bg)
	: fg_color(fg), bg_color(bg), csbi() 
{}

void ConsoleColor::setColor(Color fg, Color bg)
{
    fg_color = fg;
    bg_color = bg;
}

void ConsoleColor::saveCurrentColor()
{
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE) {
        GetConsoleScreenBufferInfo(console, &csbi);
    }
#else
    // 对于Unix系统，不需要手动保存颜色，因为我们使用ANSI控制码直接设置颜色，恢复到默认颜色即可。
#endif
}

void ConsoleColor::applyColor(std::ostream& os)
{
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE) {
        // 设置新的控制台颜色属性
        SetConsoleTextAttribute(console, generateAttributes(fg_color, bg_color, csbi.wAttributes));
    }
#else
    applyUnixColor(os, fg_color, bg_color);
#endif
}

void ConsoleColor::resetColor(std::ostream& os)
{
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE) {
        // 恢复控制台的颜色
        SetConsoleTextAttribute(console, csbi.wAttributes);
    }
#else
    os << "\x1B[0m";  // Unix系统恢复默认颜色
#endif
}

void ConsoleColor::printColored(Color fg, Color bg, const std::string& text)
{
    ConsoleColor console(fg, bg);   // 创建颜色对象
    console.saveCurrentColor();     // 保存当前颜色
    console.applyColor(std::cout);  // 应用新颜色
    std::cout << text; // 输出文本
    console.resetColor(std::cout);  // 恢复之前保存的颜色
}

#ifdef _WIN32
WORD ConsoleColor::generateAttributes(Color fg, Color bg, WORD attributes)
{
    static const WORD FG[] = { 0, 0, FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_RED | FOREGROUND_GREEN,
                              FOREGROUND_BLUE, FOREGROUND_RED | FOREGROUND_BLUE, FOREGROUND_GREEN | FOREGROUND_BLUE,
                              FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, FOREGROUND_INTENSITY,
                              FOREGROUND_INTENSITY | FOREGROUND_RED, FOREGROUND_INTENSITY | FOREGROUND_GREEN,
                              FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_INTENSITY | FOREGROUND_BLUE,
                              FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
                              FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE };

    static const WORD BG[] = { 0, 0, BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_RED | BACKGROUND_GREEN,
                              BACKGROUND_BLUE, BACKGROUND_RED | BACKGROUND_BLUE, BACKGROUND_GREEN | BACKGROUND_BLUE,
                              BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE, BACKGROUND_INTENSITY,
                              BACKGROUND_INTENSITY | BACKGROUND_RED, BACKGROUND_INTENSITY | BACKGROUND_GREEN,
                              BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN, BACKGROUND_INTENSITY | BACKGROUND_BLUE,
                              BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE, BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
                              BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE };

    WORD result = attributes & ~(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED);
    result |= FG[fg];
    result |= BG[bg];
    return result;
}
#else
void ConsoleColor::applyUnixColor(std::ostream& os, Color fg, Color bg)
{
    os << "\x1B[" << 30 + fg << ";" << 40 + bg << "m";
}
#endif // !_WIN32