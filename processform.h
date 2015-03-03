#ifndef PROCESSFORM_H
#define PROCESSFORM_H

#include <QWidget>

namespace Ui {
class processForm;
}

class processForm : public QWidget
{
    Q_OBJECT

public:
    explicit processForm(QWidget *parent = 0);
    ~processForm();
    void insertText(QString);

private:
    Ui::processForm *ui;

signals:
    void closeWindows();
    void RunSQL();
public slots:

private slots:

    void on_pushButton_clicked();
};

#endif // PROCESSFORM_H
