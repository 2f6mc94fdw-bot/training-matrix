import React, { useState } from 'react';
import { Plus, Trash2, Award } from 'lucide-react';
import { PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, RadarChart, PolarGrid, PolarAngleAxis, PolarRadiusAxis, Radar } from 'recharts';

const CoreSkills = ({ data, dataHook, currentUser }) => {
  const [activeTab, setActiveTab] = useState('assessment');

  const getCoreSkillScore = (engineerId, categoryId, skillId) => {
    const key = `${engineerId}-${categoryId}-${skillId}`;
    const assessment = data.coreSkills?.assessments?.[key];
    if (typeof assessment === 'object' && assessment !== null) {
      return assessment.score || 0;
    }
    return assessment || 0;
  };

  const updateCoreSkillScore = (engineerId, categoryId, skillId, score) => {
    dataHook.updateCoreSkillAssessment(engineerId, categoryId, skillId, score);
  };

  // Calculate statistics
  const calculateStats = () => {
    if (!data.coreSkills?.categories || !data.engineers) return null;

    let totalScores = 0;
    let maxPossibleScore = 0;
    let scoreDistribution = { 0: 0, 1: 0, 2: 0, 3: 0 };

    data.engineers.forEach(engineer => {
      data.coreSkills.categories.forEach(category => {
        category.skills.forEach(skill => {
          const score = getCoreSkillScore(engineer.id, category.id, skill.id);
          totalScores += score;
          maxPossibleScore += skill.maxScore;
          scoreDistribution[score]++;
        });
      });
    });

    const completionRate = maxPossibleScore > 0
      ? ((totalScores / maxPossibleScore) * 100).toFixed(1)
      : 0;

    return {
      completionRate,
      scoreDistribution,
      totalScores,
      maxPossibleScore
    };
  };

  const stats = calculateStats();

  // Calculate engineer core skills average
  const calculateEngineerAverage = (engineerId) => {
    if (!data.coreSkills?.categories) return 0;

    let total = 0;
    let count = 0;

    data.coreSkills.categories.forEach(category => {
      category.skills.forEach(skill => {
        total += getCoreSkillScore(engineerId, category.id, skill.id);
        count++;
      });
    });

    return count > 0 ? (total / count).toFixed(2) : 0;
  };

  // Pie chart data
  const pieData = stats ? [
    { name: 'Not Trained (0)', value: stats.scoreDistribution[0], color: '#ff6b6b' },
    { name: 'Basic (1)', value: stats.scoreDistribution[1], color: '#fbbf24' },
    { name: 'Competent (2)', value: stats.scoreDistribution[2], color: '#60a5fa' },
    { name: 'Expert (3)', value: stats.scoreDistribution[3], color: '#4ade80' }
  ] : [];

  // Engineer performance data
  const engineerPerformanceData = data.engineers.map(engineer => ({
    name: engineer.name,
    average: parseFloat(calculateEngineerAverage(engineer.id)),
    shift: engineer.shift
  })).sort((a, b) => b.average - a.average);

  // Radar chart data for an engineer
  const getEngineerRadarData = (engineerId) => {
    if (!data.coreSkills?.categories) return [];

    return data.coreSkills.categories.map(category => {
      let total = 0;
      category.skills.forEach(skill => {
        total += getCoreSkillScore(engineerId, category.id, skill.id);
      });
      const average = category.skills.length > 0 ? total / category.skills.length : 0;

      return {
        category: category.name,
        score: parseFloat(average.toFixed(2)),
        fullMark: 3
      };
    });
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="bg-gradient-to-r from-gray-900 to-gray-800 rounded-xl shadow-card p-8 text-white">
        <h1 className="text-3xl font-bold mb-2">Core Engineering Skills</h1>
        <p className="text-gray-300">
          Fundamental skills independent of specific production equipment
        </p>
      </div>

      {/* Sidebar + Content Layout */}
      <div className="flex flex-col lg:flex-row gap-6">
        {/* Sidebar Navigation */}
        <div className="w-full lg:w-64 flex-shrink-0">
          <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-4 lg:sticky lg:top-4">
            <h3 className="text-sm font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider mb-4 px-3 lg:block hidden">
              Core Skills
            </h3>
            <nav className="flex lg:flex-col gap-2 lg:gap-1 overflow-x-auto lg:overflow-x-visible pb-2 lg:pb-0">
              {[
                { id: 'assessment', icon: '📝', label: 'Assessment' },
                { id: 'analytics', icon: '📊', label: 'Analytics' },
                { id: 'management', icon: '⚙️', label: 'Management' }
              ].map(tab => (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`flex-shrink-0 lg:w-full flex items-center gap-3 px-4 py-3 rounded-lg text-left transition-all duration-200 whitespace-nowrap ${
                    activeTab === tab.id
                      ? 'bg-accent text-white shadow-btn font-medium'
                      : 'text-gray-700 dark:text-gray-300 hover:bg-gray-100 dark:hover:bg-gray-700'
                  }`}
                >
                  <span className="text-xl">{tab.icon}</span>
                  <span>{tab.label}</span>
                </button>
              ))}
            </nav>
          </div>
        </div>

        {/* Content Area */}
        <div className="flex-1 min-w-0 space-y-6">

      {/* Assessment Tab */}
      {activeTab === 'assessment' && (
        <div className="space-y-4">
          {data.engineers.map(engineer => {
            // For engineers, only show their own data
            if (currentUser.role === 'engineer' && currentUser.engineerId !== engineer.id) {
              return null;
            }

            return (
              <div key={engineer.id} className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
                <div className="flex justify-between items-center mb-4">
                  <div>
                    <h3 className="text-xl font-bold text-gray-900 dark:text-white">{engineer.name}</h3>
                    <p className="text-sm text-gray-600 dark:text-gray-400">{engineer.shift}</p>
                  </div>
                  <div className="text-right">
                    <p className="text-2xl font-bold text-accent">{calculateEngineerAverage(engineer.id)}</p>
                    <p className="text-xs text-gray-500">Average Score</p>
                  </div>
                </div>

                {data.coreSkills?.categories?.map(category => (
                  <div key={category.id} className="mt-4">
                    <h4 className="font-semibold text-primary-navy dark:text-white mb-3 flex items-center gap-2">
                      <Award size={18} className="text-accent" />
                      {category.name}
                    </h4>
                    <div className="grid grid-cols-1 md:grid-cols-2 gap-3 ml-6">
                      {category.skills.map(skill => {
                        const currentScore = getCoreSkillScore(engineer.id, category.id, skill.id);
                        const canEdit = currentUser.role === 'admin' ||
                          (currentUser.role === 'engineer' && currentUser.engineerId === engineer.id);

                        return (
                          <div key={skill.id} className="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                            <span className="text-sm font-medium text-gray-700 dark:text-gray-300">{skill.name}</span>
                            <div className="flex gap-1">
                              {[0, 1, 2, 3].map(value => (
                                <button
                                  key={value}
                                  onClick={() => {
                                    if (canEdit) {
                                      updateCoreSkillScore(engineer.id, category.id, skill.id, value);
                                    }
                                  }}
                                  disabled={!canEdit}
                                  className={`w-8 h-8 rounded text-sm font-medium transition-colors ${
                                    currentScore === value
                                      ? 'bg-accent text-white'
                                      : !canEdit
                                      ? 'bg-gray-200 dark:bg-gray-600 text-gray-500 cursor-not-allowed'
                                      : 'bg-white dark:bg-gray-800 border-2 border-gray-300 dark:border-gray-600 hover:border-accent'
                                  }`}
                                >
                                  {value}
                                </button>
                              ))}
                            </div>
                          </div>
                        );
                      })}
                    </div>
                  </div>
                ))}
              </div>
            );
          })}
        </div>
      )}

      {/* Analytics Tab */}
      {activeTab === 'analytics' && (
        <div className="space-y-6">
          {/* Overview Stats */}
          {stats && (
            <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-accent">
                <p className="text-sm text-gray-600 mb-1">Completion Rate</p>
                <p className="text-3xl font-bold text-accent">{stats.completionRate}%</p>
              </div>
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-success">
                <p className="text-sm text-gray-600 mb-1">Expert Level</p>
                <p className="text-3xl font-bold text-success">{stats.scoreDistribution[3]}</p>
              </div>
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-info">
                <p className="text-sm text-gray-600 mb-1">Competent Level</p>
                <p className="text-3xl font-bold text-info">{stats.scoreDistribution[2]}</p>
              </div>
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-warning">
                <p className="text-sm text-gray-600 mb-1">Needs Training</p>
                <p className="text-3xl font-bold text-warning">{stats.scoreDistribution[0] + stats.scoreDistribution[1]}</p>
              </div>
            </div>
          )}

          {/* Charts */}
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
            {/* Pie Chart */}
            <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
              <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Score Distribution</h2>
              <ResponsiveContainer width="100%" height={300}>
                <PieChart>
                  <Pie
                    data={pieData}
                    cx="50%"
                    cy="50%"
                    labelLine={false}
                    label={({ name, percent }) => `${name.split(' ')[0]}: ${(percent * 100).toFixed(0)}%`}
                    outerRadius={80}
                    fill="#8884d8"
                    dataKey="value"
                  >
                    {pieData.map((entry, index) => (
                      <Cell key={`cell-${index}`} fill={entry.color} />
                    ))}
                  </Pie>
                  <Tooltip />
                </PieChart>
              </ResponsiveContainer>
            </div>

            {/* Bar Chart */}
            <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
              <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Engineer Performance</h2>
              <ResponsiveContainer width="100%" height={300}>
                <BarChart data={engineerPerformanceData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="name" angle={-45} textAnchor="end" height={100} />
                  <YAxis domain={[0, 3]} />
                  <Tooltip />
                  <Bar dataKey="average" fill="#ff6b6b" name="Average Score" />
                </BarChart>
              </ResponsiveContainer>
            </div>
          </div>

          {/* Individual Engineer Radar Charts */}
          <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
            <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Skill Profiles by Engineer</h2>
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
              {data.engineers.slice(0, 6).map(engineer => (
                <div key={engineer.id} className="border border-gray-200 dark:border-gray-700 rounded-lg p-4">
                  <h3 className="font-semibold text-gray-900 dark:text-white mb-2 text-center">{engineer.name}</h3>
                  <ResponsiveContainer width="100%" height={200}>
                    <RadarChart data={getEngineerRadarData(engineer.id)}>
                      <PolarGrid />
                      <PolarAngleAxis dataKey="category" tick={{ fontSize: 10 }} />
                      <PolarRadiusAxis domain={[0, 3]} />
                      <Radar name="Score" dataKey="score" stroke="#ff6b6b" fill="#ff6b6b" fillOpacity={0.6} />
                    </RadarChart>
                  </ResponsiveContainer>
                </div>
              ))}
            </div>
          </div>
        </div>
      )}

      {/* Management Tab */}
      {activeTab === 'management' && currentUser.role === 'admin' && (
        <div className="space-y-6">
          <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
            <div className="flex justify-between items-center mb-4">
              <h2 className="text-xl font-bold text-gray-900 dark:text-white">Manage Core Skills</h2>
              <button
                onClick={() => {
                  const name = prompt('Enter category name:');
                  if (name) {
                    dataHook.addCoreSkillCategory(name);
                  }
                }}
                className="flex items-center gap-2 px-4 py-2 bg-accent text-white rounded-lg hover:bg-accent-light shadow-md"
              >
                <Plus size={20} /> Add Category
              </button>
            </div>

            {data.coreSkills?.categories?.map(category => (
              <div key={category.id} className="mb-6 border-2 border-gray-200 dark:border-gray-700 rounded-lg p-4">
                <div className="flex justify-between items-center mb-3">
                  <h3 className="text-lg font-bold text-primary-navy dark:text-white">{category.name}</h3>
                  <button
                    onClick={() => {
                      const skillName = prompt(`Add skill to "${category.name}":`);
                      if (skillName) {
                        dataHook.addCoreSkill(category.id, skillName);
                      }
                    }}
                    className="flex items-center gap-2 px-3 py-1 bg-info text-white rounded text-sm hover:bg-blue-500"
                  >
                    <Plus size={16} /> Add Skill
                  </button>
                </div>

                <div className="space-y-2">
                  {category.skills.map(skill => (
                    <div key={skill.id} className="flex justify-between items-center p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                      <span className="text-sm font-medium text-gray-700 dark:text-gray-300">{skill.name}</span>
                      <button
                        onClick={() => {
                          if (confirm(`Delete "${skill.name}"?`)) {
                            dataHook.deleteCoreSkill(category.id, skill.id);
                          }
                        }}
                        className="p-2 text-accent hover:bg-red-50 dark:hover:bg-red-900 rounded"
                      >
                        <Trash2 size={16} />
                      </button>
                    </div>
                  ))}
                  {category.skills.length === 0 && (
                    <p className="text-sm text-gray-500 dark:text-gray-400 italic">No skills yet. Click "Add Skill" above.</p>
                  )}
                </div>
              </div>
            ))}
          </div>
        </div>
      )}
        </div> {/* End Content Area */}
      </div> {/* End Flex Container */}
    </div>
  );
};

export default CoreSkills;
