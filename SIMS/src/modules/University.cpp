#include "University.h"

const size_t University::MAX_NAME = 1024;
const int University::UNKNOWN_SCHOOL_ID = -1;

University::University()
	: _school_id(University::UNKNOWN_SCHOOL_ID),
    _name("")
{
}

University::~University()
{
}

const int University::getSchoolID() const
{
    return _school_id;
}

const std::string University::getName() const
{
	return _name;
}

const University::UniCurriculums& University::getUniversityCurriculums() const
{
	return _university_curriculums;
}

const Person& University::getPresident() const
{
	return _president;
}

const University::Deaneries& University::getUniversityDeaneries() const
{
	return _university_deaneries;
}

const std::string& University::getCollegeName(const College& college)
{
    return _college_translator[college];
}

const std::string& University::getMajorName(const Major& major)
{
    return _major_translator[major];
}

void University::setSchoolID(const int school_id)
{
    if (school_id <= UNKNOWN_SCHOOL_ID) {
        return;
    }

    _school_id = school_id;
}

void University::setName(const std::string& name)
{
	if (name.empty() || name.size() > University::MAX_NAME) {
		return;
	}

	_name = name;
}

void University::setUniversityCurriculums(const UniCurriculums& ucs)
{
	if (ucs.empty()) {
		return;
	}

	_university_curriculums = ucs;
}

void University::setPresident(const Person& president)
{
	if (!Person::isRealPerson(president)) {
		return;
	}

	_president = president;
}

void University::setUniversityDeaneries(const Deaneries& des)
{
	if (des.empty()) {
		return;
	}

	_university_deaneries = des;
}

void University::setCollegeTranslator(const std::map<College, const std::string>& college_translator)
{
    if (college_translator.size() != _college_translator.size()) {
        return;
    }

    for (const auto& item : _college_translator) {
        // 对应“键”必须存在
        auto it = college_translator.find(item.first);
        if (it == college_translator.end()) {
            return;
        }

        // 对应“键”的翻译必须不为空
        if (it->second.empty()) {
            return;
        }
    }
    
    _college_translator = college_translator;
}

void University::setMajorTranslator(const std::map<Major, const std::string>& major_translator)
{
    if (major_translator.size() != _major_translator.size()) {
        return;
    }

    for (const auto& item : _major_translator) {
        // 对应“键”必须存在
        auto it = major_translator.find(item.first);
        if (it == major_translator.end()) {
            return;
        }

        // 对应“键”的翻译必须不为空
        if (it->second.empty()) {
            return;
        }
    }

    _major_translator = major_translator;
}

bool University::isRealUniversity(const University& univ)
{
    // 学校代码不正确
    if (univ.getSchoolID() <= UNKNOWN_SCHOOL_ID) {
        return false;
    }

	// 学校名称必须不为空
	if (univ.getName().empty()) {
		return false;
	}

	const UniCurriculums& temp_ucs =  univ.getUniversityCurriculums();

	// 至少包含一个学院
	if (temp_ucs.size() <= 0) {
		return false;
	}

	// 每个学院至少一个专业
	for (const auto& temp_ucs_item : temp_ucs) {
		if (temp_ucs_item.second.size() <= 0) {
			return false;
		}
	}

	// 校长必须真实存在
	if (!univ.getPresident().isRealPerson()) {
		return false;
	}

	const Deaneries& temp_des = univ.getUniversityDeaneries();

	// 学院数量必须和院长数量对应上
	if (temp_des.size() != temp_ucs.size()) {
		return false;
	}

	// 每个学院的负责人必须真实存在
	for (const auto& temp_des_item : temp_des) {
		if (!temp_des_item.second.isRealPerson()) {
			return false;
		}
	}

	return true;
}

bool University::isRealUniversity() const
{
	return University::isRealUniversity(*this);
}

