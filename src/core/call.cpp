#include "agent.hpp"
#include <iostream>

bool Agent::call(size_t tasks, const char* const* prompts, const size_t promptSize[])
{
    blocker.lock();

    size_t pending = 0;
    size_t done = 0;
    while (done < tasks)
    {
        for (PiProcess& agent : agents)
        {
            if (!agent.isWorking && pending < tasks)
            {
                std::string json1 = "{\"type\":\"prompt\",\"message\":\"";
                std::string json2 = "\"}\n";
                std::string prompt;
                prompt.reserve(json1.size() + json2.size() + promptSize[pending]);
                prompt += json1;
                prompt += prompts[pending];
                prompt += json2;

                std::cout << prompt << '\n';
                int x = write(agent.toPiWrite, prompt.data(), prompt.size());
                if (x != -1)
                {
                    agent.isWorking = true;
                    ++pending;
                }
                else
                {
                    stop();
                    return false;
                }
            }
            else if (agent.isWorking)
            {
                char buffer[128];
                ssize_t size = read(agent.fromPiRead, buffer, 128);
                if (size != -1)
                {
                    for (ssize_t i = 0; i < size; ++i)
                    {
                        if (buffer[i] == '{')
                        {
                            ++jsonCarries[agent.id].bracket;
                            if (jsonCarries[agent.id].bracket == 1)
                            {
                                jsonCarries[agent.id].isNew = true;
                                jsonCarries[agent.id].isType = false;
                            }
                            else
                            {
                                jsonCarries[agent.id].isNew = false;
                                jsonCarries[agent.id].isType = false;
                            }
                        }
                        else if (buffer[i] == '}')
                        {
                            --jsonCarries[agent.id].bracket;
                            jsonCarries[agent.id].isNew = false;
                            jsonCarries[agent.id].isType = false;
                        }
                        else if (jsonCarries[agent.id].isNew)
                        {
                            if (buffer[i] == ':')
                            {
                                jsonCarries[agent.id].isType = true;
                                jsonCarries[agent.id].carry.clear();
                            }
                            else if (jsonCarries[agent.id].isType && buffer[i] != '\"' && buffer[i] != ',')
                            {
                                jsonCarries[agent.id].carry.push_back(buffer[i]);
                            }
                            else if (jsonCarries[agent.id].isType && buffer[i] == ',') {
                                jsonCarries[agent.id].isType = false;
                                jsonCarries[agent.id].isNew = false;
                                if (jsonCarries[agent.id].carry == "agent_settled")
                                {
                                    ++done;
                                    agent.isWorking = false;
                                }
                            }
                        }
                    }
                }
                else
                {
                    stop();
                    return false;
                }
            }
        }
    }

    blocker.unlock();

    return true;
}