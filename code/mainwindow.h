// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

extern QString hostPathName;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addEntry();
    void loadHostsFile();
    void onSelectionChanged();
    void onSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void deleteHost();


private:
    Ui::MainWindow *ui;
    QTableView *tableView;
    QStandardItemModel *model;
    QLineEdit *ipLineEdit;
    QLineEdit *domainLineEdit;

    void parseHostsFile(const QString &filePath);
    void initConfigSettings();

    QString searchString;
    QString filePathName;

    int colCount;
};


#endif // MAINWINDOW_H
