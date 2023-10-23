#include <fstream>

#include "./lib/jobsysteminterface.h"

using namespace std;

// Example Job 1
string sortString(string a)
{
    int i, key, j, n = a.size();
    for (i = 1; i < n; i++)
    {
        key = a[i];
        j = i - 1;
        while (j >= 0 && a[j] > key)
        {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = key;
    }
    return a;
}

// Example Job 2
string crazyCase(string a)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (i % 2 == 0)
        {
            a[i] = toupper(a[i]);
        }
        else
        {
            a[i] = tolower(a[i]);
        }
    }
    return a;
}

int main()
{
    // Create job system object
    JobSystemInterface js;

    js.CreateJobSystem();
    js.CreateThreads();

    // Register all jobs
    js.RegisterJob("sort_string", new Job(sortString, 1));
    js.RegisterJob("crazy_case", new Job(crazyCase, 2));

    // Spin off jobs
    int job1ID = js.CreateJob("sort_string", "jdrbfkjdb");
    int job2ID = js.CreateJob("sort_string", "abcd");
    int job3ID = js.CreateJob("sort_string", "twuigui");
    int job4ID = js.CreateJob("sort_string", "giovanni");

    // Check job status and try to complete the jobs
    string output1, output2, output3, output4;
    while (js.AreJobsRunning())
    {
        //  Wait to complete all the jobs
    }
    output1 = js.CompleteJob(job1ID);
    output2 = js.CompleteJob(job2ID);
    output3 = js.CompleteJob(job3ID);
    output4 = js.CompleteJob(job4ID);

    // Spin off different jobs
    int job5ID = js.CreateJob("crazy_case", output1);
    int job6ID = js.CreateJob("crazy_case", output3);

    while (js.AreJobsRunning())
    {
        //  Wait to complete all the jobs
    }
    output1 = js.CompleteJob(job5ID);
    output3 = js.CompleteJob(job6ID);

    // Print the outputs
    cout << "Output: " << output1 << endl;
    cout << "Output: " << output2 << endl;
    cout << "Output: " << output3 << endl;
    cout << "Output: " << output4 << endl;

    // Destroy Job System
    js.DestroyJobSystem();

    return 0;
}