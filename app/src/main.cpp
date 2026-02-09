#include <Core/Core.hpp> // for Core

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    revise::Core core(app);
    return core.run();
}
