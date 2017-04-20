#ifndef HHCC_H
#define HHCC_H

#include <QMainWindow>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <QFileDialog>
#include <QPointer>
#include <locale>
#include "serial.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Ui {
class HHCC;
}

class HHCC : public QMainWindow
{
    Q_OBJECT

public:
    explicit HHCC(QWidget *parent = 0);
    ~HHCC();
    // Sets active-ness of components
    void setActive(const QString& section);
    // Check if string is a digit, alpha or neither
    bool checkString(const std::string& str, const std::string& type);
    // Updates all the info for the infobox
    void infoBoxUpdate();
    QString to_QString(const int& intstr);
    QString to_QString(const double& dubstr);
    void convertUnits();
    void setupSimulation();
    QString isPos(double num);
    QString arduino(QString data);
public slots:

    // Browses for the file
    void browseFile();
    // Connect to Ports
    void connectPorts();
    void resetAll();
    void runSimulation();
    void endSimulation();
    void updateMeasurements();


    // Loads simple testing data
    void loadTestingData();
    // Extracts data for use in the sim
    void extractData();
    // Check if ports have been set
    void checkPortLabels();
    // Disconnect to Ports
    void disconnectPorts();
    // Check if time and offsets have been set
    void checkSimulationControls();
    void disablePower();
    void storeData();

signals:
    void update();
    // Window Specific Variables & Functions
private:
    void signalSetup();

    int    _flags[3];

    QString _unitsTmp;
    QString _coorSystemTmp;
    QString _filenameTmp;
    int     _numDataTmp;
    QVector<QVector<double>> _rawData;

    QString _units;
    QString _coorSystem;
    QString _filename;
    int     _numData;
    QVector<double> _xData;
    QVector<double> _yData;
    QVector<double> _zData;

    Serial* _arduino;
    Serial* _PSUX;
    Serial* _PSUY;
    Serial* _PSUZ;

    double _offsetX;
    double _offsetY;
    double _offsetZ;
    int    _delay;
    QString _unitsTime;
    double  _delayTime;
    QString _simTime;

    double _theoX;
    double _theoY;
    double _theoZ;
    double _measX;
    double _measY;
    double _measZ;
    double _voltX;
    double _voltY;
    double _voltZ;
    double _currX;
    double _currY;
    double _currZ;
    int _simProgress;
    bool endSim;
    QVector<QVector<double>> _measData;
    QVector<QVector<double>> _voltData;
    QVector<QVector<double>> _currData;

    QVector<QVector<QString>> _simPSUData;
    QVector<QVector<QString>> _simArdData;
    Ui::HHCC *ui;


};

#endif // HHCC_H
