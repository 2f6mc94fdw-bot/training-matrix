-- Core Skills Import Script
-- Uses auto-incrementing INT IDs with SCOPE_IDENTITY()

-- MECHANICAL SKILLS

-- Welding
INSERT INTO core_skill_categories (name, created_at) VALUES ('Welding', GETDATE());
DECLARE @WeldingId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@WeldingId, 'Can you stick weld?', 3, GETDATE()),
(@WeldingId, 'Can you TIG weld?', 3, GETDATE()),
(@WeldingId, 'Can you MIG weld?', 3, GETDATE()),
(@WeldingId, 'Can you Braze?', 3, GETDATE());

-- Lathe
INSERT INTO core_skill_categories (name, created_at) VALUES ('Lathe', GETDATE());
DECLARE @LatheId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@LatheId, 'Can you use a lathe to turn metal?', 3, GETDATE()),
(@LatheId, 'Can you use a lathe to thread metal?', 3, GETDATE()),
(@LatheId, 'Can you use a lathe to drill metal?', 3, GETDATE());

-- Mill
INSERT INTO core_skill_categories (name, created_at) VALUES ('Mill', GETDATE());
DECLARE @MillId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@MillId, 'Can you use a mill to cut metal?', 3, GETDATE()),
(@MillId, 'Can you use a mill to drill metal?', 3, GETDATE());

-- Gearbox
INSERT INTO core_skill_categories (name, created_at) VALUES ('Gearbox', GETDATE());
DECLARE @GearboxId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@GearboxId, 'Can you dismantle a gearbox to inspect the bearings, shaft, and gears?', 3, GETDATE()),
(@GearboxId, 'Can you re-assemble a gearbox?', 3, GETDATE()),
(@GearboxId, 'Can you align a gearbox to a motor using a dial gauge?', 3, GETDATE()),
(@GearboxId, 'Can you change gearbox oil?', 3, GETDATE()),
(@GearboxId, 'Can you replace gearbox seals?', 3, GETDATE());

-- Pumps
INSERT INTO core_skill_categories (name, created_at) VALUES ('Pumps', GETDATE());
DECLARE @PumpsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@PumpsId, 'Can you dismantle a pump to inspect the bearings, shaft, and impeller?', 3, GETDATE()),
(@PumpsId, 'Can you re-assemble a pump?', 3, GETDATE()),
(@PumpsId, 'Can you align a pump to a motor using a dial gauge?', 3, GETDATE()),
(@PumpsId, 'Can you replace pump seals?', 3, GETDATE());

-- Pneumatics
INSERT INTO core_skill_categories (name, created_at) VALUES ('Pneumatics', GETDATE());
DECLARE @PneumaticsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@PneumaticsId, 'Can you troubleshoot a pneumatic circuit?', 3, GETDATE()),
(@PneumaticsId, 'Can you replace a pneumatic valve?', 3, GETDATE()),
(@PneumaticsId, 'Can you replace a pneumatic cylinder?', 3, GETDATE()),
(@PneumaticsId, 'Can you replace a pneumatic air line?', 3, GETDATE());

-- Hydraulics
INSERT INTO core_skill_categories (name, created_at) VALUES ('Hydraulics', GETDATE());
DECLARE @HydraulicsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@HydraulicsId, 'Can you troubleshoot a hydraulic circuit?', 3, GETDATE()),
(@HydraulicsId, 'Can you replace a hydraulic valve?', 3, GETDATE()),
(@HydraulicsId, 'Can you replace a hydraulic cylinder?', 3, GETDATE()),
(@HydraulicsId, 'Can you replace a hydraulic hose?', 3, GETDATE()),
(@HydraulicsId, 'Can you change hydraulic oil?', 3, GETDATE());

-- ELECTRICAL SKILLS

-- Electrical Knowledge
INSERT INTO core_skill_categories (name, created_at) VALUES ('Electrical Knowledge', GETDATE());
DECLARE @ElecKnowId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@ElecKnowId, 'Do you understand Ohm''s Law?', 3, GETDATE()),
(@ElecKnowId, 'Can you read electrical drawings?', 3, GETDATE()),
(@ElecKnowId, 'Can you troubleshoot an electrical circuit?', 3, GETDATE());

