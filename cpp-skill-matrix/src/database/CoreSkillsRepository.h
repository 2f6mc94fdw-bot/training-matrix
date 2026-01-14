#ifndef CORESKILLSREPOSITORY_H
#define CORESKILLSREPOSITORY_H

#include "../models/CoreSkillCategory.h"
#include "../models/CoreSkill.h"
#include "../models/CoreSkillAssessment.h"
#include <QList>

class CoreSkillsRepository
{
public:
    CoreSkillsRepository();
    ~CoreSkillsRepository();

    QList<CoreSkillCategory> findAllCategories();
    QList<CoreSkill> findAllSkills();
    QList<CoreSkillAssessment> findAllAssessments();
    bool saveOrUpdateAssessment(CoreSkillAssessment& assessment);

    // Category management
    bool saveCategory(const CoreSkillCategory& category);
    bool deleteCategory(const QString& categoryId);

    // Skill management
    bool saveSkill(const CoreSkill& skill);
    bool deleteSkill(const QString& skillId);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // CORESKILLSREPOSITORY_H
