#pragma once
#ifndef UNIVERSITY_H
#define UNIVERSITY_H

#include <string>
#include <map>
#include <vector>

#include "./Person.h"

class University
{
public:
    explicit University();
    ~University();
    University(const University&) = default;
    University& operator=(const University&) = default;

public:
    // 学院
    enum class College
    {
        UNKNOWN = -1,

        FACULTY_OF_COMPUTER_SCIENCE = 0x520,    // 计算机科学学院
        FACULTY_OF_ENGINEERING = 0x521,         // 工程学院
        FACULTY_OF_MEDICINE = 0x522,            // 医学院
        FACULTY_OF_LAW = 0x523,                 // 法学院
        FACULTY_OF_BUSINESS = 0x524,            // 商学院
        FACULTY_OF_ARTS = 0x525,                // 艺术学院
        FACULTY_OF_SOCIAL_SCIENCES = 0x526,     // 社会科学学院
        FACULTY_OF_AGRICULTURE = 0x527,         // 农学院
        FACULTY_OF_SPORTS_SCIENCE = 0x528,      // 体育科学学院
        FACULTY_OF_FINANCE = 0x529,             // 金融学院
    };

    // 专业
    enum class Major
    {
        UNKNOWN = -1,

        // 计算机科学学院
        MAJOR_COMPUTER_SCIENCE = 0x520,        // 计算机科学
        MAJOR_SOFTWARE_ENGINEERING = 0x521,    // 软件工程
        MAJOR_ARTIFICIAL_INTELLIGENCE = 0x522, // 人工智能
        MAJOR_DATA_SCIENCE = 0x523,            // 数据科学
        MAJOR_NETWORK_SECURITY = 0x524,        // 网络安全
        MAJOR_GAME_DEVELOPMENT = 0x525,        // 游戏开发
        MAJOR_COMPUTER_GRAPHICS = 0x526,       // 计算机图形学
        MAJOR_HUMAN_COMPUTER_INTERACTION = 0x527,// 人机交互
        MAJOR_CLOUD_COMPUTING = 0x528,         // 云计算

        // 工程学院
        MAJOR_MECHANICAL_ENGINEERING = 0x529,  // 机械工程
        MAJOR_ELECTRICAL_ENGINEERING = 0x52A,  // 电气工程
        MAJOR_CIVIL_ENGINEERING = 0x52B,       // 土木工程
        MAJOR_CHEMICAL_ENGINEERING = 0x52C,    // 化学工程
        MAJOR_ENVIRONMENTAL_ENGINEERING = 0x52D,// 环境工程
        MAJOR_MATERIALS_SCIENCE = 0x52E,       // 材料科学
        MAJOR_ENGINEERING_PHYSICS = 0x52F,     // 工程物理
        MAJOR_RENEWABLE_ENERGY = 0x530,        // 可再生能源
        MAJOR_ROBOTICS = 0x531,                // 机器人技术

        // 医学院
        MAJOR_MEDICINE = 0x532,                // 医学
        MAJOR_SURGERY = 0x533,                 // 外科学
        MAJOR_INTERNAL_MEDICINE = 0x534,       // 内科学
        MAJOR_PEDIATRICS = 0x535,              // 儿科学
        MAJOR_PSYCHIATRY = 0x536,              // 精神病学
        MAJOR_RADIOLOGY = 0x537,               // 放射学
        MAJOR_PATHOLOGY = 0x538,               // 病理学
        MAJOR_PUBLIC_HEALTH = 0x539,           // 公共卫生
        MAJOR_EPIDEMIOLOGY = 0x53A,            // 流行病学

        // 法学院
        MAJOR_LAW = 0x53B,                     // 法律
        MAJOR_CORPORATE_LAW = 0x53C,           // 公司法
        MAJOR_INTELLECTUAL_PROPERTY = 0x53D,   // 知识产权法
        MAJOR_CRIMINAL_LAW = 0x53E,            // 刑法
        MAJOR_FAMILY_LAW = 0x53F,              // 家庭法
        MAJOR_INTERNATIONAL_LAW = 0x540,       // 国际法
        MAJOR_ENVIRONMENTAL_LAW = 0x541,       // 环境法
        MAJOR_TAX_LAW = 0x542,                 // 税法
        MAJOR_CONTRACT_LAW = 0x543,            // 合同法

        // 商学院
        MAJOR_BUSINESS_ADMINISTRATION = 0x544, // 商业管理
        MAJOR_FINANCE = 0x545,                 // 金融
        MAJOR_MARKETING = 0x546,               // 市场营销
        MAJOR_ACCOUNTING = 0x547,              // 会计学
        MAJOR_MANAGEMENT = 0x548,              // 管理学
        MAJOR_HUMAN_RESOURCES = 0x549,         // 人力资源管理
        MAJOR_OPERATIONS_MANAGEMENT = 0x54A,   // 运营管理
        MAJOR_ENTREPRENEURSHIP = 0x54B,        // 创业
        MAJOR_STRATEGIC_MANAGEMENT = 0x54C,    // 战略管理

