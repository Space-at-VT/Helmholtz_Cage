#include <iostream>
#include "serial.h"
#include "time.h"
#include <QVector>
using namespace std;

int main()
{
    Serial *_PSU = new Serial();

        _PSU->newPort("COM17");
        Sleep(50);
            cout << "Serial Started >" << endl;

            if(_PSU->IsConnected()){
                std::cout << "in";
                _PSU->WriteData("ABOR\n",5);
                Sleep(50);
                _PSU->WriteData("SYST:PRES\n",10);
                Sleep(50);
                _PSU->WriteData("DISP:MENU:NAME 3\n",17);
                Sleep(50);
                _PSU->WriteData(QString::fromStdString("OUTP:STAT ON\n").toStdString().c_str(),13);
                Sleep(50);
                _PSU->WriteData("APPL 0.99\n",10);
                Sleep(50);
                _PSU->WriteData("APPL 1.99\n",10);
                Sleep(50);
                _PSU->WriteData("APPL 2.99\n",10);
                Sleep(50);
                _PSU->WriteData("APPL 3.99\n",10);
                Sleep(50);
                _PSU->WriteData("APPL 4.99\n",10);
                Sleep(50);
                _PSU->WriteData("APPL 0.99\n",10);
                }

            delete _PSU;
    cout << "\nSerial Ended >" << endl;
    return 0;
}
