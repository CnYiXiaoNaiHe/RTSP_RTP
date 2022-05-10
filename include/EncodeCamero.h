#pragma once

#include <QtWidgets/QWidget>
#include "ui_EncodeCamero.h"

class EncodeCamero : public QWidget
{
    Q_OBJECT

public:
    EncodeCamero(QWidget *parent = Q_NULLPTR);

private:
    Ui::EncodeCameroClass ui;
};
