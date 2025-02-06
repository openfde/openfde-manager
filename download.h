#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QTimer>

class FileDownloader : public QObject {
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr) : QObject(parent) {
        manager = new QNetworkAccessManager(this);
    }

    void downloadFile(const QUrl &url, const QString &savePath);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile file;
};

