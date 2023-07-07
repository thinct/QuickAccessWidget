#include "QuickAccessWidget.h"
#include "ui_QuickAccessWidget.h"

QuickAccessWidget::QuickAccessWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_bActionItemChanged(false)
{
    ui->setupUi(this);
    m_pPushButtonList[0] = ui->pushButton_1;
    m_pPushButtonList[1] = ui->pushButton_2;
    m_pPushButtonList[2] = ui->pushButton_3;

    LoadRecentActionParams();
    PushButtonRectentConnectSigs();
}

QuickAccessWidget::~QuickAccessWidget()
{
    delete ui;
}

json QuickAccessWidget::LoadConfigParams()
{
    return JsonHelper::ReadJsonObject(g_strFilePath_GlobalConfig);
}

void QuickAccessWidget::LoadRecentActionParams()
{
    const json  jsonObj            = JsonHelper::ReadJsonObject(g_strFilePath_Action);
    const json& recentActionObject = jsonObj["RecentAction"];

    QList<QPair<QString, int>> sortedKeys;
    QMap<QString, int> mapRecentAction;
    // 遍历所有项目
    for (auto it = recentActionObject.begin(); it != recentActionObject.end(); ++it)
    {
        QString actionName = it.key().data();
        int count = it.value().get<int>();
        // qDebug() << actionName << ":" << count;
        mapRecentAction[actionName] = count;

        ui->comboBox->addItem(actionName);
    }

    // 将 QMap 的键值对按值从大到小排序并插入到 QList 中
    for (auto it = mapRecentAction.begin(); it != mapRecentAction.end(); ++it)
    {
        sortedKeys.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedKeys.begin(), sortedKeys.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
        return a.second > b.second; // 降序排列
        });


    for (int i = 0; i < 3; i++)
    {
        m_pPushButtonList[i]->hide();
        if (i < sortedKeys.count())
        {
            m_pPushButtonList[i]->setText(sortedKeys.at(i).first);
            m_pPushButtonList[i]->show();
        }

    }
}

void QuickAccessWidget::OutputMessage(QString strMsg)
{
    ui->plainTextEdit->appendPlainText(strMsg);
}
void QuickAccessWidget::on_comboBox_activated(int index)
{
    m_bActionItemChanged = true;
}

void QuickAccessWidget::RegistActionFunc(QString strActionName, CallBackFunc func, void* pParam)
{
    m_funcReg[strActionName].cbFunc = func;
    m_funcReg[strActionName].pParam = pParam;
}

void QuickAccessWidget::UpdateRecentActionCount(const QString &strItemName)
{
    json  jsonObj = JsonHelper::ReadJsonObject(g_strFilePath_Action);
    json& recentActionObject = jsonObj["RecentAction"];

    bool bFoundFlag = false;
    // 遍历所有项目
    for (auto it = recentActionObject.begin(); it != recentActionObject.end(); ++it)
    {
        QString actionName = it.key().data();
        if (actionName == strItemName)
        {
            bFoundFlag = true;
        }
    }

    if (bFoundFlag)
    {
        int nCurrCount = recentActionObject[strItemName.toStdString()].get<int>();
        recentActionObject[strItemName.toStdString()] = nCurrCount + 1;
    }
    else
    {
        recentActionObject[strItemName.toStdString()] = 1;
    }

    jsonObj["RecentAction"] = recentActionObject;
    JsonHelper::WriteJsonObject(g_strFilePath_Action, jsonObj);
}

void QuickAccessWidget::PushButtonRectentConnectSigs()
{
    for (int i=0; i<3; i++)
    {
        connect(m_pPushButtonList[i], &QPushButton::clicked, this, [this, i](bool){

            QString strItemName = m_pPushButtonList[i]->text();
            if (m_funcReg.contains(strItemName))
            {
                m_funcReg[strItemName].cbFunc(m_funcReg[strItemName].pParam);
            }
        });
    }
}

void QuickAccessWidget::on_pushButton_run_clicked()
{
    QString strItemName = ui->comboBox->currentText();
    if (m_funcReg.contains(strItemName))
    {
        m_funcReg[strItemName].cbFunc(m_funcReg[strItemName].pParam);
        if (m_bActionItemChanged)
        {
            m_bActionItemChanged = false;
            UpdateRecentActionCount(strItemName);
        }
    }
}
