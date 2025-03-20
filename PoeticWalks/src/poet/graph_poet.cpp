#include "graph_poet.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>

#include <cstring>
#include <algorithm>
#include <cctype>
#include <regex>
#include <stdexcept>

#define WORD_MAX 1024

int appendStrToCharPointer(char *bufferPtr, std::string str, int bufferSize) {
    int bufferLength = strlen(bufferPtr);
    int stringLength = str.length();
    int finalLength = bufferLength + stringLength;
    
    if (finalLength + 1 > bufferSize) return -1;

    for (int i = 0;  i < stringLength; i++) {
        bufferPtr[bufferLength + i] = str[i];
    }
    bufferPtr[finalLength] = '\0';

    return finalLength;
}

int appendCharPointer(char *bufferPtr, char *charPtr, int bufferSize) {
    int bufferLength = strlen(bufferPtr);
    int charLength = strlen(charPtr);
    int finalLength = bufferLength + charLength;

    if (finalLength + 1 > bufferSize) return -1;

    for (int i = 0;  i < charLength; i++) {
        bufferPtr[bufferLength + i] = charPtr[i];
    }
    bufferPtr[finalLength] = '\0';

    return finalLength;
}

void processWord(Graph<std::string> *graph, std::string newWord, std::string lastWord) {
    if (newWord.length() == 0 || lastWord.length() == 0) return;

    std::unordered_map<std::string, int> lastWordEdges = graph->targets(lastWord);
    int newWeight;

    if (lastWordEdges.find(newWord) == lastWordEdges.end()) newWeight = 1;
    else newWeight = lastWordEdges[newWord] + 1;

    graph->set(lastWord, newWord, newWeight);
}

void processPoet(Graph<std::string> *graph, int fd) {
    char buffer;
    char lastWord[WORD_MAX]{0};
    char newWord[WORD_MAX]{0};
    int i = 0;

    while (read(fd, &buffer, sizeof(buffer)) > 0) {
        if (i == WORD_MAX) throw std::runtime_error("Word is too long.");

        if (buffer == ' ' || buffer == '\n') {
            newWord[i] = '\0';
            processWord(graph, std::string(newWord), std::string(lastWord));
            
            i = 0;
            strncpy(lastWord, newWord, WORD_MAX);

            continue;
        }

        newWord[i++] = tolower(buffer);
    }

    if (i == WORD_MAX) throw std::runtime_error("Word is too long.");

    if (i > 0) {
        newWord[i++]= '\0';
        processWord(graph, std::string(newWord), std::string(lastWord));
    }
}

GraphPoet::GraphPoet(Graph<std::string> *graphImplementation, std::string poetFilePath, bool absolutePath):
    graph{graphImplementation}, fd{} {
    
    char path[PATH_MAX] {};
    if (!absolutePath) getcwd(path, PATH_MAX);
    if (appendStrToCharPointer(path, poetFilePath, PATH_MAX) == -1) throw std::runtime_error("File path is too long.");

    fd = open(path, O_RDONLY);
    if (fd == -1) throw std::runtime_error("File not found/read failed.");
    processPoet(graph, fd);
}

std::string GraphPoet::poem(std::string input) {    
    std::regex word_find("\\S+");
    std::sregex_iterator begin = std::sregex_iterator(input.begin(), input.end(), word_find);
    std::sregex_iterator end = std::sregex_iterator();

    if (std::distance(begin, end) == 0) return "";

    std::string prevWord;
    std::unordered_map<std::string, int> prevWordTargets;

    std::string nextWord = ((std::smatch) *begin).str();
    std::unordered_map<std::string, int> nextWordSource;

    std::string nextWordLowerCase = nextWord;
    std::transform(nextWordLowerCase.begin(), nextWordLowerCase.end(), nextWordLowerCase.begin(), [](unsigned char c) {return tolower(c);});
    
    std::string output;

    for (std::sregex_iterator i = ++begin; i != end; i++) {
        output = output.append(nextWord);
        output = output.append(" ");
        
        prevWord = nextWordLowerCase;
        prevWordTargets = graph->targets(prevWord);

        nextWord = ((std::smatch) *i).str();
        nextWordLowerCase = nextWord;
        std::transform(nextWordLowerCase.begin(), nextWordLowerCase.end(), nextWordLowerCase.begin(), [](unsigned char c) {return tolower(c);});
        nextWordSource = graph->sources(nextWordLowerCase);

        std::string bridgeWord = "";
        int maxWeight = 0;
        for (auto&[key, val] : nextWordSource) {
            if (prevWordTargets.find(key) == prevWordTargets.end()) continue;
            int newWeight = val + prevWordTargets[key];
            if (newWeight > maxWeight) {
                bridgeWord = key;
                maxWeight = newWeight;
            }
        }
        if (bridgeWord == "") continue;
        output = output.append(bridgeWord);
        output = output.append(" ");
    }
    output.append(nextWord);
    return output;
}

GraphPoet& GraphPoet::operator=(GraphPoet&& that) {
    this->graph = that.graph;
    this->fd = that.fd;

    that.graph = nullptr;
    that.fd = 0;

    return *this;
}

GraphPoet::~GraphPoet() {
    delete graph;
    close(fd);
}