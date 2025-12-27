-- =============================================
-- Insert Core Skills Questionnaires
-- =============================================

-- Note: This script uses NEWID() for SQL Server GUIDs
-- If a category/skill already exists, you may need to delete it first

-- =============================================
-- MECHANICAL SKILLS CATEGORIES
-- =============================================

-- 1.1 Welding
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Welding', GETDATE());

DECLARE @WeldingId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Welding' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @WeldingId, 'Proficient in setting up TIG/MIG welding equipment including setting parameters such as gas flow and voltage or setting up the torch', 3, GETDATE()),
(NEWID(), @WeldingId, 'Knowledge of correct welding techniques for TIG/MIG welding', 3, GETDATE()),
(NEWID(), @WeldingId, 'Understand the safety practices. Including proper ventilation, correct PPE and precautions to prevent exposure to UV radiation', 3, GETDATE());

-- 1.2 Lathe
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Lathe', GETDATE());

DECLARE @LatheId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Lathe' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @LatheId, 'Able to correctly set up the lathe including the correct set up of tool rests/centers', 3, GETDATE()),
(NEWID(), @LatheId, 'Able to select correct tool and knowledge of their application such as turning tools/boring bars and parting tools', 3, GETDATE()),
(NEWID(), @LatheId, 'Knowledge of various lathe operations including facing, turning, boring, threading.', 3, GETDATE()),
(NEWID(), @LatheId, 'Understand potential hazards including rotating parts, debris or entanglement', 3, GETDATE()),
(NEWID(), @LatheId, 'Able to set correct cutting speed, feed rate and depth of cut and understand the difference with type of material', 3, GETDATE());

-- 1.3 Mill
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Mill', GETDATE());

DECLARE @MillId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Mill' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @MillId, 'Knowledge of correctly setting the milling machine up including alignment of piece to the cutting tool.', 3, GETDATE()),
(NEWID(), @MillId, 'Knowledge of different types of cutters such as end/face/slot and their application or impact on different material', 3, GETDATE()),
(NEWID(), @MillId, 'Understanding of speed/feed selection and their impact depending on the material being used.', 3, GETDATE()),
(NEWID(), @MillId, 'Knowledge of different milling operations such as face,end slotting, drilling and profiling.', 3, GETDATE()),
(NEWID(), @MillId, 'Understand potential hazards including rotating parts, debris or entanglement', 3, GETDATE());

-- 1.4 Gearbox
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Gearbox', GETDATE());

DECLARE @GearboxId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Gearbox' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @GearboxId, 'Understanding of different gearbox types such as spur, helical, bevel, worm and planetary gearboxes and their application', 3, GETDATE()),
(NEWID(), @GearboxId, 'Able to inspect gearboxes for wear/misalignment and other issues that could impact performance', 3, GETDATE()),
(NEWID(), @GearboxId, 'Able to troubleshoot and identify issues causing noise/vibration or overheating.', 3, GETDATE());

-- 1.5 Pumps
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Pumps', GETDATE());

DECLARE @PumpsId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Pumps' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @PumpsId, 'Understanding of different pump types such as centrifugal, vacuum pump, postitive displacement.', 3, GETDATE()),
(NEWID(), @PumpsId, 'Knowledge or pump components such as impellars, casings, seals and bearings', 3, GETDATE()),
(NEWID(), @PumpsId, 'Proficient in pump maintenance tasks such as replacement of mechanical seals and bearings', 3, GETDATE());

-- 1.6 Pneumatics
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Pneumatics', GETDATE());

DECLARE @PneumaticsId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Pneumatics' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @PneumaticsId, 'Knowledge of pneumatic components such as compressors, actuators, and solenoid valves', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Proficient at fault finding on pneumatic systems such as air leaks, valve failure or actuator failure', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Awareness of potential hazards such as high pressure air, flying debris.', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Able to read and interpret pneumatic diagrams', 3, GETDATE());

-- 1.7 Hydraulics
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Hydraulics', GETDATE());

DECLARE @HydraulicsId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Hydraulics' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @HydraulicsId, 'Knowledge of hydraulic systems, including components such as pumps, actuators , valves/reservoirs, and hoses', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Able to troubleshoot and identify issues such as air entrapment, valve malfunctions or cylinder leaks', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Able to select correct hydraulic components based on application. Understand safety implications of incorrect ratings', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Understand safety protocols including LOTO before working, correct PPE and awareness of high pressures.', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Able to read and interpret hydraulic diagrams', 3, GETDATE());

-- =============================================
-- ELECTRICAL CATEGORIES
-- =============================================

-- Electrical Knowledge
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Electrical Knowledge', GETDATE());

