#include "agent.hpp"

bool Agent::call(size_t subAgents, const char **prompts)
{
    size_t count = 0;
    while (count < subAgents)
    {
        for (PiProcess& agent : agents)
        {
            if (!agent.isWorking)
            {
                agent.isWorking = true;
                ++count;
            }
        }
    }

    return true;
}