import React, { useState, useEffect, lazy, Suspense } from 'react';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, PieChart, Pie, Cell } from 'recharts';
import { Download, Upload, Search, Filter, Plus, Trash2, Edit2, Save, X, FileDown, Users, Award, TrendingUp, AlertCircle, Key, Moon, Sun } from 'lucide-react';
import toast, { Toaster } from 'react-hot-toast';
import { useAuth } from './hooks/useAuth';
import { useData } from './hooks/useData';
import { exportToExcel, exportEngineerReport } from './utils/excelExport';
import { importFromExcel, validateImportedData } from './utils/excelImport';
import { exportBackup, importBackup, getAuditLogs } from './utils/storage';
import { useTheme } from './contexts/ThemeContext';

// Lazy load major components for code splitting
const Dashboard = lazy(() => import('./components/Dashboard'));
const ProgressGraph = lazy(() => import('./components/ProgressGraph'));
const CoreSkills = lazy(() => import('./components/CoreSkills'));
const AdvancedAnalytics = lazy(() => import('./components/AdvancedAnalytics'));
const EngineerAnalysis = lazy(() => import('./components/EngineerAnalysis'));

const COLORS = ['#0088FE', '#00C49F', '#FFBB28', '#FF8042', '#8884d8'];

// Loading fallback component
const ComponentLoader = () => (
  <div className="flex items-center justify-center py-12">
    <div className="text-center">
      <div className="w-12 h-12 border-4 border-gray-200 border-t-accent rounded-full animate-spin mx-auto"></div>
      <p className="mt-4 text-sm text-gray-500">Loading component...</p>
    </div>
  </div>
);

