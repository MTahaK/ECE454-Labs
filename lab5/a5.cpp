#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>
#include <vector>

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

void map_files(std::vector<std::unordered_map<std::string, Occurrence*>*>& hash_tables, std::vector<std::string>& files) {
    // Number of files MUST match number of hash tables
    if (hash_tables.size() != files.size()) {
        std::cout << "Error: number of hash tables does not match number of files\n";
        return;
    }
    for (int i = 0; i < files.size(); i++) {
        process_file(files[i], *hash_tables[i]);
    }
}

void shuffle_hash_tables(std::vector<std::unordered_map<std::string, Occurrence*>*>& hash_tables) {
    // Select first hash table as the one to shuffle into
    auto hash_table1 = hash_tables[0];
    for (auto hash_table : hash_tables) {
        if (hash_table != hash_table1) {
            for (auto &pair : *hash_table){
                if (hash_table1->find(pair.first) == hash_table1->end()) {
                    (*hash_table1)[pair.first] = pair.second;
                } else {
                    Occurrence* occ = (*hash_table1)[pair.first];
                    while (occ->next != nullptr) {
                        occ = occ->next;
                    }
                    occ->next = pair.second;
                }
            }
        }
    }

}
int main() {
    // Initialize hash tables: <title, list of occurrences>
    std::unordered_map<std::string, Occurrence*> hash_table1, hash_table2, hash_table3, hash_table4;
    std::vector<std::unordered_map<std::string, Occurrence*>*> hash_tables = {&hash_table1, &hash_table2, &hash_table3, &hash_table4};
    std::vector<std::string> files = {"machine1.txt", "machine2.txt", "machine3.txt", "machine4.txt"};

    map_files(hash_tables, files);

    // std::cout<<"==============\nmachine1.txt\n==============\n\n";
    // for (auto& pair : hash_table1) {
    //     std::cout << "Title: " << pair.first << "\nOccurrences:\n";
    //     for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
    //         std::cout << occ->line << "\n";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout<<"==============\nmachine2.txt\n==============\n\n";
    // for (auto& pair : hash_table2) {
    //     std::cout << "Title: " << pair.first << "\nOccurrences:\n";
    //     for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
    //         std::cout << occ->line << "\n";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout<<"==============\nmachine3.txt\n==============\n\n";
    // for (auto& pair : hash_table3) {
    //     std::cout << "Title: " << pair.first << "\nOccurrences:\n";
    //     for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
    //         std::cout << occ->line << "\n";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout<<"==============\nmachine4.txt\n==============\n\n";
    // for (auto& pair : hash_table4) {
    //     std::cout << "Title: " << pair.first << "\nOccurrences:\n";
    //     for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
    //         std::cout << occ->line << "\n";
    //     }
    //     std::cout << "\n";
    // }

    // Amalgamate all hash tables into hash_table1

    shuffle_hash_tables(hash_tables);

    std::cout<<"==============\nAmalgamate of Hash Table 1, 2, 3, and 4\n==============\n\n";
    for (auto& pair : hash_table1) {
        std::cout << "Title: " << pair.first << "\nOccurrences:\n";
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            std::cout << occ->line << "\n";
        }
        std::cout << "\n";
    }


    hash_table2.clear();
    hash_table3.clear();
    hash_table4.clear();

    // Free hash table memory
    for (const auto& pair : hash_table1) {
        Occurrence* occ = pair.second;
        while (occ != nullptr) {
            Occurrence* next = occ->next;
            delete occ;
            occ = next;
        }
}

    return 0;
}
