#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    importfromfile = false;
    importfromCustomSql = false;

    ui->runButton->setEnabled(false);    

    //ui->statusBar->showMessage("Разработка скриптов и программ для ПО Frontol. Email: info@jasposoft.ru");
    localdb = QSqlDatabase::addDatabase("QIBASE");


    localdb.setHostName("localhost");
    localdb.setDatabaseName(QCoreApplication::applicationDirPath()+"//MAIN.FDB");
    localdb.setUserName("SYSDBA");
    localdb.setPassword("masterkey");



        if (!localdb.open())
        {
            qDebug() << localdb.lastError();
            localdb.close();

        }

        model = new QSqlTableModel(this,localdb);
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        model->setTable("DATEBASES");
        model->select();
        model->setHeaderData(0, Qt::Horizontal, tr("ID"));
        model->setHeaderData(1, Qt::Horizontal, tr("Название"));
        model->setHeaderData(2, Qt::Horizontal, tr("IP Адрес"));
        model->setHeaderData(3, Qt::Horizontal, tr("Каталог"));
        model->setHeaderData(4, Qt::Horizontal, tr("Пользователь"));
        model->setHeaderData(5, Qt::Horizontal, tr("Пароль"));


        ui->tableView->setModel(model);
        ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeToContents);
        ui->tableView->hideColumn(0);


    }

    MainWindow::~MainWindow()
    {

        delete ui;

    }

    void MainWindow::on_pushButton_clicked()
    {
        QString filename = QFileDialog::getOpenFileName(this,tr("Выбор файла для импорта"),"",tr("Импорт (*.xch)"));
        ui->PathFileLine->setText(filename);

    }

    void MainWindow::on_ExitButton_clicked()
    {
        this->close();
    }

    void MainWindow::on_applyxchButton_clicked()
    {
        QString filename = ui->PathFileLine->text();
        if (!filename.isEmpty())
        {
            sqlqueries.clear();
            ParserXCH *xch = new ParserXCH();
            xch->OpenFile(filename);
            sqlqueries = xch->GetSQLQueries();
            sqlblobqueries = xch->GetBlobQueries();
            blobdata = xch->GetBlobData();

            ui->statusBar->showMessage("Данные будут загружены из файла: "+ filename);
            ui->runButton->setEnabled(true);
            importfromfile = true;
            importfromCustomSql = false;
        }
    }


    void MainWindow::on_runButton_clicked()
    {
        ui->outEdit->clear();
        ui->progressBar->setValue(0);
        ui->tabWidget->setCurrentIndex(1);
        ui->tabWidget->setTabEnabled(0,false);
        ui->tabWidget->setTabEnabled(2,false);
        ui->tabWidget->setTabEnabled(3,false);

        this->runApply();
        ui->progressBar->setValue(100);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(2,true);
        ui->tabWidget->setTabEnabled(3,true);

    }


    void MainWindow::on_addDB_Button_clicked()
    {
       QSqlQuery q;
       q.exec("INSERT INTO DATEBASES ( NAME, HOST, PATH, USERNAME, PWD) VALUES ('Новая база данных', 'localhost', 'D:\\ATOL\\DB\\', 'SYSDBA', 'masterkey');");
       model->select();
       QModelIndex index = ui->tableView->model()->index(model->rowCount()-1, 1, QModelIndex());
       ui->tableView->setCurrentIndex(index);
       ui->tableView->edit(index);
    }

    void MainWindow::on_pushButton_5_clicked()
    {

        QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
        if (selected.size() > 0)
        {
            for (int i=0;i<selected.size();i++)
            {
                model->removeRow(selected.value(i).row());
            }
            model->select();
        }
    }


    void MainWindow::runApply()
    {
        ui->outEdit->insertHtml("<H3>Запуск обработки!</H3>");

        QString name,host,path,login,password;
        QSqlDatabase targetdb = QSqlDatabase::addDatabase("QIBASE","targetdb");
        QSqlQuery q1(targetdb);
        QSqlQuery q2(targetdb);
        QString err;
        int size = ui->tableView->model()->rowCount();
        float halfPercent=100/size;

        qDebug() << size;

        for (int i=0; i<size;i++)
        {
            name = ui->tableView->model()->data(ui->tableView->model()->index(i,1)).toString();
            host = ui->tableView->model()->data(ui->tableView->model()->index(i,2)).toString();
            path = ui->tableView->model()->data(ui->tableView->model()->index(i,3)).toString();
            path = path+"\\MAIN.GDB";
            login = ui->tableView->model()->data(ui->tableView->model()->index(i,4)).toString();
            password = ui->tableView->model()->data(ui->tableView->model()->index(i,5)).toString();

            targetdb.setHostName(host);
            targetdb.setDatabaseName(path);
            targetdb.setUserName(login);
            targetdb.setPassword(password);

            ui->statusBar->showMessage(name);
            ui->outEdit->insertHtml("<br><p>Подключение к базе данных : "+name+"</p>");
            targetdb.setConnectOptions("ISC_DPB_LC_CTYPE=UNICODE_FFS");
            if (!targetdb.open())
            {
                ui->progressBar->setValue((i+1)*halfPercent);
                err = (targetdb.lastError().nativeErrorCode());
                ui->outEdit->insertHtml("<br><strong>Ошибка при подключении к базе данных! Код ошибки  "+err+"</strong>");
            }
            else
            {
                targetdb.setConnectOptions("ISC_DPB_LC_CTYPE=CP1251");
                for (int j=0;j < sqlqueries.size();j++)
                {
                    q1.clear();
                    if (!sqlqueries.value(j).isEmpty())
                    {
                        if (!q1.exec(sqlqueries.value(j)))
                        {
                            ui->outEdit->insertHtml("<br><strong>"+q1.lastQuery()+"</strong>");
                            ui->outEdit->insertHtml("<br><strong>"+q1.lastError().text()+"</strong>");
                        }
                    }
                }
                targetdb.commit();
                for (int j=0;j < sqlblobqueries.size();j++)
                {
                    q2.clear();
                    q2.prepare(sqlblobqueries.value(j));
                    q2.bindValue(":data",blobdata.value(j));
                    if (!q2.exec())
                    {
                        ui->outEdit->insertHtml("<br><strong>"+q2.lastQuery()+"</strong>");
                        ui->outEdit->insertHtml("<br><strong>"+q2.lastError().text()+"</strong>");
                    }
                }
                targetdb.commit();
                targetdb.close();
                ui->progressBar->setValue((i+1)*halfPercent);
                ui->outEdit->insertHtml("<br>Обновление базы завершено!<br><br>");

            }
        }
    }

