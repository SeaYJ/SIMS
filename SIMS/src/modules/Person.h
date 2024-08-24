#pragma once
#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <regex>

class Person
{
public:
	explicit Person();
	~Person();
	Person(const Person&) = default;
	Person& operator=(const Person&) = default;

public:
	static const size_t MAX_NAME;
	static const size_t MAX_GENDER;
	static const size_t MAX_AGE;
	static const size_t MIN_AGE;
	static const int NULL_AGE;
	static const size_t MAX_CONTACT_INFO;

	// Getter
	const std::string& getIDNumber() const;
	const std::string& getName() const;
	const std::string& getGender() const;
	const int getAge() const;
	const std::string& getContactInfo() const;

	// Setter
	void setIDNumber(const std::string& id_no);
	void setName(const std::string& name);
	void setGender(const std::string& gender);
	void setAge(const int age);
	void setContactInfo(const std::string& contact_info);

	// Others
	static bool isValidIDNumber(const std::string& id_no);
	static bool isRealPerson(const Person& p);
	bool isRealPerson() const;

private:
	std::string _id_number;
	std::string _name;
	std::string _gender;
	int _age;
	std::string _contact_info;

	static const std::regex _id_number_pattern15;
	static const std::regex _id_number_pattern18;
};

#endif // !PERSON_H

