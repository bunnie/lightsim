#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <QWidget>

class LightSim : public QWidget
{
    Q_OBJECT

public:
    LightSim(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif
