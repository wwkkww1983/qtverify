#ifndef ADJUSTRATEDLG_H
#define ADJUSTRATEDLG_H

#ifdef ADJUSTRATEDLG_DLL
#  ifdef WIN32
#  define ADJUSTRATEDLG_EXPORT __declspec(dllexport)
#  else
#  define ADJUSTRATEDLG_EXPORT
#  endif
#else
#  ifdef WIN32
#  define ADJUSTRATEDLG_EXPORT __declspec(dllimport)
#  else
#  define ADJUSTRATEDLG_EXPORT
#  endif
#endif

#include <QtGui/QWidget>
#include <QSettings>
#include "ui_adjustratedlg.h"
#include "comobject.h"
#include "qtexdb.h"
#include "basedef.h"

class ParaSetReader;
class ParaSetDlg;
class ReadComConfig;

/*
** ������AdjustRateDlg
** ���ܣ���������
*/

#define PRECISION (0.03*m_targetRate)//�����趨�����
#define BIG_RBTN 0
#define MID_RBTN 1
#define GAIN_TARGET_TIMES (10)//����ﵽĿ������޵Ĵ������ڴ���, ����Ϊ�����ɹ�, ֹͣ��������
#define ONE_MINUTES	(60000)
#define ADJUST_MINUTES 10//���������õ���ʱ��
#define PUMP_FREQ_MIN 20 //ˮ����������ʱ����СƵ��
#define PUMP_FREQ_MAX 49 //ˮ����������ʱ�����Ƶ��
#define DELETE_RESOURCE(Pointer)	if (NULL != Pointer)\
									{\
										delete Pointer;\
										Pointer = NULL;\
									}

class ADJUSTRATEDLG_EXPORT AdjustRateDlg : public QWidget
{
	Q_OBJECT

public:
	AdjustRateDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~AdjustRateDlg();

	ParaSetReader *m_paraSetReader;
	ParaSetDlg *m_paraSetDlg;

	ReadComConfig *m_readComConfig; //����������


	ComThread m_valveThread;   //���ſ����߳�
	ControlComObject *m_controlObj;

	QMap<int, bool> m_valveStatus;//<���Ŷ˿ںţ�����״̬>
	QMap<int, QPushButton*> m_valveBtn;	//<���Ŷ˿ںţ����Ű�ť>
	int m_nowPortNo;	//��ǰ���Ʒ��Ŷ˿ں�

	QMap<int, bool> m_regStatus; //<���ڷ��˿ںţ����ڷ�״̬>
	QMap<int, QPushButton*> m_regBtn;//<���ڷ��˿ںţ����ڷ���ť>	
	int m_nowRegNo;  //��ǰ���ڷ��˿ں�

	PortSet_Ini_STR m_portsetinfo; //�˿�����

	void closeEvent(QCloseEvent * event);
	void showEvent(QShowEvent *event);
	void initControlCom();        //���ſ��ƴ���
	void initValveStatus();	   //��ʼ������״̬
	void initRegulateStatus(); //��ʼ�����ڷ�״̬

public slots:
	void on_btnWaterIn_clicked(); //��ˮ��
	void on_btnWaterOut_clicked();//��ˮ��
	void on_btnValveBig_clicked(); //��������
	void on_btnValveMiddle1_clicked(); //����һ
	void on_btnValveMiddle2_clicked(); //������
	void on_btnValveSmall_clicked(); //С������
	void on_btnWaterPump_clicked(); //ˮ��

	void on_btnSetFreq_clicked(); //����Ƶ��
	void on_btnRegulate1_clicked();
	void on_btnRegulate2_clicked();

	void on_btnParaSet_clicked();
	void on_btnExit_clicked();

	void slotSetValveBtnStatus(const UINT8 &portno, const bool &status); //�̵������سɹ���Ӧ�Ĳۺ���
	void slotSetRegulateOk();

	void setValveBtnBackColor(QPushButton *btn, bool status); //���÷��Ű�ť����ɫ
	void setRegBtnBackColor(QPushButton *btn, bool status);	//���õ��ڷ���ť����ɫ

	void on_btnStartSet_clicked();
	void on_btnSopSet_clicked();

	void on_lnEditKp_big_returnPressed();
	void on_lnEditKi_big_returnPressed();
	void on_lnEditKd_big_returnPressed();
	void on_lnEditCycleTime_big_returnPressed();
	void on_lnEditMaxRate_big_returnPressed();
	void on_lnEditTargetRate_big_returnPressed();

	void on_lnEditKp_mid_returnPressed();
	void on_lnEditKi_mid_returnPressed();
	void on_lnEditKd_mid_returnPressed();
	void on_lnEditCycleTime_mid_returnPressed();
	void on_lnEditMaxRate_mid_returnPressed();
	void on_lnEditTargetRate_mid_returnPressed();

