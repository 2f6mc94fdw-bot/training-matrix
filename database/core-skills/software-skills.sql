-- Software Skills - Core Skills Update
-- Replace default software skills with detailed competencies

-- Delete existing software skills
DELETE FROM core_skills WHERE category_id = 'software';
GO

-- 1. RSLinx
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('rslinx-ethernet', 'software', '1.1 Able to set up new connection type for Ethernet Devices', 3),
    ('rslinx-usb', 'software', '1.2 Able to set up new connection type for USB Devices', 3);

-- 2. Rockwell Logix 500
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('logix500-connect', 'software', '2.1 Able to open Logix 500 project and connect online to PLC', 3),
    ('logix500-operations', 'software', '2.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('logix500-referencing', 'software', '2.3 Able to use referencing feature to trace variables for fault finding', 3),
    ('logix500-modify', 'software', '2.4 Able to toggle or modify variables within the program', 3),
    ('logix500-download', 'software', '2.5 Able to download a program to the PLC', 3);

-- 3. Rockwell Logix 5000
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('logix5000-connect', 'software', '3.1 Able to open Logix 5000 project and connect online to PLC', 3),
    ('logix5000-operations', 'software', '3.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('logix5000-languages', 'software', '3.3 Able to read Ladder Logic, FB Diagram, Graph, Structured Text, Statement List', 3),
    ('logix5000-referencing', 'software', '3.4 Able to use referencing feature to trace variables for fault finding', 3),
    ('logix5000-modify', 'software', '3.5 Able to toggle or modify variables within the program', 3),
    ('logix5000-download', 'software', '3.6 Able to download a program to the PLC', 3);

-- 4. Rockwell Studio 5000
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('studio5000-connect', 'software', '4.1 Able to open Studio 5000 project and connect online to PLC', 3),
    ('studio5000-operations', 'software', '4.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('studio5000-languages', 'software', '4.3 Able to read Ladder Logic, FB Diagram, Graph, Structured Text, Statement List', 3),
    ('studio5000-referencing', 'software', '4.4 Able to use referencing feature to trace variables for fault finding', 3),
    ('studio5000-modify', 'software', '4.5 Able to toggle or modify variables within the program', 3),
    ('studio5000-download', 'software', '4.6 Able to download a program to the PLC', 3);

-- 5. Allen Bradley Inverters
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('ab-inverter-setup', 'software', '5.1 Able to install inverter and set parameters (IP, Motor Info, Set Point, Start/Stop)', 3);

-- 6. Staubli Robots
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('staubli-movement', 'software', '6.1 Able to manually move robots in joint mode and cartesian mode', 3),
    ('staubli-menus', 'software', '6.2 Navigate menus to see applications/processes and variable data', 3),
    ('staubli-reteach', 'software', '6.3 Able to reteach positions within a program', 3),
    ('staubli-backup', 'software', '6.4 Able to Backup/Restore a Program', 3);

-- 7. Mitsubishi GX Developers
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('mitsubishi-connect', 'software', '7.1 Able to open project and connect online to PLC', 3),
    ('mitsubishi-operations', 'software', '7.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('mitsubishi-referencing', 'software', '7.3 Able to use referencing feature to trace variables for fault finding', 3),
    ('mitsubishi-modify', 'software', '7.4 Able to toggle or modify variables within the program', 3),
    ('mitsubishi-download', 'software', '7.5 Able to download a program to the PLC', 3);

-- 8. ABB Robots
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('abb-movement', 'software', '8.1 Able to manually move robots in joint mode and cartesian mode', 3),
    ('abb-calibration', 'software', '8.2 Able to recalibrate each axis on the robot', 3),
    ('abb-reteach', 'software', '8.3 Able to reteach positions within a program', 3),
    ('abb-backup', 'software', '8.4 Able to Backup/Restore a Program', 3);

-- 9. Cognex Camera Insight
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('cognex-connect', 'software', '9.1 Able to connect to Cognex camera using Insight Explorer', 3),
    ('cognex-load', 'software', '9.2 Able to load a job from a Cognex Camera', 3),
    ('cognex-regions', 'software', '9.3 Able to modify search regions / reference positions', 3),
    ('cognex-trigger', 'software', '9.4 Able to manually trigger camera from Insight Explorer', 3),
    ('cognex-save', 'software', '9.5 Able to save a job to the Camera', 3);

-- 10. Siemens S7
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('siemens-s7-connect', 'software', '10.1 Able to open project and connect online to PLC', 3),
    ('siemens-s7-operations', 'software', '10.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('siemens-s7-languages', 'software', '10.3 Able to read Ladder Logic, FB Diagram, Graph, Structured Text, Statement List', 3),
    ('siemens-s7-referencing', 'software', '10.4 Able to use referencing feature to trace variables for fault finding', 3),
    ('siemens-s7-modify', 'software', '10.5 Able to toggle or modify variables within the program', 3),
    ('siemens-s7-download', 'software', '10.6 Able to download a program to the PLC', 3);

-- 11. Siemens TIA
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('siemens-tia-connect', 'software', '11.1 Able to open project and connect online to PLC', 3),
    ('siemens-tia-operations', 'software', '11.2 Understanding of Ladder Logic operations (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3),
    ('siemens-tia-languages', 'software', '11.3 Able to read Ladder Logic, FB Diagram, Graph, Structured Text, Statement List', 3),
    ('siemens-tia-referencing', 'software', '11.4 Able to use referencing feature to trace variables for fault finding', 3),
    ('siemens-tia-modify', 'software', '11.5 Able to toggle or modify variables within the program', 3),
    ('siemens-tia-download', 'software', '11.6 Able to download a program to the PLC', 3);

-- 12. Siemens Inverters
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('siemens-inverter-setup', 'software', '12.1 Able to install inverter and set parameters (IP, Motor Info, Set Point, Start/Stop)', 3),
    ('siemens-inverter-connect', 'software', '12.2 Able to connect to inverter through Simatic Manager or TIA', 3),
    ('siemens-inverter-address', 'software', '12.3 Able to re-address inverter in network (IP address, Profibus Node)', 3);

GO

PRINT 'Software Skills updated successfully!';
PRINT 'Categories: 12 (RSLinx, Logix 500, Logix 5000, Studio 5000, AB Inverters, Staubli, Mitsubishi, ABB, Cognex, Siemens S7, Siemens TIA, Siemens Inverters)';
PRINT 'Total Skills: 53';
GO

-- Verify the update
SELECT
    cs.category_id,
    COUNT(*) AS SkillCount
FROM core_skills cs
WHERE cs.category_id = 'software'
GROUP BY cs.category_id;

SELECT
    cs.id,
    cs.name,
    cs.max_score
FROM core_skills cs
WHERE cs.category_id = 'software'
ORDER BY cs.id;
GO
