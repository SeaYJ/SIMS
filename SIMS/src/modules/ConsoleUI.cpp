#include "ConsoleUI.h"

#include <chrono>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#else
extern "C" {
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
}
#endif

ConsoleUI::ConsoleUI()
	: _logo("")
{
}

ConsoleUI::~ConsoleUI()
{
}

void ConsoleUI::typewriter(std::string text, SPEEDCLASS speed) const
{
	int interval = 100 / static_cast<int>(speed);

	for (std::string::iterator it = text.begin(); it != text.end(); it++) {
		std::cout << *it;

		// 使用std::this_thread::sleep_for来实现毫秒级别的休眠
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}

void ConsoleUI::clearConsole() const
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // !_WIN32
}

ConsoleUI::VIRTUALKEY ConsoleUI::getKeyboardInput() const
{
#ifdef _WIN32
	int ch = _getch();  // Windows 下获取键盘输入

	if (ch == 27) return VIRTUALKEY::KEY_ESC;	// ESC
	if (ch == 13) return VIRTUALKEY::KEY_ENTER; // Enter

	if (ch >= '1' && ch <= '9') return static_cast<VIRTUALKEY>(ch - '0');  // 1-9 返回 1-9
	if (ch >= 'a' && ch <= 'z') return static_cast<VIRTUALKEY>(ch - 'a' + 10);  // a-z 返回 10-35
	if (ch >= 'A' && ch <= 'Z') return static_cast<VIRTUALKEY>(ch - 'A' + 10);  // A-Z 转为小写处理

#else
	struct termios oldt, newt;
	int ch;

	// 设置终端为无回显模式
	tcgetattr(STDIN_FILENO, &oldt);  // 获取当前终端属性
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);  // 禁用缓冲和回显
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // 立即设置

	// 设置标准输入为非阻塞模式
	int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();  // 获取键盘输入

	// 恢复终端设置
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch == 27) return VIRTUALKEY::KEY_ESC;  // ESC
	if (ch == 10) return VIRTUALKEY::KEY_ENTER; // Enter

	if (ch >= '1' && ch <= '9') return static_caset<VIRTUALKEY>(ch - '0');  // 1-9 返回 1-9
	if (ch >= 'a' && ch <= 'z') return static_caset<VIRTUALKEY>(ch - 'a' + 10);  // a-z 返回 10-35
	if (ch >= 'A' && ch <= 'Z') return static_caset<VIRTUALKEY>(ch - 'A' + 10);  // A-Z 转为小写处理
#endif

	return KEY_UNKNOWN;  // 未知按键
}

void ConsoleUI::setLogo(const std::string& logo)
{
	_logo = logo;
}

ConsoleUI::LoginResult ConsoleUI::renderLogin()
{
	std::string input_username;
	std::string input_password;

	std::cout << "\n\nPlease input the info:\n"
		<< "$ Username: ";
	std::cin >> input_username;

	const size_t MIN_PASSWORD_LEN = 4;
	const size_t MAX_PASSWORD_LEN = 16;
	while (true) {
		std::cout << "$ Password: ";
		input_password = getPassword(MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);

		// 检查密码长度是否符合要求
		if (input_password.length() >= MIN_PASSWORD_LEN && input_password.length() <= MAX_PASSWORD_LEN) {
			break;
		}
		else {
			std::cout << "\tPassword must be between " << MIN_PASSWORD_LEN << " and " << MAX_PASSWORD_LEN << " characters. Please try again.\n";
		}
	}
	return std::make_pair(input_username, input_password);
}

void ConsoleUI::renderLogoImpl(const std::string start, const std::string end) const
{
	std::cout << start << _logo << end;
}

