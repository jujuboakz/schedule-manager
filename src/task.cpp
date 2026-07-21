#include "task.h"
#include <iostream>
#include <iomanip>


Task::Task(
        int id,
        const std::string& name,
        const std::string& startTime,
        const std::string& remindTime,
        Priority priority,
        Category category,
        bool completed
):
    id(id),
    name(name),
    startTime(startTime),
    remindTime(remindTime),
    priority(priority),
    category(category){};


int Task::getId() const{
    return id;
};

std::string Task::getName() const{
    return name;
};

std::string Task::getStartTime() const{
    return startTime;
};


void Task::print() const
{
    std::cout << std::left;

    std::cout << std::setw(12) << "id" 
              << ": " << id << std::endl;

    std::cout << std::setw(12) << "name" 
              << ": " << name << std::endl;

    std::cout << std::setw(12) << "startTime" 
              << ": " << startTime << std::endl;

    std::cout << std::setw(12) << "remindTime" 
              << ": " << remindTime << std::endl;

    std::cout << std::setw(12) << "priority" 
              << ": " << priorityToString() << std::endl;

    std::cout << std::setw(12) << "category" 
              << ": " << categoryToString() << std::endl;
}

std::string Task::priorityToString() const
{
    switch(priority)
    {
        case Priority::HIGH:
            return "HIGH";

        case Priority::MEDIUM:
            return "MEDIUM";

        case Priority::LOW:
            return "LOW";
    }

    return "";
}

std::string Task::categoryToString() const
{
    switch(category)
    {
        case Category::STUDY:
            return "STUDY";

        case Category::ENTERTAINMENT:
            return "ENTERTAINMENT";

        case Category::LIFE:
            return "LIFE";
    }

    return "";
}