#include "DevelopmentPlanRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

DevelopmentPlanRepository::DevelopmentPlanRepository() = default;

bool DevelopmentPlanRepository::ensureSchema()
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return false;
    }

    if (schemaEnsured_) {
        return true;
    }

    schemaEnsured_ = true;
    return true;
}

QList<DevelopmentPlanItem> DevelopmentPlanRepository::findByEngineer(
    const QString& engineerId, const QString& status)
{
    QList<DevelopmentPlanItem> result;
    if (!ensureSchema()) {
        return result;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    QString sql =
        "SELECT p.id,p.engineer_id,p.production_area_id,p.machine_id,p.competency_id,"
        "pa.name,m.name,c.name,p.current_score,p.target_score,p.due_date,p.manager_notes,"
        "p.guidance,p.status,p.created_by_user_id,p.created_by_name,p.created_at,p.updated_at "
        "FROM dbo.development_plan_items p "
        "JOIN dbo.production_areas pa ON pa.id=p.production_area_id "
        "JOIN dbo.machines m ON m.id=p.machine_id "
        "JOIN dbo.competencies c ON c.id=p.competency_id "
        "WHERE p.engineer_id=? ";
    if (!status.isEmpty()) {
        sql += "AND p.status=? ";
    }
    sql += "ORDER BY CASE p.status WHEN 'active' THEN 0 WHEN 'completed' THEN 1 ELSE 2 END, "
           "p.due_date,pa.name,m.name,c.name";
    query.prepare(sql);
    query.addBindValue(engineerId);
    if (!status.isEmpty()) {
        query.addBindValue(status);
    }
    if (!query.exec()) {
        lastError_ = query.lastError().text();
        return result;
    }

    while (query.next()) {
        DevelopmentPlanItem item;
        item.id = query.value(0).toInt();
        item.engineerId = query.value(1).toString();
        item.productionAreaId = query.value(2).toInt();
        item.machineId = query.value(3).toInt();
        item.competencyId = query.value(4).toInt();
        item.productionAreaName = query.value(5).toString();
        item.machineName = query.value(6).toString();
        item.competencyName = query.value(7).toString();
        item.currentScore = query.value(8).toInt();
        item.targetScore = query.value(9).toInt();
        item.dueDate = query.value(10).toDate();
        item.managerNotes = query.value(11).toString();
        item.guidance = query.value(12).toString();
        item.status = query.value(13).toString();
        item.createdByUserId = query.value(14).toString();
        item.createdByName = query.value(15).toString();
        item.createdAt = query.value(16).toDateTime();
        item.updatedAt = query.value(17).toDateTime();
        result.append(item);
    }
    return result;
}

bool DevelopmentPlanRepository::create(const DevelopmentPlanItem& item)
{
    if (!ensureSchema()) {
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "IF NOT EXISTS (SELECT 1 FROM dbo.development_plan_items "
        "WHERE engineer_id=? AND competency_id=? AND status='active') "
        "INSERT INTO dbo.development_plan_items "
        "(engineer_id,production_area_id,machine_id,competency_id,current_score,target_score,due_date,"
        "manager_notes,guidance,status,created_by_user_id,created_by_name,created_at,updated_at) "
        "VALUES (?,?,?,?,?,?,?,?,?,'active',?,?,GETDATE(),GETDATE())");
    query.addBindValue(item.engineerId);
    query.addBindValue(item.competencyId);
    query.addBindValue(item.engineerId);
    query.addBindValue(item.productionAreaId);
    query.addBindValue(item.machineId);
    query.addBindValue(item.competencyId);
    query.addBindValue(item.currentScore);
    query.addBindValue(item.targetScore);
    query.addBindValue(item.dueDate.isValid() ? QVariant(item.dueDate) : QVariant());
    query.addBindValue(item.managerNotes);
    query.addBindValue(item.guidance);
    query.addBindValue(item.createdByUserId);
    query.addBindValue(item.createdByName);
    if (!query.exec()) {
        lastError_ = query.lastError().text();
        return false;
    }
    return true;
}

bool DevelopmentPlanRepository::updateStatus(int id, const QString& status)
{
    if (!ensureSchema()) {
        return false;
    }
    if (status != "active" && status != "completed" && status != "cancelled") {
        lastError_ = "Invalid development plan status";
        return false;
    }
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE dbo.development_plan_items SET status=?,updated_at=GETDATE() WHERE id=?");
    query.addBindValue(status);
    query.addBindValue(id);
    if (!query.exec()) {
        lastError_ = query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool DevelopmentPlanRepository::remove(int id)
{
    if (!ensureSchema()) {
        return false;
    }
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM dbo.development_plan_items WHERE id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        lastError_ = query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
