#ifndef READCOMCONFIG_H
#define READCOMCONFIG_H

#include <QtXml/QtXml>

#include "comsetdlg_global.h"
#include "comsetdlg.h"

class COMSETDLG_EXPORT ReadComConfig
{
public:
	ReadComConfig();
	~ReadComConfig();
	QStringList ReadIndexByName(QString ConfigId);//读取配置文件中相应配置的索引值
	/************读取配置文件****************/
	ComInfoStruct ReadValveConfig();
	ComInfoStruct ReadBalanceConfig();
	ComInfoStruct ReadTempConfig();
	ComInfoStruct ReadStdTempConfig();
	ComInfoStruct ReadMeterConfigByNum(QString MeterNum);
	/***************************************************/
private:
	QString ConfigFileName;
	QDomDocument m_doc;
	ComInfoStruct ReadConfigByName(QString ConfigId);
	bool OpenConfigFile();
};

#endif // READCOMCONFIG_H