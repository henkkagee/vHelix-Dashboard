/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#include "console.h"
#include "boost/algorithm/string/find.hpp"
#include <iostream>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);
    setReadOnly(true);
}

void Console::write_(std::string str)
{
    /*
     *
     *
     * TODO: use HTML QStrings to colour specific lines
     *
     *
     *
    QPalette p = palette();
    boost::iterator_range<std::string::const_iterator> rng;
    if (boost::ifind_first(str, std::string("ERROR"))) {
        p.setColor(QPalette::Text, Qt::red);
    }
    if (boost::ifind_first(str, std::string("Success"))) {
        p.setColor(QPalette::Text, Qt::blue);
    }
    else {
        p.setColor(QPalette::Text, Qt::white);
    }
    setPalette(p);*/
    insertPlainText(str.c_str());
    show();
}