void MainWindow::on_buttonExportCSV_clicked()
{
    QSqlQuery q(localdb);
    QString filename = QFileDialog::getSaveFileName(this,"Экспорт списка баз данных","",tr("Файл экспорта (*.txt)"));
    QFile file(filename);
    QTextStream out(&file);
    out.setCodec("CP1251");
    if (file.open(QIODevice::WriteOnly))
    {


    }
    else
    {
        qDebug() << file.errorString();
    }

    if (q.exec("SELECT * FROM DATEBASES"))
    {
        while(q.next())
        {
            out << q.value(1).toString()+";";
            out << q.value(2).toString()+";";
            out << q.value(3).toString()+";";
            out << q.value(4).toString()+";";
            out << q.value(5).toString()+";";
            out << "\r\n";
        }
    }
    else
    {
        qDebug() << "Ошибка: "+q.lastError().text();
    }
    file.close();
}

void MainWindow::on_buttonImportCSV_clicked()
{
    QSqlQuery q(localdb);
    QString filename = QFileDialog::getOpenFileName(this,tr("Импорт списка баз данных"),"",tr("Файл для импорта (*.txt)"));
    QFile file(filename);

    QString line;
    QStringList list;
    QString name,host,dir,user,pwd;
    if (file.open(QIODevice::ReadOnly))
    {
       QTextStream text(&file);
       while(!text.atEnd())
       {

           line = text.readLine();

           list = line.split(";");

           name = list.value(0);
           host =list.value(1);
           dir =list.value(2);
           user =list.value(3);
           pwd =list.value(4);


           q.exec("INSERT INTO DATEBASES ( NAME, HOST, PATH, USERNAME, PWD) VALUES ('"+name+"', '"+host+"', '"+dir+"', '"+user+"', '"+pwd+"');");
       }

    }
    else
    {
        qDebug() << file.errorString() << "Error";
    }
     file.close();
     model->select();
}

void MainWindow::on_applyCustomButton_clicked()
{
    sqlqueries.clear();
    QString text = ui->CustomSQLEdit->toPlainText();
    QStringList list = text.split(";");
    for (int i =0; i<list.size(); i++)
    {

        if (list.value(i) != "" && list.value(i) != "\n")
        {
            qDebug() << list.value(i);
            sqlqueries.append(list.value(i));
        }
    }

    ui->statusBar->showMessage("Будет выполнен произвольный SQL запрос");
    ui->runButton->setEnabled(true);
    importfromfile = false;
    importfromCustomSql = true;
}
