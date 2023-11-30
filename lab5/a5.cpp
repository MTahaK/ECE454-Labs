#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>

struct Occurrence {
    std::string title;
    std::string line;
    Occurrence* next;

    Occurrence(const std::string& title, const std::string& line) : title(title), line(line), next(nullptr) {}
};

void processFile(const std::string& filename, std::unordered_map<std::string, Occurrence*>& hashTable) {
    std::ifstream file(filename);
    std::string line;

    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        bool isTitle = false;
        std::string title;

        while (iss >> word) {
            if (word.front() == '<') {
                isTitle = true;
                title = word;
            } else if (isTitle && word.back() == '>' || word[word.length() - 2] == '>'){
                title += " " + word;
                isTitle = false;

                // If title had trailing punctuation after the closing angle bracket,
                // remove it.
                if(word.back() != '>') {
                    title = title.substr(0, title.size() - 1);
                }
                // Remove angle brackets
                title = title.substr(1, title.size() - 2);
                

                // Insert into hash table
                if (hashTable.find(title) == hashTable.end()) {
                    hashTable[title] = new Occurrence(title, line);
                } else {
                    Occurrence* newOccurrence = new Occurrence(title, line);
                    newOccurrence->next = hashTable[title];
                    hashTable[title] = newOccurrence;
                }
            } else if (isTitle) {
                title += " " + word;
            }
        }
    }

    file.close();
}


int main() {
    std::unordered_map<std::string, Occurrence*> hashTable;
    

    processFile("machine4.txt", hashTable);

    for (const auto& pair : hashTable) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }

    // Remember to free allocated memory
    for (const auto& pair : hashTable) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
    }

    return 0;
}
