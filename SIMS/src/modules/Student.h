#pragma once
#ifndef STUDENT_H
#define STUDENT_H

#include <vector>

#include "./Person.h"
#include "./University.h"

class Student :
    public Person
{
public:
    explicit Student();
    ~Student();

    /*
    * 注意 const University* _university; 虽然为指针，
    * 但是实际上并不需要做深拷贝操作。
    * 因为， University 被设计为全局只存在一份（数据存放在 UNIVERSITIES 变量中，具体可见 University.h 文件），
    * 也就是说，只需要一个指针指向对应的大学对象即可。
    * 
    * 释义：做深拷贝的目的是方式同一个类的不同实例化对象的指针指向了同一个内容，
    * 从而导致其中一个实例化对象通过修改指针指向的内容“不小心”将另一个实例化对象的指针指向的内容也修改了（本质上二者指向的就是同一个内容）！
    * 而这里，每个大学作为一个对象存在，其发生变化一定是所有在该大学上学的学生的学校发生了变化，
    * 而不是某一个学生的大学发生了变化！
    */
    Student(const Student&) = default;
    Student& operator=(const Student&) = default;

public:
    static const size_t MAX_STU_ID;

    // Getter
    const std::string& getStudentID() const;
    const University* const getUniversity() const;
    const University::College& getCollege() const;
    const University::Major& getMajor() const;

    // Setter
    void setStudentID(const std::string& stu_id);
    void setUniversity(const University* const univ);
    void setCollege(const University::College& college);
    void setMajor(const University::Major& major);

private:
    std::string _student_id;
    const University* _university;
    University::College _college;
    University::Major _major;
};

// 所有的关于 Student 的数据都应该存放在此，保持全局有且只有一份！
static std::vector<Student> STUDENTS = {};

#endif // !STUDENT_H


