#include<iostream>
#include<omp.h>
#include<chrono>
#include<limits>
#include<vector>
#include<numeric>

using namespace std;
using namespace std::chrono;

int sequentialMax(const vector<int> &data)
{
    if(data.empty())
    {
        return numeric_limits<int>::min();
    }
    int max_val=data[0];
    for(int value:data)
    {
        if(value>max_val)
        {
            max_val=value;
        }
    }
    return max_val;
}
int sequentialMin(const vector<int> &data)
{
    if(data.empty())
    {
        return numeric_limits<int>::max();
    }
    int min_val=data[0];
    for(int value:data)
    {
        if(value<min_val)
        {
            min_val=value;
        }
    }
    return min_val;
}

int sequentialSum(const vector<int> &data)
{
    int sum=0;
    for(int value:data)
    {
        sum+=value;
    }
    return sum;
}

double sequentialAverage(const vector<int> &data)
{
    if(data.empty())
    {
        return 0.0;
    }
    return static_cast<double>(sequentialSum(data))/data.size();
}

int parallelMin(const vector<int> &data)
{
    if(data.empty())
    {
        return numeric_limits<int>::max();
    }
    int min_val=data[0];
    #pragma omp parallel for reduction(min:min_val);
    for(size_t i=1;i<data.size();++i)
    {
        if(data[i]<min_val)
        {
            min_val=data[i];
        }
    }
    return min_val;
}
int parallelMax(const vector<int> &data)
{
    if(data.empty())
    {
        return numeric_limits<int>::min();
    }
    int max_val=data[0];
    #pragma omp parallel for reduction(max:max_val);
    for(size_t i=1;i<data.size();++i)
    {
        if(data[i]>max_val)
        {
            max_val=data[i];
        }
    }
    return max_val;
}
int parallelSum(const vector<int> &data)
{
   int sum=0;
   #pragma omp parallel for reduction(+:sum)
   for(int value:data)
   {
    sum+=value;
   }
   return sum;
}

double parallelAverage(const vector<int> &data)
{
    if(data.empty())
    {
        return 0.0;
    }
   return static_cast<double>(parallelMax(data)) / data.size();
}

int main()
{
    vector <int> data={1,4,8,6,9,5,3,11,22,33,44,55,66};
    for(int i=0;i<100000;i++)
    {
        data.push_back(rand() %1000);
    }
    
    auto start_seq_min=high_resolution_clock::now();
    int seq_min=sequentialMin(data);
    auto stop_seq_min=high_resolution_clock::now();
    auto duration_seq_min=duration_cast<microseconds>(stop_seq_min-start_seq_min);

    auto start_seq_max=high_resolution_clock::now();
    int seq_max=sequentialMax(data);
    auto stop_seq_max=high_resolution_clock::now();
    auto duration_seq_max=duration_cast<microseconds>(stop_seq_max-start_seq_max);

    auto start_seq_sum=high_resolution_clock::now();
    int seq_sum=sequentialSum(data);
    auto stop_seq_sum=high_resolution_clock::now();
    auto duration_seq_sum=duration_cast<microseconds>(stop_seq_sum-start_seq_sum);

    auto start_seq_avg=high_resolution_clock::now();
    int seq_avg=sequentialAverage(data);
    auto stop_seq_avg=high_resolution_clock::now();
    auto duration_seq_avg=duration_cast<microseconds>(stop_seq_avg-start_seq_avg);

    auto start_par_min=high_resolution_clock::now();
    int par_min=parallelMin(data);
    auto stop_par_min=high_resolution_clock::now();
    auto duration_par_min=duration_cast<microseconds>(stop_par_min-start_par_min);

    auto start_par_sum=high_resolution_clock::now();
    int par_sum=parallelSum(data);
    auto stop_par_sum=high_resolution_clock::now();
    auto duration_par_sum=duration_cast<microseconds>(stop_par_sum-start_par_sum);

    auto start_par_max=high_resolution_clock::now();
    int par_max=parallelMax(data);
    auto stop_par_max=high_resolution_clock::now();
    auto duration_par_max=duration_cast<microseconds>(stop_par_max-start_par_max);

    auto start_par_avg=high_resolution_clock::now();
    int par_avg=parallelAverage(data);
    auto stop_par_avg=high_resolution_clock::now();
    auto duration_par_avg=duration_cast<microseconds>(stop_par_avg-start_par_avg);

    cout<<"\n Minimum:\n\n";
    cout<<"\n Sequantial Time "<<duration_seq_min.count()<<" microseconds"<<endl;
    cout<<"\n Parallel Time :"<<duration_par_min.count()<<" microseconds"<<endl;
    cout<<"\nSpeedup factor:"<<static_cast<double>(duration_seq_min.count()) / duration_par_min.count()<<endl;

    cout<<"\n Maximum:\n\n";
    cout<<"\n Sequantial Time "<<duration_seq_max.count()<<" microseconds"<<endl;
    cout<<"\n Parallel Time :"<<duration_par_max.count()<<" microseconds"<<endl;
    cout<<"\nSpeedup factor:"<<static_cast<double>(duration_seq_max.count()) / duration_par_max.count()<<endl;

    cout<<"\n Sum:\n\n";
    cout<<"\n Sequantial Time "<<duration_seq_sum.count()<<" microseconds"<<endl;
    cout<<"\n Parallel Time :"<<duration_par_sum.count()<<" microseconds"<<endl;
    cout<<"\nSpeedup factor:"<<static_cast<double>(duration_seq_sum.count()) / duration_par_sum.count()<<endl;

    cout<<"\n Average:\n\n";
    cout<<"\n Sequantial Time "<<duration_seq_avg.count()<<" microseconds"<<endl;
    cout<<"\n Parallel Time :"<<duration_par_avg.count()<<" microseconds"<<endl;
    cout<<"\nSpeedup factor:"<<static_cast<double>(duration_seq_avg.count()) / duration_par_avg.count()<<endl;


    return 0;

}