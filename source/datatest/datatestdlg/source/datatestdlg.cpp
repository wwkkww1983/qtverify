/***********************************************
**  文件名:     datatestdlg.cpp
**  功能:       采集与控制测试程序
**  操作系统:   基于Trolltech Qt4.8.5的跨平台系统
**  生成时间:   2014/6/12
**  专业组:     德鲁计量软件组
**  程序设计者: YS
**  程序员:     YS
**  版本历史:   2014/06 第一版
**  内容包含:
**  说明:		只用于测试采集与控制，不作为质量法检定的主界面(2014-07-15备注)
**  更新记录:   
***********************************************/

#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include "datatestdlg.h"
#include "commondefine.h"

DataTestDlg::DataTestDlg(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	qDebug()<<"QualityDlg thread:"<<QThread::currentThreadId();
	ui.setupUi(this);

	//获取下位机端口配置信息
	if (!getPortSetIni(&m_portsetinfo))
	{
		QMessageBox::warning(this, tr("Warning"), tr("get port set info failed!"));//获取下位机端口配置信息失败!请重新设置！
	}

	initValveStatus();      //初始化阀门状态
	initRegulateStatus();   //初始化调节阀状态

	//获取参数设置信息
	if (!getParaSetIni(&m_parasetinfo))
	{
		QMessageBox::warning(this, tr("Warning"), tr("get para set info failed"));//获取质量法参数配置信息失败!请重新设置！
	}
	qDebug()<<"metertype:"<<m_parasetinfo.metertype;

	m_paraset = NULL;

	m_readComConfig = NULL;
	m_readComConfig = new ReadComConfig();

	m_tempObj = NULL;
	m_tempTimer = NULL;
	initTemperatureCom();	//初始化温度采集串口

	m_controlObj = NULL;
	initControlCom();		//初始化控制串口

	m_balanceObj = NULL;
	initBalanceCom();		//初始化天平串口

	bal_quan = 0.0;
	m_setBalTimer = new QTimer();
	connect(m_setBalTimer, SIGNAL(timeout()), this, SLOT(setBalQuantity()));
	m_setBalTimer->start(TIMEOUT_TEMPER/10);

	m_flowcount = 0;		//计算流量用
	m_totalcount = 0;
	m_flow1 = 0.0;
	m_flow2 = 0.0;
	start_quan = 0.0;
	end_quan = 0.0;
	total_quantity = 0.0;
	m_flowTimer = new QTimer();
	connect(m_flowTimer, SIGNAL(timeout()), this,  SLOT(slotFreshFlow()));
	connect(m_flowTimer, SIGNAL(timeout()), this,  SLOT(slotFreshFlow_total()));
	m_flowTimer->start(TIMEOUT_TEMPER);

	m_meterObj = NULL;
 	initComOfHeatMeter();	//初始化热量表串口
}

DataTestDlg::~DataTestDlg()
{
}

void DataTestDlg::closeEvent( QCloseEvent * event)
{
	qDebug()<<"^^^^^QualityDlg::closeEvent";

	if (m_paraset)
	{
		delete m_paraset;
		m_paraset = NULL;
	}

	if (m_readComConfig)  //读串口设置
	{
		delete m_readComConfig;
		m_readComConfig = NULL;
	}

	if (m_tempObj)  // 温度采集
	{
		delete m_tempObj;
		m_tempObj = NULL;

		m_tempThread.exit(); //否则日志中会有警告"QThread: Destroyed while thread is still running"
	}

	if (m_controlObj)  //阀门控制
	{
		delete m_controlObj;
		m_controlObj = NULL;

		m_valveThread.exit();
	}

	if (m_balanceObj)  //天平采集
	{
		delete m_balanceObj;
		m_balanceObj = NULL;

		m_balanceThread.exit();
	}
	
	if (m_meterObj)  //热量表串口通讯
	{
		delete m_meterObj;
		m_meterObj = NULL;

		m_meterThread.exit();
	}

	if (m_tempTimer) //计时器
	{
		if (m_tempTimer->isActive())
		{
			m_tempTimer->stop();
		}
		delete m_tempTimer;
		m_tempTimer = NULL;
	}

	if (m_flowTimer)
	{
		if (m_flowTimer->isActive())
		{
			m_flowTimer->stop();
		}
		delete m_flowTimer;
		m_flowTimer = NULL;
	}

	if (m_setBalTimer)
	{
		if (m_setBalTimer->isActive())
		{
			m_setBalTimer->stop();
		}
		delete m_setBalTimer;
		m_setBalTimer = NULL;
	}
	
}

