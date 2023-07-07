#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include <functional>
#include <fstream>

#include "json.hpp"

class QPushButton;
using json = nlohmann::json;
using CallBackFunc = std::function<long(void*)>;

namespace Ui {
class Widget;
}

struct SFuncCall
{
    CallBackFunc cbFunc;
    void*        pParam;
    SFuncCall():pParam(nullptr){}
};

class QuickAccessWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickAccessWidget(QWidget *parent = 0);
    ~QuickAccessWidget();

    json LoadConfigParams();
    void RegistActionFunc(QString strActionName, CallBackFunc func, void* pParam=nullptr);
    void OutputMessage(QString strMsg);

public slots:
    void on_pushButton_run_clicked();
    void on_comboBox_activated(int index);

private:
    void LoadRecentActionParams();
    void UpdateRecentActionCount(const QString& strItemName);
    void PushButtonRectentConnectSigs();

public:
    Ui::Widget *ui;
    QPushButton*                m_pPushButtonList[3];
    bool                        m_bActionItemChanged;
    QMap<QString, SFuncCall>    m_funcReg;
};


#define g_strFilePath_GlobalConfig QString(QApplication::applicationDirPath()+"/config.json")
#define g_strFilePath_Action       QString(QApplication::applicationDirPath()+"/action.json")
#define g_strHalconDescFile        QString(QApplication::applicationDirPath()+"/caltab.descr").toStdString().data()
#define g_strHalconImagePath       QString(QApplication::applicationDirPath()+"/Image.bmp").toStdString().data()

namespace JsonHelper
{
static json ReadJsonObject(const QString strFilePath)
{
    // read json file
    std::ifstream file(strFilePath.toStdString());
    json jsonData;
    file >> jsonData;
    return jsonData;
}

static void WriteJsonObject(const QString strFilePath, const json& jsonData)
{
    std::ofstream file(strFilePath.toStdString());
    file << jsonData.dump(4); // 使用缩进级别4写入JSON数据
}
}

#endif // WIDGET_H
