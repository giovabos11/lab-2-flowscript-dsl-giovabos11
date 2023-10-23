#include "jobsysteminterface.h"

void JobSystemInterface::CreateJobSystem()
{
    // Create job system object
    js = JobSystem::CreateOrGet();
}

void JobSystemInterface::StopJobSystem()
{
    // Stop Job System
    js->Stop();
}

void JobSystemInterface::ResumeJobSystem()
{
    // Resume Job System
    js->Resume();
}

void JobSystemInterface::DestroyJobSystem()
{
    // Destroy Job System
    js->Destroy();
}

void JobSystemInterface::CreateThreads()
{
    // Create the maximum thread amount supported by the system
    for (int i = 0; i < std::thread::hardware_concurrency() - 1; i++)
    {
        js->CreateWorkerThread(("Thread" + std::to_string(i)).c_str(), 0xFFFFFFFF);
    }
}

int JobSystemInterface::CreateJob(std::string jobType, std::string input)
{
    return js->CreateJob(jobType, input);
}

void JobSystemInterface::DestroyJob(std::string name)
{
    // Destroy Job
    // ...
}

int JobSystemInterface::JobStatus(int id)
{
    // Return the job status
    return (int)js->GetJobStatus(id);
}

std::string JobSystemInterface::CompleteJob(int id)
{
    // Finish job
    return js->FinishJob(id);
}

bool JobSystemInterface::AreJobsRunning()
{
    // Return if jobs are running or completed
    return js->areJobsRunning();
}

void JobSystemInterface::RegisterJob(std::string name, Job *ptr)
{
    // Register job
    js->Register(name, ptr);
}

std::vector<std::string> JobSystemInterface::GetJobTypes()
{
    return js->GetJobTypes();
}