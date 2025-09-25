#ifndef CUART_H
#define CUART_H

#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "comm_protocol.h"

class Optical_Detection_UI;
class CUART
{
public:
    CUART(Optical_Detection_UI* object);
    ~CUART();

public:
    // members
    Optical_Detection_UI*  m_object;   // UI Object
    QSerialPort*  m_SerialPort = nullptr;

    STC_VERSIONINFO_BUF m_Recv_stVERSIONINFO;
    STC_VERSIONINFO_BUF m_Send_stVERSIONINFO;

    STC_UI_STATUS_BUF m_Recv_stUI_STATUS;
    STC_UI_STATUS_BUF m_Send_stUI_STATUS;

    STC_WAVE_BUF m_Recv_DownLink_stWAVE[ADC_DT_MAXLEN];
    STC_WAVE_BUF m_Recv_DownLink_stWAVE_Tempbuf;
    STC_WAVE_BUF m_Send_DownLink_stWAVE;

    STC_OFFSET_BUF m_Recv_DownLink_stOFFSET_A[ADC_DT_MAXLEN];
    STC_OFFSET_BUF m_Recv_DownLink_stOFFSET_A_Tempbuf;
    STC_OFFSET_BUF m_Send_DownLink_stOFFSET_A;

    STC_OFFSET_BUF m_Recv_DownLink_stOFFSET_B[ADC_DT_MAXLEN];
    STC_OFFSET_BUF m_Recv_DownLink_stOFFSET_B_Tempbuf;
    STC_OFFSET_BUF m_Send_DownLink_stOFFSET_B;

    STC_ALARM_BUF m_Recv_DownLink_stALARM[ADC_DT_MAXLEN];
    STC_ALARM_BUF m_Recv_DownLink_stALARM_Tempbuf;
    STC_ALARM_BUF m_Send_DownLink_stALARM;

    STC_WAVE_BUF m_Recv_UpLink_stWAVE[ADC_DT_MAXLEN];
    STC_WAVE_BUF m_Recv_UpLink_stWAVE_Tempbuf;
    STC_WAVE_BUF m_Send_UpLink_stWAVE;

    STC_OFFSET_BUF m_Recv_UpLink_stOFFSET_A[ADC_DT_MAXLEN];
    STC_OFFSET_BUF m_Recv_UpLink_stOFFSET_A_Tempbuf;
    STC_OFFSET_BUF m_Send_UpLink_stOFFSET_A;

    STC_OFFSET_BUF m_Recv_UpLink_stOFFSET_B[ADC_DT_MAXLEN];
    STC_OFFSET_BUF m_Recv_UpLink_stOFFSET_B_Tempbuf;
    STC_OFFSET_BUF m_Send_UpLink_stOFFSET_B;

    STC_ALARM_BUF m_Recv_UpLink_stALARM[ADC_DT_MAXLEN];
    STC_ALARM_BUF m_Recv_UpLink_stALARM_Tempbuf;
    STC_ALARM_BUF m_Send_UpLink_stALARM;

    STC_DATA_BUF m_Recv_stDATA[MAX_DISPLAYBUF_INDEX_EEP + 1];
    STC_DATA_BUF m_Recv_stDATA_Tempbuf;
    STC_DATA_BUF m_Send_stDATA;

    STC_NETWORK_BUF m_Recv_stNETWORK;
    STC_NETWORK_BUF m_Send_stNETWORK;

    // methods
    QList<QSerialPortInfo> GetPortsInfo();
    int StartSerialPort(QString name, quint32 baudrate);
    int StopSerialPort();
    int RecvDataFromUART();
    int SendCmdToUART(quint16 Cmd);
    int uartError(QSerialPort::SerialPortError error);
};

#endif // CUART_H
