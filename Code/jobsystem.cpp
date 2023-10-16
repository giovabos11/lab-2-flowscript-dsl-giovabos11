#include <iostream>
#include <cstring>
#include "jobsystem.h"
#include "jobworkerthread.h"

JobSystem *JobSystem::s_jobSystem = nullptr;

typedef void (*JobCallback)(Job *completedJob);

JobSystem::JobSystem()
{
    m_jobHistory.reserve(256 * 1024);

    // Reset job history file
    std::ofstream o("../Data/job_history.txt");
    o << "";
    o.close();
}

JobSystem::~JobSystem()
{
    m_workerThreadsMutex.lock();
    int numWorkerThreads = (int)m_workerThreads.size();

    // First, tell each worker thread to stop picking up jobs
    for (int i = 0; i < numWorkerThreads; i++)
    {
        m_workerThreads[i]->ShutDown();
    }

    while (!m_workerThreads.empty())
    {
        delete m_workerThreads.back();
        m_workerThreads.pop_back();
    }
    m_workerThreadsMutex.unlock();
}

JobSystem *JobSystem::CreateOrGet()
{
    if (!s_jobSystem)
    {
        s_jobSystem = new JobSystem();
    }
    return s_jobSystem;
}

void JobSystem::Destroy()
{
    if (s_jobSystem)
    {
        delete s_jobSystem;
        s_jobSystem = nullptr;
    }
}

void JobSystem::CreateWorkerThread(const char *uniqueName, unsigned long workerJobChannels)
{
    JobWorkerThread *newWorker = new JobWorkerThread(uniqueName, workerJobChannels, this);
    m_workerThreadsMutex.lock();
    m_workerThreads.push_back(newWorker);
    m_workerThreads.back()->StartUp();
    m_workerThreadsMutex.unlock();
}

void JobSystem::DestroyWorkerThread(const char *uniqueName)
{
    m_workerThreadsMutex.lock();
    JobWorkerThread *doomedWorker = nullptr;
    std::vector<JobWorkerThread *>::iterator it = m_workerThreads.begin();

    for (; it != m_workerThreads.end(); ++it)
    {
        if (std::strcmp((*it)->m_uniqueName, uniqueName) == 0)
        {
            doomedWorker = *it;
            m_workerThreads.erase(it);
            break;
        }
    }
    m_workerThreadsMutex.unlock();

    if (doomedWorker)
    {
        doomedWorker->ShutDown();
        delete doomedWorker;
    }
}

void JobSystem::QueueJob(Job *job)
{
    m_jobsQueuedMutex.lock();

    m_jobHistoryMutex.lock();
    m_jobHistory.emplace_back(JobHistoryEntry(job->m_jobType, JOB_STATUS_QUEUED));
    // Write to job history file
    std::ofstream o("../Data/job_history.txt", std::ios_base::app);
    o << "Job ID " << job->GetUniqueID() << " was queued" << std::endl;
    o.close();
    m_jobHistoryMutex.unlock();

    m_jobsQueued.push_back(job);
    m_jobsQueuedMutex.unlock();
}

JobStatus JobSystem::GetJobStatus(int jobID) const
{
    m_jobHistoryMutex.lock();

    JobStatus jobStatus = JOB_STATUS_NEVER_SEEN;
    if (jobID < m_jobHistory.size())
    {
        jobStatus = (JobStatus)m_jobHistory[jobID].m_jobStatus;
    }

    m_jobHistoryMutex.unlock();

    return jobStatus;
}

bool JobSystem::IsJobComplete(int jobID) const
{
    return (JobStatus(jobID)) == (JOB_STATUS_COMPLETED);
}

void JobSystem::FinishCompletedJobs()
{
    std::deque<Job *> jobsCompleted;

    m_jobsCompletedMutex.lock();
    jobsCompleted.swap(m_jobsCompleted);
    m_jobsCompletedMutex.unlock();

    for (Job *job : jobsCompleted)
    {
        job->JobCompleteCallback();
        m_jobHistoryMutex.lock();
        // Write to job history file
        std::ofstream o("../Data/job_history.txt", std::ios_base::app);
        o << "Job ID " << job->GetUniqueID() << " was retired" << std::endl;
        o.close();
        m_jobHistory[job->m_jobID].m_jobStatus = JOB_STATUS_RETIRED;
        m_jobHistoryMutex.unlock();
        delete job;
    }
}

