#pragma once
#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <sstream>

/*
* 因为键盘事件监听使用的是基于 0~9 + a~z + ESC + ENTER 的设计，
* 所以支持的菜单项数是有限的，
* 故而该异常为检查该问题。
*/
class MenuOverflowException : public std::exception {
public:
	MenuOverflowException(int maxMenuNum) : maxMenuNum(maxMenuNum) {
		std::stringstream err_message;
		err_message << "[Error]: Menu options exceed the maximum allowed (" << maxMenuNum << ").\n";
		message = err_message.str();
	}

	const char* what() const noexcept override {
		return message.c_str();
	}

private:
	int maxMenuNum;
	std::string message;
};

class ConsoleUI
{
public:
	explicit ConsoleUI();
	~ConsoleUI();
	ConsoleUI(const ConsoleUI&) = delete;
	ConsoleUI& operator=(const ConsoleUI&) = delete;

public:
	enum class SPEEDCLASS
	{
		SLUGGISH = 1,	// 缓慢
		SLOW,			// 慢
		STEADY,			// 稳定
		MODERATE,		// 温和
		BRISK,			// 轻快
		SWIFT,			// 迅速
		RAPID,			// 快速
		FAST,			// 快
		SPEEDY,			// 迅速
		LIGHTNING		// 闪电
	};
	void typewriter(std::string text, SPEEDCLASS speed) const;

	void clearConsole() const;

	enum VIRTUALKEY {
		KEY_UNKNOWN = -3,
		KEY_ESC = -2,
		KEY_ENTER = -1,
		KEY_1 = 1,
		KEY_2 = 2,
		KEY_3 = 3,
		KEY_4 = 4,
		KEY_5 = 5,
		KEY_6 = 6,
		KEY_7 = 7,
		KEY_8 = 8,
		KEY_9 = 9,
		KEY_A = 10,
		KEY_B = 11,
		KEY_C = 12,
		KEY_D = 13,
		KEY_E = 14,
		KEY_F = 15,
		KEY_G = 16,
		KEY_H = 17,
		KEY_I = 18,
		KEY_J = 19,
		KEY_K = 20,
		KEY_L = 21,
		KEY_M = 22,
		KEY_N = 23,
		KEY_O = 24,
		KEY_P = 25,
		KEY_Q = 26,
		KEY_R = 27,
		KEY_S = 28,
		KEY_T = 29,
		KEY_U = 30,
		KEY_V = 31,
		KEY_W = 32,
		KEY_X = 33,
		KEY_Y = 34,
		KEY_Z = 35
	};
	VIRTUALKEY getKeyboardInput() const;


	/*
	* Substitution Failure Is Not An Error (SFINAE) 机制解决函数重载解析冲突问题。
	* 
	* 简单解释：
	*     void renderLogo() const;
	*     void renderLogo(std::string end = "\n") const;
	*     void renderLogo(std::string start = "", std::string end = "\n") const;
	* 这三个函数如果直接这么定义就会出现函数重载解析错误（MSVC E0308），
	* 编译器会无法确定到底调用哪个重载函数。
	* 例如：我直接 renderLogo(); 这么调用，编译器就无法确定到底调用哪个重载函数。
	* 
	* 下面是该场景下 SFINAE 机制的详细描述：
	* 通过 std::enable_if 结合 sizeof...(Args) 和 std::is_convertible 限制模板匹配条件，从而实现不同的函数重载：
	* 
	*         template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	*     - 这是 “无参数” 的模板函数版本。Args... 表示可以接收任意数量的模板参数，
	*       但通过 std::enable_if_t<sizeof...(Args) == 0> 限制，确保只能接收 0 个参数。
	* 
	*         template<typename T, typename = std::enable_if_t<std::is_convertible<T, std::string>::value>>
	*     - 这是接受 “一个参数” 的模板函数。std::is_convertible<T, std::string>::value 保证传入的参数类型
	*       可以隐式转换为 std::string（即可以传入 const char[] 或 std::string）。
	* 
	*         template<typename T1, typename T2, typename = std::enable_if_t<std::is_convertible<T1, std::string>::value && std::is_convertible<T2, std::string>::value>>
	*     - 这是接受 “两个参数” 的模板函数，要求两个参数都能转换为 std::string 类型。
	*/

