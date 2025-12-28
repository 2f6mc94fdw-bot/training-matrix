#include "CoreSkillsRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

CoreSkillsRepository::CoreSkillsRepository() : lastError_("") {}
CoreSkillsRepository::~CoreSkillsRepository() {}

QList<CoreSkillCategory> CoreSkillsRepository::findAllCategories()
{
    lastError_.clear();
    QList<CoreSkillCategory> categories;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return categories;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, name, created_at FROM core_skill_categories ORDER BY name");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "findAllCategories failed: " + lastError_);
        return categories;
    }

    while (query.next()) {
        CoreSkillCategory category;
        category.setId(query.value(0).toString());
        category.setName(query.value(1).toString());
        category.setCreatedAt(query.value(2).toDateTime());
        categories.append(category);
    }

    Logger::instance().debug("CoreSkillsRepository", QString("Found %1 core skill categories").arg(categories.size()));
    return categories;
}

QList<CoreSkill> CoreSkillsRepository::findAllSkills()
{
    lastError_.clear();
    QList<CoreSkill> skills;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return skills;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, category_id, name, max_score, created_at FROM core_skills ORDER BY category_id, name");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "findAllSkills failed: " + lastError_);
        return skills;
    }

    while (query.next()) {
        CoreSkill skill;
        skill.setId(query.value(0).toString());
        skill.setCategoryId(query.value(1).toString());
        skill.setName(query.value(2).toString());
        skill.setMaxScore(query.value(3).toInt());
        skill.setCreatedAt(query.value(4).toDateTime());
        skills.append(skill);
    }

    Logger::instance().debug("CoreSkillsRepository", QString("Found %1 core skills").arg(skills.size()));
    return skills;
}

QList<CoreSkillAssessment> CoreSkillsRepository::findAllAssessments()
{
    lastError_.clear();
    QList<CoreSkillAssessment> assessments;
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return assessments;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, category_id, skill_id, score, created_at, updated_at "
                  "FROM core_skill_assessments ORDER BY engineer_id, category_id, skill_id");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "findAllAssessments failed: " + lastError_);
        return assessments;
    }

    while (query.next()) {
        CoreSkillAssessment assessment;
        assessment.setId(query.value(0).toInt());
        assessment.setEngineerId(query.value(1).toString());
        assessment.setCategoryId(query.value(2).toString());
        assessment.setSkillId(query.value(3).toString());
        assessment.setScore(query.value(4).toInt());
        assessment.setCreatedAt(query.value(5).toDateTime());
        assessment.setUpdatedAt(query.value(6).toDateTime());
        assessments.append(assessment);
    }

    Logger::instance().debug("CoreSkillsRepository", QString("Found %1 core skill assessments").arg(assessments.size()));
    return assessments;
}

bool CoreSkillsRepository::saveOrUpdateAssessment(CoreSkillAssessment& assessment)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);

    // Check if assessment already exists (using UNIQUE constraint on engineer_id, category_id, skill_id)
    query.prepare("SELECT id FROM core_skill_assessments "
                  "WHERE engineer_id = ? AND category_id = ? AND skill_id = ?");
    query.addBindValue(assessment.engineerId());
    query.addBindValue(assessment.categoryId());
    query.addBindValue(assessment.skillId());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "saveOrUpdateAssessment check failed: " + lastError_);
        return false;
    }

    if (query.next()) {
        // Update existing assessment
        int existingId = query.value(0).toInt();
        assessment.setId(existingId);

        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE core_skill_assessments SET score = ?, updated_at = GETDATE() "
                           "WHERE id = ?");
        updateQuery.addBindValue(assessment.score());
        updateQuery.addBindValue(existingId);

        if (!updateQuery.exec()) {
            lastError_ = updateQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveOrUpdateAssessment update failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CoreSkillsRepository",
            QString("Updated core skill assessment for engineer %1, skill %2, score %3")
            .arg(assessment.engineerId()).arg(assessment.skillId()).arg(assessment.score()));
        return true;
    } else {
        // Insert new assessment
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO core_skill_assessments "
                           "(engineer_id, category_id, skill_id, score, created_at, updated_at) "
                           "VALUES (?, ?, ?, ?, GETDATE(), GETDATE())");
        insertQuery.addBindValue(assessment.engineerId());
        insertQuery.addBindValue(assessment.categoryId());
        insertQuery.addBindValue(assessment.skillId());
        insertQuery.addBindValue(assessment.score());

        if (!insertQuery.exec()) {
            lastError_ = insertQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveOrUpdateAssessment insert failed: " + lastError_);
            return false;
        }

        // Get the auto-generated ID
        QVariant lastId = insertQuery.lastInsertId();
        if (lastId.isValid()) {
            assessment.setId(lastId.toInt());
        }

        Logger::instance().info("CoreSkillsRepository",
            QString("Created core skill assessment for engineer %1, skill %2, score %3")
            .arg(assessment.engineerId()).arg(assessment.skillId()).arg(assessment.score()));
        return true;
    }
}

