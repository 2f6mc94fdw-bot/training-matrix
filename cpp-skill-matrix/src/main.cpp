#include "core/Application.h"
#include "core/Constants.h"
#include "utils/Logger.h"

#include <QApplication>
#include <QMessageBox>
#include <iostream>

int main(int argc, char* argv[])
{
    try {
        // Get application instance
        Application& app = Application::instance();

        // Initialize application
        if (!app.initialize(argc, argv)) {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }

        // Run application
        int result = app.run();

        // Shutdown
        app.shutdown();

        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        Logger::instance().error("main", QString("Fatal error: %1").arg(e.what()));

        QMessageBox::critical(
            nullptr,
            "Fatal Error",
            QString("A fatal error occurred:\n\n%1\n\nThe application will now exit.").arg(e.what())
        );

        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        Logger::instance().error("main", "Unknown fatal error");

        QMessageBox::critical(
            nullptr,
            "Fatal Error",
            "An unknown fatal error occurred.\n\nThe application will now exit."
        );

        return 1;
    }
}
