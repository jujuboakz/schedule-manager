#ifndef TASK_H
#define TASK_H

#include <string>


enum class Priority
{
    HIGH,
    MEDIUM,
    LOW
};


enum class Category
{
    STUDY,
    ENTERTAINMENT,
    LIFE
};



class Task
{

public:

    Task(
        int id,
        const std::string& name,
        const std::string& startTime,
        const std::string& remindTime,
        Priority priority = Priority::MEDIUM,
        Category category = Category::LIFE,
        bool completed = false
    );


    int getId() const;

    std::string getName() const;

    std::string getStartTime() const;


    void print() const;

    std::string priorityToString() const;

    std::string categoryToString() const;


private:

    int id;

    std::string name;

    std::string startTime;

    Priority priority;

    Category category;

    std::string remindTime;

    bool completed;

};


#endif