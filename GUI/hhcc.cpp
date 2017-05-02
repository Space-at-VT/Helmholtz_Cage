#ifndef HHCC_CPP
#define HHCC_CPP

#include "HHCC.h"
#include "ui_HHCC.h"
#include <algorithm>
#include <iostream>
#include <QDebug>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>

HHCC::HHCC(QWidget *parent) : QMainWindow(parent), ui(new Ui::HHCC)
{
    ui->setupUi(this);
    resetAll();
}

HHCC::~HHCC()
{
    disablePower();
    delete ui;
    delete _arduino;
    delete _PSUX;
    delete _PSUY;
    delete _PSUZ;
}

// Setups signals for use
void HHCC::signalSetup(){ // Complete
    QObject::connect(ui->browse_button, &QPushButton::clicked, this, &HHCC::browseFile);
    QObject::connect(ui->testData_button, &QPushButton::clicked, this, &HHCC::loadTestingData);
    QObject::connect(ui->extract_button, &QPushButton::clicked, this, &HHCC::extractData);

    QObject::connect(ui->connect_com_button, &QPushButton::clicked, this, &HHCC::connectPorts);
    QObject::connect(ui->disconnect_com_button, &QPushButton::clicked, this, &HHCC::disconnectPorts);
    QObject::connect(ui->psuX_com_ledit, &QLineEdit::textEdited, this, &HHCC::checkPortLabels);
    QObject::connect(ui->psuY_com_ledit, &QLineEdit::textEdited, this, &HHCC::checkPortLabels);
    QObject::connect(ui->psuZ_com_ledit, &QLineEdit::textEdited, this, &HHCC::checkPortLabels);
    QObject::connect(ui->ardPort_com_ledit, &QLineEdit::textEdited, this, &HHCC::checkPortLabels);

    QObject::connect(ui->delay_sim_ledit, &QLineEdit::textEdited, this, &HHCC::checkSimulationControls);
    //QObject::connect(ui->units_sim_cbox,  &QComboBox::highlighted, this, &HHCC::checkSimulationControls);
    QObject::connect(ui->offsetX_sim_ledit, &QLineEdit::textEdited, this, &HHCC::checkSimulationControls);
    QObject::connect(ui->offsetY_sim_ledit, &QLineEdit::textEdited, this, &HHCC::checkSimulationControls);
    QObject::connect(ui->offsetZ_sim_ledit, &QLineEdit::textEdited, this, &HHCC::checkSimulationControls);
    QObject::connect(ui->runSim_sim_button, &QPushButton::clicked, this, &HHCC::runSimulation);
    QObject::connect(ui->endSim_sim_button, &QPushButton::clicked, this, &HHCC::endSimulation);

    QObject::connect(ui->reset_info_button, &QPushButton::clicked, this, &HHCC::resetAll);
    QObject::connect(this,&HHCC::update,this,&HHCC::updateMeasurements);
}

