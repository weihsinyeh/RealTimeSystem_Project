#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
using namespace rapidjson;
int main(){
// JSON string
const char *json = "{\"name\":\"Joe\",\"grade\":\"A\"}";
// Parsing the JSON string into DOM.
Document DOM;
DOM.Parse(json);
// Stringifying the DOM
StringBuffer buffer;
Writer<StringBuffer> writer(buffer);
DOM.Accept(writer);
std::string completeJsonData = buffer.GetString();
std::cout << completeJsonData << std::endl;
return 0;
}