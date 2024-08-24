#include "Student.h"

const size_t Student::MAX_STU_ID = 16;

Student::Student()
	: _student_id(""),
	_university(nullptr),
	_college(University::College::UNKNOWN),
	_major(University::Major::UNKNOWN)
{
}

Student::~Student()
{
}

const std::string& Student::getStudentID() const
{
	return _student_id;
}

const University* const Student::getUniversity() const
{
	return _university;
}

const University::College& Student::getCollege() const
{
	return _college;
}

const University::Major& Student::getMajor() const
{
	return _major;
}

void Student::setStudentID(const std::string& stu_id)
{
	if (stu_id.empty() || stu_id.size() > MAX_STU_ID) {
		return;
	}

	_student_id = stu_id;
}

void Student::setUniversity(const University* const univ)
{
	if (!univ->isRealUniversity()) {
		return;
	}

	_university = univ;
}

void Student::setCollege(const University::College& college)
{
	_college = college;
}

void Student::setMajor(const University::Major& major)
{
	_major = major;
}
