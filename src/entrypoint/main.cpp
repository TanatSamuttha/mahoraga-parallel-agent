#include "core/agent.hpp"
#include <iostream>
#include <csignal>
#include <wait.h>
#include <chrono>

int main ()
{
    std::signal(SIGPIPE, SIG_IGN);
    Agent agent;

    agent.initial(5);

    std::string prompt1 = "write c++ code that input a and b then print a + b. Create file in /home/nine/Projects/Coding/mahoraga-parallel-agent/test/abplus.cpp";
    std::string prompt2 = "write c++ code that input a and b then print a / b. Create file in /home/nine/Projects/Coding/mahoraga-parallel-agent/test/abdiv.cpp";
    std::string prompt3 = "Write a C++ program that performs matrix multiplication.The program should:- Read the dimensions of two matrices.- Read all elements of both matrices.- Check whether the matrices can be multiplied.- Multiply the matrices using an efficient algorithm.- Print the resulting matrix in a readable format.- Handle invalid dimensions gracefully.Use std::vector for dynamic memory management and long long for matrix elements.Do not use external libraries.Create the file at:/home/nine/Projects/Coding/mahoraga-parallel-agent/test/matrix.cpp";
    std::string prompt4 = "Write a C++ program that solves a maze using Breadth-First Search (BFS).The program should:- Read the height and width of a grid.- Read the grid, where:'#' represents a wall,'.' represents an open cell,'S' represents the starting position,'E' represents the destination.- Find the shortest path from S to E.- Allow movement in four directions: up, down, left, and right.- Print the length of the shortest path.- Print the actual path by marking visited cells with '*'.- Print -1 if no valid path exists.Use std::queue and std::vector. Avoid global variables where possible.Create the file at:/home/nine/Projects/Coding/mahoraga-parallel-agent/test/maze.cpp";
    std::string prompt5 = "Write a C++ program that implements an LRU (Least Recently Used) Cache.Requirements:- Implement the cache using std::list and std::unordered_map.- Both get() and put() operations must have O(1) average time complexity.- When the cache reaches its capacity, inserting a new element must evict the least recently used element.- Accessing an existing element must update its position as the most recently used.- Support the following commands:PUT key value,GET key,DISPLAY- GET should print the value if the key exists, otherwise print -1.- DISPLAY should print all cache entries from most recently used to least recently used.- Read the cache capacity and number of commands from standard input.Implement the LRUCache class separately from the main function.Use modern C++ (C++17 or later) and do not use external libraries.Create the file at:/home/nine/Projects/Coding/mahoraga-parallel-agent/test/lru_cache.cpp";
    
    char* prompts[5] = {prompt1.data(), prompt2.data(), prompt3.data(), prompt4.data(), prompt5.data()};
    size_t promptSizes[5] = {prompt1.size(), prompt2.size(), prompt3.size(), prompt4.size(), prompt5.size()};

    sleep(1);

    auto start = std::chrono::steady_clock::now();
    agent.call(5, prompts, promptSizes);
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double>(end - start).count() << '\n';

    return 0;
}