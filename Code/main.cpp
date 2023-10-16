#include <iostream>
#include <string>

#include "jobsystem.h"
#include "compilejob.h"

using namespace std;

int main()
{
    // Create job system object
    JobSystem *js = JobSystem::CreateOrGet();

    // Create a vector with different commands
    string make = "";
#ifdef __linux__
    make = "make";
#elif _WIN32
    make = "cd C:\\Users\\giova_pwwkjqa\\OneDrive\\Escritorio\\SMU\\lab-2-flowscript-dsl-giovabos11\\Code & MinGW32-make";
#else
    make = "make";
#endif
    vector<string> commands = {make + " project1",
                               make + " project2",
                               make + " project3",
                               make + " project4"};

    // Create the maximum thread amount supported by the system
    cout << "Creating Worker Threads..." << endl;
    for (int i = 0; i < thread::hardware_concurrency() - 1; i++)
    {
        js->CreateWorkerThread(("Thread" + std::to_string(i)).c_str(), 0xFFFFFFFF);
    }

    // Create a different job for each make command (makefile needed)
    cout << "Creating Jobs..." << endl;
    vector<Job *> jobs;
    for (int i = 0; i < commands.size(); i++)
    {
        CompileJob *cjb = new CompileJob(0xFFFFFFFF, i);
        cjb->command = commands[i];
        jobs.push_back(cjb);
    }

    // Queue the jobs
    cout << "Queuing Jobs..." << endl;
    vector<Job *>::iterator it = jobs.begin();
    for (; it != jobs.end(); it++)
    {
        js->QueueJob(*it);
    }

    // Check all job statuses again
    std::cout << "Job Status:" << std::endl;
    for (int i = 0; i < jobs.size(); i++)
    {
        std::cout << "Job " << i << " Status: " << (int)js->GetJobStatus(i) << std::endl;
    }

    // Finish all jobs
    while (js->areJobsRunning() || js->areJobsCompleted())
    {
        js->FinishCompletedJobs();
    }

    // Check all job statuses again
    std::cout << "Job Status:" << std::endl;
    for (int i = 0; i < jobs.size(); i++)
    {
        std::cout << "Job " << i << " Status: " << (int)js->GetJobStatus(i) << std::endl;
    }

    // Destroy Job System
    js->Destroy();

    return 0;
}