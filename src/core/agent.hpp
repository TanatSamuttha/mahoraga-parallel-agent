#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstdio>

class Agent
{
public:
    size_t MAX_AGENTS = 20;

    struct PiProcess
    {
        pid_t pid;
        int toPiWrite;
        int fromPiRead;
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

    bool initial (size_t subAgents)
    {
        for (int i = 0; i < subAgents; ++i)
        {
            int toPi[2], fromPi[2];

            if (pipe(toPi) == -1)
                return false;

            if (pipe(fromPi) == -1)
            {
                close(toPi[0]);
                close(toPi[1]);
                return false;
            }

            pid_t pid = fork();

            if (pid == -1)
            {
                perror("fork");
                close(toPi[0]);
                close(toPi[1]);
                close(fromPi[0]);
                close(fromPi[1]);
                return false;
            }

            if (pid == 0)
            {
                for (const auto& agent : agents)
                {
                    close(agent.toPiWrite);
                    close(agent.fromPiRead);
                }

                close(toPi[1]);
                close(fromPi[0]);

                if (dup2(toPi[0], STDIN_FILENO) == -1)
                    _exit(126);

                if (dup2(fromPi[1], STDOUT_FILENO) == -1)
                    _exit(126);

                close(toPi[0]);
                close(fromPi[1]);

                execl("./pi-init.sh", "pi-init.sh", "--mode", "rpc", (char*)nullptr);

                _exit(127);
            }

            close(toPi[0]);
            close(fromPi[1]);

            agents.push_back({pid, toPi[1], fromPi[0]});
        }

        return true;
    }
};
