#ifndef OPTICAL_DETECTION_UI_H
#define OPTICAL_DETECTION_UI_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QtGui>
#include <QMessageBox>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkInterface>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QValidator>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "CTCP.h"
#include "CUART.h"
#include "cgraph.h"
#include "cverification.h"
#include "cnetwork.h"
#include "csetting.h"

enum
{
    TIMER_STATE_VERSION,
    TIMER_STATE_DOWNLINK_WAVE,
    TIMER_STATE_DOWNLINK_OFFSET_A,
    TIMER_STATE_DOWNLINK_OFFSET_B,
    TIMER_STATE_DOWNLINK_ALARM,
    TIMER_STATE_UPLINK_WAVE,
    TIMER_STATE_UPLINK_OFFSET_A,
    TIMER_STATE_UPLINK_OFFSET_B,
    TIMER_STATE_UPLINK_ALARM,
    TIMER_STATE_STATUS
};

#define TIMER_CLOCK_COMM 1000
#define TIMER_CLOCK_START 100

QT_BEGIN_NAMESPACE
namespace Ui { class Optical_Detection_UI; }
QT_END_NAMESPACE

class Optical_Detection_UI : public QMainWindow
{
    Q_OBJECT

public:
    Optical_Detection_UI(QWidget *parent = nullptr);
    ~Optical_Detection_UI();

private slots:
    // ParentTap