bool CoreSkillsRepository::saveCategory(const CoreSkillCategory& category)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);

    // Check if category already exists
    query.prepare("SELECT id FROM core_skill_categories WHERE id = ?");
    query.addBindValue(category.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "saveCategory check failed: " + lastError_);
        return false;
    }

    if (query.next()) {
        // Update existing category
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE core_skill_categories SET name = ? WHERE id = ?");
        updateQuery.addBindValue(category.name());
        updateQuery.addBindValue(category.id());

        if (!updateQuery.exec()) {
            lastError_ = updateQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveCategory update failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CoreSkillsRepository", QString("Updated category: %1").arg(category.name()));
        return true;
    } else {
        // Insert new category
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO core_skill_categories (id, name, created_at) "
                           "VALUES (?, ?, GETDATE())");
        insertQuery.addBindValue(category.id());
        insertQuery.addBindValue(category.name());

        if (!insertQuery.exec()) {
            lastError_ = insertQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveCategory insert failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CoreSkillsRepository", QString("Created category: %1").arg(category.name()));
        return true;
    }
}

bool CoreSkillsRepository::deleteCategory(const QString& categoryId)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM core_skill_categories WHERE id = ?");
    query.addBindValue(categoryId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "deleteCategory failed: " + lastError_);
        return false;
    }

    Logger::instance().info("CoreSkillsRepository", QString("Deleted category: %1").arg(categoryId));
    return true;
}

bool CoreSkillsRepository::saveSkill(const CoreSkill& skill)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);

    // Check if skill already exists
    query.prepare("SELECT id FROM core_skills WHERE id = ?");
    query.addBindValue(skill.id());

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "saveSkill check failed: " + lastError_);
        return false;
    }

    if (query.next()) {
        // Update existing skill
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE core_skills SET category_id = ?, name = ?, max_score = ? WHERE id = ?");
        updateQuery.addBindValue(skill.categoryId());
        updateQuery.addBindValue(skill.name());
        updateQuery.addBindValue(skill.maxScore());
        updateQuery.addBindValue(skill.id());

        if (!updateQuery.exec()) {
            lastError_ = updateQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveSkill update failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CoreSkillsRepository", QString("Updated skill: %1").arg(skill.name()));
        return true;
    } else {
        // Insert new skill
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO core_skills (id, category_id, name, max_score, created_at) "
                           "VALUES (?, ?, ?, ?, GETDATE())");
        insertQuery.addBindValue(skill.id());
        insertQuery.addBindValue(skill.categoryId());
        insertQuery.addBindValue(skill.name());
        insertQuery.addBindValue(skill.maxScore());

        if (!insertQuery.exec()) {
            lastError_ = insertQuery.lastError().text();
            Logger::instance().error("CoreSkillsRepository", "saveSkill insert failed: " + lastError_);
            return false;
        }

        Logger::instance().info("CoreSkillsRepository", QString("Created skill: %1").arg(skill.name()));
        return true;
    }
}

bool CoreSkillsRepository::deleteSkill(const QString& skillId)
{
    lastError_.clear();
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("CoreSkillsRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM core_skills WHERE id = ?");
    query.addBindValue(skillId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("CoreSkillsRepository", "deleteSkill failed: " + lastError_);
        return false;
    }

    Logger::instance().info("CoreSkillsRepository", QString("Deleted skill: %1").arg(skillId));
    return true;
}