function App() {
  // Authentication
  const { currentUser, isAuthenticated, login, logout } = useAuth();

  // Theme
  const { theme, toggleTheme } = useTheme();

  // Data management
  const dataHook = useData(currentUser);
  const { data, loading } = dataHook;

  // UI State
  const [loginForm, setLoginForm] = useState({ username: '', password: '' });
  const [activeTab, setActiveTab] = useState(currentUser?.role === 'engineer' ? 'mydashboard' : 'dashboard');
  const [adminSubTab, setAdminSubTab] = useState('engineers');
  const [reportsSubTab, setReportsSubTab] = useState('skillgap');
  const [useWeightedScores, setUseWeightedScores] = useState(false);
  const [searchTerm, setSearchTerm] = useState('');
  const [filterShift, setFilterShift] = useState('all');
  const [filterArea, setFilterArea] = useState('all');
  const [reportFilterArea, setReportFilterArea] = useState('all');
  const [editingItem, setEditingItem] = useState(null);
  const [confirmDelete, setConfirmDelete] = useState(null);
  const [bulkSelectMode, setBulkSelectMode] = useState(false);
  const [selectedEngineers, setSelectedEngineers] = useState([]);
  const [showModal, setShowModal] = useState(null);
  const [showShortcutsHelp, setShowShortcutsHelp] = useState(false);
  const [showGlobalSearch, setShowGlobalSearch] = useState(false);
  const [searchQuery, setSearchQuery] = useState('');

  // Login handling
  const handleLogin = async (e) => {
    e.preventDefault();
    try {
      const result = await login(loginForm.username, loginForm.password);
      if (!result.success) {
        toast.error(result.message || 'Invalid credentials');
      }
    } catch (error) {
      console.error('Login error:', error);
      toast.error('Login failed. Please try again.');
    }
  };

  const handleLogout = () => {
    logout();
    setLoginForm({ username: '', password: '' });
  };

  // Keyboard shortcuts
  useEffect(() => {
    const handleKeyPress = async (e) => {
      // Ignore if typing in an input field
      if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;

      // Cmd/Ctrl + S: Create snapshot
      if ((e.metaKey || e.ctrlKey) && e.key === 's') {
        e.preventDefault();
        if (currentUser?.role === 'admin') {
          await dataHook.takeSnapshot('Manual snapshot');
          toast.success('Snapshot created!');
        }
      }

      // Cmd/Ctrl + E: Export data
      if ((e.metaKey || e.ctrlKey) && e.key === 'e') {
        e.preventDefault();
        if (currentUser?.role === 'admin') {
          handleExportToExcel();
        }
      }

      // Cmd/Ctrl + K: Global search
      if ((e.metaKey || e.ctrlKey) && e.key === 'k') {
        e.preventDefault();
        setShowGlobalSearch(true);
      }

      // Escape: Close modals
      if (e.key === 'Escape') {
        setShowGlobalSearch(false);
        setShowShortcutsHelp(false);
      }

      // ?: Show keyboard shortcuts help
      if (e.key === '?' && !e.shiftKey) {
        e.preventDefault();
        setShowShortcutsHelp(true);
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, [currentUser, data]);

  // Production Area Management
  const addProductionArea = async () => {
    const name = prompt('Enter production area name:');
    if (name) {
      try {
        await dataHook.addProductionArea({ name });
        toast.success('Production area added!');
      } catch (error) {
        console.error('Error adding production area:', error);
        toast.error('Failed to add production area');
      }
    }
  };

  const deleteProductionArea = (areaId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this production area and all its machines?',
      onConfirm: async () => {
        try {
          await dataHook.deleteProductionArea(areaId);
          setConfirmDelete(null);
          toast.success('Production area deleted!');
        } catch (error) {
          console.error('Error deleting production area:', error);
          toast.error('Failed to delete production area');
        }
      }
    });
  };

  // Machine Management
  const addMachine = async (areaId) => {
    const name = prompt('Enter machine name:');
    const importance = prompt('Enter importance (1-10):', '5');
    if (name && importance) {
      try {
        await dataHook.addMachine(areaId, { name, importance: parseInt(importance) });
        toast.success('Machine added!');
      } catch (error) {
        console.error('Error adding machine:', error);
        toast.error('Failed to add machine');
      }
    }
  };

  const deleteMachine = (areaId, machineId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this machine and all its competencies?',
      onConfirm: async () => {
        try {
          await dataHook.deleteMachine(areaId, machineId);
          setConfirmDelete(null);
          toast.success('Machine deleted!');
        } catch (error) {
          console.error('Error deleting machine:', error);
          toast.error('Failed to delete machine');
        }
      }
    });
  };

  // Competency Management
  const addCompetency = async (areaId, machineId) => {
    const name = prompt('Enter competency name:');
    if (name) {
      try {
        await dataHook.addCompetency(areaId, machineId, { name, maxScore: 3 });
        toast.success('Competency added!');
      } catch (error) {
        console.error('Error adding competency:', error);
        toast.error('Failed to add competency');
      }
    }
  };

  const deleteCompetency = (areaId, machineId, competencyId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this competency?',
      onConfirm: async () => {
        try {
          await dataHook.deleteCompetency(areaId, machineId, competencyId);
          setConfirmDelete(null);
          toast.success('Competency deleted!');
        } catch (error) {
          console.error('Error deleting competency:', error);
          toast.error('Failed to delete competency');
        }
      }
    });
  };

  // Engineer Management
  const addEngineer = async () => {
    const name = prompt('Enter engineer name:');
    const shift = prompt('Enter shift (A Shift, B Shift, C Shift, D Shift, Day Shift):');
    if (name && shift) {
      try {
        // Create the engineer and auto-create user account in one operation
        const result = await dataHook.addEngineer({ name, shift });

        if (result.userCreated) {
          toast.success(`Engineer added! Login: ${result.username} / password`, {
            duration: 6000,
            icon: '✅',
          });
          toast.info('Engineer should change password after first login', {
            duration: 5000,
          });
        } else {
          toast.success(`Engineer added!`, {
            duration: 4000,
          });
          if (result.username) {
            toast.info(`User account "${result.username}" already exists`, {
              duration: 4000,
            });
          }
        }
      } catch (error) {
        console.error('Error adding engineer:', error);
        toast.error('Failed to add engineer. Please try again.');
      }
    }
  };

  const deleteEngineer = (engineerId) => {
    setConfirmDelete({
      message: 'Are you sure you want to delete this engineer?',
      onConfirm: async () => {
        try {
          await dataHook.deleteEngineer(engineerId);
          setConfirmDelete(null);
          toast.success('Engineer deleted!');
        } catch (error) {
          console.error('Error deleting engineer:', error);
          toast.error('Failed to delete engineer');
        }
      }
    });
  };

  // Assessment Management
  const updateAssessment = async (engineerId, areaId, machineId, competencyId, score) => {
    try {
      await dataHook.updateAssessment(engineerId, areaId, machineId, competencyId, score);
    } catch (error) {
      console.error('Error updating assessment:', error);
      toast.error('Failed to update assessment');
    }
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
  const bulkUpdateScores = async (competencyId, score) => {
    if (selectedEngineers.length === 0) {
      toast.error('Please select engineers first');
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

    try {
      await dataHook.bulkUpdateAssessments(updates);
      toast.success(`Updated ${updates.length} assessments!`);
    } catch (error) {
      console.error('Error bulk updating assessments:', error);
      toast.error('Failed to update assessments');
    }
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

    const areasToAnalyze = reportFilterArea === 'all'
      ? data.productionAreas
      : data.productionAreas.filter(area => area.id === parseInt(reportFilterArea));

    areasToAnalyze.forEach(area => {
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
  const createSnapshot = async () => {
    try {
      await dataHook.takeSnapshot(`Snapshot at ${new Date().toLocaleString()}`);
      toast.success('Snapshot created successfully!');
    } catch (error) {
      console.error('Error creating snapshot:', error);
      toast.error('Failed to create snapshot');
    }
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
              toast.success('Data imported successfully! Reloading...', {
                duration: 2000,
              });
              // Small delay to ensure localStorage write completes
              setTimeout(() => {
                window.location.reload();
              }, 100);
            }
          } else {
            toast.error(result.message);
          }
        } catch (error) {
          toast.error('Error importing file. Please check the format.');
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
          toast.error('Import failed: ' + validation.errors[0]);
          validation.errors.slice(1).forEach(err => {
            toast.error(err, { duration: 6000 });
          });
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
          toast.success('Excel data imported successfully! Reloading...', {
            duration: 2000,
          });
          // Small delay to ensure localStorage write completes
          setTimeout(() => {
            window.location.reload();
          }, 100);
        }
      } catch (error) {
        toast.error('Error importing Excel file: ' + error.message);
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
        <div className="text-center">
          <div className="relative">
            <div className="w-20 h-20 border-4 border-gray-200 border-t-accent rounded-full animate-spin mx-auto"></div>
            <div className="absolute inset-0 w-20 h-20 border-4 border-transparent border-t-accent/30 rounded-full animate-spin mx-auto" style={{ animationDuration: '1.5s' }}></div>
          </div>
          <p className="mt-6 text-xl font-semibold text-gray-900">Loading Training Matrix...</p>
          <p className="mt-2 text-sm text-gray-500">Preparing your dashboard</p>
        </div>
      </div>
    );
  }

  // Render Login Screen
  if (!isAuthenticated) {
    return (
      <div className="min-h-screen bg-gradient-to-br from-gray-100 via-gray-200 to-gray-300 flex items-center justify-center p-4">
        <div className="bg-white rounded-2xl shadow-soft-2xl p-10 w-full max-w-md">
          <div className="text-center mb-8">
            <div className="text-5xl mb-4">🎯</div>
            <h1 className="text-4xl font-bold text-gray-900 mb-2">
              Training Matrix
            </h1>
            <p className="text-gray-600">Competency Management System</p>
          </div>
          <form onSubmit={handleLogin} className="space-y-5">
            <div>
              <label className="block text-sm font-semibold text-gray-700 mb-2">Username</label>
              <input
                type="text"
                value={loginForm.username}
                onChange={(e) => setLoginForm({ ...loginForm, username: e.target.value })}
                className="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-accent focus:border-accent transition-all shadow-inner-light"
                placeholder="Enter your username"
                required
              />
            </div>
            <div>
              <label className="block text-sm font-semibold text-gray-700 mb-2">Password</label>
              <input
                type="password"
                value={loginForm.password}
                onChange={(e) => setLoginForm({ ...loginForm, password: e.target.value })}
                className="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-accent focus:border-accent transition-all shadow-inner-light"
                placeholder="Enter your password"
                required
              />
            </div>
            <button
              type="submit"
              className="w-full bg-accent text-white py-3 rounded-lg hover:bg-accent-600 transition-all duration-200 font-semibold shadow-btn hover:shadow-btn-hover"
            >
              Sign In
            </button>
          </form>
          <div className="mt-6 p-4 bg-gray-50 rounded-lg border border-gray-200">
            <p className="text-sm text-gray-700 text-center">
              <span className="font-semibold">Default Login:</span> admin / admin123
            </p>
          </div>
        </div>
      </div>
    );
  }

  // Main Application
  return (
    <div className="min-h-screen bg-bg dark:from-gray-900 dark:to-gray-800 transition-colors duration-200">
      {/* Toast Notifications */}
      <Toaster
        position="top-right"
        toastOptions={{
          duration: 4000,
          style: {
            background: '#fff',
            color: '#171717',
            boxShadow: '0 4px 6px -1px rgba(0, 0, 0, 0.08), 0 2px 4px -1px rgba(0, 0, 0, 0.04)',
            borderRadius: '0.5rem',
            padding: '1rem',
          },
          success: {
            iconTheme: {
              primary: '#10b981',
              secondary: '#fff',
            },
          },
          error: {
            iconTheme: {
              primary: '#ff6b6b',
              secondary: '#fff',
            },
          },
        }}
      />

      {/* Header */}
      <div className="bg-gray-900 dark:bg-gray-950 shadow-soft-lg">
        <div className="max-w-7xl mx-auto px-6 py-6 flex justify-between items-center">
          <div>
            <h1 className="text-3xl font-bold text-white tracking-tight">Training Matrix System</h1>
            <p className="text-gray-300 text-sm mt-1">Competency Management & Analytics</p>
          </div>
          <div className="flex items-center gap-4">
            <div className="bg-gray-800 bg-opacity-90 backdrop-blur-sm rounded-lg px-5 py-2 shadow-soft">
              <span className="text-sm text-gray-200 font-medium">
                {currentUser.role === 'admin' ? '👨‍💼 Admin' : '👷 Engineer'}: {currentUser.username}
              </span>
            </div>
            <button
              onClick={toggleTheme}
              className="p-2.5 bg-gray-800 bg-opacity-90 backdrop-blur-sm rounded-lg hover:bg-gray-700 transition-all duration-200 shadow-btn hover:shadow-btn-hover"
              title={theme === 'light' ? 'Switch to dark mode' : 'Switch to light mode'}
            >
              {theme === 'light' ? (
                <Moon className="text-gray-300" size={20} />
              ) : (
                <Sun className="text-warning" size={20} />
              )}
            </button>
            <button
              onClick={handleLogout}
              className="px-5 py-2 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all duration-200 font-medium shadow-btn hover:shadow-btn-hover"
            >
              Logout
            </button>
          </div>
        </div>
      </div>

      {/* Tabs */}
      <div className="bg-white dark:bg-gray-800 shadow-sm">
        <div className="max-w-7xl mx-auto px-4">
          <div className="flex gap-1 overflow-x-auto">
            {currentUser.role === 'admin' ? (
              ['dashboard', 'assessment', 'reports', 'coreskills', 'admin', 'data', 'advanced'].map(tab => (
                <button
                  key={tab}
                  onClick={() => setActiveTab(tab)}
                  className={`px-6 py-4 font-medium capitalize whitespace-nowrap transition-all duration-200 ${
                    activeTab === tab
                      ? 'bg-accent text-white rounded-t-lg shadow-md'
                      : 'text-gray-600 dark:text-gray-300 hover:text-accent dark:hover:text-accent-light hover:bg-red-50 dark:hover:bg-gray-700 rounded-t-lg'
                  }`}
                >
                  {tab === 'dashboard' && '📈 '}
                  {tab === 'admin' && '⚙️ '}
                  {tab === 'data' && '💾 '}
                  {tab === 'reports' && '📊 '}
                  {tab === 'advanced' && '🚀 '}
                  {tab === 'assessment' && '✍️ '}
                  {tab === 'coreskills' && '🎓 '}
                  {tab === 'coreskills' ? 'Core Skills' : tab}
                </button>
              ))
            ) : (
              ['mydashboard', 'mytraining', 'assessment', 'coreskills'].map(tab => (
                <button
                  key={tab}
                  onClick={() => setActiveTab(tab)}
                  className={`px-6 py-4 font-medium capitalize whitespace-nowrap transition-all duration-200 ${
                    activeTab === tab
                      ? 'bg-accent text-white rounded-t-lg shadow-md'
                      : 'text-gray-600 dark:text-gray-300 hover:text-accent dark:hover:text-accent-light hover:bg-red-50 dark:hover:bg-gray-700 rounded-t-lg'
                  }`}
                >
                  {tab === 'mydashboard' && '🏠 My Dashboard'}
                  {tab === 'mytraining' && '📋 My Training Plan'}
                  {tab === 'assessment' && '✍️ My Scores'}
                  {tab === 'coreskills' && '🎓 Core Skills'}
                </button>
              ))
            )}
          </div>
        </div>
      </div>

      {/* Confirmation Modal */}
      {confirmDelete && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4 backdrop-blur-sm">
          <div className="bg-white rounded-lg p-8 max-w-md w-full shadow-soft-2xl">
            <h3 className="text-xl font-bold mb-4 text-gray-900">Confirm Delete</h3>
            <p className="text-gray-600 mb-8">{confirmDelete.message}</p>
            <div className="flex gap-3 justify-end">
              <button
                onClick={() => setConfirmDelete(null)}
                className="px-5 py-2.5 border-2 border-gray-300 text-gray-700 rounded-lg hover:bg-gray-50 transition-all font-medium shadow-btn hover:shadow-btn-hover"
              >
                Cancel
              </button>
              <button
                onClick={confirmDelete.onConfirm}
                className="px-5 py-2.5 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all font-medium shadow-btn hover:shadow-btn-hover"
              >
                Delete
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Content */}
      <div className="max-w-7xl mx-auto p-4">
        {/* Dashboard Tab */}
        {activeTab === 'dashboard' && currentUser.role === 'admin' && (
          <Suspense fallback={<ComponentLoader />}>
            <Dashboard data={data} />
          </Suspense>
        )}

        {/* Core Skills Tab */}
        {activeTab === 'coreskills' && (
          <Suspense fallback={<ComponentLoader />}>
            <CoreSkills data={data} dataHook={dataHook} currentUser={currentUser} />
          </Suspense>
        )}

        {/* Admin Tab */}
        {activeTab === 'admin' && currentUser.role === 'admin' && (
          <div className="flex flex-col lg:flex-row gap-6">
            {/* Sidebar Navigation */}
            <div className="w-full lg:w-64 flex-shrink-0">
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-4 lg:sticky lg:top-4">
                <h3 className="text-sm font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider mb-4 px-3 lg:block hidden">
                  Admin Settings
                </h3>
                <nav className="flex lg:flex-col gap-2 lg:gap-1 overflow-x-auto lg:overflow-x-visible pb-2 lg:pb-0">
                  {[
                    { id: 'engineers', icon: '👷', label: 'Engineers' },
                    { id: 'production', icon: '🏭', label: 'Production Areas' },
                    { id: 'users', icon: '👤', label: 'User Accounts' }
                  ].map(item => (
                    <button
                      key={item.id}
                      onClick={() => setAdminSubTab(item.id)}
                      className={`flex-shrink-0 lg:w-full flex items-center gap-3 px-4 py-3 rounded-lg text-left transition-all duration-200 whitespace-nowrap ${
                        adminSubTab === item.id
                          ? 'bg-accent text-white shadow-btn font-medium'
                          : 'text-gray-700 dark:text-gray-300 hover:bg-gray-100 dark:hover:bg-gray-700'
                      }`}
                    >
                      <span className="text-xl">{item.icon}</span>
                      <span>{item.label}</span>
                    </button>
                  ))}
                </nav>
              </div>
            </div>

            {/* Content Area */}
            <div className="flex-1 min-w-0">{/* This ensures proper flex behavior */}

            {/* Engineers Sub-Tab */}
            {adminSubTab === 'engineers' && (
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-6">
                <div className="flex justify-between items-center mb-6">
                  <h2 className="text-xl font-bold dark:text-white text-gray-900">Engineers</h2>
                <button
                  onClick={addEngineer}
                  className="flex items-center gap-2 px-5 py-2.5 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all duration-200 shadow-btn hover:shadow-btn-hover font-medium"
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
                    className="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg bg-white dark:bg-gray-700 text-gray-900 dark:text-white placeholder-gray-500 dark:placeholder-gray-400"
                  />
                </div>
                <select
                  value={filterShift}
                  onChange={(e) => setFilterShift(e.target.value)}
                  className="px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg bg-white dark:bg-gray-700 text-gray-900 dark:text-white"
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
                {getFilteredEngineers().length === 0 ? (
                  <div className="text-center py-16 bg-gray-50 dark:bg-gray-700 rounded-lg border-2 border-dashed border-gray-300 dark:border-gray-600">
                    <Users size={64} className="mx-auto text-gray-400 mb-4" />
                    <h3 className="text-xl font-bold text-gray-700 dark:text-gray-300 mb-2">No Engineers Found</h3>
                    <p className="text-gray-500 dark:text-gray-400 mb-6">
                      {searchTerm || filterShift !== 'all'
                        ? 'Try adjusting your filters'
                        : 'Get started by adding your first engineer'}
                    </p>
                    {!searchTerm && filterShift === 'all' && (
                      <button
                        onClick={addEngineer}
                        className="inline-flex items-center gap-2 px-6 py-3 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                      >
                        <Plus size={20} /> Add Engineer
                      </button>
                    )}
                  </div>
                ) : (
                  getFilteredEngineers().map(engineer => (
                  <div key={engineer.id} className="flex justify-between items-center p-4 border border-gray-200 dark:border-gray-700 rounded-lg hover:bg-gray-50 dark:hover:bg-gray-700 bg-white dark:bg-gray-800">
                    <div>
                      <p className="font-medium dark:text-white">{engineer.name}</p>
                      <p className="text-sm text-gray-600 dark:text-gray-400">{engineer.shift}</p>
                    </div>
                    <button
                      onClick={() => deleteEngineer(engineer.id)}
                      className="p-2 text-red-600 hover:bg-red-50 rounded"
                    >
                      <Trash2 size={20} />
                    </button>
                  </div>
                ))
                )}
              </div>
            </div>
            )}

            {/* Production Areas Sub-Tab */}
            {adminSubTab === 'production' && (
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-6">
                <div className="flex justify-between items-center mb-6">
                  <h2 className="text-xl font-bold dark:text-white text-gray-900">Production Areas</h2>
                <button
                  onClick={addProductionArea}
                  className="flex items-center gap-2 px-5 py-2.5 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all duration-200 shadow-btn hover:shadow-btn-hover font-medium"
                >
                  <Plus size={20} /> Add Area
                </button>
              </div>

              <div className="space-y-4">
                {data.productionAreas.length === 0 ? (
                  <div className="text-center py-16 bg-gray-50 dark:bg-gray-700 rounded-lg border-2 border-dashed border-gray-300 dark:border-gray-600">
                    <TrendingUp size={64} className="mx-auto text-gray-400 mb-4" />
                    <h3 className="text-xl font-bold text-gray-700 dark:text-gray-300 mb-2">No Production Areas Yet</h3>
                    <p className="text-gray-500 dark:text-gray-400 mb-6">
                      Start by creating your first production area to organize machines and competencies
                    </p>
                    <button
                      onClick={addProductionArea}
                      className="inline-flex items-center gap-2 px-6 py-3 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                    >
                      <Plus size={20} /> Add Production Area
                    </button>
                  </div>
                ) : (
                  data.productionAreas.map(area => (
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
                              <p className="font-medium dark:text-white">{machine.name}</p>
                              <div className="flex items-center gap-2">
                                <p className="text-sm text-gray-600 dark:text-gray-400">Importance: {machine.importance}/10</p>
                                <button
                                  onClick={async () => {
                                    const newImportance = prompt(`Set importance for "${machine.name}" (1-10):`, machine.importance);
                                    if (newImportance !== null) {
                                      const importance = parseInt(newImportance);
                                      if (importance >= 1 && importance <= 10) {
                                        try {
                                          await dataHook.updateMachine(area.id, machine.id, { importance });
                                          toast.success(`Updated importance to ${importance}`);
                                        } catch (error) {
                                          console.error('Error updating machine importance:', error);
                                          toast.error('Failed to update importance');
                                        }
                                      } else {
                                        toast.error('Importance must be between 1 and 10');
                                      }
                                    }
                                  }}
                                  className="p-1 text-accent hover:bg-red-50 dark:hover:bg-gray-700 rounded"
                                  title="Edit Importance"
                                >
                                  <Edit2 size={14} />
                                </button>
                              </div>
                            </div>
                            <div className="flex gap-2">
                              <button
                                onClick={() => addCompetency(area.id, machine.id)}
                                className="px-3 py-1 bg-red-500 text-white text-sm rounded hover:bg-blue-600"
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
                ))
                )}
              </div>
            </div>
            )}

            {/* User Accounts Sub-Tab */}
            {adminSubTab === 'users' && (
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-6">
                <div className="flex justify-between items-center mb-6">
                  <h2 className="text-xl font-bold dark:text-white text-gray-900">User Accounts</h2>
                <button
                  onClick={async () => {
                    const username = prompt('Enter username:');
                    if (!username) return;

                    const password = prompt('Enter password:');
                    if (!password) return;

                    const role = prompt('Enter role (admin or engineer):');
                    if (role !== 'admin' && role !== 'engineer') {
                      toast.error('Role must be "admin" or "engineer"');
                      return;
                    }

                    let engineerId = null;
                    if (role === 'engineer') {
                      const engineerName = prompt('Link to which engineer? Enter engineer name:');
                      const engineer = data.engineers.find(e => e.name.toLowerCase() === engineerName.toLowerCase());
                      if (!engineer) {
                        toast.error('Engineer not found. Please add the engineer first.');
                        return;
                      }
                      engineerId = engineer.id;
                    }

                    try {
                      await dataHook.addUser({ username, password, role, engineerId });
                      toast.success(`User "${username}" created successfully!`);
                    } catch (error) {
                      console.error('Error adding user:', error);
                      toast.error('Failed to add user. Please try again.');
                    }
                  }}
                  className="flex items-center gap-2 px-5 py-2.5 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all duration-200 shadow-btn hover:shadow-btn-hover font-medium"
                >
                  <Plus size={20} /> Add User
                </button>
              </div>

              <div className="space-y-2">
                {data.users.map(user => (
                  <div key={user.id} className="flex justify-between items-center p-4 border border-gray-200 rounded-lg hover:bg-gray-50">
                    <div>
                      <p className="font-medium">{user.username}</p>
                      <p className="text-sm text-gray-600">
                        Role: {user.role === 'admin' ? '👨‍💼 Admin' : '👷 Engineer'}
                        {user.engineerId && ` (${data.engineers.find(e => e.id === user.engineerId)?.name || 'Unknown'})`}
                      </p>
                    </div>
                    <div className="flex gap-2">
                      <button
                        onClick={async () => {
                          const newPassword = prompt(`Reset password for "${user.username}".\n\nEnter new password:`);
                          if (newPassword) {
                            try {
                              await dataHook.resetPassword(user.id, newPassword);
                              toast.success(`Password reset for "${user.username}"!`);
                            } catch (error) {
                              console.error('Error resetting password:', error);
                              toast.error('Failed to reset password');
                            }
                          }
                        }}
                        className="p-2 text-accent hover:bg-red-50 rounded"
                        title="Reset Password"
                      >
                        <Key size={20} />
                      </button>
                      {user.username !== 'admin' && (
                        <button
                          onClick={async () => {
                            if (confirm(`Delete user "${user.username}"?`)) {
                              try {
                                await dataHook.deleteUser(user.id);
                                toast.success(`User "${user.username}" deleted!`);
                              } catch (error) {
                                console.error('Error deleting user:', error);
                                toast.error('Failed to delete user');
                              }
                            }
                          }}
                          className="p-2 text-red-600 hover:bg-red-50 rounded"
                          title="Delete User"
                        >
                          <Trash2 size={20} />
                        </button>
                      )}
                    </div>
                  </div>
                ))}
              </div>

              <div className="mt-4 p-4 bg-red-50 border border-blue-200 rounded-lg">
                <p className="text-sm text-blue-800">
                  <strong>💡 Tip:</strong> Engineer users can only see and update their own scores.
                  Admin users can see and manage everything.
                </p>
              </div>
            </div>
            )}
            </div>
          </div>
        )}

        {/* Data Management Tab */}
        {activeTab === 'data' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            <div className="bg-white rounded-lg shadow-card p-8">
              <h2 className="text-2xl font-bold mb-6 text-gray-900">Data Management</h2>

              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                <button
                  onClick={handleExportToExcel}
                  className="flex items-center justify-center gap-3 p-5 border-2 border-gray-300 text-gray-700 rounded-lg hover:border-accent hover:bg-gray-50 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                >
                  <Download size={24} />
                  <span className="font-medium">Export to Excel</span>
                </button>

                <label className="flex items-center justify-center gap-3 p-5 border-2 border-gray-300 text-gray-700 rounded-lg hover:border-success hover:bg-gray-50 cursor-pointer transition-all shadow-btn hover:shadow-btn-hover font-medium">
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
                  className="flex items-center justify-center gap-3 p-5 border-2 border-gray-300 text-gray-700 rounded-lg hover:border-accent hover:bg-gray-50 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                >
                  <Download size={24} />
                  <span className="font-medium">Export to JSON</span>
                </button>

                <label className="flex items-center justify-center gap-3 p-5 border-2 border-gray-300 text-gray-700 rounded-lg hover:border-success hover:bg-gray-50 cursor-pointer transition-all shadow-btn hover:shadow-btn-hover font-medium">
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
                  className="flex items-center justify-center gap-3 p-5 border-2 border-gray-300 text-gray-700 rounded-lg hover:border-accent hover:bg-gray-50 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                >
                  <Save size={24} />
                  <span className="font-medium">Create Backup</span>
                </button>
              </div>

              <div className="mt-6 p-4 bg-gray-50 border border-gray-200 rounded-lg">
                <p className="text-sm text-gray-700">
                  <strong>Note:</strong> Always create backups before importing data. Imports will replace all current data.
                </p>
              </div>
            </div>
          </div>
        )}

        {/* Reports Tab */}
        {activeTab === 'reports' && currentUser.role === 'admin' && (
          <div className="flex flex-col lg:flex-row gap-6">
            {/* Sidebar Navigation */}
            <div className="w-full lg:w-64 flex-shrink-0">
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow-card p-4 lg:sticky lg:top-4">
                <h3 className="text-sm font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider mb-4 px-3 lg:block hidden">
                  Reports
                </h3>
                <nav className="flex lg:flex-col gap-2 lg:gap-1 overflow-x-auto lg:overflow-x-visible pb-2 lg:pb-0 mb-4 lg:mb-6">
                  {[
                    { id: 'skillgap', icon: '📊', label: 'Skill Gap' },
                    { id: 'progress', icon: '📈', label: 'Progress' },
                    { id: 'heatmap', icon: '🗺️', label: 'Heatmap' },
                    { id: 'shifts', icon: '🔄', label: 'Shifts' },
                    { id: 'individual', icon: '👤', label: 'Individual' }
                  ].map(item => (
                    <button
                      key={item.id}
                      onClick={() => setReportsSubTab(item.id)}
                      className={`flex-shrink-0 lg:w-full flex items-center gap-3 px-4 py-3 rounded-lg text-left transition-all duration-200 whitespace-nowrap ${
                        reportsSubTab === item.id
                          ? 'bg-accent text-white shadow-btn font-medium'
                          : 'text-gray-700 dark:text-gray-300 hover:bg-gray-100 dark:hover:bg-gray-700'
                      }`}
                    >
                      <span className="text-xl">{item.icon}</span>
                      <span>{item.label}</span>
                    </button>
                  ))}
                </nav>

                {/* Production Area Filter */}
                <div className="mb-4">
                  <label className="block text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider mb-2 px-3">
                    Filter by Area
                  </label>
                  <select
                    value={reportFilterArea}
                    onChange={(e) => setReportFilterArea(e.target.value)}
                    className="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg bg-white dark:bg-gray-700 text-gray-900 dark:text-white text-sm focus:ring-2 focus:ring-accent focus:border-accent"
                  >
                    <option value="all">All Areas</option>
                    {data.productionAreas.map(area => (
                      <option key={area.id} value={area.id}>{area.name}</option>
                    ))}
                  </select>
                </div>

                {/* Weighted/Unweighted Toggle */}
                <div className="pt-4 border-t border-gray-200 dark:border-gray-700">
                  <div className="flex items-center justify-between gap-3 px-3 py-2">
                    <span className="text-sm font-medium text-gray-700 dark:text-gray-300">
                      {useWeightedScores ? '⚖️ Weighted' : '📊 Unweighted'}
                    </span>
                    <button
                      onClick={() => setUseWeightedScores(!useWeightedScores)}
                      className={`relative inline-flex h-6 w-11 items-center rounded-full transition-colors shadow-inner-light ${
                        useWeightedScores ? 'bg-accent' : 'bg-gray-300 dark:bg-gray-600'
                      }`}
                      title={useWeightedScores ? 'Switch to unweighted scores' : 'Switch to weighted scores (by machine importance)'}
                    >
                      <span
                        className={`inline-block h-4 w-4 transform rounded-full bg-white transition-transform shadow-soft ${
                          useWeightedScores ? 'translate-x-6' : 'translate-x-1'
                        }`}
                      />
                    </button>
                  </div>
                </div>
              </div>
            </div>

            {/* Content Area */}
            <div className="flex-1 min-w-0 space-y-6">{/* This ensures proper flex behavior */}

            {/* Skill Gap Analysis Sub-Tab */}
            {reportsSubTab === 'skillgap' && (
              <div className="space-y-6">
                {/* Check if there's data to analyze */}
                {data.engineers.length === 0 || data.productionAreas.length === 0 ? (
                  <div className="bg-white rounded-lg shadow-card p-12 text-center">
                    <div className="text-6xl mb-4">📊</div>
                    <h3 className="text-2xl font-bold text-gray-900 mb-2">No Data Available</h3>
                    <p className="text-gray-600 mb-4">
                      Add engineers and production areas to start seeing skill gap analysis.
                    </p>
                    <div className="text-left max-w-md mx-auto bg-blue-50 border border-blue-200 rounded-lg p-4">
                      <p className="font-semibold text-blue-900 mb-2">Quick Start:</p>
                      <ol className="list-decimal list-inside space-y-1 text-sm text-blue-800">
                        <li>Go to Admin → Engineers to add engineers</li>
                        <li>Production areas are already loaded from your import</li>
                        <li>Start scoring competencies in the Dashboard tab</li>
                        <li>Return here to see insights and analysis</li>
                      </ol>
                    </div>
                  </div>
                ) : (
                  <>
                {/* Overall Stats */}
                <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
              <div className="bg-white rounded-lg shadow-card p-6 border-l-4 border-accent">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600 font-medium">Total Engineers</p>
                    <p className="text-3xl font-bold text-gray-900">{data.engineers.length}</p>
                  </div>
                  <Users size={32} className="text-accent" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow-card p-6 border-l-4 border-success">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600 font-medium">Production Areas</p>
                    <p className="text-3xl font-bold text-gray-900">
                      {reportFilterArea === 'all' ? data.productionAreas.length : 1}
                    </p>
                  </div>
                  <TrendingUp size={32} className="text-success" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow-card p-6 border-l-4 border-gray-500">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600 font-medium">Total Machines</p>
                    <p className="text-3xl font-bold text-gray-900">
                      {(reportFilterArea === 'all'
                        ? data.productionAreas
                        : data.productionAreas.filter(area => area.id === parseInt(reportFilterArea))
                      ).reduce((sum, area) => sum + area.machines.length, 0)}
                    </p>
                  </div>
                  <Award size={32} className="text-gray-500" />
                </div>
              </div>

              <div className="bg-white rounded-lg shadow-card p-6 border-l-4 border-warning">
                <div className="flex items-center justify-between">
                  <div>
                    <p className="text-sm text-gray-600 font-medium">Avg Completion</p>
                    <p className="text-3xl font-bold text-gray-900">
                      {data.engineers.length > 0
                        ? (data.engineers.reduce((sum, eng) => {
                            const scores = calculateScores(eng.id);
                            return sum + (useWeightedScores ? scores.weightedPercent : scores.rawPercent);
                          }, 0) / data.engineers.length).toFixed(1)
                        : 0}%
                    </p>
                  </div>
                  <AlertCircle size={32} className="text-warning" />
                </div>
              </div>
            </div>

            {/* Skills Gap Analysis */}
            <div className="bg-white rounded-lg shadow-card p-6">
              <h2 className="text-2xl font-bold mb-6 text-gray-900">Skills Gap Analysis</h2>
              <div className="overflow-x-auto">
                <table className="w-full">
                  <thead>
                    <tr className="border-b-2 border-gray-200 bg-gray-50">
                      <th className="text-left py-4 px-4 font-semibold text-gray-700">Area</th>
                      <th className="text-left py-4 px-4 font-semibold text-gray-700">Machine</th>
                      <th className="text-left py-4 px-4 font-semibold text-gray-700">Competency</th>
                      <th className="text-center py-4 px-4 font-semibold text-gray-700">Current Avg</th>
                      <th className="text-center py-4 px-4 font-semibold text-gray-700">Target</th>
                      <th className="text-center py-4 px-4 font-semibold text-gray-700">Gap</th>
                      <th className="text-center py-4 px-4 font-semibold text-gray-700">Priority</th>
                    </tr>
                  </thead>
                  <tbody>
                    {calculateSkillsGap().map((gap, index) => (
                      <tr key={index} className="border-b border-gray-200 hover:bg-gray-50 transition-colors">
                        <td className="py-4 px-4 text-gray-900">{gap.area}</td>
                        <td className="py-4 px-4 text-gray-900">{gap.machine}</td>
                        <td className="py-4 px-4 text-gray-900">{gap.competency}</td>
                        <td className="text-center py-4 px-4 text-gray-700">{gap.currentAvg}</td>
                        <td className="text-center py-4 px-4 text-gray-700">{gap.target}</td>
                        <td className="text-center py-4 px-4 font-bold text-gray-900">{gap.gap}</td>
                        <td className="text-center py-4 px-4">
                          <span className={`px-3 py-1 rounded-full text-xs font-semibold shadow-soft ${
                            gap.priority === 'High' ? 'bg-accent-100 text-accent-800' :
                            gap.priority === 'Medium' ? 'bg-warning/10 text-warning' :
                            'bg-success/10 text-success'
                          }`}>
                            {gap.priority}
                          </span>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
                {calculateSkillsGap().length === 0 && (
                  <div className="text-center py-8 text-gray-500">
                    <p className="text-lg font-medium">No skill gaps detected!</p>
                    <p className="text-sm mt-1">All competencies are being met. Great job! 🎉</p>
                  </div>
                )}
              </div>
            </div>
                  </>
                )}
              </div>
            )}

            {/* Engineer Progress Sub-Tab */}
            {reportsSubTab === 'progress' && (
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Engineer Progress Overview</h2>
              {data.engineers.length === 0 ? (
                <div className="text-center py-12 text-gray-500">
                  <div className="text-6xl mb-4">📈</div>
                  <p className="text-lg font-medium">No Engineers Yet</p>
                  <p className="text-sm mt-2">Add engineers in the Admin tab to see progress charts.</p>
                </div>
              ) : (
              <ResponsiveContainer width="100%" height={400}>
                <BarChart data={data.engineers.map(eng => {
                  const scores = calculateScores(eng.id);
                  return {
                    name: eng.name,
                    completion: useWeightedScores ? scores.weightedPercent : scores.rawPercent
                  };
                })}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="name" angle={-45} textAnchor="end" height={100} />
                  <YAxis />
                  <Tooltip />
                  <Legend />
                  <Bar dataKey="completion" fill="#3B82F6" name="Completion %" />
                </BarChart>
              </ResponsiveContainer>
              )}
            </div>
            )}

            {/* Competency Heatmap Sub-Tab */}
            {reportsSubTab === 'heatmap' && (
              <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Competency Heatmap</h2>
              {data.engineers.length === 0 || data.productionAreas.length === 0 ? (
                <div className="text-center py-12 text-gray-500">
                  <div className="text-6xl mb-4">🗺️</div>
                  <p className="text-lg font-medium">No Data for Heatmap</p>
                  <p className="text-sm mt-2">Add engineers and score competencies to see the heatmap.</p>
                </div>
              ) : (
              <>
                <div className="overflow-x-auto">
                  <table className="w-full text-sm">
                    <thead>
                      <tr>
                        <th className="border p-2 bg-gray-100">Engineer</th>
                        {(reportFilterArea === 'all'
                          ? data.productionAreas
                          : data.productionAreas.filter(area => area.id === parseInt(reportFilterArea))
                        ).flatMap(area =>
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
                          {(reportFilterArea === 'all'
                            ? data.productionAreas
                            : data.productionAreas.filter(area => area.id === parseInt(reportFilterArea))
                          ).flatMap(area =>
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
              </>
              )}
            </div>
            )}

            {/* Shift Comparison Sub-Tab */}
            {reportsSubTab === 'shifts' && (
              <div className="space-y-6">
                <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                  <h2 className="text-xl font-bold dark:text-white mb-4">Shift Comparison Analysis</h2>
                  <p className="text-gray-600 dark:text-gray-400 mb-6">Compare performance across different shifts</p>

                  {/* Shift Performance Comparison */}
                  <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
                    {['A Shift', 'B Shift', 'C Shift', 'D Shift', 'Day Shift'].map(shift => {
                      const shiftEngineers = data.engineers.filter(e => e.shift === shift);
                      const avgCompletion = shiftEngineers.length > 0
                        ? (shiftEngineers.reduce((sum, eng) => {
                            const scores = calculateScores(eng.id);
                            return sum + (useWeightedScores ? scores.weightedPercent : scores.rawPercent);
                          }, 0) / shiftEngineers.length).toFixed(1)
                        : 0;

                      return (
                        <div key={shift} className="bg-gradient-to-br from-blue-50 to-purple-50 dark:from-gray-700 dark:to-gray-600 rounded-lg p-6 border-2 border-blue-200 dark:border-gray-500">
                          <h3 className="text-lg font-bold text-gray-900 dark:text-white mb-3">{shift}</h3>
                          <div className="space-y-2">
                            <div className="flex justify-between">
                              <span className="text-gray-600 dark:text-gray-300">Engineers:</span>
                              <span className="font-bold dark:text-white">{shiftEngineers.length}</span>
                            </div>
                            <div className="flex justify-between">
                              <span className="text-gray-600 dark:text-gray-300">Avg Completion:</span>
                              <span className="font-bold text-accent">{avgCompletion}%</span>
                            </div>
                          </div>
                          <div className="mt-4">
                            <div className="w-full bg-gray-200 dark:bg-gray-600 rounded-full h-3">
                              <div
                                className="bg-blue-600 h-3 rounded-full transition-all duration-300"
                                style={{ width: `${avgCompletion}%` }}
                              ></div>
                            </div>
                          </div>
                        </div>
                      );
                    })}
                  </div>
                </div>
              </div>
            )}

            {/* Individual Analysis Sub-Tab */}
            {reportsSubTab === 'individual' && (
              <div className="space-y-6">
                <div className="bg-white dark:bg-gray-800 rounded-lg shadow-md p-6">
                  <h2 className="text-xl font-bold dark:text-white mb-2">Individual Engineer Analysis</h2>
                  <p className="text-gray-600 dark:text-gray-400 mb-6">
                    Deep dive into individual engineer performance with interactive visualizations
                  </p>

                  {/* Engineer Selection */}
                  <select
                    onChange={(e) => setShowModal({ type: 'individualAnalysis', engineerId: e.target.value })}
                    value={showModal?.type === 'individualAnalysis' ? showModal.engineerId : ''}
                    className="w-full px-4 py-3 border-2 border-accent rounded-lg mb-6 bg-white dark:bg-gray-700 text-gray-900 dark:text-white font-medium shadow-sm"
                  >
                    <option value="">🔍 Select an engineer to analyze...</option>
                    {data.engineers.map(eng => (
                      <option key={eng.id} value={eng.id}>
                        {eng.name} - {eng.shift}
                      </option>
                    ))}
                  </select>
                </div>

                {/* Enhanced Engineer Analysis Component */}
                {showModal?.type === 'individualAnalysis' && showModal.engineerId && (() => {
                  const engineer = data.engineers.find(e => e.id === showModal.engineerId);
                  return engineer ? (
                    <Suspense fallback={<ComponentLoader />}>
                      <EngineerAnalysis
                        data={data}
                        engineer={engineer}
                        getAssessmentScore={getAssessmentScore}
                        useWeightedScores={useWeightedScores}
                      />
                    </Suspense>
                  ) : null;
                })()}
              </div>
            )}
            </div>
          </div>
        )}

        {/* Advanced Tab */}
        {activeTab === 'advanced' && currentUser.role === 'admin' && (
          <Suspense fallback={<ComponentLoader />}>
            <AdvancedAnalytics data={data} />
          </Suspense>
        )}

        {/* Training Management Tab (moved from Advanced) */}
        {activeTab === 'training-mgmt' && currentUser.role === 'admin' && (
          <div className="space-y-6">
            {/* Training Plans */}
            <div className="bg-white rounded-lg shadow p-6">
              <h2 className="text-xl font-bold mb-4">Training Plans</h2>
              <select
                onChange={(e) => {
                  if (e.target.value) {
                    const plan = generateTrainingPlan(parseInt(e.target.value));
                    console.log('Training plan generated:', plan); // Debug log
                    setShowModal({ type: 'trainingPlan', data: plan, engineerId: e.target.value });
                  } else {
                    setShowModal(null);
                  }
                }}
                value={showModal?.type === 'trainingPlan' ? showModal.engineerId : ''}
                className="w-full px-4 py-2 border border-gray-300 rounded-lg mb-4"
              >
                <option value="">Select engineer to generate training plan...</option>
                {data.engineers.map(eng => (
                  <option key={eng.id} value={eng.id}>{eng.name}</option>
                ))}
              </select>

              {showModal?.type === 'trainingPlan' && (
                <div className="mt-4 border-2 border-accent rounded-lg p-6 bg-red-50 animate-fadeIn">
                  <div className="flex justify-between items-start mb-4">
                    <h3 className="font-bold text-lg">
                      Training Plan for {data.engineers.find(e => e.id === showModal.engineerId)?.name}
                    </h3>
                    <button onClick={() => setShowModal(null)} className="text-gray-500 hover:text-gray-700">
                      <X size={20} />
                    </button>
                  </div>

                  {showModal.data.length === 0 ? (
                    <div className="text-center py-12 bg-green-50 border-2 border-green-300 rounded-lg">
                      <div className="text-6xl mb-4">🎉</div>
                      <p className="text-green-700 font-bold text-2xl mb-3">Excellent Work!</p>
                      <p className="text-gray-700 text-lg">
                        <strong>{data.engineers.find(e => e.id === showModal.engineerId)?.name}</strong> has scored <strong>2 or higher</strong> on all competencies.
                      </p>
                      <p className="text-green-600 font-medium mt-2">
                        ✓ No training currently needed
                      </p>
                    </div>
                  ) : (
                    <>
                      <div className="mb-3 p-3 bg-white rounded border border-accent">
                        <p className="text-sm font-medium text-primary-navy">
                          📋 Found {showModal.data.length} competenc{showModal.data.length === 1 ? 'y' : 'ies'} needing training
                        </p>
                        <p className="text-xs text-gray-600 mt-1">
                          Showing competencies scored below 2 (0=Not trained, 1=Basic, 2=Competent, 3=Expert)
                        </p>
                      </div>

                      <div className="max-h-96 overflow-y-auto">
                        <table className="w-full">
                          <thead className="bg-red-100 sticky top-0">
                            <tr className="border-b">
                              <th className="text-left py-2 px-2">Area</th>
                              <th className="text-left py-2 px-2">Machine</th>
                              <th className="text-left py-2 px-2">Competency</th>
                              <th className="text-center py-2 px-2">Current</th>
                              <th className="text-center py-2 px-2">Target</th>
                              <th className="text-center py-2 px-2">Priority</th>
                            </tr>
                          </thead>
                          <tbody>
                            {showModal.data.map((item, index) => (
                              <tr key={index} className="border-b hover:bg-white">
                                <td className="py-2 px-2 text-sm">{item.area}</td>
                                <td className="py-2 px-2 text-sm">{item.machine}</td>
                                <td className="py-2 px-2 text-sm">{item.competency}</td>
                                <td className="text-center py-2 px-2 font-bold">{item.currentScore}</td>
                                <td className="text-center py-2 px-2">{item.targetScore}</td>
                                <td className="text-center py-2 px-2">
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
                    </>
                  )}
                </div>
              )}
            </div>

            {/* Certifications */}
            <div className="bg-white rounded-lg shadow p-6">
              <div className="flex justify-between items-center mb-4">
                <h2 className="text-xl font-bold">Certifications</h2>
                <button
                  onClick={async () => {
                    const name = prompt('Enter certification name:');
                    const days = prompt('Enter validity period (days):', '365');
                    if (name && days) {
                      try {
                        await dataHook.addCertification({ name, validityDays: parseInt(days) });
                        toast.success('Certification added!');
                      } catch (error) {
                        console.error('Error adding certification:', error);
                        toast.error('Failed to add certification');
                      }
                    }
                  }}
                  className="flex items-center gap-2 px-4 py-2 bg-accent text-white rounded-lg hover:bg-accent-light shadow-md"
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
                          onConfirm: async () => {
                            try {
                              await dataHook.deleteCertification(cert.id);
                              setConfirmDelete(null);
                              toast.success('Certification deleted!');
                            } catch (error) {
                              console.error('Error deleting certification:', error);
                              toast.error('Failed to delete certification');
                            }
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
                  className="flex items-center gap-2 px-4 py-2 bg-accent text-white rounded-lg hover:bg-accent-light shadow-md"
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
                        className="px-3 py-1 bg-accent text-white rounded hover:bg-accent-light text-sm"
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

        {/* Engineer Dashboard Tab */}
        {activeTab === 'mydashboard' && currentUser.role === 'engineer' && (
          <div className="space-y-6">
            {(() => {
              const engineer = data.engineers.find(e => e.id === currentUser.engineerId);
              if (!engineer) {
                return <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                  <p className="text-gray-600 dark:text-gray-400">Engineer profile not found. Please contact an administrator.</p>
                </div>;
              }

              const scores = calculateScores(engineer.id);

              return (
                <>
                  {/* Welcome Card */}
                  <div className="bg-gradient-to-r from-gray-900 to-gray-800 rounded-xl shadow-soft-xl p-8 text-white">
                    <h1 className="text-3xl font-bold mb-2">Welcome, {engineer.name}!</h1>
                    <p className="text-gray-300 text-lg">{engineer.shift}</p>
                    <div className="mt-8 grid grid-cols-1 md:grid-cols-3 gap-6">
                      <div className="bg-white bg-opacity-10 backdrop-blur-sm rounded-lg p-5 border border-white border-opacity-20 shadow-soft">
                        <p className="text-sm text-gray-300 mb-2 font-medium">Overall Completion</p>
                        <p className="text-5xl font-bold text-accent">{scores.rawPercent.toFixed(1)}%</p>
                      </div>
                      <div className="bg-white bg-opacity-10 backdrop-blur-sm rounded-lg p-5 border border-white border-opacity-20 shadow-soft">
                        <p className="text-sm text-gray-300 mb-2 font-medium">Total Competencies</p>
                        <p className="text-5xl font-bold">{data.productionAreas.reduce((sum, a) => sum + a.machines.reduce((s, m) => s + m.competencies.length, 0), 0)}</p>
                      </div>
                      <div className="bg-white bg-opacity-10 backdrop-blur-sm rounded-lg p-5 border border-white border-opacity-20 shadow-soft">
                        <p className="text-sm text-gray-300 mb-2 font-medium">Competencies Mastered</p>
                        <p className="text-5xl font-bold text-success">
                          {data.productionAreas.reduce((count, area) => {
                            return count + area.machines.reduce((mCount, machine) => {
                              return mCount + machine.competencies.filter(comp =>
                                getAssessmentScore(engineer.id, area.id, machine.id, comp.id) >= 2
                              ).length;
                            }, 0);
                          }, 0)}
                        </p>
                      </div>
                    </div>
                  </div>

                  {/* Performance by Production Area */}
                  <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
                    <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-6">My Skills by Production Area</h2>
                    <div className="space-y-4">
                      {data.productionAreas.map(area => {
                        let areaTotal = 0;
                        let areaMax = 0;
                        let areaMastered = 0;

                        area.machines.forEach(machine => {
                          machine.competencies.forEach(comp => {
                            const score = getAssessmentScore(engineer.id, area.id, machine.id, comp.id);
                            areaTotal += score;
                            areaMax += comp.maxScore;
                            if (score >= 2) areaMastered++;
                          });
                        });

                        const areaPercent = areaMax > 0 ? ((areaTotal / areaMax) * 100).toFixed(1) : 0;
                        const totalComps = area.machines.reduce((sum, m) => sum + m.competencies.length, 0);

                        return (
                          <div key={area.id} className="border-2 border-gray-200 dark:border-gray-700 rounded-lg p-5 hover:border-accent dark:hover:border-blue-500 transition-colors">
                            <div className="flex justify-between items-start mb-3">
                              <div>
                                <h3 className="text-lg font-bold text-gray-900 dark:text-white">{area.name}</h3>
                                <p className="text-sm text-gray-600 dark:text-gray-400">{areaMastered} of {totalComps} competencies mastered</p>
                              </div>
                              <div className="text-right">
                                <p className="text-3xl font-bold text-accent">{areaPercent}%</p>
                                <p className="text-xs text-gray-500 dark:text-gray-400">completion</p>
                              </div>
                            </div>
                            <div className="relative">
                              <div className="w-full bg-gray-200 dark:bg-gray-600 rounded-full h-4 overflow-hidden">
                                <div
                                  className="bg-gradient-to-r from-accent to-accent-light h-4 rounded-full transition-all duration-300 flex items-center justify-end pr-2"
                                  style={{ width: `${areaPercent}%` }}
                                >
                                  {areaPercent > 10 && (
                                    <span className="text-xs font-bold text-white">{areaPercent}%</span>
                                  )}
                                </div>
                              </div>
                            </div>
                          </div>
                        );
                      })}
                    </div>
                  </div>

                  {/* Progress Graph */}
                  <Suspense fallback={<ComponentLoader />}>
                    <ProgressGraph data={data} engineerId={engineer.id} />
                  </Suspense>

                  {/* Recent Updates / Historical Progress */}
                  {data.snapshots && data.snapshots.length > 0 && (
                    <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
                      <h2 className="text-2xl font-bold text-gray-900 dark:text-white mb-6">Progress History</h2>
                      <div className="space-y-3">
                        {data.snapshots.slice(-5).reverse().map((snapshot, index) => {
                          // Calculate score at this snapshot
                          let snapshotTotal = 0;
                          let snapshotMax = 0;

                          data.productionAreas.forEach(area => {
                            area.machines.forEach(machine => {
                              machine.competencies.forEach(comp => {
                                const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
                                const assessment = snapshot.data.assessments[key];
                                const score = typeof assessment === 'object' ? (assessment?.score || 0) : (assessment || 0);
                                snapshotTotal += score;
                                snapshotMax += comp.maxScore;
                              });
                            });
                          });

                          const snapshotPercent = snapshotMax > 0 ? ((snapshotTotal / snapshotMax) * 100).toFixed(1) : 0;

                          return (
                            <div key={snapshot.id} className="flex items-center justify-between p-4 border border-gray-200 dark:border-gray-700 rounded-lg hover:bg-gray-50 dark:hover:bg-gray-700">
                              <div>
                                <p className="font-medium text-gray-900 dark:text-white">{snapshot.name}</p>
                                <p className="text-sm text-gray-600 dark:text-gray-400">{new Date(snapshot.timestamp).toLocaleDateString()}</p>
                              </div>
                              <div className="text-right">
                                <p className="text-2xl font-bold text-accent">{snapshotPercent}%</p>
                                <p className="text-xs text-gray-500 dark:text-gray-400">completion</p>
                              </div>
                            </div>
                          );
                        })}
                      </div>
                    </div>
                  )}
                </>
              );
            })()}
          </div>
        )}

        {/* Engineer Training Plan Tab */}
        {activeTab === 'mytraining' && currentUser.role === 'engineer' && (
          <div className="space-y-6">
            {(() => {
              const engineer = data.engineers.find(e => e.id === currentUser.engineerId);
              if (!engineer) {
                return <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
                  <p className="text-gray-600 dark:text-gray-400">Engineer profile not found.</p>
                </div>;
              }

              const trainingPlan = generateTrainingPlan(engineer.id);

              return (
                <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6">
                  <h1 className="text-3xl font-bold text-gray-900 dark:text-white mb-2">My Training Plan</h1>
                  <p className="text-gray-600 dark:text-gray-400 mb-6">Areas where I need training (scored below 2)</p>

                  {trainingPlan.length === 0 ? (
                    <div className="text-center py-12 bg-green-50 dark:bg-green-900 dark:bg-opacity-20 border-2 border-green-300 dark:border-green-700 rounded-lg">
                      <div className="text-6xl mb-4">🎉</div>
                      <p className="text-green-700 dark:text-green-400 font-bold text-2xl mb-3">Excellent Work!</p>
                      <p className="text-gray-700 dark:text-gray-300 text-lg">
                        You've scored <strong>2 or higher</strong> on all competencies!
                      </p>
                      <p className="text-green-600 dark:text-green-400 font-medium mt-2">
                        ✓ No training currently needed
                      </p>
                    </div>
                  ) : (
                    <>
                      <div className="mb-6 p-4 bg-red-50 dark:bg-blue-900 dark:bg-opacity-20 border border-accent dark:border-blue-700 rounded-lg">
                        <p className="text-sm font-medium text-primary-navy dark:text-blue-300">
                          📋 {trainingPlan.length} competenc{trainingPlan.length === 1 ? 'y' : 'ies'} needing training
                        </p>
                        <p className="text-xs text-gray-600 dark:text-gray-400 mt-1">
                          Focus on these areas to improve your skills (0=Not trained, 1=Basic, 2=Competent, 3=Expert)
                        </p>
                      </div>

                      <div className="max-h-96 overflow-y-auto">
                        <table className="w-full">
                          <thead className="bg-red-100 dark:bg-blue-900 sticky top-0">
                            <tr className="border-b dark:border-gray-700">
                              <th className="text-left py-3 px-4 dark:text-white">Area</th>
                              <th className="text-left py-3 px-4 dark:text-white">Machine</th>
                              <th className="text-left py-3 px-4 dark:text-white">Competency</th>
                              <th className="text-center py-3 px-4 dark:text-white">Current</th>
                              <th className="text-center py-3 px-4 dark:text-white">Target</th>
                              <th className="text-center py-3 px-4 dark:text-white">Priority</th>
                            </tr>
                          </thead>
                          <tbody>
                            {trainingPlan.map((item, index) => (
                              <tr key={index} className="border-b dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-700">
                                <td className="py-3 px-4 text-sm dark:text-gray-300">{item.area}</td>
                                <td className="py-3 px-4 text-sm dark:text-gray-300">{item.machine}</td>
                                <td className="py-3 px-4 text-sm dark:text-gray-300">{item.competency}</td>
                                <td className="text-center py-3 px-4 font-bold dark:text-white">{item.currentScore}</td>
                                <td className="text-center py-3 px-4 dark:text-gray-300">{item.targetScore}</td>
                                <td className="text-center py-3 px-4">
                                  <span className={`px-2 py-1 rounded text-xs font-medium ${
                                    item.priority === 'Critical' ? 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' :
                                    item.priority === 'High' ? 'bg-orange-100 text-orange-800 dark:bg-orange-900 dark:text-orange-200' :
                                    'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200'
                                  }`}>
                                    {item.priority}
                                  </span>
                                </td>
                              </tr>
                            ))}
                          </tbody>
                        </table>
                      </div>

                      <div className="mt-6 p-4 bg-gray-50 dark:bg-gray-700 rounded-lg">
                        <p className="text-sm text-gray-700 dark:text-gray-300">
                          <strong>💡 Tip:</strong> Work with your manager to create a plan to improve these competencies. Regular practice and training will help you progress from Basic (1) to Competent (2) and eventually Expert (3).
                        </p>
                      </div>
                    </>
                  )}
                </div>
              );
            })()}
          </div>
        )}

        {/* Assessment Tab */}
        {activeTab === 'assessment' && (
          <div className="space-y-6">
            {currentUser.role === 'engineer' && (
              <div className="bg-red-50 dark:bg-blue-900 dark:bg-opacity-30 border-2 border-accent dark:border-blue-700 rounded-lg p-4">
                <h3 className="font-bold text-primary-navy dark:text-blue-200 mb-2">📝 Self-Assessment</h3>
                <p className="text-sm text-blue-800 dark:text-blue-300">
                  You can update your own competency scores by clicking on the score buttons (0-3) below.
                  All changes are automatically tracked, and your progress will be displayed in your dashboard!
                </p>
              </div>
            )}
            <div className="bg-white dark:bg-gray-800 rounded-lg shadow p-6">
              <h2 className="text-xl font-bold dark:text-white mb-4">Engineer Assessments</h2>

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
                        ? 'bg-accent text-white'
                        : 'border border-gray-300 hover:bg-gray-50'
                    }`}
                  >
                    {bulkSelectMode ? 'Exit Bulk Mode' : 'Bulk Update Mode'}
                  </button>
                )}
              </div>

              {/* Bulk Update Panel */}
              {bulkSelectMode && selectedEngineers.length > 0 && (
                <div className="mb-4 p-4 bg-red-50 border-2 border-accent rounded-lg">
                  <div className="flex items-center justify-between mb-3">
                    <h3 className="font-bold text-primary-navy">
                      Bulk Update - {selectedEngineers.length} Engineer{selectedEngineers.length !== 1 ? 's' : ''} Selected
                    </h3>
                    <button
                      onClick={() => {
                        setSelectedEngineers([]);
                        setBulkSelectMode(false);
                      }}
                      className="text-sm text-accent hover:text-blue-800"
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
                                  toast.success(`Successfully updated ${selectedEngineers.length} engineer(s)!`);
                                }
                              }}
                              className="flex-1 px-4 py-3 bg-white border-2 border-gray-300 rounded-lg hover:border-blue-500 hover:bg-red-50 font-bold text-lg transition-colors"
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
                            <h4 className="font-medium text-accent mb-2">{area.name}</h4>
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
                                          {[0, 1, 2, 3].map(value => {
                                            const canEdit = currentUser.role === 'admin' ||
                                              (currentUser.role === 'engineer' && currentUser.engineerId === engineer.id);
                                            return (
                                              <button
                                                key={value}
                                                onClick={() => {
                                                  if (canEdit) {
                                                    updateAssessment(engineer.id, area.id, machine.id, comp.id, value);
                                                  }
                                                }}
                                                disabled={!canEdit}
                                                className={`w-8 h-8 rounded text-sm font-medium transition-colors ${
                                                  currentScore === value
                                                    ? 'bg-accent text-white'
                                                    : !canEdit
                                                    ? 'bg-gray-200 text-gray-500 cursor-not-allowed'
                                                    : 'bg-white border-2 border-gray-300 hover:border-accent'
                                                }`}
                                              >
                                                {value}
                                              </button>
                                            );
                                          })}
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

        {/* Global Search Modal */}
        {showGlobalSearch && (
          <div className="fixed inset-0 bg-black bg-opacity-50 flex items-start justify-center z-50 p-4 backdrop-blur-sm pt-20">
            <div className="bg-white dark:bg-gray-800 rounded-xl w-full max-w-3xl shadow-soft-2xl overflow-hidden">
              {/* Search Input */}
              <div className="p-4 border-b border-gray-200 dark:border-gray-700">
                <div className="relative">
                  <Search className="absolute left-3 top-3 text-gray-400" size={20} />
                  <input
                    type="text"
                    autoFocus
                    placeholder="Search engineers, areas, machines, competencies..."
                    value={searchQuery}
                    onChange={(e) => setSearchQuery(e.target.value)}
                    className="w-full pl-10 pr-4 py-3 bg-gray-50 dark:bg-gray-700 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-accent focus:border-accent text-gray-900 dark:text-white"
                  />
                  <kbd className="absolute right-3 top-3 px-2 py-1 bg-gray-200 dark:bg-gray-600 border border-gray-300 dark:border-gray-500 rounded text-xs font-mono text-gray-600 dark:text-gray-300">
                    ESC
                  </kbd>
                </div>
              </div>

              {/* Search Results */}
              <div className="max-h-96 overflow-y-auto p-2">
                {searchQuery.trim() === '' ? (
                  <div className="text-center py-12 text-gray-500 dark:text-gray-400">
                    <Search size={48} className="mx-auto mb-3 opacity-50" />
                    <p>Start typing to search...</p>
                    <p className="text-sm mt-2">Search across engineers, production areas, machines, and competencies</p>
                  </div>
                ) : (() => {
                  const query = searchQuery.toLowerCase();
                  const results = [];

                  // Search engineers
                  data.engineers.forEach(engineer => {
                    if (engineer.name.toLowerCase().includes(query) || engineer.shift.toLowerCase().includes(query)) {
                      results.push({
                        type: 'engineer',
                        title: engineer.name,
                        subtitle: engineer.shift,
                        icon: '👷',
                        action: () => {
                          setActiveTab('assessment');
                          setShowGlobalSearch(false);
                          setSearchQuery('');
                          toast.success(`Navigated to ${engineer.name}`);
                        }
                      });
                    }
                  });

                  // Search production areas
                  data.productionAreas.forEach(area => {
                    if (area.name.toLowerCase().includes(query)) {
                      results.push({
                        type: 'area',
                        title: area.name,
                        subtitle: `${area.machines.length} machines`,
                        icon: '🏭',
                        action: () => {
                          setActiveTab('admin');
                          setAdminSubTab('production');
                          setShowGlobalSearch(false);
                          setSearchQuery('');
                          toast.success(`Navigated to ${area.name}`);
                        }
                      });
                    }

                    // Search machines
                    area.machines.forEach(machine => {
                      if (machine.name.toLowerCase().includes(query)) {
                        results.push({
                          type: 'machine',
                          title: machine.name,
                          subtitle: `${area.name} • ${machine.competencies.length} competencies`,
                          icon: '⚙️',
                          action: () => {
                            setActiveTab('assessment');
                            setFilterArea(area.id);
                            setShowGlobalSearch(false);
                            setSearchQuery('');
                            toast.success(`Filtered by ${machine.name}`);
                          }
                        });
                      }

                      // Search competencies
                      machine.competencies.forEach(comp => {
                        if (comp.name.toLowerCase().includes(query)) {
                          results.push({
                            type: 'competency',
                            title: comp.name,
                            subtitle: `${machine.name} • ${area.name}`,
                            icon: '📋',
                            action: () => {
                              setActiveTab('assessment');
                              setFilterArea(area.id);
                              setShowGlobalSearch(false);
                              setSearchQuery('');
                              toast.success(`Found competency: ${comp.name}`);
                            }
                          });
                        }
                      });
                    });
                  });

                  return results.length > 0 ? (
                    <div className="space-y-1">
                      {results.slice(0, 20).map((result, index) => (
                        <button
                          key={index}
                          onClick={result.action}
                          className="w-full flex items-start gap-3 p-3 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition-colors text-left group"
                        >
                          <span className="text-2xl">{result.icon}</span>
                          <div className="flex-1 min-w-0">
                            <p className="font-medium text-gray-900 dark:text-white truncate group-hover:text-accent">
                              {result.title}
                            </p>
                            <p className="text-sm text-gray-500 dark:text-gray-400 truncate">
                              {result.subtitle}
                            </p>
                          </div>
                          <span className="text-xs text-gray-400 dark:text-gray-500 uppercase tracking-wide">
                            {result.type}
                          </span>
                        </button>
                      ))}
                      {results.length > 20 && (
                        <p className="text-center text-sm text-gray-500 dark:text-gray-400 py-2">
                          Showing first 20 of {results.length} results
                        </p>
                      )}
                    </div>
                  ) : (
                    <div className="text-center py-12 text-gray-500 dark:text-gray-400">
                      <AlertCircle size={48} className="mx-auto mb-3 opacity-50" />
                      <p>No results found for "{searchQuery}"</p>
                      <p className="text-sm mt-2">Try a different search term</p>
                    </div>
                  );
                })()}
              </div>

              {/* Footer */}
              <div className="p-3 border-t border-gray-200 dark:border-gray-700 bg-gray-50 dark:bg-gray-750 flex items-center justify-between text-xs text-gray-500 dark:text-gray-400">
                <div className="flex items-center gap-4">
                  <span className="flex items-center gap-1">
                    <kbd className="px-1.5 py-0.5 bg-white dark:bg-gray-700 border border-gray-300 dark:border-gray-600 rounded font-mono">↑↓</kbd>
                    Navigate
                  </span>
                  <span className="flex items-center gap-1">
                    <kbd className="px-1.5 py-0.5 bg-white dark:bg-gray-700 border border-gray-300 dark:border-gray-600 rounded font-mono">Enter</kbd>
                    Select
                  </span>
                </div>
                <span>Press ESC to close</span>
              </div>
            </div>
          </div>
        )}

        {/* Keyboard Shortcuts Help Modal */}
        {showShortcutsHelp && (
          <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4 backdrop-blur-sm">
            <div className="bg-white dark:bg-gray-800 rounded-xl p-8 max-w-2xl w-full shadow-soft-2xl max-h-[90vh] overflow-y-auto">
              <div className="flex justify-between items-center mb-6">
                <h2 className="text-2xl font-bold text-gray-900 dark:text-white">⌨️ Keyboard Shortcuts</h2>
                <button
                  onClick={() => setShowShortcutsHelp(false)}
                  className="p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition-colors"
                >
                  <X size={24} className="text-gray-500" />
                </button>
              </div>

              <div className="space-y-6">
                <div>
                  <h3 className="text-lg font-semibold text-gray-900 dark:text-white mb-3">General</h3>
                  <div className="space-y-2">
                    <div className="flex justify-between items-center p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                      <span className="text-gray-700 dark:text-gray-300">Global search</span>
                      <kbd className="px-3 py-1 bg-white dark:bg-gray-800 border-2 border-gray-300 dark:border-gray-600 rounded font-mono text-sm shadow-soft">⌘K / Ctrl+K</kbd>
                    </div>
                    <div className="flex justify-between items-center p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                      <span className="text-gray-700 dark:text-gray-300">Show this help</span>
                      <kbd className="px-3 py-1 bg-white dark:bg-gray-800 border-2 border-gray-300 dark:border-gray-600 rounded font-mono text-sm shadow-soft">?</kbd>
                    </div>
                  </div>
                </div>

                {currentUser?.role === 'admin' && (
                  <div>
                    <h3 className="text-lg font-semibold text-gray-900 dark:text-white mb-3">Admin Actions</h3>
                    <div className="space-y-2">
                      <div className="flex justify-between items-center p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                        <span className="text-gray-700 dark:text-gray-300">Create snapshot</span>
                        <kbd className="px-3 py-1 bg-white dark:bg-gray-800 border-2 border-gray-300 dark:border-gray-600 rounded font-mono text-sm shadow-soft">⌘S / Ctrl+S</kbd>
                      </div>
                      <div className="flex justify-between items-center p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                        <span className="text-gray-700 dark:text-gray-300">Export to Excel</span>
                        <kbd className="px-3 py-1 bg-white dark:bg-gray-800 border-2 border-gray-300 dark:border-gray-600 rounded font-mono text-sm shadow-soft">⌘E / Ctrl+E</kbd>
                      </div>
                    </div>
                  </div>
                )}

                <div className="pt-4 border-t border-gray-200 dark:border-gray-700">
                  <p className="text-sm text-gray-500 dark:text-gray-400">
                    💡 <strong>Tip:</strong> Shortcuts work when you're not typing in an input field.
                  </p>
                </div>
              </div>

              <div className="mt-8 flex justify-end">
                <button
                  onClick={() => setShowShortcutsHelp(false)}
                  className="px-6 py-2.5 bg-accent text-white rounded-lg hover:bg-accent-600 transition-all shadow-btn hover:shadow-btn-hover font-medium"
                >
                  Got it!
                </button>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

export default App;