    // Main
    void on_checkBox_Main_Use_UART_clicked();
    void on_pushButton_Main_ConnectUART_clicked();
    void on_pushButton_Main_DisconnectUART_clicked();
    void on_checkBox_Main_Use_TCP_clicked();
    void on_pushButton_Main_ConnectTCP_clicked();
    void on_pushButton_Main_DisconnectTCP_clicked();
    void on_pushButton_Main_Network_clicked();
    void on_pushButton_Main_Setting_clicked();
    void on_pushButton_Main_WaveGraph_clicked();
    void on_groupBox_Main_DownLink_Channel_0_clicked();
    void on_groupBox_Main_DownLink_Channel_1_clicked();
    void on_groupBox_Main_DownLink_Channel_2_clicked();
    void on_groupBox_Main_DownLink_Channel_3_clicked();
    void on_groupBox_Main_DownLink_Channel_4_clicked();
    void on_groupBox_Main_DownLink_Channel_5_clicked();
    void on_groupBox_Main_DownLink_Channel_6_clicked();
    void on_groupBox_Main_DownLink_Channel_7_clicked();
    void on_groupBox_Main_DownLink_Channel_8_clicked();
    void on_groupBox_Main_DownLink_Channel_9_clicked();
    void on_groupBox_Main_DownLink_Channel_10_clicked();
    void on_groupBox_Main_DownLink_Channel_11_clicked();
    void on_groupBox_Main_DownLink_Channel_12_clicked();
    void on_groupBox_Main_DownLink_Channel_13_clicked();
    void on_groupBox_Main_DownLink_Channel_14_clicked();
    void on_groupBox_Main_DownLink_Channel_15_clicked();
    void on_groupBox_Main_DownLink_Channel_16_clicked();
    void on_groupBox_Main_DownLink_Channel_17_clicked();
    void on_groupBox_Main_DownLink_Channel_18_clicked();
    void on_groupBox_Main_DownLink_Channel_19_clicked();
    void on_groupBox_Main_DownLink_Channel_20_clicked();
    void on_groupBox_Main_DownLink_Channel_21_clicked();
    void on_groupBox_Main_DownLink_Channel_22_clicked();
    void on_groupBox_Main_DownLink_Channel_23_clicked();
    void on_groupBox_Main_DownLink_Channel_24_clicked();
    void on_groupBox_Main_DownLink_Channel_25_clicked();
    void on_groupBox_Main_DownLink_Channel_26_clicked();
    void on_groupBox_Main_DownLink_Channel_27_clicked();
    void on_groupBox_Main_DownLink_Channel_28_clicked();
    void on_groupBox_Main_DownLink_Channel_29_clicked();
    void on_groupBox_Main_DownLink_Channel_30_clicked();
    void on_groupBox_Main_DownLink_Channel_31_clicked();
    void on_groupBox_Main_DownLink_Channel_32_clicked();
    void on_groupBox_Main_DownLink_Channel_33_clicked();
    void on_groupBox_Main_DownLink_Channel_34_clicked();
    void on_groupBox_Main_DownLink_Channel_35_clicked();
    void on_groupBox_Main_DownLink_Channel_36_clicked();
    void on_groupBox_Main_DownLink_Channel_37_clicked();
    void on_groupBox_Main_DownLink_Channel_38_clicked();
    void on_groupBox_Main_DownLink_Channel_39_clicked();
    void on_groupBox_Main_DownLink_Channel_40_clicked();
    void on_groupBox_Main_DownLink_Channel_41_clicked();
    void on_groupBox_Main_DownLink_Channel_42_clicked();
    void on_groupBox_Main_DownLink_Channel_43_clicked();
    void on_groupBox_Main_DownLink_Channel_44_clicked();
    void on_groupBox_Main_DownLink_Channel_45_clicked();
    void on_groupBox_Main_DownLink_Channel_46_clicked();
    void on_groupBox_Main_DownLink_Channel_47_clicked();
    void on_groupBox_Main_DownLink_Channel_48_clicked();
    void on_groupBox_Main_DownLink_Channel_49_clicked();
    void on_groupBox_Main_DownLink_Channel_50_clicked();
    void on_groupBox_Main_DownLink_Channel_51_clicked();
    void on_groupBox_Main_DownLink_Channel_52_clicked();
    void on_groupBox_Main_DownLink_Channel_53_clicked();
    void on_groupBox_Main_DownLink_Channel_54_clicked();
    void on_groupBox_Main_DownLink_Channel_55_clicked();
    void on_groupBox_Main_DownLink_Channel_56_clicked();
    void on_groupBox_Main_DownLink_Channel_57_clicked();
    void on_groupBox_Main_DownLink_Channel_58_clicked();
    void on_groupBox_Main_DownLink_Channel_59_clicked();
    void on_groupBox_Main_DownLink_Channel_60_clicked();
    void on_groupBox_Main_DownLink_Channel_61_clicked();
    void on_groupBox_Main_DownLink_Channel_62_clicked();
    void on_groupBox_Main_DownLink_Channel_63_clicked();
    void on_groupBox_Main_UpLink_Channel_0_clicked();
    void on_groupBox_Main_UpLink_Channel_1_clicked();
    void on_groupBox_Main_UpLink_Channel_2_clicked();
    void on_groupBox_Main_UpLink_Channel_3_clicked();
    void on_groupBox_Main_UpLink_Channel_4_clicked();
    void on_groupBox_Main_UpLink_Channel_5_clicked();
    void on_groupBox_Main_UpLink_Channel_6_clicked();
    void on_groupBox_Main_UpLink_Channel_7_clicked();
    void on_groupBox_Main_UpLink_Channel_8_clicked();
    void on_groupBox_Main_UpLink_Channel_9_clicked();
    void on_groupBox_Main_UpLink_Channel_10_clicked();
    void on_groupBox_Main_UpLink_Channel_11_clicked();
    void on_groupBox_Main_UpLink_Channel_12_clicked();
    void on_groupBox_Main_UpLink_Channel_13_clicked();
    void on_groupBox_Main_UpLink_Channel_14_clicked();
    void on_groupBox_Main_UpLink_Channel_15_clicked();
    void on_groupBox_Main_UpLink_Channel_16_clicked();
    void on_groupBox_Main_UpLink_Channel_17_clicked();
    void on_groupBox_Main_UpLink_Channel_18_clicked();
    void on_groupBox_Main_UpLink_Channel_19_clicked();
    void on_groupBox_Main_UpLink_Channel_20_clicked();
    void on_groupBox_Main_UpLink_Channel_21_clicked();
    void on_groupBox_Main_UpLink_Channel_22_clicked();
    void on_groupBox_Main_UpLink_Channel_23_clicked();
    void on_groupBox_Main_UpLink_Channel_24_clicked();
    void on_groupBox_Main_UpLink_Channel_25_clicked();
    void on_groupBox_Main_UpLink_Channel_26_clicked();
    void on_groupBox_Main_UpLink_Channel_27_clicked();
    void on_groupBox_Main_UpLink_Channel_28_clicked();
    void on_groupBox_Main_UpLink_Channel_29_clicked();
    void on_groupBox_Main_UpLink_Channel_30_clicked();
    void on_groupBox_Main_UpLink_Channel_31_clicked();
    void on_groupBox_Main_UpLink_Channel_32_clicked();
    void on_groupBox_Main_UpLink_Channel_33_clicked();
    void on_groupBox_Main_UpLink_Channel_34_clicked();
    void on_groupBox_Main_UpLink_Channel_35_clicked();
    void on_groupBox_Main_UpLink_Channel_36_clicked();
    void on_groupBox_Main_UpLink_Channel_37_clicked();
    void on_groupBox_Main_UpLink_Channel_38_clicked();
    void on_groupBox_Main_UpLink_Channel_39_clicked();
    void on_groupBox_Main_UpLink_Channel_40_clicked();
    void on_groupBox_Main_UpLink_Channel_41_clicked();
    void on_groupBox_Main_UpLink_Channel_42_clicked();
    void on_groupBox_Main_UpLink_Channel_43_clicked();
    void on_groupBox_Main_UpLink_Channel_44_clicked();
    void on_groupBox_Main_UpLink_Channel_45_clicked();
    void on_groupBox_Main_UpLink_Channel_46_clicked();
    void on_groupBox_Main_UpLink_Channel_47_clicked();
    void on_groupBox_Main_UpLink_Channel_48_clicked();
    void on_groupBox_Main_UpLink_Channel_49_clicked();
    void on_groupBox_Main_UpLink_Channel_50_clicked();
    void on_groupBox_Main_UpLink_Channel_51_clicked();
    void on_groupBox_Main_UpLink_Channel_52_clicked();
    void on_groupBox_Main_UpLink_Channel_53_clicked();
    void on_groupBox_Main_UpLink_Channel_54_clicked();
    void on_groupBox_Main_UpLink_Channel_55_clicked();
    void on_groupBox_Main_UpLink_Channel_56_clicked();
    void on_groupBox_Main_UpLink_Channel_57_clicked();
    void on_groupBox_Main_UpLink_Channel_58_clicked();
    void on_groupBox_Main_UpLink_Channel_59_clicked();
    void on_groupBox_Main_UpLink_Channel_60_clicked();
    void on_groupBox_Main_UpLink_Channel_61_clicked();
    void on_groupBox_Main_UpLink_Channel_62_clicked();
    void on_groupBox_Main_UpLink_Channel_63_clicked();
    void on_pushButton_Main_OpticalSW_Control_DownLink_clicked();
    void on_pushButton_Main_OpticalSW_Control_UpLink_clicked();
    void on_pushButton_Main_SetAttenuation_clicked();
    void on_pushButton_Main_ResetAttenuation_clicked();
    void on_pushButton_Main_Save_clicked();

