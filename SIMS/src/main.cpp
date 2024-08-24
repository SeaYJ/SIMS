#include "main.h"

#include <algorithm>

int main()
{
    // 获取数据库文件路径
    std::string db_path = "data/" + getEncryptedFileName("student_information") + ".db";

    int rc;

    // 打开数据库
    rc = sqlite3_open(db_path.c_str(), &sims_db);
    if (rc != SQLITE_OK) {
        std::cerr << "无法打开数据库\n" << sqlite3_errmsg(sims_db) << std::endl;
        return rc;
    }

    // 初始化数据
    initUniversityData(sims_db);
    initStudentData(sims_db);

    ConsoleUI ui;
    Page current_page = Page::MAIN_MENU;
    while (current_page != Page::EXIT) {
        current_page = nextPage[current_page](ui);
    }
    nextPage[Page::EXIT](ui); // exit UI 渲染

    // 关闭数据库
    sqlite3_close(sims_db);

    return 0;
}

std::string getEncryptedFileName(const std::string& student_information)
{
    const char* const salt_str = "SeaYJ";

    // 盐值计算：对 "SeaYJ" 进行 SHA256 加密
    CryptoPP::SHA256 hash;
    byte salt[CryptoPP::SHA256::DIGESTSIZE] = { 0 };
    hash.CalculateDigest(salt, reinterpret_cast<const byte*>(salt_str), std::strlen(salt_str));

    // 生成 PBKDF2 加密的密文
    byte derived_key[16] = { 0 }; // Key长度128位
    CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> pbkdf2;
    pbkdf2.DeriveKey(derived_key, sizeof(derived_key), 0,
        reinterpret_cast<const byte*>(student_information.data()), student_information.size(),
        salt, sizeof(salt), 916);

    // 将加密的密文转换为十六进制表示
    std::string encoded;
    CryptoPP::HexEncoder key_encoder(new CryptoPP::StringSink(encoded));
    key_encoder.Put(derived_key, sizeof(derived_key));
    key_encoder.MessageEnd();

    // 作为 SQLite3 数据库文件名
    return encoded;
}

void initUniversityData(sqlite3* db)
{
    // 准备 SQL 查询语句
    const char* sql_university = "SELECT University.school_id, SchoolName.name AS school_name, University.college_id AS college_id, CM.major_id AS major_id, SchoolStaff_Principal.id_number AS principal_id_number, SchoolStaff_Principal.name AS principal_name, SchoolStaff_Principal.gender AS principal_gender, SchoolStaff_Principal.age AS principal_age, SchoolStaff_Principal.contact_info AS principal_contact_info, SchoolStaff_Dean.id_number AS dean_id_number, SchoolStaff_Dean.name AS dean_name, SchoolStaff_Dean.gender AS dean_gender, SchoolStaff_Dean.age AS dean_age, SchoolStaff_Dean.contact_info AS dean_contact_info FROM University JOIN SchoolName ON University.school_id = SchoolName.id JOIN CollegeName ON University.college_id = CollegeName.id JOIN SchoolStaff AS SchoolStaff_Principal ON University.principal_id = SchoolStaff_Principal.id_number JOIN SchoolStaff AS SchoolStaff_Dean ON University.dean_id = SchoolStaff_Dean.id_number JOIN CM ON University.college_id = CM.college_id JOIN MajorName ON CM.major_id = MajorName.id ORDER BY University.school_id;";
    sqlite3_stmt* stmt_university;
    sqlite3_prepare_v2(db, sql_university, -1, &stmt_university, 0);

    int last_school_id = -1;
    while (sqlite3_step(stmt_university) == SQLITE_ROW) {
        // 从数据库读取相关信息
        int school_id = sqlite3_column_int(stmt_university, 0);
        std::string school_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 1));
        int college_id = sqlite3_column_int(stmt_university, 2);
        int major_id = sqlite3_column_int(stmt_university, 3);
        std::string principal_id_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 4));
        std::string principal_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 5));
        std::string principal_gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 6));
        int principal_age = sqlite3_column_int(stmt_university, 7);
        std::string principal_contact_info = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 8));
        std::string dean_id_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 9));
        std::string dean_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 10));
        std::string dean_gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 11));
        int dean_age = sqlite3_column_int(stmt_university, 12);
        std::string dean_contact_info = reinterpret_cast<const char*>(sqlite3_column_text(stmt_university, 13));

#ifdef _WIN32
        /***************************************************************************\
        * WINDOWS 平台默认编码是 GBK，但是 SQLite3 数据库中 TEXT 使用的编码是 UTF-8。   *
        * 所以，在 Windows 平台上需要对 SQLite3 数据库中读取到的 TEXT 内容进行转码，     *
        * 否则在 Windows 平台将无法进行正常显示中文内容。                               *
        * 更详细的内容请看：https://www.seayj.cn/articles/94e9/                       *
        \***************************************************************************/
        school_name = UTF8ToGBK(school_name);
        principal_id_number = UTF8ToGBK(principal_id_number);
        principal_name = UTF8ToGBK(principal_name);
        principal_gender = UTF8ToGBK(principal_gender);
        principal_contact_info = UTF8ToGBK(principal_contact_info);
        dean_id_number = UTF8ToGBK(dean_id_number);
        dean_name = UTF8ToGBK(dean_name);
        dean_gender = UTF8ToGBK(dean_gender);
        dean_contact_info = UTF8ToGBK(dean_contact_info);
