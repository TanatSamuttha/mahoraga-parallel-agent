#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstdio>
#include <string>
#include <mutex>

class Agent
{
private:
    struct PiProcess
    {
        size_t id;
        pid_t pid;
        int toPiWrite;
        int fromPiRead;
        bool isWorking = false;
    };

    struct JSONCarry
    {
        size_t bracket = 0;
        bool isNew = false;
        bool isType = false;
        std::string carry;
    };

    std::vector<PiProcess> agents;
    std::vector<JSONCarry> jsonCarries;
    std::mutex blocker;

public:
    ~Agent ()
    {
        stop();
    }

    bool initial (const size_t subAgents);
    
    void stop ()
    {
        for (auto& agent : agents)
        {
            close(agent.toPiWrite);
            close(agent.fromPiRead);
            kill(agent.pid, SIGTERM);
        }
        agents.clear();
        jsonCarries.clear();
    }

    bool call (size_t tasks, const char* const* prompts, const size_t promptSize[]);

    bool receive ();
};