void DataTestDlg::showEvent(QShowEvent *event)
{
	m_flowcount = 0;
	m_totalcount = 0;
	total_quantity = 0;
	//if (!m_setBalTimer)
	//{
	//	bal_quan = 0.0;
	//	m_setBalTimer = new QTimer();
	//	connect(m_setBalTimer, SIGNAL(timeout()), this, SLOT(setBalQuan()));
	//	m_setBalTimer->start(TIMEOUT_TEMPER/10);
	//}

	//if (!m_flowTimer)
	//{
	//	m_flowcount = 0;		//计算流量用
	//	m_flow1 = 0.0;
	//	m_flow2 = 0.0;
	//	total_quantity = 0.0;
	//	m_flowTimer = new QTimer();
	//	connect(m_flowTimer, SIGNAL(timeout()), this,  SLOT(slotFreshFlow()));
	//	m_flowTimer->start(TIMEOUT_TEMPER/10);
	//}
}

//模拟天平读数
void DataTestDlg::setBalQuantity()
{
//	qDebug()<< " current thread id: "<< QThread::currentThreadId();
	QTime time;
	time= QTime::currentTime();
	qsrand(time.msec()+time.second()*1000);
	float random=qrand()%10;//生成随机数 0~10
	float delta = 0.1;
	QString txt = QString::number(ui.lnEditBigBalance->text().toFloat() + delta);
	ui.lnEditBigBalance->setText(txt);
}

/***************************************
	温度采集
	周期请求
****************************************/
void DataTestDlg::initTemperatureCom()
{
	ComInfoStruct tempStruct = m_readComConfig->ReadTempConfig();
	m_tempObj = new TempComObject();
	m_tempObj->moveToThread(&m_tempThread);
	m_tempThread.start();
	m_tempObj->openTemperatureCom(&tempStruct);
	connect(m_tempObj, SIGNAL(temperatureIsReady(const QString &)), this, SLOT(slotFreshComTempValue(const QString &)));

	m_tempTimer = new QTimer();
	connect(m_tempTimer, SIGNAL(timeout()), m_tempObj, SLOT(writeTemperatureComBuffer()));
	//connect(m_tempTimer, SIGNAL(timeout()), this, SLOT(slotFreshFlow()));
	
	m_tempTimer->start(TIMEOUT_TEMPER); //周期请求温度
}

//控制板通讯串口
void DataTestDlg::initControlCom()
{
	ComInfoStruct valveStruct = m_readComConfig->ReadValveConfig();
	m_controlObj = new ControlComObject();
	m_controlObj->setProtocolVersion(m_portsetinfo.version); //设置协议版本
	m_controlObj->moveToThread(&m_valveThread);
	m_valveThread.start();
	m_controlObj->openControlCom(&valveStruct);

	//控制板有反馈	
	connect(m_controlObj, SIGNAL(controlRelayIsOk(const UINT8 &, const bool &)), this, SLOT(slotSetValveBtnStatus(const UINT8 &, const bool &)));
	connect(m_controlObj, SIGNAL(controlRegulateIsOk()), this, SLOT(slotSetRegulateOk()));
	//天平数值从控制板获取
	connect(m_controlObj, SIGNAL(controlGetBalanceValueIsOk(const QString&)), this, SLOT(slotFreshBalanceValue(const QString &)));
}

