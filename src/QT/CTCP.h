#ifndef CTCP_H
#define CTCP_H

#include <QString>
#include <winsock.h>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkInterface>
#include "comm_protocol.h"

class Optical_Detection_UI;
class CTCP
{
public:
    CTCP(Optical_Detection_UI* object);
    ~CTCP();

public:
    // members
    Optical_Detection_UI*  m_object;   // UI Object
    QTcpSocket*  m_ClientSocket = nullptr;

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
    int ConnectToTCP(QString ip, quint16 port);
    int CloseClientSocket();
    int ConnectedToTCP(QString wfname);
    int disConnectedToTCP();
    int RecvDataFromTCP();
    int SendCmdToTCP(quint16 Cmd);
    int tcpError(QAbstractSocket::SocketError error);
};

#endif // CTCP_H
