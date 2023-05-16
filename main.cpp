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
    int nextArrivalTime;
    bool Isaccept;
};
struct currentJob
{
    int ID;
    int JobType;
};
struct periodic
{
    int P, C;
    bool Isaccept;
};
struct aperiodic
{
    int A, C;
    bool Isaccept;
};
struct sporadic
{
    int A, C;
    bool Isaccept;
};
bool operator<(Job const &a, Job const &b)
{
    if (a.type == SPORADIC && b.type == SPORADIC) // Sporadic job sort by arrival time
        return a.A < b.A;
    if (a.type == PERIODIC && b.type == PERIODIC) // Periodic job sort by deadline
        return a.P < b.P;
    else
        return a.A < b.A;                         // Aparadic job sort by arrival time
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
    // Iterate over the array of objects
    rapidjson::Value::ConstValueIterator itr;
    int indexOfTaskGroup = 0;
    int idCount = 0;
    cout << "number of schedule : " << doc.Size() << endl; // MUST : #Schedules: number of Schedules, same as length of input vector
    for (itr = doc.Begin(); itr != doc.End(); ++itr)
    {
        pArr.clear();
        aArr.clear();
        sArr.clear();
        allJobArr.clear();
        cout << "index of taskgroup : " << indexOfTaskGroup++ << endl; // MUST : #TaskGroup: index of TaskGroup, starting from 0
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
                newJob.nextArrivalTime = 0;
                newJob.Isaccept = false;
                allJobArr.push_back(newJob);
                if (newJob.P < newJob.C) // reject directily (when P < C)
                    newJob.Isaccept = false;
                else
                {
                    newJob.Isaccept = true;
                    pArr.push_back(newJob);
                }
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
                newJob.nextArrivalTime = 0;
                newJob.Isaccept = false;
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
                newJob.nextArrivalTime = 0;
                newJob.Isaccept = false;
                allJobArr.push_back(newJob);
                sArr.push_back(newJob);
            }
        }

        // Test : Just print the task array
        cout << "Print the task array : " << endl;
        for (int i = 0; i < pArr.size(); i++)
            cout << "P" << i << " " << pArr[i].P << " " << pArr[i].C << endl;
        for (int i = 0; i < aArr.size(); i++)
            cout << "A" << i << " " << aArr[i].A << " " << aArr[i].C << endl;
        for (int i = 0; i < sArr.size(); i++)
            cout << "S" << i << " " << sArr[i].A << " " << sArr[i].C << endl;

        /** Step 1 : Hyperperioid **/ // V
        int hyperperiod = 1;
        for (int i = 0; i < pArr.size(); i++)
        {
            if(pArr[i].Isaccept) // If the task is not accepted, we do not consider
            {
                hyperperiod = lca(hyperperiod, pArr[i].P);
            }
        }
        /***********************************************************/
        /** Step 2 : Caluculate appropriate Frame size **/
        // Constraint 1 : Calculate the maximum of execution time //V
        int eMax = -1;
        for (int i = 0; i < pArr.size(); i++)
        {
            if (pArr[i].Isaccept && pArr[i].C > eMax) // If the task is not accepted, we do not consider
            {
                eMax = pArr[i].C;
            }
        }
        cout << "maximum execution time : " << eMax << endl;
        // Constraint 2 : Find pArr[i].P's factor
        set<int> possiblefactor;
        for (int i = 0; i < pArr.size(); i++)
        {
            if(pArr[i].Isaccept){ // If the task is not accepted, we do not consider
                for (int j = 1; j <= pArr[i].P; j++)
                {
                    if (pArr[i].P % j == 0)
                        possiblefactor.insert(j);
                }
            }
        }
        // Constraint 3 : Check deadline
        // iterator the set
        set<int>::iterator it;
        set<int> correctfactor;

        for (it = possiblefactor.begin(); it != possiblefactor.end(); it++)
        {
            int frameSize = *it;
            bool framesizeAccept = true;
            for (int i = 0; i < pArr.size(); i++)
            {
                if(!pArr[i].Isaccept) //// If the task is not accepted, we do not consider
                { 
                    continue;
                }
                if (!(2 * frameSize - gcd(frameSize, pArr[i].P) <= pArr[i].P)) 
                {
                    framesizeAccept = false;
                }
            }
            if (framesizeAccept)
            {
                correctfactor.insert(frameSize);
            }
        }

        /***********************************************************/
        // TODO: frameSize slice

        // Test : Just print the correct framesize need to consider later
        // cout << "possible frameSize : " << endl;
        // int maxFrameSize = -1;
        // for (it = correctfactor.begin(); it != correctfactor.end(); it++)
        // {
        //     int frameSize = *it;
        //     if (frameSize > maxFrameSize)
        //         maxFrameSize = frameSize;
        //     cout << frameSize << endl;
        // }

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

        for (int indexInpArr = 0; indexInpArr < pArr.size(); indexInpArr++)
        {
            if(!pArr[indexInpArr].Isaccept) // If the task is not accepted, we do not consider
                continue; 
            bool isAccept = true;
            // first check the perioidic task can be accept when every time it arrive
            for (int j = 0; j < hyperperiod; j += pArr[indexInpArr].P)
            {
                int curTime = j;
                int executionTime = 0;
                while (executionTime < pArr[indexInpArr].C)
                { // V
                    if (curTime >= j + pArr[indexInpArr].P)
                    {
                        isAccept = false;
                        break;
                    }
                    if (HyperPeriodJob[curTime++].JobType != IDLE)
                        continue;
                    executionTime++;
                }
                if (!isAccept)
                    break;
            }
            if (isAccept)
            {
                allJobArr[pArr[indexInpArr].ID].Isaccept = true;
                // record the job in HyperPeriodJob array
                for (int j = 0; j < hyperperiod; j += pArr[indexInpArr].P)
                {
                    int executionTime = 0;
                    int curTime = j;
                    while (executionTime < pArr[indexInpArr].C)
                    {
                        if (HyperPeriodJob[curTime].JobType != IDLE)
                        {
                            curTime++;
                            continue;
                        }
                        executionTime++;
                        HyperPeriodJob[curTime].JobType = PERIODIC;
                        HyperPeriodJob[curTime].ID = pArr[indexInpArr].ID;
                    }
                }
            }
            else
            {
                allJobArr[pArr[indexInpArr].ID].Isaccept = false;
            }
        }
        // TODO: schedule sporadic job
        // sort sporaidic job with arrival time
        sort(sArr.begin(), sArr.end());

        for (int i = 0; i < hyperperiod; i++)
        {
            cout << "frame" << i << ":"
                 << " ID:" << HyperPeriodJob[i].ID << ", type:" << HyperPeriodJob[i].JobType << endl;
        }

        for (auto &temp : pArr)
        {
            cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
            if (temp.Isaccept)
                cout << "Accept\n";
            else
                cout << "Not Accept\n";
        }
        for (auto &temp : aArr)
        {
            cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
            if (temp.Isaccept)
                cout << "Accept\n";
            else
                cout << "Not Accept\n";
        }
        for (auto &temp : sArr)
        {
            cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
            if (temp.Isaccept)
                cout << "Accept\n";
            else
                cout << "Not Accept\n";
        }

        // TODO: schedule aperiodic job W
        //  for(int i = 0; i< 100; i ++){
        //      if( HyperPeriodJob[i % hyperperiod]. JobType )
        //  }
        // sort aperiodic job with arrival time
        sort(aArr.begin(), aArr.end());

        cout << endl;
    }
    cout << "-1" << endl;
    return 0;
}
// print task state
//  for (auto &temp : pArr)
//  {
//      cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
//      if (temp.Isaccept)
//          cout << "Accept\n";
//      else
//          cout << "Not Accept\n";
//  }
//  for (auto &temp : aArr)
//  {
//      cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
//      if (temp.Isaccept)
//          cout << "Accept\n";
//      else
//          cout << "Not Accept\n";
//  }
//  for (auto &temp : sArr)
//  {
//      cout << "P:" << temp.P << ", A:" << temp.A << ", C: " << temp.C << ", ID:" << temp.ID << ", NEXT:" << temp.nextArrivalTime << ", type:" << temp.type << endl;
//      if (temp.Isaccept)
//          cout << "Accept\n";
//      else
//          cout << "Not Accept\n";
//  }