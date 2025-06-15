#include <iostream>
#include <fstream>//universal header file for all modes
#include <string>
using namespace std;
int main()
{
    string line;

    fstream my_file;//file to be used for the code

    //WRITE MODE
    my_file.open("intern.txt", ios::out);//opening the file in write mode

    if(!my_file)//checks if the file can be found or available or not
    {
        cout<<"Unable to open the file!"<<endl;
        return 1;
    }

    //now we write data into the file
    my_file<<"ABC"<<endl;
    my_file<<"PQR"<<endl;
    my_file.close();

    //APPEND MODE
    my_file.open("intern.txt", ios::app);//opening the file in append mode

    //now we append into the file
    my_file<<"XYZ"<<endl;
    my_file.close();

    //READ MODE
    my_file.open("intern.txt", ios::in);//reads from the file
    while(!my_file.eof())//loop runs to the end of the file
    {
        getline(my_file, line);//stores the current line of the file in 'line' variable
        cout<<line<<endl;//prints the 'line' variable
    }

    my_file.close();
    return 0;
}