-- Electrical Hand Skills
INSERT INTO core_skill_categories (name, created_at) VALUES ('Electrical Hand Skills', GETDATE());
DECLARE @ElecHandId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@ElecHandId, 'Can you use a multi-meter to measure voltage?', 3, GETDATE()),
(@ElecHandId, 'Can you use a multi-meter to measure current?', 3, GETDATE()),
(@ElecHandId, 'Can you use a multi-meter to measure resistance?', 3, GETDATE()),
(@ElecHandId, 'Can you crimp a terminal?', 3, GETDATE()),
(@ElecHandId, 'Can you solder a wire?', 3, GETDATE());

-- Motors
INSERT INTO core_skill_categories (name, created_at) VALUES ('Motors', GETDATE());
DECLARE @MotorsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@MotorsId, 'Can you replace a motor?', 3, GETDATE()),
(@MotorsId, 'Can you wire a motor?', 3, GETDATE()),
(@MotorsId, 'Can you troubleshoot a motor?', 3, GETDATE());

-- Inverters
INSERT INTO core_skill_categories (name, created_at) VALUES ('Inverters', GETDATE());
DECLARE @InvertersId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@InvertersId, 'Can you replace an inverter?', 3, GETDATE()),
(@InvertersId, 'Can you wire an inverter?', 3, GETDATE()),
(@InvertersId, 'Can you troubleshoot an inverter?', 3, GETDATE()),
(@InvertersId, 'Can you configure an inverter?', 3, GETDATE());

-- Control Panels
INSERT INTO core_skill_categories (name, created_at) VALUES ('Control Panels', GETDATE());
DECLARE @ControlPanelsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@ControlPanelsId, 'Can you troubleshoot a control panel?', 3, GETDATE()),
(@ControlPanelsId, 'Can you replace a relay?', 3, GETDATE()),
(@ControlPanelsId, 'Can you replace a contactor?', 3, GETDATE()),
(@ControlPanelsId, 'Can you replace a circuit breaker?', 3, GETDATE()),
(@ControlPanelsId, 'Can you replace a fuse?', 3, GETDATE());

-- SOFTWARE SKILLS

-- RSLinx
INSERT INTO core_skill_categories (name, created_at) VALUES ('RSLinx', GETDATE());
DECLARE @RSLinxId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@RSLinxId, 'Can you use RSLinx to connect to a PLC?', 3, GETDATE()),
(@RSLinxId, 'Can you use RSLinx to troubleshoot a PLC?', 3, GETDATE());

-- RSLogix 500
INSERT INTO core_skill_categories (name, created_at) VALUES ('RSLogix 500', GETDATE());
DECLARE @RSLogix500Id INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@RSLogix500Id, 'Can you use RSLogix 500 to program a PLC?', 3, GETDATE()),
(@RSLogix500Id, 'Can you use RSLogix 500 to troubleshoot a PLC?', 3, GETDATE()),
(@RSLogix500Id, 'Can you use RSLogix 500 to download a program to a PLC?', 3, GETDATE()),
(@RSLogix500Id, 'Can you use RSLogix 500 to upload a program from a PLC?', 3, GETDATE()),
(@RSLogix500Id, 'Can you use RSLogix 500 to go online with a PLC?', 3, GETDATE());

-- RSLogix 5000
INSERT INTO core_skill_categories (name, created_at) VALUES ('RSLogix 5000', GETDATE());
DECLARE @RSLogix5000Id INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@RSLogix5000Id, 'Can you use RSLogix 5000 to program a PLC?', 3, GETDATE()),
(@RSLogix5000Id, 'Can you use RSLogix 5000 to troubleshoot a PLC?', 3, GETDATE()),
(@RSLogix5000Id, 'Can you use RSLogix 5000 to download a program to a PLC?', 3, GETDATE()),
(@RSLogix5000Id, 'Can you use RSLogix 5000 to upload a program from a PLC?', 3, GETDATE()),
(@RSLogix5000Id, 'Can you use RSLogix 5000 to go online with a PLC?', 3, GETDATE());

-- Studio 5000
INSERT INTO core_skill_categories (name, created_at) VALUES ('Studio 5000', GETDATE());
DECLARE @Studio5000Id INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@Studio5000Id, 'Can you use Studio 5000 to program a PLC?', 3, GETDATE()),
(@Studio5000Id, 'Can you use Studio 5000 to troubleshoot a PLC?', 3, GETDATE()),
(@Studio5000Id, 'Can you use Studio 5000 to download a program to a PLC?', 3, GETDATE()),
(@Studio5000Id, 'Can you use Studio 5000 to upload a program from a PLC?', 3, GETDATE()),
(@Studio5000Id, 'Can you use Studio 5000 to go online with a PLC?', 3, GETDATE());

