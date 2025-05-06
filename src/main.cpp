#include "Engine.h"
#include "Timer.h"

int main()
{
    Timer::Start();
    // Declare an instance of Engine
    Engine engine;
    // Start the engine
    engine.run();
    // Quit in the usual way when the engine is stopped
    Timer::printData();
}
