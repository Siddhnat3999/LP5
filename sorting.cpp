#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;

// ----------- BUBBLE SORTS -----------

void sequentialbubble(vector<int>& arr) { 
    int n = arr.size();
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);

    cout << "First 10 elements after sequential bubble sort:\n";
    for (int i = 0; i < min(10, n); ++i)
        cout << arr[i] << " ";
    cout << endl;
}

void parallelbubble(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; ++i) {
        int start = i % 2;
#pragma omp parallel for
        for (int j = start; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
    cout << "First 10 elements after parallel bubble sort:\n";
    for (int i = 0; i < min(10, n); ++i)
        cout << arr[i] << " ";
    cout << endl;
}

// ----------- MERGE SORTS -----------

void merge(vector<int>& arr, int l, int m, int r) {
    vector<int> temp;
    int left = l, right = m + 1;

    while (left <= m && right <= r) {
        if (arr[left] < arr[right])
            temp.push_back(arr[left++]);
        else
            temp.push_back(arr[right++]);
    }

    while (left <= m)
        temp.push_back(arr[left++]);

    while (right <= r)
        temp.push_back(arr[right++]);

    for (int i = 0; i < temp.size(); ++i)
        arr[l + i] = temp[i];
}

void sequentialmergesort(vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        sequentialmergesort(arr, l, m);
        sequentialmergesort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void parallelMergeSort(vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

#pragma omp parallel sections
        {
#pragma omp section
            parallelMergeSort(arr, l, m);
#pragma omp section
            parallelMergeSort(arr, m + 1, r);
        }

        merge(arr, l, m, r);
    }
}

// ----------- UTILITY -----------

vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    for (int i = 0; i < size; ++i)
        arr[i] = rand() % 1000;
    return arr;
}

// ----------- MAIN -----------

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed random once

    int size = 1000;
    vector<int> arr = generateRandomArray(size);

    // ---- BUBBLE SORT ----
    vector<int> arr_seq_bubble = arr;
    auto start = chrono::high_resolution_clock::now();
    sequentialbubble(arr_seq_bubble);
    auto end = chrono::high_resolution_clock::now();
    double time_seq_bubble = chrono::duration<double>(end - start).count();
    cout << "Time required for sequential Bubble Sort: " << time_seq_bubble << " seconds\n\n";

    vector<int> arr_par_bubble = arr;
    start = chrono::high_resolution_clock::now();
    parallelbubble(arr_par_bubble);
    end = chrono::high_resolution_clock::now();
    double time_par_bubble = chrono::duration<double>(end - start).count();
    cout << "Time required for parallel Bubble Sort: " << time_par_bubble << " seconds\n\n";

    // ---- MERGE SORT ----
    vector<int> arr_seq_merge = arr;
    start = chrono::high_resolution_clock::now();
    sequentialmergesort(arr_seq_merge, 0, arr_seq_merge.size() - 1);
    end = chrono::high_resolution_clock::now();
    double time_seq_merge = chrono::duration<double>(end - start).count();
    cout << "Time required for sequential Merge Sort: " << time_seq_merge << " seconds\n\n";

    vector<int> arr_par_merge = arr;
    start = chrono::high_resolution_clock::now();
    parallelMergeSort(arr_par_merge, 0, arr_par_merge.size() - 1);  // ✅ Fixed function name
    end = chrono::high_resolution_clock::now();
    double time_par_merge = chrono::duration<double>(end - start).count();
    cout << "Time required for parallel Merge Sort: " << time_par_merge << " seconds\n\n";

    return 0;
}