#endif // !_WIN32

        if (school_id == last_school_id) {  // 继续处理同一学校的数据
            University& last_university = UNIVERSITIES.back();

            University::UniCurriculums& last_uni_curriculums
                = const_cast<University::UniCurriculums&>(last_university.getUniversityCurriculums());

            // 查找是否已经存在对应的学院(课程体系)
            University::UniCurriculums::iterator it_uni_curriculums
                = last_uni_curriculums.find(static_cast<University::College>(college_id));
            if (it_uni_curriculums != last_uni_curriculums.end()) {
                University::Majors& last_majors = it_uni_curriculums->second;
                last_majors.push_back(static_cast<University::Major>(major_id));
            }
            else {  // 未找到对应的学院信息
                // 创建某学院的专业列表
                University::Majors majors;
                majors.push_back(static_cast<University::Major>(major_id));

                last_uni_curriculums[static_cast<University::College>(college_id)] = majors;
            }

            University::Deaneries& last_deaneries
                = const_cast<University::Deaneries&>(last_university.getUniversityDeaneries());

            // 查找是否已经存在对应的学院(院长)
            University::Deaneries::iterator it_deaneries
                = last_deaneries.find(static_cast<University::College>(college_id));
            if (it_deaneries == last_deaneries.end()) {  // 未找到对应的学院信息
                Person dean = Person();
                dean.setIDNumber(dean_id_number);
                dean.setName(dean_name);
                dean.setGender(dean_gender);
                dean.setAge(dean_age);
                dean.setContactInfo(dean_contact_info);

                last_deaneries[static_cast<University::College>(college_id)] = dean;
            }
        }
        else {  // 处理一个新学校的数据
            University university;

            // 某学院的专业列表
            University::Majors majors;
            majors.push_back(static_cast<University::Major>(major_id));

            // 该大学的课程体系（学院——专业列表）
            University::UniCurriculums uni_curriculums;
            uni_curriculums[static_cast<University::College>(college_id)] = majors;

            // 校长
            Person principal = Person();
            principal.setIDNumber(principal_id_number);
            principal.setName(principal_name);
            principal.setGender(principal_gender);
            principal.setAge(principal_age);
            principal.setContactInfo(principal_contact_info);

            // 该专业的院长
            Person dean = Person();
            dean.setIDNumber(dean_id_number);
            dean.setName(dean_name);
            dean.setGender(dean_gender);
            dean.setAge(dean_age);
            dean.setContactInfo(dean_contact_info);

            University::Deaneries deaneries;
            deaneries[static_cast<University::College>(college_id)] = dean;

            // 初步设置大学信息
            university.setSchoolID(school_id);
            university.setName(school_name);
            university.setUniversityCurriculums(uni_curriculums);
            university.setPresident(principal);
            university.setUniversityDeaneries(deaneries);

            // 保存大学到全局
            UNIVERSITIES.push_back(university);

            // 更新记录
            last_school_id = school_id;
        }
    }

    //for (auto& it : UNIVERSITIES) {
    //    std::cout << it.getName() << " {" << it.getSchoolID() << "} ";

    //    const Person& principal = it.getPresident();
    //    std::cout
    //        << principal.getIDNumber() << " "
    //        << principal.getName() << " "
    //        << principal.getGender() << " "
    //        << principal.getAge() << " "
    //        << principal.getContactInfo() << "\n";

    //    const University::UniCurriculums& uni_curriculums
    //        = it.getUniversityCurriculums();

    //    for (auto& uc : uni_curriculums) {
    //        std::cout << University::getCollegeName(uc.first) << "\t{ ";

    //        for (auto& major : uc.second) {
    //            std::cout << " " << University::getMajorName(major) << " ";
    //        }

    //        std::cout << " }\n";
    //    }

    //    const University::Deaneries& deaneries
    //        = it.getUniversityDeaneries();

    //    for (auto& ds : deaneries) {
    //        std::cout << University::getCollegeName(ds.first) << "\t";

    //        auto& dean = ds.second;

    //        std::cout
    //            << dean.getIDNumber() << " "
    //            << dean.getName() << " "
    //            << dean.getGender() << " "
    //            << dean.getAge() << " "
    //            << dean.getContactInfo() << "\n";
    //    }

    //    std::cout << std::endl;
    //}

    // 释放资源
    sqlite3_finalize(stmt_university);
}

