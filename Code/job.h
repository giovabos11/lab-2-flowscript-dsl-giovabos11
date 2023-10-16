#ifndef JOB_SYSTEM_JOB_H
#define JOB_SYSTEM_JOB_H

#include <mutex>
#include <map>
#include <deque>
#include <vector>
#include <thread>

class Job
{
    friend class JobSystem;
    friend class JobWorkerThread;

public:
    Job(unsigned long jobChannels = 0xFFFFFFFF, int jobType = -1) : m_jobChannels(jobChannels), m_jobType(jobType)
    {
        static int s_nextJobID = 0;
        m_jobID = s_nextJobID++;
    }

    virtual ~Job() {}

    virtual void Execute() = 0; // Must be inherited by children because it doesn't have a body
    virtual void JobCompleteCallback(){};
    int GetUniqueID() const { return m_jobID; }

private:
    int m_jobID = -1;
    int m_jobType = -1;
    unsigned long m_jobChannels = 0xFFFFFFFF;
};

#endif