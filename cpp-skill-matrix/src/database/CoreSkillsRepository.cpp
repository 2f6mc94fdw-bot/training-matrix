#include "CoreSkillsRepository.h"

CoreSkillsRepository::CoreSkillsRepository() : lastError_("") {}
CoreSkillsRepository::~CoreSkillsRepository() {}

QList<CoreSkillCategory> CoreSkillsRepository::findAllCategories() { return QList<CoreSkillCategory>(); }
QList<CoreSkill> CoreSkillsRepository::findAllSkills() { return QList<CoreSkill>(); }
QList<CoreSkillAssessment> CoreSkillsRepository::findAllAssessments() { return QList<CoreSkillAssessment>(); }
bool CoreSkillsRepository::saveOrUpdateAssessment(CoreSkillAssessment& assessment) { return false; }
