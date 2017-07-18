#include "widget.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFile>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Pikabu searcher"));

    mainLay = new QVBoxLayout(this);
    mainLay->setAlignment(Qt::AlignCenter);

    pagesNumLay = new QHBoxLayout(this);
    pagesNumLab = new QLabel(tr("Pages num"), this);
    pagesNumSpin = new QSpinBox(this);
    pagesNumSpin->setValue(5);
    pagesNumLay->addWidget(pagesNumLab);
    pagesNumLay->addWidget(pagesNumSpin);

    userNameLay = new QHBoxLayout(this);
    userNameLab = new QLabel(tr("User name"), this);
    userNameLine = new QLineEdit(this);
    userNameLay->addWidget(userNameLab);
    userNameLay->addWidget(userNameLine);

    linksBrowser = new QTextBrowser(this);
    linksBrowser->setAcceptRichText(true);
    linksBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction);
    linksBrowser->setOpenExternalLinks(true);

    startButt = new QPushButton(tr("start"), this);
    connect(startButt, SIGNAL(clicked(bool)), this, SLOT(startSearching()));

    progressLab = new QLabel("Progress");
    progressLab->setAlignment(Qt::AlignCenter);
    progressBar = new QProgressBar(this);
    progressBar->setAlignment(Qt::AlignCenter);

    mainLay->addLayout(pagesNumLay);
    mainLay->addLayout(userNameLay);
    mainLay->addWidget(linksBrowser);
    mainLay->addWidget(startButt);
    mainLay->addWidget(progressLab);
    mainLay->addWidget(progressBar);

    requestTemplate = "http://pikabu.ru/hot";
}

Widget::~Widget()
{

}

void Widget::startSearching()
{
    startButt->setDisabled(true);
    maxPages = pagesNumSpin->value();
    nickName = userNameLine->text();
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(wasFinishedPostsGetting(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(requestTemplate.toLatin1())));
}

void Widget::wasFinishedPostsGetting(QNetworkReply *reply)
{
    QString data = reply->readAll();
    progressLab->setText(tr("Getting posts list"));
    progressBar->setMaximum(maxPages-1);
    progressBar->setValue(lastPage);

    if(data.contains("Отличная работа, все прочитано!"))
    {
        return;
    }

    QList<QString> tempList = data.split(QRegularExpression("_end-->"));
    tempList.removeFirst();

    for(int i = 0; i < tempList.length(); i++)
    {
        int temp = tempList.at(i).indexOf("http://pikabu.ru/story/");
        tempList[i].remove(0, temp);
        temp = tempList.at(i).indexOf("\"");
        tempList[i].remove(temp, tempList.at(i).length()-temp);
    }

    list.append(tempList);

    lastPage++;
    if(lastPage<maxPages)
    {
        manager->get(QNetworkRequest(QUrl((requestTemplate+"?page="+QString::number(lastPage)).toLatin1())));
    }
    else
    {
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(wasFinishedPostsGetting(QNetworkReply*)));
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(wasFinishedPageGetting(QNetworkReply*)));
        manager->get(QNetworkRequest(QUrl(list.at(postIndex++))));
    }
}

void Widget::wasFinishedPageGetting(QNetworkReply *reply)
{
    progressLab->setText(tr("Checking posts"));
    progressBar->setMaximum(list.length()-1);
    progressBar->setValue(postIndex);
    if(reply->readAll().contains(nickName.toLatin1()))
    {
        linksBrowser->append("<a href=\"" + list.at(postIndex-1) + "\">" + list.at(postIndex-1) + "</a>");
        linksBrowser->append("--------------------------------");
    }
    if(postIndex<list.length())
    {
        manager->get(QNetworkRequest(QUrl(list.at(postIndex++))));
    }
    else
    {
        progressLab->setText(tr("finished"));
        lastPage = 1;
        postIndex = 0;
        list.clear();
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(wasFinishedPageGetting(QNetworkReply*)));
        startButt->setEnabled(true);
    }
}