    // Table
    void on_pushButton_Table_DownLink_LoadWave_clicked();
    void on_pushButton_Table_DownLink_LoadOffset_A_clicked();
    void on_pushButton_Table_DownLink_LoadOffset_B_clicked();
    void on_pushButton_Table_DownLink_LoadAlarm_clicked();
    void on_pushButton_Table_DownLink_UploadWave_clicked();
    void on_pushButton_Table_DownLink_UploadOffset_A_clicked();
    void on_pushButton_Table_DownLink_UploadOffset_B_clicked();
    void on_pushButton_Table_DownLink_UploadAlarm_clicked();
    void on_pushButton_Table_DownLink_Set_AlarmLimitValue_clicked();
    void on_pushButton_Table_DownLink_Set_OffsetBoundaryValue_clicked();
    void on_tableWidget_Table_DownLink_cellChanged(int row, int column);
    void on_pushButton_Table_UpLink_LoadWave_clicked();
    void on_pushButton_Table_UpLink_LoadOffset_A_clicked();
    void on_pushButton_Table_UpLink_LoadOffset_B_clicked();
    void on_pushButton_Table_UpLink_LoadAlarm_clicked();
    void on_pushButton_Table_UpLink_UploadWave_clicked();
    void on_pushButton_Table_UpLink_UploadOffset_A_clicked();
    void on_pushButton_Table_UpLink_UploadOffset_B_clicked(); 
    void on_pushButton_Table_UpLink_UploadAlarm_clicked();
    void on_pushButton_Table_UpLink_Set_AlarmLimitValue_clicked();
    void on_pushButton_Table_UpLink_Set_OffsetBoundaryValue_clicked();
    void on_tableWidget_Table_UpLink_cellChanged(int row, int column);
    void on_pushButton_Table_SetTableLength_clicked();

