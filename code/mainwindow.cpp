#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QDir>

QString hostPathName = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    tableView = new QTableView(this);
    ipLineEdit = new QLineEdit(this);
    domainLineEdit = new QLineEdit(this);
    QPushButton *addButton = new QPushButton("添加", this);
    QPushButton *deleteButton = new QPushButton("删除", this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(new QLabel("IP地址:", this));
    inputLayout->addWidget(ipLineEdit);
    inputLayout->addWidget(new QLabel("域名:", this));
    inputLayout->addWidget(domainLineEdit);
    inputLayout->addWidget(addButton);
    inputLayout->addWidget(deleteButton);

    model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal, "IP地址");
    model->setHeaderData(1, Qt::Horizontal, "域名");

    colCount = 2;
    tableView->setModel(model);
    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 300);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(tableView);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addEntry);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteHost);
    connect(model, &QStandardItemModel::dataChanged, this, &MainWindow::onSelectionChanged);
    connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::onSelection);

    initConfigSettings();

    filePathName = hostPathName;
    loadHostsFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addEntry()
{
    QString ip = ipLineEdit->text().trimmed();
    QString domain = domainLineEdit->text().trimmed();
    if (!ip.isEmpty() && !domain.isEmpty()) {
       for (int row = 0; row < colCount; ++row) {
           QStandardItem *item = model->item(row, 1);
           if (item && domain == item->text()) {
               QMessageBox::warning(this, "错误", "域名已经存在！");
               return;
           }
       }
        QStandardItem *ipItem = new QStandardItem(ip);
        QStandardItem *domainItem = new QStandardItem(domain);
        model->appendRow({ipItem, domainItem});
    }

    QFile file(filePathName);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << ip << " " << domain << "\n";

        file.close();
        QMessageBox::warning(this, "成功", "写入成功");
    } else {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入：");
    }
}

void MainWindow::loadHostsFile()
{
    int rowCount = model->rowCount();
    model->removeRows(0, rowCount);
    QFile file(filePathName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误：", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith('#')) {
            QStringList parts = line.split(QRegExp("\\s+"));
            if (parts.size() >= 2) {
                QStandardItem *ipItem = new QStandardItem(parts.first());
                QString domain = parts.mid(1).join(" ");
                QStandardItem *domainItem = new QStandardItem(domain);
                model->appendRow({ipItem, domainItem});
            }
        }
    }
    file.close();
}

void MainWindow::onSelectionChanged()
{

       QModelIndexList selectedIndexes = tableView->selectionModel()->selectedIndexes();
       QString replaceString = "";

       foreach (const QModelIndex &index, selectedIndexes) {
           if (index.isValid()) {
               for(int i = 0; i < colCount;i++){
                   replaceString += model->item(index.row(), i)->text();
                   if(i != colCount-1)replaceString += " ";
               }
           }
       }

       QFile file(filePathName);
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
           QMessageBox::warning(this, "错误", "无法打开文件："+filePathName);
           return;
       }
       QTextStream in(&file);
       QStringList lines;
       QString line;
       bool found = false; // 标记是否已找到并替换了第一个匹配项
       while (!in.atEnd()) {
           line = in.readLine();
           if (!found && line.contains(searchString)) {
               line.replace(searchString, replaceString, Qt::CaseSensitive);
               found = true;
           }
           lines.append(line);
       }
       file.close();

       if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
           QMessageBox::warning(this, "错误", "无法打开HOSTS文件以写入");

           return;
       }
       QTextStream out(&file);
       for(QString l:lines){
           out << l << "\n";
       }
       QMessageBox::warning(this, "成功", "写入完成");

       file.close();

}

void MainWindow::onSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    QModelIndexList selectedIndexes = tableView->selectionModel()->selectedIndexes();

    QString data = "";

    foreach (const QModelIndex &index, selectedIndexes) {
        if (index.isValid()) {
            for(int i = 0; i < colCount;i++){
                data += model->item(index.row(), i)->text();
                if(i != colCount-1)data += " ";
            }
        }
    }
    searchString = data;
}

void MainWindow::deleteHost()
{
    QFile file(filePathName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行读写：" + file.errorString());
        return;
    }

    QStringList lines;
    QTextStream in(&file);
    QString line;
    bool found = false;
    QString domainToDelete = domainLineEdit->text().trimmed();
    // 读取文件内容到QStringList中
    while (!in.atEnd()) {
        line = in.readLine();

        if(line.startsWith('#') || line.startsWith("\t") || line.isEmpty()){
            lines.append(line);
            continue;
        }

        if (line.split(' ')[1] != domainToDelete) {
            lines.append(line); // 只保留不包含要删除域名的行
        } else {
            found = true; // 标记已找到要删除的行
        }

    }
    // 如果文件指针不在文件末尾，我们需要将其移回文件开头以准备写入
    if (!file.seek(0)) {
        QMessageBox::critical(this, "错误", "无法将文件指针移回文件开头：" + file.errorString());
        file.close();
        return;
    }

    // 清空文件内容
    if (!file.resize(0)) {
        QMessageBox::critical(this, "错误", "无法清空文件内容：" + file.errorString());
        file.close();
        return;
    }

    // 将修改后的内容写回文件
    QTextStream out(&file);
    foreach (const QString &modifiedLine, lines) {
        out << modifiedLine << "\n";
    }

    file.close();

    if (found) {
        QMessageBox::information(this, "成功", "域名已从文件中删除！");
    } else {
        QMessageBox::warning(this, "错误", "未找到指定的域名！");
    }
    loadHostsFile();
}

void MainWindow::initConfigSettings()
{
    QString fileName = "config.ini";
    QString appPath = QCoreApplication::applicationDirPath();
    QString configPath = appPath + '/' + fileName;
    QFile file(configPath);
    if(!file.exists()){
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "没有config文件且无法创建");
            return;
        }
        QTextStream out(&file);
        out << "[settings]" <<"\n";
        out << "filePathName = C:/Windows/System32/drivers/etc/HOSTS.txt" <<"\n";

    }

    file.close();
    QSettings settings(configPath, QSettings::IniFormat);
    hostPathName = settings.value("settings/filePathName").toString();
}
