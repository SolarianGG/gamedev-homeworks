#include <SDL2/SDL_version.h>
#include <iostream>

int main(int argc, char **argv)
{
        using namespace std;
        SDL_version compiled;
        SDL_version linked;
        SDL_version test;
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        SDL_GetVersion(&test);
        cout << "compiled: " << static_cast<int>(compiled.major) << "." << static_cast<int>(compiled.minor) << "." << static_cast<int>(compiled.patch) << endl;
        cout << "linked: " << static_cast<int>(linked.major) << "." << static_cast<int>(linked.minor) << "." << static_cast<int>(linked.patch) << endl;
        cout << "test: " << static_cast<int>(test.major) << "." << static_cast<int>(test.minor) << "." << static_cast<int>(test.patch) << endl;
        return cout.good() ? 0 : 1;
}