void ConsoleUI::renderMenuImpl(MenuBox& menus, const int selected, int left_margin_width, const std::string start, const std::string end) const
{
	if (menus.size() > MAX_MENU_NUM) {
		throw MenuOverflowException(MAX_MENU_NUM);
	}

	if (menus.size() != 0 && (selected < 0 || selected >= static_cast<int>(menus.size()))) {
		std::cerr << "[Warning]: The parameter 'selected'(" << selected << ") is outside the menu index boundary(0~" << menus.size() << ").\n";
	}

	if (left_margin_width < 0) {
		std::cerr << "[Warning]: The left margin cannot be less than 0 spaces. It has been reverted to the default value of 8 spaces.\n";
		left_margin_width = 8;
	}

	// 制作 left margin
	std::string left_margin(left_margin_width, ' ');

	// 输出用户自定义头
	std::cout << start;

	for (MenuBox::iterator it = menus.begin(); it != menus.end(); it++) {
		int index = static_cast<int>(it - menus.begin()); // 计算当前项的索引

		std::cout << left_margin;

		// 渲染选中的标记（如果当前项是选中的）
		std::cout << (index == selected ? "* " : "");

		// 渲染索引编号
		if (index + 1 > 9) {
			char displayChar = 'a' + (index - 9); // 10 显示为 'a'
			//if (displayChar > 'z') {
			//	displayChar = 'z'; // 如果超出 'z'，则显示 'z'
			//}
			std::cout << displayChar;
		}
		else {
			std::cout << (index + 1); // 打印数字索引
		}

		std::cout << " - " << *it << "\n";
	}

	// 渲染按键提示信息
	std::cout << "\nPress [key] to select\n[ ";
	for (int i = 1; i <= static_cast<int>(menus.size()); ++i) {
		if (i >= 10) {
			char displayChar = 'a' + (i - 10); // 从 10 开始映射到字母 'a'
			//if (displayChar > 'z') {
			//	displayChar = 'z'; // 如果超出 'z'，则显示 'z'
			//}
			std::cout << displayChar;
		}
		else {
			std::cout << i; // 打印数字索引
		}

		std::cout << " | ";
	}
	std::cout << "Enter | Esc ]";

	// 输出用户自定义尾
	std::cout << end;
}

#ifndef _WIN32
void ConsoleUI::disableEcho()
{
	termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	tty.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void ConsoleUI::enableEcho()
{
	termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	tty.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int ConsoleUI::getch()
{
	termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
#endif // !_WIN32

const std::string ConsoleUI::getPassword(const size_t MIN_LEN, const size_t MAX_LEN)
{
	std::string password;
	char ch;

	/*
	* 实际上这段代码这样写才是最简洁的：
	*
	* #ifdef _WIN32
	*         while ((ch = _getch()) != '\r') { // Windows 换行符是 '\r'
	* #else
	*         disableEcho();
	*         while ((ch = getch()) != '\n') { // Linux 换行符是 '\n'
	* #endif
	*             if ((ch == 127 || ch == 8) && !password.empty()) { // 处理退格键
	*                 std::cout << "\b \b"; // 删除一个字符
	*                 password.pop_back();
	*             } else if (isprint(ch) && password.size() < MAX_LEN) { // 处理可打印字符且长度不超过最大限制
	*                 password += ch;
	*                 std::cout << '*';
	*             }
	*         }
	*
	* 但是 VS IDE 认为这种写法不安全，如果这么写就会导致 main.h 头文件中的函数声明一直提示找不到定义，
	* 而实际上定义已经存在并且可以被 VS IDE 正确链接。
	*
	* 目前并未找到好的解决方案，所以采用冗余的写法进而规避 VS IDE 的警告。
	*/

#ifdef _WIN32
	while ((ch = _getch()) != '\r') { // Windows 换行符是 '\r'
		if ((ch == 127 || ch == 8) && !password.empty()) { // 处理退格键
			std::cout << "\b \b"; // 删除一个字符
			password.pop_back();
		}
		else if (isprint(ch) && password.size() < MAX_LEN) { // 处理可打印字符且长度不超过最大限制
			password += ch;
			std::cout << '*';
		}
	}
#else
	disableEcho();
	while ((ch = getch()) != '\n') { // Linux 换行符是 '\n'
		if ((ch == 127 || ch == 8) && !password.empty()) { // 处理退格键
			std::cout << "\b \b"; // 删除一个字符
			password.pop_back();
		}
		else if (isprint(ch) && password.size() < MAX_LEN) { // 处理可打印字符且长度不超过最大限制
			password += ch;
			std::cout << '*';
		}
	}
#endif

#ifndef _WIN32
	enableEcho();
#endif

	return password;
}
