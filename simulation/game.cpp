#include <iostream>
#include <fstream>
#include <vector>
#include <map>

struct Event
{
    unsigned int turn;
    unsigned int peopleNb;
    unsigned int srcFloor;
    unsigned int dstFloor;
    unsigned int paitence;
};


struct TextArchive
{
    TextArchive(const std::string& filename);

    template<class T>
    void save(const T& val)
    {
        std::cout << val << " ";
    }

    template<class T>
    void save(const std::vector<T> vec)
    {
        std::cout << vec.size() << std::endl;
        for(const T& val : vec)
        {
            save(val);
            std::cout << std::endl;
        }
    }

    template<class T>
    void save(const T& val)
    {
        val.load(*this);
    }

    template<class T>
    void load(T& val)
    {
        std::cin >> val;
    }

    void load(std::string& str, bool fullLine = false)
    {
        if(fullLine)
            std::getline(std::cin, str);
        else
            std::cin >> str;
    }

    template<class T>
    void load(std::vector<T> vec)
    {
        size_t vecSize = 0;
        std::cin >> vecSize;
        for(size_t ind = 0; ind < vecSize; ++ind)
        {
            load(vec[ind]);
            std::cin.ignore();
        }

        for(const T& val : vec)
        {
            save(val);
            std::cout << std::endl;
        }
    }
};

struct Elevator
{
    std::string elevatorName;
    unsigned int minFloor;
    unsigned int maxFloor;
    unsigned int capacity;
};

struct Level
{
    std::string name;
    std::string description;
    unsigned int floorsNb;
    unsigned int elevatorsNb;
    std::vector<std::string> elevators;
    std::map<unsigned int, std::vector<Event>> timeline;


    template <class Archive>
    void save(Archive& ar)
    {
        ar.save(name, true); std::cout << std::endl;
        ar.save(description, true); std::cout << std::endl;
        ar.save(floorsNb); std::cout << std::endl;
        ar.save(elevatorsNb); std::cout << std::endl;
        ar.save(elevators); std::cout << std::endl;

        std::vector<Event> events;
        for(const std::pair<const unsigned int, std::vector<Event>>& vecForSingleTurn : timeline)
        {
            for(const Event& ev : vecForSingleTurn.second)
            {
                events.emplace_back(ev);
            }
        }

        ar.save(events.size()); std::cout << std::endl;
        ar.save(events);

    }
};

struct Game
{
    Level level;

    void loadLevel(const char * filename)
    {
        TextArchive ar(filename);
        ar.load(level);
    }
};

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
    }
    return 0;
}
