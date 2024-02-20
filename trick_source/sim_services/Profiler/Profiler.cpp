
#include "trick/Executive.hh"
#include "trick/Profiler.hh"
#include "trick/SimObject.hh"
#include "trick/JobData.hh"
#include <algorithm>

typedef struct {
    Trick::JobData* job;
    double total_time;
    double avg_time;
    unsigned long long call_time;
} JobProfile;

typedef struct {
    Trick::SimObject* obj; 
    double total_time;
    std::vector<JobProfile> jobProfiles;
} ObjProfile;

bool sortJobs(JobProfile j, JobProfile k) { return j.total_time < k.total_time; }

bool sortObjs(ObjProfile o, ObjProfile p) { return o.total_time < p.total_time; }

extern bool profileEnabled();


Trick::Profiler::Profiler() {

}

Trick::Profiler::~Profiler() {

}

void Trick::Profiler::shutdown() {
    if(profileEnabled()) {
        double total_job_time;
        std::vector<ObjProfile> objProfiles;
        for(Trick::SimObject* s : *sim_objects) {
            ObjProfile op;
            op.obj = s;
            op.total_time = 0;
            for(Trick::JobData* j : s->jobs) {
                JobProfile jp;
                jp.job = j;
                jp.call_time = j->call_times.size();
                jp.total_time = 0;
                for(double d : j->call_times) 
                    jp.total_time += d;

                jp.avg_time = jp.total_time/jp.call_time;
                op.jobProfiles.push_back(jp);
                op.total_time += jp.total_time;
            }
            total_job_time += op.total_time;
            std::sort(std::begin(op.jobProfiles), std::end(op.jobProfiles), sortJobs);
            objProfiles.push_back(op);
        }
        std::sort(std::begin(objProfiles), std::end(objProfiles), sortObjs);

        FILE* profileFile = fopen("JobProfile.csv", "w");
        fprintf(profileFile, "Name,Total Runtime, Runtime Percentage, Call Count, Average Time\n");
        for(ObjProfile o : objProfiles) {
            fprintf(profileFile, "\n%s, %.4f, %.4f\n", o.obj->name.data(), o.total_time, 100 * (o.total_time/total_job_time));
            for(JobProfile j : o.jobProfiles)  {
                if(j.call_time)
                    fprintf(profileFile, "%s, %.4f, %.4f, %llu, %.4f\n", j.job->name.data(), j.total_time, 100 * (j.total_time/total_job_time), j.call_time, j.avg_time);
                else
                    fprintf(profileFile, "%s, %.4f, %.4f, 0, 0.0000\n", j.job->name.data(), j.total_time, 100 * (j.total_time/total_job_time));
            }
        }
        fclose(profileFile);
    }
}