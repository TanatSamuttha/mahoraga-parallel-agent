#include "agent.hpp"
#include "iostream"

bool Agent::initial(size_t subAgents)
{
    if (status != 0)
    {
        std::cerr << "Pi dependency setup failed\n";
        return false;
    }

    std::cout << "Done\n";
    agents.reserve(subAgents);

    for (size_t i = 0; i < subAgents; ++i)
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

            execl("/bin/bash", "bash", "./src/setup/pi-init.sh", "--mode", "rpc", (char*)nullptr);

            std::cout << "Exit\n";
            _exit(127);
        }

        close(toPi[0]);
        close(fromPi[1]);

        agents.push_back({i, pid, toPi[1], fromPi[0]});
    }

    jsonCarries.resize(agents.size());

    return true;
}