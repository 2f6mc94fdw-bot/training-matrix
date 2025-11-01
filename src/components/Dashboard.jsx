import React from 'react';
import { PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { Users, Target, TrendingUp, Award, AlertTriangle } from 'lucide-react';

const Dashboard = ({ data }) => {
  // Calculate statistics
  const totalEngineers = data.engineers.length;
  const totalCompetencies = data.productionAreas.reduce((total, area) => {
    return total + area.machines.reduce((machineTotal, machine) => {
      return machineTotal + machine.competencies.length;
    }, 0);
  }, 0);

  // Get all assessment scores
  const allScores = [];
  data.engineers.forEach(engineer => {
    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        machine.competencies.forEach(comp => {
          const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
          const assessment = data.assessments[key];
          const score = typeof assessment === 'object' ? (assessment?.score || 0) : (assessment || 0);
          allScores.push({ engineerId: engineer.id, score, maxScore: comp.maxScore });
        });
      });
    });
  });

  // Calculate score distribution
  const scoreDistribution = {
    notTrained: allScores.filter(s => s.score === 0).length,
    basic: allScores.filter(s => s.score === 1).length,
    competent: allScores.filter(s => s.score === 2).length,
    expert: allScores.filter(s => s.score === 3).length
  };

  const totalAssessments = allScores.length;
  const scorePercentages = {
    notTrained: ((scoreDistribution.notTrained / totalAssessments) * 100).toFixed(1),
    basic: ((scoreDistribution.basic / totalAssessments) * 100).toFixed(1),
    competent: ((scoreDistribution.competent / totalAssessments) * 100).toFixed(1),
    expert: ((scoreDistribution.expert / totalAssessments) * 100).toFixed(1)
  };

  // Average skill level
  const averageScore = (allScores.reduce((sum, s) => sum + s.score, 0) / totalAssessments).toFixed(2);

  // Pie chart data
  const pieData = [
    { name: 'Not Trained (0)', value: scoreDistribution.notTrained, color: '#ff6b6b' },
    { name: 'Basic (1)', value: scoreDistribution.basic, color: '#fbbf24' },
    { name: 'Competent (2)', value: scoreDistribution.competent, color: '#60a5fa' },
    { name: 'Expert (3)', value: scoreDistribution.expert, color: '#4ade80' }
  ];

  // Engineer performance data
  const engineerPerformance = data.engineers.map(engineer => {
    const engineerScores = allScores.filter(s => s.engineerId === engineer.id);
    const avgScore = engineerScores.reduce((sum, s) => sum + s.score, 0) / engineerScores.length;
    return {
      name: engineer.name,
      avgScore: parseFloat(avgScore.toFixed(2)),
      totalAssessments: engineerScores.length
    };
  }).sort((a, b) => b.avgScore - a.avgScore);

  // Top performers (top 5)
  const topPerformers = engineerPerformance.slice(0, 5);

  // Engineers needing attention (bottom 5)
  const needsAttention = engineerPerformance.slice(-5).reverse();

  // Competency completion rate
  const fullyTrained = allScores.filter(s => s.score >= 2).length;
  const completionRate = ((fullyTrained / totalAssessments) * 100).toFixed(1);

  return (
    <div className="space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-gray-900 dark:text-white mb-2">Dashboard</h1>
        <p className="text-gray-600 dark:text-gray-400">Team performance overview and key metrics</p>
      </div>

      {/* Quick Stats Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <div className="bg-white rounded-xl shadow-card p-6 border-l-4 border-gray-900">
          <div className="flex items-center justify-between mb-2">
            <Users size={32} className="text-gray-700" />
          </div>
          <p className="text-4xl font-bold mb-1 text-gray-900">{totalEngineers}</p>
          <p className="text-gray-600 text-sm font-medium">Total Engineers</p>
        </div>

        <div className="bg-white rounded-xl shadow-card p-6 border-l-4 border-accent">
          <div className="flex items-center justify-between mb-2">
            <Target size={32} className="text-accent" />
          </div>
          <p className="text-4xl font-bold mb-1 text-accent">{totalCompetencies}</p>
          <p className="text-gray-600 text-sm font-medium">Total Competencies</p>
        </div>

        <div className="bg-white rounded-xl shadow-card p-6 border-l-4 border-success">
          <div className="flex items-center justify-between mb-2">
            <TrendingUp size={32} className="text-success" />
          </div>
          <p className="text-4xl font-bold mb-1 text-success">{averageScore}</p>
          <p className="text-gray-600 text-sm font-medium">Average Skill Level</p>
        </div>

        <div className="bg-white rounded-xl shadow-card p-6 border-l-4 border-warning">
          <div className="flex items-center justify-between mb-2">
            <Award size={32} className="text-warning" />
          </div>
          <p className="text-4xl font-bold mb-1 text-warning">{completionRate}%</p>
          <p className="text-gray-600 text-sm font-medium">Completion Rate</p>
        </div>
      </div>

      {/* Score Distribution */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Pie Chart */}
        <div className="bg-white dark:bg-gray-800 rounded-xl shadow-card p-6">
          <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-6">Score Distribution</h2>
          <ResponsiveContainer width="100%" height={300}>
            <PieChart>
              <Pie
                data={pieData}
                cx="50%"
                cy="50%"
                labelLine={false}
                label={({ name, percent }) => `${name}: ${(percent * 100).toFixed(0)}%`}
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
          <div className="grid grid-cols-2 gap-3 mt-4">
            <div className="flex items-center gap-2 p-3 bg-red-50 rounded-lg border border-red-200">
              <div className="w-4 h-4 rounded" style={{ backgroundColor: '#ff6b6b' }}></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Not Trained</p>
                <p className="text-lg font-bold text-accent">{scorePercentages.notTrained}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-yellow-50 rounded-lg border border-yellow-200">
              <div className="w-4 h-4 rounded" style={{ backgroundColor: '#fbbf24' }}></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Basic</p>
                <p className="text-lg font-bold text-warning">{scorePercentages.basic}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-blue-50 rounded-lg border border-blue-200">
              <div className="w-4 h-4 rounded" style={{ backgroundColor: '#60a5fa' }}></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Competent</p>
                <p className="text-lg font-bold text-info">{scorePercentages.competent}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-green-50 rounded-lg border border-green-200">
              <div className="w-4 h-4 rounded" style={{ backgroundColor: '#4ade80' }}></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Expert</p>
                <p className="text-lg font-bold text-success">{scorePercentages.expert}%</p>
              </div>
            </div>
          </div>
        </div>

        {/* Bar Chart */}
        <div className="bg-white dark:bg-gray-800 rounded-xl shadow-card p-6">
          <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-6">Engineer Performance</h2>
          <ResponsiveContainer width="100%" height={300}>
            <BarChart data={engineerPerformance}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis dataKey="name" angle={-45} textAnchor="end" height={100} />
              <YAxis domain={[0, 3]} />
              <Tooltip />
              <Bar dataKey="avgScore" fill="#ff6b6b" name="Average Score" />
            </BarChart>
          </ResponsiveContainer>
        </div>
      </div>

      {/* Top Performers and Needs Attention */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Top Performers */}
        <div className="bg-white dark:bg-gray-800 rounded-xl shadow-card p-6">
          <div className="flex items-center gap-2 mb-6">
            <Award className="text-success" size={28} />
            <h2 className="text-2xl font-bold text-gray-900 dark:text-white">Top Performers</h2>
          </div>
          <div className="space-y-3">
            {topPerformers.map((engineer, index) => (
              <div key={index} className="flex items-center justify-between p-3 bg-green-50 border border-green-200 rounded-lg">
                <div className="flex items-center gap-3">
                  <div className="w-8 h-8 bg-success text-white rounded-full flex items-center justify-center font-bold">
                    {index + 1}
                  </div>
                  <div>
                    <p className="font-medium text-gray-900">{engineer.name}</p>
                    <p className="text-sm text-gray-600">{engineer.totalAssessments} competencies</p>
                  </div>
                </div>
                <div className="text-right">
                  <p className="text-2xl font-bold text-success">{engineer.avgScore}</p>
                  <p className="text-xs text-gray-500">avg score</p>
                </div>
              </div>
            ))}
          </div>
        </div>

        {/* Needs Attention */}
        <div className="bg-white dark:bg-gray-800 rounded-xl shadow-card p-6">
          <div className="flex items-center gap-2 mb-6">
            <AlertTriangle className="text-warning" size={28} />
            <h2 className="text-2xl font-bold text-gray-900 dark:text-white">Needs Attention</h2>
          </div>
          <div className="space-y-3">
            {needsAttention.map((engineer, index) => (
              <div key={index} className="flex items-center justify-between p-3 bg-yellow-50 border border-yellow-200 rounded-lg">
                <div className="flex items-center gap-3">
                  <div className="w-8 h-8 bg-warning text-white rounded-full flex items-center justify-center font-bold">
                    !
                  </div>
                  <div>
                    <p className="font-medium text-gray-900">{engineer.name}</p>
                    <p className="text-sm text-gray-600">{engineer.totalAssessments} competencies</p>
                  </div>
                </div>
                <div className="text-right">
                  <p className="text-2xl font-bold text-warning">{engineer.avgScore}</p>
                  <p className="text-xs text-gray-500">avg score</p>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Summary Insights */}
      <div className="bg-white dark:bg-gray-800 rounded-xl shadow-card p-8 border-l-4 border-gray-900">
        <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-6">Key Insights</h2>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <div className="bg-gray-50 dark:bg-gray-700 rounded-lg p-5 border border-gray-200 dark:border-gray-600 shadow-soft">
            <p className="text-sm text-gray-600 dark:text-gray-400 mb-2 font-medium">Total Assessments</p>
            <p className="text-4xl font-bold text-gray-900 dark:text-white">{totalAssessments}</p>
          </div>
          <div className="bg-gray-50 dark:bg-gray-700 rounded-lg p-5 border border-gray-200 dark:border-gray-600 shadow-soft">
            <p className="text-sm text-gray-600 dark:text-gray-400 mb-2 font-medium">Fully Trained (≥2)</p>
            <p className="text-4xl font-bold text-success">{fullyTrained}</p>
          </div>
          <div className="bg-gray-50 dark:bg-gray-700 rounded-lg p-5 border border-gray-200 dark:border-gray-600 shadow-soft">
            <p className="text-sm text-gray-600 dark:text-gray-400 mb-2 font-medium">Need Training (&lt;2)</p>
            <p className="text-4xl font-bold text-warning">{totalAssessments - fullyTrained}</p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