// Sets active-ness of components
void HHCC::setActive(const QString& section){ // Complete
    if(section == "data"){
        if(_flags[0] == 0){
            ui->coorSystem_data_label2->setText(_coorSystemTmp);
            ui->units_data_label2->setText(_unitsTmp);
            ui->numData_data_label2->setText(to_QString(_numDataTmp));
            ui->extract_button->setEnabled(true);}
        else{
            ui->extract_button->setEnabled(false);
            _unitsTmp = "";
            _coorSystemTmp = "";
            _filenameTmp = "";
            _numDataTmp = 0;}
        _flags[0] = 0;}
    else if(section == "com"){
        if(_flags[1] == 1){
            ui->connect_com_button->setEnabled(false);
            ui->disconnect_com_button->setEnabled(true);
            ui->psuX_com_ledit->setDisabled(true);
            ui->psuY_com_ledit->setDisabled(true);
            ui->psuZ_com_ledit->setDisabled(true);
            ui->ardPort_com_ledit->setDisabled(true);}
        else if(_flags[1] == 0){
            ui->connect_com_button->setEnabled(true);
            ui->disconnect_com_button->setEnabled(false);
            ui->psuX_com_ledit->setEnabled(true);
            ui->psuY_com_ledit->setEnabled(true);
            ui->psuZ_com_ledit->setEnabled(true);
            ui->ardPort_com_ledit->setEnabled(true);}
        else if(_flags[1] == -1){
            ui->connect_com_button->setEnabled(false);
            ui->disconnect_com_button->setEnabled(false);
            ui->psuX_com_ledit->setEnabled(true);
            ui->psuY_com_ledit->setEnabled(true);
            ui->psuZ_com_ledit->setEnabled(true);
            ui->ardPort_com_ledit->setEnabled(true);}}
    else if(section == "sim"){
        if(_flags[2] == -1 || _flags[1] != 1 || _flags[0] != 0){
            ui->runSim_sim_button ->setEnabled(false);
            ui->endSim_sim_button->setEnabled(false);}
        else if(_flags[2] == 0 && _flags[1] == 1 && _flags[0] == 0){
            ui->runSim_sim_button ->setEnabled(true);
            ui->endSim_sim_button->setEnabled(false);
            ui->reset_info_button->setEnabled(true);
            ui->data_tab->setEnabled(true);
            ui->com_tab->setEnabled(true);
            ui->help_tab->setEnabled(true);
            ui->delay_sim_ledit->setEnabled(true);
            ui->units_sim_cbox->setEnabled(true);
            ui->offsetX_sim_ledit->setEnabled(true);
            ui->offsetY_sim_ledit->setEnabled(true);
            ui->offsetZ_sim_ledit->setEnabled(true);}
        else if(_flags[2] == 1){
            ui->runSim_sim_button ->setEnabled(false);
            ui->endSim_sim_button->setEnabled(true);
            ui->reset_info_button->setEnabled(false);
            ui->data_tab->setEnabled(false);
            ui->com_tab->setEnabled(false);
            ui->help_tab->setEnabled(false);
            ui->delay_sim_ledit->setEnabled(false);
            ui->units_sim_cbox->setEnabled(false);
            ui->offsetX_sim_ledit->setEnabled(false);
            ui->offsetY_sim_ledit->setEnabled(false);
            ui->offsetZ_sim_ledit->setEnabled(false);}}
}
// Updates all the info for the infobox
void HHCC::infoBoxUpdate(){ // Complete
    int currValue = 0;
    if(_numData != 0)
        currValue += 33;
    else if(_numDataTmp != 0)
        currValue += 16;

    if(_flags[1] == 0)
        currValue += 16;
    else if(_flags[1] == 1)
        currValue += 33;

    if(_flags[2] == 0)
        currValue += 16;
    else if(_flags[2] == 1)
        currValue += 34;

    ui->progressBar->setValue(currValue);
    ui->numData_info_label2->setText(to_QString(_numData));
    ui->magUnit_info_label2->setText(_units);
    ui->coorSystem_info_label2->setText(_coorSystem);
    if(_unitsTime.toStdString() == "min")
        _delayTime = (double)(_delay*60*1000);
    else if(_unitsTime.toStdString() == "sec")
        _delayTime = (double)(_delay*1000);
    else
        _delayTime = (double)(_delay);
    ui->delay_info_label2->setText(to_QString(_delay)+ " " + _unitsTime);
    convertUnits();
    ui->simTime_sim_label2->setText(_simTime);
    if(_flags[1] == 1 && !ui->ignorePSU_cbox->isChecked()){
        ui->psuX_info_label2->setText(_PSUX->checkPort());
        ui->psuY_info_label2->setText(_PSUY->checkPort());
        ui->psuZ_info_label2->setText(_PSUZ->checkPort());}
    else if(_flags[1] == 1){
        ui->psuX_info_label2->setText("N/A");
        ui->psuY_info_label2->setText("N/A");
        ui->psuZ_info_label2->setText("N/A");}
    else{
        ui->psuX_info_label2->setText("");
        ui->psuY_info_label2->setText("");
        ui->psuZ_info_label2->setText("");}

    if(_flags[1] == 1 && !ui->ignoreArd_cbox->isChecked()){
        ui->ardPort_info_label2->setText(_arduino->checkPort());}
    else if(_flags[1] == 1){
        ui->ardPort_info_label2->setText("N/A");}
    else{
        ui->ardPort_info_label2->setText("");}

    if(_flags[2] == 1)
        ui->store_cbox->setEnabled(false);
    else
        ui->store_cbox->setEnabled(true);

    if(_flags[1] == 1){
        ui->ignoreArd_cbox->setEnabled(false);
        ui->ignorePSU_cbox->setEnabled(false);}
    else{
        ui->ignoreArd_cbox->setEnabled(true);
        ui->ignorePSU_cbox->setEnabled(true);}

    ui->dm_com_widget->clearContents();
    ui->dm_com_widget->setRowCount(0);
    ui->dm_com_widget->setColumnWidth(0, 100);
    ui->dm_com_widget->setColumnWidth(1, 100);
    QList<QSerialPortInfo> tmpSerialList = QSerialPortInfo::availablePorts();
    for(int idx = 0; idx < 20;idx++){
        ui->dm_com_widget->insertRow(ui->dm_com_widget->rowCount());
        int tmpint = 0;
        if(!tmpSerialList.isEmpty()){
            QString tmp = tmpSerialList.front().portName();
            tmp = tmp.mid(3);
            tmpint = tmp.toInt();}
        if(tmpint == idx){
            QTableWidgetItem *newItem = new QTableWidgetItem(tmpSerialList.front().portName());
            QTableWidgetItem *newItem2 = new QTableWidgetItem("Connected");
            ui->dm_com_widget->setItem(idx-1,0, newItem);
            ui->dm_com_widget->setItem(idx-1,1, newItem2);
            tmpSerialList.removeFirst();}
        else{
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::fromStdString("COM"+std::to_string(idx)));
            QTableWidgetItem *newItem2 = new QTableWidgetItem("Disconnected");
            ui->dm_com_widget->setItem(idx-1,0, newItem);
            ui->dm_com_widget->setItem(idx-1,1, newItem2);}}
}
// Resets all values
void HHCC::resetAll(){ // Complete
    _flags[0] = 0;
    _flags[1] = -1;
    _flags[2] = -1;
    _unitsTmp = "";
    _coorSystemTmp = "";
    _filenameTmp = "";
    _numDataTmp = 0;
    _units = "";
    _coorSystem = "";
    _filename = "";
    _numData = 0;
    _offsetX = 0;
    _offsetY = 0;
    _offsetZ = 0;
    _delay = 0;
    _unitsTime = "";
    signalSetup();
    _rawData.clear();
    _xData.clear();
    _yData.clear();
    _zData.clear();
    QVector<double> tmp;
    _rawData.push_back(tmp);
    _rawData.push_back(tmp);
    _rawData.push_back(tmp);
    QStringList tmpList;
    tmpList.push_back("BField X");
    tmpList.push_back("BField Y");
    tmpList.push_back("BField Z");
    ui->table_data_widget->setHorizontalHeaderLabels(tmpList);
    ui->table_data_widget->clearContents();
    ui->table_data_widget->setRowCount(0);
    ui->table_data_widget->setColumnWidth(0, 80);
    ui->table_data_widget->setColumnWidth(1, 80);
    ui->table_data_widget->setColumnWidth(2, 80);

    _arduino = new Serial();
    _PSUX = new Serial();
    _PSUY = new Serial();
    _PSUZ = new Serial();

    ui->units_sim_cbox->clear();
    QStringList tmpList2;
    tmpList2.push_back("");
    tmpList2.push_back("ms");
    tmpList2.push_back("sec");
    tmpList2.push_back("min");
    ui->units_sim_cbox->addItems(tmpList2);

    QStringList tmpList3;
    tmpList3.push_back("Port Names");
    tmpList3.push_back("Status");
    ui->dm_com_widget->setHorizontalHeaderLabels(tmpList3);
    ui->dm_com_widget->clearContents();
    ui->dm_com_widget->setRowCount(0);
    ui->dm_com_widget->setColumnWidth(0, 100);
    ui->dm_com_widget->setColumnWidth(1, 100);

    ui->path_ledit->setText("");
    ui->psuX_com_ledit->setText("");
    ui->psuY_com_ledit->setText("");
    ui->psuZ_com_ledit->setText("");
    ui->status_data_label->setText("Browse for file or use testing data!");
    ui->status_com_label->setText("Use Device Manager to find ports!");
    ui->simProgress_sim_bar->setValue(0);
    ui->delay_sim_ledit->setText("");
    ui->offsetX_sim_ledit->setText("");
    ui->offsetY_sim_ledit->setText("");
    ui->offsetZ_sim_ledit->setText("");

    ui->simTime_sim_label2->setText("");
    ui->offsetX_sim_ledit->setText("");
    ui->offsetY_sim_ledit->setText("");
    ui->offsetZ_sim_ledit->setText("");

    _theoX = 0;
    _theoY = 0;
    _theoZ = 0;
    _measX = 0;
    _measY = 0;
    _measZ = 0;
    _voltX = 0;
    _voltY = 0;
    _voltZ = 0;
    _currX = 0;
    _currY = 0;
    _currZ = 0;
    _simProgress = 0;
    QVector<double> tmpVec;
    _measData.clear();
    _measData.push_back(tmpVec);
    _measData.push_back(tmpVec);
    _measData.push_back(tmpVec);
    _voltData.clear();
    _voltData.push_back(tmpVec);
    _voltData.push_back(tmpVec);
    _voltData.push_back(tmpVec);
    _currData.clear();
    _currData.push_back(tmpVec);
    _currData.push_back(tmpVec);
    _currData.push_back(tmpVec);

    ui->measX_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->voltX_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->currX_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->measY_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->voltY_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->currY_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->currZ_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->voltZ_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->measZ_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->theorX_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->theorY_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->theorZ_sim_label->setText(QApplication::translate("m_Window", "", 0));
    ui->simProgress_sim_bar->setValue(0);
    _simTime = "";
    _delayTime = 0;

    _simPSUData.clear();
    _simArdData.clear();
    QVector<QString> tmpQVec;
    _simPSUData.push_back(tmpQVec);
    _simPSUData.push_back(tmpQVec);
    _simPSUData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);
    setActive("data");
    setActive("com");
    setActive("sim");
    infoBoxUpdate();
    endSim = false;
}

