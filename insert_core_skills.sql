-- Core Skills Import Script
-- Uses UNIQUEIDENTIFIER (GUID) types with NEWID()

-- MECHANICAL SKILLS

-- Welding
DECLARE @WeldingId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@WeldingId, 'Welding', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @WeldingId, 'Can you stick weld?', 3, GETDATE()),
(NEWID(), @WeldingId, 'Can you TIG weld?', 3, GETDATE()),
(NEWID(), @WeldingId, 'Can you MIG weld?', 3, GETDATE()),
(NEWID(), @WeldingId, 'Can you Braze?', 3, GETDATE());

-- Lathe
DECLARE @LatheId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@LatheId, 'Lathe', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @LatheId, 'Can you use a lathe to turn metal?', 3, GETDATE()),
(NEWID(), @LatheId, 'Can you use a lathe to thread metal?', 3, GETDATE()),
(NEWID(), @LatheId, 'Can you use a lathe to drill metal?', 3, GETDATE());

-- Mill
DECLARE @MillId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@MillId, 'Mill', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @MillId, 'Can you use a mill to cut metal?', 3, GETDATE()),
(NEWID(), @MillId, 'Can you use a mill to drill metal?', 3, GETDATE());

-- Gearbox
DECLARE @GearboxId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@GearboxId, 'Gearbox', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @GearboxId, 'Can you dismantle a gearbox to inspect the bearings, shaft, and gears?', 3, GETDATE()),
(NEWID(), @GearboxId, 'Can you re-assemble a gearbox?', 3, GETDATE()),
(NEWID(), @GearboxId, 'Can you align a gearbox to a motor using a dial gauge?', 3, GETDATE()),
(NEWID(), @GearboxId, 'Can you change gearbox oil?', 3, GETDATE()),
(NEWID(), @GearboxId, 'Can you replace gearbox seals?', 3, GETDATE());

-- Pumps
DECLARE @PumpsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@PumpsId, 'Pumps', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @PumpsId, 'Can you dismantle a pump to inspect the bearings, shaft, and impeller?', 3, GETDATE()),
(NEWID(), @PumpsId, 'Can you re-assemble a pump?', 3, GETDATE()),
(NEWID(), @PumpsId, 'Can you align a pump to a motor using a dial gauge?', 3, GETDATE()),
(NEWID(), @PumpsId, 'Can you replace pump seals?', 3, GETDATE());

-- Pneumatics
DECLARE @PneumaticsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@PneumaticsId, 'Pneumatics', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @PneumaticsId, 'Can you troubleshoot a pneumatic circuit?', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Can you replace a pneumatic valve?', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Can you replace a pneumatic cylinder?', 3, GETDATE()),
(NEWID(), @PneumaticsId, 'Can you replace a pneumatic air line?', 3, GETDATE());

-- Hydraulics
DECLARE @HydraulicsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@HydraulicsId, 'Hydraulics', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @HydraulicsId, 'Can you troubleshoot a hydraulic circuit?', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Can you replace a hydraulic valve?', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Can you replace a hydraulic cylinder?', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Can you replace a hydraulic hose?', 3, GETDATE()),
(NEWID(), @HydraulicsId, 'Can you change hydraulic oil?', 3, GETDATE());

-- ELECTRICAL SKILLS

-- Electrical Knowledge
DECLARE @ElecKnowId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@ElecKnowId, 'Electrical Knowledge', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ElecKnowId, 'Do you understand Ohm''s Law?', 3, GETDATE()),
(NEWID(), @ElecKnowId, 'Can you read electrical drawings?', 3, GETDATE()),
(NEWID(), @ElecKnowId, 'Can you troubleshoot an electrical circuit?', 3, GETDATE());

-- Electrical Hand Skills
DECLARE @ElecHandId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@ElecHandId, 'Electrical Hand Skills', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ElecHandId, 'Can you use a multi-meter to measure voltage?', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Can you use a multi-meter to measure current?', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Can you use a multi-meter to measure resistance?', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Can you crimp a terminal?', 3, GETDATE()),
(NEWID(), @ElecHandId, 'Can you solder a wire?', 3, GETDATE());

-- Motors
DECLARE @MotorsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@MotorsId, 'Motors', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @MotorsId, 'Can you replace a motor?', 3, GETDATE()),
(NEWID(), @MotorsId, 'Can you wire a motor?', 3, GETDATE()),
(NEWID(), @MotorsId, 'Can you troubleshoot a motor?', 3, GETDATE());

-- Inverters
DECLARE @InvertersId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@InvertersId, 'Inverters', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @InvertersId, 'Can you replace an inverter?', 3, GETDATE()),
(NEWID(), @InvertersId, 'Can you wire an inverter?', 3, GETDATE()),
(NEWID(), @InvertersId, 'Can you troubleshoot an inverter?', 3, GETDATE()),
(NEWID(), @InvertersId, 'Can you configure an inverter?', 3, GETDATE());

-- Control Panels
DECLARE @ControlPanelsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@ControlPanelsId, 'Control Panels', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @ControlPanelsId, 'Can you troubleshoot a control panel?', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Can you replace a relay?', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Can you replace a contactor?', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Can you replace a circuit breaker?', 3, GETDATE()),
(NEWID(), @ControlPanelsId, 'Can you replace a fuse?', 3, GETDATE());

-- SOFTWARE SKILLS

-- RSLinx
DECLARE @RSLinxId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@RSLinxId, 'RSLinx', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @RSLinxId, 'Can you use RSLinx to connect to a PLC?', 3, GETDATE()),
(NEWID(), @RSLinxId, 'Can you use RSLinx to troubleshoot a PLC?', 3, GETDATE());

