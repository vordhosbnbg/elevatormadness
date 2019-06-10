#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        std::fstream levelFile(argv[1], std::fstream::in);
    }
    return 0;
}