-- FactoryTalk View
INSERT INTO core_skill_categories (name, created_at) VALUES ('FactoryTalk View', GETDATE());
DECLARE @FactoryTalkId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@FactoryTalkId, 'Can you use FactoryTalk View to create a HMI screen?', 3, GETDATE()),
(@FactoryTalkId, 'Can you use FactoryTalk View to troubleshoot a HMI screen?', 3, GETDATE()),
(@FactoryTalkId, 'Can you use FactoryTalk View to download a HMI screen to a panel?', 3, GETDATE()),
(@FactoryTalkId, 'Can you use FactoryTalk View to upload a HMI screen from a panel?', 3, GETDATE());

-- TIA Portal
INSERT INTO core_skill_categories (name, created_at) VALUES ('TIA Portal', GETDATE());
DECLARE @TIAPortalId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@TIAPortalId, 'Can you use TIA Portal to program a PLC?', 3, GETDATE()),
(@TIAPortalId, 'Can you use TIA Portal to troubleshoot a PLC?', 3, GETDATE()),
(@TIAPortalId, 'Can you use TIA Portal to download a program to a PLC?', 3, GETDATE()),
(@TIAPortalId, 'Can you use TIA Portal to upload a program from a PLC?', 3, GETDATE()),
(@TIAPortalId, 'Can you use TIA Portal to go online with a PLC?', 3, GETDATE());

-- Step 7
INSERT INTO core_skill_categories (name, created_at) VALUES ('Step 7', GETDATE());
DECLARE @Step7Id INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@Step7Id, 'Can you use Step 7 to program a PLC?', 3, GETDATE()),
(@Step7Id, 'Can you use Step 7 to troubleshoot a PLC?', 3, GETDATE()),
(@Step7Id, 'Can you use Step 7 to download a program to a PLC?', 3, GETDATE()),
(@Step7Id, 'Can you use Step 7 to upload a program from a PLC?', 3, GETDATE()),
(@Step7Id, 'Can you use Step 7 to go online with a PLC?', 3, GETDATE());

-- WinCC
INSERT INTO core_skill_categories (name, created_at) VALUES ('WinCC', GETDATE());
DECLARE @WinCCId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@WinCCId, 'Can you use WinCC to create a HMI screen?', 3, GETDATE()),
(@WinCCId, 'Can you use WinCC to troubleshoot a HMI screen?', 3, GETDATE()),
(@WinCCId, 'Can you use WinCC to download a HMI screen to a panel?', 3, GETDATE()),
(@WinCCId, 'Can you use WinCC to upload a HMI screen from a panel?', 3, GETDATE());

-- Robots
INSERT INTO core_skill_categories (name, created_at) VALUES ('Robots', GETDATE());
DECLARE @RobotsId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@RobotsId, 'Can you program a robot?', 3, GETDATE()),
(@RobotsId, 'Can you troubleshoot a robot?', 3, GETDATE()),
(@RobotsId, 'Can you teach a robot?', 3, GETDATE());

-- Vision Systems
INSERT INTO core_skill_categories (name, created_at) VALUES ('Vision Systems', GETDATE());
DECLARE @VisionId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@VisionId, 'Can you program a vision system?', 3, GETDATE()),
(@VisionId, 'Can you troubleshoot a vision system?', 3, GETDATE()),
(@VisionId, 'Can you configure a vision system?', 3, GETDATE());

-- SCADA Systems
INSERT INTO core_skill_categories (name, created_at) VALUES ('SCADA Systems', GETDATE());
DECLARE @SCADAId INT = SCOPE_IDENTITY();

INSERT INTO core_skills (category_id, name, max_score, created_at) VALUES
(@SCADAId, 'Can you use SCADA to create a screen?', 3, GETDATE()),
(@SCADAId, 'Can you use SCADA to troubleshoot a screen?', 3, GETDATE()),
(@SCADAId, 'Can you use SCADA to download a screen to a panel?', 3, GETDATE()),
(@SCADAId, 'Can you use SCADA to upload a screen from a panel?', 3, GETDATE());

PRINT 'Core skills import completed successfully';
PRINT 'Total Categories: 24';
PRINT 'Total Skills: 114';
