#ifndef PARSERXCH_H
#define PARSERXCH_H


#include<QStringList>

class ParserXCH
{

public:
     ParserXCH();
    ~ParserXCH();

   void OpenFile(QString);
   QStringList GetSQLQueries();
   QStringList GetBlobQueries();
   QByteArrayList GetBlobData();

private:
   QStringList sqlqueries;      //Список  SQL Запросов
   QStringList sqlblobqueries;      //Список  SQL Запросов для Update Blob
   QByteArrayList sqlblobdata;      //Список BLOB данных

   QString lastline;            //Переменная для получения последней строки для ловли Blob


   void ParseLineForSQL(QString);
   void ParseLineForBlob(QString);
   void ParseStringBlob(QString);
   QByteArray BlobToStr(QString);
   int ord(QChar ch);

signals:

public slots:
};

#endif // PARSERXCH_H
