#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label_2->setText(APP_VERSION);
}

About::~About()
{
    delete ui;
}