    // Data
    void on_pushButton_Data_LoadData_clicked();

public slots:
    // TCP
    void RecvDataFromTCP();
    void ConnectedToTCP();
    void disConnectedToTCP();
    void tcpError(QAbstractSocket::SocketError error);

    // UART
    void RecvDataFromUART();
    void uartError(QSerialPort::SerialPortError error);

    // Timer
    void OnTimerSendCommCmd_UART();
    void OnTimerSendCommCmd_TCP();
    void OnTimerSendCommCmd2_TCP();

    // Ping slot
    void OnPingResult();

    // Table cell CheckBox
    void DownLinkTableCellCheckBox_stateChanged(int);
    void UpLinkTableCellCheckBox_stateChanged(int);

private:
    Ui::Optical_Detection_UI *ui;

    quint16 m_HDPI = 1200;
    quint16 m_VDPI = 800;

private:    // override methods
    bool eventFilter(QObject* dest, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
public:
    // members  
    // Widgets
    QGroupBox* m_arrGroupBox_DownLink_Channel[ADC_DT_MAXLEN];
    QGroupBox* m_arrGroupBox_UpLink_Channel[ADC_DT_MAXLEN];
    QLCDNumber* m_arrLCDNumber_DownLink_WaveLength[ADC_DT_MAXLEN];
    QLCDNumber* m_arrLCDNumber_UpLink_WaveLength[ADC_DT_MAXLEN];
    QLCDNumber* m_arrLCDNumber_DownLink_OptPower[ADC_DT_MAXLEN];
    QLCDNumber* m_arrLCDNumber_UpLink_OptPower[ADC_DT_MAXLEN];

    // Network
    CUART* m_ptrUART;
    CTCP* m_ptrTCP;
    QProcess *m_Proc_Ping;
    quint8 m_Comm_Type = 0;     // 0 : UART, 1 : TCP

    // Graph
    CGraph* m_ptrGraph;

    // Verification
    CVerification* m_ptrVerification;

    // Network
    CNetwork* m_ptrNetwork;

    // Setting
    CSetting* m_ptrSetting;
    QString m_strPath_SettingFile = "Optical_Detection_UI";

    // Timer
    quint8 m_Timer_State;
    quint16 m_Timer_Cmd;
    quint32 m_TimerClock_Comm;      // The Cycle time of to request data from the server (ms)
    quint32 m_TimerDelay_Comm = 10;
    std::shared_ptr<QTimer> m_pTimer_SendCommCmd_UART;
    std::shared_ptr<QTimer> m_pTimer_SendCommCmd_TCP;
    std::shared_ptr<QTimer> m_pTimer_SendCommCmd2_TCP;

    // Main
    quint8 m_groupBox_CurrSelChannel = 0;
    quint8 m_arrToggleLED_Main[2] = { 0, }; // 0 : TXD, 1 : RXD
    QString m_arrStyleSheet_ToggleLED[3] =
    {
        "border-radius: 15px;min-height: 30px;min-width: 30px;background-color: rgb(127,127,127);",
        "border-radius: 15px;min-height: 30px;min-width: 30px;background-color: rgb(0,255,0);",
        "border-radius: 15px;min-height: 30px;min-width: 30px;background-color: rgb(255,0,0);"
    };  
    quint8 m_SetChannellen = 0;
    quint8 m_beforeChannellen = ADC_DT_MAXLEN; // 채널 길이를 바꾸기 직전 값
    quint8 m_beforeChannel = 0; // 채널을 바꾸기 직전 값
    quint8 m_beforeOptSW = 0;   // OptSW 를 바꾸기 직전 값
    qint32 m_Boundary_Offset = 0;   // Offset 적용 경계 값

    // Table
    quint8 m_Table_Wavelength_SendCount = 0;
    quint8 m_Table_Offset_A_SendCount = 0;
    quint8 m_Table_Offset_B_SendCount = 0;
    quint8 m_Table_Alarm_SendCount = 0;
    quint8 m_Table_Wavelength_RecvCount = 0;
    quint8 m_Table_Offset_A_RecvCount = 0;
    quint8 m_Table_Offset_B_RecvCount = 0;
    quint8 m_Table_Alarm_RecvCount = 0;
    QTableWidgetItem m_Table_DownLink_Wavelength[ADC_DT_MAXLEN];
    QTableWidgetItem m_Table_DownLink_Offset_A[ADC_DT_MAXLEN];
    QTableWidgetItem m_Table_DownLink_Offset_B[ADC_DT_MAXLEN];
    quint8 m_Table_DownLink_Alarm_Enable[ADC_DT_MAXLEN];
    QTableWidgetItem m_Table_UpLink_Wavelength[ADC_DT_MAXLEN];
    QTableWidgetItem m_Table_UpLink_Offset_A[ADC_DT_MAXLEN];
    QTableWidgetItem m_Table_UpLink_Offset_B[ADC_DT_MAXLEN];
    quint8 m_Table_UpLink_Alarm_Enable[ADC_DT_MAXLEN];
    QModelIndexList m_Table_copyIndexList;


    // Data
    quint8 m_Data_RecvCount = 0;
    QTableWidgetItem m_Data_Wavelength[MAX_DISPLAYBUF_INDEX_EEP + 1];
    QTableWidgetItem m_Data_OptPower[MAX_DISPLAYBUF_INDEX_EEP + 1];
    QTableWidgetItem m_Data_Attenuation[MAX_DISPLAYBUF_INDEX_EEP + 1];
    QTableWidgetItem m_Data_OptSw[MAX_DISPLAYBUF_INDEX_EEP + 1];

    // methods
    void Init();
    void InitWidget();
    void InitTable();
    void UpdateUIStatus_UART();
    void UpdateSetChannellen_UART();
    void UpdateSetChannel_UART();
    void UpdateOptSW_UART();
    void UpdateLoad_DownLink_Wavelength_UART();
    void UpdateLoad_DownLink_Offset_A_UART();
    void UpdateLoad_DownLink_Offset_B_UART();
    void UpdateLoad_DownLink_Alarm_UART();
    void UpdateLoad_UpLink_Wavelength_UART();
    void UpdateLoad_UpLink_Offset_A_UART();
    void UpdateLoad_UpLink_Offset_B_UART();
    void UpdateLoad_UpLink_Alarm_UART();
    void UpdateLoadData_UART();
    void UpdateUIStatus_TCP();
    void UpdateSetChannellen_TCP();
    void UpdateSetChannel_TCP();
    void UpdateOptSW_TCP();
    void UpdateLoad_DownLink_Wavelength_TCP();
    void UpdateLoad_DownLink_Offset_A_TCP();
    void UpdateLoad_DownLink_Offset_B_TCP();
    void UpdateLoad_DownLink_Alarm_TCP();
    void UpdateLoad_UpLink_Wavelength_TCP();
    void UpdateLoad_UpLink_Offset_A_TCP();
    void UpdateLoad_UpLink_Offset_B_TCP();
    void UpdateLoad_UpLink_Alarm_TCP();
    void UpdateLoadData_TCP();
    void setStyleSheet_ToggleLED(quint8 index); // 0 : TXD, 1 : RXD

    // Uart
    void InitUART();

    // Tcp
    void InitTCP();

    // Graph
    void InitPtrCGraph();

    // Verification
    void InitPtrCVerification();

    // Network
    void InitPtrCNetwork();
    void LoadNetworkInfo();
    void SaveNetworkInfo();

    // Setting
    void InitPtrCSetting();
    void InitSetting();
    void CreateSettingDialog();
    void SaveSettingInfo();
    void LoadSettingInfo();

};
#endif // OPTICAL_DETECTION_UI_H
