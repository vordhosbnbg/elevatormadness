#include <iostream>
#include <fstream>
#include <vector>
#include <map>


struct TextArchive
{
    std::fstream fs;

    TextArchive(const std::string& filename)
    {
        fs.open(filename);
        if(!fs.is_open())
        {
            throw std::runtime_error("Error while opening file: " + filename);
        }
    }

    template<class T>
    void save(const T& val)
    {
        val.save(*this);
    }

    template<class T>
    void save(const std::vector<T>& vec)
    {
        fs << vec.size() << std::endl;
        for(const T& val : vec)
        {
            save(val);
            endSaveObject();
        }
    }

    template<class T>
    void load(T& val)
    {
        val.load(*this);
    }

    void load(int& val)
    {
        fs >> val;
    }

    void load(unsigned int& val)
    {
        fs >> val;
    }

    void load(long int& val)
    {
        fs >> val;
    }

    void load(long unsigned int& val)
    {
        fs >> val;
    }

    void load(std::string& str, bool fullLine = false)
    {
        if(fullLine)
            std::getline(fs, str);
        else
            fs >> str;
    }

    template<class T>
    void load(std::vector<T>& vec)
    {
        size_t vecSize = 0;
        load(vecSize); endLoadObject();
        vec.resize(vecSize);
        for(size_t ind = 0; ind < vecSize; ++ind)
        {
            load(vec[ind]);
        }
    }

    void endLoadObject()
    {
        fs.ignore();
    }

    void endSaveObject()
    {
        fs << std::endl;
    }
};

struct Event
{
    unsigned int turn;
    unsigned int peopleNb;
    unsigned int srcFloor;
    unsigned int dstFloor;
    unsigned int patience;

    template <class Archive>
    void save(Archive& ar) const
    {
        ar.save(turn);
        ar.save(peopleNb);
        ar.save(srcFloor);
        ar.save(dstFloor);
        ar.save(patience);
        ar.endSaveObject();
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar.load(turn);
        ar.load(peopleNb);
        ar.load(srcFloor);
        ar.load(dstFloor);
        ar.load(patience);
        ar.endLoadObject();
    }
};

struct Elevator
{
    std::string elevatorName;
    unsigned int minFloor;
    unsigned int maxFloor;
    unsigned int capacity;

    template <class Archive>
    void load(Archive& ar)
    {
        ar.load(elevatorName);
        ar.load(minFloor);
        ar.load(maxFloor);
        ar.load(capacity);
        ar.endLoadObject();
    }
};

struct Level
{
    std::string name;
    std::string description;
    unsigned int floorsNb;
    std::vector<Elevator> elevators;
    std::map<unsigned int, std::vector<Event>> timeline;

    void addEvent(const Event& ev)
    {
        timeline[ev.turn].emplace_back(ev);
    }


    template <class Archive>
    void save(Archive& ar) const
    {
        ar.save(name); ar.endSaveObject();
        ar.save(description); ar.endSaveObject();
        ar.save(floorsNb); ar.endSaveObject();
        ar.save(elevators);

        std::vector<Event> events;
        for(const std::pair<const unsigned int, std::vector<Event>>& vecForSingleTurn : timeline)
        {
            for(const Event& ev : vecForSingleTurn.second)
            {
                events.emplace_back(ev);
            }
        }

        ar.save(events);

    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar.load(name, true);
        ar.load(description, true);
        ar.load(floorsNb); ar.endLoadObject();
        ar.load(elevators);

        size_t eventsNb;

        ar.load(eventsNb); ar.endLoadObject();
        for(size_t eventId = 0; eventId < eventsNb; ++eventId)
        {
            Event ev;
            ar.load(ev);
            addEvent(ev);
        }
    }
};

struct Person
{
    unsigned int position = 0;
    unsigned int srcFloor = 0;
    unsigned int dstFloor = 0;
    unsigned int patience = 0;
    bool inElevator = false;
};

struct Game
{
    Game(const std::string& logfile, const std::string& levelfile)
    {
        log.open(logfile, std::ios_base::out);
        if(!log.is_open())
        {
            throw std::runtime_error("Error while opening log file for writing: " + logfile);
        }
        loadLevel(levelfile);
    }

    std::fstream log;
    Level level;
    size_t turn = 0;
    std::vector<Person> people;

    void loadLevel(const std::string& filename)
    {
        log << "Opening level from : " << filename << std::endl;
        TextArchive ar(filename);
        ar.load(level);
        log << "Level loaded." << std::endl;
    }

    void processEvent(const Event& ev)
    {
        log << "Processing event for turn " << turn << std::endl;
        log << ev.peopleNb << " new people calling on floor " << ev.srcFloor
            << " to floor " << ev.dstFloor
            << " with patience " << ev.patience << std::endl;

        for(unsigned int idx = 0; idx < ev.peopleNb; ++idx)
        {
            Person p;
            p.position = ev.srcFloor;
            p.srcFloor = ev.srcFloor;
            p.dstFloor = ev.dstFloor;
            p.patience = ev.patience;
            people.emplace_back(p);
        }
    }

    void runEventsForTurn()
    {
        auto eventVecIt = level.timeline.find(turn);

        if(eventVecIt != level.timeline.end())
        {
            const std::vector<Event>& eventsForTurn = eventVecIt->second;

            for(const Event& ev : eventsForTurn)
            {
                processEvent(ev);
            }

            level.timeline.erase(eventVecIt);
        }
    }

    void run()
    {
        do
        {
            runEventsForTurn();
            ++turn;
        }
        while(!level.timeline.empty());

    }
};

int main(int argc, char* argv[])
{
    if(argc == 3)
    {
        Game game(argv[1], argv[2]);
        game.run();
    }
    return 0;
}