//初始化阀门状态
void DataTestDlg::initValveStatus()
{
	m_nowPortNo = 0;

	//端口号-阀门按钮 映射关系
	m_valveBtn[m_portsetinfo.bigNo] = ui.btnValveBig;
	m_valveBtn[m_portsetinfo.smallNo] = ui.btnValveSmall;
	m_valveBtn[m_portsetinfo.middle1No] = ui.btnValveMiddle1;
	m_valveBtn[m_portsetinfo.middle2No] = ui.btnValveMiddle2;
	m_valveBtn[m_portsetinfo.waterInNo] = ui.btnWaterIn;
	m_valveBtn[m_portsetinfo.waterOutNo] = ui.btnWaterOut;

	//端口号-阀门状态 全部阀门状态为关闭
	m_valveStatus[m_portsetinfo.bigNo] = VALVE_CLOSE;
	m_valveStatus[m_portsetinfo.smallNo] = VALVE_CLOSE;
	m_valveStatus[m_portsetinfo.middle1No] = VALVE_CLOSE;
	m_valveStatus[m_portsetinfo.middle2No] = VALVE_CLOSE;
	m_valveStatus[m_portsetinfo.waterInNo] = VALVE_CLOSE;
	m_valveStatus[m_portsetinfo.waterOutNo] = VALVE_CLOSE;

	setValveBtnBackColor(m_valveBtn[m_portsetinfo.bigNo], m_valveStatus[m_portsetinfo.bigNo]);
	setValveBtnBackColor(m_valveBtn[m_portsetinfo.smallNo], m_valveStatus[m_portsetinfo.smallNo]);
	setValveBtnBackColor(m_valveBtn[m_portsetinfo.middle1No], m_valveStatus[m_portsetinfo.middle1No]);
	setValveBtnBackColor(m_valveBtn[m_portsetinfo.middle2No], m_valveStatus[m_portsetinfo.middle2No]);
	setValveBtnBackColor(m_valveBtn[m_portsetinfo.waterInNo], m_valveStatus[m_portsetinfo.waterInNo]);
	setValveBtnBackColor(m_valveBtn[m_portsetinfo.waterOutNo], m_valveStatus[m_portsetinfo.waterOutNo]);
}

//初始化调节阀状态
void DataTestDlg::initRegulateStatus()
{
	m_nowRegNo = 0;

	//端口号-阀门按钮 映射关系
	m_regBtn[m_portsetinfo.pumpNo] = ui.btnWaterPump;
	m_regBtn[m_portsetinfo.regflow1No] = ui.btnRegulate1;

	m_regStatus[m_portsetinfo.pumpNo] = REG_SUCCESS;
	m_regStatus[m_portsetinfo.regflow1No] = REG_SUCCESS;

	setRegBtnBackColor(m_regBtn[m_portsetinfo.pumpNo], m_regStatus[m_portsetinfo.pumpNo]);
	setRegBtnBackColor(m_regBtn[m_portsetinfo.regflow1No], m_regStatus[m_portsetinfo.regflow1No]);
}

//天平采集串口 上位机直接采集
void DataTestDlg::initBalanceCom()
{
	ComInfoStruct balanceStruct = m_readComConfig->ReadBalanceConfig();
	m_balanceObj = new BalanceComObject();
	m_balanceObj->moveToThread(&m_balanceThread);
	m_balanceThread.start();
	m_balanceObj->openBalanceCom(&balanceStruct);

	//天平数值由上位机直接通过天平串口采集
 	connect(m_balanceObj, SIGNAL(balanceValueIsReady(const QString &)), this, SLOT(slotFreshBalanceValue(const QString &)));
}

