#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
  
#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
using namespace std;
struct periodic{
    int P,C;
};
struct aperiodic{
    int A,C;
};
struct sporadic{
    int A,C;
};
int gcd (int a,int b){
    if(b == 0) return a;
    else return gcd(b,a%b);
}
int main(int argc,char * argv[])
{
    // Open the file
    FILE* fp = fopen(argv[1], "rb");
  
    // Check if the file was opened successfully
    if (!fp) {
        std::cerr << "Error: unable to open file"
                  << std::endl;
        return 1;
    }
  
    // Read the file into a buffer
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer,sizeof(readBuffer));
  
    // Parse the JSON document
    rapidjson::Document doc;
    doc.ParseStream(is);
  
    // Check for parse errors
    if (doc.HasParseError()) {
        std::cerr << "Error parsing JSON: " << doc.GetParseError() << std::endl;
        return 1;
    }
    // Record the task array
    vector<struct periodic> pArr; //periodic task
    vector<struct aperiodic> aArr;//aperiodic task
    vector<struct sporadic> sArr; //sporadic task

    // Iterate over the array of objects
    rapidjson::Value::ConstValueIterator itr;
    for (itr = doc.Begin(); itr != doc.End(); ++itr) {
        if(itr->GetObject()["Periodic"].IsArray()){
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Periodic"].GetArray().Begin(); itr2 != itr->GetObject()["Periodic"].GetArray().End(); ++itr2){
                struct periodic newPeriodic;
                newPeriodic.P = itr2->GetObject()["P"].GetInt();
                newPeriodic.C = itr2->GetObject()["C"].GetInt();
                pArr.push_back(newPeriodic);
            }
        }
        if(itr->GetObject()["Aperiodic"].IsArray()){
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Aperiodic"].GetArray().Begin(); itr2 != itr->GetObject()["Aperiodic"].GetArray().End(); ++itr2){
                struct aperiodic newAperiodic;
                newAperiodic.A = itr2->GetObject()["A"].GetInt();
                newAperiodic.C = itr2->GetObject()["C"].GetInt();
                aArr.push_back(newAperiodic);
            }
        }
        if(itr->GetObject()["Sporadic"].IsArray()){
            rapidjson::Value::ConstValueIterator itr2;
            for (itr2 = itr->GetObject()["Sporadic"].GetArray().Begin(); itr2 != itr->GetObject()["Sporadic"].GetArray().End(); ++itr2){
                struct sporadic newSporadic;
                newSporadic.A = itr2->GetObject()["A"].GetInt();
                newSporadic.C = itr2->GetObject()["C"].GetInt();
                sArr.push_back(newSporadic);
            }
        }
    }
    // Just print the task array
    for(int i = 0; i< pArr.size();i++){
        cout << "P" << i << " " << pArr[i].P << " " << pArr[i].C << endl;
    }
    for(int i = 0; i< aArr.size();i++){
        cout << "A" << i << " " << aArr[i].A << " " << aArr[i].C << endl;
    }
    for(int i = 0; i< sArr.size();i++){
        cout << "S" << i << " " << sArr[i].A << " " << sArr[i].C << endl;
    }
    //Frame size
    //Calculate the maximum of execution time
    //TODO: find the maximum of execution time (check need to work)
    /*******************
    int eMax = -1;  //the maximum of execution time 
    for(int i = 0; i< pArr.size();i++){
        if(pArr[i].C > eMax) eMax = pArr[i].C;
    }
    cout<<eMax<<endl;
    *******************/
    //find pArr[i].P's factor
    set<int> possiblefactor;
    for(int i = 0; i< pArr.size();i++){
        for(int j = 1; j <= pArr[i].P; j++){
            if(pArr[i].P % j == 0 ) possiblefactor.insert(j);
        }
    }
    //Check deadline
    //iterator the set
    set<int>::iterator it;
    set<int> correctfactor;
    for(it = possiblefactor.begin(); it != possiblefactor.end(); it++){
        int frameSize = *it;
        for(int i = 0; i< pArr.size();i++){
            if(2 * frameSize - gcd(frameSize,pArr[i].P)<= pArr[i].P)
                correctfactor.insert(frameSize);
            else
                break;
        }
    }
    // Just print the correct framesize need to consider later
    for(it = correctfactor.begin(); it != correctfactor.end(); it++){
        int frameSize = *it;
        cout<<frameSize<<endl;
    }
    return 0;
}