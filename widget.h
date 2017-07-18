#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>

#include <QLabel>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void startSearching();
    void wasFinishedPostsGetting(QNetworkReply *reply);
    void wasFinishedPageGetting(QNetworkReply *reply);

private:
    QString requestTemplate;
    QNetworkAccessManager *manager;
    QList<QString> list;
    QString nickName;
    int lastPage = 1;
    int maxPages;
    int postIndex = 0;

    QVBoxLayout *mainLay;

    QHBoxLayout *pagesNumLay;
    QLabel      *pagesNumLab;
    QSpinBox    *pagesNumSpin;

    QHBoxLayout *userNameLay;
    QLabel      *userNameLab;
    QLineEdit   *userNameLine;

    QTextBrowser *linksBrowser;

    QPushButton *startButt;

    QLabel          *progressLab;
    QProgressBar    *progressBar;
};

#endif // WIDGET_H