//热量表串口通讯
void DataTestDlg::initComOfHeatMeter()
{
	m_meterObj = new MeterComObject();
	m_meterObj->setProtocolVersion(0);//设置协议版本号
	m_meterObj->moveToThread(&m_meterThread);
	m_meterThread.start();

	connect(m_meterObj, SIGNAL(readMeterNoIsOK(const QString&, const QString&)), this, SLOT(slotFreshMeterNo(const QString&, const QString&)));
	connect(m_meterObj, SIGNAL(readMeterFlowIsOK(const QString&, const QString&)), this, SLOT(slotFreshMeterFlow(const QString&, const QString&)));
	connect(m_meterObj, SIGNAL(readMeterHeatIsOK(const QString&, const QString&)), this, SLOT(slotFreshMeterHeat(const QString&, const QString&)));
	connect(m_meterObj, SIGNAL(readMeterDateIsOK(const QString&, const QString&)), this, SLOT(slotFreshMeterDate(const QString&, const QString&)));
	connect(m_meterObj, SIGNAL(readMeterTemperIsOK(const QString&, const QString&, const QString&)), \
		this, SLOT(slotFreshMeterTemper(const QString&, const QString&, const QString&)));

// 	ComInfoStruct comStruct = m_readComConfig->ReadMeterConfigByNum("1");
// 	ui.parityComboBox->setCurrentIndex(comStruct.parity);
// 	ui.stopBitsComboBox->setCurrentIndex(comStruct.stopBit);
	QStringList cfgList = m_readComConfig->ReadIndexByName("meter1");
	ui.baudRateComboBox->setCurrentIndex(cfgList.at(1).toInt());
	ui.dataBitsComboBox->setCurrentIndex(cfgList.at(2).toInt());
	ui.parityComboBox->setCurrentIndex(cfgList.at(3).toInt());
	ui.stopBitsComboBox->setCurrentIndex(cfgList.at(4).toInt());

	ui.btnCloseCom->setEnabled(false);
	ui.btnSetVerifyStatus->setEnabled(false);
	ui.btnReadMeterData->setEnabled(false);
	ui.btnModifyMeterNo->setEnabled(false);
	ui.btnModifyFlowPara->setEnabled(false);
}

//打开热量表通讯串口
void DataTestDlg::on_btnOpenCom_clicked()
{
	ComInfoStruct comStruct;// = m_readComConfig->ReadMeterConfigByNum("0");
	comStruct.portName = ui.portNameComboBox->currentText();
	comStruct.baudRate = ui.baudRateComboBox->currentText().toInt();
	comStruct.dataBit = ui.dataBitsComboBox->currentText().toInt();
	comStruct.parity = ui.parityComboBox->currentIndex();
	comStruct.stopBit = ui.stopBitsComboBox->currentIndex();
	if (!m_meterObj->openMeterCom(&comStruct))
	{
		return; //打开热表通讯串口失败
	}

	ui.btnCloseCom->setEnabled(true);
	ui.btnSetVerifyStatus->setEnabled(true);
	ui.btnReadMeterData->setEnabled(true);
	ui.btnModifyMeterNo->setEnabled(true);
	ui.btnModifyFlowPara->setEnabled(true);
	ui.btnOpenCom->setEnabled(false);

	ui.portNameComboBox->setEnabled(false);
	ui.baudRateComboBox->setEnabled(false);
	ui.dataBitsComboBox->setEnabled(false);
	ui.parityComboBox->setEnabled(false);
	ui.stopBitsComboBox->setEnabled(false);
}

//关闭热量表通讯串口
void DataTestDlg::on_btnCloseCom_clicked()
{
	m_meterObj->closeMeterCom();

	ui.btnCloseCom->setEnabled(false);
	ui.btnSetVerifyStatus->setEnabled(false);
	ui.btnReadMeterData->setEnabled(false);
	ui.btnModifyMeterNo->setEnabled(false);
	ui.btnModifyFlowPara->setEnabled(false);
	ui.btnOpenCom->setEnabled(true);

	ui.portNameComboBox->setEnabled(true);
	ui.baudRateComboBox->setEnabled(true);
	ui.dataBitsComboBox->setEnabled(true);
	ui.parityComboBox->setEnabled(true);
	ui.stopBitsComboBox->setEnabled(true);
}

