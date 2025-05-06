#include<iostream>
#include<omp.h>
#include<chrono>
#include<vector>
#include<limits>

using namespace std;
using namespace std::chrono;

// Sequential Bubble Sort
void sequentialBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n - 1; i++)
        for(int j = 0; j < n - i - 1; j++)
            if(arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// Parallel Bubble Sort (Odd-Even Transposition Sort)
void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        int start = i % 2;
        #pragma omp parallel for
        for (int j = start; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

// Merge Helper
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while(i <= mid && j <= right)
        temp[k++] = (arr[i] < arr[j]) ? arr[i++] : arr[j++];

    while(i <= mid)
        temp[k++] = arr[i++];
    while(j <= right)
        temp[k++] = arr[j++];

    for(int i = 0; i < k; i++)
        arr[left + i] = temp[i];
}

// Sequential Merge Sort
void sequentialMergeSort(vector<int>& arr, int left, int right) {
    if(left < right) {
        int mid = (left + right)/2;
        sequentialMergeSort(arr, left, mid);
        sequentialMergeSort(arr, mid+1, right);
        merge(arr, left, mid, right);
    }
}

// Parallel Merge Sort
void parallelMergeSort(vector<int>& arr, int left, int right, int depth = 0) {
    if(left < right) {
        int mid = (left + right)/2;
        if(depth <= 3) {
            #pragma omp parallel sections
            {
                #pragma omp section
                parallelMergeSort(arr, left, mid, depth + 1);

                #pragma omp section
                parallelMergeSort(arr, mid + 1, right, depth + 1);
            }
        } else {
            sequentialMergeSort(arr, left, mid);
            sequentialMergeSort(arr, mid + 1, right);
        }
        merge(arr, left, mid, right);
    }
}

int main() {
    vector<int> data;
    for(int i = 0; i < 10000; i++)
        data.push_back(rand() % 10000);

    vector<int> b1 = data, b2 = data;
    vector<int> m1 = data, m2 = data;

    // Bubble Sort Timings
    auto start_seq_bubble = high_resolution_clock::now();
    sequentialBubbleSort(b1);
    auto stop_seq_bubble = high_resolution_clock::now();
    auto duration_seq_bubble = duration_cast<milliseconds>(stop_seq_bubble - start_seq_bubble);

    auto start_par_bubble = high_resolution_clock::now();
    parallelBubbleSort(b2);
    auto stop_par_bubble = high_resolution_clock::now();
    auto duration_par_bubble = duration_cast<milliseconds>(stop_par_bubble - start_par_bubble);

    // Merge Sort Timings
    auto start_seq_merge = high_resolution_clock::now();
    sequentialMergeSort(m1, 0, m1.size() - 1);
    auto stop_seq_merge = high_resolution_clock::now();
    auto duration_seq_merge = duration_cast<milliseconds>(stop_seq_merge - start_seq_merge);

    auto start_par_merge = high_resolution_clock::now();
    parallelMergeSort(m2, 0, m2.size() - 1);
    auto stop_par_merge = high_resolution_clock::now();
    auto duration_par_merge = duration_cast<milliseconds>(stop_par_merge - start_par_merge);

    // Results
    cout << "\nBubble Sort Performance:\n";
    cout << "\nSequential Time: " << duration_seq_bubble.count() << " ms";
    cout << "\nParallel Time  : " << duration_par_bubble.count() << " ms";
    cout << "\nSpeedup Factor : " << static_cast<double>(duration_seq_bubble.count()) / duration_par_bubble.count() << "\n";

    cout << "\nMerge Sort Performance:\n";
    cout << "\nSequential Time: " << duration_seq_merge.count() << " ms";
    cout << "\nParallel Time  : " << duration_par_merge.count() << " ms";
    cout << "\nSpeedup Factor : " << static_cast<double>(duration_seq_merge.count()) / duration_par_merge.count() << "\n";

    return 0;
}
