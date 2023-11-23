# ECE454 A4
By: Taha Khokhar, ID: 1005813069

Group #30

Lab Session 02

Date: 2023-11-23

# Lab 4 Part 1:
## Instructions for Launch and Configuration:
Within a4_sync.cpp, change the values of the defined constants as desired.
Compile using `g++ a4_sync.cpp -o a4_sync -pthread`.

# Lab 4 Part 2:
## 4.2.1:
### Instructions for Launch and Configuration:
Compile using `g++ a4_part2_1.cpp -o a4_part2_1 -pthread`.

## 4.2.1 Results:
| Run #       | Coarse-Grained Time (µs) | Fine-Grained Time (µs) |
| ----------- | ------------------------ | ---------------------- |
| 1           | 176                      | 98                     |
| 2           | 109                      | 101                    |
| 3           | 96                       | 100                    |
| 4           | 102                      | 108                    |
| 5           | 95                       | 138                    |
| **Average** | 115                      | 109                    |

## 4.2.2: 
### Instructions for Launch and Configuration:
Compile using `g++ a4_part2_2.cpp -o a4_part2_2 -pthread`.

## 4.2.2 Results:
| Thread # | Avg Spin Time (µs) | Avg Total Time (µs) |
| -------- | ------------------ | ------------------- |
| 1        | 0                  | 5085                |
| 2        | 11161              | 16250               |
| 3        | 14167              | 19258               |
| 4        | 12108              | 17194               |
| 5        | 13073              | 18160               |