/*
**	控制继电器开断
*/
void DataTestDlg::on_btnWaterIn_clicked() //进水阀
{
	m_nowPortNo = m_portsetinfo.waterInNo;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

void DataTestDlg::on_btnWaterOut_clicked() //出水阀
{
	m_nowPortNo = m_portsetinfo.waterOutNo;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

void DataTestDlg::on_btnValveBig_clicked() //大流量阀
{
	m_nowPortNo = m_portsetinfo.bigNo;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

void DataTestDlg::on_btnValveMiddle1_clicked() //中流一阀
{
	m_nowPortNo = m_portsetinfo.middle1No;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

void DataTestDlg::on_btnValveMiddle2_clicked() //中流二阀
{
	m_nowPortNo = m_portsetinfo.middle2No;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

void DataTestDlg::on_btnValveSmall_clicked() //小流量阀
{
	m_nowPortNo = m_portsetinfo.smallNo;
	m_controlObj->askControlRelay(m_nowPortNo, !m_valveStatus[m_nowPortNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		slotSetValveBtnStatus(m_nowPortNo, !m_valveStatus[m_nowPortNo]);
	}
}

/*
**	控制水泵开关
*/
void DataTestDlg::on_btnWaterPump_clicked() //水泵
{
	m_nowRegNo = m_portsetinfo.pumpNo;
	m_controlObj->askControlWaterPump(m_nowRegNo, !m_regStatus[m_nowRegNo]);

	if (m_portsetinfo.version == OLD_CTRL_VERSION) //老控制板 无反馈
	{
		m_regStatus[m_nowRegNo] = !m_regStatus[m_nowRegNo]; 
		setRegBtnBackColor(m_regBtn[m_nowRegNo], m_regStatus[m_nowRegNo]);
	}
}


//设置频率
void DataTestDlg::on_btnSetFreq_clicked()
{
	m_controlObj->askSetDriverFreq(ui.spinBoxFreq->value());
}

//调节阀
void DataTestDlg::on_btnRegulate1_clicked() //调节阀1
{
	m_nowRegNo = m_portsetinfo.regflow1No;
	setRegBtnBackColor(m_regBtn[m_nowRegNo], false); //初始化调节阀背景色
	m_controlObj->askControlRegulate(m_nowRegNo, ui.spinBoxFreq->value());
}

//查询从机(控制板)状态
void DataTestDlg::on_btnQueryStatus_clicked()
{
	m_controlObj->askControlQuery();
}

//参数设置
void DataTestDlg::on_btnParaSet_clicked()
{
	if (NULL == m_paraset)
	{
		m_paraset = new ParaSetDlg();
	}
	else
	{
		delete m_paraset;
		m_paraset = new ParaSetDlg();
	}
	m_paraset->show();
}

void DataTestDlg::on_btnStart_clicked()
{
}

void DataTestDlg::on_btnExit_clicked()
{
	this->close();
}

//刷新温度
void DataTestDlg::slotFreshComTempValue(const QString& tempStr)
{
	ui.lnEditTempIn->setText(tempStr.left(DATA_WIDTH));   //入口温度 PV
	ui.lnEditTempOut->setText(tempStr.right(DATA_WIDTH)); //出口温度 SV
}

//刷新天平数值
void DataTestDlg::slotFreshBalanceValue(const QString& Str)
{
	ui.lnEditBigBalance->setText(Str);
	if (Str.toFloat() > 100); //防止天平溢出 暂设天平容量为100kg
	{
		// 打开放水阀
		// 关闭进水阀
	}
}

//响应阀门状态设置成功
void DataTestDlg::slotSetValveBtnStatus(const UINT8 &portno, const bool &status)
{
	m_valveStatus[portno] = status;
	setValveBtnBackColor(m_valveBtn[portno], m_valveStatus[portno]);
}

//响应调节阀调节成功
void DataTestDlg::slotSetRegulateOk()
{
	setRegBtnBackColor(m_regBtn[m_nowRegNo], REG_SUCCESS);
}

//设置阀门按钮背景色
void DataTestDlg::setValveBtnBackColor(QPushButton *btn, bool status)
{
	if (NULL == btn)
	{
		return;
	}
	if (status) //阀门打开 绿色
	{
		btn->setStyleSheet("background:green;border:0px;"); 
		btn->setIcon(QIcon(":/datatestdlg/images/open.png"));
	}
	else //阀门关闭 红色
	{
		btn->setStyleSheet("background-color:rgb(255,0,0);border:0px;");
		btn->setIcon(QIcon(":/datatestdlg/images/close.png"));
	}
}

//设置调节阀按钮背景色
void DataTestDlg::setRegBtnBackColor(QPushButton *btn, bool status)
{
	if (NULL == btn)
	{
		return;
	}
	if (status) //调节成功
	{
		btn->setStyleSheet("background:rgb(180,255,200);");  
		btn->setIcon(QIcon(":/datatestdlg/images/success.png"));
	}
	else //调节失败
	{
 		btn->setStyleSheet("background:rgb(255,100,180);");  
		btn->setIcon(QIcon(":/datatestdlg/images/failed.png"));
	}
}

/************************************************************************/
/* 计算瞬时流量(待改进、需要实验验证)                        */
/************************************************************************/
//void QualityDlg::slotFreshFlow()
//{
// 	qDebug()<<"slotFreshFlow thread:"<<QThread::currentThreadId(); //主线程
//	float flowValue = 0.0;
//	int length = ui.lnEditBigBalance->text().length();
//	if (m_flowcount == 0)
//	{
//		QString a1 = ui.lnEditBigBalance->text();
//		m_flow1 = ui.lnEditBigBalance->text().right(length-1).toFloat();
//		qDebug()<<"start weight: "<<m_flow1;
//	}
//	m_flowcount ++;
//	if (m_flowcount == CALC_FLOW_COUNT) //实际计算频率 = CALC_FLOW_COUNT*TIMEOUT_TEMPER
//	{
//		QString a2 = ui.lnEditBigBalance->text();
//		m_flow2 = ui.lnEditBigBalance->text().right(length-1).toFloat();
//		qDebug()<<"end weight: "<<m_flow2;
//		m_flowcount = 0;
//		flowValue = 3.6*(m_flow2 - m_flow1)*1000/(CALC_FLOW_COUNT*TIMEOUT_TEMPER);
//		float difFlow = m_flow2 - m_flow1;
//		qDebug()<<"delta weight: "<<difFlow;
//		qDebug()<<"flow: "<<flowValue<<"\n";
//		ui.lnEditFlow->setText(QString("%1").arg(flowValue, 6, 'g', 4));
//	}
//}


/************************************************************************/
/* 计算瞬时流量(瞬时法)                                                */
/************************************************************************/
void DataTestDlg::slotFreshFlow()
{
// 	qDebug()<<"slotFreshFlow thread:"<<QThread::currentThreadId(); //主线程
	float flowValue = 0.0;
	/**************************************************************************/
	/* 按照原来程序的逻辑, 那么就丢失了一个δt的质量增量 */
	/**************************************************************************/
	m_flowcount++;
	//if ( m_flowcount == 1)
	//{
	//	QString a1 = ui.lnEditBigBalance->text();
	//	m_flow1 = ui.lnEditBigBalance->text().toFloat();
	//}

	if (m_flowcount == CALC_FLOW_COUNT)//实际计算频率 = CALC_FLOW_COUNT*TIMEOUT_TEMPER
	{
		m_flow2 = ui.lnEditBigBalance->text().toFloat();
		float difFlow = m_flow2 - m_flow1;
		flowValue = 3.6*(difFlow)*1000/(CALC_FLOW_COUNT*TIMEOUT_TEMPER);
		//flowValue = (m_flow2 - m_flow1)*1000/(CALC_FLOW_COUNT*TIMEOUT_TEMPER);// kg/s
		
		m_flowcount = 0;
		ui.lnEditFlow->setText(QString::number(flowValue, 'f', 2));
		m_flow1 = m_flow2;
	}
	
}

/************************************************************************/
/* 计算瞬时流量(累积水量法, 参考老程序的算法)                           */
/************************************************************************/
void DataTestDlg::slotFreshFlow_total()
{
	if (m_totalcount > 4294967290) //防止m_totalcount溢出 32位无符号整数范围0~4294967295
	{
		m_totalcount = 0;
		total_quantity = 0;
	}

	float flowValue = 0.0;
	m_totalcount ++;//计数器累加
	end_quan = ui.lnEditBigBalance->text().toFloat();//取当前天平值, 作为当前运算的终值
	float delta_weight = end_quan - start_quan;

	total_quantity += delta_weight;
	flowValue = 3.6*(total_quantity)*1000/(m_totalcount*TIMEOUT_TEMPER);//总累积水量/总时间  (吨/小时, t/h)
	//flowValue = (total_quantity)*1000/(m_totalcount*TIMEOUT_TEMPER);// kg/s
	ui.lnEditSmallBalance->setText(QString::number(flowValue, 'f', 2)); //暂时在ui.lnEditSmallBalance中显示流量
	start_quan = end_quan;//将当前值保存, 作为下次运算的初值
}

//设置检定状态
void DataTestDlg::on_btnSetVerifyStatus_clicked()
{
	qDebug()<<"设置进入检定状态...";
	m_meterObj->askSetVerifyStatus(); //设置进入检定状态
}

//读表数据
void DataTestDlg::on_btnReadMeterData_clicked()
{
	ui.lnEditMeterNo->clear();
	ui.lnEditMeterTempIn->clear();
	ui.lnEditMeterTempOut->clear();
	ui.lnEditMeterFlow->clear();
	ui.lnEditMeterHeat->clear();
	ui.dateEditMeter->setDate(QDate(2000,1,1));

	qDebug()<<"读表 开始...";
 	m_meterObj->askReadMeter(); //请求读表
}

//修改表号
void DataTestDlg::on_btnModifyMeterNo_clicked()
{
	QString oldMeterNo = ui.lnEditMeterNo->text();
	QString newMeterNo = ui.lnEditNewMeterNo->text();
	if (oldMeterNo.isEmpty())
	{
		QMessageBox::warning(this, tr("Warning"), tr("old meter number is empty! please read meter first!"));
		return;
	}
	if (newMeterNo.isEmpty() || newMeterNo.size()!=2*METER_ADDR_LEN)
	{
		QMessageBox::warning(this, tr("Warning"), tr("new meter number is error!\nplease input 14 bits meter number!"));
		ui.lnEditNewMeterNo->setFocus();
		return;
	}

	m_meterObj->askModifyMeterNo(oldMeterNo, newMeterNo); //请求修改表号
}

//修改流量参数
void DataTestDlg::on_btnModifyFlowPara_clicked()
{
}

//响应读取表号成功
void DataTestDlg::slotFreshMeterNo(const QString& comName, const QString& meterNo)
{
	ui.lnEditMeterNo->setText(meterNo);
	qDebug()<<"读取表号 成功...";
}

//响应读取表流量成功
void DataTestDlg::slotFreshMeterFlow(const QString& comName, const QString& flow)
{
	ui.lnEditMeterFlow->setText(flow);
	qDebug()<<"读取表流量 成功...";
}

//响应读取表热量成功
void DataTestDlg::slotFreshMeterHeat(const QString& comName, const QString& heat)
{
	ui.lnEditMeterHeat->setText(heat);
	qDebug()<<"读取表热量 成功...";
}

//响应读取表日期成功
void DataTestDlg::slotFreshMeterDate(const QString& comName, const QString& date)
{
	ui.dateEditMeter->setDate(QDate::fromString(date, "yyyyMMdd"));
	qDebug()<<"读取表日期 成功...";
}

//响应读取表进出水温度成功
void DataTestDlg::slotFreshMeterTemper(const QString& comName, const QString& tempIn, const QString& tempOut)
{
	ui.lnEditMeterTempIn->setText(tempIn);
	ui.lnEditMeterTempOut->setText(tempOut);
	qDebug()<<"读取表进出水温度 成功...";
}

