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

void process_file(const std::string& filename, std::unordered_map<std::string, Occurrence*>& hash_table) {
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
                if (hash_table.find(title) == hash_table.end()) {
                    hash_table[title] = new Occurrence(title, line);
                } else {
                    Occurrence* newOccurrence = new Occurrence(title, line);
                    newOccurrence->next = hash_table[title];
                    hash_table[title] = newOccurrence;
                }
            } else if (isTitle) {
                title += " " + word;
            }
        }
    }

    file.close();
}


int main() {
    std::unordered_map<std::string, Occurrence*> hash_table1, hash_table2, hash_table3, hash_table4;
    
    process_file("machine1.txt", hash_table1);
    process_file("machine2.txt", hash_table2);
    process_file("machine3.txt", hash_table3);
    process_file("machine4.txt", hash_table4);

    std::cout<<"==============\nmachine1.txt\n==============\n\n";
    for (const auto& pair : hash_table1) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }
    std::cout<<"==============\nmachine2.txt\n==============\n\n";
    for (const auto& pair : hash_table2) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }
    std::cout<<"==============\nmachine3.txt\n==============\n\n";
    for (const auto& pair : hash_table3) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }
    std::cout<<"==============\nmachine4.txt\n==============\n\n";
    for (const auto& pair : hash_table4) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }

    // Free hash table memory
    for (const auto& pair : hash_table1) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
    }
    for (const auto& pair : hash_table2) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
    }
    for (const auto& pair : hash_table3) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
    }
    for (const auto& pair : hash_table4) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
    }

    return 0;
}
