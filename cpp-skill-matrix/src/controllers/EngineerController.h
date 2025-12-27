#ifndef ENGINEERCONTROLLER_H
#define ENGINEERCONTROLLER_H

#include "../models/Engineer.h"
#include <QList>
#include <QString>

/**
 * @brief Controller for Engineer business logic
 *
 * Provides high-level operations for managing engineers
 */
class EngineerController
{
public:
    EngineerController();
    ~EngineerController();

    /**
     * @brief Get all engineers
     * @return List of all engineers
     */
    QList<Engineer> getAllEngineers();

    /**
     * @brief Get engineers by shift
     * @param shift Shift name
     * @return List of engineers on the shift
     */
    QList<Engineer> getEngineersByShift(const QString& shift);

    /**
     * @brief Get engineer by ID
     * @param id Engineer ID
     * @return Engineer object, or invalid engineer if not found
     */
    Engineer getEngineerById(const QString& id);

    /**
     * @brief Create a new engineer
     * @param name Engineer name
     * @param shift Shift assignment
     * @return Engineer ID if successful, empty string otherwise
     */
    QString createEngineer(const QString& name, const QString& shift);

    /**
     * @brief Update engineer information
     * @param id Engineer ID
     * @param name New name
     * @param shift New shift
     * @return true if successful
     */
    bool updateEngineer(const QString& id, const QString& name, const QString& shift);

    /**
     * @brief Delete an engineer
     * @param id Engineer ID
     * @return true if successful
     */
    bool deleteEngineer(const QString& id);

    /**
     * @brief Validate engineer data
     * @param name Engineer name
     * @param shift Shift assignment
     * @return true if valid
     */
    bool validateEngineer(const QString& name, const QString& shift);

    /**
     * @brief Get last error message
     * @return Error message from last failed operation
     */
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // ENGINEERCONTROLLER_H
