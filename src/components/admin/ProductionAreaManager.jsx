import { useState } from 'react';
import { Plus, Edit2, Trash2, ChevronDown, ChevronRight } from 'lucide-react';

export const ProductionAreaManager = ({ 
  data, 
  onAddArea, 
  onUpdateArea, 
  onDeleteArea,
  onAddMachine,
  onUpdateMachine,
  onDeleteMachine,
  onAddCompetency,
  onUpdateCompetency,
  onDeleteCompetency
}) => {
  const [expandedAreas, setExpandedAreas] = useState({});
  const [expandedMachines, setExpandedMachines] = useState({});
  const [editingArea, setEditingArea] = useState(null);
  const [editingMachine, setEditingMachine] = useState(null);
  const [editingComp, setEditingComp] = useState(null);
  const [showAddArea, setShowAddArea] = useState(false);
  const [showAddMachine, setShowAddMachine] = useState(null);
  const [showAddComp, setShowAddComp] = useState(null);
  
  const [newAreaName, setNewAreaName] = useState('');
  const [newMachineName, setNewMachineName] = useState('');
  const [newMachineImportance, setNewMachineImportance] = useState(1);
  const [newCompName, setNewCompName] = useState('');
  const [newCompMaxScore, setNewCompMaxScore] = useState(3);
  
  const toggleArea = (areaId) => {
    setExpandedAreas(prev => ({ ...prev, [areaId]: !prev[areaId] }));
  };
  
  const toggleMachine = (machineId) => {
    setExpandedMachines(prev => ({ ...prev, [machineId]: !prev[machineId] }));
  };
  
  const handleAddArea = () => {
    if (newAreaName.trim()) {
      onAddArea({ name: newAreaName });
      setNewAreaName('');
      setShowAddArea(false);
    }
  };
  
  const handleAddMachine = (areaId) => {
    if (newMachineName.trim()) {
      onAddMachine(areaId, { name: newMachineName, importance: newMachineImportance });
      setNewMachineName('');
      setNewMachineImportance(1);
      setShowAddMachine(null);
    }
  };
  
  const handleAddComp = (areaId, machineId) => {
    if (newCompName.trim()) {
      onAddCompetency(areaId, machineId, { name: newCompName, maxScore: newCompMaxScore });
      setNewCompName('');
      setNewCompMaxScore(3);
      setShowAddComp(null);
    }
  };
  
  return (
    <div className="space-y-4">
      <div className="flex justify-between items-center">
        <h3 className="text-lg font-semibold">Production Areas & Machines</h3>
        <button
          onClick={() => setShowAddArea(!showAddArea)}
          className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
        >
          <Plus className="w-4 h-4" />
          Add Production Area
        </button>
      </div>
      
      {showAddArea && (
        <div className="p-4 bg-gray-50 rounded-lg border border-gray-200">
          <input
            type="text"
            value={newAreaName}
            onChange={(e) => setNewAreaName(e.target.value)}
            placeholder="Area name"
            className="w-full px-3 py-2 border rounded mb-2"
            onKeyPress={(e) => e.key === 'Enter' && handleAddArea()}
          />
          <div className="flex gap-2">
            <button
              onClick={handleAddArea}
              className="px-4 py-2 bg-green-600 text-white rounded hover:bg-green-700"
            >
              Add
            </button>
            <button
              onClick={() => {
                setShowAddArea(false);
                setNewAreaName('');
              }}
              className="px-4 py-2 bg-gray-300 rounded hover:bg-gray-400"
            >
              Cancel
            </button>
          </div>
        </div>
      )}
      
      <div className="space-y-2">
        {data.productionAreas.map(area => (
          <div key={area.id} className="border rounded-lg overflow-hidden">
            <div className="bg-gray-100 p-3 flex items-center justify-between">
              <div className="flex items-center gap-2 flex-1">
                <button onClick={() => toggleArea(area.id)} className="p-1">
                  {expandedAreas[area.id] ? (
                    <ChevronDown className="w-4 h-4" />
                  ) : (
                    <ChevronRight className="w-4 h-4" />
                  )}
                </button>
                
                {editingArea === area.id ? (
                  <input
                    type="text"
                    defaultValue={area.name}
                    onBlur={(e) => {
                      onUpdateArea(area.id, { name: e.target.value });
                      setEditingArea(null);
                    }}
                    onKeyPress={(e) => {
                      if (e.key === 'Enter') {
                        onUpdateArea(area.id, { name: e.target.value });
                        setEditingArea(null);
                      }
                    }}
                    className="px-2 py-1 border rounded flex-1"
                    autoFocus
                  />
                ) : (
                  <span className="font-medium flex-1">{area.name}</span>
                )}
              </div>
              
              <div className="flex gap-2">
                <button
                  onClick={() => setShowAddMachine(area.id)}
                  className="p-1 text-blue-600 hover:bg-blue-50 rounded"
                  title="Add machine"
                >
                  <Plus className="w-4 h-4" />
                </button>
                <button
                  onClick={() => setEditingArea(area.id)}
                  className="p-1 text-gray-600 hover:bg-gray-200 rounded"
                  title="Edit"
                >
                  <Edit2 className="w-4 h-4" />
                </button>
                <button
                  onClick={() => {
                    if (window.confirm(`Delete production area "${area.name}"?`)) {
                      onDeleteArea(area.id);
                    }
                  }}
                  className="p-1 text-red-600 hover:bg-red-50 rounded"
                  title="Delete"
                >
                  <Trash2 className="w-4 h-4" />
                </button>
              </div>
            </div>
            
            {showAddMachine === area.id && (
              <div className="p-4 bg-blue-50 border-t">
                <div className="space-y-2">
                  <input
                    type="text"
                    value={newMachineName}
                    onChange={(e) => setNewMachineName(e.target.value)}
                    placeholder="Machine name"
                    className="w-full px-3 py-2 border rounded"
                  />
                  <div>
                    <label className="block text-sm mb-1">Importance (1-5)</label>
                    <input
                      type="number"
                      min="1"
                      max="5"
                      value={newMachineImportance}
                      onChange={(e) => setNewMachineImportance(parseInt(e.target.value))}
                      className="w-full px-3 py-2 border rounded"
                    />
                  </div>
                  <div className="flex gap-2">
                    <button
                      onClick={() => handleAddMachine(area.id)}
                      className="px-4 py-2 bg-green-600 text-white rounded hover:bg-green-700"
                    >
                      Add Machine
                    </button>
                    <button
                      onClick={() => {
                        setShowAddMachine(null);
                        setNewMachineName('');
                        setNewMachineImportance(1);
                      }}
                      className="px-4 py-2 bg-gray-300 rounded hover:bg-gray-400"
                    >
                      Cancel
                    </button>
                  </div>
                </div>
              </div>
            )}
            
            {expandedAreas[area.id] && (
              <div className="p-3 space-y-2">
                {area.machines.map(machine => (
                  <div key={machine.id} className="border rounded-lg overflow-hidden bg-white">
                    <div className="bg-gray-50 p-2 flex items-center justify-between">
                      <div className="flex items-center gap-2 flex-1">
                        <button onClick={() => toggleMachine(machine.id)} className="p-1">
                          {expandedMachines[machine.id] ? (
                            <ChevronDown className="w-4 h-4" />
                          ) : (
                            <ChevronRight className="w-4 h-4" />
                          )}
                        </button>
                        
                        {editingMachine === machine.id ? (
                          <div className="flex-1 space-y-1">
                            <input
                              type="text"
                              defaultValue={machine.name}
                              className="px-2 py-1 border rounded w-full"
                              onBlur={(e) => {
                                onUpdateMachine(area.id, machine.id, { name: e.target.value });
                                setEditingMachine(null);
                              }}
                            />
                          </div>
                        ) : (
                          <span className="flex-1">
                            {machine.name}
                            <span className="ml-2 text-xs text-gray-500">
                              (Importance: {machine.importance})
                            </span>
                          </span>
                        )}
                      </div>
                      
                      <div className="flex gap-2">
                        <button
                          onClick={() => setShowAddComp(`${area.id}-${machine.id}`)}
                          className="p-1 text-blue-600 hover:bg-blue-50 rounded"
                          title="Add competency"
                        >
                          <Plus className="w-4 h-4" />
                        </button>
                        <button
                          onClick={() => setEditingMachine(machine.id)}
                          className="p-1 text-gray-600 hover:bg-gray-200 rounded"
                          title="Edit"
                        >
                          <Edit2 className="w-4 h-4" />
                        </button>
                        <button
                          onClick={() => {
                            if (window.confirm(`Delete machine "${machine.name}"?`)) {
                              onDeleteMachine(area.id, machine.id);
                            }
                          }}
                          className="p-1 text-red-600 hover:bg-red-50 rounded"
                          title="Delete"
                        >
                          <Trash2 className="w-4 h-4" />
                        </button>
                      </div>
                    </div>
                    
                    {showAddComp === `${area.id}-${machine.id}` && (
                      <div className="p-3 bg-green-50 border-t">
                        <div className="space-y-2">
                          <input
                            type="text"
                            value={newCompName}
                            onChange={(e) => setNewCompName(e.target.value)}
                            placeholder="Competency name"
                            className="w-full px-3 py-2 border rounded"
                          />
                          <div>
                            <label className="block text-sm mb-1">Max Score</label>
                            <input
                              type="number"
                              min="1"
                              max="10"
                              value={newCompMaxScore}
                              onChange={(e) => setNewCompMaxScore(parseInt(e.target.value))}
                              className="w-full px-3 py-2 border rounded"
                            />
                          </div>
                          <div className="flex gap-2">
                            <button
                              onClick={() => handleAddComp(area.id, machine.id)}
                              className="px-4 py-2 bg-green-600 text-white rounded hover:bg-green-700 text-sm"
                            >
                              Add Competency
                            </button>
                            <button
                              onClick={() => {
                                setShowAddComp(null);
                                setNewCompName('');
                                setNewCompMaxScore(3);
                              }}
                              className="px-4 py-2 bg-gray-300 rounded hover:bg-gray-400 text-sm"
                            >
                              Cancel
                            </button>
                          </div>
                        </div>
                      </div>
                    )}
                    
                    {expandedMachines[machine.id] && (
                      <div className="p-3 space-y-1">
                        {machine.competencies.map(comp => (
                          <div key={comp.id} className="flex items-center justify-between p-2 bg-gray-50 rounded">
                            {editingComp === comp.id ? (
                              <input
                                type="text"
                                defaultValue={comp.name}
                                className="flex-1 px-2 py-1 border rounded mr-2"
                                onBlur={(e) => {
                                  onUpdateCompetency(area.id, machine.id, comp.id, { name: e.target.value });
                                  setEditingComp(null);
                                }}
                              />
                            ) : (
                              <span className="flex-1">
                                {comp.name}
                                <span className="ml-2 text-xs text-gray-500">
                                  (Max: {comp.maxScore})
                                </span>
                              </span>
                            )}
                            
                            <div className="flex gap-2">
                              <button
                                onClick={() => setEditingComp(comp.id)}
                                className="p-1 text-gray-600 hover:bg-gray-200 rounded"
                                title="Edit"
                              >
                                <Edit2 className="w-3 h-3" />
                              </button>
                              <button
                                onClick={() => {
                                  if (window.confirm(`Delete competency "${comp.name}"?`)) {
                                    onDeleteCompetency(area.id, machine.id, comp.id);
                                  }
                                }}
                                className="p-1 text-red-600 hover:bg-red-50 rounded"
                                title="Delete"
                              >
                                <Trash2 className="w-3 h-3" />
                              </button>
                            </div>
                          </div>
                        ))}
                        {machine.competencies.length === 0 && (
                          <p className="text-sm text-gray-500 italic">No competencies added yet</p>
                        )}
                      </div>
                    )}
                  </div>
                ))}
                {area.machines.length === 0 && (
                  <p className="text-sm text-gray-500 italic">No machines added yet</p>
                )}
              </div>
            )}
          </div>
        ))}
        {data.productionAreas.length === 0 && (
          <p className="text-gray-500 italic">No production areas added yet</p>
        )}
      </div>
    </div>
  );
};
