#include "PhView.h"
#include "ui_phview.h"

#include <QMouseEvent>

PhView::PhView(QWidget *parent) :
    QLabel(parent),
    ui(new Ui::PhView)
{
    ui->setupUi(this);
}

PhView::~PhView()
{
    delete ui;
}

void PhView::moveCenter(int x, int y)
{
    int w, h;

    w = ui->cutRect->rect().width();
    h = ui->cutRect->rect().height();

    ui->cutRect->move(x - w/2, y - h/2);
}
