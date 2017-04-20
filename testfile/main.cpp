#include <iostream>
#include "serial.h"
#include "time.h"
using namespace std;

int main()
{
    Serial *_PSU = new Serial();

        _PSU->newPort("COM17");
            cout << "Serial Started >" << endl;

            if(!_PSU->IsConnected()){
                    cout << "\nSerial Failed >" << endl;
               _PSU = new Serial();}
            else{
                    cout << "\nSerial Worked? >" << endl;
              //  _PSU->WriteData("ABORt ; SYST:PRES ; DISPlay:MENU:NAME 3\n",255);
           /*     char buf[100];
                Sleep(50);
            _PSU->ReadData(buf,100);
            //strcpy(buf,"app");
            for( int i = 0 ; i < strlen(buf) ; i ++ ){
                cout << buf[i];}
            cout << endl;*/
                _PSU->WriteData("OUTP:STAT ON\n",255);
             /*   char buf2[100];
                Sleep(50);
            _PSU->ReadData(buf2,100);
            for( int i = 0 ; i < strlen(buf2) ; i ++ ){
                cout << buf2[i];}
            cout << endl;*/


            _PSU->WriteData("APPL 0.1\n",255);
                char buf3[100];
                Sleep(50);
            _PSU->ReadData(buf3,100);
            for( int i = 0 ; i < strlen(buf3) ; i ++ ){
                cout << buf3[i];}
            cout << endl;
                }
cout << "\nSerial >" << endl;

        _PSU->WriteData("MEAS:SCAL:VOLT:DC\n",255);
        char buf[100];
        Sleep(100);
    _PSU->ReadData(buf,100);
    //strcpy(buf,"app");
    for( int i = 0 ; i < strlen(buf) ; i ++ ){
        cout << buf[i];}
    cout << endl;
    cout << "\nSerial Ended >" << endl;
    return 0;
}