	template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	void renderLogo(Args... args) const;

	template<typename T, typename = std::enable_if_t<std::is_convertible<T, std::string>::value>>
	void renderLogo(const T& end) const;

	template<typename T1, typename T2, typename = std::enable_if_t<std::is_convertible<T1, std::string>::value && std::is_convertible<T2, std::string>::value>>
	void renderLogo(const T1& start, const T2& end) const;


	using MenuBox = std::vector<std::string>;
	static const size_t MAX_MENU_NUM = 35;	// 最多支持 35 个菜单选项

	template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	void renderMenu(MenuBox& menus, const int selected) const;

	template<typename T, typename = std::enable_if_t<std::is_constructible<T, int>::value>>
	void renderMenu(MenuBox& menus, const int selected, T left_margin_width) const;

	template<typename T1, typename T2, typename = std::enable_if_t<std::is_constructible<T1, int>::value && std::is_convertible<T2, std::string>::value>>
	void renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& end) const;

	template<typename T1, typename T2, typename T3, typename = std::enable_if_t<std::is_constructible<T1, int>::value && std::is_convertible<T2, std::string>::value && std::is_convertible<T3, std::string>::value>>
	void renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& start, const T3& end) const;

	void setLogo(const std::string& logo);

	// 该函数在设计时可以采用引用类型作为参数的方式返回结果，
	// 但是我更习惯将函数参数作为一种输入，而不是输出，
	// 所以，这里的结果返回值尽量还是以 return 为实现，故而采用 pair 类型。
	using LoginResult = std::pair<std::string, std::string>;
	LoginResult renderLogin();

private:
	std::string _logo;

	void renderLogoImpl(const std::string start = "", const std::string end = "\n") const;
	void renderMenuImpl(MenuBox& menus, const int selected, int left_margin_width = 8, const std::string start = "", const std::string end = "\n") const;

#ifndef _WIN32
	void disableEcho(); // Linux 下禁用控制台回显

	void enableEcho();  // Linux 下启用控制台回显

	int getch();        // Linux 下获取字符输入
#endif // !_WIN32


	const std::string getPassword(const size_t MIN_LEN = 4, const size_t MAX_LEN = 16);
};

/*
* 在 C++ 中，模板函数的定义必须在声明的同一个头文件中，以确保编译器可以在实例化模板时找到所有必要的代码。
* 如果将模板函数的实现放在 .cpp 文件中，这将会导致链接器无法找到这些模板的定义。
* 
* 在该场景中就会：
* 遇到链接错误（LNK2019），也就是链接器无法找到 ConsoleUI::renderLogo 和 ConsoleUI::renderMenu 的定义。
*/

template<typename... Args, typename>
void ConsoleUI::renderLogo(Args... args) const {
	renderLogoImpl("", "\n");
}

template<typename T, typename>
void ConsoleUI::renderLogo(const T& end) const {
	renderLogoImpl("", std::string(end));
}

template<typename T1, typename T2, typename>
void ConsoleUI::renderLogo(const T1& start, const T2& end) const {
	renderLogoImpl(std::string(start), std::string(end));
}

template<typename ...Args, typename>
void ConsoleUI::renderMenu(MenuBox& menus, const int selected) const {
	renderMenuImpl(menus, selected, 8, "", "\n");
}

template<typename T, typename>
void ConsoleUI::renderMenu(MenuBox& menus, const int selected, T left_margin_width) const
{
	renderMenuImpl(menus, selected, left_margin_width, "", "\n");
}

template<typename T1, typename T2, typename>
void ConsoleUI::renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& end) const
{
	renderMenuImpl(menus, selected, left_margin_width, "", std::string(end));
}

template<typename T1, typename T2, typename T3, typename>
void ConsoleUI::renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& start, const T3& end) const
{
	renderMenuImpl(menus, selected, left_margin_width, std::string(start), std::string(end));
}

#endif // !CONSOLE_UI_H
