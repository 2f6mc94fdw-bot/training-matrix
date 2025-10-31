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
    { name: 'Not Trained (0)', value: scoreDistribution.notTrained, color: '#ef4444' },
    { name: 'Basic (1)', value: scoreDistribution.basic, color: '#f59e0b' },
    { name: 'Competent (2)', value: scoreDistribution.competent, color: '#eab308' },
    { name: 'Expert (3)', value: scoreDistribution.expert, color: '#22c55e' }
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
        <h1 className="text-3xl font-bold text-gray-900 mb-2">Dashboard</h1>
        <p className="text-gray-600">Team performance overview and key metrics</p>
      </div>

      {/* Quick Stats Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        <div className="bg-gradient-to-br from-blue-500 to-blue-600 rounded-xl shadow-lg p-6 text-white">
          <div className="flex items-center justify-between mb-2">
            <Users size={32} className="opacity-80" />
          </div>
          <p className="text-3xl font-bold mb-1">{totalEngineers}</p>
          <p className="text-blue-100 text-sm">Total Engineers</p>
        </div>

        <div className="bg-gradient-to-br from-purple-500 to-purple-600 rounded-xl shadow-lg p-6 text-white">
          <div className="flex items-center justify-between mb-2">
            <Target size={32} className="opacity-80" />
          </div>
          <p className="text-3xl font-bold mb-1">{totalCompetencies}</p>
          <p className="text-purple-100 text-sm">Total Competencies</p>
        </div>

        <div className="bg-gradient-to-br from-green-500 to-green-600 rounded-xl shadow-lg p-6 text-white">
          <div className="flex items-center justify-between mb-2">
            <TrendingUp size={32} className="opacity-80" />
          </div>
          <p className="text-3xl font-bold mb-1">{averageScore}</p>
          <p className="text-green-100 text-sm">Average Skill Level</p>
        </div>

        <div className="bg-gradient-to-br from-orange-500 to-orange-600 rounded-xl shadow-lg p-6 text-white">
          <div className="flex items-center justify-between mb-2">
            <Award size={32} className="opacity-80" />
          </div>
          <p className="text-3xl font-bold mb-1">{completionRate}%</p>
          <p className="text-orange-100 text-sm">Completion Rate</p>
        </div>
      </div>

      {/* Score Distribution */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Pie Chart */}
        <div className="bg-white rounded-xl shadow-lg p-6">
          <h2 className="text-xl font-bold text-gray-900 mb-4">Score Distribution</h2>
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
            <div className="flex items-center gap-2 p-3 bg-red-50 rounded-lg">
              <div className="w-4 h-4 bg-red-500 rounded"></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Not Trained</p>
                <p className="text-lg font-bold text-gray-900">{scorePercentages.notTrained}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-orange-50 rounded-lg">
              <div className="w-4 h-4 bg-orange-500 rounded"></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Basic</p>
                <p className="text-lg font-bold text-gray-900">{scorePercentages.basic}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-yellow-50 rounded-lg">
              <div className="w-4 h-4 bg-yellow-500 rounded"></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Competent</p>
                <p className="text-lg font-bold text-gray-900">{scorePercentages.competent}%</p>
              </div>
            </div>
            <div className="flex items-center gap-2 p-3 bg-green-50 rounded-lg">
              <div className="w-4 h-4 bg-green-500 rounded"></div>
              <div>
                <p className="text-sm font-medium text-gray-700">Expert</p>
                <p className="text-lg font-bold text-gray-900">{scorePercentages.expert}%</p>
              </div>
            </div>
          </div>
        </div>

        {/* Bar Chart */}
        <div className="bg-white rounded-xl shadow-lg p-6">
          <h2 className="text-xl font-bold text-gray-900 mb-4">Engineer Performance</h2>
          <ResponsiveContainer width="100%" height={300}>
            <BarChart data={engineerPerformance}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis dataKey="name" angle={-45} textAnchor="end" height={100} />
              <YAxis domain={[0, 3]} />
              <Tooltip />
              <Bar dataKey="avgScore" fill="#3b82f6" name="Average Score" />
            </BarChart>
          </ResponsiveContainer>
        </div>
      </div>

      {/* Top Performers and Needs Attention */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Top Performers */}
        <div className="bg-white rounded-xl shadow-lg p-6">
          <div className="flex items-center gap-2 mb-4">
            <Award className="text-green-600" size={24} />
            <h2 className="text-xl font-bold text-gray-900">Top Performers</h2>
          </div>
          <div className="space-y-3">
            {topPerformers.map((engineer, index) => (
              <div key={index} className="flex items-center justify-between p-3 bg-green-50 border border-green-200 rounded-lg">
                <div className="flex items-center gap-3">
                  <div className="w-8 h-8 bg-green-600 text-white rounded-full flex items-center justify-center font-bold">
                    {index + 1}
                  </div>
                  <div>
                    <p className="font-medium text-gray-900">{engineer.name}</p>
                    <p className="text-sm text-gray-600">{engineer.totalAssessments} competencies</p>
                  </div>
                </div>
                <div className="text-right">
                  <p className="text-2xl font-bold text-green-600">{engineer.avgScore}</p>
                  <p className="text-xs text-gray-500">avg score</p>
                </div>
              </div>
            ))}
          </div>
        </div>

        {/* Needs Attention */}
        <div className="bg-white rounded-xl shadow-lg p-6">
          <div className="flex items-center gap-2 mb-4">
            <AlertTriangle className="text-orange-600" size={24} />
            <h2 className="text-xl font-bold text-gray-900">Needs Attention</h2>
          </div>
          <div className="space-y-3">
            {needsAttention.map((engineer, index) => (
              <div key={index} className="flex items-center justify-between p-3 bg-orange-50 border border-orange-200 rounded-lg">
                <div className="flex items-center gap-3">
                  <div className="w-8 h-8 bg-orange-600 text-white rounded-full flex items-center justify-center font-bold">
                    !
                  </div>
                  <div>
                    <p className="font-medium text-gray-900">{engineer.name}</p>
                    <p className="text-sm text-gray-600">{engineer.totalAssessments} competencies</p>
                  </div>
                </div>
                <div className="text-right">
                  <p className="text-2xl font-bold text-orange-600">{engineer.avgScore}</p>
                  <p className="text-xs text-gray-500">avg score</p>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Summary Insights */}
      <div className="bg-gradient-to-r from-blue-50 to-purple-50 rounded-xl shadow-lg p-6 border border-blue-200">
        <h2 className="text-xl font-bold text-gray-900 mb-4">Key Insights</h2>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="bg-white rounded-lg p-4">
            <p className="text-sm text-gray-600 mb-1">Total Assessments</p>
            <p className="text-3xl font-bold text-gray-900">{totalAssessments}</p>
          </div>
          <div className="bg-white rounded-lg p-4">
            <p className="text-sm text-gray-600 mb-1">Fully Trained (≥2)</p>
            <p className="text-3xl font-bold text-green-600">{fullyTrained}</p>
          </div>
          <div className="bg-white rounded-lg p-4">
            <p className="text-sm text-gray-600 mb-1">Need Training (&lt;2)</p>
            <p className="text-3xl font-bold text-orange-600">{totalAssessments - fullyTrained}</p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