void initStudentData(sqlite3* db)
{
    // 准备 SQL 查询语句
    const char* sql_student = "SELECT Student.id_number AS student_id_number, Student.name AS student_name, Student.gender AS student_gender, Student.age AS student_age, Student.contact_info AS student_contact_info, Student.student_id AS student_id, Student.university_id AS university_id, Student.college_id AS college_id, Student.major_id AS major_id FROM Student";
    sqlite3_stmt* stmt_student;
    sqlite3_prepare_v2(db, sql_student, -1, &stmt_student, 0);

    while (sqlite3_step(stmt_student) == SQLITE_ROW) {
        // 从数据库读取相关信息
        std::string stu_id_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt_student, 0));
        std::string stu_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt_student, 1));
        std::string stu_gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt_student, 2));
        int stu_age = sqlite3_column_int(stmt_student, 3);
        std::string stu_contact_info = reinterpret_cast<const char*>(sqlite3_column_text(stmt_student, 4));
        std::string stu_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt_student, 5));
        int stu_university_id = sqlite3_column_int(stmt_student, 6);
        int stu_college_id = sqlite3_column_int(stmt_student, 7);
        int stu_major_id = sqlite3_column_int(stmt_student, 8);

#ifdef _WIN32
        /***************************************************************************\
        * WINDOWS 平台默认编码是 GBK，但是 SQLite3 数据库中 TEXT 使用的编码是 UTF-8。   *
        * 所以，在 Windows 平台上需要对 SQLite3 数据库中读取到的 TEXT 内容进行转码，     *
        * 否则在 Windows 平台将无法进行正常显示中文内容。                               *
        * 更详细的内容请看：https://www.seayj.cn/articles/94e9/                       *
        \***************************************************************************/
        stu_id_number = UTF8ToGBK(stu_id_number);
        stu_name = UTF8ToGBK(stu_name);
        stu_gender = UTF8ToGBK(stu_gender);
        stu_contact_info = UTF8ToGBK(stu_contact_info);
        stu_id = UTF8ToGBK(stu_id);
#endif // !_WIN32

        // 保存学生信息
        Student stu;

        University* stu_univ = nullptr;
        for (University& univ : UNIVERSITIES) {
            if (stu_university_id == univ.getSchoolID()) {
                stu_univ = &univ;
            }
        }

        stu.setIDNumber(stu_id_number);
        stu.setName(stu_name);
        stu.setGender(stu_gender);
        stu.setAge(stu_age);
        stu.setContactInfo(stu_contact_info);
        stu.setUniversity(stu_univ);
        stu.setCollege(static_cast<University::College>(stu_college_id));
        stu.setMajor(static_cast<University::Major>(stu_major_id));

        STUDENTS.emplace_back(stu);
    }

    //for (auto& stu : STUDENTS) {
    //    std::cout 
    //        << stu.getIDNumber() << "\t" 
    //        << stu.getName() << "\t" 
    //        << stu.getGender() << "\t" 
    //        << stu.getAge() << "\t" 
    //        << stu.getContactInfo() << "\t"
    //        << stu.getUniversity()->getName() << "-"
    //        << University::getCollegeName(stu.getCollege()) << "-" 
    //        << University::getMajorName(stu.getMajor()) << "\n\n";
    //}

    //std::cout << "共计 " << STUDENTS.size() << " 个学生数据\n";

    // 释放资源
    sqlite3_finalize(stmt_student);
}

