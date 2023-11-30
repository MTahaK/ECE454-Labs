// ECE454 A5
// By: Taha Khokhar, ID: 1005813069
// Group #30
// Lab Session 02
// Date: 2023-11-30

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>
#include <vector>
#include <chrono>
#include <thread>
struct Occurrence {
    std::string title;
    std::string line;
    int count; // Set to -1 if not used
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

        // Parse line as string stream
        while (iss >> word) {
            // If word starts with '<', it is a title; start adding words to title until
            // end demarcated by '>'
            if (word.front() == '<') {
                isTitle = true;
                title = word;
            } else if (isTitle && word.back() == '>' || word[word.length() - 2] == '>'){
                title += " " + word;
                isTitle = false;

                // If title had trailing punctuation after the closing angle bracket,
                // remove it. Example: <Elden Ring>, -> <Elden Ring>
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
                    // Initialize count to -1. 
                    newOccurrence->count = -1;
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
    std::vector<std::thread> threads;

    for (int i = 0; i < files.size(); ++i) {
        threads.push_back(std::thread(process_file, files[i], std::ref(*hash_tables[i])));
    }

    for (auto& th : threads) {
        th.join();
    }
}

void shuffle_hash_tables(std::vector<std::unordered_map<std::string, Occurrence*>*>& hash_tables) {
    // Select first hash table as the one to shuffle into
    auto hash_table1 = hash_tables[0];
    for (auto hash_table : hash_tables) {
        if (hash_table != hash_table1) {
            for (auto &pair : *hash_table){
                // If title is not in hash_table1, add it
                if (hash_table1->find(pair.first) == hash_table1->end()) {
                    (*hash_table1)[pair.first] = pair.second;
                }
                // Otherwise, append the list of occurrences to the end of the list in hash_table1 
                else {
                    Occurrence* occ = (*hash_table1)[pair.first];
                    while (occ->next != nullptr) {
                        occ = occ->next;
                    }
                    occ->next = pair.second;
                }
            }
            hash_table->clear();
        }
    }

}
int main() {
    // Initialize hash tables: <title, list of occurrences>
    std::unordered_map<std::string, Occurrence*> hash_table1, hash_table2, hash_table3, hash_table4;
    std::vector<std::unordered_map<std::string, Occurrence*>*> hash_tables = {&hash_table1, &hash_table2, &hash_table3, &hash_table4};
    std::vector<std::string> files = {"machine1.txt", "machine2.txt", "machine3.txt", "machine4.txt"};

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();
    // Map files to hash tables
    map_files(hash_tables, files);

    // Amalgamate all hash tables into hash_table1
    shuffle_hash_tables(hash_tables);

    // Go through hash table; count occurrences, add new node to start of list that tracks count.
    for (auto& pair : hash_table1) {
        int count = 0;
        for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
            count++;
        }
        // Add new node to start of list that tracks count. Faster access to count.
        Occurrence* newOccurrence = new Occurrence(pair.first, "");
        newOccurrence->count = count;
        newOccurrence->next = pair.second;
        pair.second = newOccurrence;
    }

    // Get the count of one of the titles
    std::string title = "Elden Ring";
    // Access hash table, get count from first node in pair.second list
    int count = hash_table1[title]->count; 
    std::cout << "Number of occurrences of " << title << ": " << count << std::endl;
    // End timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // std::cout<<"==============\nAmalgamate of Hash Table 1, 2, 3, and 4\n==============\n\n";
    // for (auto& pair : hash_table1) {
    //     std::cout << "\n============================\nTitle: " << pair.first <<std::endl;
    //     if(pair.second->count != -1) {
    //         std::cout << "Number of Occurrences: " << pair.second->count << std::endl;
    //     }
    //     std::cout << "Occurrences:\n";
    //     for (Occurrence* occ = pair.second; occ != nullptr; occ = occ->next) {
    //         std::cout << occ->line << "\n";
    //     }
    //     std::cout << "\n============================\n\n";
    // }

    std::cout << "Time taken: " << elapsed.count() << " seconds\n";
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
