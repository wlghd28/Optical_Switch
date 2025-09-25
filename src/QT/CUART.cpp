#include "CUART.h"
#include "optical_detection_ui.h"

CUART::CUART(Optical_Detection_UI* object)
{
    m_object = object;

    bool Ret = true;
    m_SerialPort = new QSerialPort(m_object);
    if(m_SerialPort)
    {
        Ret = m_object->connect(m_SerialPort, SIGNAL(readyRead()), m_object, SLOT(RecvDataFromUART()));
        Ret = m_object->connect(m_SerialPort, SIGNAL(errorOccurred()), m_object, SLOT(uartError(QSerialPort::SerialPortError)));
        if(!Ret) qDebug() << "fail to connect uart signal";
    }

    memset(&m_Recv_stVERSIONINFO, 0, sizeof(m_Recv_stVERSIONINFO));
    memset(&m_Send_stVERSIONINFO, 0, sizeof(m_Send_stVERSIONINFO));
    memset(&m_Recv_stUI_STATUS, 0, sizeof(m_Recv_stUI_STATUS));
    memset(&m_Send_stUI_STATUS, 0, sizeof(m_Recv_stUI_STATUS));
    memset(&m_Recv_DownLink_stWAVE_Tempbuf, 0, sizeof(m_Recv_DownLink_stWAVE_Tempbuf));
    memset(&m_Send_DownLink_stWAVE, 0, sizeof(m_Send_DownLink_stWAVE));
    memset(&m_Recv_DownLink_stOFFSET_A_Tempbuf, 0, sizeof(m_Recv_DownLink_stOFFSET_A_Tempbuf));
    memset(&m_Send_DownLink_stOFFSET_A, 0, sizeof(m_Send_DownLink_stOFFSET_A));
    memset(&m_Recv_DownLink_stOFFSET_B_Tempbuf, 0, sizeof(m_Recv_DownLink_stOFFSET_B_Tempbuf));
    memset(&m_Send_DownLink_stOFFSET_B, 0, sizeof(m_Send_DownLink_stOFFSET_B));
    memset(&m_Recv_DownLink_stALARM_Tempbuf, 0, sizeof(m_Recv_DownLink_stALARM_Tempbuf));
    memset(&m_Send_DownLink_stALARM, 0, sizeof(m_Send_DownLink_stALARM));
    memset(&m_Recv_UpLink_stWAVE_Tempbuf, 0, sizeof(m_Recv_UpLink_stWAVE_Tempbuf));
    memset(&m_Send_UpLink_stWAVE, 0, sizeof(m_Send_UpLink_stWAVE));
    memset(&m_Recv_UpLink_stOFFSET_A_Tempbuf, 0, sizeof(m_Recv_UpLink_stOFFSET_A_Tempbuf));
    memset(&m_Send_UpLink_stOFFSET_A, 0, sizeof(m_Send_UpLink_stOFFSET_A));
    memset(&m_Recv_UpLink_stOFFSET_B_Tempbuf, 0, sizeof(m_Recv_UpLink_stOFFSET_B_Tempbuf));
    memset(&m_Send_UpLink_stOFFSET_B, 0, sizeof(m_Send_UpLink_stOFFSET_B));
    memset(&m_Recv_UpLink_stALARM_Tempbuf, 0, sizeof(m_Recv_UpLink_stALARM_Tempbuf));
    memset(&m_Send_UpLink_stALARM, 0, sizeof(m_Send_UpLink_stALARM));
    memset(&m_Recv_stDATA_Tempbuf, 0, sizeof(m_Recv_stDATA_Tempbuf));
    memset(&m_Send_stDATA, 0, sizeof(m_Send_stDATA));
}

CUART::~CUART()
{
    if(m_SerialPort != NULL)
    {
        m_SerialPort->close();
        delete m_SerialPort;
        m_SerialPort = nullptr;
    }
}

/*
    Methods
*/
QList<QSerialPortInfo> CUART::GetPortsInfo()
{
    const auto infos = QSerialPortInfo::availablePorts();
    return infos;
}

