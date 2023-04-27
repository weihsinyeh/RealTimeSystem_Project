#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
  
#include <cstdio>
#include <iostream>
#include <vector>
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
    rapidjson::FileReadStream is(fp, readBuffer,
                                 sizeof(readBuffer));
  
    // Parse the JSON document
    rapidjson::Document doc;
    doc.ParseStream(is);
  
    // Check for parse errors
    if (doc.HasParseError()) {
        std::cerr << "Error parsing JSON: "
                  << doc.GetParseError() << std::endl;
        return 1;
    }
 
    vector<struct periodic> pArr;
    vector<struct aperiodic> aArr;
    vector<struct sporadic> sArr;
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
    
    for(int i = 0; i< pArr.size();i++){
        cout << "P" << i << " " << pArr[i].P << " " << pArr[i].C << endl;
    }
    for(int i = 0; i< aArr.size();i++){
        cout << "A" << i << " " << aArr[i].A << " " << aArr[i].C << endl;
    }
    for(int i = 0; i< sArr.size();i++){
        cout << "S" << i << " " << sArr[i].A << " " << sArr[i].C << endl;
    }
    return 0;
}