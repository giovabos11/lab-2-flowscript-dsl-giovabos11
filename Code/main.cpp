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

    // Get all job types available
    vector<string> jobTypes = js.GetJobTypes();
    cout << "Job types available: ";
    for (int i = 0; i < jobTypes.size(); i++)
    {
        cout << jobTypes[i] << " ";
    }
    cout << endl;

    // Spin off jobs
    int job1ID = js.CreateJob("sort_string", "jdrbfkjdb");
    int job2ID = js.CreateJob("sort_string", "abcd");
    int job3ID = js.CreateJob("sort_string", "twuigui");
    int job4ID = js.CreateJob("sort_string", "giovanni");

    // Get Job statuses
    cout << "Job ID " << job1ID << " status: " << js.JobStatus(job1ID) << endl;
    cout << "Job ID " << job2ID << " status: " << js.JobStatus(job2ID) << endl;
    cout << "Job ID " << job3ID << " status: " << js.JobStatus(job3ID) << endl;
    cout << "Job ID " << job4ID << " status: " << js.JobStatus(job4ID) << endl;

    // Check job status and try to complete the jobs
    string output1, output2, output3, output4;

    while (js.AreJobsRunning())
    {
        //  Wait to complete all the jobs
    }

    // Get job outputs
    output1 = js.CompleteJob(job1ID);
    output2 = js.CompleteJob(job2ID);
    output3 = js.CompleteJob(job3ID);
    output4 = js.CompleteJob(job4ID);

    // Get Job statuses
    cout << "Job ID " << job1ID << " status: " << js.JobStatus(job1ID) << endl;
    cout << "Job ID " << job2ID << " status: " << js.JobStatus(job2ID) << endl;
    cout << "Job ID " << job3ID << " status: " << js.JobStatus(job3ID) << endl;
    cout << "Job ID " << job4ID << " status: " << js.JobStatus(job4ID) << endl;

    // Stop Job System
    js.StopJobSystem();

    // Resume Job System
    js.ResumeJobSystem();

    // Spin off different jobs
    int job5ID = js.CreateJob("crazy_case", output1);
    int job6ID = js.CreateJob("crazy_case", output3);
    int job7ID = js.CreateJob("crazy_case", output4);

    while (js.AreJobsRunning())
    {
        //  Wait to complete all the jobs
    }
    // Destroy job 7 while running
    js.DestroyJob(job7ID);

    // Get job outputs
    output1 = js.CompleteJob(job5ID);
    output3 = js.CompleteJob(job6ID);

    // Get job statuses
    cout << "Job ID " << job5ID << " status: " << js.JobStatus(job5ID) << endl;
    cout << "Job ID " << job6ID << " status: " << js.JobStatus(job6ID) << endl;

    // Print the outputs
    cout << "Output: " << output1 << endl;
    cout << "Output: " << output2 << endl;
    cout << "Output: " << output3 << endl;
    cout << "Output: " << output4 << endl;

    // Destroy Job System
    js.DestroyJobSystem();

    return 0;
}