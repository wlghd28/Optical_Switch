#include "optical_detection_ui.h"
#include "ui_optical_detection_ui.h"

Optical_Detection_UI::Optical_Detection_UI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Optical_Detection_UI)
    , m_ptrUART(nullptr)
    , m_ptrTCP(nullptr)
{
    ui->setupUi(this);
    Init();
}

Optical_Detection_UI::~Optical_Detection_UI()
{
    delete ui;
}

/*
    Event Slots
*/
// ParentTap

// Main
void Optical_Detection_UI::on_checkBox_Main_Use_UART_clicked()
{
    m_pTimer_SendCommCmd_TCP->stop();
    if(m_ptrUART->m_SerialPort->isOpen())
    {
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm );
    }
    ui->checkBox_Main_Use_UART->setCheckState(Qt::CheckState::Checked);
    ui->groupBox_Main_SetUART->setEnabled(true);
    ui->checkBox_Main_Use_TCP->setCheckState(Qt::CheckState::Unchecked);
    ui->groupBox_Main_SetTCP->setEnabled(false);
    m_Comm_Type = 0;
}

void Optical_Detection_UI::on_pushButton_Main_ConnectUART_clicked()
{
    int Ret = 0;
    Ret = m_ptrUART->StartSerialPort
    (
        ui->comboBox_Main_UART_Comport->currentText(),
        ui->comboBox_Main_UART_BaudRate->currentText().toInt()
    );

    if(Ret >= 0)
    {
        ui->pushButton_Main_ConnectUART->setEnabled(false);
        ui->pushButton_Main_DisconnectUART->setEnabled(true);
        m_Timer_State = TIMER_STATE_VERSION;
        m_TimerClock_Comm = TIMER_CLOCK_START;
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
    }
    else
    {
        QMessageBox::critical(this, windowTitle(), "Fail to connect comport");
    }
}

void Optical_Detection_UI::on_pushButton_Main_DisconnectUART_clicked()
{
    m_ptrUART->StopSerialPort();
    m_pTimer_SendCommCmd_UART->stop();
    ui->pushButton_Main_ConnectUART->setEnabled(true);
    ui->pushButton_Main_DisconnectUART->setEnabled(false);
    ui->ledwidget_Main_TXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    ui->ledwidget_Main_RXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
}


void Optical_Detection_UI::on_checkBox_Main_Use_TCP_clicked()
{
    m_pTimer_SendCommCmd_UART->stop();
    if(m_ptrTCP->m_ClientSocket->state() == QTcpSocket::ConnectedState)
    {
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
    }
    ui->checkBox_Main_Use_UART->setCheckState(Qt::CheckState::Unchecked);
    ui->groupBox_Main_SetUART->setEnabled(false);
    ui->checkBox_Main_Use_TCP->setCheckState(Qt::CheckState::Checked);
    ui->groupBox_Main_SetTCP->setEnabled(true);
    m_Comm_Type = 1;
}

void Optical_Detection_UI::on_pushButton_Main_ConnectTCP_clicked()
{
    ui->pushButton_Main_ConnectTCP->setEnabled(false);
    ui->pushButton_Main_DisconnectTCP->setEnabled(true);
    m_Timer_State = TIMER_STATE_VERSION;
    m_TimerClock_Comm = TIMER_CLOCK_START;
    m_Proc_Ping->start("ping", QStringList() << "-n" << "1" << ui->comboBox_Main_TCP_WIFIList->currentText());
}

void Optical_Detection_UI::on_pushButton_Main_DisconnectTCP_clicked()
{
    m_ptrTCP->CloseClientSocket();
    m_pTimer_SendCommCmd_TCP->stop();
    ui->pushButton_Main_ConnectTCP->setEnabled(true);
    ui->pushButton_Main_DisconnectTCP->setEnabled(false);
    ui->ledwidget_Main_TXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    ui->ledwidget_Main_RXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
}


void Optical_Detection_UI::on_pushButton_Main_Network_clicked()
{
    if(m_ptrNetwork == nullptr)
    {
        m_ptrNetwork = new CNetwork(this);
        m_ptrNetwork->setAttribute(Qt::WA_DeleteOnClose, true);
        m_ptrNetwork->show();
    }
    else
    {
        m_ptrNetwork->raise();
    }
}

void Optical_Detection_UI::on_pushButton_Main_Setting_clicked()
{
    m_ptrVerification = new CVerification(this);
    m_ptrVerification->setModal(true);
    m_ptrVerification->show();
}

void Optical_Detection_UI::on_pushButton_Main_WaveGraph_clicked()
{
    if(m_ptrGraph == nullptr)
    {
        m_ptrGraph = new CGraph(this);
        m_ptrGraph->setAttribute(Qt::WA_DeleteOnClose, true);
        m_ptrGraph->show();
    }
    else
    {
        m_ptrGraph->raise();
    }
}