// Loads simple testing data
void HHCC::loadTestingData(){ // Complete
    _unitsTmp = "nT";
    _coorSystemTmp = "N/A";
    _filenameTmp = "N/A";
    ui->path_ledit->setText(_filenameTmp);
    _numDataTmp = 100;
    ui->table_data_widget->clearContents();
    ui->table_data_widget->setRowCount(0);
    ui->table_data_widget->setColumnWidth(0, 80);
    ui->table_data_widget->setColumnWidth(1, 80);
    ui->table_data_widget->setColumnWidth(2, 80);
    _rawData[0].clear();
    _rawData[1].clear();
    _rawData[2].clear();
    for(int idx2 = 0; idx2 < 100; idx2++){
        ui->table_data_widget->insertRow(ui->table_data_widget->rowCount());
        for(int idx = 0; idx < 3; idx++){
            if((idx == 0 && idx2 < 33) ||(idx == 1 && idx2 < 66  && idx2 >= 33) || (idx == 2 && idx2 >= 66))
                _rawData[idx].push_back(3000*(idx2+1));
            else
                _rawData[idx].push_back(0);
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(_rawData[idx].last()));
            ui->table_data_widget->setItem(idx2, idx, newItem);
        }}
    setActive("data");
    infoBoxUpdate();
}
// Browses for the file
void HHCC::browseFile(){ // Complete
    QString filename = QFileDialog::getOpenFileName(0, "Open a file", "directoryToOpen","Comma-separated values (*.csv)");
    std::string tmp_string;
    std::string line;
    QString coorSystem[2] = {"ECI","ECF"};
    QString units[3] = {"(nT)","(T)","G"};
    QString coorDirection[3] = {"x","y","z"};

    std::ifstream file;
    if(filename.mid(filename.size() - 4) != ".csv" && _flags[0] != -1){
        ui->status_data_label->setText(QApplication::translate("m_Window", "Incorrect File Type", 0));
        _flags[0] = -1;}
    else if(_flags[0] != -1){
        _filenameTmp = filename;
        ui->path_ledit->setText(_filenameTmp);
        file.open(filename.toStdString());
    }

    std::getline(file,line);


    std::istringstream iss(line);
    if(ui->ignoreFirst_cbox->isChecked() && _flags[0] != -1){
        _unitsTmp = "nT";
        _coorSystemTmp = "N/A";}
    else if(_flags[0] != -1)
        for(int idx = 0; idx < 3; idx++){
            std::getline(iss, tmp_string, ',');
            if (tmp_string.find(coorDirection[idx].toStdString()) != std::string::npos && _flags[0] != -1) {
                for(int idx2 = 0; idx2 < 3; idx2++){
                    if (tmp_string.find(units[idx2].toStdString()) != std::string::npos && _flags[0] != -1) {
                        if(tmp_string.find(coorSystem[0].toStdString()) != std::string::npos){
                            _unitsTmp = units[idx2];
                            _coorSystemTmp = coorSystem[0];}
                        else if(tmp_string.find(coorSystem[1].toStdString()) != std::string::npos){
                             _unitsTmp = units[idx2];
                            _coorSystemTmp = coorSystem[1];}
                        else if(_flags[0] != -1 && idx2 == 2){
                            ui->status_data_label->setText(QApplication::translate("m_Window", "Header was in the wrong format", 0));
                            _flags[0] = -1;
                            break;}
                        break;}
                   else if(_flags[0] != - 1 && idx2 == 2){
                       ui->status_data_label->setText(QApplication::translate("m_Window", "Header was in the wrong format", 0));
                       _flags[0] = -1;
                   break;}}
                break;}
            else if(_flags[0] != - 1 && idx == 2){
                ui->status_data_label->setText(QApplication::translate("m_Window", "Header was in the wrong format", 0));
                _flags[0] = -1;}}

    if(file.is_open() && _flags[0] != -1){
        int idx2 = 0;
        ui->table_data_widget->clearContents();
        ui->table_data_widget->setRowCount(0);
        ui->table_data_widget->setColumnWidth(0, 80);
        ui->table_data_widget->setColumnWidth(1, 80);
        ui->table_data_widget->setColumnWidth(2, 80);
        _rawData[0].clear();
        _rawData[1].clear();
        _rawData[2].clear();
        while(std::getline(file,line) && _flags[0] != -1){
            std::istringstream iss(line);
            ui->table_data_widget->insertRow(ui->table_data_widget->rowCount());
            for(int idx = 0; idx < 3; idx++){
                std::getline(iss, tmp_string, ',');
                if(!checkString(tmp_string,"digits")){
                    ui->status_data_label->setText(QApplication::translate("m_Window", "File Formating Incorrect", 0));
                    _flags[0] = -1;
                    break;}
                _rawData[idx].push_back(atof(line.c_str()));
                QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(atof(line.c_str())));
                ui->table_data_widget->setItem(idx2, idx, newItem);}
            idx2++;}
        _numDataTmp = _rawData[0].size();}
    else if(_flags[0] != -1){
        ui->status_data_label->setText(QApplication::translate("m_Window", "Could Not Open File", 0));
        _flags[0] = -1;}
    setActive("data");
    infoBoxUpdate();
}
// Extracts data for use in the sim
void HHCC::extractData(){ // Complete
    _xData = _rawData[0];
    _yData = _rawData[1];
    _zData = _rawData[2];

    _units = _unitsTmp;
    _coorSystem = _coorSystemTmp;
    _filename = _filenameTmp;
    _numData = _numDataTmp;
    setActive("data");
    setActive("sim");
    infoBoxUpdate();
}

