#pragma once
#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <sstream>

/*
* ��Ϊ�����¼�����ʹ�õ��ǻ��� 0~9 + a~z + ESC + ENTER ����ƣ�
* ����֧�ֵĲ˵����������޵ģ�
* �ʶ����쳣Ϊ�������⡣
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
		SLUGGISH = 1,	// ����
		SLOW,			// ��
		STEADY,			// �ȶ�
		MODERATE,		// �º�
		BRISK,			// ���
		SWIFT,			// Ѹ��
		RAPID,			// ����
		FAST,			// ��
		SPEEDY,			// Ѹ��
		LIGHTNING		// ����
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
	* Substitution Failure Is Not An Error (SFINAE) ���ƽ���������ؽ�����ͻ���⡣
	* 
	* �򵥽��ͣ�
	*     void renderLogo() const;
	*     void renderLogo(std::string end = "\n") const;
	*     void renderLogo(std::string start = "", std::string end = "\n") const;
	* �������������ֱ����ô����ͻ���ֺ������ؽ�������MSVC E0308����
	* ���������޷�ȷ�����׵����ĸ����غ�����
	* ���磺��ֱ�� renderLogo(); ��ô���ã����������޷�ȷ�����׵����ĸ����غ�����
	* 
	* �����Ǹó����� SFINAE ���Ƶ���ϸ������
	* ͨ�� std::enable_if ��� sizeof...(Args) �� std::is_convertible ����ģ��ƥ���������Ӷ�ʵ�ֲ�ͬ�ĺ������أ�
	* 
	*         template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	*     - ���� ���޲����� ��ģ�庯���汾��Args... ��ʾ���Խ�������������ģ�������
	*       ��ͨ�� std::enable_if_t<sizeof...(Args) == 0> ���ƣ�ȷ��ֻ�ܽ��� 0 ��������
	* 
	*         template<typename T, typename = std::enable_if_t<std::is_convertible<T, std::string>::value>>
	*     - ���ǽ��� ��һ�������� ��ģ�庯����std::is_convertible<T, std::string>::value ��֤����Ĳ�������
	*       ������ʽת��Ϊ std::string�������Դ��� const char[] �� std::string����
	* 
	*         template<typename T1, typename T2, typename = std::enable_if_t<std::is_convertible<T1, std::string>::value && std::is_convertible<T2, std::string>::value>>
	*     - ���ǽ��� ������������ ��ģ�庯����Ҫ��������������ת��Ϊ std::string ���͡�
	*/

	template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	void renderLogo(Args... args) const;

	template<typename T, typename = std::enable_if_t<std::is_convertible<T, std::string>::value>>
	void renderLogo(const T& end) const;

	template<typename T1, typename T2, typename = std::enable_if_t<std::is_convertible<T1, std::string>::value && std::is_convertible<T2, std::string>::value>>
	void renderLogo(const T1& start, const T2& end) const;


	using MenuBox = std::vector<std::string>;
	static const size_t MAX_MENU_NUM = 35;	// ���֧�� 35 ���˵�ѡ��

	template<typename... Args, typename = std::enable_if_t<sizeof...(Args) == 0>>
	void renderMenu(MenuBox& menus, const int selected) const;

	template<typename T, typename = std::enable_if_t<std::is_constructible<T, int>::value>>
	void renderMenu(MenuBox& menus, const int selected, T left_margin_width) const;

	template<typename T1, typename T2, typename = std::enable_if_t<std::is_constructible<T1, int>::value && std::is_convertible<T2, std::string>::value>>
	void renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& end) const;

	template<typename T1, typename T2, typename T3, typename = std::enable_if_t<std::is_constructible<T1, int>::value && std::is_convertible<T2, std::string>::value && std::is_convertible<T3, std::string>::value>>
	void renderMenu(MenuBox& menus, const int selected, T1 left_margin_width, const T2& start, const T3& end) const;

	void setLogo(const std::string& logo);

	// �ú��������ʱ���Բ�������������Ϊ�����ķ�ʽ���ؽ����
	// �����Ҹ�ϰ�߽�����������Ϊһ�����룬�����������
	// ���ԣ�����Ľ������ֵ���������� return Ϊʵ�֣��ʶ����� pair ���͡�
	using LoginResult = std::pair<std::string, std::string>;
	LoginResult renderLogin();

private:
	std::string _logo;

	void renderLogoImpl(const std::string start = "", const std::string end = "\n") const;
	void renderMenuImpl(MenuBox& menus, const int selected, int left_margin_width = 8, const std::string start = "", const std::string end = "\n") const;

#ifndef _WIN32
	void disableEcho(); // Linux �½��ÿ���̨����

	void enableEcho();  // Linux �����ÿ���̨����

	int getch();        // Linux �»�ȡ�ַ�����
#endif // !_WIN32


	const std::string getPassword(const size_t MIN_LEN = 4, const size_t MAX_LEN = 16);
};

/*
* �� C++ �У�ģ�庯���Ķ��������������ͬһ��ͷ�ļ��У���ȷ��������������ʵ����ģ��ʱ�ҵ����б�Ҫ�Ĵ��롣
* �����ģ�庯����ʵ�ַ��� .cpp �ļ��У��⽫�ᵼ���������޷��ҵ���Щģ��Ķ��塣
* 
* �ڸó����оͻ᣺
* �������Ӵ���LNK2019����Ҳ�����������޷��ҵ� ConsoleUI::renderLogo �� ConsoleUI::renderMenu �Ķ��塣
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
