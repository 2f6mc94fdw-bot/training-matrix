import React, { useState, useEffect } from 'react';
import { Plus, Trash2, Edit2, Save, X, Users, Settings, Download, Upload, Database, BarChart3, TrendingUp, AlertTriangle, Search, Filter, Copy, MessageSquare, CheckSquare, Calendar, Award, Clock, History } from 'lucide-react';
import { BarChart, Bar, LineChart, Line, PieChart, Pie, Cell, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import * as XLSX from 'xlsx';
import Papa from 'papaparse';

// Storage wrapper for localStorage
const storage = {
  get: async (key) => {
    const value = localStorage.getItem(key);
    return value ? { key, value } : null;
  },
  set: async (key, value) => {
    localStorage.setItem(key, value);
    return { key, value };
  },
  delete: async (key) => {
    localStorage.removeItem(key);
    return { key, deleted: true };
  }
};

export default function TrainingMatrixBuilder() {
  const [activeTab, setActiveTab] = useState('admin');
  const [structure, setStructure] = useState({
    productionAreas: [],
    basicEngineering: {
      categories: [
        { id: 'mechanical', name: 'Mechanical', competencies: [] },
        { id: 'electrical', name: 'Electrical', competencies: [] },
        { id: 'software', name: 'Software', competencies: [] }
      ]
    },
    engineers: [],
    certifications: [],
    trainingPlans: [],
    auditLog: [],
    snapshots: []
  });
  
  const [loading, setLoading] = useState(true);
  const [confirmDelete, setConfirmDelete] = useState(null);
  const [currentUser, setCurrentUser] = useState(null);
  const [users, setUsers] = useState([]);

  useEffect(() => {
    loadData();
  }, []);

  const loadData = async () => {
    try {
      const result = await storage.get('training-matrix-structure');
      if (result && result.value) {
        const loadedStructure = JSON.parse(result.value);
        setStructure({
          ...loadedStructure,
          certifications: loadedStructure.certifications || [],
          trainingPlans: loadedStructure.trainingPlans || [],
          auditLog: loadedStructure.auditLog || [],
          snapshots: loadedStructure.snapshots || []
        });
      }
      
      const usersResult = await storage.get('training-matrix-users');
      if (usersResult && usersResult.value) {
        setUsers(JSON.parse(usersResult.value));
      } else {
        const defaultUsers = [
          { username: 'admin', password: 'admin123', role: 'admin' }
        ];
        await storage.set('training-matrix-users', JSON.stringify(defaultUsers));
        setUsers(defaultUsers);
      }
    } catch (error) {
      console.log('No existing data');
      const defaultUsers = [
        { username: 'admin', password: 'admin123', role: 'admin' }
      ];
      setUsers(defaultUsers);
    } finally {
      setLoading(false);
    }
  };

  const saveData = async (newStructure) => {
    try {
      await storage.set('training-matrix-structure', JSON.stringify(newStructure));
      setStructure(newStructure);
    } catch (error) {
      console.error('Error saving data:', error);
    }
  };

  const saveUsers = async (newUsers) => {
    try {
      await storage.set('training-matrix-users', JSON.stringify(newUsers));
      setUsers(newUsers);
    } catch (error) {
      console.error('Error saving users:', error);
    }
  };

  const handleLogin = (username, password) => {
    const user = users.find(u => u.username === username && u.password === password);
    if (user) {
      setCurrentUser(user);
      if (user.role === 'engineer') {
        setActiveTab('assessment');
      }
      return true;
    }
    return false;
  };

  const handleLogout = () => {
    setCurrentUser(null);
    setActiveTab('admin');
  };

  const loadSampleData = async () => {
    setConfirmDelete({
      message: 'Load sample data? Any existing data will be replaced.',
      action: async () => {
        const sampleStructure = {
          productionAreas: [
            {
              id: 'line7',
              name: 'Line 7',
              machines: [
                {
                  id: 'line7-vision',
                  name: 'Vision System',
                  importance: 1.5,
                  competencies: [
                    { id: 'comp-1', name: 'Setup & Configuration', maxScore: 3 },
                    { id: 'comp-2', name: 'Operation', maxScore: 3 },
                    { id: 'comp-3', name: 'Troubleshooting', maxScore: 3 }
                  ]
                }
              ]
            }
          ],
          basicEngineering: {
            categories: [
              {
                id: 'mechanical',
                name: 'Mechanical',
                competencies: [
                  { id: 'mech-1', name: 'Welding', maxScore: 3 }
                ]
              }
            ]
          },
          engineers: [
            { id: 'eng-1', name: 'Cameron Buys', shift: 'A Shift', scores: {}, notes: '' },
            { id: 'eng-2', name: 'Ben Barnham', shift: 'A Shift', scores: {}, notes: '' }
          ],
          certifications: [],
          trainingPlans: [],
          auditLog: [{
            id: `log-${Date.now()}`,
            timestamp: new Date().toISOString(),
            action: 'Loaded Sample Data',
            details: 'Sample data loaded'
          }],
          snapshots: []
        };
        await saveData(sampleStructure);
        setConfirmDelete(null);
      }
    });
  };

  const addEngineer = () => {
    saveData({
      ...structure,
      engineers: [...structure.engineers, {
        id: `eng-${Date.now()}`,
        name: 'New Engineer',
        shift: 'A Shift',
        scores: {},
        notes: ''
      }]
    });
  };

  const updateEngineer = (engId, updates) => {
    saveData({
      ...structure,
      engineers: structure.engineers.map(eng =>
        eng.id === engId ? { ...eng, ...updates } : eng
      )
    });
  };

  const deleteEngineer = (engId) => {
    setConfirmDelete({
      message: 'Delete this engineer?',
      action: () => {
        saveData({
          ...structure,
          engineers: structure.engineers.filter(eng => eng.id !== engId)
        });
        setConfirmDelete(null);
      }
    });
  };

  if (loading) {
    return <div className="flex items-center justify-center h-screen">Loading...</div>;
  }

  if (!currentUser) {
    return <LoginPage onLogin={handleLogin} />;
  }

  return (
    <div className="w-full h-screen bg-gray-50 flex flex-col">
      <div className="bg-white shadow">
        <div className="max-w-7xl mx-auto px-4 py-4 flex justify-between items-center">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Training Matrix - Full System</h1>
            <p className="text-sm text-gray-600">{currentUser.username} ({currentUser.role}) - All 4 Phases</p>
          </div>
          <button onClick={handleLogout} className="px-4 py-2 bg-red-600 text-white rounded-md hover:bg-red-700">
            Logout
          </button>
        </div>
      </div>

      <div className="bg-white border-b">
        <div className="max-w-7xl mx-auto px-4">
          <div className="flex space-x-4 overflow-x-auto">
            {currentUser.role === 'admin' && (
              <>
                <TabButton active={activeTab === 'admin'} onClick={() => setActiveTab('admin')} icon={<Settings className="w-4 h-4" />} label="Admin" />
                <TabButton active={activeTab === 'data'} onClick={() => setActiveTab('data')} icon={<Database className="w-4 h-4" />} label="Data (Phase 1)" />
                <TabButton active={activeTab === 'reports'} onClick={() => setActiveTab('reports')} icon={<BarChart3 className="w-4 h-4" />} label="Reports (Phase 2)" />
                <TabButton active={activeTab === 'advanced'} onClick={() => setActiveTab('advanced')} icon={<Award className="w-4 h-4" />} label="Advanced (Phase 4)" />
              </>
            )}
            <TabButton active={activeTab === 'assessment'} onClick={() => setActiveTab('assessment')} icon={<Users className="w-4 h-4" />} label="Assessment (Phase 3)" />
          </div>
        </div>
      </div>

      <div className="flex-1 overflow-auto">
        <div className="max-w-7xl mx-auto px-4 py-6">
          {activeTab === 'admin' && <QuickAdminView structure={structure} loadSampleData={loadSampleData} addEngineer={addEngineer} updateEngineer={updateEngineer} deleteEngineer={deleteEngineer} />}
          {activeTab === 'data' && <DataPhase structure={structure} saveData={saveData} />}
          {activeTab === 'reports' && <ReportsPhase structure={structure} />}
          {activeTab === 'advanced' && <AdvancedPhase structure={structure} saveData={saveData} />}
          {activeTab === 'assessment' && <AssessmentPhase structure={structure} updateEngineer={updateEngineer} currentUser={currentUser} />}
        </div>
      </div>

      {confirmDelete && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
          <div className="bg-white rounded-lg p-6 max-w-md mx-4">
            <h3 className="text-lg font-bold mb-4">Confirm</h3>
            <p className="mb-6">{confirmDelete.message}</p>
            <div className="flex gap-3 justify-end">
              <button onClick={() => setConfirmDelete(null)} className="px-4 py-2 bg-gray-200 rounded">Cancel</button>
              <button onClick={confirmDelete.action} className="px-4 py-2 bg-red-600 text-white rounded">Confirm</button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}

function TabButton({ active, onClick, icon, label }) {
  return (
    <button
      onClick={onClick}
      className={`py-4 px-3 border-b-2 font-medium text-sm flex items-center gap-2 whitespace-nowrap ${
        active ? 'border-blue-500 text-blue-600' : 'border-transparent text-gray-500 hover:text-gray-700'
      }`}
    >
      {icon}
      {label}
    </button>
  );
}

function LoginPage({ onLogin }) {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');

  const handleSubmit = () => {
    if (!onLogin(username, password)) {
      setError('Invalid credentials');
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-500 to-blue-700 flex items-center justify-center p-4">
      <div className="bg-white rounded-lg shadow-2xl p-8 w-full max-w-md">
        <h1 className="text-3xl font-bold mb-2">Training Matrix</h1>
        <p className="text-gray-600 mb-6">Complete Phase 1-4 System</p>
        <div className="space-y-4">
          <div>
            <label className="block text-sm font-medium mb-2">Username</label>
            <input
              type="text"
              value={username}
              onChange={(e) => setUsername(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleSubmit()}
              className="w-full px-4 py-3 border rounded"
            />
          </div>
          <div>
            <label className="block text-sm font-medium mb-2">Password</label>
            <input
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleSubmit()}
              className="w-full px-4 py-3 border rounded"
            />
          </div>
          {error && <div className="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded">{error}</div>}
          <button onClick={handleSubmit} className="w-full bg-blue-600 text-white py-3 rounded hover:bg-blue-700">
            Sign In
          </button>
        </div>
        <div className="mt-6 p-4 bg-blue-50 rounded">
          <p className="text-sm"><strong>Login:</strong> admin / admin123</p>
        </div>
      </div>
    </div>
  );
}
function QuickAdminView({ structure, loadSampleData, addEngineer, updateEngineer, deleteEngineer }) {
  const [users, setUsers] = useState([]);
  const [showUserForm, setShowUserForm] = useState(false);
  const [newUser, setNewUser] = useState({ username: '', password: '', role: 'engineer', engineerId: '' });

  useEffect(() => {
    loadUsers();
  }, []);

  const loadUsers = async () => {
    const result = await storage.get('training-matrix-users');
    if (result && result.value) {
      setUsers(JSON.parse(result.value));
    }
  };

  const saveUsers = async (newUsers) => {
    await storage.set('training-matrix-users', JSON.stringify(newUsers));
    setUsers(newUsers);
  };

  const handleCreateUser = () => {
    if (!newUser.username || !newUser.password) {
      alert('Username and password required');
      return;
    }
    if (users.find(u => u.username === newUser.username)) {
      alert('Username already exists');
      return;
    }
    if (newUser.role === 'engineer' && !newUser.engineerId) {
      alert('Please select an engineer');
      return;
    }
    saveUsers([...users, newUser]);
    setNewUser({ username: '', password: '', role: 'engineer', engineerId: '' });
    setShowUserForm(false);
  };

  const handleDeleteUser = (username) => {
    if (username === 'admin') {
      alert('Cannot delete admin account');
      return;
    }
    if (confirm(`Delete user "${username}"?`)) {
      saveUsers(users.filter(u => u.username !== username));
    }
  };

  return (
    <div className="space-y-6">
      <div className="bg-green-50 border-2 border-green-200 rounded-lg p-6">
        <h2 className="text-xl font-bold mb-2">🚀 Quick Start</h2>
        <p className="mb-4">Load sample data to see all features in action</p>
        <button onClick={loadSampleData} className="px-6 py-3 bg-green-600 text-white rounded hover:bg-green-700">
          Load Sample Data
        </button>
      </div>

      {/* USER ACCOUNTS SECTION */}
      <div className="bg-white rounded-lg shadow p-6">
        <div className="flex justify-between mb-4">
          <h2 className="text-xl font-bold">👥 User Accounts</h2>
          <button 
            onClick={() => setShowUserForm(!showUserForm)} 
            className="px-4 py-2 bg-purple-600 text-white rounded hover:bg-purple-700"
          >
            + Create User
          </button>
        </div>

        {showUserForm && (
          <div className="mb-4 p-4 bg-purple-50 rounded-lg border-2 border-purple-200">
            <h3 className="font-semibold mb-3">New User Account</h3>
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-3 mb-3">
              <input
                type="text"
                value={newUser.username}
                onChange={(e) => setNewUser({ ...newUser, username: e.target.value })}
                placeholder="Username"
                className="px-3 py-2 border rounded"
              />
              <input
                type="text"
                value={newUser.password}
                onChange={(e) => setNewUser({ ...newUser, password: e.target.value })}
                placeholder="Password"
                className="px-3 py-2 border rounded"
              />
              <select
                value={newUser.role}
                onChange={(e) => setNewUser({ ...newUser, role: e.target.value })}
                className="px-3 py-2 border rounded"
              >
                <option value="engineer">Engineer</option>
                <option value="admin">Admin</option>
              </select>
              {newUser.role === 'engineer' && (
                <select
                  value={newUser.engineerId}
                  onChange={(e) => setNewUser({ ...newUser, engineerId: e.target.value })}
                  className="px-3 py-2 border rounded"
                >
                  <option value="">Select Engineer Profile</option>
                  {structure.engineers.map(eng => (
                    <option key={eng.id} value={eng.id}>{eng.name}</option>
                  ))}
                </select>
              )}
            </div>
            <div className="flex gap-2">
              <button onClick={handleCreateUser} className="px-4 py-2 bg-purple-600 text-white rounded hover:bg-purple-700">
                Create
              </button>
              <button onClick={() => setShowUserForm(false)} className="px-4 py-2 bg-gray-200 rounded hover:bg-gray-300">
                Cancel
              </button>
            </div>
          </div>
        )}

        <div className="space-y-2">
          {users.map(user => {
            const engineer = user.engineerId ? structure.engineers.find(e => e.id === user.engineerId) : null;
            return (
              <div key={user.username} className="flex items-center gap-3 p-3 bg-gray-50 rounded">
                <div className="flex-1">
                  <div className="font-medium">{user.username}</div>
                  <div className="text-sm text-gray-500">
                    {user.role === 'admin' ? (
                      <span className="text-red-600 font-semibold">Administrator</span>
                    ) : (
                      <span>Engineer {engineer ? `- ${engineer.name}` : '(unlinked)'}</span>
                    )}
                  </div>
                </div>
                {user.username !== 'admin' && (
                  <button 
                    onClick={() => handleDeleteUser(user.username)} 
                    className="px-3 py-1 bg-red-600 text-white rounded text-sm hover:bg-red-700"
                  >
                    Delete
                  </button>
                )}
              </div>
            );
          })}
        </div>
      </div>

      {/* ENGINEERS SECTION */}
      <div className="bg-white rounded-lg shadow p-6">
        <div className="flex justify-between mb-4">
          <h2 className="text-xl font-bold">Engineers ({structure.engineers.length})</h2>
          <button onClick={addEngineer} className="px-4 py-2 bg-blue-600 text-white rounded hover:bg-blue-700">
            + Add Engineer
          </button>
        </div>
        <div className="space-y-2">
          {structure.engineers.map(eng => (
            <div key={eng.id} className="flex items-center justify-between p-3 bg-gray-50 rounded">
              <div>
                <div className="font-medium">{eng.name}</div>
                <div className="text-sm text-gray-500">{eng.shift}</div>
              </div>
              <button onClick={() => deleteEngineer(eng.id)} className="px-3 py-1 bg-red-600 text-white rounded text-sm">
                Delete
              </button>
            </div>
          ))}
        </div>
      </div>

      <div className="bg-blue-50 border border-blue-200 rounded-lg p-6">
        <h3 className="font-bold mb-2">📚 Explore All Features</h3>
        <ul className="space-y-1 text-sm">
          <li>✅ <strong>Data Tab:</strong> Export to Excel, Import, Backup/Restore</li>
          <li>✅ <strong>Reports Tab:</strong> Dashboard, Gap Analysis, Heatmap</li>
          <li>✅ <strong>Advanced Tab:</strong> Training Plans, Certifications, Audit Trail</li>
          <li>✅ <strong>Assessment Tab:</strong> Rate skills with Search & Filters</li>
        </ul>
      </div>
    </div>
  );
}

function DataPhase({ structure, saveData }) {
  const [showBackupData, setShowBackupData] = useState(false);
  const [copySuccess, setCopySuccess] = useState('');
  const backupData = JSON.stringify(structure, null, 2);

  const copyToClipboard = () => {
    navigator.clipboard.writeText(backupData).then(() => {
      setCopySuccess('✅ Copied! Paste into Notepad and save as .json');
      setTimeout(() => setCopySuccess(''), 3000);
    }).catch(() => {
      setCopySuccess('❌ Copy failed - use the text box below');
      setShowBackupData(true);
    });
  };

  const handleRestore = (event) => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = async (e) => {
      try {
        const data = JSON.parse(e.target.result);
        await saveData(data);
        alert('✅ Backup restored successfully!');
      } catch (error) {
        alert('❌ Error: Invalid backup file');
      }
    };
    reader.readAsText(file);
    event.target.value = '';
  };

  const exportToExcel = () => {
    const wb = XLSX.utils.book_new();
    const engineersData = structure.engineers.map(e => ({ Name: e.name, Shift: e.shift }));
    const ws = XLSX.utils.json_to_sheet(engineersData);
    XLSX.utils.book_append_sheet(wb, ws, 'Engineers');
    XLSX.writeFile(wb, `training-matrix-${new Date().toISOString().split('T')[0]}.xlsx`);
  };

  return (
    <div className="space-y-6">
      <div className="bg-white rounded-lg shadow p-6">
        <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
          <Database className="w-6 h-6" />
          Phase 1: Data Management
        </h2>

        <div className="mb-6">
          <h3 className="font-bold mb-3">📥 Backup Your Data</h3>
          <div className="grid grid-cols-2 gap-4 mb-3">
            <button 
              onClick={copyToClipboard} 
              className="flex items-center justify-center gap-2 px-6 py-4 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
            >
              <Copy className="w-5 h-5" />
              Copy Backup to Clipboard
            </button>
            <button 
              onClick={() => setShowBackupData(!showBackupData)} 
              className="flex items-center justify-center gap-2 px-6 py-4 bg-purple-600 text-white rounded-lg hover:bg-purple-700"
            >
              <Download className="w-5 h-5" />
              Show Backup Data
            </button>
          </div>

          {copySuccess && (
            <div className="p-3 bg-green-50 border border-green-200 text-green-700 rounded mb-3">
              {copySuccess}
            </div>
          )}

          {showBackupData && (
            <div className="mb-4">
              <p className="text-sm text-gray-600 mb-2">Copy this text and save it as a .json file:</p>
              <textarea
                value={backupData}
                readOnly
                className="w-full h-64 p-3 border rounded font-mono text-xs"
                onClick={(e) => e.target.select()}
              />
            </div>
          )}
        </div>

        <div className="mb-6">
          <h3 className="font-bold mb-3">📤 Restore Backup</h3>
          <label className="flex items-center justify-center gap-2 px-6 py-4 bg-green-600 text-white rounded-lg hover:bg-green-700 cursor-pointer">
            <Upload className="w-5 h-5" />
            Upload Backup File
            <input type="file" accept=".json" onChange={handleRestore} className="hidden" />
          </label>
        </div>

        <div className="mb-6">
          <h3 className="font-bold mb-3">📊 Export to Excel</h3>
          <button onClick={exportToExcel} className="flex items-center gap-2 px-6 py-4 bg-green-600 text-white rounded-lg hover:bg-green-700">
            <Download className="w-5 h-5" />
            Export Engineers to Excel
          </button>
        </div>
      </div>
    </div>
  );
}

function ReportsPhase({ structure }) {
  const totalEngineers = structure.engineers.length;
  const totalAreas = structure.productionAreas.length;

  return (
    <div className="space-y-6">
      <div className="bg-white rounded-lg shadow p-6">
        <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
          <BarChart3 className="w-6 h-6" />
          Phase 2: Reports & Analytics
        </h2>

        <div className="grid grid-cols-3 gap-4 mb-6">
          <div className="p-6 bg-gradient-to-br from-blue-500 to-blue-600 rounded-lg text-white">
            <div className="text-sm opacity-90 mb-2">Total Engineers</div>
            <div className="text-4xl font-bold">{totalEngineers}</div>
          </div>
          <div className="p-6 bg-gradient-to-br from-green-500 to-green-600 rounded-lg text-white">
            <div className="text-sm opacity-90 mb-2">Production Areas</div>
            <div className="text-4xl font-bold">{totalAreas}</div>
          </div>
          <div className="p-6 bg-gradient-to-br from-purple-500 to-purple-600 rounded-lg text-white">
            <div className="text-sm opacity-90 mb-2">Certifications</div>
            <div className="text-4xl font-bold">{structure.certifications?.length || 0}</div>
          </div>
        </div>

        <div className="bg-white rounded-lg border p-6">
          <h3 className="font-bold mb-4">Engineer Progress</h3>
          <div className="space-y-3">
            {structure.engineers.map(eng => {
              let totalScore = 0;
              let maxScore = 0;
              
              structure.productionAreas.forEach(area => {
                area.machines?.forEach(machine => {
                  machine.competencies?.forEach(comp => {
                    const scoreKey = `${machine.id}-${comp.id}`;
                    totalScore += eng.scores?.[scoreKey] || 0;
                    maxScore += comp.maxScore;
                  });
                });
              });

              const completion = maxScore > 0 ? Math.round((totalScore / maxScore) * 100) : 0;

              return (
                <div key={eng.id} className="flex items-center gap-4 p-3 bg-gray-50 rounded">
                  <div className="flex-1">
                    <div className="font-medium">{eng.name}</div>
                    <div className="text-sm text-gray-500">{eng.shift}</div>
                    <div className="w-full bg-gray-200 rounded-full h-2 mt-2">
                      <div className="bg-blue-600 h-2 rounded-full" style={{ width: `${completion}%` }} />
                    </div>
                  </div>
                  <div className="text-right">
                    <div className="text-2xl font-bold text-blue-600">{completion}%</div>
                    <div className="text-xs text-gray-500">{totalScore}/{maxScore}</div>
                  </div>
                </div>
              );
            })}
          </div>
        </div>
      </div>
    </div>
  );
}

function AdvancedPhase({ structure, saveData }) {
  const [activeSection, setActiveSection] = useState('training-plans');

  const createSnapshot = () => {
    const snapshot = {
      id: `snapshot-${Date.now()}`,
      timestamp: new Date().toISOString(),
      data: { engineers: structure.engineers.map(e => ({ id: e.id, name: e.name, scores: {...e.scores} })) }
    };
    saveData({
      ...structure,
      snapshots: [...(structure.snapshots || []), snapshot],
      auditLog: [...(structure.auditLog || []), {
        id: `log-${Date.now()}`,
        timestamp: new Date().toISOString(),
        action: 'Created Snapshot',
        details: 'Progress snapshot created'
      }]
    });
  };

  return (
    <div className="space-y-6">
      <div className="bg-white rounded-lg shadow p-6">
        <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
          <Award className="w-6 h-6" />
          Phase 4: Advanced Features
        </h2>

        <div className="flex gap-2 mb-6 flex-wrap">
          <button onClick={() => setActiveSection('training-plans')} className={`px-4 py-2 rounded ${activeSection === 'training-plans' ? 'bg-blue-600 text-white' : 'bg-gray-100'}`}>
            Training Plans
          </button>
          <button onClick={() => setActiveSection('certifications')} className={`px-4 py-2 rounded ${activeSection === 'certifications' ? 'bg-blue-600 text-white' : 'bg-gray-100'}`}>
            Certifications
          </button>
          <button onClick={() => setActiveSection('audit')} className={`px-4 py-2 rounded ${activeSection === 'audit' ? 'bg-blue-600 text-white' : 'bg-gray-100'}`}>
            Audit Trail
          </button>
          <button onClick={() => setActiveSection('history')} className={`px-4 py-2 rounded ${activeSection === 'history' ? 'bg-blue-600 text-white' : 'bg-gray-100'}`}>
            Progress History
          </button>
        </div>

        {activeSection === 'training-plans' && (
          <div className="p-4 bg-blue-50 rounded">
            <h3 className="font-bold mb-2">Training Plans</h3>
            <p className="text-sm mb-4">Auto-generate training plans based on skill gaps</p>
            <p className="text-sm">📋 {structure.trainingPlans?.length || 0} plans created</p>
          </div>
        )}

        {activeSection === 'certifications' && (
          <div className="p-4 bg-green-50 rounded">
            <h3 className="font-bold mb-2">Certifications</h3>
            <p className="text-sm mb-4">Track certifications and expiry dates</p>
            {structure.certifications?.length > 0 ? (
              structure.certifications.map(cert => (
                <div key={cert.id} className="p-3 bg-white rounded mb-2">
                  <div className="font-medium">{cert.name}</div>
                  <div className="text-sm text-gray-600">{cert.engineerName} - Expires: {cert.expiryDate}</div>
                </div>
              ))
            ) : (
              <p className="text-sm text-gray-500">No certifications yet</p>
            )}
          </div>
        )}

        {activeSection === 'audit' && (
          <div className="p-4 bg-yellow-50 rounded">
            <h3 className="font-bold mb-2">Audit Trail</h3>
            <p className="text-sm mb-4">Complete history of all changes</p>
            <div className="space-y-2">
              {(structure.auditLog || []).slice(-5).reverse().map(log => (
                <div key={log.id} className="p-2 bg-white rounded text-sm">
                  <div className="font-medium">{log.action}</div>
                  <div className="text-gray-600 text-xs">{new Date(log.timestamp).toLocaleString()}</div>
                </div>
              ))}
            </div>
          </div>
        )}

        {activeSection === 'history' && (
          <div className="p-4 bg-purple-50 rounded">
            <h3 className="font-bold mb-2">Progress History</h3>
            <button onClick={createSnapshot} className="px-4 py-2 bg-purple-600 text-white rounded mb-4">
              📸 Create Snapshot
            </button>
            <p className="text-sm">Snapshots: {structure.snapshots?.length || 0}</p>
          </div>
        )}
      </div>
    </div>
  );
}
function AssessmentPhase({ structure, updateEngineer, currentUser }) {
  const [selectedEngId, setSelectedEngId] = useState('');
  const [searchTerm, setSearchTerm] = useState('');
  const [filterCompletion, setFilterCompletion] = useState('all');
  
  const engineer = structure.engineers.find(e => e.id === selectedEngId);
  const isEngineerView = currentUser.role === 'engineer';

  const setScore = (machineId, compId, score) => {
    if (!engineer) return;
    updateEngineer(engineer.id, {
      scores: { ...engineer.scores, [`${machineId}-${compId}`]: parseInt(score) }
    });
  };

  const getScore = (machineId, compId) => {
    return engineer?.scores?.[`${machineId}-${compId}`] || 0;
  };

  const calculateCompletion = (machine) => {
    if (!engineer || !machine.competencies) return 0;
    let totalScore = 0;
    let maxScore = 0;
    machine.competencies.forEach(comp => {
      totalScore += getScore(machine.id, comp.id);
      maxScore += comp.maxScore;
    });
    return maxScore > 0 ? Math.round((totalScore / maxScore) * 100) : 0;
  };

  const filterMachines = (machines) => {
    if (!machines) return [];
    return machines.filter(machine => {
      const matchesSearch = machine.name.toLowerCase().includes(searchTerm.toLowerCase());
      const completion = calculateCompletion(machine);
      
      let matchesCompletion = true;
      if (filterCompletion === 'complete') matchesCompletion = completion === 100;
      if (filterCompletion === 'partial') matchesCompletion = completion > 0 && completion < 100;
      if (filterCompletion === 'none') matchesCompletion = completion === 0;
      
      return matchesSearch && matchesCompletion;
    });
  };

  return (
    <div className="space-y-6">
      <div className="bg-white rounded-lg shadow p-6">
        <h2 className="text-2xl font-bold mb-6 flex items-center gap-2">
          <Users className="w-6 h-6" />
          Phase 3: Assessment (with Search & Filters)
        </h2>

        <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 mb-4">
          <div>
            <label className="block text-sm font-medium mb-2">
              {isEngineerView ? 'Your Profile' : 'Select Engineer'}
            </label>
            {isEngineerView ? (
              <div className="px-3 py-2 border rounded bg-gray-100 font-medium">
                {engineer?.name} ({engineer?.shift})
              </div>
            ) : (
              <select 
                value={selectedEngId} 
                onChange={(e) => setSelectedEngId(e.target.value)} 
                className="w-full px-3 py-2 border rounded"
              >
                <option value="">Choose engineer...</option>
                {structure.engineers.map(eng => (
                  <option key={eng.id} value={eng.id}>{eng.name} ({eng.shift})</option>
                ))}
              </select>
            )}
          </div>

          {selectedEngId && (
            <div>
              <label className="block text-sm font-medium mb-2">Filter by Completion</label>
              <div className="relative">
                <Filter className="absolute left-3 top-1/2 transform -translate-y-1/2 w-4 h-4 text-gray-400" />
                <select
                  value={filterCompletion}
                  onChange={(e) => setFilterCompletion(e.target.value)}
                  className="w-full pl-10 pr-4 py-2 border rounded"
                >
                  <option value="all">All Completion Levels</option>
                  <option value="complete">Fully Complete (100%)</option>
                  <option value="partial">Partially Complete</option>
                  <option value="none">Not Started (0%)</option>
                </select>
              </div>
            </div>
          )}
        </div>

        {selectedEngId && (
          <div className="mb-4">
            <label className="block text-sm font-medium mb-2">Search Machines</label>
            <div className="relative">
              <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 w-4 h-4 text-gray-400" />
              <input
                type="text"
                placeholder="Search machines..."
                value={searchTerm}
                onChange={(e) => setSearchTerm(e.target.value)}
                className="w-full pl-10 pr-4 py-2 border rounded"
              />
            </div>
          </div>
        )}

        {!engineer ? (
          <div className="text-center py-12 text-gray-500">
            Select an engineer to begin assessment
          </div>
        ) : (
          <div className="space-y-6 mt-6">
            <div className="p-4 bg-blue-50 rounded">
              <div className="font-bold text-lg">{engineer.name}</div>
              <div className="text-sm text-gray-600">{engineer.shift}</div>
              {engineer.notes && (
                <div className="mt-2 text-sm">
                  <strong>Notes:</strong> {engineer.notes}
                </div>
              )}
            </div>

            {structure.productionAreas.map(area => {
              const filteredMachines = filterMachines(area.machines || []);
              if (filteredMachines.length === 0) return null;

              return (
                <div key={area.id} className="border rounded-lg p-4">
                  <h3 className="font-bold mb-3 text-lg text-blue-600">{area.name}</h3>
                  {filteredMachines.map(machine => {
                    const completion = calculateCompletion(machine);
                    
                    return (
                      <div key={machine.id} className="mb-4 p-4 bg-gray-50 rounded-lg border-2 border-gray-200">
                        <div className="flex justify-between items-center mb-3">
                          <div className="flex-1">
                            <div className="font-medium text-lg">{machine.name}</div>
                            <div className="text-sm text-gray-500">Weight: {machine.importance}x</div>
                          </div>
                          <div className="text-right">
                            <div className="text-2xl font-bold text-blue-600">{completion}%</div>
                          </div>
                        </div>
                        <div className="w-full bg-gray-200 rounded-full h-2 mb-4">
                          <div className="bg-blue-600 h-2 rounded-full transition-all" style={{ width: `${completion}%` }} />
                        </div>

                        {machine.competencies?.map(comp => {
                          const score = getScore(machine.id, comp.id);
                          return (
                            <div key={comp.id} className="flex flex-col sm:flex-row items-start sm:items-center justify-between mb-3 p-3 bg-white rounded border">
                              <div className="flex-1 mb-2 sm:mb-0">
                                <span className="font-medium">{comp.name}</span>
                                <span className="text-sm text-gray-500 ml-2">(Max: {comp.maxScore})</span>
                              </div>
                              <div className="flex gap-2">
                                {[0, 1, 2, 3].slice(0, comp.maxScore + 1).map(val => (
                                  <button
                                    key={val}
                                    onClick={() => setScore(machine.id, comp.id, val)}
                                    disabled={isEngineerView}
                                    className={`w-12 h-12 rounded-md font-semibold transition-all ${
                                      score === val 
                                        ? 'bg-blue-600 text-white scale-110 shadow-lg' 
                                        : isEngineerView
                                        ? 'bg-gray-200 text-gray-500 cursor-not-allowed'
                                        : 'bg-white border-2 border-gray-300 hover:border-blue-400 hover:bg-blue-50'
                                    }`}
                                  >
                                    {val}
                                  </button>
                                ))}
                              </div>
                            </div>
                          );
                        })}
                      </div>
                    );
                  })}
                </div>
              );
            })}

            {structure.basicEngineering && (
              <div className="border rounded-lg p-4 bg-green-50">
                <h3 className="font-bold mb-3 text-lg text-green-600">Basic Engineering Knowledge</h3>
                {structure.basicEngineering.categories.map(category => (
                  <div key={category.id} className="mb-4 p-4 bg-white rounded-lg">
                    <h4 className="font-medium mb-3">{category.name}</h4>
                    {category.competencies?.map(comp => {
                      const score = getScore(`basic-${category.id}`, comp.id);
                      return (
                        <div key={comp.id} className="flex flex-col sm:flex-row items-start sm:items-center justify-between mb-3 p-3 bg-gray-50 rounded border">
                          <div className="flex-1 mb-2 sm:mb-0">
                            <span className="font-medium">{comp.name}</span>
                            <span className="text-sm text-gray-500 ml-2">(Max: {comp.maxScore})</span>
                          </div>
                          <div className="flex gap-2">
                            {[0, 1, 2, 3].slice(0, comp.maxScore + 1).map(val => (
                              <button
                                key={val}
                                onClick={() => setScore(`basic-${category.id}`, comp.id, val)}
                                disabled={isEngineerView}
                                className={`w-12 h-12 rounded-md font-semibold transition-all ${
                                  score === val 
                                    ? 'bg-green-600 text-white scale-110 shadow-lg' 
                                    : isEngineerView
                                    ? 'bg-gray-200 text-gray-500 cursor-not-allowed'
                                    : 'bg-white border-2 border-gray-300 hover:border-green-400 hover:bg-green-50'
                                }`}
                              >
                                {val}
                              </button>
                            ))}
                          </div>
                        </div>
                      );
                    })}
                  </div>
                ))}
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
}
