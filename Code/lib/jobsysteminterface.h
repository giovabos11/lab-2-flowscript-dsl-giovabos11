#pragma once
#include <iostream>
#include <string>

#include "jobsystem.h"
#include "job.h"

#include "json.hpp"
using json = nlohmann::json;

class JobSystemInterface
{

public:
    void CreateJobSystem();
    void StopJobSystem();
    void ResumeJobSystem();
    void DestroyJobSystem();

    void CreateThreads();

    int CreateJob(std::string jobType, std::string input); // Spinning off the job. Takes in JSON
    void DestroyJob(std::string name);
    int JobStatus(int id);
    std::string CompleteJob(int id); // Returns JSON
    std::vector<std::string> GetJobTypes();
    bool AreJobsRunning();

    void RegisterJob(std::string name, Job *ptr); // Create new job type
private:
    JobSystem *js;
};