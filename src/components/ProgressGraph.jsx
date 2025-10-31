import React, { useMemo } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { TrendingUp, Calendar } from 'lucide-react';

const ProgressGraph = ({ data, engineerId }) => {
  // Collect all score history for the engineer
  const progressData = useMemo(() => {
    if (!data || !data.assessments) return [];

    const historyMap = new Map();

    // Iterate through all assessments for this engineer
    Object.entries(data.assessments).forEach(([key, assessment]) => {
      if (key.startsWith(`${engineerId}-`)) {
        // Extract area, machine, competency info
        const [engId, areaId, machineId, compId] = key.split('-');

        // Find the competency name
        let competencyName = 'Unknown';
        let areaName = 'Unknown';
        let machineName = 'Unknown';

        for (const area of data.productionAreas) {
          if (area.id === areaId) {
            areaName = area.name;
            for (const machine of area.machines) {
              if (machine.id === machineId) {
                machineName = machine.name;
                const comp = machine.competencies.find(c => c.id === compId);
                if (comp) {
                  competencyName = comp.name;
                }
              }
            }
          }
        }

        // Process history entries
        if (assessment.history && assessment.history.length > 0) {
          assessment.history.forEach(entry => {
            const timestamp = new Date(entry.timestamp);
            const dateKey = timestamp.toISOString().split('T')[0]; // YYYY-MM-DD

            if (!historyMap.has(dateKey)) {
              historyMap.set(dateKey, {
                date: dateKey,
                dateFormatted: timestamp.toLocaleDateString('en-US', {
                  month: 'short',
                  day: 'numeric',
                  year: 'numeric'
                }),
                timestamp: timestamp.getTime(),
                improvements: 0,
                totalChanges: 0,
                details: []
              });
            }

            const dayData = historyMap.get(dateKey);
            const scoreChange = entry.newScore - entry.oldScore;

            if (scoreChange > 0) {
              dayData.improvements += scoreChange;
            }
            dayData.totalChanges += Math.abs(scoreChange);
            dayData.details.push({
              competency: competencyName,
              area: areaName,
              machine: machineName,
              oldScore: entry.oldScore,
              newScore: entry.newScore,
              change: scoreChange
            });
          });
        }
      }
    });

    // Convert to array and sort by date
    return Array.from(historyMap.values())
      .sort((a, b) => a.timestamp - b.timestamp)
      .map((item, index, array) => {
        // Calculate cumulative improvements
        const cumulativeImprovements = array
          .slice(0, index + 1)
          .reduce((sum, day) => sum + day.improvements, 0);

        return {
          ...item,
          cumulativeImprovements
        };
      });
  }, [data, engineerId]);

  // Calculate overall statistics
  const stats = useMemo(() => {
    if (progressData.length === 0) return { totalImprovements: 0, totalChanges: 0, daysActive: 0 };

    return {
      totalImprovements: progressData.reduce((sum, day) => sum + day.improvements, 0),
      totalChanges: progressData.reduce((sum, day) => sum + day.totalChanges, 0),
      daysActive: progressData.length
    };
  }, [progressData]);

  const CustomTooltip = ({ active, payload }) => {
    if (active && payload && payload.length) {
      const data = payload[0].payload;
      return (
        <div className="bg-white dark:bg-gray-800 p-4 border-2 border-accent rounded-lg shadow-lg max-w-md">
          <p className="font-bold text-gray-900 dark:text-white mb-2">{data.dateFormatted}</p>
          <p className="text-sm text-accent dark:text-accent-light font-semibold mb-2">
            +{data.improvements} points improvement
          </p>
          <div className="text-xs text-gray-600 dark:text-gray-300 space-y-1 max-h-40 overflow-y-auto">
            {data.details.map((detail, idx) => (
              <div key={idx} className={`py-1 ${detail.change > 0 ? 'text-success dark:text-success' : 'text-gray-500'}`}>
                {detail.competency}: {detail.oldScore} → {detail.newScore}
                {detail.change > 0 && ' 📈'}
              </div>
            ))}
          </div>
        </div>
      );
    }
    return null;
  };

  if (progressData.length === 0) {
    return (
      <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
        <div className="flex items-center gap-2 mb-4">
          <TrendingUp className="text-blue-600 dark:text-blue-400" size={24} />
          <h3 className="text-xl font-bold text-gray-900 dark:text-white">Progress Over Time</h3>
        </div>
        <div className="text-center py-8 text-gray-500 dark:text-gray-400">
          <Calendar size={48} className="mx-auto mb-3 opacity-50" />
          <p>No score changes recorded yet.</p>
          <p className="text-sm mt-2">As you update your scores, your progress will be tracked here!</p>
        </div>
      </div>
    );
  }

  return (
    <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
      <div className="flex items-center gap-2 mb-4">
        <TrendingUp className="text-accent dark:text-accent-light" size={24} />
        <h3 className="text-xl font-bold text-gray-900 dark:text-white">Progress Over Time</h3>
      </div>

      {/* Statistics Cards */}
      <div className="grid grid-cols-3 gap-4 mb-6">
        <div className="bg-white rounded-lg p-4 text-center border-l-4 border-success shadow-md">
          <div className="text-3xl font-bold text-success">+{stats.totalImprovements}</div>
          <div className="text-sm text-gray-600 font-medium">Total Improvements</div>
        </div>
        <div className="bg-white rounded-lg p-4 text-center border-l-4 border-info shadow-md">
          <div className="text-3xl font-bold text-info">{stats.totalChanges}</div>
          <div className="text-sm text-gray-600 font-medium">Total Updates</div>
        </div>
        <div className="bg-white rounded-lg p-4 text-center border-l-4 border-accent shadow-md">
          <div className="text-3xl font-bold text-accent">{stats.daysActive}</div>
          <div className="text-sm text-gray-600 font-medium">Active Days</div>
        </div>
      </div>

      {/* Line Chart */}
      <ResponsiveContainer width="100%" height={300}>
        <LineChart data={progressData}>
          <CartesianGrid strokeDasharray="3 3" stroke="#e5e7eb" />
          <XAxis
            dataKey="dateFormatted"
            stroke="#6b7280"
            style={{ fontSize: '12px' }}
            angle={-45}
            textAnchor="end"
            height={80}
          />
          <YAxis
            stroke="#6b7280"
            style={{ fontSize: '12px' }}
            label={{ value: 'Cumulative Points', angle: -90, position: 'insideLeft' }}
          />
          <Tooltip content={<CustomTooltip />} />
          <Legend />
          <Line
            type="monotone"
            dataKey="cumulativeImprovements"
            stroke="#ff6b6b"
            strokeWidth={3}
            name="Cumulative Score Improvements"
            dot={{ fill: '#ff6b6b', r: 5 }}
            activeDot={{ r: 8 }}
          />
        </LineChart>
      </ResponsiveContainer>

      {/* Recent Activity */}
      <div className="mt-6">
        <h4 className="font-semibold text-gray-900 dark:text-white mb-3">Recent Activity</h4>
        <div className="space-y-2 max-h-60 overflow-y-auto">
          {progressData.slice(-5).reverse().map((day, idx) => (
            <div key={idx} className="flex justify-between items-start p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
              <div>
                <p className="font-medium text-gray-900 dark:text-white">{day.dateFormatted}</p>
                <p className="text-sm text-gray-600 dark:text-gray-300">
                  {day.details.length} competenc{day.details.length === 1 ? 'y' : 'ies'} updated
                </p>
              </div>
              <div className="text-right">
                <span className="text-lg font-bold text-success dark:text-success">
                  +{day.improvements}
                </span>
                <p className="text-xs text-gray-500 dark:text-gray-400">points</p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default ProgressGraph;
