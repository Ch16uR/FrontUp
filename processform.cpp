#include "processform.h"
#include "ui_processform.h"
#include<QDebug>

processForm::processForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::processForm)
{
    ui->setupUi(this);    
    ui->progressBar->setValue(0);
    ui->textEdit->setReadOnly(true);
}

processForm::~processForm()
{
    emit closeWindows();
    delete ui;
}

void processForm::insertText(QString text)
{
   ui->textEdit->insertPlainText(text);
}


void processForm::on_pushButton_clicked()
{
    emit RunSQL();
}
