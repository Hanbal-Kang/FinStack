#include "utils/Formatter.h"

QString Formatter::formatCurrency(double amount)
{
    //Formatting amount only till 2 decilaml places
    QString formatted = QString::number(amount, 'f' , 2);

    //Splitting into two parts (whole part and decimal part)
    QStringList parts =  formatted.split('.');

    QString whole = parts[0];
    QString decimal = parts[1];

    //Amount displayed having comma after every 3 digits from the right
    int commaPos = whole.length() - 3;

    while (commaPos > 0) {
        whole.insert(commaPos , ',');
        commaPos -= 3;

    }
    return ("Rs. " + whole + "." + decimal);
}

QString Formatter::formatDate(const QDateTime& date)
{
    return date.toString("MMM dd, yyyy");
}

//this return "Today" , "Yesturday" or just a short date like (Apr 17) etc
QString Formatter::shortDate(const QDateTime& short_date)
{
    //Todays Date autmatically fetched by QDate
    QDate today = QDate::currentDate();
    QDate yesturday = today.addDays(-1);
    QDate inputDate = short_date.date();

    if(inputDate == today)
        return "Today";

    else if(inputDate == yesturday)
        return "Yesterday";

    else
        return short_date.toString("MMM dd");
}