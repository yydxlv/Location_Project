#include <iostream>
#include <string>
using namespace std;
using namespace System;
void MarshalString ( String^ s, std::string& os );
bool To_CharStar(String^ source, char*& target);
bool To_string(String^ source, std::string &target);
bool JudgeNumber(std::string&);
int  Timediffer(String^ line1,String^ line2)