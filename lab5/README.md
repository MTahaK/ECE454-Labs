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

Inlining a small function that contained a basic loop reduced the execution time of the program by a small, yet consistent, amount. 
All other optimizations I attempted - constant propagation, LICM, and copy propagation - did not improve the execution time by an observable amount.
This is likely due to the overall structure of my code; most of my loops already operate solely on variant data structures.

## Part E:
Most prominently, I can parallelize the `map_files` function. Given that I've already organized each hash table and each file that each table is generated
through into vector elements, I can simply launch a thread for each file mapping operation.

| Run #       | Execution Time (s) | Execution Time (µs) |
| ----------- | ------------------ | ------------------- |
| 1           | 0.000320449        | 320.449             |
| 2           | 0.000986617        | 986.617             |
| 3           | 0.000358149        | 358.149             |
| 4           | 0.000335169        | 335.169             |
| 5           | 0.000534508        | 534.508             |
| **Average** | 0.0005069784       | 506.9784            |

Multithreading very noticeably increased the execution time of the program. The overhead of waiting for all threads to finish, on this particular architecture and
for such a small data load, outweighs the streamlined execution of just mapping the data sequentially.
For larger databases, GPU-parallelized computation would be extremely helpful. On a load this small, however, it may produce results similar to multi-threading.