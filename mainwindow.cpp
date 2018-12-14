#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "capture.h"
#include <unistd.h> // FIXME

USBItem* createSampleData()
{
    USBItem *rootItem = new USBItem(new USBPacket(0, QByteArray()));

    USBItem *node = new USBItem(new USBPacket(1, QByteArray::fromHex("555342")), rootItem);

    node->appendChild(new USBItem(new USBPacket(1, QByteArray::fromHex("698218")), node));
    node->appendChild(new USBItem(new USBPacket(2, QByteArray::fromHex("4b5553425310")), node));
    node->appendChild(new USBItem(new USBPacket(3, QByteArray::fromHex("d2")), node));

    rootItem->appendChild(node);

    return rootItem;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->statusPacketNum);

    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::updateAscii);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::updateDetails);

    // loadFile(); // FIXME for dev
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::loadFile);
    connect(ui->actionStart, &QAction::triggered, this, &MainWindow::startCapture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleResults(USBModel *usbModel)
{
    ui->treeView->setModel(usbModel);
}

void MainWindow::startCapture()
{
    CaptureThread *workerThread = new CaptureThread();

    connect(workerThread, &CaptureThread::resultReady, this, &MainWindow::handleResults);
    connect(workerThread, &CaptureThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();

    // ui->statusPacketNum->setText(QString("Records: %1").arg(aggregator.count()));
}

void MainWindow::loadFile()
{
    //QString file = QFileDialog::getOpenFileName(this,
    //    "Open File", "", "*.bin");
    QString file = "../output.bin"; // FIXME for dev

    FILE *in;
    int len;
    char *buf;
    char *data;
    unsigned long long int timestamp;

    in = fopen(file.toUtf8().constData(), "rb");

    USBAggregator aggregator;
    USBModel *usbModel = new USBModel(aggregator.getRoot());

    ui->treeView->setModel(usbModel);

    while(!feof(in)){
        fread(&len, 1, sizeof(int), in);
        buf = static_cast<char *>(malloc(len));
        fread(buf, 1, len, in);

        memcpy(&len, buf, sizeof(int));
        memcpy(&timestamp, buf + sizeof(int), 8);
        data = buf + 12;

        aggregator.append(new USBPacket(timestamp, QByteArray(data, len)));

        free(buf);
    }

    ui->statusPacketNum->setText(QString("Records: %1").arg(aggregator.count()));
}

void MainWindow::updateAscii(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    USBItem *item = static_cast<USBItem*>(index.internalPointer());

    ui->textAsciiPacket->setPlainText(item->asciiPacket());
    ui->textAsciiData->setPlainText(item->asciiData());
}

void MainWindow::updateDetails(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    USBItem *item = static_cast<USBItem*>(index.internalPointer());

    ui->textDetails->setPlainText(item->details());
}
