#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
using namespace std;
enum JobType
{
    PERIODIC,
    APERIODIC,
    SPORADIC,
    IDLE
};
class Job
{
public:
    int ID;
    JobType type;
    int P, C, A;
    int complete_time;
    bool Isaccept;
};
struct currentJob
{
    int ID;
    int JobType;
};
bool operator<(Job const &a, Job const &b)
{
    if (a.type == SPORADIC && b.type == SPORADIC) // Sporadic job sort by arrival time
        return a.A < b.A;
    if (a.type == PERIODIC && b.type == PERIODIC) // Periodic job sort by deadline
        return a.P < b.P;
    else
        return a.A < b.A; // Aparadic job sort by arrival time
}
bool ArrTimeInceasing(Job const &a, Job const &b)
{
    return a.A < b.A;
}

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}
int lca(int a, int b)
{
    return a * b / gcd(a, b);
}
int main(int argc, char *argv[])
{
    // Open the file
    FILE *fp = fopen(argv[1], "rb");

    // Check if the file was opened successfully
    if (!fp)
    {
        std::cerr << "Error: unable to open file"
                  << std::endl;
        return 1;
    }

    // Read the file into a buffer
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    // Parse the JSON document
    rapidjson::Document doc;
    doc.ParseStream(is);

    // Check for parse errors
    if (doc.HasParseError())
    {
        std::cerr << "Error parsing JSON: " << doc.GetParseError() << std::endl;
        return 1;
    }

    // Record the task array
    vector<Job> pArr; // periodic task
    vector<Job> aArr; // aperiodic task
    vector<Job> sArr; // sporadic task
    vector<Job> allJobArr;
    // used frame table
    vector<bool> used_time;
    // Iterate over the array of objects
    rapidjson::Value::ConstValueIterator itr;
    int indexOfTaskGroup = 0;
    int idCount = 0;
    cout << doc.Size() << endl; // MUST : #Schedules: number of Schedules, same as length of input vector
    for (itr = doc.Begin(); itr != doc.End(); ++itr)
    {
        pArr.clear();
        aArr.clear();
        sArr.clear();
        allJobArr.clear();
        used_time = vector<bool>(100, false);
        cout << indexOfTaskGroup++ << endl; // MUST : #TaskGroup: index of TaskGroup, starting from 0
        idCount = 0;
        if (itr->GetObject()["Periodic"].IsArray())
        {
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Periodic"].GetArray().Begin(); itr2 != itr->GetObject()["Periodic"].GetArray().End(); ++itr2)
            {
                Job newJob;
                newJob.ID = idCount++;
                newJob.type = PERIODIC;
                newJob.P = itr2->GetObject()["P"].GetInt();
                newJob.C = itr2->GetObject()["C"].GetInt(); // deadline = p next arrival time
                newJob.A = 0;
                newJob.Isaccept = false;
                newJob.complete_time = newJob.A + newJob.C;
                allJobArr.push_back(newJob);
                if (newJob.P < newJob.C) // reject directily (when P < C)
                {
                    newJob.Isaccept = false;
                    cout << "P " << idCount - 1 << " -1 -1 Reject\n";
                }
                else
                {
                    newJob.Isaccept = true;
                }
                pArr.push_back(newJob);
            }
        }
        idCount = 0;
        if (itr->GetObject()["Aperiodic"].IsArray())
        {
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Aperiodic"].GetArray().Begin(); itr2 != itr->GetObject()["Aperiodic"].GetArray().End(); ++itr2)
            {
                Job newJob;
                newJob.ID = idCount++;
                newJob.type = APERIODIC;
                newJob.P = 0;
                newJob.C = itr2->GetObject()["C"].GetInt(); // deadline = a+c
                newJob.A = itr2->GetObject()["A"].GetInt();
                newJob.Isaccept = false;
                newJob.complete_time = newJob.A + newJob.C;
                allJobArr.push_back(newJob);
                aArr.push_back(newJob);
            }
        }
        idCount = 0;
        if (itr->GetObject()["Sporadic"].IsArray())
        {
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Sporadic"].GetArray().Begin(); itr2 != itr->GetObject()["Sporadic"].GetArray().End(); ++itr2)
            {
                Job newJob;
                newJob.ID = idCount++;
                newJob.type = SPORADIC;
                newJob.P = 0;
                newJob.C = itr2->GetObject()["C"].GetInt(); // deadline = a+c
                newJob.A = itr2->GetObject()["A"].GetInt();
                newJob.Isaccept = false;
                newJob.complete_time = newJob.A + newJob.C;
                allJobArr.push_back(newJob);
                sArr.push_back(newJob);
            }
        }

        // Test : Just print the task array
        // cout << "Print the task array : " << endl;
        // for (int i = 0; i < pArr.size(); i++)
        //     cout << "P" << i << " " << pArr[i].P << " " << pArr[i].C << endl;
        // for (int i = 0; i < aArr.size(); i++)
        //     cout << "A" << i << " " << aArr[i].A << " " << aArr[i].C << endl;
        // for (int i = 0; i < sArr.size(); i++)
        //     cout << "S" << i << " " << sArr[i].A << " " << sArr[i].C << endl;

        /** Step 1 : Hyperperioid **/ // V
        int hyperperiod = 1;
        for (int i = 0; i < pArr.size(); i++)
        {
            if (pArr[i].Isaccept) // If the task is not accepted, we do not consider
            {
                hyperperiod = lca(hyperperiod, pArr[i].P);
            }
        }
        
        // Schedule periodic job in HyperPerioid
        // Initialize HyperPeriodJob array every time moment is idle
        vector<struct currentJob> HyperPeriodJob; // 50 element
        for (int i = 0; i < hyperperiod; i++)
        {
            struct currentJob newJob;
            newJob.ID = -1;
            newJob.JobType = IDLE;
            HyperPeriodJob.push_back(newJob);
        }
        // I schedule the shortest period(earliest deadline) job first
        // Thus, I sort periodic job by the early deadline first
        sort(pArr.begin(), pArr.end());
        vector<int> starttime;
        for (int indexInpArr = 0; indexInpArr < pArr.size(); indexInpArr++)
        {
            if(!pArr[indexInpArr].Isaccept) // If the task is not accepted, we do not consider
                continue; 
            bool isAccept = true;
            for (int j = 0; j <= hyperperiod - pArr[indexInpArr].P; j += pArr[indexInpArr].P)
            {
                int curTime = j;
                int executionTime = 0;
                bool start = true;
                while (executionTime < pArr[indexInpArr].C)
                { 
                    if(curTime == hyperperiod) break;
                    if (curTime >= j + pArr[indexInpArr].P)
                    {
                        isAccept = false;
                        break;
                    }
                    if (HyperPeriodJob[curTime].JobType != IDLE){
                        executionTime = 0 ;
                        curTime++;
                        continue;
                    }
                    if(executionTime == 0){
                        if(start){
                            start = false;
                            starttime.push_back(curTime);
                        }
                        else
                            starttime[starttime.size()-1] = curTime;
                    }
                    curTime++;
                    executionTime++;
                }
                if (!isAccept)
                    break;
            }
            if (isAccept)
            {
                allJobArr[pArr[indexInpArr].ID].Isaccept = true;
                // record the job in HyperPeriodJob array
                for (int j = 0; j < starttime.size(); j ++)
                {
                    for(int k = starttime[j]; k < starttime[j] + pArr[indexInpArr].C; k++)
                    {
                        HyperPeriodJob[k].JobType = PERIODIC;
                        HyperPeriodJob[k].ID = pArr[indexInpArr].ID;
                    }
                }
            }
            else
            {
                allJobArr[pArr[indexInpArr].ID].Isaccept = false;
            }
            starttime.clear();
        }
        // for (int i = 0; i < hyperperiod; i++)
        // {
        //     cout << "frame" << i << ":"
        //          << " ID:" << HyperPeriodJob[i].ID << ", type:" << HyperPeriodJob[i].JobType << endl;
        // }

        // sort sporaidic job with arrival time
        sort(sArr.begin(), sArr.end());

        // construct used time frame table
        for (int i = 0; i < 100 / hyperperiod; i++)
        {
            for (int j = 0; j < hyperperiod; j++)
            {
                if (i * hyperperiod + j < 100, HyperPeriodJob[j].JobType != IDLE)
                {
                    used_time[i * hyperperiod + j] = true;
                }
            }
        }

        for (int i = 0; i < sArr.size(); i++)
        {
            int start = sArr[i].A, end = start + sArr[i].C;
            bool accept = true;
            /*check enough time for using*/
            for (int j = start; j < end && end < 100; j++)
            {
                if (used_time[j])
                    accept = false;
            }
            if (accept && end < 100)
            {
                /* disable used time */
                for (int j = start; j < end && end < 100; j++)
                {
                    used_time[j] = true;
                }
                /* IsAccept flag up */
                sArr[i].Isaccept = true;
                allJobArr[pArr.size() + aArr.size() + i].Isaccept = true;
            }
            else
            {
                /* IsAccept flag down */
                sArr[i].Isaccept = false;
                allJobArr[pArr.size() + aArr.size() + i].Isaccept = false;
                /* output reject */
                cout << "S " << sArr[i].ID << " -1 -1 Reject\n";
            }
        }


        //  for(int i = 0; i< 100; i ++){
        //      if( HyperPeriodJob[i % hyperperiod]. JobType )
        //  }
        // sort aperiodic job with arrival time
        sort(aArr.begin(), aArr.end());

        for (int i = 0; i < aArr.size(); i++)
        {
            int start = aArr[i].A, exec_time = aArr[i].C;
            int time_remaining = 0;
            /*check enough time for using*/
            int j;
            for (j = start; j < 100; j++)
            {
                if (!used_time[j])
                    time_remaining++;
            }
            /* modify complete time by soft deadline */
            aArr[i].complete_time = j;
            if (time_remaining >= exec_time)
            {
                int already_used = 0;
                /* disable used time */
                for (int j = start; j < 100 && already_used < exec_time; j++)
                {
                    if (!used_time[j])
                    {
                        used_time[j] = true;
                        already_used++;
                    }
                }
                /* IsAccept flag up */
                aArr[i].Isaccept = true;
                allJobArr[pArr.size() + i].Isaccept = true;
            }
            else
            {
                /* IsAccept flag down */
                aArr[i].Isaccept = false;
                allJobArr[pArr.size() + i].Isaccept = false;
                /* output reject */
                cout << "A " << aArr[i].ID << " -1 -1 Reject\n";
            }
        }

        /* output format */
        sort(allJobArr.begin(), allJobArr.end(), ArrTimeInceasing);
        // reject complete output
        int Arate[4] = {0};
        for (int i = 0; i < allJobArr.size(); i++)
        {
            if (!allJobArr[i].Isaccept)
                Arate[allJobArr[i].type]++;
            if (allJobArr[i].Isaccept)
            {
                if (allJobArr[i].type == PERIODIC)
                {
                    cout << "P " << allJobArr[i].ID << " " << allJobArr[i].A << " " << allJobArr[i].complete_time << " Complete\n";
                }
                else if (allJobArr[i].type == SPORADIC)
                {
                    cout << "S " << allJobArr[i].ID << " " << allJobArr[i].A << " " << allJobArr[i].complete_time << " Complete\n";
                }
                else if (allJobArr[i].type == APERIODIC)
                {
                    cout << "A " << allJobArr[i].ID << " " << allJobArr[i].A << " " << allJobArr[i].complete_time << " Complete\n";
                }
            }
        }
        // accept rate output
        if (pArr.size() != 0)
            cout << (float)Arate[0] / pArr.size() << " ";
        else
            cout << "0.0 ";
        if (aArr.size() != 0)
            cout << (float)Arate[1] / aArr.size() << " ";
        else
            cout << "0.0 ";
        if (sArr.size() != 0)
            cout << (float)Arate[2] / sArr.size() << " ";
        else
            cout << "0.0 ";
        cout << endl;
    }
    cout << "-1" << endl;
    return 0;
}
// print task state
// for (auto &temp : allJobArr)
// {
//     cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", type:" << temp.type << endl;
//     if (temp.Isaccept)
//         cout << "Accept\n";
//     else
//         cout << "Not Accept\n";
// }
// cout << "\n";
// for (auto &temp : pArr)
// {
//     cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", type:" << temp.type << endl;
//     if (temp.Isaccept)
//         cout << "Accept\n";
//     else
//         cout << "Not Accept\n";
// }
// for (auto &temp : aArr)
// {
//     cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", type:" << temp.type << endl;
//     if (temp.Isaccept)
//         cout << "Accept\n";
//     else
//         cout << "Not Accept\n";
// }
// for (auto &temp : sArr)
// {
//     cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", type:" << temp.type << endl;
//     if (temp.Isaccept)
//         cout << "Accept\n";
//     else
//         cout << "Not Accept\n";
// }