#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <future>
#include <chrono>

int react(std::string polymer);
int part2Thread(std::string polymer, char c);

int main(int argc, char* argv[]) {
    std::string inputFilePath = "../input.txt";
    std::ifstream inputFile(inputFilePath);

    if(inputFile.fail()){
        std::cerr << "Error opening file \"" << inputFilePath << "\"n";
        return 0;
    }

    std::string line;
    std::string polymer;
    while(std::getline(inputFile, line)){
        polymer += line;
    }

    int startingUnits = polymer.size();
    int remainingUnits = react(polymer);

    std::cout << "Part 1:\n" << "\tStarting units : " << startingUnits << "\n\tRemaining units : " << remainingUnits << "\n";
    std::vector<int> polymerSizes = {remainingUnits};
    // Part 2
    std::vector<std::future<int>> threads;
    std::cout << "Number of threads : " << std::thread::hardware_concurrency() << "\n";
    for(char c = 'A'; c <= 'Z'; c++){
        while(threads.size() >= std::thread::hardware_concurrency()){
            for(int i = 0; i < threads.size(); i++){
                if(threads[i].wait_for(std::chrono::milliseconds(1)) == std::future_status::ready) {
                    polymerSizes.push_back(threads[i].get());
                    threads.erase(threads.begin() + i);
                }
            }
        }
        threads.push_back(std::async(std::launch::async, part2Thread, polymer, c));
    }
    
    int minimumSize = *std::min_element(polymerSizes.begin(), polymerSizes.end());
    std::cout << "Part 2:\n" << "\tMinimum units : " << minimumSize << "\n";

    return 0;
}

int part2Thread(std::string polymer, char c) {
        polymer.erase(std::remove(polymer.begin(), polymer.end(), c), polymer.end());
        polymer.erase(std::remove(polymer.begin(), polymer.end(), std::tolower(c)), polymer.end());
        return react(polymer);
}

int react(std::string polymer){
    bool destroyed;
    do{
        destroyed = false;
        for(int i = 0; i < polymer.size() - 1; i++){
            if(polymer[i] != polymer[i + 1] && std::tolower(polymer[i]) == std::tolower(polymer[i + 1])){
                polymer.erase(i, 2);
                destroyed = true;
                break;
            }
        }
    } while(destroyed);

    return polymer.size();
}