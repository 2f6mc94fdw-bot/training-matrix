-- Mechanical Skills - Core Skills Update
-- Replace default mechanical skills with detailed competencies

-- Delete existing mechanical skills
DELETE FROM core_skills WHERE category_id = 'mechanical';
GO

-- 1.1 Welding
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('welding-setup', 'mechanical', '1.1.1 Proficient in setting up TIG/MIG welding equipment (gas flow, voltage, torch)', 3),
    ('welding-techniques', 'mechanical', '1.1.2 Knowledge of correct welding techniques for TIG/MIG welding', 3),
    ('welding-safety', 'mechanical', '1.1.3 Understand safety practices (ventilation, PPE, UV radiation protection)', 3);

-- 1.2 Lathe
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('lathe-setup', 'mechanical', '1.2.1 Able to correctly set up lathe including tool rests/centers', 3),
    ('lathe-tools', 'mechanical', '1.2.2 Able to select correct tool and knowledge of application (turning/boring/parting)', 3),
    ('lathe-operations', 'mechanical', '1.2.3 Knowledge of lathe operations (facing, turning, boring, threading)', 3),
    ('lathe-safety', 'mechanical', '1.2.4 Understand potential hazards (rotating parts, debris, entanglement)', 3),
    ('lathe-parameters', 'mechanical', '1.2.5 Able to set cutting speed, feed rate, depth of cut for different materials', 3);

-- 1.3 Mill
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('mill-setup', 'mechanical', '1.3.1 Knowledge of setting up milling machine (alignment of piece to cutting tool)', 3),
    ('mill-cutters', 'mechanical', '1.3.2 Knowledge of cutter types (end/face/slot) and their application/material impact', 3),
    ('mill-parameters', 'mechanical', '1.3.3 Understanding of speed/feed selection and impact on different materials', 3),
    ('mill-operations', 'mechanical', '1.3.4 Knowledge of milling operations (face, end slotting, drilling, profiling)', 3),
    ('mill-safety', 'mechanical', '1.3.5 Understand potential hazards (rotating parts, debris, entanglement)', 3);

-- 1.4 Gearbox
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('gearbox-types', 'mechanical', '1.4.1 Understanding of gearbox types (spur, helical, bevel, worm, planetary)', 3),
    ('gearbox-inspection', 'mechanical', '1.4.2 Able to inspect gearboxes for wear/misalignment and performance issues', 3),
    ('gearbox-troubleshooting', 'mechanical', '1.4.3 Able to troubleshoot issues causing noise/vibration or overheating', 3);

-- 1.5 Pumps
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('pump-types', 'mechanical', '1.5.1 Understanding of pump types (centrifugal, vacuum, positive displacement)', 3),
    ('pump-components', 'mechanical', '1.5.2 Knowledge of pump components (impellers, casings, seals, bearings)', 3),
    ('pump-maintenance', 'mechanical', '1.5.3 Proficient in pump maintenance (mechanical seal and bearing replacement)', 3);

-- 1.6 Pneumatics
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('pneumatics-components', 'mechanical', '1.6.1 Knowledge of pneumatic components (compressors, actuators, solenoid valves)', 3),
    ('pneumatics-troubleshooting', 'mechanical', '1.6.2 Proficient at fault finding (air leaks, valve failure, actuator failure)', 3),
    ('pneumatics-safety', 'mechanical', '1.6.3 Awareness of potential hazards (high pressure air, flying debris)', 3),
    ('pneumatics-diagrams', 'mechanical', '1.6.4 Able to read and interpret pneumatic diagrams', 3);

-- 1.7 Hydraulics
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('hydraulics-systems', 'mechanical', '1.7.1 Knowledge of hydraulic systems and components (pumps, actuators, valves, hoses)', 3),
    ('hydraulics-troubleshooting', 'mechanical', '1.7.2 Able to troubleshoot issues (air entrapment, valve malfunctions, cylinder leaks)', 3),
    ('hydraulics-selection', 'mechanical', '1.7.3 Able to select correct components and understand safety of incorrect ratings', 3),
    ('hydraulics-safety', 'mechanical', '1.7.4 Understand safety protocols (LOTO, correct PPE, high pressure awareness)', 3),
    ('hydraulics-diagrams', 'mechanical', '1.7.5 Able to read and interpret hydraulic diagrams', 3);

GO

PRINT 'Mechanical Skills updated successfully!';
PRINT 'Categories: 7 (Welding, Lathe, Mill, Gearbox, Pumps, Pneumatics, Hydraulics)';
PRINT 'Total Skills: 28';
GO

-- Verify the update
SELECT
    cs.category_id,
    COUNT(*) AS SkillCount
FROM core_skills cs
WHERE cs.category_id = 'mechanical'
GROUP BY cs.category_id;

SELECT
    cs.id,
    cs.name,
    cs.max_score
FROM core_skills cs
WHERE cs.category_id = 'mechanical'
ORDER BY cs.id;
GO
