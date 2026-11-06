#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstdio>

class Agent
{
public:
    struct PiProcess
    {
        pid_t pid;
        int toPiWrite;
        int fromPiRead;
        bool isWorking;
    };

    std::vector<PiProcess> agents;

    ~Agent ()
    {
        for (auto& agent : agents)
        {
            close(agent.toPiWrite);
            close(agent.fromPiRead);
            kill(agent.pid, SIGTERM);
        }
    }

    bool initial (const size_t subAgents);

    bool call (size_t subAgents, const char* prompts[]);
};