QString HHCC::arduino(QString data){
    char output[4];
    char incomingData[4];
    std::string input_string = data.toStdString();
    char *c_string = new char[input_string.size() + 1];
    std::copy(input_string.begin(), input_string.end(), c_string);
    c_string[input_string.size()] = '\n';

    _arduino->WriteData(c_string, 4);
    Sleep(100);
    _arduino->ReadData(output, 4);
    //qDebug() << "output" << output;
    std::string test(output);
    QString desired = QString::fromStdString(test).mid(0, QString::fromStdString(test).indexOf("\r"));
    delete[] c_string;
    return desired;
}


// Check if ports have been set
void HHCC::checkPortLabels(){ // Complete
    if((checkString(ui->psuX_com_ledit->text().toStdString(),"alpha") && (ui->psuX_com_ledit->text().toStdString().find("COM") != std::string::npos) ) &&
            (checkString(ui->psuY_com_ledit->text().toStdString(),"alpha") && (ui->psuY_com_ledit->text().toStdString().find("COM") != std::string::npos) ) &&
            (checkString(ui->psuZ_com_ledit->text().toStdString(),"alpha") && (ui->psuZ_com_ledit->text().toStdString().find("COM") != std::string::npos) ) &&
            (checkString(ui->ardPort_com_ledit->text().toStdString(),"alpha") && (ui->ardPort_com_ledit->text().toStdString().find("COM") != std::string::npos) ))
                _flags[1] = 0;
    else
        _flags[1] = -1;
    setActive("com");
    infoBoxUpdate();
}
// Connect to Ports
void HHCC::connectPorts(){ // Needs Testing
    if(!ui->ignoreArd_cbox->isChecked()){
        _arduino->newPort(ui->ardPort_com_ledit->text().toStdString());
        if(!_arduino->IsConnected()){
            ui->status_com_label->setText("Could Not Connect to Arduino");
            delete _arduino;
            _arduino = new Serial();}}
    if(!ui->ignorePSU_cbox->isChecked()){
        _PSUX->newPort(ui->psuX_com_ledit->text().toStdString());
        _PSUY->newPort(ui->psuY_com_ledit->text().toStdString());
        _PSUZ->newPort(ui->psuZ_com_ledit->text().toStdString());
        Sleep(50);
         if(!_PSUX->IsConnected() || !_PSUY->IsConnected() || !_PSUZ->IsConnected()){
            ui->status_com_label->setText("Could Not Connect to PSUs");
            delete _PSUX;
            delete _PSUY;
            delete _PSUZ;
            _PSUX = new Serial();
            _PSUY = new Serial();
            _PSUZ = new Serial();}
         else{
             _PSUX->WriteData("ABOR\n",5);
             Sleep(50);
             _PSUX->WriteData("SYST:PRES\n",10);
             Sleep(50);
             _PSUX->WriteData("DISP:MENU:NAME 3\n",17);
             Sleep(50);
             _PSUY->WriteData("ABOR\n",5);
             Sleep(50);
             _PSUY->WriteData("SYST:PRES\n",10);
             Sleep(50);
             _PSUY->WriteData("DISP:MENU:NAME 3\n",17);
             Sleep(50);
             _PSUZ->WriteData("ABOR\n",5);
             Sleep(50);
             _PSUZ->WriteData("SYST:PRES\n",10);
             Sleep(50);
             _PSUZ->WriteData("DISP:MENU:NAME 3\n",17);
             Sleep(50);
             _PSUX->WriteData("OUTP:STAT ON\n",13);
             Sleep(50);
             _PSUY->WriteData("OUTP:STAT ON\n",13);
             Sleep(50);
             _PSUZ->WriteData("OUTP:STAT ON\n",13);
             Sleep(50);}}
    disablePower();
    if( (ui->ignoreArd_cbox->isChecked() || _arduino->IsConnected()) &&
            (ui->ignorePSU_cbox->isChecked() || (_PSUX->IsConnected() && _PSUY->IsConnected() && _PSUZ->IsConnected())) ){
        _flags[1] = 1;
        setActive("com");
        setActive("sim");}
    infoBoxUpdate();
}
// Disui->connect to Ports
void HHCC::disconnectPorts(){ // Complete
    disablePower();
    if(!ui->ignoreArd_cbox->isChecked()){
        delete _arduino;
        _arduino = new Serial();}

    if(!ui->ignorePSU_cbox->isChecked()){
        _PSUX->WriteData("ABOR\n",5);
        Sleep(50);
        _PSUX->WriteData("SYST:PRES\n",10);
        Sleep(50);
        _PSUX->WriteData("DISP:MENU:NAME 3\n",17);
        Sleep(50);
        _PSUY->WriteData("ABOR\n",5);
        Sleep(50);
        _PSUY->WriteData("SYST:PRES\n",10);
        Sleep(50);
        _PSUY->WriteData("DISP:MENU:NAME 3\n",17);
        Sleep(50);
        _PSUZ->WriteData("ABOR\n",5);
        Sleep(50);
        _PSUZ->WriteData("SYST:PRES\n",10);
        Sleep(50);
        _PSUZ->WriteData("DISP:MENU:NAME 3\n",17);
        Sleep(50);
        _PSUX->WriteData("OUTP:STAT OFF\n",14);
        Sleep(50);
        _PSUY->WriteData("OUTP:STAT OFF\n",14);
        Sleep(50);
        _PSUZ->WriteData("OUTP:STAT OFF\n",14);
        Sleep(50);
        delete _PSUX;
        delete _PSUY;
        delete _PSUZ;
        _PSUX = new Serial();
        _PSUY = new Serial();
        _PSUZ = new Serial();
        qDebug() << _PSUX->IsConnected() <<_PSUY->IsConnected()<<_PSUZ->IsConnected();

    }
    _flags[1] = 0;
    setActive("com");
    infoBoxUpdate();
}