bool deleteStudentInfo(sqlite3* db, std::string id_number)
{
    int rc;

    // 准备 SQL 查询语句
    const char* sql_delete = "DELETE FROM Student WHERE Student.id_number = ?;";
    sqlite3_stmt* stmt_delete;
    rc = sqlite3_prepare_v2(db, sql_delete, -1, &stmt_delete, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 绑定 id_number
    rc = sqlite3_bind_text(stmt_delete, 1, id_number.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_delete);
        return false;
    }

    // 执行删除操作
    rc = sqlite3_step(stmt_delete);
    if (rc != SQLITE_DONE) {
        std::cerr << "[Error]: Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_delete);
        return false;
    }
    else {
        std::cout << "[Info]: Record deleted successfully." << std::endl;
    }

    // 释放资源
    sqlite3_finalize(stmt_delete);
    return true;
}

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
    University::Major major_id)
{
    sqlite3_stmt* stmt_add;
    std::string sql_add = "INSERT INTO Student (id_number, name, gender, age, contact_info, student_id, university_id, college_id, major_id) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    // 准备 SQL 语句
    int rc = sqlite3_prepare_v2(db, sql_add.c_str(), -1, &stmt_add, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    // 绑定参数
    rc = sqlite3_bind_text(stmt_add, 1, id_number.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind id_number: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_text(stmt_add, 2, name.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind name: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_text(stmt_add, 3, gender.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind gender: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_int(stmt_add, 4, age);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind age: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_text(stmt_add, 5, contact_info.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind contact_info: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_text(stmt_add, 6, student_id.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind student_id: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_int(stmt_add, 7, univ->getSchoolID());
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind university_id: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_int(stmt_add, 8, static_cast<int>(college_id));
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind college_id: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    rc = sqlite3_bind_int(stmt_add, 9, static_cast<int>(major_id));
    if (rc != SQLITE_OK) {
        std::cerr << "[Error]: Failed to bind major_id: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    // 执行 SQL 语句
    rc = sqlite3_step(stmt_add);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt_add);
        return false;
    }

    // 清理
    sqlite3_finalize(stmt_add);
    return true;
}

Page mainMenuUI(ConsoleUI& main_menu_ui)
{
    static const std::string main_menu_logo = R"(_____/\\\\\\\\\\\____/\\\\\\\\\\\__/\\\\____________/\\\\_____/\\\\\\\\\\\___        
 ___/\\\/////////\\\_\/////\\\///__\/\\\\\\________/\\\\\\___/\\\/////////\\\_       
  __\//\\\______\///______\/\\\_____\/\\\//\\\____/\\\//\\\__\//\\\______\///__      
   ___\////\\\_____________\/\\\_____\/\\\\///\\\/\\\/_\/\\\___\////\\\_________     
    ______\////\\\__________\/\\\_____\/\\\__\///\\\/___\/\\\______\////\\\______    
     _________\////\\\_______\/\\\_____\/\\\____\///_____\/\\\_________\////\\\___   
      __/\\\______\//\\\______\/\\\_____\/\\\_____________\/\\\__/\\\______\//\\\__  
       _\///\\\\\\\\\\\/____/\\\\\\\\\\\_\/\\\_____________\/\\\_\///\\\\\\\\\\\/___ 
        ___\///////////_____\///////////__\///______________\///____\///////////_____)";

    main_menu_ui.setLogo(main_menu_logo);

    ConsoleUI::MenuBox main_menus = {
        "登录",
        "关于",
        "退出"
    };

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        main_menu_ui.clearConsole();

        // 渲染界面
        main_menu_ui.renderLogo();
        main_menu_ui.renderMenu(main_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = main_menu_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(main_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::EXIT;          // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= main_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(main_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: return Page::LOGIN;
        case 1: return Page::ABOUT;
        case 2:
        default: return Page::EXIT;
    }
}

Page loginUI(ConsoleUI& login_ui)
{
    static const std::string login_logo = R"(*****************************************
 ██╗      ██████╗  ██████╗ ██╗███╗   ██╗
 ██║     ██╔═══██╗██╔════╝ ██║████╗  ██║
 ██║     ██║   ██║██║  ███╗██║██╔██╗ ██║
 ██║     ██║   ██║██║   ██║██║██║╚██╗██║
 ███████╗╚██████╔╝╚██████╔╝██║██║ ╚████║
 ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝╚═╝  ╚═══╝
*****************************************)";

    login_ui.setLogo(login_logo);

    // 清除界面内容
    login_ui.clearConsole();

//#define AUTO_LOGIN

#ifndef AUTO_LOGIN
    // 渲染界面（注意与后面渲染登录失败界面不是一回事）
    login_ui.renderLogo();
    ConsoleUI::LoginResult result = login_ui.renderLogin();
    std::string input_username = result.first;
    std::string input_password = result.second;
#else
    std::string input_username = "SeaYJ";
    std::string input_password = "seayjnb666";
#endif // !AUTO_LOGIN

    // 错误信息（包括一切导致登录失败的信息）
    std::string err_msg;

    // 获取加密后的账户信息文件路径
    std::string account_path = "data/" + getEncryptedFileName("account_database");

    // 设置AES密钥和IV
    byte key[CryptoPP::AES::DEFAULT_KEYLENGTH] = { 0 };
    byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };

    try {
        // 读取存储的加密数据（账号和密码）
        std::string stored_encrypted_username;
        std::string stored_encrypted_password;

        std::ifstream file(account_path);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open the account-infomation file.");
        }
        std::getline(file, stored_encrypted_username);  // 第一行是账户密文
        std::getline(file, stored_encrypted_password);  // 第二行是密码密文
        file.close();

        // 对输入的账号和密码进行加密
        std::string encrypted_username;
        std::string encrypted_password;

        CryptoPP::GCM<CryptoPP::AES>::Encryption encryption;
        encryption.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));

        // 加密账号
        CryptoPP::StringSource account_encrypting(input_username, true,
            new CryptoPP::AuthenticatedEncryptionFilter(encryption,
                new CryptoPP::StringSink(encrypted_username),
                false // 不要追加校验码
            )
        );

        // 重置加密器，用于密码加密
        encryption.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));

        // 加密密码
        CryptoPP::StringSource password_encrypting(input_password, true,
            new CryptoPP::AuthenticatedEncryptionFilter(encryption,
                new CryptoPP::StringSink(encrypted_password),
                false // 不要追加校验码
            )
        );

        // 校验加密后的账号和密码是否与文件中的密文一致
        if (stored_encrypted_username == encrypted_username && stored_encrypted_password == encrypted_password) {
            return Page::CONSOLE_MENUS;
        }
        else {
            err_msg = "\nLogin failed! Username or password is incorrect.\n";
        }

    }
    catch (const CryptoPP::Exception& e) {
        err_msg = "\n[Error]{Encryption/Decryption error}: " + std::string(e.what()) + "\n";
    }
    catch (const std::exception& e) {
        err_msg = "\n[Error]: " + std::string(e.what()) + "\n";
    }

    // 登录失败、发生异常等情况一律执行以下逻辑：
    // 1、提供失败菜单；
    // 2、用户选择重新登录或者放弃登录。

    ConsoleUI::MenuBox login_failed_menus = {
        "重新登录",
        "放弃登录"
    };

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        login_ui.clearConsole();

        // 渲染界面
        login_ui.renderLogo();
        std::cout << err_msg;
        login_ui.renderMenu(login_failed_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = login_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(login_failed_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::MAIN_MENU;     // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= login_failed_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(login_failed_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: return Page::LOGIN;
        case 1:
        default: return Page::MAIN_MENU;
    }
}

