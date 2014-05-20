#include <QtGui/QMessageBox>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QTextEdit>

#include "dbshow.h"


DbShow::DbShow(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	dbshowui.setupUi(this);

	connect(dbshowui.conMySqlBtn, SIGNAL(clicked()), this, SLOT(slotConnectMySqlDataBase()));
	connect(dbshowui.QueryBtn, SIGNAL(clicked()), this, SLOT(slotQueryData()));
	dbshowui.QueryBtn->setEnabled(false);

}

DbShow::~DbShow()
{

}

void DbShow::slotConnectMySqlDataBase()
{
	db = QSqlDatabase::addDatabase("QMYSQL"); // 使用mysql数据库驱动 
	db.setHostName("localhost");
	db.setDatabaseName("dbs1"); // 我们之前建立的数据库
	db.setUserName("xopens"); // 我们创建的 xopens 用户名
	db.setPassword("ytdf000"); // xopens 用户的密码
	bool ok = db.open(); // 尝试连接数据库
// 	db.exec("SET NAMES 'GB2312'");
	if(ok) // 这里用xopens已经成功连上数据库
	{
		QMessageBox::information(this, "DbShow", "connect mysql database success !", "Ok", "Cancel");	
		dbshowui.QueryBtn->setEnabled(true);
	}
	else
	{
		QMessageBox::information(this, "DbShow", "connect mysql database failed !", "Ok", "Cancel");	
		dbshowui.QueryBtn->setEnabled(false);
	}
}

void DbShow::slotQueryData()
{
	QSqlQuery query; // 新建一个查询的实例
	if(query.exec("select * from employee")) // 尝试列出 employee 表的所有记录
	{
		// 本次查询成功
		int numRows = 0; // 询问数据库驱动，是否驱动含有某种特性 
		if(db.driver()->hasFeature(QSqlDriver::QuerySize))
		{
			numRows = query.size(); // 如果支持结果影响的行数，那么直接记录下来
		}
		else
		{
			query.last(); //否则定位到结果最后，qt 文档说，这个方法非常慢
			numRows = query.at() + 1;
		}
		QString id, lname, fname, phone; QDateTime dob; 
		dbshowui.display->append("==========================================="); 
		dbshowui.display->append(QString::fromLocal8Bit(" id | 姓名 | 生日 | 电话")); 
		dbshowui.display->append("--------------------------------------");
		while(query.next())// 定位结果到下一条记录
		{ 
			id = query.value(0).toString();
			lname = QString::fromLocal8Bit(query.value(1).toByteArray());
			fname = QString::fromLocal8Bit(query.value(2).toByteArray());
			dob = query.value(3).toDateTime();
			phone = QString::fromLocal8Bit(query.value(4).toByteArray());
			QString result = id + " " + fname + lname + " " + (dob.toString()) + " "+phone;
			dbshowui.display->append(result); 
		}
		dbshowui.display->append("============================================");
		dbshowui.display->append(QString("totally %1 rows").arg( numRows) );
	}
	else  // 如果查询失败，用下面的方法得到具体数据库返回的原因
	{
		QSqlError error = query.lastError();
		dbshowui.display->append("From mysql database: " + error.databaseText());
	}
}