// Disables power for safety
void HHCC::disablePower(){ // Needs Testing
    if(!ui->ignoreArd_cbox->isChecked() && _arduino->IsConnected()){
        _arduino->WriteData("000",3);}

    if(!ui->ignorePSU_cbox->isChecked() && _PSUX->IsConnected() && _PSUY->IsConnected() && _PSUZ->IsConnected()){
        _PSUX->WriteData("APPL 0.00,0.00\n",15);
        _PSUY->WriteData("APPL 0.00,0.00\n",15);
        _PSUZ->WriteData("APPL 0.00,0.00\n",15);}
}
// Check if time and offsets have been set
void HHCC::checkSimulationControls(){ // Complete
    if(checkString(ui->delay_sim_ledit->text().toStdString(),"digits") && ui->units_sim_cbox->currentIndex() != -1){
        _delay = atoi(ui->delay_sim_ledit->text().toStdString().c_str());
        _unitsTime = ui->units_sim_cbox->currentText();
        if(checkString(ui->offsetX_sim_ledit->text().toStdString(),"digits") &&
            checkString(ui->offsetY_sim_ledit->text().toStdString(),"digits") &&
            checkString(ui->offsetZ_sim_ledit->text().toStdString(),"digits")){
            _offsetX = atoi(ui->offsetX_sim_ledit->text().toStdString().c_str());
            _offsetY = atoi(ui->offsetY_sim_ledit->text().toStdString().c_str());
            _offsetZ = atoi(ui->offsetZ_sim_ledit->text().toStdString().c_str());
            _flags[2] = 0;}
        else
            _flags[2] = -1;}
    else
        _flags[2] = -1;
    setActive("sim");
    infoBoxUpdate();
}
// Updates sim measurements
void HHCC::updateMeasurements(){ // Complete
    ui->measX_sim_label->setText(QString::number(_measX,'f',2));
    ui->voltX_sim_label->setText(QString::number(_voltX,'f',2));
    ui->currX_sim_label->setText(QString::number(_currX,'f',2));
    ui->measY_sim_label->setText(QString::number(_measY,'f',2));
    ui->voltY_sim_label->setText(QString::number(_voltY,'f',2));
    ui->currY_sim_label->setText(QString::number(_currY,'f',2));
    ui->currZ_sim_label->setText(QString::number(_currZ,'f',2));
    ui->voltZ_sim_label->setText(QString::number(_voltZ,'f',2));
    ui->measZ_sim_label->setText(QString::number(_measZ,'f',2));
    ui->theorX_sim_label->setText(QString::number(_theoX,'f',2));
    ui->theorY_sim_label->setText(QString::number(_theoY,'f',2));
    ui->theorZ_sim_label->setText(QString::number(_theoZ,'f',2));
    ui->simProgress_sim_bar->setValue(_simProgress);
}
// Setups simulation for faster looping
void HHCC::setupSimulation(){ // Needs Testing (missing conversion

    _simPSUData.clear();
    _simArdData.clear();
    QVector<QString> tmpQVec;
    _simPSUData.push_back(tmpQVec);
    _simPSUData.push_back(tmpQVec);
    _simPSUData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);
    _simArdData.push_back(tmpQVec);

    double tmp, tmp2;
    double Xres = 2.1;
    double Yres = 1.9;
    double Zres = 2.3;

    for(int idx = 0; idx < _xData.size(); ++idx){
        tmp = 1.4492/35*613647*(_xData[idx]*.000000001*1+.000000001*ui->offsetX_sim_ledit->text().toDouble()); //X Middle Coil 1.4492 35 Coils
        tmp2 = tmp*Xres;
        tmp = roundf(tmp * 100) / 100;
        tmp2 = roundf(tmp2 * 100) / 100;
        _currData[0].push_back(tmp);
        _voltData[0].push_back(tmp2);
        //_simPSUData[0].push_back("APPL "+to_QString(tmp2)+","+to_QString(tmp)+"\n");
        _simPSUData[0].push_back("APPL "+QString::number(20)+","+QString::number(tmp,'f',2)+"\n");
        _simArdData[0].push_back(isPos(tmp));
        tmp = 1.3984/35*613647*(_yData[idx]*.000000001*1+.000000001*ui->offsetY_sim_ledit->text().toDouble()); //Y Small Coil 1.3984 35 Coils
        tmp2 = tmp*Yres;
        tmp = roundf(tmp * 100) / 100;
        tmp2 = roundf(tmp2 * 100) / 100;
        _currData[1].push_back(tmp);
        _voltData[1].push_back(tmp2);
        //_simPSUData[1].push_back("APPL "+to_QString(tmp2)+","+to_QString(tmp)+"\n");
        _simPSUData[1].push_back("APPL "+QString::number(20.00,'f',2)+","+QString::number(tmp,'f',2)+"\n");
        _simArdData[1].push_back(isPos(tmp));
        tmp =    1.5/35*613647*(_zData[idx]*.000000001*1+.000000001*ui->offsetZ_sim_ledit->text().toDouble()); //Z Large Coil 1.5 35 Coils
        tmp2 = tmp*Zres;
        tmp = roundf(tmp * 100) / 100;
        tmp2 = roundf(tmp2 * 100) / 100;
        qDebug() << tmp;
        _currData[2].push_back(tmp);
        _voltData[2].push_back(tmp2);
        //_simPSUData[2].push_back("APPL "+to_QString(tmp2)+","+to_QString(tmp)+"\n");
        _simPSUData[2].push_back("APPL "+QString::number(20.00,'f',2)+","+QString::number(tmp,'f',2)+"\n");
        _simArdData[2].push_back(isPos(tmp));}
}
// Runs simulation
void HHCC::runSimulation(){ // Needs Testing
    endSim = false;
    checkSimulationControls();
    setupSimulation();
    _flags[2] = 1;
    time_t tic,toc;
    double tmpTime;
    setActive("sim");
    infoBoxUpdate();
    QString newData;
    for(int idx = 0;idx < _xData.size(); ++idx){
        tic = std::clock();
        if(!ui->ignorePSU_cbox->isChecked()){
            Sleep(50);
            _PSUX->WriteData((_simPSUData[0])[idx].toStdString().c_str(),16);
            Sleep(50);
            _PSUY->WriteData((_simPSUData[1])[idx].toStdString().c_str(),16);
            Sleep(50);
            _PSUZ->WriteData((_simPSUData[2])[idx].toStdString().c_str(),16);
            Sleep(50);
            }
        if(!ui->ignoreArd_cbox->isChecked())
            newData = arduino(QString::fromStdString(((_simArdData[0])[idx].toStdString()+(_simArdData[1])[idx].toStdString()+(_simArdData[2])[idx].toStdString())));
        qDebug() << newData;
        _theoX = 0;
        _theoY = 0;
        _theoZ = 0;

        _measData[0].push_back(_xData[idx]);
        _measData[1].push_back(_yData[idx]);
        _measData[2].push_back(_zData[idx]);

        _measX = (_measData[0])[idx];
        _measY = (_measData[1])[idx];
        _measZ = (_measData[2])[idx];

        _currX = (_currData[0])[idx];
        _currY = (_currData[1])[idx];
        _currZ = (_currData[2])[idx];

        _voltX = (_voltData[0])[idx];
        _voltY = (_voltData[1])[idx];
        _voltZ = (_voltData[2])[idx];

        _simProgress = (int)(idx/_xData.size());
        emit update();
        QApplication::processEvents();
        if(endSim)
            break;
        toc = std::clock();
        tmpTime = (_delayTime >= 500) ? _delayTime: 500;
        tmpTime = ((toc-tic) <= tmpTime) ? (tmpTime-(toc-tic)) : 0;
        while(tmpTime > 250){
            Sleep(250);
            tmpTime -= 250;
            QApplication::processEvents();
            if(endSim)
                break;}
        Sleep(tmpTime);
        QApplication::processEvents();
        if(endSim)
           break;
    }
    if(!endSim)
        endSimulation();
}




