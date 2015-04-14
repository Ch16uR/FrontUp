#include "parserxch.h"
#include <QFile>
#include <QTextStream>

ParserXCH::ParserXCH()
{

}

ParserXCH::~ParserXCH()
{
    delete this;
}
QStringList ParserXCH::GetSQLQueries()
{
    return sqlqueries;
}

QStringList ParserXCH::GetBlobQueries()
{
    return sqlblobqueries;
}

QByteArrayList ParserXCH::GetBlobData()
{
    return sqlblobdata;
}

void ParserXCH::OpenFile(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << file.error();
    }
    else
    {
        //qDebug() << "Файл успешно открыт!";
    }
    QTextStream text(&file);
    QString line;
    int i = 0;
    while(!text.atEnd())
    {
        line = text.readLine();
        if (i > 2)
        {
            this->ParseLineForSQL(line);
            this->ParseLineForBlob(line);
        }

        i++;

    }
    file.close();
}

void ParserXCH::ParseLineForSQL(QString line)
{
    QStringList list;
    if (!line.indexOf("INSERT") || !line.indexOf("DELETE") || !line.indexOf("UPDATE") || !line.indexOf("EXECUTE") || !line.indexOf("SET"))
    {
         list = line.split(";");
        for (int i =0; i<list.size(); i++)
        {
           if (list.value(i) != "" || list.value(i) != "\n")
               sqlqueries.append(list.value(i));
        }
    }
}

void ParserXCH::ParseLineForBlob(QString line)
{
    if (lastline == "/*" && line != "*/" && line != "")
        this->ParseStringBlob(line);
    lastline = line;
}

void ParserXCH::ParseStringBlob(QString line)
{

    QStringList list = line.split(",");

    int j=0;
    QString blobtable;
    QString blobfield;
    QString blobid;
    QByteArray blobdata;
    QString blobtemp;

    for (int i=0;i<list.size();i++)
    {
         if (j == 0)
            blobtable = list.value(i);
         if (j == 1)
             blobfield = list.value(i);
         if (j == 2)
             blobid = list.value(i);
         if (j == 3)
         {
             if ((blobtable == "WINREPORTS") || (blobtable == "PRICELABEL"))
             {
                  blobdata = this->BlobToStr(list.value(i));
             }
             else
             {
                 blobtemp = QString::fromLocal8Bit(this->BlobToStr(list.value(i)));
                 blobdata = blobtemp.toUtf8();
             }

             sqlblobqueries.append("UPDATE "+blobtable+" SET "+blobfield+" = :data WHERE ID = "+blobid);
             sqlblobdata.append(blobdata);
             j =-1;
         }

        j++;
    }
}

QByteArray ParserXCH::BlobToStr(QString line)
{

    QByteArray arr;    
    int num_ch;
    char ch;


    for (int i=0; i<line.length();i=i+2)
    {
         num_ch = (ord(line.at(i)) - ord('A'))*16 + (ord(line.at(i+1)) - ord('A'));
         ch = (char) num_ch;
         arr.append(ch);
    }

    //return QByteArray::fromLocal8Bit(arr);

    return arr;


}

int ParserXCH::ord(QChar ch)
{
    //return int(ch.toLatin1());
    return int(ch.toLatin1());
}
