#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Function to compute parallel minimum
int parallelMin(const vector<int>& vec)
{
    auto start = high_resolution_clock::now();
    int min_val = vec[0];
    #pragma omp parallel for reduction(min:min_val)
    for (int i = 1; i < vec.size(); i++)
    {
        if (vec[i] < min_val)
        {
            min_val = vec[i];
        }
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Parallel Min Time: " << duration << " ms" << endl;
    return min_val;
}

// Function to compute parallel maximum
int parallelMax(const vector<int>& vec)
{
    auto start = high_resolution_clock::now();
    int max_val = vec[0];
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 1; i < vec.size(); i++)
    {
        if (vec[i] > max_val)
        {
            max_val = vec[i];
        }
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Parallel Max Time: " << duration << " ms" << endl;
    return max_val;
}

// Function to compute parallel sum
int parallelSum(const vector<int>& vec)
{
    auto start = high_resolution_clock::now();
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < vec.size(); i++)
    {
        sum += vec[i];
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Parallel Sum Time: " << duration << " ms" << endl;
    return sum;
}

// Function to compute parallel average
float parallelAverage(const vector<int>& vec)
{
    auto start = high_resolution_clock::now();
    int sum = parallelSum(vec); // This already prints time for sum
    float avg = static_cast<float>(sum) / vec.size();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Parallel Average Time: " << duration << " ms" << endl;
    return avg;
}

int main()
{
    int n = 1000000000;//rease this for better parallelism
    cout << "Array size: " << n << endl;

    vector<int> vec(n);
    srand(time(0));
    for (int i = 0; i < n; ++i)
    {
        vec[i] = rand() % 1000000;
    }

    int min_val = parallelMin(vec);
    cout << "Minimum value: " << min_val << endl << endl;

    int max_val = parallelMax(vec);
    cout << "Maximum value: " << max_val << endl << endl;

    int sum = parallelSum(vec);
    cout << "Sum of values: " << sum << endl << endl;

    float avg = parallelAverage(vec);
    cout << fixed << setprecision(2);
    cout << "Average of values: " << avg << endl;

    return 0;
}
