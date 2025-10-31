import React, { useState, useMemo } from 'react';
import { LineChart, Line, BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, AreaChart, Area } from 'recharts';
import { TrendingUp, TrendingDown, AlertCircle, Award, Users, Target, Calendar, Download } from 'lucide-react';

const AdvancedAnalytics = ({ data }) => {
  const [activeTab, setActiveTab] = useState('trends');

  // Calculate trend data from snapshots
  const trendData = useMemo(() => {
    if (!data.snapshots || data.snapshots.length === 0) return [];

    return data.snapshots.slice().reverse().map(snapshot => {
      let totalScore = 0;
      let maxScore = 0;

      data.engineers.forEach(engineer => {
        data.productionAreas.forEach(area => {
          area.machines.forEach(machine => {
            machine.competencies.forEach(comp => {
              const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
              const assessment = snapshot.data.assessments[key];
              const score = typeof assessment === 'object' ? (assessment?.score || 0) : (assessment || 0);
              totalScore += score;
              maxScore += comp.maxScore;
            });
          });
        });
      });

      const completionRate = maxScore > 0 ? (totalScore / maxScore) * 100 : 0;

      return {
        date: new Date(snapshot.timestamp).toLocaleDateString(),
        timestamp: snapshot.timestamp,
        completionRate: parseFloat(completionRate.toFixed(2)),
        totalScore,
        maxScore
      };
    });
  }, [data]);

  // Predict next month's completion rate using linear regression
  const prediction = useMemo(() => {
    if (trendData.length < 2) return null;

    const recent = trendData.slice(-5); // Last 5 snapshots
    const n = recent.length;

    // Simple linear regression
    let sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    recent.forEach((point, index) => {
      sumX += index;
      sumY += point.completionRate;
      sumXY += index * point.completionRate;
      sumX2 += index * index;
    });

    const slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    const intercept = (sumY - slope * sumX) / n;
    const predicted = slope * n + intercept;

    const current = recent[recent.length - 1]?.completionRate || 0;
    const change = predicted - current;

    return {
      current: current.toFixed(1),
      predicted: Math.max(0, Math.min(100, predicted)).toFixed(1),
      change: change.toFixed(1),
      trend: change > 0 ? 'up' : change < 0 ? 'down' : 'stable'
    };
  }, [trendData]);

  // Shift comparison
  const shiftComparison = useMemo(() => {
    const shifts = {};

    data.engineers.forEach(engineer => {
      if (!shifts[engineer.shift]) {
        shifts[engineer.shift] = { totalScore: 0, maxScore: 0, engineers: 0 };
      }

      shifts[engineer.shift].engineers++;

      data.productionAreas.forEach(area => {
        area.machines.forEach(machine => {
          machine.competencies.forEach(comp => {
            const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
            const assessment = data.assessments[key];
            const score = typeof assessment === 'object' ? (assessment?.score || 0) : (assessment || 0);
            shifts[engineer.shift].totalScore += score;
            shifts[engineer.shift].maxScore += comp.maxScore;
          });
        });
      });
    });

    return Object.entries(shifts).map(([shift, stats]) => ({
      shift,
      average: stats.maxScore > 0 ? ((stats.totalScore / stats.maxScore) * 100).toFixed(1) : 0,
      engineers: stats.engineers,
      totalScore: stats.totalScore,
      maxScore: stats.maxScore
    })).sort((a, b) => parseFloat(b.average) - parseFloat(a.average));
  }, [data]);

  // Automated insights
  const insights = useMemo(() => {
    const autoInsights = [];

    // Trend insight
    if (prediction) {
      if (prediction.trend === 'up') {
        autoInsights.push({
          type: 'positive',
          icon: TrendingUp,
          title: 'Positive Growth Trend',
          message: `Team competency is projected to increase by ${prediction.change}% based on recent progress.`
        });
      } else if (prediction.trend === 'down') {
        autoInsights.push({
          type: 'warning',
          icon: TrendingDown,
          title: 'Declining Trend Detected',
          message: `Team competency is projected to decrease by ${Math.abs(prediction.change)}%. Consider additional training initiatives.`
        });
      }
    }

    // Shift performance gap
    if (shiftComparison.length > 1) {
      const best = shiftComparison[0];
      const worst = shiftComparison[shiftComparison.length - 1];
      const gap = parseFloat(best.average) - parseFloat(worst.average);

      if (gap > 10) {
        autoInsights.push({
          type: 'alert',
          icon: AlertCircle,
          title: 'Shift Performance Gap',
          message: `${best.shift} outperforms ${worst.shift} by ${gap.toFixed(1)}%. Consider cross-shift training programs.`
        });
      }
    }

    // Low competency alert
    const lowScores = Object.values(data.assessments).filter(a => {
      const score = typeof a === 'object' ? a.score : a;
      return score < 2;
    }).length;

    const totalAssessments = Object.keys(data.assessments).length;
    const lowPercentage = totalAssessments > 0 ? (lowScores / totalAssessments) * 100 : 0;

    if (lowPercentage > 30) {
      autoInsights.push({
        type: 'warning',
        icon: Target,
        title: 'Training Priority Alert',
        message: `${lowPercentage.toFixed(0)}% of competencies are below proficient level. Prioritize structured training programs.`
      });
    }

    // Top performer recognition
    const engineerScores = data.engineers.map(engineer => {
      let total = 0;
      let max = 0;

      data.productionAreas.forEach(area => {
        area.machines.forEach(machine => {
          machine.competencies.forEach(comp => {
            const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
            const assessment = data.assessments[key];
            const score = typeof assessment === 'object' ? (assessment?.score || 0) : (assessment || 0);
            total += score;
            max += comp.maxScore;
          });
        });
      });

      return {
        name: engineer.name,
        percentage: max > 0 ? (total / max) * 100 : 0
      };
    }).sort((a, b) => b.percentage - a.percentage);

    if (engineerScores[0] && engineerScores[0].percentage >= 85) {
      autoInsights.push({
        type: 'positive',
        icon: Award,
        title: 'Top Performer Recognition',
        message: `${engineerScores[0].name} leads the team with ${engineerScores[0].percentage.toFixed(0)}% competency mastery.`
      });
    }

    return autoInsights;
  }, [data, prediction, shiftComparison]);

  // Generate automated report summary
  const generateReport = () => {
    const report = {
      generatedAt: new Date().toLocaleString(),
      summary: {
        totalEngineers: data.engineers.length,
        totalCompetencies: data.productionAreas.reduce((sum, area) =>
          sum + area.machines.reduce((s, m) => s + m.competencies.length, 0), 0
        ),
        overallCompletion: prediction?.current || '0',
        trend: prediction?.trend || 'stable'
      },
      shifts: shiftComparison,
      insights: insights.map(i => ({ title: i.title, message: i.message })),
      recommendations: []
    };

    // Add recommendations
    if (prediction?.trend === 'down') {
      report.recommendations.push('Increase frequency of training sessions');
      report.recommendations.push('Implement peer mentoring program');
    }

    if (shiftComparison.length > 1) {
      const worst = shiftComparison[shiftComparison.length - 1];
      report.recommendations.push(`Focus training resources on ${worst.shift}`);
    }

    const blob = new Blob([JSON.stringify(report, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = `training-analytics-report-${Date.now()}.json`;
    link.click();
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
        <div className="flex justify-between items-center">
          <div>
            <h1 className="text-3xl font-bold text-gray-900 dark:text-white mb-2">Advanced Analytics</h1>
            <p className="text-gray-600 dark:text-gray-400">
              Predictive insights, trends, and automated reporting
            </p>
          </div>
          <button
            onClick={generateReport}
            className="flex items-center gap-2 px-4 py-2 bg-accent text-white rounded-lg hover:bg-accent-light shadow-md"
          >
            <Download size={20} />
            Export Report
          </button>
        </div>
      </div>

      {/* Sub-Tabs */}
      <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-2">
        <div className="flex gap-2">
          {['trends', 'shifts', 'insights'].map(tab => (
            <button
              key={tab}
              onClick={() => setActiveTab(tab)}
              className={`px-6 py-3 font-medium capitalize rounded-lg transition-all duration-200 ${
                activeTab === tab
                  ? 'bg-accent text-white shadow-md'
                  : 'text-gray-600 dark:text-gray-300 hover:bg-red-50 dark:hover:bg-gray-700'
              }`}
            >
              {tab === 'trends' && '📈 Trends & Predictions'}
              {tab === 'shifts' && '🔄 Shift Comparison'}
              {tab === 'insights' && '💡 Automated Insights'}
            </button>
          ))}
        </div>
      </div>

      {/* Trends Tab */}
      {activeTab === 'trends' && (
        <div className="space-y-6">
          {/* Prediction Cards */}
          {prediction && (
            <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-info">
                <p className="text-sm text-gray-600 mb-1">Current Completion</p>
                <p className="text-3xl font-bold text-info">{prediction.current}%</p>
              </div>
              <div className={`bg-white rounded-xl shadow-lg p-6 border-l-4 ${
                prediction.trend === 'up' ? 'border-success' : prediction.trend === 'down' ? 'border-warning' : 'border-gray-400'
              }`}>
                <p className="text-sm text-gray-600 mb-1">Predicted Next Period</p>
                <p className={`text-3xl font-bold ${
                  prediction.trend === 'up' ? 'text-success' : prediction.trend === 'down' ? 'text-warning' : 'text-gray-600'
                }`}>{prediction.predicted}%</p>
              </div>
              <div className="bg-white rounded-xl shadow-lg p-6 border-l-4 border-accent">
                <p className="text-sm text-gray-600 mb-1">Expected Change</p>
                <p className="text-3xl font-bold text-accent flex items-center gap-2">
                  {prediction.change > 0 ? '+' : ''}{prediction.change}%
                  {prediction.trend === 'up' && <TrendingUp className="text-success" size={24} />}
                  {prediction.trend === 'down' && <TrendingDown className="text-warning" size={24} />}
                </p>
              </div>
            </div>
          )}

          {/* Trend Chart */}
          <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
            <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Historical Performance Trend</h2>
            {trendData.length > 0 ? (
              <ResponsiveContainer width="100%" height={400}>
                <AreaChart data={trendData}>
                  <defs>
                    <linearGradient id="colorCompletion" x1="0" y1="0" x2="0" y2="1">
                      <stop offset="5%" stopColor="#ff6b6b" stopOpacity={0.8}/>
                      <stop offset="95%" stopColor="#ff6b6b" stopOpacity={0.1}/>
                    </linearGradient>
                  </defs>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="date" />
                  <YAxis domain={[0, 100]} label={{ value: 'Completion %', angle: -90, position: 'insideLeft' }} />
                  <Tooltip />
                  <Legend />
                  <Area
                    type="monotone"
                    dataKey="completionRate"
                    stroke="#ff6b6b"
                    fillOpacity={1}
                    fill="url(#colorCompletion)"
                    name="Team Competency %"
                  />
                </AreaChart>
              </ResponsiveContainer>
            ) : (
              <div className="text-center py-12 text-gray-500">
                <Calendar size={48} className="mx-auto mb-3 opacity-50" />
                <p>No historical data yet. Create snapshots to track trends over time.</p>
              </div>
            )}
          </div>
        </div>
      )}

      {/* Shifts Tab */}
      {activeTab === 'shifts' && (
        <div className="space-y-6">
          {/* Shift Stats Cards */}
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
            {shiftComparison.map((shift, index) => (
              <div
                key={shift.shift}
                className={`bg-white rounded-xl shadow-lg p-6 border-l-4 ${
                  index === 0 ? 'border-success' : index === shiftComparison.length - 1 ? 'border-warning' : 'border-info'
                }`}
              >
                <div className="flex items-center justify-between mb-2">
                  <h3 className="font-bold text-gray-900">{shift.shift}</h3>
                  {index === 0 && <Award className="text-success" size={20} />}
                </div>
                <p className="text-3xl font-bold text-accent mb-1">{shift.average}%</p>
                <p className="text-sm text-gray-600">{shift.engineers} engineers</p>
              </div>
            ))}
          </div>

          {/* Comparison Chart */}
          <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
            <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Shift Performance Comparison</h2>
            <ResponsiveContainer width="100%" height={400}>
              <BarChart data={shiftComparison}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="shift" />
                <YAxis domain={[0, 100]} label={{ value: 'Completion %', angle: -90, position: 'insideLeft' }} />
                <Tooltip />
                <Legend />
                <Bar dataKey="average" fill="#ff6b6b" name="Competency %" />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>
      )}

      {/* Insights Tab */}
      {activeTab === 'insights' && (
        <div className="space-y-6">
          <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
            <h2 className="text-xl font-bold text-gray-900 dark:text-white mb-4">Automated Insights</h2>

            {insights.length > 0 ? (
              <div className="space-y-4">
                {insights.map((insight, index) => {
                  const Icon = insight.icon;
                  return (
                    <div
                      key={index}
                      className={`p-4 rounded-lg border-l-4 ${
                        insight.type === 'positive' ? 'bg-green-50 border-success' :
                        insight.type === 'warning' ? 'bg-yellow-50 border-warning' :
                        'bg-red-50 border-accent'
                      }`}
                    >
                      <div className="flex items-start gap-3">
                        <Icon className={`mt-1 ${
                          insight.type === 'positive' ? 'text-success' :
                          insight.type === 'warning' ? 'text-warning' :
                          'text-accent'
                        }`} size={24} />
                        <div>
                          <h3 className="font-bold text-gray-900 mb-1">{insight.title}</h3>
                          <p className="text-gray-700">{insight.message}</p>
                        </div>
                      </div>
                    </div>
                  );
                })}
              </div>
            ) : (
              <div className="text-center py-12 text-gray-500">
                <Users size={48} className="mx-auto mb-3 opacity-50" />
                <p>Collecting data to generate insights...</p>
                <p className="text-sm mt-2">Add more assessments and snapshots for automated analysis.</p>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
};

export default AdvancedAnalytics;