std::map<University::College, const std::string> University::_college_translator = {
    { University::College::UNKNOWN, "未知" },
    { University::College::FACULTY_OF_COMPUTER_SCIENCE, "计算机科学学院" },
    { University::College::FACULTY_OF_ENGINEERING, "工程学院" },
    { University::College::FACULTY_OF_MEDICINE, "医学院" },
    { University::College::FACULTY_OF_LAW, "法学院" },
    { University::College::FACULTY_OF_BUSINESS, "商学院" },
    { University::College::FACULTY_OF_ARTS, "艺术学院" },
    { University::College::FACULTY_OF_SOCIAL_SCIENCES, "社会科学学院" },
    { University::College::FACULTY_OF_AGRICULTURE, "农学院" },
    { University::College::FACULTY_OF_SPORTS_SCIENCE, "体育科学学院" },
    { University::College::FACULTY_OF_FINANCE, "金融学院" }
};

std::map<University::Major, const std::string> University::_major_translator = {
    { University::Major::UNKNOWN, "未知" },

    // 计算机科学学院
    { University::Major::MAJOR_COMPUTER_SCIENCE, "计算机科学" },
    { University::Major::MAJOR_SOFTWARE_ENGINEERING, "软件工程" },
    { University::Major::MAJOR_ARTIFICIAL_INTELLIGENCE, "人工智能" },
    { University::Major::MAJOR_DATA_SCIENCE, "数据科学" },
    { University::Major::MAJOR_NETWORK_SECURITY, "网络安全" },
    { University::Major::MAJOR_GAME_DEVELOPMENT, "游戏开发" },
    { University::Major::MAJOR_COMPUTER_GRAPHICS, "计算机图形学" },
    { University::Major::MAJOR_HUMAN_COMPUTER_INTERACTION, "人机交互" },
    { University::Major::MAJOR_CLOUD_COMPUTING, "云计算" },

    // 工程学院
    { University::Major::MAJOR_MECHANICAL_ENGINEERING, "机械工程" },
    { University::Major::MAJOR_ELECTRICAL_ENGINEERING, "电气工程" },
    { University::Major::MAJOR_CIVIL_ENGINEERING, "土木工程" },
    { University::Major::MAJOR_CHEMICAL_ENGINEERING, "化学工程" },
    { University::Major::MAJOR_ENVIRONMENTAL_ENGINEERING, "环境工程" },
    { University::Major::MAJOR_MATERIALS_SCIENCE, "材料科学" },
    { University::Major::MAJOR_ENGINEERING_PHYSICS, "工程物理" },
    { University::Major::MAJOR_RENEWABLE_ENERGY, "可再生能源" },
    { University::Major::MAJOR_ROBOTICS, "机器人技术" },

    // 医学院
    { University::Major::MAJOR_MEDICINE, "医学" },
    { University::Major::MAJOR_SURGERY, "外科学" },
    { University::Major::MAJOR_INTERNAL_MEDICINE, "内科学" },
    { University::Major::MAJOR_PEDIATRICS, "儿科学" },
    { University::Major::MAJOR_PSYCHIATRY, "精神病学" },
    { University::Major::MAJOR_RADIOLOGY, "放射学" },
    { University::Major::MAJOR_PATHOLOGY, "病理学" },
    { University::Major::MAJOR_PUBLIC_HEALTH, "公共卫生" },
    { University::Major::MAJOR_EPIDEMIOLOGY, "流行病学" },

    // 法学院
    { University::Major::MAJOR_LAW, "法律" },
    { University::Major::MAJOR_CORPORATE_LAW, "公司法" },
    { University::Major::MAJOR_INTELLECTUAL_PROPERTY, "知识产权法" },
    { University::Major::MAJOR_CRIMINAL_LAW, "刑法" },
    { University::Major::MAJOR_FAMILY_LAW, "家庭法" },
    { University::Major::MAJOR_INTERNATIONAL_LAW, "国际法" },
    { University::Major::MAJOR_ENVIRONMENTAL_LAW, "环境法" },
    { University::Major::MAJOR_TAX_LAW, "税法" },
    { University::Major::MAJOR_CONTRACT_LAW, "合同法" },

    // 商学院
    { University::Major::MAJOR_BUSINESS_ADMINISTRATION, "商业管理" },
    { University::Major::MAJOR_FINANCE, "金融" },
    { University::Major::MAJOR_MARKETING, "市场营销" },
    { University::Major::MAJOR_ACCOUNTING, "会计学" },
    { University::Major::MAJOR_MANAGEMENT, "管理学" },
    { University::Major::MAJOR_HUMAN_RESOURCES, "人力资源管理" },
    { University::Major::MAJOR_OPERATIONS_MANAGEMENT, "运营管理" },
    { University::Major::MAJOR_ENTREPRENEURSHIP, "创业" },
    { University::Major::MAJOR_STRATEGIC_MANAGEMENT, "战略管理" },

    // 艺术学院
    { University::Major::MAJOR_FINE_ARTS, "美术" },
    { University::Major::MAJOR_MUSIC, "音乐" },
    { University::Major::MAJOR_THEATRE, "戏剧" },
    { University::Major::MAJOR_DANCE, "舞蹈" },
    { University::Major::MAJOR_FILM, "电影" },
    { University::Major::MAJOR_PHOTOGRAPHY, "摄影" },
    { University::Major::MAJOR_DESIGN, "设计" },
    { University::Major::MAJOR_ARCHITECTURE, "建筑学" },
    { University::Major::MAJOR_ART_HISTORY, "艺术史" },

    // 社会科学学院
    { University::Major::MAJOR_SOCIOLOGY, "社会学" },
    { University::Major::MAJOR_PSYCHOLOGY, "心理学" },
    { University::Major::MAJOR_POLITICAL_SCIENCE, "政治学" },
    { University::Major::MAJOR_ECONOMICS, "经济学" },
    { University::Major::MAJOR_ANTHROPOLOGY, "人类学" },
    { University::Major::MAJOR_GEOGRAPHY, "地理学" },
    { University::Major::MAJOR_HISTORY, "历史学" },
    { University::Major::MAJOR_CRIMINOLOGY, "犯罪学" },
    { University::Major::MAJOR_SOCIAL_WORK, "社会工作" },

    // 农学院
    { University::Major::MAJOR_AGRONOMY, "农学" },
    { University::Major::MAJOR_PLANT_SCIENCE, "植物科学" },
    { University::Major::MAJOR_ANIMAL_SCIENCE, "动物科学" },
    { University::Major::MAJOR_AGRICULTURAL_ECONOMICS, "农业经济学" },
    { University::Major::MAJOR_SOIL_SCIENCE, "土壤学" },
    { University::Major::MAJOR_AGRICULTURAL_ENGINEERING, "农业工程" },
    { University::Major::MAJOR_FOOD_SCIENCE, "食品科学" },
    { University::Major::MAJOR_HORTICULTURE, "园艺学" },
    { University::Major::MAJOR_AGROFORESTRY, "农林学" },

    // 体育科学学院
    { University::Major::MAJOR_SPORTS_SCIENCE, "体育科学" },
    { University::Major::MAJOR_PHYSICAL_EDUCATION, "体育教育" },
    { University::Major::MAJOR_ATHLETIC_TRAINING, "运动训练" },
    { University::Major::MAJOR_EXERCISE_SCIENCE, "运动科学" },
    { University::Major::MAJOR_SPORTS_PSYCHOLOGY, "运动心理学" },
    { University::Major::MAJOR_SPORTS_MANAGEMENT, "体育管理" },
    { University::Major::MAJOR_SPORTS_MEDICINE, "运动医学" },
    { University::Major::MAJOR_SPORTS_COACHING, "体育教练" },
    { University::Major::MAJOR_SPORTS_NUTRITION, "运动营养学" },

    // 金融学院
    { University::Major::MAJOR_BANKING, "银行学" },
    { University::Major::MAJOR_FINANCIAL_ENGINEERING, "金融工程" },
    { University::Major::MAJOR_ASSET_MANAGEMENT, "资产管理" },
    { University::Major::MAJOR_FINANCIAL_ANALYSIS, "金融分析" },
    { University::Major::MAJOR_RISK_MANAGEMENT, "风险管理" },
    { University::Major::MAJOR_INVESTMENT, "投资学" },
    { University::Major::MAJOR_CORPORATE_FINANCE, "公司金融" },
    { University::Major::MAJOR_BEHAVIORAL_FINANCE, "行为金融学" },
    { University::Major::MAJOR_INSURANCE, "保险学" }
};