	void on_lnEditKp_pump_returnPressed();
	void on_lnEditKi_pump_returnPressed();
	void on_lnEditKd_pump_returnPressed();
	void on_lnEditCycleTime_pump_returnPressed();

private:
	Ui::AdjustVelocityDlgClass ui;
	/*******************��׼������******************************/
	void initInstStdCom();//˲ʱ�������ڳ�ʼ��
	void initAccumStdCom();//�ۻ��������ڳ�ʼ��

	uchar m_accumDevAddress;//��ǰ�ۻ������ɼ���ʹ�õ�����ģ���豸��ַ, Ĭ��Ϊ0x01
	uchar m_instDevAddress;//��ǰ˲ʱ�����ɼ���ʹ�õ�����ģ���豸��ַ, Ĭ��Ϊ0x01

	lcModRtuComObject *m_instantFlowCom;//˲ʱ�������ڶ���
	ComThread m_instantFlowThread;//˲ʱ�����ɼ��߳�
	QTimer* m_instSTDMeterTimer;//˲ʱ������ʱ��
	QByteArray m_instStdPulse;//˲ʱ��������ֵ, ����μӹ�

	lcModRtuComObject *m_accumulateFlowCom;//�ۻ��������ڶ���
	ComThread m_accumFlowThread;//�ۻ������ɼ��߳�
	QTimer* m_accumSTDMeterTimer;//�ۻ�������ʱ��
	QByteArray m_accumStdPulse;//16·�ۻ���������ֵ, ����μӹ�

	QList<int> m_instRouteIsRead;//˲ʱ������ͨ�����Ƿ񱻲ɼ���
	QList<int> m_accumRouteIsRead;//�ۻ�������ͨ�����Ƿ񱻲ɼ���
	QSettings *m_stdParam;//��ȡ��׼������

	int getRouteByWdg(flow_rate_wdg, flow_type);//���ݲ����Ŷ�ȡ��׼����ͨ����
	float getStdUpperFlow(flow_rate_wdg wdgIdx);//���ݲ����Ŷ�ȡ��Ӧ��׼������������ֵ
	double getStdPulse(flow_rate_wdg wdgIdx);//���ݲ����Ŷ�ȡ��Ӧ��׼��������ֵ

	void freshInstStdMeter();//ˢ��˲ʱ����
	void freshAccumStdMeter();//ˢ���ۻ�����

	float getInstFlowRate(flow_rate_wdg idx);
	float getAccumFLowVolume(flow_rate_wdg idx);

	/******************��׼������end***************************/

	/*******************�綯���ڷ�******************************/
	void initAdjustParams();
	QSettings *m_pidConfig;
	QButtonGroup *m_btnGroupValve;

	QRegExp m_rx;
	QTimer *m_setRegularTimer;
	
	QTime m_elapsetime;//��ʱ��, ��¼���趨��ʼ����ǰ������ʱ��
	int m_gainPreciseTimes;//�ﵽĿ�������Ĵ���

	float m_pre_error;
	float m_curr_error;
	float m_integral;
	int m_degree;
	int m_openRegulateTimes;

	float m_Kp;
	float m_Ki;
	float m_Kd;
	int m_pickCycleTime;//�趨����
	bool m_ifGainTargetRate;//�Ƿ��ѴﵽĿ������ֵ
	QString m_timeStamp;
	float m_currentRate;
	float m_targetRate;
	float m_maxRate;

	PIDDataPtr m_pidDataPtr;

	void setRegulate(float currentRate, float targetRate);
	void degreeGet(float currentRate, float targetRate);
	void stopSetRegularTimer();
	void savePidParams();
	void installPidParams();
	void initLineEdits();
	void initBtnGroup();
	void forbidInputParams();
	void enableInputParams();
	/******************�綯���ڷ�end************************/

	/******************����ˮ��***************************/
	QTimer *m_setPumpTimer;
	float m_pumpKp;
	float m_pumpKi;
	float m_pumpKd;
	int m_pumpCycleTime;

	int m_pumpFreq;

	float m_prePumpErr;
	float m_curPumpErr;
	float m_pumpIntegral;

	void freqGet(float, float);
	/******************����ˮ��end************************/


private slots:
	/*******************��׼������******************************/
	void slotAskInstPulse();//����˲ʱ����
	void slotAskAccumPulse();//�����ۻ�����

	void slotGetInstStdMeterPulse(const QByteArray &);//˲ʱ�����ۺ���
	void slotGetAccumStdMeterPulse(const QByteArray &);//�ۻ������ۺ���
	/******************��׼������end***************************/

	/*******************�綯���ڷ�******************************/
	void slotSetRegulate();
	void slotSetPumpFreq();

	void openPump();
	void closePump();
	int openValve(UINT8 portno);
	int closeValve(UINT8 portno);
	void operateBigPidVales();
	void operateMidPidVales();
	void slotValveClicked(int);
	/******************�綯���ڷ�end***************************/


};

#endif // ADJUSTRATEDLG_H