void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_0_clicked()
{
    m_groupBox_CurrSelChannel = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_1_clicked()
{
    m_groupBox_CurrSelChannel = 1;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_2_clicked()
{
    m_groupBox_CurrSelChannel = 2;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_3_clicked()
{
    m_groupBox_CurrSelChannel = 3;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_4_clicked()
{
    m_groupBox_CurrSelChannel = 4;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_5_clicked()
{
    m_groupBox_CurrSelChannel = 5;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_6_clicked()
{
    m_groupBox_CurrSelChannel = 6;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_7_clicked()
{
    m_groupBox_CurrSelChannel = 7;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_8_clicked()
{
    m_groupBox_CurrSelChannel = 8;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_9_clicked()
{
    m_groupBox_CurrSelChannel = 9;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_10_clicked()
{
    m_groupBox_CurrSelChannel = 10;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_11_clicked()
{
    m_groupBox_CurrSelChannel = 11;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_12_clicked()
{
    m_groupBox_CurrSelChannel = 12;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_13_clicked()
{
    m_groupBox_CurrSelChannel = 13;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_14_clicked()
{
    m_groupBox_CurrSelChannel = 14;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_15_clicked()
{
    m_groupBox_CurrSelChannel = 15;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_16_clicked()
{
    m_groupBox_CurrSelChannel = 16;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_17_clicked()
{
    m_groupBox_CurrSelChannel = 17;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_18_clicked()
{
    m_groupBox_CurrSelChannel = 18;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_19_clicked()
{
    m_groupBox_CurrSelChannel = 19;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_20_clicked()
{
    m_groupBox_CurrSelChannel = 20;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_21_clicked()
{
    m_groupBox_CurrSelChannel = 21;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_22_clicked()
{
    m_groupBox_CurrSelChannel = 22;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_23_clicked()
{
    m_groupBox_CurrSelChannel = 23;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_24_clicked()
{
    m_groupBox_CurrSelChannel = 24;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_25_clicked()
{
    m_groupBox_CurrSelChannel = 25;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_26_clicked()
{
    m_groupBox_CurrSelChannel = 26;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_27_clicked()
{
    m_groupBox_CurrSelChannel = 27;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_28_clicked()
{
    m_groupBox_CurrSelChannel = 28;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_29_clicked()
{
    m_groupBox_CurrSelChannel = 29;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_30_clicked()
{
    m_groupBox_CurrSelChannel = 30;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_31_clicked()
{
    m_groupBox_CurrSelChannel = 31;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_32_clicked()
{
    m_groupBox_CurrSelChannel = 32;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_33_clicked()
{
    m_groupBox_CurrSelChannel = 33;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_34_clicked()
{
    m_groupBox_CurrSelChannel = 34;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_35_clicked()
{
    m_groupBox_CurrSelChannel = 35;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_36_clicked()
{
    m_groupBox_CurrSelChannel = 36;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_37_clicked()
{
    m_groupBox_CurrSelChannel = 37;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_38_clicked()
{
    m_groupBox_CurrSelChannel = 38;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_39_clicked()
{
    m_groupBox_CurrSelChannel = 39;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_40_clicked()
{
    m_groupBox_CurrSelChannel = 40;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_41_clicked()
{
    m_groupBox_CurrSelChannel = 41;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_42_clicked()
{
    m_groupBox_CurrSelChannel = 42;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_43_clicked()
{
    m_groupBox_CurrSelChannel = 43;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_44_clicked()
{
    m_groupBox_CurrSelChannel = 44;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_45_clicked()
{
    m_groupBox_CurrSelChannel = 45;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_46_clicked()
{
    m_groupBox_CurrSelChannel = 46;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_47_clicked()
{
    m_groupBox_CurrSelChannel = 47;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_48_clicked()
{
    m_groupBox_CurrSelChannel = 48;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_49_clicked()
{
    m_groupBox_CurrSelChannel = 49;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_50_clicked()
{
    m_groupBox_CurrSelChannel = 50;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_51_clicked()
{
    m_groupBox_CurrSelChannel = 51;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_52_clicked()
{
    m_groupBox_CurrSelChannel = 52;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_53_clicked()
{
    m_groupBox_CurrSelChannel = 53;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_54_clicked()
{
    m_groupBox_CurrSelChannel = 54;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_55_clicked()
{
    m_groupBox_CurrSelChannel = 55;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_56_clicked()
{
    m_groupBox_CurrSelChannel = 56;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_57_clicked()
{
    m_groupBox_CurrSelChannel = 57;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_58_clicked()
{
    m_groupBox_CurrSelChannel = 58;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_59_clicked()
{
    m_groupBox_CurrSelChannel = 59;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_60_clicked()
{
    m_groupBox_CurrSelChannel = 60;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_61_clicked()
{
    m_groupBox_CurrSelChannel = 61;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_62_clicked()
{
    m_groupBox_CurrSelChannel = 62;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_DownLink_Channel_63_clicked()
{
    m_groupBox_CurrSelChannel = 63;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_0_clicked()
{
    m_groupBox_CurrSelChannel = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_1_clicked()
{
    m_groupBox_CurrSelChannel = 1;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_2_clicked()
{
    m_groupBox_CurrSelChannel = 2;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_3_clicked()
{
    m_groupBox_CurrSelChannel = 3;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_4_clicked()
{
    m_groupBox_CurrSelChannel = 4;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_5_clicked()
{
    m_groupBox_CurrSelChannel = 5;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_6_clicked()
{
    m_groupBox_CurrSelChannel = 6;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_7_clicked()
{
    m_groupBox_CurrSelChannel = 7;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_8_clicked()
{
    m_groupBox_CurrSelChannel = 8;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_9_clicked()
{
    m_groupBox_CurrSelChannel = 9;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_10_clicked()
{
    m_groupBox_CurrSelChannel = 10;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_11_clicked()
{
    m_groupBox_CurrSelChannel = 11;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_12_clicked()
{
    m_groupBox_CurrSelChannel = 12;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_13_clicked()
{
    m_groupBox_CurrSelChannel = 13;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_14_clicked()
{
    m_groupBox_CurrSelChannel = 14;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_15_clicked()
{
    m_groupBox_CurrSelChannel = 15;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_16_clicked()
{
    m_groupBox_CurrSelChannel = 16;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_17_clicked()
{
    m_groupBox_CurrSelChannel = 17;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_18_clicked()
{
    m_groupBox_CurrSelChannel = 18;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_19_clicked()
{
    m_groupBox_CurrSelChannel = 19;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_20_clicked()
{
    m_groupBox_CurrSelChannel = 20;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_21_clicked()
{
    m_groupBox_CurrSelChannel = 21;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_22_clicked()
{
    m_groupBox_CurrSelChannel = 22;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_23_clicked()
{
    m_groupBox_CurrSelChannel = 23;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_24_clicked()
{
    m_groupBox_CurrSelChannel = 24;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_25_clicked()
{
    m_groupBox_CurrSelChannel = 25;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_26_clicked()
{
    m_groupBox_CurrSelChannel = 26;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_27_clicked()
{
    m_groupBox_CurrSelChannel = 27;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_28_clicked()
{
    m_groupBox_CurrSelChannel = 28;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_29_clicked()
{
    m_groupBox_CurrSelChannel = 29;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_30_clicked()
{
    m_groupBox_CurrSelChannel = 30;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_31_clicked()
{
    m_groupBox_CurrSelChannel = 31;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_32_clicked()
{
    m_groupBox_CurrSelChannel = 32;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_33_clicked()
{
    m_groupBox_CurrSelChannel = 33;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_34_clicked()
{
    m_groupBox_CurrSelChannel = 34;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_35_clicked()
{
    m_groupBox_CurrSelChannel = 35;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_36_clicked()
{
    m_groupBox_CurrSelChannel = 36;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_37_clicked()
{
    m_groupBox_CurrSelChannel = 37;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_38_clicked()
{
    m_groupBox_CurrSelChannel = 38;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_39_clicked()
{
    m_groupBox_CurrSelChannel = 39;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_40_clicked()
{
    m_groupBox_CurrSelChannel = 40;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_41_clicked()
{
    m_groupBox_CurrSelChannel = 41;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_42_clicked()
{
    m_groupBox_CurrSelChannel = 42;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_43_clicked()
{
    m_groupBox_CurrSelChannel = 43;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_44_clicked()
{
    m_groupBox_CurrSelChannel = 44;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_45_clicked()
{
    m_groupBox_CurrSelChannel = 45;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_46_clicked()
{
    m_groupBox_CurrSelChannel = 46;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_47_clicked()
{
    m_groupBox_CurrSelChannel = 47;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_48_clicked()
{
    m_groupBox_CurrSelChannel = 48;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_49_clicked()
{
    m_groupBox_CurrSelChannel = 49;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_50_clicked()
{
    m_groupBox_CurrSelChannel = 50;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_51_clicked()
{
    m_groupBox_CurrSelChannel = 51;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_52_clicked()
{
    m_groupBox_CurrSelChannel = 52;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_53_clicked()
{
    m_groupBox_CurrSelChannel = 53;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_54_clicked()
{
    m_groupBox_CurrSelChannel = 54;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_55_clicked()
{
    m_groupBox_CurrSelChannel = 55;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_56_clicked()
{
    m_groupBox_CurrSelChannel = 56;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_57_clicked()
{
    m_groupBox_CurrSelChannel = 57;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_58_clicked()
{
    m_groupBox_CurrSelChannel = 58;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_59_clicked()
{
    m_groupBox_CurrSelChannel = 59;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_60_clicked()
{
    m_groupBox_CurrSelChannel = 60;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_61_clicked()
{
    m_groupBox_CurrSelChannel = 61;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_62_clicked()
{
    m_groupBox_CurrSelChannel = 62;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_groupBox_Main_UpLink_Channel_63_clicked()
{
    m_groupBox_CurrSelChannel = 63;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_CHANNEL);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_CHANNEL);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Main_OpticalSW_Control_DownLink_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_OPTSW_DOWN);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_OPTSW_DOWN);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Main_OpticalSW_Control_UpLink_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_OPTSW_UP);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_OPTSW_UP);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Main_SetAttenuation_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_stUI_STATUS.Attenuation = (int16_t)(ui->doubleSpinBox_Main_Attenuation->value() * (double)DIV_OPTPOWER);
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_ATT);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_stUI_STATUS.Attenuation = (int16_t)(ui->doubleSpinBox_Main_Attenuation->value() * (double)DIV_OPTPOWER);
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_ATT);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Main_ResetAttenuation_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_RESET_ATT);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_RESET_ATT);
        return;
    }
}


void Optical_Detection_UI::on_pushButton_Main_Save_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_SAVE);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SAVE);
        return;
    }
}

// Table
void Optical_Detection_UI::on_pushButton_Table_DownLink_LoadWave_clicked()
{
    m_Table_Wavelength_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_WAVE);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_WAVE);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_LoadOffset_A_clicked()
{
    m_Table_Offset_A_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_LoadOffset_B_clicked()
{
    m_Table_Offset_B_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_LoadAlarm_clicked()
{
    m_Table_Alarm_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_ALARM);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_ALARM);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_UploadWave_clicked()
{
    m_Table_Wavelength_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_WAVE);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_UPLOAD_WAVE);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_UploadOffset_A_clicked()
{
    m_Table_Offset_A_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_UploadOffset_B_clicked()
{
    m_Table_Offset_B_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_UploadAlarm_clicked()
{
    m_Table_Alarm_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_DownLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_ALARM);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_DownLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_UPLOAD_ALARM);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_Set_AlarmLimitValue_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_stUI_STATUS.OptPower_Alarm_Limit[0] = (int16_t)(ui->doubleSpinBox_Table_DownLink_AlarmLimitValue->value() * (double)DIV_ALARM);
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_SET_ALARM_LIMIT);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_stUI_STATUS.OptPower_Alarm_Limit[0] = (int16_t)(ui->doubleSpinBox_Table_DownLink_AlarmLimitValue->value() * (double)DIV_ALARM);
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_SET_ALARM_LIMIT);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_DownLink_Set_OffsetBoundaryValue_clicked()
{
    m_Boundary_Offset = (qint32)(ui->doubleSpinBox_Table_DownLink_OffsetBoundaryValue->text().toDouble() * (double)100);

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET);
        return;
    }
}

void Optical_Detection_UI::on_tableWidget_Table_DownLink_cellChanged(int row, int column)
{
    switch(column)
    {
    case 0:
        m_Table_DownLink_Wavelength[row] = *(ui->tableWidget_Table_DownLink->item(row, column));
        break;
    case 1:
        m_Table_DownLink_Offset_A[row] = *(ui->tableWidget_Table_DownLink->item(row, column));
        break;
    case 2:
        m_Table_DownLink_Offset_B[row] = *(ui->tableWidget_Table_DownLink->item(row, column));
        break;
    default:
        break;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_LoadWave_clicked()
{
    m_Table_Wavelength_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_WAVE);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_WAVE);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_LoadOffset_A_clicked()
{
    m_Table_Offset_A_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_A);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_OFFSET_A);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_LoadOffset_B_clicked()
{
    m_Table_Offset_B_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_B);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_OFFSET_B);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_LoadAlarm_clicked()
{
    m_Table_Alarm_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_ALARM);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_ALARM);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_UploadWave_clicked()
{
    m_Table_Wavelength_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_WAVE);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stWAVE.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_UPLOAD_WAVE);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_UploadOffset_A_clicked()
{
    m_Table_Offset_A_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_OFFSET_A);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_UPLOAD_OFFSET_A);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_UploadOffset_B_clicked()
{
    m_Table_Offset_B_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_OFFSET_B);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_UPLOAD_OFFSET_B);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_UploadAlarm_clicked()
{
    m_Table_Alarm_SendCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_UpLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_ALARM);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_UpLink_stALARM.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_UPLOAD_ALARM);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_Set_AlarmLimitValue_clicked()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_stUI_STATUS.OptPower_Alarm_Limit[1] = (int16_t)(ui->doubleSpinBox_Table_UpLink_AlarmLimitValue->value() * (double)DIV_ALARM);
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_SET_ALARM_LIMIT);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_stUI_STATUS.OptPower_Alarm_Limit[1] = (int16_t)(ui->doubleSpinBox_Table_UpLink_AlarmLimitValue->value() * (double)DIV_ALARM);
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_SET_ALARM_LIMIT);
        return;
    }
}

void Optical_Detection_UI::on_pushButton_Table_UpLink_Set_OffsetBoundaryValue_clicked()
{
    m_Boundary_Offset = (qint32)(ui->doubleSpinBox_Table_UpLink_OffsetBoundaryValue->text().toDouble() * (double)100);

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET);
        return;
    }
}

void Optical_Detection_UI::on_tableWidget_Table_UpLink_cellChanged(int row, int column)
{
    switch(column)
    {
    case 0:
        m_Table_UpLink_Wavelength[row] = *(ui->tableWidget_Table_UpLink->item(row, column));
        break;
    case 1:
        m_Table_UpLink_Offset_A[row] = *(ui->tableWidget_Table_UpLink->item(row, column));
        break;
    case 2:
        m_Table_UpLink_Offset_B[row] = *(ui->tableWidget_Table_UpLink->item(row, column));
        break;
    default:
        break;
    }
}

void Optical_Detection_UI::on_pushButton_Table_SetTableLength_clicked()
{
    m_SetChannellen = ui->spinBox_Table_SetTableLength->text().toInt();

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_SET_MAXDATALEN);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SET_MAXDATALEN);
        return;
    }
}

// Data
void Optical_Detection_UI::on_pushButton_Data_LoadData_clicked()
{
    m_Data_RecvCount = 0;

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_ptrUART->m_Send_stDATA.Index = 0;
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_LOAD_DATA);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_ptrTCP->m_Send_stDATA.Index = 0;
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_DATA);
        return;
    }
}

/*
    Custom slot
*/
void Optical_Detection_UI::DownLinkTableCellCheckBox_stateChanged(int state)
{
    QWidget *widget_sender = qobject_cast<QWidget *>(sender());
    QModelIndex index_sender;
    if(widget_sender)
    {
        index_sender = ui->tableWidget_Table_DownLink->indexAt(widget_sender->pos());
        ui->tableWidget_Table_DownLink->setCurrentIndex(index_sender);
    }

    switch(index_sender.column())
    {
    case 3:
    {
        m_Table_DownLink_Alarm_Enable[index_sender.row()] = state > 0;
        if(!(state > 0)) m_arrLCDNumber_DownLink_WaveLength[index_sender.row()]->setStyleSheet("background-color:rgb(96, 96, 96); color: rgb(255, 255, 255);");
    }
        break;
    default:
        break;
    }
}

void Optical_Detection_UI::UpLinkTableCellCheckBox_stateChanged(int state)
{
    QWidget *widget_sender = qobject_cast<QWidget *>(sender());
    QModelIndex index_sender;
    if(widget_sender)
    {
        index_sender = ui->tableWidget_Table_UpLink->indexAt(widget_sender->pos());
        ui->tableWidget_Table_UpLink->setCurrentIndex(index_sender);
    }

    switch(index_sender.column())
    {
    case 3:
    {
        m_Table_UpLink_Alarm_Enable[index_sender.row()] = state > 0;
        if(!(state > 0)) m_arrLCDNumber_UpLink_WaveLength[index_sender.row()]->setStyleSheet("background-color:rgb(96, 96, 96); color: rgb(255, 255, 255);");
    }
        break;
    default:
        break;
    }
}

/*
    override Mothods
*/
bool Optical_Detection_UI::eventFilter(QObject* dest, QEvent* event)
{
    switch(event->type())
    {
    case QEvent::WindowStateChange:
        break;


    default:
        break;
    }

    return QObject::eventFilter(dest, event);
}

void Optical_Detection_UI::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Your code here.
    resize(event->size());
}

void Optical_Detection_UI::closeEvent(QCloseEvent *event)
{
    event->accept();
    if(m_ptrGraph != nullptr) m_ptrGraph->close();
    if(m_ptrVerification != nullptr) m_ptrVerification->close();
    if(m_ptrSetting != nullptr) m_ptrSetting->close();

    m_pTimer_SendCommCmd_UART->stop();
    m_pTimer_SendCommCmd_TCP->stop();
}

/*
    Methods
*/
void Optical_Detection_UI::Init()
{
    // 빌드 날짜 프로그램 타이틀바에 표시
    this->setWindowTitle(QString::asprintf("Optical Detection UI (ver %s %s)", __DATE__, __TIME__));

    // event setting
    installEventFilter(this);

    // DPI setting
    //setFixedSize(m_HDPI, m_VDPI);
    //setMaximumWidth(m_HDPI);
    //setMaximumHeight(m_VDPI);

    InitSetting();

    InitWidget();

    InitTable();

    InitUART();

    InitTCP();
}

void Optical_Detection_UI::InitWidget()
{
    ui->groupBox_Main_Battery_Power_Image->setVisible(false);
    ui->groupBox_Main_OpticalSW_DownLink->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(255, 219, 147), stop: 1 #FFFFFF);");
    ui->groupBox_Main_OpticalSW_UpLink->setStyleSheet("");
    ui->groupBox_Main_OpticalSW_DownLink->setEnabled(true);
    ui->groupBox_Main_OpticalSW_UpLink->setEnabled(false);
    ui->groupBox_Main_UpLink_Channel_0->setStyleSheet("background-color: #660099");
    ui->groupBox_Main_DownLink_Channel_0->setStyleSheet("background-color: #660099");


    QLCDNumber* arrLCDNumber_DownLink_WaveLength[ADC_DT_MAXLEN] =
    {
        ui->lcdNumber_Main_DownLink_Wavelength_0,
        ui->lcdNumber_Main_DownLink_Wavelength_1,
        ui->lcdNumber_Main_DownLink_Wavelength_2,
        ui->lcdNumber_Main_DownLink_Wavelength_3,
        ui->lcdNumber_Main_DownLink_Wavelength_4,
        ui->lcdNumber_Main_DownLink_Wavelength_5,
        ui->lcdNumber_Main_DownLink_Wavelength_6,
        ui->lcdNumber_Main_DownLink_Wavelength_7,
        ui->lcdNumber_Main_DownLink_Wavelength_8,
        ui->lcdNumber_Main_DownLink_Wavelength_9,
        ui->lcdNumber_Main_DownLink_Wavelength_10,
        ui->lcdNumber_Main_DownLink_Wavelength_11,
        ui->lcdNumber_Main_DownLink_Wavelength_12,
        ui->lcdNumber_Main_DownLink_Wavelength_13,
        ui->lcdNumber_Main_DownLink_Wavelength_14,
        ui->lcdNumber_Main_DownLink_Wavelength_15,
        ui->lcdNumber_Main_DownLink_Wavelength_16,
        ui->lcdNumber_Main_DownLink_Wavelength_17,
        ui->lcdNumber_Main_DownLink_Wavelength_18,
        ui->lcdNumber_Main_DownLink_Wavelength_19,
        ui->lcdNumber_Main_DownLink_Wavelength_20,
        ui->lcdNumber_Main_DownLink_Wavelength_21,
        ui->lcdNumber_Main_DownLink_Wavelength_22,
        ui->lcdNumber_Main_DownLink_Wavelength_23,
        ui->lcdNumber_Main_DownLink_Wavelength_24,
        ui->lcdNumber_Main_DownLink_Wavelength_25,
        ui->lcdNumber_Main_DownLink_Wavelength_26,
        ui->lcdNumber_Main_DownLink_Wavelength_27,
        ui->lcdNumber_Main_DownLink_Wavelength_28,
        ui->lcdNumber_Main_DownLink_Wavelength_29,
        ui->lcdNumber_Main_DownLink_Wavelength_30,
        ui->lcdNumber_Main_DownLink_Wavelength_31,
        ui->lcdNumber_Main_DownLink_Wavelength_32,
        ui->lcdNumber_Main_DownLink_Wavelength_33,
        ui->lcdNumber_Main_DownLink_Wavelength_34,
        ui->lcdNumber_Main_DownLink_Wavelength_35,
        ui->lcdNumber_Main_DownLink_Wavelength_36,
        ui->lcdNumber_Main_DownLink_Wavelength_37,
        ui->lcdNumber_Main_DownLink_Wavelength_38,
        ui->lcdNumber_Main_DownLink_Wavelength_39,
        ui->lcdNumber_Main_DownLink_Wavelength_40,
        ui->lcdNumber_Main_DownLink_Wavelength_41,
        ui->lcdNumber_Main_DownLink_Wavelength_42,
        ui->lcdNumber_Main_DownLink_Wavelength_43,
        ui->lcdNumber_Main_DownLink_Wavelength_44,
        ui->lcdNumber_Main_DownLink_Wavelength_45,
        ui->lcdNumber_Main_DownLink_Wavelength_46,
        ui->lcdNumber_Main_DownLink_Wavelength_47,
        ui->lcdNumber_Main_DownLink_Wavelength_48,
        ui->lcdNumber_Main_DownLink_Wavelength_49,
        ui->lcdNumber_Main_DownLink_Wavelength_50,
        ui->lcdNumber_Main_DownLink_Wavelength_51,
        ui->lcdNumber_Main_DownLink_Wavelength_52,
        ui->lcdNumber_Main_DownLink_Wavelength_53,
        ui->lcdNumber_Main_DownLink_Wavelength_54,
        ui->lcdNumber_Main_DownLink_Wavelength_55,
        ui->lcdNumber_Main_DownLink_Wavelength_56,
        ui->lcdNumber_Main_DownLink_Wavelength_57,
        ui->lcdNumber_Main_DownLink_Wavelength_58,
        ui->lcdNumber_Main_DownLink_Wavelength_59,
        ui->lcdNumber_Main_DownLink_Wavelength_60,
        ui->lcdNumber_Main_DownLink_Wavelength_61,
        ui->lcdNumber_Main_DownLink_Wavelength_62,
        ui->lcdNumber_Main_DownLink_Wavelength_63
    };

    QLCDNumber* arrLCDNumber_UpLink_WaveLength[ADC_DT_MAXLEN] =
    {
        ui->lcdNumber_Main_UpLink_Wavelength_0,
        ui->lcdNumber_Main_UpLink_Wavelength_1,
        ui->lcdNumber_Main_UpLink_Wavelength_2,
        ui->lcdNumber_Main_UpLink_Wavelength_3,
        ui->lcdNumber_Main_UpLink_Wavelength_4,
        ui->lcdNumber_Main_UpLink_Wavelength_5,
        ui->lcdNumber_Main_UpLink_Wavelength_6,
        ui->lcdNumber_Main_UpLink_Wavelength_7,
        ui->lcdNumber_Main_UpLink_Wavelength_8,
        ui->lcdNumber_Main_UpLink_Wavelength_9,
        ui->lcdNumber_Main_UpLink_Wavelength_10,
        ui->lcdNumber_Main_UpLink_Wavelength_11,
        ui->lcdNumber_Main_UpLink_Wavelength_12,
        ui->lcdNumber_Main_UpLink_Wavelength_13,
        ui->lcdNumber_Main_UpLink_Wavelength_14,
        ui->lcdNumber_Main_UpLink_Wavelength_15,
        ui->lcdNumber_Main_UpLink_Wavelength_16,
        ui->lcdNumber_Main_UpLink_Wavelength_17,
        ui->lcdNumber_Main_UpLink_Wavelength_18,
        ui->lcdNumber_Main_UpLink_Wavelength_19,
        ui->lcdNumber_Main_UpLink_Wavelength_20,
        ui->lcdNumber_Main_UpLink_Wavelength_21,
        ui->lcdNumber_Main_UpLink_Wavelength_22,
        ui->lcdNumber_Main_UpLink_Wavelength_23,
        ui->lcdNumber_Main_UpLink_Wavelength_24,
        ui->lcdNumber_Main_UpLink_Wavelength_25,
        ui->lcdNumber_Main_UpLink_Wavelength_26,
        ui->lcdNumber_Main_UpLink_Wavelength_27,
        ui->lcdNumber_Main_UpLink_Wavelength_28,
        ui->lcdNumber_Main_UpLink_Wavelength_29,
        ui->lcdNumber_Main_UpLink_Wavelength_30,
        ui->lcdNumber_Main_UpLink_Wavelength_31,
        ui->lcdNumber_Main_UpLink_Wavelength_32,
        ui->lcdNumber_Main_UpLink_Wavelength_33,
        ui->lcdNumber_Main_UpLink_Wavelength_34,
        ui->lcdNumber_Main_UpLink_Wavelength_35,
        ui->lcdNumber_Main_UpLink_Wavelength_36,
        ui->lcdNumber_Main_UpLink_Wavelength_37,
        ui->lcdNumber_Main_UpLink_Wavelength_38,
        ui->lcdNumber_Main_UpLink_Wavelength_39,
        ui->lcdNumber_Main_UpLink_Wavelength_40,
        ui->lcdNumber_Main_UpLink_Wavelength_41,
        ui->lcdNumber_Main_UpLink_Wavelength_42,
        ui->lcdNumber_Main_UpLink_Wavelength_43,
        ui->lcdNumber_Main_UpLink_Wavelength_44,
        ui->lcdNumber_Main_UpLink_Wavelength_45,
        ui->lcdNumber_Main_UpLink_Wavelength_46,
        ui->lcdNumber_Main_UpLink_Wavelength_47,
        ui->lcdNumber_Main_UpLink_Wavelength_48,
        ui->lcdNumber_Main_UpLink_Wavelength_49,
        ui->lcdNumber_Main_UpLink_Wavelength_50,
        ui->lcdNumber_Main_UpLink_Wavelength_51,
        ui->lcdNumber_Main_UpLink_Wavelength_52,
        ui->lcdNumber_Main_UpLink_Wavelength_53,
        ui->lcdNumber_Main_UpLink_Wavelength_54,
        ui->lcdNumber_Main_UpLink_Wavelength_55,
        ui->lcdNumber_Main_UpLink_Wavelength_56,
        ui->lcdNumber_Main_UpLink_Wavelength_57,
        ui->lcdNumber_Main_UpLink_Wavelength_58,
        ui->lcdNumber_Main_UpLink_Wavelength_59,
        ui->lcdNumber_Main_UpLink_Wavelength_60,
        ui->lcdNumber_Main_UpLink_Wavelength_61,
        ui->lcdNumber_Main_UpLink_Wavelength_62,
        ui->lcdNumber_Main_UpLink_Wavelength_63
    };

    for(int i = 0; i < ADC_DT_MAXLEN; i++)
    {
        m_arrLCDNumber_DownLink_WaveLength[i] = arrLCDNumber_DownLink_WaveLength[i];
        m_arrLCDNumber_UpLink_WaveLength[i] = arrLCDNumber_UpLink_WaveLength[i];
    }

    QLCDNumber* arrLCDNumber_DownLink_OptPower[ADC_DT_MAXLEN] =
    {
        ui->lcdNumber_Main_DownLink_OptPower_0,
        ui->lcdNumber_Main_DownLink_OptPower_1,
        ui->lcdNumber_Main_DownLink_OptPower_2,
        ui->lcdNumber_Main_DownLink_OptPower_3,
        ui->lcdNumber_Main_DownLink_OptPower_4,
        ui->lcdNumber_Main_DownLink_OptPower_5,
        ui->lcdNumber_Main_DownLink_OptPower_6,
        ui->lcdNumber_Main_DownLink_OptPower_7,
        ui->lcdNumber_Main_DownLink_OptPower_8,
        ui->lcdNumber_Main_DownLink_OptPower_9,
        ui->lcdNumber_Main_DownLink_OptPower_10,
        ui->lcdNumber_Main_DownLink_OptPower_11,
        ui->lcdNumber_Main_DownLink_OptPower_12,
        ui->lcdNumber_Main_DownLink_OptPower_13,
        ui->lcdNumber_Main_DownLink_OptPower_14,
        ui->lcdNumber_Main_DownLink_OptPower_15,
        ui->lcdNumber_Main_DownLink_OptPower_16,
        ui->lcdNumber_Main_DownLink_OptPower_17,
        ui->lcdNumber_Main_DownLink_OptPower_18,
        ui->lcdNumber_Main_DownLink_OptPower_19,
        ui->lcdNumber_Main_DownLink_OptPower_20,
        ui->lcdNumber_Main_DownLink_OptPower_21,
        ui->lcdNumber_Main_DownLink_OptPower_22,
        ui->lcdNumber_Main_DownLink_OptPower_23,
        ui->lcdNumber_Main_DownLink_OptPower_24,
        ui->lcdNumber_Main_DownLink_OptPower_25,
        ui->lcdNumber_Main_DownLink_OptPower_26,
        ui->lcdNumber_Main_DownLink_OptPower_27,
        ui->lcdNumber_Main_DownLink_OptPower_28,
        ui->lcdNumber_Main_DownLink_OptPower_29,
        ui->lcdNumber_Main_DownLink_OptPower_30,
        ui->lcdNumber_Main_DownLink_OptPower_31,
        ui->lcdNumber_Main_DownLink_OptPower_32,
        ui->lcdNumber_Main_DownLink_OptPower_33,
        ui->lcdNumber_Main_DownLink_OptPower_34,
        ui->lcdNumber_Main_DownLink_OptPower_35,
        ui->lcdNumber_Main_DownLink_OptPower_36,
        ui->lcdNumber_Main_DownLink_OptPower_37,
        ui->lcdNumber_Main_DownLink_OptPower_38,
        ui->lcdNumber_Main_DownLink_OptPower_39,
        ui->lcdNumber_Main_DownLink_OptPower_40,
        ui->lcdNumber_Main_DownLink_OptPower_41,
        ui->lcdNumber_Main_DownLink_OptPower_42,
        ui->lcdNumber_Main_DownLink_OptPower_43,
        ui->lcdNumber_Main_DownLink_OptPower_44,
        ui->lcdNumber_Main_DownLink_OptPower_45,
        ui->lcdNumber_Main_DownLink_OptPower_46,
        ui->lcdNumber_Main_DownLink_OptPower_47,
        ui->lcdNumber_Main_DownLink_OptPower_48,
        ui->lcdNumber_Main_DownLink_OptPower_49,
        ui->lcdNumber_Main_DownLink_OptPower_50,
        ui->lcdNumber_Main_DownLink_OptPower_51,
        ui->lcdNumber_Main_DownLink_OptPower_52,
        ui->lcdNumber_Main_DownLink_OptPower_53,
        ui->lcdNumber_Main_DownLink_OptPower_54,
        ui->lcdNumber_Main_DownLink_OptPower_55,
        ui->lcdNumber_Main_DownLink_OptPower_56,
        ui->lcdNumber_Main_DownLink_OptPower_57,
        ui->lcdNumber_Main_DownLink_OptPower_58,
        ui->lcdNumber_Main_DownLink_OptPower_59,
        ui->lcdNumber_Main_DownLink_OptPower_60,
        ui->lcdNumber_Main_DownLink_OptPower_61,
        ui->lcdNumber_Main_DownLink_OptPower_62,
        ui->lcdNumber_Main_DownLink_OptPower_63
    };

    QLCDNumber* arrLCDNumber_UpLink_OptPower[ADC_DT_MAXLEN] =
    {
        ui->lcdNumber_Main_UpLink_OptPower_0,
        ui->lcdNumber_Main_UpLink_OptPower_1,
        ui->lcdNumber_Main_UpLink_OptPower_2,
        ui->lcdNumber_Main_UpLink_OptPower_3,
        ui->lcdNumber_Main_UpLink_OptPower_4,
        ui->lcdNumber_Main_UpLink_OptPower_5,
        ui->lcdNumber_Main_UpLink_OptPower_6,
        ui->lcdNumber_Main_UpLink_OptPower_7,
        ui->lcdNumber_Main_UpLink_OptPower_8,
        ui->lcdNumber_Main_UpLink_OptPower_9,
        ui->lcdNumber_Main_UpLink_OptPower_10,
        ui->lcdNumber_Main_UpLink_OptPower_11,
        ui->lcdNumber_Main_UpLink_OptPower_12,
        ui->lcdNumber_Main_UpLink_OptPower_13,
        ui->lcdNumber_Main_UpLink_OptPower_14,
        ui->lcdNumber_Main_UpLink_OptPower_15,
        ui->lcdNumber_Main_UpLink_OptPower_16,
        ui->lcdNumber_Main_UpLink_OptPower_17,
        ui->lcdNumber_Main_UpLink_OptPower_18,
        ui->lcdNumber_Main_UpLink_OptPower_19,
        ui->lcdNumber_Main_UpLink_OptPower_20,
        ui->lcdNumber_Main_UpLink_OptPower_21,
        ui->lcdNumber_Main_UpLink_OptPower_22,
        ui->lcdNumber_Main_UpLink_OptPower_23,
        ui->lcdNumber_Main_UpLink_OptPower_24,
        ui->lcdNumber_Main_UpLink_OptPower_25,
        ui->lcdNumber_Main_UpLink_OptPower_26,
        ui->lcdNumber_Main_UpLink_OptPower_27,
        ui->lcdNumber_Main_UpLink_OptPower_28,
        ui->lcdNumber_Main_UpLink_OptPower_29,
        ui->lcdNumber_Main_UpLink_OptPower_30,
        ui->lcdNumber_Main_UpLink_OptPower_31,
        ui->lcdNumber_Main_UpLink_OptPower_32,
        ui->lcdNumber_Main_UpLink_OptPower_33,
        ui->lcdNumber_Main_UpLink_OptPower_34,
        ui->lcdNumber_Main_UpLink_OptPower_35,
        ui->lcdNumber_Main_UpLink_OptPower_36,
        ui->lcdNumber_Main_UpLink_OptPower_37,
        ui->lcdNumber_Main_UpLink_OptPower_38,
        ui->lcdNumber_Main_UpLink_OptPower_39,
        ui->lcdNumber_Main_UpLink_OptPower_40,
        ui->lcdNumber_Main_UpLink_OptPower_41,
        ui->lcdNumber_Main_UpLink_OptPower_42,
        ui->lcdNumber_Main_UpLink_OptPower_43,
        ui->lcdNumber_Main_UpLink_OptPower_44,
        ui->lcdNumber_Main_UpLink_OptPower_45,
        ui->lcdNumber_Main_UpLink_OptPower_46,
        ui->lcdNumber_Main_UpLink_OptPower_47,
        ui->lcdNumber_Main_UpLink_OptPower_48,
        ui->lcdNumber_Main_UpLink_OptPower_49,
        ui->lcdNumber_Main_UpLink_OptPower_50,
        ui->lcdNumber_Main_UpLink_OptPower_51,
        ui->lcdNumber_Main_UpLink_OptPower_52,
        ui->lcdNumber_Main_UpLink_OptPower_53,
        ui->lcdNumber_Main_UpLink_OptPower_54,
        ui->lcdNumber_Main_UpLink_OptPower_55,
        ui->lcdNumber_Main_UpLink_OptPower_56,
        ui->lcdNumber_Main_UpLink_OptPower_57,
        ui->lcdNumber_Main_UpLink_OptPower_58,
        ui->lcdNumber_Main_UpLink_OptPower_59,
        ui->lcdNumber_Main_UpLink_OptPower_60,
        ui->lcdNumber_Main_UpLink_OptPower_61,
        ui->lcdNumber_Main_UpLink_OptPower_62,
        ui->lcdNumber_Main_UpLink_OptPower_63
    };

    for(int i = 0; i < ADC_DT_MAXLEN; i++)
    {
        m_arrLCDNumber_DownLink_OptPower[i] = arrLCDNumber_DownLink_OptPower[i];
        m_arrLCDNumber_UpLink_OptPower[i] = arrLCDNumber_UpLink_OptPower[i];
    }

    QGroupBox* arrGroupBox_DownLink_Channel[ADC_DT_MAXLEN] =
    {
        ui->groupBox_Main_DownLink_Channel_0,
        ui->groupBox_Main_DownLink_Channel_1,
        ui->groupBox_Main_DownLink_Channel_2,
        ui->groupBox_Main_DownLink_Channel_3,
        ui->groupBox_Main_DownLink_Channel_4,
        ui->groupBox_Main_DownLink_Channel_5,
        ui->groupBox_Main_DownLink_Channel_6,
        ui->groupBox_Main_DownLink_Channel_7,
        ui->groupBox_Main_DownLink_Channel_8,
        ui->groupBox_Main_DownLink_Channel_9,
        ui->groupBox_Main_DownLink_Channel_10,
        ui->groupBox_Main_DownLink_Channel_11,
        ui->groupBox_Main_DownLink_Channel_12,
        ui->groupBox_Main_DownLink_Channel_13,
        ui->groupBox_Main_DownLink_Channel_14,
        ui->groupBox_Main_DownLink_Channel_15,
        ui->groupBox_Main_DownLink_Channel_16,
        ui->groupBox_Main_DownLink_Channel_17,
        ui->groupBox_Main_DownLink_Channel_18,
        ui->groupBox_Main_DownLink_Channel_19,
        ui->groupBox_Main_DownLink_Channel_20,
        ui->groupBox_Main_DownLink_Channel_21,
        ui->groupBox_Main_DownLink_Channel_22,
        ui->groupBox_Main_DownLink_Channel_23,
        ui->groupBox_Main_DownLink_Channel_24,
        ui->groupBox_Main_DownLink_Channel_25,
        ui->groupBox_Main_DownLink_Channel_26,
        ui->groupBox_Main_DownLink_Channel_27,
        ui->groupBox_Main_DownLink_Channel_28,
        ui->groupBox_Main_DownLink_Channel_29,
        ui->groupBox_Main_DownLink_Channel_30,
        ui->groupBox_Main_DownLink_Channel_31,
        ui->groupBox_Main_DownLink_Channel_32,
        ui->groupBox_Main_DownLink_Channel_33,
        ui->groupBox_Main_DownLink_Channel_34,
        ui->groupBox_Main_DownLink_Channel_35,
        ui->groupBox_Main_DownLink_Channel_36,
        ui->groupBox_Main_DownLink_Channel_37,
        ui->groupBox_Main_DownLink_Channel_38,
        ui->groupBox_Main_DownLink_Channel_39,
        ui->groupBox_Main_DownLink_Channel_40,
        ui->groupBox_Main_DownLink_Channel_41,
        ui->groupBox_Main_DownLink_Channel_42,
        ui->groupBox_Main_DownLink_Channel_43,
        ui->groupBox_Main_DownLink_Channel_44,
        ui->groupBox_Main_DownLink_Channel_45,
        ui->groupBox_Main_DownLink_Channel_46,
        ui->groupBox_Main_DownLink_Channel_47,
        ui->groupBox_Main_DownLink_Channel_48,
        ui->groupBox_Main_DownLink_Channel_49,
        ui->groupBox_Main_DownLink_Channel_50,
        ui->groupBox_Main_DownLink_Channel_51,
        ui->groupBox_Main_DownLink_Channel_52,
        ui->groupBox_Main_DownLink_Channel_53,
        ui->groupBox_Main_DownLink_Channel_54,
        ui->groupBox_Main_DownLink_Channel_55,
        ui->groupBox_Main_DownLink_Channel_56,
        ui->groupBox_Main_DownLink_Channel_57,
        ui->groupBox_Main_DownLink_Channel_58,
        ui->groupBox_Main_DownLink_Channel_59,
        ui->groupBox_Main_DownLink_Channel_60,
        ui->groupBox_Main_DownLink_Channel_61,
        ui->groupBox_Main_DownLink_Channel_62,
        ui->groupBox_Main_DownLink_Channel_63
    };

    QGroupBox* arrGroupBox_UpLink_Channel[ADC_DT_MAXLEN] =
    {
        ui->groupBox_Main_UpLink_Channel_0,
        ui->groupBox_Main_UpLink_Channel_1,
        ui->groupBox_Main_UpLink_Channel_2,
        ui->groupBox_Main_UpLink_Channel_3,
        ui->groupBox_Main_UpLink_Channel_4,
        ui->groupBox_Main_UpLink_Channel_5,
        ui->groupBox_Main_UpLink_Channel_6,
        ui->groupBox_Main_UpLink_Channel_7,
        ui->groupBox_Main_UpLink_Channel_8,
        ui->groupBox_Main_UpLink_Channel_9,
        ui->groupBox_Main_UpLink_Channel_10,
        ui->groupBox_Main_UpLink_Channel_11,
        ui->groupBox_Main_UpLink_Channel_12,
        ui->groupBox_Main_UpLink_Channel_13,
        ui->groupBox_Main_UpLink_Channel_14,
        ui->groupBox_Main_UpLink_Channel_15,
        ui->groupBox_Main_UpLink_Channel_16,
        ui->groupBox_Main_UpLink_Channel_17,
        ui->groupBox_Main_UpLink_Channel_18,
        ui->groupBox_Main_UpLink_Channel_19,
        ui->groupBox_Main_UpLink_Channel_20,
        ui->groupBox_Main_UpLink_Channel_21,
        ui->groupBox_Main_UpLink_Channel_22,
        ui->groupBox_Main_UpLink_Channel_23,
        ui->groupBox_Main_UpLink_Channel_24,
        ui->groupBox_Main_UpLink_Channel_25,
        ui->groupBox_Main_UpLink_Channel_26,
        ui->groupBox_Main_UpLink_Channel_27,
        ui->groupBox_Main_UpLink_Channel_28,
        ui->groupBox_Main_UpLink_Channel_29,
        ui->groupBox_Main_UpLink_Channel_30,
        ui->groupBox_Main_UpLink_Channel_31,
        ui->groupBox_Main_UpLink_Channel_32,
        ui->groupBox_Main_UpLink_Channel_33,
        ui->groupBox_Main_UpLink_Channel_34,
        ui->groupBox_Main_UpLink_Channel_35,
        ui->groupBox_Main_UpLink_Channel_36,
        ui->groupBox_Main_UpLink_Channel_37,
        ui->groupBox_Main_UpLink_Channel_38,
        ui->groupBox_Main_UpLink_Channel_39,
        ui->groupBox_Main_UpLink_Channel_40,
        ui->groupBox_Main_UpLink_Channel_41,
        ui->groupBox_Main_UpLink_Channel_42,
        ui->groupBox_Main_UpLink_Channel_43,
        ui->groupBox_Main_UpLink_Channel_44,
        ui->groupBox_Main_UpLink_Channel_45,
        ui->groupBox_Main_UpLink_Channel_46,
        ui->groupBox_Main_UpLink_Channel_47,
        ui->groupBox_Main_UpLink_Channel_48,
        ui->groupBox_Main_UpLink_Channel_49,
        ui->groupBox_Main_UpLink_Channel_50,
        ui->groupBox_Main_UpLink_Channel_51,
        ui->groupBox_Main_UpLink_Channel_52,
        ui->groupBox_Main_UpLink_Channel_53,
        ui->groupBox_Main_UpLink_Channel_54,
        ui->groupBox_Main_UpLink_Channel_55,
        ui->groupBox_Main_UpLink_Channel_56,
        ui->groupBox_Main_UpLink_Channel_57,
        ui->groupBox_Main_UpLink_Channel_58,
        ui->groupBox_Main_UpLink_Channel_59,
        ui->groupBox_Main_UpLink_Channel_60,
        ui->groupBox_Main_UpLink_Channel_61,
        ui->groupBox_Main_UpLink_Channel_62,
        ui->groupBox_Main_UpLink_Channel_63
    };

    for(int i = 0; i < ADC_DT_MAXLEN; i++)
    {
        m_arrGroupBox_DownLink_Channel[i] = arrGroupBox_DownLink_Channel[i];
        m_arrGroupBox_UpLink_Channel[i] = arrGroupBox_UpLink_Channel[i];
    }
}

void Optical_Detection_UI::InitTable()
{
    // Table
    QStringList tableHeader_Horizon;
    QStringList tableHeader_Vetical;
    tableHeader_Horizon << "Wavelength" << "Offset A" << "Offset B" << "Alarm.En";
    for(int i = 0; i < ADC_DT_MAXLEN; i++)
    {
        tableHeader_Vetical << QString::asprintf("%d", i + 1);
        QCheckBox* DownLink_cell_3 = new QCheckBox(ui->tableWidget_Table_DownLink);
        QCheckBox* UpLink_cell_3 = new QCheckBox(ui->tableWidget_Table_UpLink);
        connect(DownLink_cell_3, SIGNAL(stateChanged(int)), this, SLOT(DownLinkTableCellCheckBox_stateChanged(int)));
        connect(UpLink_cell_3, SIGNAL(stateChanged(int)), this, SLOT(UpLinkTableCellCheckBox_stateChanged(int)));
        ui->tableWidget_Table_DownLink->setCellWidget(i, 3, DownLink_cell_3);
        ui->tableWidget_Table_UpLink->setCellWidget(i, 3, UpLink_cell_3);
    }
    ui->tableWidget_Table_DownLink->setHorizontalHeaderLabels(tableHeader_Horizon);
    ui->tableWidget_Table_DownLink->setVerticalHeaderLabels(tableHeader_Vetical);
    ui->tableWidget_Table_UpLink->setHorizontalHeaderLabels(tableHeader_Horizon);
    ui->tableWidget_Table_UpLink->setVerticalHeaderLabels(tableHeader_Vetical);

    // Data
    QStringList dataHeader_Horizon;
    QStringList dataHeader_Vetical;
    dataHeader_Horizon << "Wavelength" << "Opt Power" << "Att Value" << "Opt Direction";
    for(int i = 0; i < MAX_DISPLAYBUF_INDEX_EEP + 1; i++)
    {
        dataHeader_Vetical << QString::asprintf("%d", i + 1);
    }
    ui->tableWidget_Data->setHorizontalHeaderLabels(dataHeader_Horizon);
    ui->tableWidget_Data->setVerticalHeaderLabels(dataHeader_Vetical);
}

void Optical_Detection_UI::UpdateUIStatus_UART()
{
    QString arrStyleSheet_LED[2] =
    {
        "border-radius: 10px;min-height: 20px;min-width: 20px;background-color: rgb(127,127,127);",
        "border-radius: 10px;min-height: 20px;min-width: 20px;background-color: rgb(0,255,0);"
    };

    QString arrStyleSheet_LcdNumber[2] =
    {
        "background-color:rgb(96, 96, 96); color: rgb(255, 255, 255);",
        "background-color:rgb(255, 0, 0); color: rgb(255, 255, 255);"
    };

    UpdateSetChannellen_UART();

    UpdateSetChannel_UART();

    UpdateOptSW_UART();

    // Alarm Limit
    ui->lcdNumber_Table_DownLink_AlarmLimitValue->display(QString::asprintf("%.1lf", (double)m_ptrUART->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[0] / (double)DIV_ALARM));
    ui->lcdNumber_Table_UpLink_AlarmLimitValue->display(QString::asprintf("%.1lf", (double)m_ptrUART->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[1] / (double)DIV_ALARM));

    // Boundary Offset
    ui->lcdNumber_Table_DownLink_OffsetBoundaryValue->display(QString::asprintf("%.1lf", (double)m_ptrUART->m_Recv_stUI_STATUS.Boundary_Offset[0] / (double)DIV_OFFSET));
    ui->lcdNumber_Table_UpLink_OffsetBoundaryValue->display(QString::asprintf("%.1lf", (double)m_ptrUART->m_Recv_stUI_STATUS.Boundary_Offset[1] / (double)DIV_OFFSET));

    // ATT Value
    ui->lcdNumber_Main_ATTValue->display(QString::asprintf("%.1lf",
      (double)m_ptrUART->m_Recv_stUI_STATUS.Attenuation / (double)DIV_OPTPOWER));

    // BATT Status
    ui->progressBar_Main_Battery_Capacity->setValue(m_ptrUART->m_Recv_stUI_STATUS.Bat_Capacity);
    ui->groupBox_Main_Battery_Power_Image->setVisible(m_ptrUART->m_Recv_stUI_STATUS.IN_Power > ALARM_VBUS_LOW);

    // Wifi Status
    ui->ledwidget_Main_TCP_WifiState->setStyleSheet(arrStyleSheet_LED[m_ptrUART->m_Recv_stUI_STATUS.Wifi_State > 0]);

    // OptSwitch Display
    if(m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 0)
    {
        for(int i = 0; i < m_ptrUART->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrLCDNumber_DownLink_OptPower[i]->display(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_stUI_STATUS.Optpower[i] / (double)DIV_OPTPOWER));
            if(m_Table_DownLink_Alarm_Enable[i])
            {
                m_arrLCDNumber_DownLink_WaveLength[i]->setStyleSheet(arrStyleSheet_LcdNumber[m_ptrUART->m_Recv_stUI_STATUS.Optpower[i] < m_ptrUART->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[0]]);
            }
        }
    }
    else if (m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 1)
    {   
        for(int i = 0;i < m_ptrUART->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrLCDNumber_UpLink_OptPower[i]->display(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_stUI_STATUS.Optpower[i] / (double)DIV_OPTPOWER));
            if(m_Table_UpLink_Alarm_Enable[i])
            {
                m_arrLCDNumber_UpLink_WaveLength[i]->setStyleSheet(arrStyleSheet_LcdNumber[m_ptrUART->m_Recv_stUI_STATUS.Optpower[i] < m_ptrUART->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[1]]);
            }
        }
    }

    if(m_ptrGraph != nullptr)   m_ptrGraph->UpdateGraph();
}

void Optical_Detection_UI::UpdateSetChannellen_UART()
{
    if(m_beforeChannellen != m_ptrUART->m_Recv_stUI_STATUS.Channellen)
    {  
        for(int i = 0; i < ADC_DT_MAXLEN; i++)
        {
            m_arrGroupBox_DownLink_Channel[i]->setVisible(false);
            m_arrGroupBox_UpLink_Channel[i]->setVisible(false);
        }

        for(int i = 0; i < m_ptrUART->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrGroupBox_DownLink_Channel[i]->setVisible(true);
            m_arrGroupBox_UpLink_Channel[i]->setVisible(true);
        }

        for(int i = 0; i < ADC_DT_MAXLEN; i++)
        {
            if(ui->tableWidget_Table_DownLink->item(i, 0))
            {
                ui->tableWidget_Table_DownLink->item(i, 0)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 0)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 1))
            {
                ui->tableWidget_Table_DownLink->item(i, 1)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 1)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 2))
            {
                ui->tableWidget_Table_DownLink->item(i, 2)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 2)->setBackground(QBrush(Qt::gray));
            }
            if(((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setChecked(false);
                ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setEnabled(false);
            }

            if(ui->tableWidget_Table_UpLink->item(i, 0))
            {
                ui->tableWidget_Table_UpLink->item(i, 0)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 0)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 1))
            {
                ui->tableWidget_Table_UpLink->item(i, 1)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 1)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 2))
            {
                ui->tableWidget_Table_UpLink->item(i, 2)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 2)->setBackground(QBrush(Qt::gray));
            }
            if(((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setChecked(false);
                ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setEnabled(false);
            }

        }

        for(int i = 0; i < m_ptrUART->m_Recv_stUI_STATUS.Channellen; i++)
        {
            if(ui->tableWidget_Table_DownLink->item(i, 0))
            {
                ui->tableWidget_Table_DownLink->item(i, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stWAVE[i].Wavelen / (double)DIV_WAVELENGTH));
                ui->tableWidget_Table_DownLink->item(i, 0)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 1))
            {
                ui->tableWidget_Table_DownLink->item(i, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stOFFSET_A[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_DownLink->item(i, 1)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 2))
            {
                ui->tableWidget_Table_DownLink->item(i, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stOFFSET_B[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_DownLink->item(i, 2)->setBackground(QBrush(Qt::white));
            }
            if(((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setChecked(m_Table_DownLink_Alarm_Enable[i] > 0);
                ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setEnabled(true);
            }

            if(ui->tableWidget_Table_UpLink->item(i, 0))
            {
                ui->tableWidget_Table_UpLink->item(i, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stWAVE[i].Wavelen / (double)DIV_WAVELENGTH));
                ui->tableWidget_Table_UpLink->item(i, 0)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 1))
            {
                ui->tableWidget_Table_UpLink->item(i, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stOFFSET_A[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_UpLink->item(i, 1)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 2))
            {
                ui->tableWidget_Table_UpLink->item(i, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stOFFSET_B[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_UpLink->item(i, 2)->setBackground(QBrush(Qt::white));
            }
            if(((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setChecked(m_Table_UpLink_Alarm_Enable[i] > 0);
                ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setEnabled(true);
            }
        }

        m_beforeChannellen = m_ptrUART->m_Recv_stUI_STATUS.Channellen;
        ui->spinBox_Table_SetTableLength->setValue(m_ptrUART->m_Recv_stUI_STATUS.Channellen);
    }
}

void Optical_Detection_UI::UpdateSetChannel_UART()
{
    if(m_beforeChannel != m_ptrUART->m_Recv_stUI_STATUS.CurrSelChannel)
    {
        // Channel Display
        QString arrStyleSheet_GroupBox[2] = { "", "background-color: #660099"};

        m_arrGroupBox_DownLink_Channel[m_beforeChannel]->setStyleSheet(arrStyleSheet_GroupBox[0]);
        m_arrGroupBox_UpLink_Channel[m_beforeChannel]->setStyleSheet(arrStyleSheet_GroupBox[0]);
        m_arrGroupBox_DownLink_Channel[m_ptrUART->m_Recv_stUI_STATUS.CurrSelChannel]->setStyleSheet(arrStyleSheet_GroupBox[1]);
        m_arrGroupBox_UpLink_Channel[m_ptrUART->m_Recv_stUI_STATUS.CurrSelChannel]->setStyleSheet(arrStyleSheet_GroupBox[1]);
        m_beforeChannel = m_ptrUART->m_Recv_stUI_STATUS.CurrSelChannel;
    }
}

void Optical_Detection_UI::UpdateOptSW_UART()
{
    if(m_beforeOptSW != m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW)
    {
        QString arrStyleSheet_GroupBox[2] =
        {
            "",
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(255, 219, 147), stop: 1 #FFFFFF);"
        };

        ui->groupBox_Main_OpticalSW_DownLink->setStyleSheet(arrStyleSheet_GroupBox[m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 0]);
        ui->groupBox_Main_OpticalSW_UpLink->setStyleSheet(arrStyleSheet_GroupBox[m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 1]);
        ui->groupBox_Main_OpticalSW_DownLink->setEnabled(m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 0);
        ui->groupBox_Main_OpticalSW_UpLink->setEnabled(m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW == 1);
        m_beforeOptSW = m_ptrUART->m_Recv_stUI_STATUS.CurrOptSW;
    }
}

void Optical_Detection_UI::UpdateLoad_DownLink_Wavelength_UART()
{
    if(m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index >= m_beforeChannellen) return;

    // Main GroupBox Title
    m_arrLCDNumber_DownLink_WaveLength[m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index]->display(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table Setting  
    m_Table_DownLink_Wavelength[m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index].setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0))
        ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));
    // Table에 Item 없을 경우는 아이템 추가
    else
       ui->tableWidget_Table_DownLink->setItem(m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0, m_Table_DownLink_Wavelength + m_ptrUART->m_Recv_DownLink_stWAVE_Tempbuf.Index);

}

void Optical_Detection_UI::UpdateLoad_DownLink_Offset_A_UART()
{
    if(m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Offset_A[m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1))
        ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_DownLink->setItem(m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1, m_Table_DownLink_Offset_A + m_ptrUART->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_DownLink_Offset_B_UART()
{  
    if(m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Offset_B[m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2))
        ui->tableWidget_Table_DownLink->item(m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_DownLink->setItem(m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2, m_Table_DownLink_Offset_B + m_ptrUART->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_DownLink_Alarm_UART()
{
    if(m_ptrUART->m_Recv_DownLink_stALARM_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Alarm_Enable[m_ptrUART->m_Recv_DownLink_stALARM_Tempbuf.Index] = m_ptrUART->m_Recv_DownLink_stALARM_Tempbuf.Enable;

    ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(m_ptrUART->m_Recv_DownLink_stALARM_Tempbuf.Index, 3))->setCheckState
    (
        m_ptrUART->m_Recv_DownLink_stALARM_Tempbuf.Enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked
    );
}

void Optical_Detection_UI::UpdateLoad_UpLink_Wavelength_UART()
{
    if(m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index >= m_beforeChannellen) return;

    // Main GroupBox Title
    m_arrLCDNumber_UpLink_WaveLength[m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index]->display(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table Setting
    m_Table_UpLink_Wavelength[m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index].setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0))
        ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));
    // Table에 Item 없을 경우는 아이템 추가
    else
       ui->tableWidget_Table_UpLink->setItem(m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0, m_Table_UpLink_Wavelength + m_ptrUART->m_Recv_UpLink_stWAVE_Tempbuf.Index);

}

void Optical_Detection_UI::UpdateLoad_UpLink_Offset_A_UART()
{
    if(m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Offset_A[m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1))
        ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_UpLink->setItem(m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1, m_Table_UpLink_Offset_A + m_ptrUART->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_UpLink_Offset_B_UART()
{
    if(m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Offset_B[m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2))
        ui->tableWidget_Table_UpLink->item(m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_UpLink->setItem(m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2, m_Table_UpLink_Offset_B + m_ptrUART->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_UpLink_Alarm_UART()
{
    if(m_ptrUART->m_Recv_UpLink_stALARM_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Alarm_Enable[m_ptrUART->m_Recv_UpLink_stALARM_Tempbuf.Index] = m_ptrUART->m_Recv_UpLink_stALARM_Tempbuf.Enable;

    ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(m_ptrUART->m_Recv_UpLink_stALARM_Tempbuf.Index, 3))->setCheckState
    (
        m_ptrUART->m_Recv_UpLink_stALARM_Tempbuf.Enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked
    );
}

void Optical_Detection_UI::UpdateLoadData_UART()
{
    if(m_ptrUART->m_Recv_stDATA_Tempbuf.Wavelen == 0xFFFFFFFF)
    {
        m_Data_Wavelength[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_OptPower[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_Attenuation[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_OptSw[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));
    }
    else
    {
        m_Data_Wavelength[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrUART->m_Recv_stDATA_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

        m_Data_OptPower[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrUART->m_Recv_stDATA_Tempbuf.OptPower / (double)DIV_OPTPOWER));

        m_Data_Attenuation[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrUART->m_Recv_stDATA_Tempbuf.Attenuation / (double)DIV_OPTPOWER));

        m_Data_OptSw[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%s", m_ptrUART->m_Recv_stDATA_Tempbuf.OptSw ? "Up" : "Down"));
    }

    // Table에 Item 있을 경우는 text를 수정
    // Table에 Item 없을 경우는 아이템 추가
    if(ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 0))
        ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 0)->setText(m_Data_Wavelength[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 0, m_Data_Wavelength + m_ptrUART->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 1))
        ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 1)->setText(m_Data_OptPower[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 1, m_Data_OptPower + m_ptrUART->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 2))
        ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 2)->setText(m_Data_Attenuation[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 2, m_Data_Attenuation + m_ptrUART->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 3))
        ui->tableWidget_Data->item(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 3)->setText(m_Data_OptSw[m_ptrUART->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrUART->m_Recv_stDATA_Tempbuf.Index, 3, m_Data_OptSw + m_ptrUART->m_Recv_stDATA_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateUIStatus_TCP()
{
    QString arrStyleSheet_LED[2] =
    {
        "border-radius: 10px;min-height: 20px;min-width: 20px;background-color: rgb(127,127,127);",
        "border-radius: 10px;min-height: 20px;min-width: 20px;background-color: rgb(0,255,0);"
    };

    QString arrStyleSheet_LcdNumber[2] =
    {
        "background-color:rgb(96, 96, 96); color: rgb(255, 255, 255);",
        "background-color:rgb(255, 0, 0); color: rgb(255, 255, 255);"
    };

    UpdateSetChannellen_TCP();

    UpdateSetChannel_TCP();

    UpdateOptSW_TCP();

    // Alarm Limit
    ui->lcdNumber_Table_DownLink_AlarmLimitValue->display(QString::asprintf("%.1lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[0] / (double)DIV_ALARM));
    ui->lcdNumber_Table_UpLink_AlarmLimitValue->display(QString::asprintf("%.1lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[1] / (double)DIV_ALARM));

    // Boundary Offset
    ui->lcdNumber_Table_DownLink_OffsetBoundaryValue->display(QString::asprintf("%.1lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.Boundary_Offset[0] / (double)DIV_OFFSET));
    ui->lcdNumber_Table_UpLink_OffsetBoundaryValue->display(QString::asprintf("%.1lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.Boundary_Offset[1] / (double)DIV_OFFSET));

    // ATT Value
    ui->lcdNumber_Main_ATTValue->display(QString::asprintf("%.1lf",
      (double)m_ptrTCP->m_Recv_stUI_STATUS.Attenuation / (double)DIV_OPTPOWER));

    // BATT Status
    ui->progressBar_Main_Battery_Capacity->setValue(m_ptrTCP->m_Recv_stUI_STATUS.Bat_Capacity);
    ui->groupBox_Main_Battery_Power_Image->setVisible(m_ptrTCP->m_Recv_stUI_STATUS.IN_Power > ALARM_VBUS_LOW);

    // Wifi Status
    ui->ledwidget_Main_TCP_WifiState->setStyleSheet(arrStyleSheet_LED[m_ptrTCP->m_Recv_stUI_STATUS.Wifi_State > 0]);

    // OptSwitch Display
    if(m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 0)
    {    
        for(int i = 0; i < m_ptrTCP->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrLCDNumber_DownLink_OptPower[i]->display(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.Optpower[i] / (double)DIV_OPTPOWER));
            if(m_Table_DownLink_Alarm_Enable[i])
            {
                m_arrLCDNumber_DownLink_WaveLength[i]->setStyleSheet(arrStyleSheet_LcdNumber[m_ptrTCP->m_Recv_stUI_STATUS.Optpower[i] < m_ptrTCP->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[0]]);
            }
        }
    }
    else if (m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 1)
    {    
        for(int i = 0;i < m_ptrTCP->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrLCDNumber_UpLink_OptPower[i]->display(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_stUI_STATUS.Optpower[i] / (double)DIV_OPTPOWER));
            if(m_Table_UpLink_Alarm_Enable[i])
            {
                m_arrLCDNumber_UpLink_WaveLength[i]->setStyleSheet(arrStyleSheet_LcdNumber[m_ptrTCP->m_Recv_stUI_STATUS.Optpower[i] < m_ptrTCP->m_Recv_stUI_STATUS.OptPower_Alarm_Limit[1]]);
            }
        }
    }

    if(m_ptrGraph != nullptr)   m_ptrGraph->UpdateGraph();
}

void Optical_Detection_UI::UpdateSetChannellen_TCP()
{
    if(m_beforeChannellen != m_ptrTCP->m_Recv_stUI_STATUS.Channellen)
    {
        for(int i = 0; i < ADC_DT_MAXLEN; i++)
        {
            m_arrGroupBox_DownLink_Channel[i]->setVisible(false);
            m_arrGroupBox_UpLink_Channel[i]->setVisible(false);
        }

        for(int i = 0; i < m_ptrTCP->m_Recv_stUI_STATUS.Channellen; i++)
        {
            m_arrGroupBox_DownLink_Channel[i]->setVisible(true);
            m_arrGroupBox_UpLink_Channel[i]->setVisible(true);
        }

        for(int i = 0; i < ADC_DT_MAXLEN; i++)
        {
            if(ui->tableWidget_Table_DownLink->item(i, 0))
            {
                ui->tableWidget_Table_DownLink->item(i, 0)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 0)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 1))
            {
                ui->tableWidget_Table_DownLink->item(i, 1)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 1)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 2))
            {
                ui->tableWidget_Table_DownLink->item(i, 2)->setText("");
                ui->tableWidget_Table_DownLink->item(i, 2)->setBackground(QBrush(Qt::gray));
            }
            if(((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setChecked(false);
                ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setEnabled(false);
            }

            if(ui->tableWidget_Table_UpLink->item(i, 0))
            {
                ui->tableWidget_Table_UpLink->item(i, 0)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 0)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 1))
            {
                ui->tableWidget_Table_UpLink->item(i, 1)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 1)->setBackground(QBrush(Qt::gray));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 2))
            {
                ui->tableWidget_Table_UpLink->item(i, 2)->setText("");
                ui->tableWidget_Table_UpLink->item(i, 2)->setBackground(QBrush(Qt::gray));
            }
            if(((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setChecked(false);
                ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setEnabled(false);
            }
        }

        for(int i = 0; i < m_ptrTCP->m_Recv_stUI_STATUS.Channellen; i++)
        {
            if(ui->tableWidget_Table_DownLink->item(i, 0))
            {
                ui->tableWidget_Table_DownLink->item(i, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stWAVE[i].Wavelen / (double)DIV_WAVELENGTH));
                ui->tableWidget_Table_DownLink->item(i, 0)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 1))
            {
                ui->tableWidget_Table_DownLink->item(i, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_A[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_DownLink->item(i, 1)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_DownLink->item(i, 2))
            {
                ui->tableWidget_Table_DownLink->item(i, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_B[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_DownLink->item(i, 2)->setBackground(QBrush(Qt::white));
            }
            if(((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setChecked(m_Table_DownLink_Alarm_Enable[i] > 0);
                ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(i, 3))->setEnabled(true);
            }

            if(ui->tableWidget_Table_UpLink->item(i, 0))
            {
                ui->tableWidget_Table_UpLink->item(i, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stWAVE[i].Wavelen / (double)DIV_WAVELENGTH));
                ui->tableWidget_Table_UpLink->item(i, 0)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 1))
            {
                ui->tableWidget_Table_UpLink->item(i, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_A[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_UpLink->item(i, 1)->setBackground(QBrush(Qt::white));
            }
            if(ui->tableWidget_Table_UpLink->item(i, 2))
            {
                ui->tableWidget_Table_UpLink->item(i, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_B[i].Offset / (double)DIV_OPTPOWER));
                ui->tableWidget_Table_UpLink->item(i, 2)->setBackground(QBrush(Qt::white));
            }
            if(((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3)))
            {
                //((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setChecked(m_Table_UpLink_Alarm_Enable[i] > 0);
                ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(i, 3))->setEnabled(true);
            }
        }

        m_beforeChannellen = m_ptrTCP->m_Recv_stUI_STATUS.Channellen;
        ui->spinBox_Table_SetTableLength->setValue(m_ptrTCP->m_Recv_stUI_STATUS.Channellen);
    }
}

void Optical_Detection_UI::UpdateSetChannel_TCP()
{
    if(m_beforeChannel != m_ptrTCP->m_Recv_stUI_STATUS.CurrSelChannel)
    {
        // Channel Display
        QString arrStyleSheet_GroupBox[2] = { "", "background-color: #660099"};

        m_arrGroupBox_DownLink_Channel[m_beforeChannel]->setStyleSheet(arrStyleSheet_GroupBox[0]);
        m_arrGroupBox_UpLink_Channel[m_beforeChannel]->setStyleSheet(arrStyleSheet_GroupBox[0]);
        m_arrGroupBox_DownLink_Channel[m_ptrTCP->m_Recv_stUI_STATUS.CurrSelChannel]->setStyleSheet(arrStyleSheet_GroupBox[1]);
        m_arrGroupBox_UpLink_Channel[m_ptrTCP->m_Recv_stUI_STATUS.CurrSelChannel]->setStyleSheet(arrStyleSheet_GroupBox[1]);
        m_beforeChannel = m_ptrTCP->m_Recv_stUI_STATUS.CurrSelChannel;
    }
}

void Optical_Detection_UI::UpdateOptSW_TCP()
{
    if(m_beforeOptSW != m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW)
    {
        QString arrStyleSheet_GroupBox[2] =
        {
            "",
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(255, 219, 147), stop: 1 #FFFFFF);"
        };

        ui->groupBox_Main_OpticalSW_DownLink->setStyleSheet(arrStyleSheet_GroupBox[m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 0]);
        ui->groupBox_Main_OpticalSW_UpLink->setStyleSheet(arrStyleSheet_GroupBox[m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 1]);
        ui->groupBox_Main_OpticalSW_DownLink->setEnabled(m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 0);
        ui->groupBox_Main_OpticalSW_UpLink->setEnabled(m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW == 1);
        m_beforeOptSW = m_ptrTCP->m_Recv_stUI_STATUS.CurrOptSW;
    }
}

void Optical_Detection_UI::UpdateLoad_DownLink_Wavelength_TCP()
{
    if(m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index >= m_beforeChannellen) return;

    // Main GroupBox Title
    m_arrLCDNumber_DownLink_WaveLength[m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index]->display(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table Setting
    m_Table_DownLink_Wavelength[m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index].setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0))
        ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));
    // Table에 Item 없을 경우는 아이템 추가
    else
       ui->tableWidget_Table_DownLink->setItem(m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index, 0, m_Table_DownLink_Wavelength + m_ptrTCP->m_Recv_DownLink_stWAVE_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_DownLink_Offset_A_TCP()
{
    if(m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Offset_A[m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1))
        ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_DownLink->setItem(m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index, 1, m_Table_DownLink_Offset_A + m_ptrTCP->m_Recv_DownLink_stOFFSET_A_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_DownLink_Offset_B_TCP()
{
    if(m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Offset_B[m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2))
        ui->tableWidget_Table_DownLink->item(m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_DownLink->setItem(m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index, 2, m_Table_DownLink_Offset_B + m_ptrTCP->m_Recv_DownLink_stOFFSET_B_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_DownLink_Alarm_TCP()
{
    if(m_ptrTCP->m_Recv_DownLink_stALARM_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_DownLink_Alarm_Enable[m_ptrTCP->m_Recv_DownLink_stALARM_Tempbuf.Index] = m_ptrTCP->m_Recv_DownLink_stALARM_Tempbuf.Enable;

    ((QCheckBox*)ui->tableWidget_Table_DownLink->cellWidget(m_ptrTCP->m_Recv_DownLink_stALARM_Tempbuf.Index, 3))->setCheckState
    (
        m_ptrTCP->m_Recv_DownLink_stALARM_Tempbuf.Enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked
    );
}

void Optical_Detection_UI::UpdateLoad_UpLink_Wavelength_TCP()
{
    if(m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index >= m_beforeChannellen) return;

    // Main GroupBox Title
    m_arrLCDNumber_UpLink_WaveLength[m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index]->display(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table Setting
    m_Table_UpLink_Wavelength[m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index].setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0))
        ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));
    // Table에 Item 없을 경우는 아이템 추가
    else
       ui->tableWidget_Table_UpLink->setItem(m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index, 0, m_Table_UpLink_Wavelength + m_ptrTCP->m_Recv_UpLink_stWAVE_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_UpLink_Offset_A_TCP()
{
    if(m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Offset_A[m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1))
        ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_UpLink->setItem(m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index, 1, m_Table_UpLink_Offset_A + m_ptrTCP->m_Recv_UpLink_stOFFSET_A_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_UpLink_Offset_B_TCP()
{
    if(m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Offset_B[m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index].
        setText(QString::asprintf("%.2lf",
        (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));

    // Table에 Item 있을 경우는 text를 수정
    if(ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2))
        ui->tableWidget_Table_UpLink->item(m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2)->setText(QString::asprintf("%.2lf", (double)m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Offset / (double)DIV_OPTPOWER));
    // Table에 Item 없을 경우는 아이템 추가
    else
        ui->tableWidget_Table_UpLink->setItem(m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index, 2, m_Table_UpLink_Offset_B + m_ptrTCP->m_Recv_UpLink_stOFFSET_B_Tempbuf.Index);
}

void Optical_Detection_UI::UpdateLoad_UpLink_Alarm_TCP()
{
    if(m_ptrTCP->m_Recv_UpLink_stALARM_Tempbuf.Index >= m_beforeChannellen) return;

    m_Table_UpLink_Alarm_Enable[m_ptrTCP->m_Recv_UpLink_stALARM_Tempbuf.Index] = m_ptrTCP->m_Recv_UpLink_stALARM_Tempbuf.Enable;

    ((QCheckBox*)ui->tableWidget_Table_UpLink->cellWidget(m_ptrTCP->m_Recv_UpLink_stALARM_Tempbuf.Index, 3))->setCheckState
    (
        m_ptrTCP->m_Recv_UpLink_stALARM_Tempbuf.Enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked
    );
}

void Optical_Detection_UI::UpdateLoadData_TCP()
{
    if(m_ptrTCP->m_Recv_stDATA_Tempbuf.Wavelen == 0xFFFFFFFF)
    {
        m_Data_Wavelength[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_OptPower[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_Attenuation[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));

        m_Data_OptSw[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].setText(QString::asprintf("N"));
    }
    else
    {
        m_Data_Wavelength[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrTCP->m_Recv_stDATA_Tempbuf.Wavelen / (double)DIV_WAVELENGTH));

        m_Data_OptPower[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrTCP->m_Recv_stDATA_Tempbuf.OptPower / (double)DIV_OPTPOWER));

        m_Data_Attenuation[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%.2lf",
            (double)m_ptrTCP->m_Recv_stDATA_Tempbuf.Attenuation / (double)DIV_OPTPOWER));

        m_Data_OptSw[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].
            setText(QString::asprintf("%s", m_ptrTCP->m_Recv_stDATA_Tempbuf.OptSw ? "Up" : "Down"));
    }

    // Table에 Item 있을 경우는 text를 수정
    // Table에 Item 없을 경우는 아이템 추가
    if(ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 0))
        ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 0)->setText(m_Data_Wavelength[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 0, m_Data_Wavelength + m_ptrTCP->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 1))
        ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 1)->setText(m_Data_OptPower[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 1, m_Data_OptPower + m_ptrTCP->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 2))
        ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 2)->setText(m_Data_Attenuation[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 2, m_Data_Attenuation + m_ptrTCP->m_Recv_stDATA_Tempbuf.Index);

    if(ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 3))
        ui->tableWidget_Data->item(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 3)->setText(m_Data_OptSw[m_ptrTCP->m_Recv_stDATA_Tempbuf.Index].text());
    else
        ui->tableWidget_Data->setItem(m_ptrTCP->m_Recv_stDATA_Tempbuf.Index, 3, m_Data_OptSw + m_ptrTCP->m_Recv_stDATA_Tempbuf.Index);
}

void Optical_Detection_UI::setStyleSheet_ToggleLED(quint8 index)
{
    QWidget* arrWidget_ToggleLED[2] =
    {
        ui->ledwidget_Main_TXD,
        ui->ledwidget_Main_RXD,
    };
    arrWidget_ToggleLED[index]->setStyleSheet(m_arrStyleSheet_ToggleLED[m_arrToggleLED_Main[index]]);
    m_arrToggleLED_Main[index] = !m_arrToggleLED_Main[index];
}

/*
    Timer
*/
void Optical_Detection_UI::OnTimerSendCommCmd_UART()
{
    //if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        switch(m_Timer_State)
        {
        case TIMER_STATE_VERSION:
            m_Timer_State = TIMER_STATE_DOWNLINK_WAVE;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_LOAD_VERSION);
            break;
        case TIMER_STATE_DOWNLINK_WAVE:
            m_Timer_State = TIMER_STATE_DOWNLINK_OFFSET_A;
            m_Table_Wavelength_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_WAVE);
            break;
        case TIMER_STATE_DOWNLINK_OFFSET_A:
            m_Timer_State = TIMER_STATE_DOWNLINK_OFFSET_B;
            m_Table_Offset_A_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
            break;
        case TIMER_STATE_DOWNLINK_OFFSET_B:
            m_Timer_State = TIMER_STATE_DOWNLINK_ALARM;
            m_Table_Offset_B_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
            break;
        case TIMER_STATE_DOWNLINK_ALARM:
            m_Timer_State = TIMER_STATE_UPLINK_WAVE;
            m_Table_Alarm_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_ALARM);
            break;
        case TIMER_STATE_UPLINK_WAVE:
            m_Timer_State = TIMER_STATE_UPLINK_OFFSET_A;
            m_Table_Wavelength_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_WAVE);
            break;
        case TIMER_STATE_UPLINK_OFFSET_A:
            m_Timer_State = TIMER_STATE_UPLINK_OFFSET_B;
            m_Table_Offset_A_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_A);
            break;
        case TIMER_STATE_UPLINK_OFFSET_B:
            m_Timer_State = TIMER_STATE_UPLINK_ALARM;
            m_Table_Offset_B_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_B);
            break;
        case TIMER_STATE_UPLINK_ALARM:
            m_Timer_State = TIMER_STATE_STATUS;
            m_Table_Alarm_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_TimerClock_Comm = TIMER_CLOCK_COMM;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_ALARM);
            break;
        case TIMER_STATE_STATUS:
            if(m_ptrUART->SendCmdToUART(COMM_CMD_STATUS) != SOCKET_ERROR)
            {
                setStyleSheet_ToggleLED(0);
            }
            break;
        default:
            m_Timer_State = TIMER_STATE_STATUS;
            break;
        }
    }
}

void Optical_Detection_UI::OnTimerSendCommCmd_TCP()
{
    //if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        switch(m_Timer_State)
        {
        case TIMER_STATE_VERSION:
            m_Timer_State = TIMER_STATE_DOWNLINK_WAVE;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_VERSION);
            break;
        case TIMER_STATE_DOWNLINK_WAVE:
            m_Timer_State = TIMER_STATE_DOWNLINK_OFFSET_A;
            m_Table_Wavelength_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_WAVE);
            break;
        case TIMER_STATE_DOWNLINK_OFFSET_A:
            m_Timer_State = TIMER_STATE_DOWNLINK_OFFSET_B;
            m_Table_Offset_A_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
            break;
        case TIMER_STATE_DOWNLINK_OFFSET_B:
            m_Timer_State = TIMER_STATE_DOWNLINK_ALARM;
            m_Table_Offset_B_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
            break;
        case TIMER_STATE_DOWNLINK_ALARM:
            m_Timer_State = TIMER_STATE_UPLINK_WAVE;
            m_Table_Alarm_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_ALARM);
            break;
        case TIMER_STATE_UPLINK_WAVE:
            m_Timer_State = TIMER_STATE_UPLINK_OFFSET_A;
            m_Table_Wavelength_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_WAVE);
            break;
        case TIMER_STATE_UPLINK_OFFSET_A:
            m_Timer_State = TIMER_STATE_UPLINK_OFFSET_B;
            m_Table_Offset_A_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_OFFSET_A);
            break;
        case TIMER_STATE_UPLINK_OFFSET_B:
            m_Timer_State = TIMER_STATE_UPLINK_ALARM;
            m_Table_Offset_B_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_OFFSET_B);
            break;
        case TIMER_STATE_UPLINK_ALARM:
            m_Timer_State = TIMER_STATE_STATUS;
            m_Table_Alarm_RecvCount = 0;
            m_Data_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_TimerClock_Comm = TIMER_CLOCK_COMM;
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_ALARM);
            break;
        case TIMER_STATE_STATUS:
            if(m_ptrTCP->SendCmdToTCP(COMM_CMD_STATUS) != SOCKET_ERROR)
            {
                setStyleSheet_ToggleLED(0);
            }
            break;
        default:
            m_Timer_State = TIMER_STATE_STATUS;
            break;
        }
    }
}

void Optical_Detection_UI::OnTimerSendCommCmd2_TCP()
{
    m_ptrTCP->SendCmdToTCP(m_Timer_Cmd);
    m_pTimer_SendCommCmd2_TCP->stop();
}

/*
    TCP/IP
*/
void Optical_Detection_UI::InitTCP()
{
    // Create TCP
    m_ptrTCP = new CTCP(this);
    if(m_ptrTCP == nullptr)
    {
        QMessageBox::warning(this, windowTitle(), "Creating TCP was failed!!");
        exit(EXIT_FAILURE);
    }

    m_pTimer_SendCommCmd_TCP = std::make_shared<QTimer>();
    connect(m_pTimer_SendCommCmd_TCP.get(), SIGNAL(timeout()), this, SLOT(OnTimerSendCommCmd_TCP()));

    m_pTimer_SendCommCmd2_TCP = std::make_shared<QTimer>();
    connect(m_pTimer_SendCommCmd2_TCP.get(), SIGNAL(timeout()), this, SLOT(OnTimerSendCommCmd2_TCP()));

    // Ping Setting
    m_Proc_Ping = new QProcess();

    // 프로세스가 종료 상태 신호 발생시 OnPingResult 함수를 호출한다.
    connect(m_Proc_Ping, SIGNAL(finished(int,QProcess::ExitStatus)) ,this ,SLOT(OnPingResult()));
}

void Optical_Detection_UI::RecvDataFromTCP()
{
    switch(m_ptrTCP->RecvDataFromTCP())
    {
    case COMM_CMD_SAVE_VERSION:
    {
        QMessageBox::information(this, windowTitle(), "Complete to save setting information.");
        QString str
        (
            QByteArray
            (
                reinterpret_cast<const char*>((uint8_t*)m_ptrTCP->m_Send_stVERSIONINFO.ModuleID),
                sizeof(m_ptrTCP->m_Send_stVERSIONINFO.ModuleID)
            )
        );
        ui->label_Main_ModuleID->setText(str);
        if(m_ptrSetting != nullptr) m_ptrSetting->SetStrToModuleID(str);
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
    }
        break;

    case COMM_CMD_LOAD_VERSION:
    {
        QString str
        (
            QByteArray
            (
                reinterpret_cast<const char*>((uint8_t*)m_ptrTCP->m_Recv_stVERSIONINFO.ModuleID),
                sizeof(m_ptrTCP->m_Recv_stVERSIONINFO.ModuleID)
            )
        );
        ui->label_Main_ModuleID->setText(str);
        if(m_ptrSetting != nullptr) m_ptrSetting->SetStrToModuleID(str);
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
    }
        break;

    case COMM_CMD_STATUS:
        UpdateUIStatus_TCP();
        setStyleSheet_ToggleLED(1);
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
        if(++m_Table_Wavelength_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload wavelength.");
            m_Table_Wavelength_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stWAVE.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_UPLOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
        if(++m_Table_Offset_A_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset A.");
            m_Table_Offset_A_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
        if(++m_Table_Offset_B_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset B.");
            m_Table_Offset_B_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
        if(++m_Table_Alarm_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload Alarm.");
            m_Table_Alarm_RecvCount = 0;
            m_ptrTCP->m_Send_DownLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_ALARM);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stALARM.Index++;
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_UPLOAD_ALARM);
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_WAVE:
        if(++m_Table_Wavelength_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Wavelength_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stWAVE.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_DownLink_Wavelength_TCP();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
        if(++m_Table_Offset_A_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Offset_A_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stOFFSET_A.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_DownLink_Offset_A_TCP();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
        if(++m_Table_Offset_B_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Offset_B_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stOFFSET_B.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_DOWNLINK_LOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_DownLink_Offset_B_TCP();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_ALARM:
        if(++m_Table_Alarm_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Alarm_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_DownLink_stALARM.Index++;
            m_ptrTCP->SendCmdToTCP(COMM_CMD_DOWNLINK_LOAD_ALARM);
            UpdateLoad_DownLink_Alarm_TCP();
        }
        break;

    case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
        break;

    case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
        break;

    case COMM_CMD_UPLINK_UPLOAD_WAVE:
        if(++m_Table_Wavelength_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload wavelength.");
            m_Table_Wavelength_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stWAVE.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_UPLINK_UPLOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
        if(++m_Table_Offset_A_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset A.");
            m_Table_Offset_A_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
        if(++m_Table_Offset_B_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset B.");
            m_Table_Offset_B_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_UPLINK_UPLOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_ALARM:
        if(++m_Table_Alarm_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload Alarm.");
            m_Table_Alarm_RecvCount = 0;
            m_ptrTCP->m_Send_UpLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_TCP->stop();
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_ALARM);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stALARM.Index++;
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_UPLOAD_ALARM);
        }
        break;

    case COMM_CMD_UPLINK_LOAD_WAVE:
        if(++m_Table_Wavelength_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Wavelength_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stWAVE.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_WAVE);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_WAVE;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_UpLink_Wavelength_TCP();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_A:
        if(++m_Table_Offset_A_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Offset_A_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stOFFSET_A.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_A);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_A;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_UpLink_Offset_A_TCP();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_B:
        if(++m_Table_Offset_B_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Offset_B_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stOFFSET_B.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_OFFSET_B);
            m_Timer_Cmd = COMM_CMD_UPLINK_LOAD_OFFSET_B;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoad_UpLink_Offset_B_TCP();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_ALARM:
        if(++m_Table_Alarm_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Alarm_TCP();
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_UpLink_stALARM.Index++;
            m_ptrTCP->SendCmdToTCP(COMM_CMD_UPLINK_LOAD_ALARM);
            UpdateLoad_UpLink_Alarm_TCP();
        }
        break;

    case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
        break;

    case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
        break;

    case COMM_CMD_LOAD_DATA:
        if(++m_Data_RecvCount > MAX_DISPLAYBUF_INDEX_EEP)
        {
            UpdateLoadData_TCP();
            QMessageBox::information(this, windowTitle(), "Complete to load data.");
            m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrTCP->m_Send_stDATA.Index++;
            //m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_DATA);
            m_Timer_Cmd = COMM_CMD_LOAD_DATA;
            m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
            UpdateLoadData_TCP();
        }
        break;

    case COMM_CMD_SET_MAXDATALEN:
        QMessageBox::information(this, windowTitle(), "Complete to Set Table length.");
        m_Timer_Cmd = COMM_CMD_STATUS;
        m_pTimer_SendCommCmd2_TCP->start(m_TimerDelay_Comm);
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        break;

    case COMM_CMD_SAVE_NETWORKINFO:
        if(m_ptrNetwork != nullptr)
        {
            QMessageBox::information(m_ptrNetwork, windowTitle(), "Complete to Set Newtork Informaion.");
            m_ptrNetwork->Enablebtn_SaveNetworkInfo(true);
        }
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        break;

    case COMM_CMD_LOAD_NETWORKINFO:
        if(m_ptrNetwork != nullptr) m_ptrNetwork->Display_NetworkInfo(m_ptrTCP->m_Recv_stNETWORK.ID, m_ptrTCP->m_Recv_stNETWORK.Pwd, m_ptrTCP->m_Recv_stNETWORK.IP[3]);
        m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
        break;

    default:
        break;
    }
}

void Optical_Detection_UI::ConnectedToTCP()
{
    m_ptrTCP->ConnectedToTCP(ui->comboBox_Main_TCP_WIFIList->currentText());
    m_pTimer_SendCommCmd_TCP->start(m_TimerClock_Comm);
}

void Optical_Detection_UI::disConnectedToTCP()
{
    m_ptrTCP->disConnectedToTCP();
    m_pTimer_SendCommCmd_TCP->stop();
    ui->ledwidget_Main_TXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    ui->ledwidget_Main_RXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    m_Proc_Ping->start("ping", QStringList() << "-n" << "1" << ui->comboBox_Main_TCP_WIFIList->currentText());
}

void Optical_Detection_UI::tcpError(QAbstractSocket::SocketError error)
{
    m_ptrTCP->tcpError(error);
    m_ptrTCP->CloseClientSocket();
    ui->ledwidget_Main_TXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    ui->ledwidget_Main_RXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    m_Proc_Ping->start("ping", QStringList() << "-n" << "1" << ui->comboBox_Main_TCP_WIFIList->currentText());
}


/*
    UART
*/
void Optical_Detection_UI::InitUART()
{
    // Create Uart
    m_ptrUART = new CUART(this);
    if(m_ptrUART == nullptr)
    {
        QMessageBox::warning(this, windowTitle(), "Creating Uart was failed!!");
        exit(EXIT_FAILURE);
    }

    // Set BuadRate
    ui->comboBox_Main_UART_BaudRate->addItem("1200");
    ui->comboBox_Main_UART_BaudRate->addItem("2400");
    ui->comboBox_Main_UART_BaudRate->addItem("4800");
    ui->comboBox_Main_UART_BaudRate->addItem("9600");
    ui->comboBox_Main_UART_BaudRate->addItem("19200");
    ui->comboBox_Main_UART_BaudRate->addItem("38400");
    ui->comboBox_Main_UART_BaudRate->addItem("57600");
    ui->comboBox_Main_UART_BaudRate->addItem("115200");
    ui->comboBox_Main_UART_BaudRate->setCurrentIndex(7);

    m_pTimer_SendCommCmd_UART = std::make_shared<QTimer>();
    connect(m_pTimer_SendCommCmd_UART.get(), SIGNAL(timeout()), this, SLOT(OnTimerSendCommCmd_UART()));
}

void Optical_Detection_UI::RecvDataFromUART()
{
    switch(m_ptrUART->RecvDataFromUART())
    {
    case COMM_CMD_SAVE_VERSION:
    {
        QMessageBox::information(this, windowTitle(), "Complete to save setting information.");
        QString str
        (
            QByteArray
            (
                reinterpret_cast<const char*>((uint8_t*)m_ptrUART->m_Send_stVERSIONINFO.ModuleID),
                sizeof(m_ptrUART->m_Send_stVERSIONINFO.ModuleID)
            )
        );
        ui->label_Main_ModuleID->setText(str);
        if(m_ptrSetting != nullptr) m_ptrSetting->SetStrToModuleID(str);
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
    }
        break;

    case COMM_CMD_LOAD_VERSION:
    {
        QString str
        (
            QByteArray
            (
                reinterpret_cast<const char*>((uint8_t*)m_ptrUART->m_Recv_stVERSIONINFO.ModuleID),
                sizeof(m_ptrUART->m_Recv_stVERSIONINFO.ModuleID)
            )
        );
        ui->label_Main_ModuleID->setText(str);
        if(m_ptrSetting != nullptr) m_ptrSetting->SetStrToModuleID(str);
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
    }
        break;

    case COMM_CMD_STATUS:
        UpdateUIStatus_UART();
        setStyleSheet_ToggleLED(1);
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_WAVE:
        if(++m_Table_Wavelength_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload wavelength.");
            m_Table_Wavelength_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_WAVE);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stWAVE.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_WAVE);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A:
        if(++m_Table_Offset_A_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset A.");
            m_Table_Offset_A_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stOFFSET_A.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_A);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B:
        if(++m_Table_Offset_B_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset B.");
            m_Table_Offset_B_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stOFFSET_B.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_OFFSET_B);
        }
        break;

    case COMM_CMD_DOWNLINK_UPLOAD_ALARM:
        if(++m_Table_Alarm_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload Alarm.");
            m_Table_Alarm_RecvCount = 0;
            m_ptrUART->m_Send_DownLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_ALARM);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stALARM.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_UPLOAD_ALARM);
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_WAVE:
        if(++m_Table_Wavelength_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Wavelength_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stWAVE.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_WAVE);
            UpdateLoad_DownLink_Wavelength_UART();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_A:
        if(++m_Table_Offset_A_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Offset_A_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stOFFSET_A.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_A);
            UpdateLoad_DownLink_Offset_A_UART();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_OFFSET_B:
        if(++m_Table_Offset_B_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Offset_B_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stOFFSET_B.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_OFFSET_B);
            UpdateLoad_DownLink_Offset_B_UART();
        }
        break;

    case COMM_CMD_DOWNLINK_LOAD_ALARM:
        if(++m_Table_Alarm_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_DownLink_Alarm_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_DownLink_stALARM.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_DOWNLINK_LOAD_ALARM);
            UpdateLoad_DownLink_Alarm_UART();
        }
        break;

    case COMM_CMD_DOWNLINK_SET_ALARM_LIMIT:
        break;

    case COMM_CMD_DOWNLINK_SET_BOUNDARY_OFFSET:
        break;

    case COMM_CMD_UPLINK_UPLOAD_WAVE:
        if(++m_Table_Wavelength_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload wavelength.");
            m_Table_Wavelength_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stWAVE.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_WAVE);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stWAVE.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_WAVE);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_OFFSET_A:
        if(++m_Table_Offset_A_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset A.");
            m_Table_Offset_A_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stOFFSET_A.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_A);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stOFFSET_A.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_OFFSET_A);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_OFFSET_B:
        if(++m_Table_Offset_B_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload offset B.");
            m_Table_Offset_B_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stOFFSET_B.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_B);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stOFFSET_B.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_OFFSET_B);
        }
        break;

    case COMM_CMD_UPLINK_UPLOAD_ALARM:
        if(++m_Table_Alarm_SendCount >= ADC_DT_MAXLEN)
        {
            QMessageBox::information(this, windowTitle(), "Complete to upload Alarm.");
            m_Table_Alarm_RecvCount = 0;
            m_ptrUART->m_Send_UpLink_stALARM.Index = 0;
            m_pTimer_SendCommCmd_UART->stop();
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_ALARM);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stALARM.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_UPLOAD_ALARM);
        }
        break;

    case COMM_CMD_UPLINK_LOAD_WAVE:
        if(++m_Table_Wavelength_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Wavelength_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stWAVE.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_WAVE);
            UpdateLoad_UpLink_Wavelength_UART();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_A:
        if(++m_Table_Offset_A_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Offset_A_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stOFFSET_A.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_A);
            UpdateLoad_UpLink_Offset_A_UART();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_OFFSET_B:
        if(++m_Table_Offset_B_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Offset_B_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stOFFSET_B.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_OFFSET_B);
            UpdateLoad_UpLink_Offset_B_UART();
        }
        break;

    case COMM_CMD_UPLINK_LOAD_ALARM:
        if(++m_Table_Alarm_RecvCount >= ADC_DT_MAXLEN)
        {
            UpdateLoad_UpLink_Alarm_UART();
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_UpLink_stALARM.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_UPLINK_LOAD_ALARM);
            UpdateLoad_UpLink_Alarm_UART();
        }
        break;

    case COMM_CMD_UPLINK_SET_ALARM_LIMIT:
        break;

    case COMM_CMD_UPLINK_SET_BOUNDARY_OFFSET:
        break;

    case COMM_CMD_LOAD_DATA:
        if(++m_Data_RecvCount > MAX_DISPLAYBUF_INDEX_EEP)
        {
            UpdateLoadData_UART();
            QMessageBox::information(this, windowTitle(), "Complete to load data.");
            m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        }
        else
        {
            m_ptrUART->m_Send_stDATA.Index++;
            m_ptrUART->SendCmdToUART(COMM_CMD_LOAD_DATA);
            UpdateLoadData_UART();
        }
        break;

    case COMM_CMD_SET_MAXDATALEN:
        QMessageBox::information(this, windowTitle(), "Complete to Set Table length.");
        m_ptrUART->SendCmdToUART(COMM_CMD_STATUS);
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        break;

    case COMM_CMD_SAVE_NETWORKINFO:
        if(m_ptrNetwork != nullptr)
        {
            QMessageBox::information(m_ptrNetwork, windowTitle(), "Complete to Set Newtork Informaion.");
            m_ptrNetwork->Enablebtn_SaveNetworkInfo(true);
        }
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        break;

    case COMM_CMD_LOAD_NETWORKINFO:
        if(m_ptrNetwork != nullptr) m_ptrNetwork->Display_NetworkInfo(m_ptrUART->m_Recv_stNETWORK.ID, m_ptrUART->m_Recv_stNETWORK.Pwd, m_ptrUART->m_Recv_stNETWORK.IP[3]);
        m_pTimer_SendCommCmd_UART->start(m_TimerClock_Comm);
        break;

    default:
        break;
    }
}

void Optical_Detection_UI::uartError(QSerialPort::SerialPortError error)
{
    m_ptrUART->uartError(error);
    m_ptrUART->StopSerialPort();
    m_pTimer_SendCommCmd_UART->stop();
    ui->ledwidget_Main_TXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
    ui->ledwidget_Main_RXD->setStyleSheet(m_arrStyleSheet_ToggleLED[2]);
}

/*
    Ping 슬롯
*/
void Optical_Detection_UI::OnPingResult()
{
    QByteArray output = m_Proc_Ping->readAllStandardOutput();
    if (!output.isEmpty())
    {
        qDebug() << output;
        if (-1 != QString(output).indexOf("ttl", 0, Qt::CaseInsensitive))
        {
           qDebug() << "PING OK";

           if(!ui->pushButton_Main_ConnectTCP->isEnabled() &&
                (ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked))
           {
               m_ptrTCP->ConnectToTCP
               (
                   ui->comboBox_Main_TCP_WIFIList->currentText(),
                   ui->spinBox_Main_TCP_WIFIPort->text().toInt()
               );
           }
        }
        else
        {
           if(!ui->pushButton_Main_ConnectTCP->isEnabled() &&
               (ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked))
           {
               qDebug() << "PING KO";
               m_Proc_Ping->start("ping", QStringList() << "-n" << "1" << ui->comboBox_Main_TCP_WIFIList->currentText());
           }
        }
    }
}

/*
    Graph
*/
void Optical_Detection_UI::InitPtrCGraph()
{
    m_ptrGraph = nullptr;
}

/*
    Verification
*/
void Optical_Detection_UI::InitPtrCVerification()
{
    m_ptrVerification = nullptr;
}


/*
    Network
*/
void Optical_Detection_UI::InitPtrCNetwork()
{
    m_ptrNetwork = nullptr;
}

void Optical_Detection_UI::LoadNetworkInfo()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_LOAD_NETWORKINFO);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_NETWORKINFO);
        return;
    }
}

void Optical_Detection_UI::SaveNetworkInfo()
{
    m_ptrNetwork->Enablebtn_SaveNetworkInfo(false);

    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_SAVE_NETWORKINFO);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SAVE_NETWORKINFO);
        return;
    }
}

/*
    Setting
*/
void Optical_Detection_UI::InitPtrCSetting()
{
    m_ptrSetting = nullptr;
}

void Optical_Detection_UI::InitSetting()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // load ini
    QSettings settings(m_strPath_SettingFile, "SettingFile");

    QPixmap pixmap_LabelLogo(settings.value("Logoimage_Path").toString());
    QPixmap fitpixmap_LabelMainLogo = pixmap_LabelLogo.scaled(ui->label_Main_Logo->width(), ui->label_Main_Logo->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_Main_Logo->setPixmap(fitpixmap_LabelMainLogo);
}

void Optical_Detection_UI::CreateSettingDialog()
{
    if(m_ptrSetting == nullptr)
    {
        m_ptrSetting = new CSetting(this);
        m_ptrSetting->setAttribute(Qt::WA_DeleteOnClose, true);
        m_ptrSetting->show();
    }
    else
    {
        m_ptrSetting->raise();
    }
}

void Optical_Detection_UI::SaveSettingInfo()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_SAVE_VERSION);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_SAVE_VERSION);
        return;
    }
}

void Optical_Detection_UI::LoadSettingInfo()
{
    if(ui->checkBox_Main_Use_UART->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_UART->stop();
        m_ptrUART->SendCmdToUART(COMM_CMD_LOAD_VERSION);
        return;
    }

    if(ui->checkBox_Main_Use_TCP->checkState() == Qt::CheckState::Checked)
    {
        m_pTimer_SendCommCmd_TCP->stop();
        m_ptrTCP->SendCmdToTCP(COMM_CMD_LOAD_VERSION);
        return;
    }
}
