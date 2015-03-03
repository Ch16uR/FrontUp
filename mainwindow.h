#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <parserxch.h>
#include <QtSql>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_ExitButton_clicked();

    void on_applyxchButton_clicked();    

    void on_runButton_clicked();

    void on_addDB_Button_clicked();

    void on_pushButton_5_clicked();
    void on_buttonExportCSV_clicked();

    void on_buttonImportCSV_clicked();

    void on_applyCustomButton_clicked();

signals:


private:
    Ui::MainWindow *ui;

    bool importfromfile;
    bool importfromCustomSql;
    QStringList sqlqueries;
    QStringList sqlblobqueries;
    QStringList blobdata;

    QSqlDatabase localdb;
    QSqlDatabase targetdb;
    QSqlTableModel *model;

    void runApply();
};

#endif // MAINWINDOW_H