// Ends the simulation safely
void HHCC::endSimulation(){ // Needs Testing
    disablePower();
    if(ui->store_cbox->isChecked())
        storeData();
    _theoX = 0;
    _theoY = 0;
    _theoZ = 0;
    _measX = 0;
    _measY = 0;
    _measZ = 0;
    _voltX = 0;
    _voltY = 0;
    _voltZ = 0;
    _currX = 0;
    _currY = 0;
    _currZ = 0;
    _simProgress = 0;
    updateMeasurements();
    _measData[0].clear();
    _measData[1].clear();
    _measData[2].clear();
    _voltData[0].clear();
    _voltData[1].clear();
    _voltData[2].clear();
    _currData[0].clear();
    _currData[1].clear();
    _currData[2].clear();
    _flags[2] = 0;
    setActive("data");
    setActive("com");
    setActive("sim");
    infoBoxUpdate();
    endSim = true;
}
// Stores data into a .csv for use
void HHCC::storeData(){ // Completed
    std::ofstream myfile;
    myfile.open ("simulation_data.csv");
    myfile << "Theo. B Field X,Theo. B Field Y,Theo. B Field Z,,Meas. B Field X,Meas. B Field Y,Meas. B Field Z,,Volt X,Volt Y,Volt Z,,Current X,Current Y,Current Z\n";
    for(int idx = 0; idx < _measData[0].size(); ++idx){
       myfile << to_QString(_xData[idx]).toStdString() +","+ to_QString(_yData[idx]).toStdString() +","+ to_QString(_zData[idx]).toStdString() +","+ " " +","+
       to_QString((_measData[0])[idx]).toStdString() +","+ to_QString((_measData[1])[idx]).toStdString() +","+ to_QString((_measData[2])[idx]).toStdString() +","+ " " +","+
       to_QString((_voltData[0])[idx]).toStdString() +","+ to_QString((_voltData[1])[idx]).toStdString() +","+ to_QString((_voltData[2])[idx]).toStdString() +","+ " " +","+
       to_QString((_currData[0])[idx]).toStdString() +","+ to_QString((_currData[1])[idx]).toStdString() +","+ to_QString((_currData[2])[idx]).toStdString() +"\n";}
    myfile.close();
}

// Check if string is a digit, alpha or neither
bool HHCC::checkString(const std::string& str, const std::string& type){ // Complete
    if(type == "digits"){
      std::istringstream os(str);
      double tmp;
      os >> tmp;
      return true;}
    else if(type == "alpha" && std::count_if(str.begin(), str.end(), isalnum) == str.size())
      return true;
    return false;
}
// Converts from an int to a QString
QString HHCC::to_QString(const int& intstr){ // Complete
   return QString::fromStdString(std::to_string(intstr));
}
// Converts from an double to a QString
QString HHCC::to_QString(const double& dubstr){ // Complete
   return QString::fromStdString(std::to_string(dubstr));
}
// Converts units to give simulation time
void HHCC::convertUnits(){ // Complete
    int tmp = _delay*_numData;
    if(_unitsTime == "sec"){
        _simTime = to_QString(tmp);
    }
    else if(_unitsTime == "min"){
        _simTime = to_QString(tmp*60);
    }
    else{
        _simTime = to_QString(tmp/1000);
    }
}
// Determines if double is positive
QString HHCC::isPos(double num){ // Complete
    return (num >= 0) ? "1" : "0";
}

#endif
