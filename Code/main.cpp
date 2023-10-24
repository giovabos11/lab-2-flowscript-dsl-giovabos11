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
    json jobTypes = json::parse(js.GetJobTypes());
    cout << "Job types available: " << endl;
    for (int i = 0; i < jobTypes.size(); i++)
    {
        cout << jobTypes[i] << endl;
    }

    // Spin off jobs
    string job1 = js.CreateJob("{\"job_type\": \"sort_string\", \"input\": \"jdrbfkjdb\"}");
    string job2 = js.CreateJob("{\"job_type\": \"sort_string\", \"input\": \"abcd\"}");
    string job3 = js.CreateJob("{\"job_type\": \"sort_string\", \"input\": \"twuigui\"}");
    string job4 = js.CreateJob("{\"job_type\": \"sort_string\", \"input\": \"giovanni\"}");

    // Get job IDs
    int job1ID = json::parse(job1)["id"];
    int job2ID = json::parse(job2)["id"];
    int job3ID = json::parse(job3)["id"];
    int job4ID = json::parse(job4)["id"];

    // Get Job statuses
    cout << "Job ID " << job1ID << " status: " << json::parse(js.JobStatus(job1))["status"] << endl;
    cout << "Job ID " << job2ID << " status: " << json::parse(js.JobStatus(job2))["status"] << endl;
    cout << "Job ID " << job3ID << " status: " << json::parse(js.JobStatus(job3))["status"] << endl;
    cout << "Job ID " << job4ID << " status: " << json::parse(js.JobStatus(job4))["status"] << endl;

    // Check job status and try to complete the jobs
    string output1, output2, output3, output4;

    while (json::parse(js.AreJobsRunning())["are_jobs_running"])
    {
        //  Wait to complete all the jobs
    }

    // Get job outputs
    output1 = json::parse(js.CompleteJob(job1))["output"];
    output2 = json::parse(js.CompleteJob(job2))["output"];
    output3 = json::parse(js.CompleteJob(job3))["output"];
    output4 = json::parse(js.CompleteJob(job4))["output"];

    // Get Job statuses
    cout << "Job ID " << job1ID << " status: " << json::parse(js.JobStatus(job1))["status"] << endl;
    cout << "Job ID " << job2ID << " status: " << json::parse(js.JobStatus(job2))["status"] << endl;
    cout << "Job ID " << job3ID << " status: " << json::parse(js.JobStatus(job3))["status"] << endl;
    cout << "Job ID " << job4ID << " status: " << json::parse(js.JobStatus(job4))["status"] << endl;

    // Stop Job System
    js.StopJobSystem();

    // Resume Job System
    js.ResumeJobSystem();

    // Spin off different jobs
    string job5 = js.CreateJob("{\"job_type\": \"crazy_case\", \"input\": \"" + output1 + "\"}");
    string job6 = js.CreateJob("{\"job_type\": \"crazy_case\", \"input\": \"" + output3 + "\"}");
    string job7 = js.CreateJob("{\"job_type\": \"crazy_case\", \"input\": \"" + output4 + "\"}");

    // Get job IDs
    int job5ID = json::parse(job5)["id"];
    int job6ID = json::parse(job6)["id"];
    int job7ID = json::parse(job7)["id"];

    while (json::parse(js.AreJobsRunning())["are_jobs_running"])
    {
        //  Wait to complete all the jobs
    }
    // Destroy job 7 while running
    js.DestroyJob(job7);

    // Get job outputs
    output1 = json::parse(js.CompleteJob(job5))["output"];
    output3 = json::parse(js.CompleteJob(job6))["output"];

    // Get job statuses
    cout << "Job ID " << job5ID << " status: " << json::parse(js.JobStatus(job5))["status"] << endl;
    cout << "Job ID " << job6ID << " status: " << json::parse(js.JobStatus(job6))["status"] << endl;

    // Print the outputs
    cout << "Output: " << output1 << endl;
    cout << "Output: " << output2 << endl;
    cout << "Output: " << output3 << endl;
    cout << "Output: " << output4 << endl;

    // Destroy Job System
    js.DestroyJobSystem();

    return 0;
}