int CUART::StartSerialPort(QString name, quint32 baudrate)
{
    m_SerialPort->setPortName(name);
    m_SerialPort->setBaudRate(baudrate);
    m_SerialPort->setDataBits(QSerialPort::Data8);
    m_SerialPort->setParity(QSerialPort::NoParity);
    m_SerialPort->setStopBits(QSerialPort::OneStop);
    m_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(m_SerialPort->open(QIODevice::ReadWrite))
    {
        return 0;
    }
    else
    {
        qDebug() << "fail to set uart serialport";
        return -1;
    }

    return -1;
}

int CUART::StopSerialPort()
{
    if(m_SerialPort->isOpen())
        m_SerialPort->close();

    return 0;
}

int CUART::RecvDataFromUART()
{
    const QByteArray buffer = m_SerialPort->readAll();
    //qDebug() << buffer.size() << "/" << buffer.data();

    quint16 Cmd = 0;
    memcpy(&Cmd, buffer.data(), sizeof(quint16));

    switch(Cmd)
    {
    case COMM_CMD_SAVE_VERSION:
        return Cmd;
    case COMM_CMD_LOAD_VERSION:
        memcpy(&m_Recv_stVERSIONINFO, buffer.data(), sizeof(STC_VERSIONINFO_BUF));
        return Cmd;
    case COMM_CMD_STATUS:
        memcpy(&m_Recv_stUI_STATUS, buffer.data(), sizeof(m_Recv_stUI_STATUS));
        return Cmd;
    case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
        return Cmd;
    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
        return Cmd;
    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
        return Cmd;
    case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
        return Cmd;
    case COMM_CMD_DOWNLINK_LOAD_WAVE:
        memcpy(&m_Recv_DownLink_stWAVE_Tempbuf, buffer.data(), sizeof(STC_WAVE_BUF));
        if(m_Recv_DownLink_stWAVE_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_DownLink_stWAVE + m_Recv_DownLink_stWAVE_Tempbuf.Index, &m_Recv_DownLink_stWAVE_Tempbuf, sizeof(STC_WAVE_BUF));
        return Cmd;
    case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
        memcpy(&m_Recv_DownLink_stOFFSET_A_Tempbuf, buffer.data(), sizeof(STC_OFFSET_BUF));
        if(m_Recv_DownLink_stOFFSET_A_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_DownLink_stOFFSET_A + m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, &m_Recv_DownLink_stOFFSET_A_Tempbuf, sizeof(STC_OFFSET_BUF));
        return Cmd;
    case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
        memcpy(&m_Recv_DownLink_stOFFSET_B_Tempbuf, buffer.data(), sizeof(STC_OFFSET_BUF));   
        if(m_Recv_DownLink_stOFFSET_B_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_DownLink_stOFFSET_B + m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, &m_Recv_DownLink_stOFFSET_B_Tempbuf, sizeof(STC_OFFSET_BUF));
        return Cmd;
    case COMM_CMD_DOWNLINK_LOAD_ALARM:
        memcpy(&m_Recv_DownLink_stALARM_Tempbuf, buffer.data(), sizeof(STC_ALARM_BUF));
        if(m_Recv_DownLink_stALARM_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_DownLink_stALARM + m_Recv_DownLink_stALARM_Tempbuf.Index, &m_Recv_DownLink_stALARM_Tempbuf, sizeof(STC_ALARM_BUF));
        return Cmd;
    case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
        return Cmd;
    case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
        return Cmd;
    case COMM_CMD_UPLINK_UPLOAD_WAVE:
        return Cmd;
    case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
        return Cmd;
    case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
        return Cmd;
    case COMM_CMD_UPLINK_UPLOAD_ALARM:
        return Cmd;
    case COMM_CMD_UPLINK_LOAD_WAVE:
        memcpy(&m_Recv_UpLink_stWAVE_Tempbuf, buffer.data(), sizeof(STC_WAVE_BUF));
        if(m_Recv_UpLink_stWAVE_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_UpLink_stWAVE + m_Recv_UpLink_stWAVE_Tempbuf.Index, &m_Recv_UpLink_stWAVE_Tempbuf, sizeof(STC_WAVE_BUF));
        return Cmd;
    case COMM_CMD_UPLINK_LOAD_OFFSET_A:
        memcpy(&m_Recv_UpLink_stOFFSET_A_Tempbuf, buffer.data(), sizeof(STC_OFFSET_BUF));
        if(m_Recv_UpLink_stOFFSET_A_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_UpLink_stOFFSET_A + m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, &m_Recv_UpLink_stOFFSET_A_Tempbuf, sizeof(STC_OFFSET_BUF));
        return Cmd;
    case COMM_CMD_UPLINK_LOAD_OFFSET_B:
        memcpy(&m_Recv_UpLink_stOFFSET_B_Tempbuf, buffer.data(), sizeof(STC_OFFSET_BUF));   
        if(m_Recv_UpLink_stOFFSET_B_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_UpLink_stOFFSET_B + m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, &m_Recv_UpLink_stOFFSET_B_Tempbuf, sizeof(STC_OFFSET_BUF));
        return Cmd;
    case COMM_CMD_UPLINK_LOAD_ALARM:
        memcpy(&m_Recv_UpLink_stALARM_Tempbuf, buffer.data(), sizeof(STC_ALARM_BUF));
        if(m_Recv_UpLink_stALARM_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_UpLink_stALARM + m_Recv_UpLink_stALARM_Tempbuf.Index, &m_Recv_UpLink_stALARM_Tempbuf, sizeof(STC_ALARM_BUF));
        return Cmd;
    case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
        return Cmd;
    case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
        return Cmd;
    case COMM_CMD_LOAD_DATA:
        memcpy(&m_Recv_stDATA_Tempbuf, buffer.data(), sizeof(STC_DATA_BUF));
        if(m_Recv_stDATA_Tempbuf.Index < ADC_DT_MAXLEN)
            memcpy(m_Recv_stDATA + m_Recv_stDATA_Tempbuf.Index, &m_Recv_stDATA_Tempbuf, sizeof(STC_DATA_BUF));
        return Cmd;
    case COMM_CMD_SET_MAXDATALEN:
        return Cmd;
    case COMM_CMD_SAVE_NETWORKINFO:
        return Cmd;
    case COMM_CMD_LOAD_NETWORKINFO:
        memcpy(&m_Recv_stNETWORK, buffer.data(), sizeof(STC_NETWORK_BUF));
        return Cmd;
    default:
        break;
    }

    return SOCKET_ERROR;
}

