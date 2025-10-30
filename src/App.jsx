import React, { useState, useEffect } from 'react';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, PieChart, Pie, Cell } from 'recharts';
import { Download, Upload, Search, Filter, Plus, Trash2, Edit2, Save, X, FileDown, Users, Award, TrendingUp, AlertCircle } from 'lucide-react';
import { useAuth } from './hooks/useAuth';
import { useData } from './hooks/useData';
import { exportToExcel, exportEngineerReport } from './utils/excelExport';
import { importFromExcel, validateImportedData } from './utils/excelImport';
import { exportBackup, importBackup, getAuditLogs } from './utils/storage';

const COLORS = ['#0088FE', '#00C49F', '#FFBB28', '#FF8042', '#8884d8'];

function App() {
  // Authentication
  const { currentUser, isAuthenticated, login, logout } = useAuth();

  // Data management
  const dataHook = useData(currentUser);
  const { data, loading } = dataHook;

  // UI State
  const [loginForm, setLoginForm] = useState({ username: '', password: '' });
  const [activeTab, setActiveTab] = useState('admin');
  const [searchTerm, setSearchTerm] = useState('');
  const [filterShift, setFilterShift] = useState('all');
  const [filterArea, setFilterArea] = useState('all');
  const [editingItem, setEditingItem] = useState(null);
  const [confirmDelete, setConfirmDelete] = useState(null);
  const [bulkSelectMode, setBulkSelectMode] = useState(false);
  const [selectedEngineers, setSelectedEngineers] = useState([]);
  const [showModal, setShowModal] = useState(null);

  // Login handling
  const handleLogin = (e) => {
    e.preventDefault();
    const result = login(loginForm.username, loginForm.password);
    if (!result.success) {
      alert(result.message || 'Invalid credentials');
    }
  };

  const handleLogout = () => {
    logout();
    setLoginForm({ username: '', password: '' });
  };

  // Production Area Management
  const addProductionArea = () => {
    const name = prompt('Enter production area name:');
    if (name) {
      dataHook.addProductionArea({ name });
    }
  };

  const deleteProductionArea = (areaId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this production area and all its machines?',
      onConfirm: () => {
        dataHook.deleteProductionArea(areaId);
        setConfirmDelete(null);
      }
    });
  };

  // Machine Management
  const addMachine = (areaId) => {
    const name = prompt('Enter machine name:');
    const importance = prompt('Enter importance (1-10):', '5');
    if (name && importance) {
      dataHook.addMachine(areaId, { name, importance: parseInt(importance) });
    }
  };

  const deleteMachine = (areaId, machineId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this machine and all its competencies?',
      onConfirm: () => {
        dataHook.deleteMachine(areaId, machineId);
        setConfirmDelete(null);
      }
    });
  };

  // Competency Management
  const addCompetency = (areaId, machineId) => {
    const name = prompt('Enter competency name:');
    if (name) {
      dataHook.addCompetency(areaId, machineId, { name, maxScore: 3 });
    }
  };

  const deleteCompetency = (areaId, machineId, competencyId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this competency?',
      onConfirm: () => {
        dataHook.deleteCompetency(areaId, machineId, competencyId);
        setConfirmDelete(null);
      }
    });
  };

  // Engineer Management
  const addEngineer = () => {
    const name = prompt('Enter engineer name:');
    const shift = prompt('Enter shift (A Shift, B Shift, C Shift, D Shift, Day Shift):');
    if (name && shift) {
      dataHook.addEngineer({ name, shift });
    }
  };

  const deleteEngineer = (engineerId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this engineer?',
      onConfirm: () => {
        dataHook.deleteEngineer(engineerId);
        setConfirmDelete(null);
      }
    });
  };

  // Assessment Management
  const updateAssessment = (engineerId, areaId, machineId, competencyId, score) => {
    dataHook.updateAssessment(engineerId, areaId, machineId, competencyId, score);
  };

  const getAssessmentScore = (engineerId, areaId, machineId, competencyId) => {
    const key = `${engineerId}-${areaId}-${machineId}-${competencyId}`;
    const assessment = data?.assessments?.[key];
    if (typeof assessment === 'object' && assessment !== null) {
      return assessment.score || 0;
    }
    return assessment || 0;
  };

  // Bulk Operations
  const bulkUpdateScores = (competencyId, score) => {
    if (selectedEngineers.length === 0) {
      alert('Please select engineers first');
      return;
    }

    const updates = [];
    selectedEngineers.forEach(engineerId => {
      data.productionAreas.forEach(area => {
        area.machines.forEach(machine => {
          const comp = machine.competencies.find(c => c.id === competencyId);
          if (comp) {
            updates.push({ engineerId, areaId: area.id, machineId: machine.id, compId: competencyId, score });
          }
        });
      });
    });

    dataHook.bulkUpdateAssessments(updates);
    setBulkSelectMode(false);
    setSelectedEngineers([]);
  };

  // Calculate scores
  const calculateScores = (engineerId) => {
    let totalRaw = 0;
    let totalWeighted = 0;
    let totalPossibleRaw = 0;
    let totalPossibleWeighted = 0;

    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        machine.competencies.forEach(comp => {
          const score = getAssessmentScore(engineerId, area.id, machine.id, comp.id);
          totalRaw += score;
          totalWeighted += score * machine.importance;
          totalPossibleRaw += comp.maxScore;
          totalPossibleWeighted += comp.maxScore * machine.importance;
        });
      });
    });

    return {
      raw: totalRaw,
      weighted: totalWeighted,
      rawPercent: totalPossibleRaw > 0 ? (totalRaw / totalPossibleRaw * 100) : 0,
      weightedPercent: totalPossibleWeighted > 0 ? (totalWeighted / totalPossibleWeighted * 100) : 0
    };
  };

  // Skills Gap Analysis
  const calculateSkillsGap = () => {
    const gaps = [];
    
    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        machine.competencies.forEach(comp => {
          let totalScore = 0;
          let engineerCount = 0;
          
          data.engineers.forEach(engineer => {
            const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
            totalScore += score;
            engineerCount++;
          });
          
          const avgScore = engineerCount > 0 ? totalScore / engineerCount : 0;
          const gap = comp.maxScore - avgScore;
          
          if (gap > 0.5) {
            gaps.push({
              area: area.name,
              machine: machine.name,
              competency: comp.name,
              currentAvg: avgScore.toFixed(2),
              target: comp.maxScore,
              gap: gap.toFixed(2),
              priority: gap > 2 ? 'High' : gap > 1 ? 'Medium' : 'Low'
            });
          }
        });
      });
    });
    
    return gaps.sort((a, b) => parseFloat(b.gap) - parseFloat(a.gap));
  };

  // Generate Training Plan
  const generateTrainingPlan = (engineerId) => {
    const engineer = data.engineers.find(e => e.id === engineerId);
    if (!engineer) return [];
    
    const weakAreas = [];
    
    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        machine.competencies.forEach(comp => {
          const score = getAssessmentScore(engineerId, area.id, machine.id, comp.id);
          if (score < 2) {
            weakAreas.push({
              area: area.name,
              machine: machine.name,
              competency: comp.name,
              currentScore: score,
              targetScore: comp.maxScore,
              priority: score === 0 ? 'Critical' : score === 1 ? 'High' : 'Medium'
            });
          }
        });
      });
    });
    
    return weakAreas.sort((a, b) => a.currentScore - b.currentScore);
  };

  // Create Snapshot
  const createSnapshot = () => {
    dataHook.takeSnapshot(`Snapshot at ${new Date().toLocaleString()}`);
  };

  // Export to Excel
  const handleExportToExcel = () => {
    exportToExcel(data);
  };

  // Export to JSON
  const handleExportToJSON = () => {
    const dataStr = exportBackup();
    const blob = new Blob([dataStr], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = `training-matrix-${new Date().toISOString().split('T')[0]}.json`;
    link.click();
  };

  // Import from JSON
  const handleImportJSON = (e) => {
    const file = e.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onload = (event) => {
        try {
          const jsonString = event.target.result;
          const result = importBackup(jsonString);
          if (result.success) {
            if (confirm('This will replace all current data. Continue?')) {
              const importedData = JSON.parse(jsonString);
              // Save directly to localStorage first
              localStorage.setItem('training_matrix_data', JSON.stringify(importedData));
              alert('Data imported successfully! Page will reload.');
              // Small delay to ensure localStorage write completes
              setTimeout(() => {
                window.location.reload();
              }, 100);
            }
          } else {
            alert(result.message);
          }
        } catch (error) {
          alert('Error importing file. Please check the format.');
          console.error('Import error:', error);
        }
      };
      reader.readAsText(file);
    }
    // Reset input so same file can be imported again
    e.target.value = '';
  };

  // Import from Excel
  const handleImportExcel = async (e) => {
    const file = e.target.files[0];
    if (file) {
      try {
        const imported = await importFromExcel(file);
        const validation = validateImportedData(imported);

        if (!validation.isValid) {
          alert('Import failed:\n' + validation.errors.join('\n'));
          return;
        }

        if (confirm('This will replace all current data. Continue?')) {
          const newData = {
            ...data,
            productionAreas: imported.productionAreas,
            engineers: imported.engineers,
            assessments: imported.assessments,
            users: data.users, // Preserve users
            certifications: data.certifications || [], // Preserve certifications
            snapshots: data.snapshots || []
          };
          // Save directly to localStorage
          localStorage.setItem('training_matrix_data', JSON.stringify(newData));
          alert('Excel data imported successfully! Page will reload.');
          // Small delay to ensure localStorage write completes
          setTimeout(() => {
            window.location.reload();
          }, 100);
        }
      } catch (error) {
        alert('Error importing Excel file: ' + error.message);
        console.error('Excel import error:', error);
      }
    }
    e.target.value = ''; // Reset input
  };

  // Filter engineers
  const getFilteredEngineers = () => {
    if (!data || !data.engineers) return [];
    return data.engineers.filter(engineer => {
      const matchesSearch = engineer.name.toLowerCase().includes(searchTerm.toLowerCase());
      const matchesShift = filterShift === 'all' || engineer.shift === filterShift;
      return matchesSearch && matchesShift;
    });
  };

  // Loading state
  if (loading || !data) {
    return (
      <div className="min-h-screen bg-gray-50 flex items-center justify-center">
        <div className="text-xl text-gray-600">Loading...</div>
      </div>
    );
  }

  // Render Login Screen
  if (!isAuthenticated) {
    return (
      <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100 flex items-center justify-center p-4">
        <div className="bg-white rounded-lg shadow-xl p-8 w-full max-w-md">
          <h1 className="text-3xl font-bold text-gray-800 mb-6 text-center">Training Matrix System</h1>
          <form onSubmit={handleLogin} className="space-y-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Username</label>
              <input
                type="text"
                value={loginForm.username}
                onChange={(e) => setLoginForm({ ...loginForm, username: e.target.value })}
                className="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-transparent"
                required
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Password</label>
              <input
                type="password"
                value={loginForm.password}
                onChange={(e) => setLoginForm({ ...loginForm, password: e.target.value })}
                className="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-transparent"
                required
              />
            </div>
            <button
              type="submit"
              className="w-full bg-blue-600 text-white py-2 rounded-lg hover:bg-blue-700 transition-colors font-medium"
            >
              Login
            </button>
          </form>
          <p className="mt-4 text-sm text-gray-600 text-center">
            Default: admin / admin123
          </p>
        </div>
      </div>
    );
  }

  // Main Application
  return (
    <div className="min-h-screen bg-gray-50">
      {/* Header */}
      <div className="bg-white shadow-sm border-b">
        <div className="max-w-7xl mx-auto px-4 py-4 flex justify-between items-center">
          <h1 className="text-2xl font-bold text-gray-800">Training Matrix System</h1>
          <div className="flex items-center gap-4">
            <span className="text-sm text-gray-600">
              {currentUser.role === 'admin' ? '👨‍💼 Admin' : '👷 Engineer'}: {currentUser.username}
            </span>
            <button
              onClick={handleLogout}
              className="px-4 py-2 bg-red-600 text-white rounded-lg hover:bg-red-700 transition-colors"
            >
              Logout
            </button>
          </div>
        </div>
      </div>

      {/* Tabs */}
      {currentUser.role === 'admin' && (
        <div className="bg-white border-b">
          <div className="max-w-7xl mx-auto px-4">
            <div className="flex gap-2 overflow-x-auto">
              {['admin', 'data', 'reports', 'advanced', 'assessment'].map(tab => (
                <button
                  key={tab}
                  onClick={() => setActiveTab(tab)}
                  className={`px-6 py-3 font-medium capitalize whitespace-nowrap ${
                    activeTab === tab
                      ? 'border-b-2 border-blue-600 text-blue-600'
                      : 'text-gray-600 hover:text-gray-800'
                  }`}
                >
                  {tab === 'admin' && '⚙️ '}
                  {tab === 'data' && '💾 '}
                  {tab === 'reports' && '📊 '}
                  {tab === 'advanced' && '🚀 '}
                  {tab === 'assessment' && '✍️ '}
                  {tab}
                </button>
              ))}
            </div>
          </div>
        </div>
      )}

      {/* Confirmation Modal */}
      {confirmDelete && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4">
          <div className="bg-white rounded-lg p-6 max-w-md w-full">
            <h3 className="text-lg font-bold mb-4">Confirm Delete</h3>
            <p className="text-gray-600 mb-6">{confirmDelete.message}</p>
            <div className="flex gap-3 justify-end">
              <button
                onClick={() => setConfirmDelete(null)}
                className="px-4 py-2 border border-gray-300 rounded-lg hover:bg-gray-50"
              >
                Cancel
              </button>
              <button
                onClick={confirmDelete.onConfirm}
                className="px-4 py-2 bg-red-600 text-white rounded-lg hover:bg-red-700"
              >
                Delete
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Content */}
      <div className="max-w-7xl mx-auto p-4">
        {/* Admin Tab */}
        {activeTab === 'admin' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex justify-between items-center mb-4">
                <h2 className="text-xl font-bold">Engineers</h2>
                <button
                  onClick={addEngineer}
                  className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
                >
                  <Plus size={20} /> Add Engineer
                </button>
              </div>
              
              <div className="mb-4 flex gap-3">
                <div className="flex-1">
                  <input
                    type="text"
                    placeholder="Search engineers..."
                    value={searchTerm}
                    onChange={(e) => setSearchTerm(e.target.value)}
                    className="w-full px-4 py-2 border border-gray-300 rounded-lg"
                  />
                </div>
                <select
                  value={filterShift}
                  onChange={(e) => setFilterShift(e.target.value)}
                  className="px-4 py-2 border border-gray-300 rounded-lg"
                >
                  <option value="all">All Shifts</option>
                  <option value="A Shift">A Shift</option>
                  <option value="B Shift">B Shift</option>
                  <option value="C Shift">C Shift</option>
                  <option value="D Shift">D Shift</option>
                  <option value="Day Shift">Day Shift</option>
                </select>
              </div>

              <div className="space-y-2">
                {getFilteredEngineers().map(engineer => (
                  <div key={engineer.id} className="flex justify-between items-center p-4 border border-gray-200 rounded-lg hover:bg-gray-50">
                    <div>
                      <p className="font-medium">{engineer.name}</p>
                      <p className="text-sm text-gray-600">{engineer.shift}</p>
                    </div>
                    <button
                      onClick={() => deleteEngineer(engineer.id)}
                      className="p-2 text-red-600 hover:bg-red-50 rounded"
                    >
                      <Trash2 size={20} />
                    </button>
                  </div>
                ))}
              </div>
            </div>

            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex justify-between items-center mb-4">
                <h2 className="text-xl font-bold">Production Areas</h2>
                <button
                  onClick={addProductionArea}
                  className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
                >
                  <Plus size={20} /> Add Area
                </button>
              </div>

              <div className="space-y-4">
                {data.productionAreas.map(area => (
                  <div key={area.id} className="border border-gray-200 rounded-lg p-4">
                    <div className="flex justify-between items-center mb-3">
                      <h3 className="text-lg font-semibold">{area.name}</h3>
                      <div className="flex gap-2">
                        <button
                          onClick={() => addMachine(area.id)}
                          className="px-3 py-1 bg-green-600 text-white text-sm rounded hover:bg-green-700"
                        >
                          + Machine
                        </button>
                        <button
                          onClick={() => deleteProductionArea(area.id)}
                          className="p-1 text-red-600 hover:bg-red-50 rounded"
                        >
                          <Trash2 size={18} />
                        </button>
                      </div>
                    </div>

                    <div className="space-y-3 ml-4">
                      {area.machines.map(machine => (
                        <div key={machine.id} className="border-l-2 border-gray-300 pl-4">
                          <div className="flex justify-between items-center mb-2">
                            <div>
                              <p className="font-medium">{machine.name}</p>
                              <p className="text-sm text-gray-600">Importance: {machine.importance}/10</p>
                            </div>
                            <div className="flex gap-2">
                              <button
                                onClick={() => addCompetency(area.id, machine.id)}
                                className="px-3 py-1 bg-blue-500 text-white text-sm rounded hover:bg-blue-600"
                              >
                                + Competency
                              </button>
                              <button
                                onClick={() => deleteMachine(area.id, machine.id)}
                                className="p-1 text-red-600 hover:bg-red-50 rounded"
                              >
                                <Trash2 size={16} />
                              </button>
                            </div>
                          </div>

                          <div className="space-y-1 ml-4">
                            {machine.competencies.map(comp => (
                              <div key={comp.id} className="flex justify-between items-center py-2 px-3 bg-gray-50 rounded">
                                <span className="text-sm">{comp.name} (max: {comp.maxScore})</span>
                                <button
                                  onClick={() => deleteCompetency(area.id, machine.id, comp.id)}
                                  className="p-1 text-red-600 hover:bg-red-100 rounded"
                                >
                                  <Trash2 size={14} />
                                </button>
                              </div>
                            ))}
                          </div>
                        </div>
                      ))}
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        )}

        {/* Data Management Tab */}
        {activeTab === 'data' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Data Management</h2>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                <button
                  onClick={handleExportToExcel}
                  className="flex items-center justify-center gap-2 p-4 border-2 border-blue-600 text-blue-600 rounded-lg hover:bg-blue-50"
                >
                  <Download size={24} />
                  <span className="font-medium">Export to Excel</span>
                </button>

                <label className="flex items-center justify-center gap-2 p-4 border-2 border-green-600 text-green-600 rounded-lg hover:bg-green-50 cursor-pointer">
                  <Upload size={24} />
                  <span className="font-medium">Import from Excel</span>
                  <input
                    type="file"
                    accept=".xlsx,.xls"
                    onChange={handleImportExcel}
                    className="hidden"
                  />
                </label>

                <button
                  onClick={handleExportToJSON}
                  className="flex items-center justify-center gap-2 p-4 border-2 border-indigo-600 text-indigo-600 rounded-lg hover:bg-indigo-50"
                >
                  <Download size={24} />
                  <span className="font-medium">Export to JSON</span>
                </button>

                <label className="flex items-center justify-center gap-2 p-4 border-2 border-teal-600 text-teal-600 rounded-lg hover:bg-teal-50 cursor-pointer">
                  <Upload size={24} />
                  <span className="font-medium">Import from JSON</span>
                  <input
                    type="file"
                    accept=".json"
                    onChange={handleImportJSON}
                    className="hidden"
                  />
                </label>

                <button
                  onClick={handleExportToJSON}
                  className="flex items-center justify-center gap-2 p-4 border-2 border-purple-600 text-purple-600 rounded-lg hover:bg-purple-50"
                >
                  <Save size={24} />
                  <span className="font-medium">Create Backup</span>
                </button>
              </div>

              <div className="mt-6 p-4 bg-yellow-50 border border-yellow-200 rounded-lg">
                <p className="text-sm text-yellow-800">
                  <strong>Note:</strong> Always create backups before importing data. Imports will replace all current data.
                </p>
              </div>
            </div>
          </div>
        )}

        {/* Reports Tab */}
        {activeTab === 'reports' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            {/* Overall Stats */}
            <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
              <div className="bg-white rounded-lg shadow p-6">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600">Total Engineers</p>
                    <p className="text-2xl font-bold text-blue-600">{data.engineers.length}</p>
                  </div>
                  <Users size={32} className="text-blue-600" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow p-6">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600">Production Areas</p>
                    <p className="text-2xl font-bold text-green-600">{data.productionAreas.length}</p>
                  </div>
                  <TrendingUp size={32} className="text-green-600" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow p-6">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600">Total Machines</p>
                    <p className="text-2xl font-bold text-purple-600">
                      {data.productionAreas.reduce((sum, area) => sum + area.machines.length, 0)}
                    </p>
                  </div>
                  <Award size={32} className="text-purple-600" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow p-6">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600">Avg Completion</p>
                    <p className="text-2xl font-bold text-orange-600">
                      {data.engineers.length > 0
                        ? (data.engineers.reduce((sum, eng) => sum + calculateScores(eng.id).rawPercent, 0) / data.engineers.length).toFixed(1)
                        : 0}%
                    </p>
                  </div>
                  <AlertCircle size={32} className="text-orange-600" />
                </div>
              </div>
            </div>

            {/* Skills Gap Analysis */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Skills Gap Analysis</h2>
              <div className="overflow-x-auto">
                <table className="w-full">
                  <thead>
                    <tr className="border-b">
                      <th className="text-left py-3 px-4">Area</th>
                      <th className="text-left py-3 px-4">Machine</th>
                      <th className="text-left py-3 px-4">Competency</th>
                      <th className="text-center py-3 px-4">Current Avg</th>
                      <th className="text-center py-3 px-4">Target</th>
                      <th className="text-center py-3 px-4">Gap</th>
                      <th className="text-center py-3 px-4">Priority</th>
                    </tr>
                  </thead>
                  <tbody>
                    {calculateSkillsGap().map((gap, index) => (
                      <tr key={index} className="border-b hover:bg-gray-50">
                        <td className="py-3 px-4">{gap.area}</td>
                        <td className="py-3 px-4">{gap.machine}</td>
                        <td className="py-3 px-4">{gap.competency}</td>
                        <td className="text-center py-3 px-4">{gap.currentAvg}</td>
                        <td className="text-center py-3 px-4">{gap.target}</td>
                        <td className="text-center py-3 px-4 font-bold">{gap.gap}</td>
                        <td className="text-center py-3 px-4">
                          <span className={`px-2 py-1 rounded text-xs font-medium ${
                            gap.priority === 'High' ? 'bg-red-100 text-red-800' :
                            gap.priority === 'Medium' ? 'bg-yellow-100 text-yellow-800' :
                            'bg-green-100 text-green-800'
                          }`}>
                            {gap.priority}
                          </span>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>

            {/* Engineer Progress Chart */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Engineer Progress Overview</h2>
              <ResponsiveContainer width="100%" height={400}>
                <BarChart data={data.engineers.map(eng => ({
                  name: eng.name,
                  completion: calculateScores(eng.id).rawPercent
                }))}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="name" angle={-45} textAnchor="end" height={100} />
                  <YAxis />
                  <Tooltip />
                  <Legend />
                  <Bar dataKey="completion" fill="#3B82F6" name="Completion %" />
                </BarChart>
              </ResponsiveContainer>
            </div>

            {/* Competency Heatmap */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Competency Heatmap</h2>
              <div className="overflow-x-auto">
                <table className="w-full text-sm">
                  <thead>
                    <tr>
                      <th className="border p-2 bg-gray-100">Engineer</th>
                      {data.productionAreas.flatMap(area =>
                        area.machines.flatMap(machine =>
                          machine.competencies.map(comp => (
                            <th key={`${area.id}-${machine.id}-${comp.id}`} className="border p-2 bg-gray-100 writing-mode-vertical">
                              {comp.name}
                            </th>
                          ))
                        )
                      )}
                    </tr>
                  </thead>
                  <tbody>
                    {data.engineers.map(engineer => (
                      <tr key={engineer.id}>
                        <td className="border p-2 font-medium">{engineer.name}</td>
                        {data.productionAreas.flatMap(area =>
                          area.machines.flatMap(machine =>
                            machine.competencies.map(comp => {
                              const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
                              const percentage = (score / comp.maxScore) * 100;
                              return (
                                <td
                                  key={`${engineer.id}-${area.id}-${machine.id}-${comp.id}`}
                                  className="border p-2 text-center"
                                  style={{
                                    backgroundColor: 
                                      percentage >= 80 ? '#86efac' :
                                      percentage >= 60 ? '#fde047' :
                                      percentage >= 40 ? '#fdba74' :
                                      percentage > 0 ? '#fca5a5' :
                                      '#f3f4f6'
                                  }}
                                >
                                  {score}
                                </td>
                              );
                            })
                          )
                        )}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
              <div className="mt-4 flex gap-4 text-sm">
                <div className="flex items-center gap-2">
                  <div className="w-4 h-4 bg-green-300"></div>
                  <span>80-100%</span>
                </div>
                <div className="flex items-center gap-2">
                  <div className="w-4 h-4 bg-yellow-300"></div>
                  <span>60-79%</span>
                </div>
                <div className="flex items-center gap-2">
                  <div className="w-4 h-4 bg-orange-300"></div>
                  <span>40-59%</span>
                </div>
                <div className="flex items-center gap-2">
                  <div className="w-4 h-4 bg-red-300"></div>
                  <span>1-39%</span>
                </div>
                <div className="flex items-center gap-2">
                  <div className="w-4 h-4 bg-gray-100"></div>
                  <span>0%</span>
                </div>
              </div>
            </div>
          </div>
        )}

        {/* Advanced Tab */}
        {activeTab === 'advanced' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            {/* Training Plans */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Training Plans</h2>
              <select
                onChange={(e) => {
                  if (e.target.value) {
                    const plan = generateTrainingPlan(parseInt(e.target.value));
                    setShowModal({ type: 'trainingPlan', data: plan, engineerId: e.target.value });
                  }
                }}
                className="w-full px-4 py-2 border border-gray-300 rounded-lg mb-4"
              >
                <option value="">Select engineer to generate training plan...</option>
                {data.engineers.map(eng => (
                  <option key={eng.id} value={eng.id}>{eng.name}</option>
                ))}
              </select>

              {showModal?.type === 'trainingPlan' && (
                <div className="border border-blue-200 rounded-lg p-4 bg-blue-50">
                  <div className="flex justify-between items-start mb-4">
                    <h3 className="font-bold text-lg">
                      Training Plan for {data.engineers.find(e => e.id === parseInt(showModal.engineerId))?.name}
                    </h3>
                    <button onClick={() => setShowModal(null)} className="text-gray-500 hover:text-gray-700">
                      <X size={20} />
                    </button>
                  </div>
                  <table className="w-full">
                    <thead>
                      <tr className="border-b">
                        <th className="text-left py-2">Area</th>
                        <th className="text-left py-2">Machine</th>
                        <th className="text-left py-2">Competency</th>
                        <th className="text-center py-2">Current</th>
                        <th className="text-center py-2">Target</th>
                        <th className="text-center py-2">Priority</th>
                      </tr>
                    </thead>
                    <tbody>
                      {showModal.data.map((item, index) => (
                        <tr key={index} className="border-b">
                          <td className="py-2">{item.area}</td>
                          <td className="py-2">{item.machine}</td>
                          <td className="py-2">{item.competency}</td>
                          <td className="text-center py-2">{item.currentScore}</td>
                          <td className="text-center py-2">{item.targetScore}</td>
                          <td className="text-center py-2">
                            <span className={`px-2 py-1 rounded text-xs font-medium ${
                              item.priority === 'Critical' ? 'bg-red-100 text-red-800' :
                              item.priority === 'High' ? 'bg-orange-100 text-orange-800' :
                              'bg-yellow-100 text-yellow-800'
                            }`}>
                              {item.priority}
                            </span>
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              )}
            </div>

            {/* Certifications */}
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex justify-between items-center mb-4">
                <h2 className="text-xl font-bold">Certifications</h2>
                <button
                  onClick={() => {
                    const name = prompt('Enter certification name:');
                    const days = prompt('Enter validity period (days):', '365');
                    if (name && days) {
                      dataHook.addCertification({ name, validityDays: parseInt(days) });
                    }
                  }}
                  className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
                >
                  <Plus size={20} /> Add Certification
                </button>
              </div>
              <div className="space-y-2">
                {(data.certifications || []).map(cert => (
                  <div key={cert.id} className="flex justify-between items-center p-4 border border-gray-200 rounded-lg">
                    <div>
                      <p className="font-medium">{cert.name}</p>
                      <p className="text-sm text-gray-600">Valid for {cert.validityDays} days</p>
                    </div>
                    <button
                      onClick={() => {
                        setConfirmDelete({
                          message: 'Delete this certification?',
                          onConfirm: () => {
                            dataHook.deleteCertification(cert.id);
                            setConfirmDelete(null);
                          }
                        });
                      }}
                      className="p-2 text-red-600 hover:bg-red-50 rounded"
                    >
                      <Trash2 size={20} />
                    </button>
                  </div>
                ))}
              </div>
            </div>

            {/* Audit Trail */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Audit Trail</h2>
              <div className="overflow-x-auto max-h-96 overflow-y-auto">
                <table className="w-full text-sm">
                  <thead className="bg-gray-50 sticky top-0">
                    <tr>
                      <th className="text-left py-2 px-4">Timestamp</th>
                      <th className="text-left py-2 px-4">User</th>
                      <th className="text-left py-2 px-4">Action</th>
                      <th className="text-left py-2 px-4">Details</th>
                    </tr>
                  </thead>
                  <tbody>
                    {getAuditLogs().map(log => (
                      <tr key={log.id} className="border-b hover:bg-gray-50">
                        <td className="py-2 px-4">{new Date(log.timestamp).toLocaleString()}</td>
                        <td className="py-2 px-4">{log.user}</td>
                        <td className="py-2 px-4 font-medium">{log.action}</td>
                        <td className="py-2 px-4 text-gray-600">{log.details}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>

            {/* Progress Snapshots */}
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex justify-between items-center mb-4">
                <h2 className="text-xl font-bold">Progress History</h2>
                <button
                  onClick={createSnapshot}
                  className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
                >
                  <Save size={20} /> Create Snapshot
                </button>
              </div>
              <div className="space-y-2">
                {data.snapshots.map(snapshot => (
                  <div key={snapshot.id} className="p-4 border border-gray-200 rounded-lg hover:bg-gray-50">
                    <div className="flex justify-between items-center">
                      <div>
                        <p className="font-medium">Snapshot from {new Date(snapshot.timestamp).toLocaleString()}</p>
                        <p className="text-sm text-gray-600">Created by {snapshot.createdBy}</p>
                      </div>
                      <button
                        onClick={() => {
                          const blob = new Blob([JSON.stringify(snapshot.data, null, 2)], { type: 'application/json' });
                          const url = URL.createObjectURL(blob);
                          const link = document.createElement('a');
                          link.href = url;
                          link.download = `snapshot-${snapshot.timestamp}.json`;
                          link.click();
                        }}
                        className="px-3 py-1 bg-blue-600 text-white rounded hover:bg-blue-700 text-sm"
                      >
                        Download
                      </button>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        )}

        {/* Assessment Tab */}
        {activeTab === 'assessment' && (
          <div className="space-y-6">
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Engineer Assessments</h2>
              
              {/* Filters */}
              <div className="mb-4 flex gap-3">
                <select
                  value={filterArea}
                  onChange={(e) => setFilterArea(e.target.value)}
                  className="px-4 py-2 border border-gray-300 rounded-lg"
                >
                  <option value="all">All Areas</option>
                  {data.productionAreas.map(area => (
                    <option key={area.id} value={area.id}>{area.name}</option>
                  ))}
                </select>

                {currentUser.role === 'admin' && (
                  <button
                    onClick={() => setBulkSelectMode(!bulkSelectMode)}
                    className={`px-4 py-2 rounded-lg ${
                      bulkSelectMode
                        ? 'bg-blue-600 text-white'
                        : 'border border-gray-300 hover:bg-gray-50'
                    }`}
                  >
                    {bulkSelectMode ? 'Exit Bulk Mode' : 'Bulk Update Mode'}
                  </button>
                )}
              </div>

              {/* Bulk Update Panel */}
              {bulkSelectMode && selectedEngineers.length > 0 && (
                <div className="mb-4 p-4 bg-blue-50 border-2 border-blue-300 rounded-lg">
                  <div className="flex items-center justify-between mb-3">
                    <h3 className="font-bold text-blue-900">
                      Bulk Update - {selectedEngineers.length} Engineer{selectedEngineers.length !== 1 ? 's' : ''} Selected
                    </h3>
                    <button
                      onClick={() => {
                        setSelectedEngineers([]);
                        setBulkSelectMode(false);
                      }}
                      className="text-sm text-blue-600 hover:text-blue-800"
                    >
                      Clear Selection
                    </button>
                  </div>

                  <div className="space-y-3">
                    <div>
                      <label className="block text-sm font-medium text-gray-700 mb-1">
                        Select Competency to Update:
                      </label>
                      <select
                        value={showModal?.bulkCompetency || ''}
                        onChange={(e) => setShowModal({ ...showModal, bulkCompetency: e.target.value })}
                        className="w-full px-3 py-2 border border-gray-300 rounded-lg"
                      >
                        <option value="">-- Choose a competency --</option>
                        {data.productionAreas.map(area =>
                          area.machines.map(machine =>
                            machine.competencies.map(comp => (
                              <option key={`${area.id}-${machine.id}-${comp.id}`} value={comp.id}>
                                {area.name} → {machine.name} → {comp.name}
                              </option>
                            ))
                          )
                        )}
                      </select>
                    </div>

                    {showModal?.bulkCompetency && (
                      <div>
                        <label className="block text-sm font-medium text-gray-700 mb-2">
                          Select Score:
                        </label>
                        <div className="flex gap-2">
                          {[0, 1, 2, 3].map(score => (
                            <button
                              key={score}
                              onClick={() => {
                                if (confirm(`Set score to ${score} for ${selectedEngineers.length} engineer(s)?`)) {
                                  bulkUpdateScores(showModal.bulkCompetency, score);
                                  setShowModal(null);
                                  alert(`Successfully updated ${selectedEngineers.length} engineer(s)!`);
                                }
                              }}
                              className="flex-1 px-4 py-3 bg-white border-2 border-gray-300 rounded-lg hover:border-blue-500 hover:bg-blue-50 font-bold text-lg transition-colors"
                            >
                              {score}
                            </button>
                          ))}
                        </div>
                      </div>
                    )}
                  </div>
                </div>
              )}

              {/* Engineer Selection */}
              <div className="space-y-4">
                {getFilteredEngineers().map(engineer => {
                  const scores = calculateScores(engineer.id);
                  const isEngineerView = currentUser.role === 'engineer' && currentUser.engineerId !== engineer.id;

                  if (isEngineerView) return null;

                  return (
                    <div key={engineer.id} className="border border-gray-200 rounded-lg p-4">
                      <div className="flex justify-between items-center mb-3">
                        <div className="flex items-center gap-3">
                          {bulkSelectMode && currentUser.role === 'admin' && (
                            <input
                              type="checkbox"
                              checked={selectedEngineers.includes(engineer.id)}
                              onChange={(e) => {
                                if (e.target.checked) {
                                  setSelectedEngineers([...selectedEngineers, engineer.id]);
                                } else {
                                  setSelectedEngineers(selectedEngineers.filter(id => id !== engineer.id));
                                }
                              }}
                              className="w-5 h-5"
                            />
                          )}
                          <div>
                            <h3 className="text-lg font-semibold">{engineer.name}</h3>
                            <p className="text-sm text-gray-600">{engineer.shift}</p>
                          </div>
                        </div>
                        <div className="text-right">
                          <p className="text-sm text-gray-600">Raw Score: {scores.raw.toFixed(0)}</p>
                          <p className="text-sm text-gray-600">Weighted: {scores.weighted.toFixed(0)}</p>
                          <div className="mt-1">
                            <div className="w-32 bg-gray-200 rounded-full h-2">
                              <div
                                className="bg-blue-600 h-2 rounded-full"
                                style={{ width: `${scores.rawPercent}%` }}
                              />
                            </div>
                            <p className="text-xs text-gray-500 mt-1">{scores.rawPercent.toFixed(1)}% Complete</p>
                          </div>
                        </div>
                      </div>

                      {/* Competency Assessments */}
                      {data.productionAreas
                        .filter(area => filterArea === 'all' || area.id === parseInt(filterArea))
                        .map(area => (
                          <div key={area.id} className="mt-4">
                            <h4 className="font-medium text-blue-600 mb-2">{area.name}</h4>
                            {area.machines.map(machine => (
                              <div key={machine.id} className="ml-4 mb-3">
                                <p className="text-sm font-medium text-gray-700 mb-2">
                                  {machine.name} (Importance: {machine.importance})
                                </p>
                                <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
                                  {machine.competencies.map(comp => {
                                    const currentScore = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
                                    return (
                                      <div key={comp.id} className="flex items-center justify-between p-2 bg-gray-50 rounded">
                                        <span className="text-sm">{comp.name}</span>
                                        <div className="flex gap-1">
                                          {[0, 1, 2, 3].map(value => (
                                            <button
                                              key={value}
                                              onClick={() => {
                                                if (currentUser.role === 'admin') {
                                                  updateAssessment(engineer.id, area.id, machine.id, comp.id, value);
                                                }
                                              }}
                                              disabled={currentUser.role !== 'admin'}
                                              className={`w-8 h-8 rounded text-sm font-medium transition-colors ${
                                                currentScore === value
                                                  ? 'bg-blue-600 text-white'
                                                  : currentUser.role !== 'admin'
                                                  ? 'bg-gray-200 text-gray-500 cursor-not-allowed'
                                                  : 'bg-white border-2 border-gray-300 hover:border-blue-400'
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
                        ))}
                    </div>
                  );
                })}
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

export default App;