void JobSystem::FinishJob(int jobID)
{
    while (!IsJobComplete(jobID))
    {
        JobStatus jobStatus = GetJobStatus(jobID);
        if (jobStatus == JOB_STATUS_NEVER_SEEN || jobStatus == JOB_STATUS_RETIRED)
        {
            std::cout << "ERROR: Waiting for Job (#" << jobID << ") - no such job in JobSystem." << std::endl;
            return;
        }

        m_jobsCompletedMutex.lock();
        Job *thisCompletedJob = nullptr;
        for (auto jcIter = m_jobsCompleted.begin(); jcIter != m_jobsCompleted.end(); ++jcIter)
        {
            Job *someCompletedJob = *jcIter;
            if (someCompletedJob->m_jobID == jobID)
            {
                thisCompletedJob = someCompletedJob;
                m_jobsCompleted.erase(jcIter);
                break;
            }
        }
        m_jobsCompletedMutex.unlock();

        if (thisCompletedJob == nullptr)
        {
            std::cout << "ERROR: Job #" << jobID << " was status complete but not found in completed list." << std::endl;
        }

        thisCompletedJob->JobCompleteCallback();

        m_jobHistoryMutex.lock();
        m_jobHistory[thisCompletedJob->m_jobID].m_jobStatus = JOB_STATUS_RETIRED;
        m_jobHistoryMutex.unlock();

        delete thisCompletedJob;
    }
}

void JobSystem::OnJobCompleted(Job *jobJustExecuted)
{
    totalJobs++;
    m_jobsCompletedMutex.lock();
    m_jobsRunningMutex.lock();

    std::deque<Job *>::iterator runningJobItr = m_jobsRunning.begin();
    for (; runningJobItr != m_jobsRunning.end(); ++runningJobItr)
    {
        if (jobJustExecuted == *runningJobItr)
        {
            m_jobHistoryMutex.lock();
            m_jobsRunning.erase(runningJobItr);
            m_jobsCompleted.push_back(jobJustExecuted);
            m_jobHistory[jobJustExecuted->m_jobID].m_jobStatus = JOB_STATUS_COMPLETED;
            // Write to job history file
            std::ofstream o("../Data/job_history.txt", std::ios_base::app);
            o << "Job ID " << jobJustExecuted->GetUniqueID() << " was completed" << std::endl;
            o.close();
            m_jobHistoryMutex.unlock();
            break;
        }
    }
    m_jobsRunningMutex.unlock();
    m_jobsCompletedMutex.unlock();
}

Job *JobSystem::ClaimAJob(unsigned long channels)
{
    m_jobsQueuedMutex.lock();
    m_jobsRunningMutex.lock();

    Job *claimedJob = nullptr;
    std::deque<Job *>::iterator queuedJobIter = m_jobsQueued.begin();
    for (; queuedJobIter != m_jobsQueued.end(); ++queuedJobIter)
    {
        Job *queuedJob = *queuedJobIter;

        if ((queuedJob->m_jobChannels & channels) != 0)
        {
            claimedJob = queuedJob;

            m_jobHistoryMutex.lock();
            // Write to job history file
            std::ofstream o("../Data/job_history.txt", std::ios_base::app);
            o << "Job ID " << claimedJob->GetUniqueID() << " was claimed" << std::endl;
            o.close();
            m_jobsQueued.erase(queuedJobIter);
            m_jobsRunning.push_back(claimedJob);
            m_jobHistory[claimedJob->m_jobID].m_jobStatus = JOB_STATUS_RUNNING;
            m_jobHistoryMutex.unlock();
            break;
        }
    }

    m_jobsRunningMutex.unlock();
    m_jobsQueuedMutex.unlock();

    return claimedJob;
}