-- RSLogix 500
DECLARE @RSLogix500Id UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@RSLogix500Id, 'RSLogix 500', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @RSLogix500Id, 'Can you use RSLogix 500 to program a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix500Id, 'Can you use RSLogix 500 to troubleshoot a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix500Id, 'Can you use RSLogix 500 to download a program to a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix500Id, 'Can you use RSLogix 500 to upload a program from a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix500Id, 'Can you use RSLogix 500 to go online with a PLC?', 3, GETDATE());

-- RSLogix 5000
DECLARE @RSLogix5000Id UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@RSLogix5000Id, 'RSLogix 5000', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @RSLogix5000Id, 'Can you use RSLogix 5000 to program a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix5000Id, 'Can you use RSLogix 5000 to troubleshoot a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix5000Id, 'Can you use RSLogix 5000 to download a program to a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix5000Id, 'Can you use RSLogix 5000 to upload a program from a PLC?', 3, GETDATE()),
(NEWID(), @RSLogix5000Id, 'Can you use RSLogix 5000 to go online with a PLC?', 3, GETDATE());

-- Studio 5000
DECLARE @Studio5000Id UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@Studio5000Id, 'Studio 5000', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @Studio5000Id, 'Can you use Studio 5000 to program a PLC?', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Can you use Studio 5000 to troubleshoot a PLC?', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Can you use Studio 5000 to download a program to a PLC?', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Can you use Studio 5000 to upload a program from a PLC?', 3, GETDATE()),
(NEWID(), @Studio5000Id, 'Can you use Studio 5000 to go online with a PLC?', 3, GETDATE());

-- FactoryTalk View
DECLARE @FactoryTalkId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@FactoryTalkId, 'FactoryTalk View', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @FactoryTalkId, 'Can you use FactoryTalk View to create a HMI screen?', 3, GETDATE()),
(NEWID(), @FactoryTalkId, 'Can you use FactoryTalk View to troubleshoot a HMI screen?', 3, GETDATE()),
(NEWID(), @FactoryTalkId, 'Can you use FactoryTalk View to download a HMI screen to a panel?', 3, GETDATE()),
(NEWID(), @FactoryTalkId, 'Can you use FactoryTalk View to upload a HMI screen from a panel?', 3, GETDATE());

-- TIA Portal
DECLARE @TIAPortalId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@TIAPortalId, 'TIA Portal', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @TIAPortalId, 'Can you use TIA Portal to program a PLC?', 3, GETDATE()),
(NEWID(), @TIAPortalId, 'Can you use TIA Portal to troubleshoot a PLC?', 3, GETDATE()),
(NEWID(), @TIAPortalId, 'Can you use TIA Portal to download a program to a PLC?', 3, GETDATE()),
(NEWID(), @TIAPortalId, 'Can you use TIA Portal to upload a program from a PLC?', 3, GETDATE()),
(NEWID(), @TIAPortalId, 'Can you use TIA Portal to go online with a PLC?', 3, GETDATE());

-- Step 7
DECLARE @Step7Id UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@Step7Id, 'Step 7', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @Step7Id, 'Can you use Step 7 to program a PLC?', 3, GETDATE()),
(NEWID(), @Step7Id, 'Can you use Step 7 to troubleshoot a PLC?', 3, GETDATE()),
(NEWID(), @Step7Id, 'Can you use Step 7 to download a program to a PLC?', 3, GETDATE()),
(NEWID(), @Step7Id, 'Can you use Step 7 to upload a program from a PLC?', 3, GETDATE()),
(NEWID(), @Step7Id, 'Can you use Step 7 to go online with a PLC?', 3, GETDATE());

-- WinCC
DECLARE @WinCCId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@WinCCId, 'WinCC', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @WinCCId, 'Can you use WinCC to create a HMI screen?', 3, GETDATE()),
(NEWID(), @WinCCId, 'Can you use WinCC to troubleshoot a HMI screen?', 3, GETDATE()),
(NEWID(), @WinCCId, 'Can you use WinCC to download a HMI screen to a panel?', 3, GETDATE()),
(NEWID(), @WinCCId, 'Can you use WinCC to upload a HMI screen from a panel?', 3, GETDATE());

-- Robots
DECLARE @RobotsId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@RobotsId, 'Robots', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @RobotsId, 'Can you program a robot?', 3, GETDATE()),
(NEWID(), @RobotsId, 'Can you troubleshoot a robot?', 3, GETDATE()),
(NEWID(), @RobotsId, 'Can you teach a robot?', 3, GETDATE());

-- Vision Systems
DECLARE @VisionId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@VisionId, 'Vision Systems', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @VisionId, 'Can you program a vision system?', 3, GETDATE()),
(NEWID(), @VisionId, 'Can you troubleshoot a vision system?', 3, GETDATE()),
(NEWID(), @VisionId, 'Can you configure a vision system?', 3, GETDATE());

-- SCADA Systems
DECLARE @SCADAId UNIQUEIDENTIFIER = NEWID();
INSERT INTO core_skill_categories (id, name, created_at) VALUES (@SCADAId, 'SCADA Systems', GETDATE());

INSERT INTO core_skills (id, category_id, name, max_score, created_at) VALUES
(NEWID(), @SCADAId, 'Can you use SCADA to create a screen?', 3, GETDATE()),
(NEWID(), @SCADAId, 'Can you use SCADA to troubleshoot a screen?', 3, GETDATE()),
(NEWID(), @SCADAId, 'Can you use SCADA to download a screen to a panel?', 3, GETDATE()),
(NEWID(), @SCADAId, 'Can you use SCADA to upload a screen from a panel?', 3, GETDATE());

PRINT 'Core skills import completed successfully';
PRINT 'Total Categories: 24';
PRINT 'Total Skills: 114';
