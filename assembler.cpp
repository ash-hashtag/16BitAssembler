#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
using std::ofstream;

struct Timer{
    long long m_start, m_end;
    Timer()
    {
        m_start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
    }
    ~Timer()
    {
        m_end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
        double duration = (m_end - m_start) * 0.001;
        cout << "\nTime taken: " << duration << " ms\n";
    }
};

struct variables{
   string varName;
   short unsigned int insNum;
};

variables vars[2000] = {{"KBD", 24576}, {"SCREEN", 16384}, {"SP", 0}, {"LCL", 1}, {"THIS", 3}, {"THAT", 4}, {"ARG", 2}};
short unsigned int n_vars = 0, labels = 0;

short unsigned int variabledetected(string var_name, short unsigned int n, bool Var){
   short int i;
   bool found = false;
   for (i = 0; i < n_vars+labels+7; i++)
   {
      if (var_name == vars[i].varName)
      {
         n = vars[i].insNum;
         found = true;
         break;
      }
   }
   if (found == false)
   {
      vars[n_vars+labels+7] = {var_name, n};
      if (Var)
         n_vars++;
      else
         labels++;
   }
   return n;
}

string ains(short unsigned int n){
   string bn = "0000000000000000";
   int i;
   for(i = 15; n!=0; n/=2)
      bn[i--] = (char)(48 + n%2);
   return bn;
}

string cins(string d, string c, string j){
    //c instruction = 111accccccdddjjj
   string cin = "1110cccccc000000";
   int i;
   for(i = 0; d[i]; i++)
   {
      if (d[i] == 'A')
         cin[10] = '1';
      if (d[i] == 'D')
         cin[11] = '1';
      if (d[i] == 'M')
         cin[12] = '1';
   }


   for(i = 0; c[i]; i++)
   {
      if(c[i]=='M')
         cin[3]='1';
   }
   if (c=="0")
      c = "101010";
   else if(c=="1")
      c = "111111";
   else if(c=="-1")
      c = "111010";
   else if(c=="D")
      c = "001100";
   else if(c=="A"||c=="M")
      c = "110000";
   else if(c=="!D")
      c = "001101";
   else if(c=="!A"||c=="!M")
      c = "110001";
   else if(c=="-D")
      c = "001111";
   else if(c=="-M"||c=="-A")
      c = "110011";
   else if(c=="D+1")
      c = "011111";
   else if(c=="M+1"||c=="A+1")
      c = "110111";
   else if(c=="D-1")
      c = "001110";
   else if(c=="A-1"||c=="M-1")
      c = "110010";
   else if(c=="D+A"||c=="D+M")
      c = "000010";
   else if(c=="D-A"||c=="D-M")
      c = "010011";
   else if(c=="A-D"||c=="M-D")
      c = "000111";
   else if(c=="D&A"||c=="D&M")
      c = "000000";
   else if(c=="D|A"||c=="D|M")
      c = "010101";
   else
      c = "000000";

   if (j == "JGT")
      j = "001";
   else if(j =="JEQ")
      j = "010";
   else if(j =="JGE")
      j = "011";
   else if(j =="JLT")
      j = "100";
   else if(j =="JNE")
      j = "101";
   else if(j =="JLE")
      j = "110";
   else if(j =="JMP")
      j = "111";
   else
      j = "000";
   for(i=4;i<10;i++)
      cin[i]=c[i-4];
   for(i=13;i<16;i++)
      cin[i]=j[i-13];
   return cin;
}

string removespaces(string i){
   short unsigned int c=0,j;
   char wos[50];
   for (j = 0; i[j]; j++){
      if(i[j]=='/')
         break;
      if(i[j]==9||i[j]==32);
      else{
         wos[c]=i[j];
         c++;
      }
   }
   wos[c] = '\0';
   if(c!=0)
      return (string)wos;
   else
      return "";
}

string strings[2];

void split(string str, char seperator)  
{   
    strings[0] = "";
    strings[1] = "";
    short unsigned int currIndex = 0, i = 0, startIndex = 0, endIndex = 0;  
    while (i <= str.length())  
    {  
        if (str[i] == seperator || i == (str.length()))  
        {  
            endIndex = i;  
            string subStr = "";  
            subStr.append(str, startIndex, endIndex - startIndex);
            strings[currIndex] = subStr;  
            currIndex += 1;  
            startIndex = endIndex + 1;  
        }  
        i++;  
   }
         
}


void findLabels(string pin)
{
   int inscount = 0;
   fstream file;
   file.open(pin, ios::in);

   string ins;
   while(getline(file, ins))
   {
      ins = removespaces(ins);
      if (ins.length())
      {
         if(ins[0]=='(')
         {
            split(ins, '(');
            split(strings[1], ')');
            short unsigned int mk = variabledetected(strings[0], inscount, false);
         }
         else
            inscount++;
      }
   }
   file.close();
}


string convertins(string ins)
{
   if(ins[0] == '@')
   {
      split(ins, '@');
      if (ins[1] > 59 && ins[1] != 'R')
         return ains(variabledetected(strings[1], n_vars+16, true)) + "\n";
      else if (ins[1] == 'R' && ins[2] < 58)
         return ains(stoi(ins.substr(2,5))) + "\n";
      else if (ins[1] < 58)
         return ains(stoi(ins.substr(1,5))) + "\n";
      else
         return ains(variabledetected(strings[1], 0, false)) + "\n";
   }
   else if(ins.length() && ins[0] != '(')
   {
      split(ins, '=');
      string d = strings[0];
      if (strings[1].length()==0){
         d="";
         split(strings[0], ';');
      }
      else
         split(strings[1], ';');
      return cins(d, strings[0], strings[1]) + "\n";
   }
   else
      return "";
}
/*
int main()
{
   cout<<convertins(removespaces("@loop_ball.setdestination"));
   return 0;
}*/


int main(int argc, char *argv[])
{
   Timer timer;
   fstream newfile;
   ofstream outfile;
   string pin = argv[1],pout = argv[2];
   findLabels(pin);
   outfile.open(pout,ios::out);
   newfile.open(pin,ios::in);
   if (newfile.is_open())
   {
      string tp;
      int inscount = 0;
      while(getline(newfile, tp))
      {
         string wosp = removespaces(tp);
         if (removespaces(tp).length())
         {
            inscount++;
            //cout<<wosp<<endl;
         }
         outfile<<convertins(wosp);
      }
      newfile.close();
      outfile.close();
      cout<<"No. of Instructions : "<<inscount;
   }
   else
      cout<<"Error opening file"<<endl;
   cout<<"No. of variables and labels: "<< n_vars+labels+7;
   return 0;
}