        // 艺术学院
        MAJOR_FINE_ARTS = 0x54D,               // 美术
        MAJOR_MUSIC = 0x54E,                   // 音乐
        MAJOR_THEATRE = 0x54F,                 // 戏剧
        MAJOR_DANCE = 0x550,                   // 舞蹈
        MAJOR_FILM = 0x551,                    // 电影
        MAJOR_PHOTOGRAPHY = 0x552,             // 摄影
        MAJOR_DESIGN = 0x553,                  // 设计
        MAJOR_ARCHITECTURE = 0x554,            // 建筑学
        MAJOR_ART_HISTORY = 0x555,             // 艺术史

        // 社会科学学院
        MAJOR_SOCIOLOGY = 0x556,               // 社会学
        MAJOR_PSYCHOLOGY = 0x557,              // 心理学
        MAJOR_POLITICAL_SCIENCE = 0x558,       // 政治学
        MAJOR_ECONOMICS = 0x559,               // 经济学
        MAJOR_ANTHROPOLOGY = 0x55A,            // 人类学
        MAJOR_GEOGRAPHY = 0x55B,               // 地理学
        MAJOR_HISTORY = 0x55C,                 // 历史学
        MAJOR_CRIMINOLOGY = 0x55D,             // 犯罪学
        MAJOR_SOCIAL_WORK = 0x55E,             // 社会工作

        // 农学院
        MAJOR_AGRONOMY = 0x55F,                // 农学
        MAJOR_PLANT_SCIENCE = 0x560,           // 植物科学
        MAJOR_ANIMAL_SCIENCE = 0x561,          // 动物科学
        MAJOR_AGRICULTURAL_ECONOMICS = 0x562,  // 农业经济学
        MAJOR_SOIL_SCIENCE = 0x563,            // 土壤学
        MAJOR_AGRICULTURAL_ENGINEERING = 0x564,// 农业工程
        MAJOR_FOOD_SCIENCE = 0x565,            // 食品科学
        MAJOR_HORTICULTURE = 0x566,            // 园艺学
        MAJOR_AGROFORESTRY = 0x567,            // 农林学

        // 体育科学学院
        MAJOR_SPORTS_SCIENCE = 0x568,          // 体育科学
        MAJOR_PHYSICAL_EDUCATION = 0x569,      // 体育教育
        MAJOR_ATHLETIC_TRAINING = 0x56A,       // 运动训练
        MAJOR_EXERCISE_SCIENCE = 0x56B,        // 运动科学
        MAJOR_SPORTS_PSYCHOLOGY = 0x56C,       // 运动心理学
        MAJOR_SPORTS_MANAGEMENT = 0x56D,       // 体育管理
        MAJOR_SPORTS_MEDICINE = 0x56E,         // 运动医学
        MAJOR_SPORTS_COACHING = 0x56F,         // 体育教练
        MAJOR_SPORTS_NUTRITION = 0x570,        // 运动营养学

        // 金融学院
        MAJOR_BANKING = 0x571,                 // 银行学
        MAJOR_FINANCIAL_ENGINEERING = 0x572,   // 金融工程
        MAJOR_ASSET_MANAGEMENT = 0x573,        // 资产管理
        MAJOR_FINANCIAL_ANALYSIS = 0x574,      // 金融分析
        MAJOR_RISK_MANAGEMENT = 0x575,         // 风险管理
        MAJOR_INVESTMENT = 0x576,              // 投资学
        MAJOR_CORPORATE_FINANCE = 0x577,       // 公司金融
        MAJOR_BEHAVIORAL_FINANCE = 0x578,      // 行为金融学
        MAJOR_INSURANCE = 0x579                // 保险学
    };

    using Majors = std::vector<Major>;
    using UniCurriculums = std::map<College, std::vector<Major>>;
    using Deaneries = std::map<College, Person>;

    static const size_t MAX_NAME;
    static const int UNKNOWN_SCHOOL_ID; // 小于等于该常数均是无效“学校代码”

    // Getter
    const int getSchoolID() const;
    const std::string getName() const;
    const UniCurriculums& getUniversityCurriculums() const;
    const Person& getPresident() const;
    const Deaneries& getUniversityDeaneries() const;

    static const std::string& getCollegeName(const College& college);
    static const std::string& getMajorName(const Major& major);

    // Setter
    void setSchoolID(const int school_id);
    void setName(const std::string& name);
    void setUniversityCurriculums(const UniCurriculums& ucs);
    void setPresident(const Person& president);
    void setUniversityDeaneries(const Deaneries& des);

    void setCollegeTranslator(const std::map<College, const std::string>& college_translator);
    void setMajorTranslator(const std::map<Major, const std::string>& major_translator);

    // Others
    static bool isRealUniversity(const University& univ);
    bool isRealUniversity() const;

private:
    int _school_id;                             // 大学代码
	std::string _name;                          // 大学名称
    UniCurriculums _university_curriculums;     // 大学课程（包含建立的“学院”及其开设的“专业”）
    Person _president;                          // 大学校长
    Deaneries _university_deaneries;            // 大学院长（建立的每个学院其对应的院长）

    static std::map<College, const std::string> _college_translator;
    static std::map<Major, const std::string> _major_translator;
};

// 所有的关于 University 的数据都应该存放在此，保持全局有且只有一份！
static std::vector<University> UNIVERSITIES = {};

#endif // !UNIVERSITY_H
