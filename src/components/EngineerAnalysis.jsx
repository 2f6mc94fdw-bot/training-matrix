import React, { useState, useMemo } from 'react';
import { RadarChart, Radar, PolarGrid, PolarAngleAxis, PolarRadiusAxis, BarChart, Bar, LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { Award, Target, TrendingUp } from 'lucide-react';

const EngineerAnalysis = ({ data, engineer, getAssessmentScore, useWeightedScores }) => {
  const [graphType, setGraphType] = useState('radar');
  const [selectedArea, setSelectedArea] = useState('all');

  // Calculate overall production knowledge for radar chart
  const overallRadarData = useMemo(() => {
    return data.productionAreas.map(area => {
      let areaTotal = 0;
      let areaMax = 0;

      area.machines.forEach(machine => {
        const weight = useWeightedScores ? machine.importance : 1;
        machine.competencies.forEach(comp => {
          const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
          areaTotal += score * weight;
          areaMax += comp.maxScore * weight;
        });
      });

      const percentage = areaMax > 0 ? (areaTotal / areaMax) * 100 : 0;

      return {
        area: area.name,
        score: parseFloat(percentage.toFixed(1)),
        fullMark: 100
      };
    });
  }, [data, engineer, getAssessmentScore, useWeightedScores]);

  // Calculate machine knowledge for selected production area
  const machineKnowledgeData = useMemo(() => {
    if (selectedArea === 'all') {
      // Show all machines across all areas
      const allMachines = [];
      data.productionAreas.forEach(area => {
        area.machines.forEach(machine => {
          let machineTotal = 0;
          let machineMax = 0;

          machine.competencies.forEach(comp => {
            const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
            machineTotal += score;
            machineMax += comp.maxScore;
          });

          const percentage = machineMax > 0 ? (machineTotal / machineMax) * 100 : 0;

          allMachines.push({
            name: `${area.name} - ${machine.name}`,
            score: parseFloat(percentage.toFixed(1)),
            importance: machine.importance,
            area: area.name
          });
        });
      });
      return allMachines;
    } else {
      // Show machines for selected area only
      const area = data.productionAreas.find(a => a.id === parseInt(selectedArea));
      if (!area) return [];

      return area.machines.map(machine => {
        let machineTotal = 0;
        let machineMax = 0;

        machine.competencies.forEach(comp => {
          const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
          machineTotal += score;
          machineMax += comp.maxScore;
        });

        const percentage = machineMax > 0 ? (machineTotal / machineMax) * 100 : 0;

        // Also calculate competency breakdown
        const competencies = machine.competencies.map(comp => ({
          name: comp.name,
          score: getAssessmentScore(engineer.id, area.id, machine.id, comp.id),
          maxScore: comp.maxScore
        }));

        return {
          name: machine.name,
          score: parseFloat(percentage.toFixed(1)),
          importance: machine.importance,
          competencies
        };
      });
    }
  }, [data, engineer, selectedArea, getAssessmentScore]);

  // Calculate competencies for selected machine (radar view)
  const getMachineCompetenciesRadar = (machineData) => {
    if (!machineData.competencies) return [];
    return machineData.competencies.map(comp => ({
      competency: comp.name,
      score: comp.score,
      fullMark: comp.maxScore
    }));
  };

  // Calculate overall stats
  const stats = useMemo(() => {
    let totalScore = 0;
    let maxScore = 0;
    let expert = 0;
    let competent = 0;
    let basic = 0;
    let notTrained = 0;

    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        machine.competencies.forEach(comp => {
          const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
          totalScore += score;
          maxScore += comp.maxScore;

          if (score === 3) expert++;
          else if (score === 2) competent++;
          else if (score === 1) basic++;
          else notTrained++;
        });
      });
    });

    return {
      completion: maxScore > 0 ? ((totalScore / maxScore) * 100).toFixed(1) : 0,
      expert,
      competent,
      basic,
      notTrained,
      total: expert + competent + basic + notTrained
    };
  }, [data, engineer, getAssessmentScore]);

  return (
    <div className="space-y-6">
      {/* Engineer Header Card */}
      <div className="bg-gradient-to-r from-accent to-accent-light rounded-xl p-6 text-white shadow-lg">
        <div className="flex justify-between items-start">
          <div>
            <h3 className="text-3xl font-bold mb-2">{engineer.name}</h3>
            <p className="text-white/80 text-lg">Shift: {engineer.shift}</p>
          </div>
          <div className="text-right">
            <p className="text-sm text-white/80">Overall Mastery</p>
            <p className="text-5xl font-bold">{stats.completion}%</p>
          </div>
        </div>

        <div className="mt-6 grid grid-cols-4 gap-4">
          <div className="bg-white/20 rounded-lg p-4 backdrop-blur-sm">
            <p className="text-sm text-white/80">Expert (3)</p>
            <p className="text-2xl font-bold">{stats.expert}</p>
          </div>
          <div className="bg-white/20 rounded-lg p-4 backdrop-blur-sm">
            <p className="text-sm text-white/80">Competent (2)</p>
            <p className="text-2xl font-bold">{stats.competent}</p>
          </div>
          <div className="bg-white/20 rounded-lg p-4 backdrop-blur-sm">
            <p className="text-sm text-white/80">Basic (1)</p>
            <p className="text-2xl font-bold">{stats.basic}</p>
          </div>
          <div className="bg-white/20 rounded-lg p-4 backdrop-blur-sm">
            <p className="text-sm text-white/80">Not Trained (0)</p>
            <p className="text-2xl font-bold">{stats.notTrained}</p>
          </div>
        </div>
      </div>

      {/* Overall Production Knowledge Radar */}
      <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
        <div className="flex items-center gap-2 mb-4">
          <Target className="text-accent" size={24} />
          <h3 className="text-xl font-bold text-gray-900 dark:text-white">Overall Production Knowledge</h3>
        </div>
        <ResponsiveContainer width="100%" height={400}>
          <RadarChart data={overallRadarData}>
            <PolarGrid />
            <PolarAngleAxis dataKey="area" />
            <PolarRadiusAxis angle={90} domain={[0, 100]} />
            <Radar
              name="Knowledge %"
              dataKey="score"
              stroke="#ff6b6b"
              fill="#ff6b6b"
              fillOpacity={0.6}
            />
            <Tooltip />
            <Legend />
          </RadarChart>
        </ResponsiveContainer>
      </div>

      {/* Machine Knowledge Analysis */}
      <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
        <div className="flex items-center justify-between mb-6">
          <div className="flex items-center gap-2">
            <Award className="text-accent" size={24} />
            <h3 className="text-xl font-bold text-gray-900 dark:text-white">Machine Knowledge Analysis</h3>
          </div>
          <div className="flex gap-3">
            {/* Production Area Selector */}
            <select
              value={selectedArea}
              onChange={(e) => setSelectedArea(e.target.value)}
              className="px-4 py-2 border-2 border-gray-300 dark:border-gray-600 rounded-lg bg-white dark:bg-gray-700 text-gray-900 dark:text-white font-medium"
            >
              <option value="all">All Production Areas</option>
              {data.productionAreas.map(area => (
                <option key={area.id} value={area.id}>{area.name}</option>
              ))}
            </select>

            {/* Graph Type Selector */}
            <select
              value={graphType}
              onChange={(e) => setGraphType(e.target.value)}
              className="px-4 py-2 border-2 border-accent rounded-lg bg-white dark:bg-gray-700 text-gray-900 dark:text-white font-medium"
            >
              <option value="radar">📊 Radar Chart</option>
              <option value="bar">📈 Bar Chart</option>
              <option value="line">📉 Line Chart</option>
            </select>
          </div>
        </div>

        {/* Visualization */}
        <ResponsiveContainer width="100%" height={400}>
          {graphType === 'radar' ? (
            <RadarChart data={machineKnowledgeData}>
              <PolarGrid />
              <PolarAngleAxis
                dataKey="name"
                tick={{ fontSize: 12 }}
              />
              <PolarRadiusAxis angle={90} domain={[0, 100]} />
              <Radar
                name="Mastery %"
                dataKey="score"
                stroke="#ff6b6b"
                fill="#ff6b6b"
                fillOpacity={0.6}
              />
              <Tooltip />
              <Legend />
            </RadarChart>
          ) : graphType === 'bar' ? (
            <BarChart data={machineKnowledgeData}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="name"
                angle={-45}
                textAnchor="end"
                height={120}
                interval={0}
                tick={{ fontSize: 11 }}
              />
              <YAxis domain={[0, 100]} label={{ value: 'Mastery %', angle: -90, position: 'insideLeft' }} />
              <Tooltip />
              <Legend />
              <Bar dataKey="score" fill="#ff6b6b" name="Mastery %" />
              {useWeightedScores && (
                <Bar dataKey="importance" fill="#60a5fa" name="Importance" />
              )}
            </BarChart>
          ) : (
            <LineChart data={machineKnowledgeData}>
              <CartesianGrid strokeDasharray="3 3" />
              <XAxis
                dataKey="name"
                angle={-45}
                textAnchor="end"
                height={120}
                interval={0}
                tick={{ fontSize: 11 }}
              />
              <YAxis domain={[0, 100]} label={{ value: 'Mastery %', angle: -90, position: 'insideLeft' }} />
              <Tooltip />
              <Legend />
              <Line
                type="monotone"
                dataKey="score"
                stroke="#ff6b6b"
                strokeWidth={3}
                name="Mastery %"
                dot={{ fill: '#ff6b6b', r: 5 }}
              />
            </LineChart>
          )}
        </ResponsiveContainer>
      </div>

      {/* Machine Detail Cards (when specific area selected) */}
      {selectedArea !== 'all' && (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {machineKnowledgeData.map((machine, idx) => (
            <div key={idx} className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-5 border-2 border-gray-200 dark:border-gray-700 hover:border-accent transition-colors">
              <div className="flex justify-between items-start mb-3">
                <h4 className="font-bold text-gray-900 dark:text-white">{machine.name}</h4>
                <div className="text-right">
                  <p className="text-2xl font-bold text-accent">{machine.score}%</p>
                  <p className="text-xs text-gray-500">Importance: {machine.importance}</p>
                </div>
              </div>

              <div className="w-full bg-gray-200 dark:bg-gray-700 rounded-full h-2 mb-4">
                <div
                  className="bg-gradient-to-r from-accent to-accent-light h-2 rounded-full transition-all"
                  style={{ width: `${machine.score}%` }}
                ></div>
              </div>

              {/* Competency breakdown */}
              {machine.competencies && (
                <div className="space-y-2">
                  <p className="text-xs font-semibold text-gray-600 dark:text-gray-400 uppercase">Competencies:</p>
                  {machine.competencies.map((comp, compIdx) => (
                    <div key={compIdx} className="flex justify-between items-center text-sm">
                      <span className="text-gray-700 dark:text-gray-300 text-xs">{comp.name}</span>
                      <span className={`font-bold ${
                        comp.score === 3 ? 'text-success' :
                        comp.score === 2 ? 'text-info' :
                        comp.score === 1 ? 'text-warning' :
                        'text-accent'
                      }`}>
                        {comp.score}/{comp.maxScore}
                      </span>
                    </div>
                  ))}
                </div>
              )}
            </div>
          ))}
        </div>
      )}

      {/* Summary Insights */}
      <div className="bg-gradient-to-r from-blue-50 to-purple-50 dark:from-gray-800 dark:to-gray-700 rounded-xl shadow-lg p-6 border-2 border-gray-200 dark:border-gray-600">
        <div className="flex items-center gap-2 mb-4">
          <TrendingUp className="text-accent" size={24} />
          <h3 className="text-xl font-bold text-gray-900 dark:text-white">Key Insights</h3>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <div className="bg-white dark:bg-gray-800 rounded-lg p-4">
            <p className="text-sm text-gray-600 dark:text-gray-400 mb-1">Strongest Area</p>
            <p className="text-lg font-bold text-success">
              {overallRadarData.length > 0 ? overallRadarData.reduce((max, area) => area.score > max.score ? area : max).area : 'N/A'}
            </p>
          </div>
          <div className="bg-white dark:bg-gray-800 rounded-lg p-4">
            <p className="text-sm text-gray-600 dark:text-gray-400 mb-1">Development Priority</p>
            <p className="text-lg font-bold text-warning">
              {overallRadarData.length > 0 ? overallRadarData.reduce((min, area) => area.score < min.score ? area : min).area : 'N/A'}
            </p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default EngineerAnalysis;
