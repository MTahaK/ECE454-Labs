# ECE454 A5
By: Taha Khokhar, ID: 1005813069

Group #30

Lab Session 02

Date: 2023-11-30

# Lab 5:
## Instructions for Launch and Configuration:
Simply compile the program by running `g++ a5.cpp`.

## Part C:
### Execution time before optimization:
Computed on the "Elden Ring" title, which occurred 14 times.

| Run #       | Execution Time (s) | Execution Time (µs) |
| ----------- | ------------------ | ------------------- |
| 1           | 0.000226969        | 226.969             |
| 2           | 0.000216689        | 216.689             |
| 3           | 0.000237319        | 237.319             |
| 4           | 0.000214019        | 214.019             |
| 5           | 0.00021626         | 216.26              |
| **Average** | 0.000222251        | 222.251             |

## Part D:
### Optimizations Applied:
- Function Inlining (inlined mapping wrapper)

| Run #       | Execution Time (s) | Execution Time (µs) |
| ----------- | ------------------ | ------------------- |
| 1           | 0.000193849        | 193.849             |
| 2           | 0.000204539        | 204.539             |
| 3           | 0.000198659        | 198.659             |
| 4           | 0.00022213         | 222.130             |
| 5           | 0.00020928         | 209.280             |
| **Average** | 0.000205691        | 205.691             |
