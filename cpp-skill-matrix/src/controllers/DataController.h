#ifndef DATACONTROLLER_H
#define DATACONTROLLER_H

#include <QString>

class DataController
{
public:
    DataController();
    ~DataController();

    /**
     * @brief Generate random assessment scores for all engineers
     * @param percentageToFill Percentage of competencies to fill per engineer (0-100)
     * @return Success message or error
     */
    static QString generateRandomAssessments(int percentageToFill = 70);
};

#endif // DATACONTROLLER_H