DECLARE @ElecKnowId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Electrical Knowledge' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ElecKnowId, 'Good understanding of electrical theory, principles and practices', 3, GETDATE()),
(NEWID(), @ElecKnowId, 'Knowledge of electrical components and equipment such as motors, transformers, control panels, inverters.', 3, GETDATE()),
(NEWID(), @ElecKnowId, 'Knowledge of troubleshooting, diagnosing and resolving electrical faults.', 3, GETDATE()),
(NEWID(), @ElecKnowId, 'Proficient in reading electrical drawings and schematics.', 3, GETDATE());

-- Electrical Hand skills
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Electrical Hand Skills', GETDATE());

DECLARE @ElecHandId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Electrical Hand Skills' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ElecHandId, 'Proficient in wire termination and cable management techniques', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Familiar with commonly used electrical testing equipment such as multimeters, MEGERs.', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Familiar with correct soldering techniques', 3, GETDATE());

-- Motors
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Motors', GETDATE());

DECLARE @MotorsId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Motors' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @MotorsId, 'Knowledge of different motor types such as AC, DC, Servo, single phase, 3 phase.', 3, GETDATE()),
(NEWID(), @MotorsId, 'Understanding of motor construction (stator, rotor, windings, bearings)', 3, GETDATE()),
(NEWID(), @MotorsId, 'Familiarity and able to identify different motor ratings (voltage, current, power, speed)', 3, GETDATE()),
(NEWID(), @MotorsId, 'Understanding and able to identify different winding connections (star, delta etc)', 3, GETDATE()),
(NEWID(), @MotorsId, 'Able to electrically test and confirm serviceability using insulation resistance testing and multimeter techniques.', 3, GETDATE());

-- Inverters
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Inverters', GETDATE());

DECLARE @InvertersId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Inverters' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @InvertersId, 'Knowledge of different inverter types (single phase, 3 phase)', 3, GETDATE()),
(NEWID(), @InvertersId, 'Familiar with inverter programming and parameter setting', 3, GETDATE()),
(NEWID(), @InvertersId, 'Able to interpret inverter fault codes and troubleshoot issues.', 3, GETDATE()),
(NEWID(), @InvertersId, 'Knowledge of inverter testing including insulation resistance and using different multimeter functions.', 3, GETDATE()),
(NEWID(), @InvertersId, 'Able to identify and replace with suitable spares.', 3, GETDATE());

-- Control Panels
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Control Panels', GETDATE());

DECLARE @ControlPanelsId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Control Panels' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ControlPanelsId, 'Able to indentify common components within an electrical control panel such as contactors, power supplies, relays etc.', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Able to read and interpret control panel drawings and documentation', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Able to perform control panel component installation/modification and replacement as per correct drawings', 3, GETDATE());

-- =============================================
-- SOFTWARE CATEGORIES
-- =============================================

-- RSLinx
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'RSLinx', GETDATE());

DECLARE @RSLinxId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'RSLinx' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @RSLinxId, 'Able to set up a new connection type for Ethernet Devices', 3, GETDATE()),
(NEWID(), @RSLinxId, 'Able to set up a new connection type for USB Devices', 3, GETDATE());

-- Rockwell Logix 500
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Rockwell Logix 500', GETDATE());

DECLARE @Logix500Id UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Rockwell Logix 500' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @Logix500Id, 'Able to open a logix 500 project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @Logix500Id, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @Logix500Id, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @Logix500Id, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @Logix500Id, 'Able to download a program to the PLC', 3, GETDATE());

-- Rockwell Logix 5000
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Rockwell Logix 5000', GETDATE());

DECLARE @Logix5000Id UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Rockwell Logix 5000' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @Logix5000Id, 'Able to open a Logix 5000 project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @Logix5000Id, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @Logix5000Id, 'Able to read Ladder Logic, FB Diagram, Graph, Structured Text and Statement List programs', 3, GETDATE()),
(NEWID(), @Logix5000Id, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @Logix5000Id, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @Logix5000Id, 'Able to download a program to the PLC', 3, GETDATE());

-- Rockwell Studio 5000
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Rockwell Studio 5000', GETDATE());

DECLARE @Studio5000Id UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Rockwell Studio 5000' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @Studio5000Id, 'Able to open a Studio 5000 project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Able to read Ladder Logic, FB Diagram, Graph, Structured Text and Statement List programs', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Able to download a program to the PLC', 3, GETDATE());

-- Allen Bradley Inverters
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Allen Bradley Inverters', GETDATE());

DECLARE @ABInvertersId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Allen Bradley Inverters' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ABInvertersId, 'Able to install a new inverter and set the basic parameters (IP address, Motor Information, Set Point Reference, Start/Stop Reference)', 3, GETDATE());

