#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <list>


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

struct Person
{
    static constexpr unsigned int scoreCoefFloors = 10;
    static constexpr unsigned int scoreCoefPatience = 20;
    unsigned int srcFloor = 0;
    unsigned int dstFloor = 0;
    int patience = 0;
    bool inElevator = false;
    bool gotToDestination = false;
    std::string expectedElevator;

    int getScore()
    {
        if(!inElevator)
        {
            return 0;
        }
        else
        {
            return (std::abs((int)(dstFloor - srcFloor)) * scoreCoefFloors + patience * scoreCoefPatience);
        }
    }
};



struct Elevator
{
    static constexpr unsigned int acceleration = 2;
    static constexpr unsigned int floorHeight = 4;
    std::string elevatorName;
    unsigned int minFloor = 0;
    unsigned int maxFloor = 0;
    unsigned int capacity = 0;
    int heightMeters = 0;
    int speed = 0;
    int command = 0;
    std::list<Person*> peopleInside;

    void accelerateDown()
    {
        command = -1;
    }

    void accelerateUp()
    {
        command = 1;
    }

    void stop()
    {
        command = 0;
    }

    void processTime()
    {
        speed += acceleration * command;
        heightMeters += speed;
    }

    int getFloor()
    {
        if(heightMeters % floorHeight != 0)
        {
            return -1;
        }
        else
        {
            return heightMeters / floorHeight;
        }
    }

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
    unsigned int floorsNb = 0;
    std::unordered_map<std::string, Elevator> elevators;
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
        std::vector<Elevator> elevatorsVec;
        ar.load(elevatorsVec);

        for(Elevator& elevator : elevatorsVec)
        {
            elevators[elevator.elevatorName] = elevator;
        }

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
    std::list<Person> people;
    size_t turn = 0;
    unsigned int score = 0;

    void loadLevel(const std::string& filename)
    {
        log << "Opening level from : " << filename << std::endl;
        TextArchive ar(filename);
        ar.load(level);
        log << "Level loaded: " << level.name << " (" << level.description << ")" << std::endl;
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
            p.srcFloor = ev.srcFloor;
            p.dstFloor = ev.dstFloor;
            p.patience = ev.patience;
            people.emplace_back(p);
        }
    }

    void spawnEventsForTurn()
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

    bool processTime()
    {
        for(std::pair<std::string, Elevator> elevatorPair : level.elevators)
        {
            Elevator& elevator = elevatorPair.second;

            elevator.processTime();
            log << "Elevator " << elevator.elevatorName <<
                   " at height " << elevator.heightMeters << " m" <<
                   " moving with speed " << elevator.speed << " m/s" <<
                   " and acceleration " << (elevator.acceleration * elevator.command) << " m/s^2" << std::endl;

            if((elevator.heightMeters < 0) ||
               (elevator.heightMeters > (level.floorsNb * Elevator::floorHeight))) // out of the building
            {
                log << "Elevator " << elevator.elevatorName << " has crashed out of the building. " << elevator.peopleInside.size() << " people died.";
                score = 0;
                return false;
            }
            else if(elevator.getFloor() >= 0 && elevator.speed == 0) // elevator on a floor and not moving
            {
                auto personIt = elevator.peopleInside.begin();

                while(personIt != elevator.peopleInside.end())
                {
                    Person& person = **personIt;
                    if(person.dstFloor == elevator.getFloor())
                    {
                        person.inElevator = false;
                        person.gotToDestination = true;
                        personIt = elevator.peopleInside.erase(personIt);
                    }
                    else
                    {
                        ++personIt;
                    }
                }
            }
        }

        auto personIt = people.begin();

        while(personIt != people.end())
        {
            Person& person = *personIt;
            if(person.gotToDestination)
            {
                unsigned int personScore = person.getScore();
                log << "A person got from floor " << person.srcFloor <<
                       " to floor " << person.dstFloor <<
                       " with remaining patience " << person.patience;

                log << "Given score: +" << personScore;
                score += personScore;
                personIt = people.erase(personIt);
            }
            else if(person.patience <= 0)
            {
                log << "A person on floor " << person.srcFloor <<
                       " got tired on waiting for elevator " << person.expectedElevator <<
                       ". No score awarded." << std::endl;
                personIt = people.erase(personIt);
            }
            else
            {
                if(!person.inElevator)
                {
                    if(person.expectedElevator.empty())
                    {
                        log << "No elevator response on floor " << person.srcFloor << std::endl;
                        score = 0;
                        return false;
                    }
                    else if(person.srcFloor == level.elevators[person.expectedElevator].getFloor())
                    {
                        log << "A person entered elevator " << person.expectedElevator <<
                               " on floor " << person.srcFloor << std::endl;
                        person.inElevator = true;
                        level.elevators[person.expectedElevator].peopleInside.emplace_back(&person);
                    }
                    else
                    {
                        --person.patience;
                    }
                }

                ++personIt;
            }
        }

        return true;
    }

    void run()
    {
        do
        {
            spawnEventsForTurn();
            if(!processTime())
            {
                break;
            }
            ++turn;
        }
        while(!level.timeline.empty() && !people.empty());

        log << "GAME OVER\nTotal Score:\n" << score << std::endl;
    }
};

int main(int argc, char* argv[])
{
    if(argc == 3)
    {
        try
        {
            Game game(argv[1], argv[2]);
            game.run();
        }
        catch (std::exception& e)
        {
            std::cout << "Critical error: " << e.what() << std::endl;
        }
    }
    return 0;
}
