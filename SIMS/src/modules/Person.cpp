#include "Person.h"

#include <iostream>

const size_t Person::MAX_NAME = 256;
const size_t Person::MAX_GENDER = 128;
const size_t Person::MAX_AGE = 200;
const size_t Person::MIN_AGE = 0;
const int Person::NULL_AGE = -1;
const size_t Person::MAX_CONTACT_INFO = 512;

const std::regex Person::_id_number_pattern15("^([1-6][1-9]|50)\\d{4}\\d{2}((0[1-9])|10|11|12)(([0-2][1-9])|10|20|30|31)\\d{3}$");
const std::regex Person::_id_number_pattern18("^([1-6][1-9]|50)\\d{4}(18|19|20)\\d{2}((0[1-9])|10|11|12)(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$");

Person::Person()
	: _id_number(""),
	_name(""),
	_gender(""),
	_age(Person::NULL_AGE),
	_contact_info("")
{
}

Person::~Person()
{
}

const std::string& Person::getIDNumber() const
{
	return _id_number;
}

const std::string& Person::getName() const
{
	return _name;
}

const std::string& Person::getGender() const
{
	return _gender;
}

const int Person::getAge() const
{
	return _age;
}

const std::string& Person::getContactInfo() const
{
	return _contact_info;
}

void Person::setIDNumber(const std::string& id_no)
{
	if (id_no.empty()) {
		return;
	}

	_id_number = id_no;
}

void Person::setName(const std::string& name)
{
	if (name.empty() || name.size() > Person::MAX_NAME) {
		return;
	}

	_name = name;
}

void Person::setGender(const std::string& gender)
{
	if (gender.empty() || gender.size() > Person::MAX_GENDER) {
		return;
	}

	_gender = gender;
}

void Person::setAge(const int age)
{
	if (age < Person::MIN_AGE || age > Person::MAX_AGE) {
		return;
	}

	_age = age;
}

void Person::setContactInfo(const std::string& contact_info)
{
	if (contact_info.empty() || contact_info.size() > Person::MAX_CONTACT_INFO) {
		return;
	}

	_contact_info = contact_info;
}

bool Person::isValidIDNumber(const std::string& id_no)
{
	return std::regex_match(id_no, _id_number_pattern15) ||
		std::regex_match(id_no, _id_number_pattern18);
}

bool Person::isRealPerson(const Person& p)
{
	// 身份证号码必须正确 && 姓名必须不为空
	return isValidIDNumber(p.getIDNumber()) && !(p.getName().empty());
}

bool Person::isRealPerson() const
{
	return Person::isRealPerson(*this);
}
