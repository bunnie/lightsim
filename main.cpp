#include <QApplication>

#include "lightsim.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LightSim sim;
    sim.show();
    return app.exec();
}