int CUART::SendCmdToUART(quint16 Cmd)
{
    if(m_SerialPort == nullptr) return SOCKET_ERROR;

    int Ret = SOCKET_ERROR;
    char strCmd[4] = { 'U', 'S', 'B', ',' };
    char strEOC[1] = { '\n' };
    char* CmdBuf = nullptr;

    switch(Cmd)
    {
    case COMM_CMD_RESET:
    {
    }
        break;

    case COMM_CMD_SAVE_VERSION:
    {
        // ',' + 8자리 문자열 = 9byte
        char strBuf[10] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%s", m_Send_stVERSIONINFO.ModuleID);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));        
    }
        break;

    case COMM_CMD_LOAD_VERSION:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_STATUS:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_DownLink_stWAVE.Index, (quint32)round(m_object->m_Table_DownLink_Wavelength[m_Send_DownLink_stWAVE.Index].text().toDouble() * (double)DIV_WAVELENGTH));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;
    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_DownLink_stOFFSET_A.Index, (qint16)round(m_object->m_Table_DownLink_Offset_A[m_Send_DownLink_stOFFSET_A.Index].text().toDouble() * (double)DIV_OPTPOWER));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;
    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_DownLink_stOFFSET_B.Index, (qint16)round(m_object->m_Table_DownLink_Offset_B[m_Send_DownLink_stOFFSET_B.Index].text().toDouble() * (double)DIV_OPTPOWER));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
    {
        // ',' + 6자리숫자 + ',' + 6자리 숫자 + ',' + 1자리 숫자 = 16byte
        char strBuf[17] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%06d,%01d",
                m_Send_DownLink_stALARM.Index, 0, m_object->m_Table_DownLink_Alarm_Enable[m_Send_DownLink_stALARM.Index]);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_LOAD_WAVE:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_DownLink_stWAVE.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_DownLink_stOFFSET_A.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_DownLink_stOFFSET_B.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_LOAD_ALARM:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_DownLink_stALARM.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
    {
        // ',' + 10자리 숫자 = 11byte
        char strBuf[12] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%010d", (qint32)(m_Send_stUI_STATUS.OptPower_Alarm_Limit[0]));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
    {
        // ',' + 10자리 숫자 = 11byte
        char strBuf[12] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%010d", (qint32)(m_object->m_Boundary_Offset));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_UPLOAD_WAVE:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_UpLink_stWAVE.Index, (quint32)round(m_object->m_Table_UpLink_Wavelength[m_Send_UpLink_stWAVE.Index].text().toDouble() * (double)DIV_WAVELENGTH));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;
    case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_UpLink_stOFFSET_A.Index, (qint16)round(m_object->m_Table_UpLink_Offset_A[m_Send_UpLink_stOFFSET_A.Index].text().toDouble() * (double)DIV_OPTPOWER));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;
    case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
    {
        // ',' + 6자리숫자 + ',' + 10자리 숫자 = 18byte
        char strBuf[19] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%010d",
                m_Send_UpLink_stOFFSET_B.Index, (qint16)round(m_object->m_Table_UpLink_Offset_B[m_Send_UpLink_stOFFSET_B.Index].text().toDouble() * (double)DIV_OPTPOWER));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_UPLOAD_ALARM:
    {
        // ',' + 6자리숫자 + ',' + 6자리 숫자 + ',' + 1자리 숫자 = 16byte
        char strBuf[17] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d,%06d,%01d",
                m_Send_UpLink_stALARM.Index, 0, m_object->m_Table_UpLink_Alarm_Enable[m_Send_UpLink_stALARM.Index]);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_LOAD_WAVE:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_UpLink_stWAVE.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_A:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_UpLink_stOFFSET_A.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_B:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_UpLink_stOFFSET_B.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_LOAD_ALARM:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_UpLink_stALARM.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
    {
        // ',' + 10자리 숫자 = 11byte
        char strBuf[12] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%010d", (qint32)(m_Send_stUI_STATUS.OptPower_Alarm_Limit[1]));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
    {
        // ',' + 10자리 숫자 = 11byte
        char strBuf[12] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%010d", (qint32)(m_object->m_Boundary_Offset));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_LOAD_DATA:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));   
        sprintf(strBuf, ",%06d", m_Send_stDATA.Index);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_OPTSW_DOWN:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_OPTSW_UP:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_SET_CHANNEL:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_object->m_groupBox_CurrSelChannel);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_SET_MAXDATALEN:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_object->m_SetChannellen);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_SET_ATT:
    {
        // ',' + 6자리 숫자 = 7byte
        char strBuf[8] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%06d", m_Send_stUI_STATUS.Attenuation);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_RESET_ATT:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_SAVE:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_SAVE_NETWORKINFO:
    {
        // ',' + 32자리 문자열 + ',' + 12자리 문자열 + ',' + 3자리 숫자 = 50byte
        char strBuf[51] = { 0, };
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        sprintf(strBuf, ",%s,%s,%03d",
                m_Send_stNETWORK.ID, m_Send_stNETWORK.Pwd, m_Send_stNETWORK.IP[3]);
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strBuf, sizeof(strBuf));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strBuf) + sizeof(strEOC));
    }
        break;

    case COMM_CMD_LOAD_NETWORKINFO:
    {
        CmdBuf = (char*)malloc(sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
        if (CmdBuf == NULL) return Ret;
        memcpy(CmdBuf, strCmd, sizeof(strCmd));
        memcpy(CmdBuf + sizeof(strCmd), &Cmd, sizeof(Cmd));
        memcpy(CmdBuf + sizeof(strCmd) + sizeof(Cmd), strEOC, sizeof(strEOC));
        Ret = m_SerialPort->write(reinterpret_cast<char*>(CmdBuf), sizeof(strCmd) + sizeof(Cmd) + sizeof(strEOC));
    }
        break;

    default:
        break;
    }

    free(CmdBuf);
    CmdBuf = nullptr;

    return Ret;
}

int CUART::uartError(QSerialPort::SerialPortError error)
{
    qDebug() << "Uart Error : " + QString(error);
    return error;
}

