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

		// ʹ��std::this_thread::sleep_for��ʵ�ֺ��뼶�������
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
	int ch = _getch();  // Windows �»�ȡ��������

	if (ch == 27) return VIRTUALKEY::KEY_ESC;	// ESC
	if (ch == 13) return VIRTUALKEY::KEY_ENTER; // Enter

	if (ch >= '1' && ch <= '9') return static_cast<VIRTUALKEY>(ch - '0');  // 1-9 ���� 1-9
	if (ch >= 'a' && ch <= 'z') return static_cast<VIRTUALKEY>(ch - 'a' + 10);  // a-z ���� 10-35
	if (ch >= 'A' && ch <= 'Z') return static_cast<VIRTUALKEY>(ch - 'A' + 10);  // A-Z תΪСд����

#else
	struct termios oldt, newt;
	int ch;

	// �����ն�Ϊ�޻���ģʽ
	tcgetattr(STDIN_FILENO, &oldt);  // ��ȡ��ǰ�ն�����
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);  // ���û���ͻ���
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // ��������

	// ���ñ�׼����Ϊ������ģʽ
	int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();  // ��ȡ��������

	// �ָ��ն�����
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch == 27) return VIRTUALKEY::KEY_ESC;  // ESC
	if (ch == 10) return VIRTUALKEY::KEY_ENTER; // Enter

	if (ch >= '1' && ch <= '9') return static_caset<VIRTUALKEY>(ch - '0');  // 1-9 ���� 1-9
	if (ch >= 'a' && ch <= 'z') return static_caset<VIRTUALKEY>(ch - 'a' + 10);  // a-z ���� 10-35
	if (ch >= 'A' && ch <= 'Z') return static_caset<VIRTUALKEY>(ch - 'A' + 10);  // A-Z תΪСд����
#endif

	return KEY_UNKNOWN;  // δ֪����
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

		// ������볤���Ƿ����Ҫ��
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

	// ���� left margin
	std::string left_margin(left_margin_width, ' ');

	// ����û��Զ���ͷ
	std::cout << start;

	for (MenuBox::iterator it = menus.begin(); it != menus.end(); it++) {
		int index = static_cast<int>(it - menus.begin()); // ���㵱ǰ�������

		std::cout << left_margin;

		// ��Ⱦѡ�еı�ǣ������ǰ����ѡ�еģ�
		std::cout << (index == selected ? "* " : "");

		// ��Ⱦ�������
		if (index + 1 > 9) {
			char displayChar = 'a' + (index - 9); // 10 ��ʾΪ 'a'
			//if (displayChar > 'z') {
			//	displayChar = 'z'; // ������� 'z'������ʾ 'z'
			//}
			std::cout << displayChar;
		}
		else {
			std::cout << (index + 1); // ��ӡ��������
		}

		std::cout << " - " << *it << "\n";
	}

	// ��Ⱦ������ʾ��Ϣ
	std::cout << "\nPress [key] to select\n[ ";
	for (int i = 1; i <= static_cast<int>(menus.size()); ++i) {
		if (i >= 10) {
			char displayChar = 'a' + (i - 10); // �� 10 ��ʼӳ�䵽��ĸ 'a'
			//if (displayChar > 'z') {
			//	displayChar = 'z'; // ������� 'z'������ʾ 'z'
			//}
			std::cout << displayChar;
		}
		else {
			std::cout << i; // ��ӡ��������
		}

		std::cout << " | ";
	}
	std::cout << "Enter | Esc ]";

	// ����û��Զ���β
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
	* ʵ������δ�������д��������ģ�
	*
	* #ifdef _WIN32
	*         while ((ch = _getch()) != '\r') { // Windows ���з��� '\r'
	* #else
	*         disableEcho();
	*         while ((ch = getch()) != '\n') { // Linux ���з��� '\n'
	* #endif
	*             if ((ch == 127 || ch == 8) && !password.empty()) { // �����˸��
	*                 std::cout << "\b \b"; // ɾ��һ���ַ�
	*                 password.pop_back();
	*             } else if (isprint(ch) && password.size() < MAX_LEN) { // ����ɴ�ӡ�ַ��ҳ��Ȳ������������
	*                 password += ch;
	*                 std::cout << '*';
	*             }
	*         }
	*
	* ���� VS IDE ��Ϊ����д������ȫ�������ôд�ͻᵼ�� main.h ͷ�ļ��еĺ�������һֱ��ʾ�Ҳ������壬
	* ��ʵ���϶����Ѿ����ڲ��ҿ��Ա� VS IDE ��ȷ���ӡ�
	*
	* Ŀǰ��δ�ҵ��õĽ�����������Բ��������д��������� VS IDE �ľ��档
	*/

#ifdef _WIN32
	while ((ch = _getch()) != '\r') { // Windows ���з��� '\r'
		if ((ch == 127 || ch == 8) && !password.empty()) { // �����˸��
			std::cout << "\b \b"; // ɾ��һ���ַ�
			password.pop_back();
		}
		else if (isprint(ch) && password.size() < MAX_LEN) { // ����ɴ�ӡ�ַ��ҳ��Ȳ������������
			password += ch;
			std::cout << '*';
		}
	}
#else
	disableEcho();
	while ((ch = getch()) != '\n') { // Linux ���з��� '\n'
		if ((ch == 127 || ch == 8) && !password.empty()) { // �����˸��
			std::cout << "\b \b"; // ɾ��һ���ַ�
			password.pop_back();
		}
		else if (isprint(ch) && password.size() < MAX_LEN) { // ����ɴ�ӡ�ַ��ҳ��Ȳ������������
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