-- Staubli Robots
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Staubli Robots', GETDATE());

DECLARE @StaubliId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Staubli Robots' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @StaubliId, 'Able to manually move the robots in joint mode and cartesian mode.', 3, GETDATE()),
(NEWID(), @StaubliId, 'Able to navigate the menus on the pendant to see what application/processes are running as well as seeing the variable data used for the loaded application.', 3, GETDATE()),
(NEWID(), @StaubliId, 'Able to reteach positions within a program', 3, GETDATE()),
(NEWID(), @StaubliId, 'Able to Backup/Restore a Program', 3, GETDATE());

-- Mitsubishi GX Developers
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Mitsubishi GX Developers', GETDATE());

DECLARE @MitsubishiId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Mitsubishi GX Developers' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @MitsubishiId, 'Able to open a project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @MitsubishiId, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @MitsubishiId, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @MitsubishiId, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @MitsubishiId, 'Able to download a program to the PLC', 3, GETDATE());

-- ABB Robots
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'ABB Robots', GETDATE());

DECLARE @ABBId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'ABB Robots' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ABBId, 'Able to manually move the robots in joint mode and cartesian mode.', 3, GETDATE()),
(NEWID(), @ABBId, 'Able to recalibrate each axis on the robot', 3, GETDATE()),
(NEWID(), @ABBId, 'Able to reteach positions within a program', 3, GETDATE()),
(NEWID(), @ABBId, 'Able to Backup/Restore a Program', 3, GETDATE());

-- Cognex Camera Insight
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Cognex Camera Insight', GETDATE());

DECLARE @CognexId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Cognex Camera Insight' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @CognexId, 'Able to connect to a cognex camera using Insight Explorer', 3, GETDATE()),
(NEWID(), @CognexId, 'Able to load a job from a Cognex Camera', 3, GETDATE()),
(NEWID(), @CognexId, 'Able to modify search regions / reference positions', 3, GETDATE()),
(NEWID(), @CognexId, 'Able to manually trigger the camera from Insight Explorer', 3, GETDATE()),
(NEWID(), @CognexId, 'Able to save a job to the Camera', 3, GETDATE());

-- Siemens S7
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Siemens S7', GETDATE());

DECLARE @SiemensS7Id UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Siemens S7' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @SiemensS7Id, 'Able to open a project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @SiemensS7Id, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @SiemensS7Id, 'Able to read Ladder Logic, FB Diagram, Graph, Structured Text and Statement List programs', 3, GETDATE()),
(NEWID(), @SiemensS7Id, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @SiemensS7Id, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @SiemensS7Id, 'Able to download a program to the PLC', 3, GETDATE());

-- Siemens TIA
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Siemens TIA', GETDATE());

DECLARE @SiemensTIAId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Siemens TIA' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @SiemensTIAId, 'Able to open a project and connect online to the PLC', 3, GETDATE()),
(NEWID(), @SiemensTIAId, 'Have an understanding of the different types of programming operations typically used in Ladder Logic (XIC, XIO, OTE, OTL, OTU, ONS, MOV, CLR)', 3, GETDATE()),
(NEWID(), @SiemensTIAId, 'Able to read Ladder Logic, FB Diagram, Graph, Structured Text and Statement List programs', 3, GETDATE()),
(NEWID(), @SiemensTIAId, 'Able to use the referencing feature to trace the use of variables through the software for fault finding', 3, GETDATE()),
(NEWID(), @SiemensTIAId, 'Able to toggle or modify variables within the program', 3, GETDATE()),
(NEWID(), @SiemensTIAId, 'Able to download a program to the PLC', 3, GETDATE());

-- Siemens Inverters
INSERT INTO CoreSkillCategories (id, name, created_at)
VALUES (NEWID(), 'Siemens Inverters', GETDATE());

DECLARE @SiemensInvId UNIQUEIDENTIFIER = (SELECT TOP 1 id FROM CoreSkillCategories WHERE name = 'Siemens Inverters' ORDER BY created_at DESC);

INSERT INTO CoreSkills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @SiemensInvId, 'Able to install a new inverter and set the basic parameters (IP address, Motor Information, Set Point Reference, Start/Stop Reference)', 3, GETDATE()),
(NEWID(), @SiemensInvId, 'Able to Connect to an Inverter through Simatic Manager Or Tia', 3, GETDATE()),
(NEWID(), @SiemensInvId, 'Able to re-address an Inverter in a network (IP address, Profibus Node)', 3, GETDATE());

PRINT 'Core Skills import completed successfully!';
PRINT 'Total Categories: 24';
PRINT 'Total Skills: Approximately 110+';