Page aboutUI(ConsoleUI& about_ui) {
    static const std::string project_info = R"(============================================================================
                                PROJECT INFO                         
============================================================================
 Project Name : StudentInformationManagementSystem(SIMS)
 Description  : An impressive Student Information Management System,
                written in C++. Every student can write their own SIMS, 
                but not every student can write such the system.
 Version      : 1.0.0
 Author       : SeaYJ
 GitHub URL   : https://github.com/SeaYJ/StudentInformationManagementSystem
 License      : MIT License
============================================================================
 Thank you for using MyAwesomeLibrary! Please consider starring the repo.
============================================================================)";

    about_ui.setLogo(project_info);

    ConsoleUI::MenuBox about_menus = {
        "GITHUB",
        "返回"
    };

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        about_ui.clearConsole();

        // 渲染界面
        about_ui.renderLogo();
        about_ui.renderMenu(about_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = about_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(about_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::MAIN_MENU;     // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= about_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(about_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: {
            openWebpage("https://github.com/SeaYJ/StudentInformationManagementSystem");
            return Page::ABOUT;
        }
        case 1:
        default: return Page::MAIN_MENU;
    }
}

Page exitUI(ConsoleUI& exit_ui)
{
    static const std::string exit_sys_logo = R"(+----------------------------------------------------+
|      ____                 _   _                    |
|     / ___| ___   ___   __| | | |__  _   _  ___     |
|    | |  _ / _ \ / _ \ / _` | | '_ \| | | |/ _ \    |
|    | |_| | (_) | (_) | (_| | | |_) | |_| |  __/    |
|     \____|\___/ \___/ \__,_| |_.__/ \__, |\___|    |
|                                     |___/          |
+----------------------------------------------------+
|                    Author Card                     |
+----------------------------------------------------+
| Author : SeaYJ                                     |
| Blog   : https://www.seayj.cn                      |
| GITHUB : https://github.com/SeaYJ                  |
|                                                    |
| Quote  : "She stood there, like a small eternity!" |
+----------------------------------------------------+)";

    exit_ui.clearConsole();

    exit_ui.setLogo(exit_sys_logo);
    exit_ui.renderLogo();

    return Page::EXIT;
}

Page consoleUI(ConsoleUI& console_ui)
{
    static const std::string console_logo = R"(========================================================================
  $$$$$$\   $$$$$$\  $$\   $$\  $$$$$$\   $$$$$$\  $$\       $$$$$$$$\ 
 $$  __$$\ $$  __$$\ $$$\  $$ |$$  __$$\ $$  __$$\ $$ |      $$  _____|
 $$ /  \__|$$ /  $$ |$$$$\ $$ |$$ /  \__|$$ /  $$ |$$ |      $$ |      
 $$ |      $$ |  $$ |$$ $$\$$ |\$$$$$$\  $$ |  $$ |$$ |      $$$$$\    
 $$ |      $$ |  $$ |$$ \$$$$ | \____$$\ $$ |  $$ |$$ |      $$  __|   
 $$ |  $$\ $$ |  $$ |$$ |\$$$ |$$\   $$ |$$ |  $$ |$$ |      $$ |      
 \$$$$$$  | $$$$$$  |$$ | \$$ |\$$$$$$  | $$$$$$  |$$$$$$$$\ $$$$$$$$\ 
  \______/  \______/ \__|  \__| \______/  \______/ \________|\________|
========================================================================)";

    console_ui.setLogo(console_logo);

    ConsoleUI::MenuBox console_menus = {
        "查看学生信息",
        "修改学生信息",
        "删除学生信息",
        "新增学生信息",
        "退出登录",
        "退出系统"
    };

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        console_ui.clearConsole();

        // 渲染界面
        console_ui.renderLogo();
        console_ui.renderMenu(console_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = console_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(console_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::MAIN_MENU;     // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= console_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(console_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: return Page::VIEW_STUINFO;
        case 1: return Page::ALTER_STUINFO;
        case 2: return Page::DELETE_STUINFO;
        case 3: return Page::ADD_STUINFO;
        case 4: return Page::MAIN_MENU;
        case 5:
        default: return Page::EXIT;
    }
}

Page viewStuInfoUI(ConsoleUI& view_stu_info_ui)
{
    static const std::string view_stu_info_logo = R"( ___    ______                    ____________            ________      ________      
 __ |  / /__(_)_______      __    __  ___/_  /____  __    ____  _/_________  __/_____ 
 __ | / /__  /_  _ \_ | /| / /    _____ \_  __/  / / /     __  / __  __ \_  /_ _  __ \
 __ |/ / _  / /  __/_ |/ |/ /     ____/ // /_ / /_/ /     __/ /  _  / / /  __/ / /_/ /
 _____/  /_/  \___/____/|__/      /____/ \__/ \__,_/      /___/  /_/ /_//_/    \____/ 

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^)";

    view_stu_info_ui.setLogo(view_stu_info_logo);

    ConsoleUI::MenuBox view_stu_info_menus = {
        "上一页",
        "下一页",
        "返回"
    };

    static int start_row = 0;   // 注意这里必须为 int，要支持负数，否则就会出现 https://www.seayj.cn/articles/ccac/ 该问题
    const int offset = 12;

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        view_stu_info_ui.clearConsole();

        // 渲染界面
        view_stu_info_ui.renderLogo();
        for (int i = start_row;
            i < static_cast<int>(STUDENTS.size()) && i < (start_row + offset);
            i++) {
            std::cout
                << (i + 1) << ".\t"
                << STUDENTS[i].getIDNumber() << "\t"
                << STUDENTS[i].getName() << "\t"
                << STUDENTS[i].getGender() << "\t"
                << STUDENTS[i].getAge() << "\t"
                << STUDENTS[i].getContactInfo() << "\t"
                << STUDENTS[i].getUniversity()->getName() << "-"
                << University::getCollegeName(STUDENTS[i].getCollege()) << "-"
                << University::getMajorName(STUDENTS[i].getMajor()) << "\n";
        }
        view_stu_info_ui.renderMenu(view_stu_info_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = view_stu_info_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(view_stu_info_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::CONSOLE_MENUS; // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= view_stu_info_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(view_stu_info_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 取消 Windows 宏对 STL 算法 max、min 的干扰
#undef max
#undef min

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: {
            start_row = std::max(start_row - offset - 1, 0);
            return Page::VIEW_STUINFO;
        }
        case 1: {
            start_row = std::min(start_row + offset - 1, static_cast<int>(STUDENTS.size()) - 1);
            return Page::VIEW_STUINFO;
        }
        case 2:
        default: return Page::CONSOLE_MENUS;;
    }
}

Page alterStuInfoUI(ConsoleUI& alter_stu_info_ui)
{
    static const std::string alter_stu_info_logo = R"( _________________                 ____________            ________      ________      
 ___    |__  /_  /_____________    __  ___/_  /____  __    ____  _/_________  __/_____ 
 __  /| |_  /_  __/  _ \_  ___/    _____ \_  __/  / / /     __  / __  __ \_  /_ _  __ \
 _  ___ |  / / /_ /  __/  /        ____/ // /_ / /_/ /     __/ /  _  / / /  __/ / /_/ /
 /_/  |_/_/  \__/ \___//_/         /____/ \__/ \__,_/      /___/  /_/ /_//_/    \____/ 

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^)";

    alter_stu_info_ui.setLogo(alter_stu_info_logo);

    ConsoleUI::MenuBox alter_stu_info_menus = {
        "修改身份证号",
        "修改姓名",
        "修改性别",
        "修改年龄",
        "修改联系方式",
        "修改学号",
        "修改学校",
        "修改学院",
        "修改专业",
        "取消"
    };

    alter_stu_info_ui.clearConsole();
    int stu_index = searchStuInlineUI();

    bool found;

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        alter_stu_info_ui.clearConsole();

        // 渲染界面
        alter_stu_info_ui.renderLogo();
        if (stu_index == NOT_FOUND) {
            std::cout << "\n[Info]: No student information found!\n";
            found = false;
        }
        else {
            std::cout
                << "\n[Info]: Find the student's information:\n-----\n"
                << STUDENTS[stu_index].getIDNumber() << "\t"
                << STUDENTS[stu_index].getName() << "\t"
                << STUDENTS[stu_index].getGender() << "\t"
                << STUDENTS[stu_index].getAge() << "\t"
                << STUDENTS[stu_index].getContactInfo() << "\t"
                << STUDENTS[stu_index].getUniversity()->getName() << "-"
                << University::getCollegeName(STUDENTS[stu_index].getCollege()) << "-"
                << University::getMajorName(STUDENTS[stu_index].getMajor()) << "\n";
            found = true;
        }
        alter_stu_info_ui.renderMenu(alter_stu_info_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = alter_stu_info_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(alter_stu_info_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::CONSOLE_MENUS; // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= alter_stu_info_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(alter_stu_info_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    alter_stu_info_ui.clearConsole();

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0:
            if (found) {
                std::string value;
                std::cout << "$ 请输入新的身份证号码: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "id_number", GBKToUTF8(value));
                STUDENTS[stu_index].setIDNumber(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 1:
            if (found) {
                std::string value;
                std::cout << "$ 请输入新的姓名: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "name", GBKToUTF8(value));
                STUDENTS[stu_index].setName(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 2:
            if (found) {
                std::string value;
                std::cout << "$ 请输入新的性别: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "gender", GBKToUTF8(value));
                STUDENTS[stu_index].setGender(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 3:
            if (found) {
                int value;
                std::cout << "$ 请输入新的年龄: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "age", value);
                STUDENTS[stu_index].setAge(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 4:
            if (found) {
                std::string value;
                std::cout << "$ 请输入新的联系方式: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "contact_info", GBKToUTF8(value));
                STUDENTS[stu_index].setContactInfo(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 5:
            if (found) {
                std::string value;
                std::cout << "$ 请输入新的学号: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "student_id", GBKToUTF8(value));
                STUDENTS[stu_index].setStudentID(value);
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 6:
            if (found) {
                int value;
                std::cout << "$ 请输入新的学校编号: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "university_id", value);
                for (auto& univ : UNIVERSITIES) {
                    if (univ.getSchoolID() == value) {
                        STUDENTS[stu_index].setUniversity(&univ);
                        break;
                    }
                }
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 7:
            if (found) {
                int value;
                std::cout << "$ 请输入新的学院编号: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "college_id", value);
                STUDENTS[stu_index].setCollege(static_cast<University::College>(value));
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 8:
            if (found) {
                int value;
                std::cout << "$ 请输入新的专业编号: ";
                std::cin >> value;
                alterStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber(), "major_id", value);
                STUDENTS[stu_index].setMajor(static_cast<University::Major>(value));
                return Page::VIEW_STUINFO;
            }
            else {
                return Page::CONSOLE_MENUS;
            }
        case 9:
        default: return Page::CONSOLE_MENUS;;
    }
}

Page deleteStuInfoUI(ConsoleUI& delete_stu_info_ui)
{
    static const std::string delete_stu_info_logo = R"( ________     ______    _____           ____________            ________      ________      
 ___  __ \_______  /______  /_____      __  ___/_  /____  __    ____  _/_________  __/_____ 
 __  / / /  _ \_  /_  _ \  __/  _ \     _____ \_  __/  / / /     __  / __  __ \_  /_ _  __ \
 _  /_/ //  __/  / /  __/ /_ /  __/     ____/ // /_ / /_/ /     __/ /  _  / / /  __/ / /_/ /
 /_____/ \___//_/  \___/\__/ \___/      /____/ \__/ \__,_/      /___/  /_/ /_//_/    \____/ 

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^)";

    delete_stu_info_ui.setLogo(delete_stu_info_logo);

    ConsoleUI::MenuBox delete_stu_info_menus = {
        "确认",
        "取消"
    };

    delete_stu_info_ui.clearConsole();
    int stu_index = searchStuInlineUI();

    bool found;

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        delete_stu_info_ui.clearConsole();

        // 渲染界面
        delete_stu_info_ui.renderLogo();
        if (stu_index == NOT_FOUND) {
            std::cout << "\n[Info]: No student information found!\n";
            found = false;
        }
        else {
            std::cout
                << "\n[Info]: Do you want to delete the student information that was found?\n-----\n"
                << STUDENTS[stu_index].getIDNumber() << "\t"
                << STUDENTS[stu_index].getName() << "\t"
                << STUDENTS[stu_index].getGender() << "\t"
                << STUDENTS[stu_index].getAge() << "\t"
                << STUDENTS[stu_index].getContactInfo() << "\t"
                << STUDENTS[stu_index].getUniversity()->getName() << "-"
                << University::getCollegeName(STUDENTS[stu_index].getCollege()) << "-"
                << University::getMajorName(STUDENTS[stu_index].getMajor()) << "\n";
            found = true;
        }
        delete_stu_info_ui.renderMenu(delete_stu_info_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = delete_stu_info_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(delete_stu_info_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::CONSOLE_MENUS; // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= delete_stu_info_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(delete_stu_info_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: if (found) {
            bool result = deleteStudentInfo(sims_db, STUDENTS[stu_index].getIDNumber());
            STUDENTS.erase(STUDENTS.begin() + stu_index);
            return Page::VIEW_STUINFO;
        } [[fallthrough]]; // fallthrough is explicit
        case 1:
        default: return Page::CONSOLE_MENUS;
    }
}

Page addStuInfoUI(ConsoleUI& add_stu_info_ui)
{
    static const std::string add_stu_info_logo = R"( ________________________   ____________            ________      ________      
 ___    |_____  /_____  /   __  ___/_  /____  __    ____  _/_________  __/_____ 
 __  /| |  __  /_  __  /    _____ \_  __/  / / /     __  / __  __ \_  /_ _  __ \
 _  ___ / /_/ / / /_/ /     ____/ // /_ / /_/ /     __/ /  _  / / /  __/ / /_/ /
 /_/  |_\__,_/  \__,_/      /____/ \__/ \__,_/      /___/  /_/ /_//_/    \____/ 

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^)";

    add_stu_info_ui.setLogo(add_stu_info_logo);

    ConsoleUI::MenuBox add_stu_info_menus = {
        "确定",
        "取消"
    };

    bool event_loop = true;
    static int selected = 0;    // 声明为 static，从而实现保留上一次选择的效果
    int virtual_key = ConsoleUI::KEY_UNKNOWN;
    while (event_loop) {
        // 清空界面内容
        add_stu_info_ui.clearConsole();

        // 渲染界面
        add_stu_info_ui.renderLogo();
        std::string id_number;
        std::string name;
        std::string gender;
        int age;
        std::string contact_info;
        std::string student_id;
        int university_id;
        University* univ = nullptr;
        int college_id;
        int major_id;

        std::cout << "请输入身份证号: ";
        std::cin >> id_number;
        std::cout << "请输入姓名: ";
        std::cin >> name;
        std::cout << "请输入性别: ";
        std::cin >> gender;
        std::cout << "请输入年龄: ";
        std::cin >> age;
        std::cout << "请输入联系方式: ";
        std::cin >> contact_info;
        std::cout << "请输入学生号: ";
        std::cin >> student_id;
        std::cout << "请输入大学代码: ";
        std::cin >> university_id;
        std::cout << "请输入学院代码: ";
        std::cin >> college_id;
        std::cout << "请输入专业代码: ";
        std::cin >> major_id;

        for (auto& u : UNIVERSITIES) {
            if (u.getSchoolID() == university_id) {
                univ = &u;
                break;
            }
        }

        addStudentInfo(
            sims_db,
            id_number,
            name,
            gender,
            age,
            contact_info,
            student_id,
            univ,
            static_cast<University::College>(college_id),
            static_cast<University::Major>(major_id)
        );

        Student stu;
        stu.setIDNumber(id_number);
        stu.setName(name);
        stu.setGender(gender);
        stu.setAge(age);
        stu.setContactInfo(contact_info);
        stu.setUniversity(univ);
        stu.setCollege(static_cast<University::College>(college_id));
        stu.setMajor(static_cast<University::Major>(major_id));
        STUDENTS.push_back(stu);
        add_stu_info_ui.renderMenu(add_stu_info_menus, selected, 8, "\n\n", "\n");

        // 过滤不符合要求的按键事件，防止过度刷新界面
        do
        {
            virtual_key = add_stu_info_ui.getKeyboardInput();
        } while (
            virtual_key == ConsoleUI::KEY_UNKNOWN ||
            virtual_key > static_cast<int>(add_stu_info_menus.size())   // 这里一定要进行类型转换，具体原因可见我的一篇文章：https://www.seayj.cn/articles/ccac/
            );

        // 处理符合要求的按键事件
        switch (virtual_key)
        {
            case ConsoleUI::KEY_ENTER: {	// Enter 确认
                event_loop = false;
            } break;
            case ConsoleUI::KEY_ESC: {		// Esc 取消/退出
                return Page::CONSOLE_MENUS; // 手动处理 ESC 事件对应的跳转页面
            } break;
            default: {						// 需要刷新菜单选中项
                // 注意，这里应该是 vitrual_key - 1 >= add_stu_info_menus.size()，这样更容易理解。
                selected = (virtual_key > static_cast<int>(add_stu_info_menus.size()) ? selected : virtual_key - 1);
            } break;
        }
    }

    // 手动处理跳转下一页
    switch (selected)
    {
        case 0: return Page::VIEW_STUINFO;
        case 1:
        default: return Page::CONSOLE_MENUS;
    }
}

const int searchStuInlineUI()
{
    std::string search_key;
    int stu_index = NOT_FOUND;

    std::cout << "$ ID Number or Student ID: ";
    std::cin >> search_key;

    if (search_key.empty()) {
        return NOT_FOUND;
    }

    // 尝试找到第一个匹配结果
    for (int i = 0; i < static_cast<int>(STUDENTS.size()); i++) {
        if (STUDENTS[i].getIDNumber() == search_key || STUDENTS[i].getStudentID() == search_key) {
            stu_index = i;
            break;
        }
    }

    return stu_index;
}

void openWebpage(const std::string& url)
{
#ifdef _WIN32
    // Windows 系统使用 "start" 命令
    std::string command = "start " + url;
#else
    // Linux 系统使用 "xdg-open" 命令
    std::string command = "xdg-open " + url;
#endif  // !_WIN32

    system(command.c_str());
}