-- Line 7 Filling - Production Area
-- Insert production area, machines, and competencies based on actual KLPs

-- Insert Production Area
INSERT INTO production_areas (name) VALUES ('Line 7 Filling');
GO

-- Get the ID of the newly created production area
DECLARE @AreaId INT = (SELECT id FROM production_areas WHERE name = 'Line 7 Filling');

-- 1. Vision System
DECLARE @VisionSystemId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Vision System', 3);
SET @VisionSystemId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@VisionSystemId, '1.1.1 Access vision viewer on HMI and adjust parameters for plugging', 3),
    (@VisionSystemId, '1.1.2 Access vision system on laptop through remote access', 3),
    (@VisionSystemId, '1.1.3 Adjust parameters for each camera and set-up new product codes', 3),
    (@VisionSystemId, '1.1.4 Proficient in fault finding to identify issues and reduce scrap rates', 3);

-- 2. Robot A
DECLARE @RobotAId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Robot A', 2);
SET @RobotAId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@RobotAId, '1.2.1 Replace and set-up vacuum valves and adjust limits on controller', 3),
    (@RobotAId, '1.2.2 Manually operate robot and update/adjust parameters', 3),
    (@RobotAId, '1.2.3 Set calibration points and identify mechanical component failures', 3);

-- 3. Robot B
DECLARE @RobotBId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Robot B', 2);
SET @RobotBId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@RobotBId, '1.3.1 Manually operate robot and update/adjust parameters', 3),
    (@RobotBId, '1.3.2 Set calibration points and identify mechanical component failures', 3);

-- 4. Robot C
DECLARE @RobotCId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Robot C', 2);
SET @RobotCId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@RobotCId, '1.4.1 Manually operate robot and update/adjust parameters', 3),
    (@RobotCId, '1.4.2 Set calibration points and identify mechanical component failures', 3);

-- 5. Robot D
DECLARE @RobotDId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Robot D', 2);
SET @RobotDId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@RobotDId, '1.5.1 Manually operate robot and update/adjust parameters', 3),
    (@RobotDId, '1.5.2 Set calibration points and identify mechanical component failures', 3);

-- 6. Robot F
DECLARE @RobotFId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Robot F', 2);
SET @RobotFId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@RobotFId, '1.6.1 Manually operate robot and update/adjust parameters', 3),
    (@RobotFId, '1.6.2 Identify and replace a faulty wiring harness', 3),
    (@RobotFId, '1.6.3 Set calibration points and identify mechanical component failures', 3);

-- 7. Check Weigh
DECLARE @CheckWeighId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Check Weigh', 2);
SET @CheckWeighId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@CheckWeighId, '1.7.1 Replace and set up checkweigh components (scales, motor, belts)', 3),
    (@CheckWeighId, '1.7.2 Adjust and calibrate conveyor speed on HMI', 3);

-- 8. HDPE
DECLARE @HDPEId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'HDPE', 3);
SET @HDPEId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@HDPEId, '1.8.1 Operate all manual operations safely', 3),
    (@HDPEId, '1.8.2 Complete all HDPE PMS', 3),
    (@HDPEId, '1.8.3 Perform annual strip down of the presses', 3),
    (@HDPEId, '1.8.4 Replace sealing bar including isolation of cooling water', 3);

-- 9. LM2
DECLARE @LM2Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'LM2', 2);
SET @LM2Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@LM2Id, '1.9.1 Replace LM2 belts and tension correctly', 3),
    (@LM2Id, '1.9.2 Understand LM2 fibre optic bag position check', 3),
    (@LM2Id, '1.9.3 Manually operate and home clamp bars/belt', 3);

-- 10. Pocket Conveyor
DECLARE @PocketConveyorId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Pocket Conveyor', 2);
SET @PocketConveyorId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@PocketConveyorId, '1.10.1 Replace pocket conveyor belts and tension correctly', 3),
    (@PocketConveyorId, '1.10.2 Manually operate and home belts correctly', 3);

-- 11. Cadencing Conveyor
DECLARE @CadencingConveyorId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Cadencing Conveyor', 1);
SET @CadencingConveyorId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@CadencingConveyorId, '1.11.1 Replace cadencing belts and tension correctly', 3);

-- 12. Printers
DECLARE @PrintersId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Printers', 2);
SET @PrintersId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@PrintersId, '1.12.1 Replace main printer cylinders', 3),
    (@PrintersId, '1.12.2 Manually operate and adjust parameters to ensure print force within limits', 3),
    (@PrintersId, '1.12.3 Home printers and size change motor from 3L to 1L', 3);

-- 13. General HMI
DECLARE @GeneralHMIId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'General HMI', 2);
SET @GeneralHMIId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@GeneralHMIId, '1.13.1 Navigate all filling room HMIs and understand manual/auto/sanitisation functions', 3),
    (@GeneralHMIId, '1.13.2 Understand key HMI functions and navigate between different functions', 3);

GO

PRINT 'Line 7 Filling production area created successfully!';
PRINT 'Machines: 13 | Total Competencies: 33';
GO

-- Verify the insert
SELECT
    pa.name AS ProductionArea,
    m.name AS Machine,
    COUNT(c.id) AS CompetencyCount
FROM production_areas pa
JOIN machines m ON m.production_area_id = pa.id
LEFT JOIN competencies c ON c.machine_id = m.id
WHERE pa.name = 'Line 7 Filling'
GROUP BY pa.name, m.name
ORDER BY m.name;
GO
-- Line 7 Packing - Production Area
-- Insert production area, machines, and competencies based on actual KLPs

-- Insert Production Area
INSERT INTO production_areas (name) VALUES ('Line 7 Packing');
GO

-- Get the ID of the newly created production area
DECLARE @AreaId INT = (SELECT id FROM production_areas WHERE name = 'Line 7 Packing');

-- 1. Bliss 6
DECLARE @Bliss6Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Bliss 6', 2);
SET @Bliss6Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Bliss6Id, '1.1.1 Make adjustments to machine setup (timing, encoder positions) to produce good box', 3),
    (@Bliss6Id, '1.1.2 Adjust glue pattern on Nordsson Unit', 3);

-- 2. Unloader
DECLARE @UnloaderId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Unloader', 2);
SET @UnloaderId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@UnloaderId, '1.2.1 Navigate menus to identify manual operations and adjust HMI parameters', 3),
    (@UnloaderId, '1.2.2 Calibrate safety controllers in event of collision (SC alarm)', 3),
    (@UnloaderId, '1.2.3 Manually move robot using pendant', 3);

-- 3. TMS
DECLARE @TMSId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'TMS', 3);
SET @TMSId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@TMSId, '1.3.1 Navigate menus for manual operations on loading, unloading, and shuttle systems', 3),
    (@TMSId, '1.3.2 Proficient in manual movements on HMI and interact between conveyor systems', 3),
    (@TMSId, '1.3.3 Perform reset sequences in event of fault or manual intervention', 3),
    (@TMSId, '1.3.4 Identify faults and perform fault finding within controls systems on HMI', 3);

-- 4. Casepacker
DECLARE @CasepackerId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Casepacker', 2);
SET @CasepackerId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@CasepackerId, '1.4.1 Make adjustments to pick and place positions of bags', 3),
    (@CasepackerId, '1.4.2 Size change between 1 litre and 3 litre programme', 3),
    (@CasepackerId, '1.4.3 Adjust conveyor speeds as required', 3);

-- 5. Leaflet Inserter
DECLARE @LeafletInserterId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Leaflet Inserter', 2);
SET @LeafletInserterId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@LeafletInserterId, '1.5.1 Set up leaflet inserter to run correctly', 3),
    (@LeafletInserterId, '1.5.2 Strip and refurbish main components of leaflet inserter', 3);

-- 6. VideoJet labeller
DECLARE @VideoJetId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'VideoJet labeller', 2);
SET @VideoJetId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@VideoJetId, '1.6.1 Move label/print position as required', 3),
    (@VideoJetId, '1.6.2 Change IP address when swapping labeller or main PCB', 3),
    (@VideoJetId, '1.6.3 Change barcode length as required', 3);

-- 7. Palletiser 4
DECLARE @Palletiser4Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Palletiser 4', 3);
SET @Palletiser4Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Palletiser4Id, '1.7.1 Move the Fanuc robot', 3),
    (@Palletiser4Id, '1.7.2 Move Fanuc robot to different positions (Home, Pick approach, Pre pick)', 3),
    (@Palletiser4Id, '1.7.3 Perform manual movements safely using HMI or tablet', 3),
    (@Palletiser4Id, '1.7.4 Update truck tracker on HMI', 3);

-- 8. Loader
DECLARE @LoaderId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Loader', 2);
SET @LoaderId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@LoaderId, '1.8.1 Calibrate safety controllers in event of collision (SC alarm)', 3),
    (@LoaderId, '1.8.2 Navigate menus to identify manual operations and adjust HMI parameters', 3),
    (@LoaderId, '1.8.3 Manually move robot using pendant', 3);

-- 9. Flex Picker
DECLARE @FlexPickerId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Flex Picker', 2);
SET @FlexPickerId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@FlexPickerId, '1.9.1 Adjust parameters to adjust pick and placement positions', 3),
    (@FlexPickerId, '1.9.2 Make adjustments to programme to remedy problems (bags not being picked)', 3);

GO

PRINT 'Line 7 Packing production area created successfully!';
PRINT 'Machines: 9 | Total Competencies: 26';
GO

-- Verify the insert
SELECT
    pa.name AS ProductionArea,
    m.name AS Machine,
    COUNT(c.id) AS CompetencyCount
FROM production_areas pa
JOIN machines m ON m.production_area_id = pa.id
LEFT JOIN competencies c ON c.machine_id = m.id
WHERE pa.name = 'Line 7 Packing'
GROUP BY pa.name, m.name
ORDER BY m.name;
GO
-- Line 7 Tank Room - Production Area
-- Insert production area, machines, and competencies based on actual KLPs

-- Insert Production Area
INSERT INTO production_areas (name) VALUES ('Line 7 Tank Room');
GO

-- Get the ID of the newly created production area
DECLARE @AreaId INT = (SELECT id FROM production_areas WHERE name = 'Line 7 Tank Room');

-- 1. ONG
DECLARE @ONGId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'ONG', 3);
SET @ONGId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@ONGId, '1.1.1 Understand phases in each section and how they interact with filling machine', 3),
    (@ONGId, '1.1.2 Able to login on tank room PC and access via remote desktop', 3),
    (@ONGId, '1.1.3 Able to reset ONG server (understand risks - contacting Belgium, site impact)', 3),
    (@ONGId, '1.1.4 Able to follow a P&ID drawing', 3);

-- 2. General Knowledge
DECLARE @GeneralKnowledgeId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'General Knowledge', 3);
SET @GeneralKnowledgeId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@GeneralKnowledgeId, '2.1.1 Understand functions of each tank (70 holding, 71/72 solution tanks)', 3),
    (@GeneralKnowledgeId, '2.1.2 Understand POU loop and its function', 3),
    (@GeneralKnowledgeId, '2.1.3 Understand WFI tab and impact on supply lines', 3),
    (@GeneralKnowledgeId, '2.1.4 Understand heat exchanger principles and perform blow down/inspect faults', 3),
    (@GeneralKnowledgeId, '2.1.5 Understand operations from tank 70 WFI to mixing, recirc loops, sanitization', 3),
    (@GeneralKnowledgeId, '2.1.6 Manually operate valves for diaphragm replacement (understand steam/WFI risks)', 3),
    (@GeneralKnowledgeId, '2.1.7 Understand the user loop system', 3),
    (@GeneralKnowledgeId, '2.1.8 Understand impacts of replacing valves, solenoids, pumps, seals (going black)', 3),
    (@GeneralKnowledgeId, '2.1.9 Understand and identify steam lines and dangers involved', 3),
    (@GeneralKnowledgeId, '2.1.10 Understand comms between phases (overcome ONG phase vs filling issues)', 3),
    (@GeneralKnowledgeId, '2.1.11 Proficient at calibrating Gemu valves positioners', 3),
    (@GeneralKnowledgeId, '2.1.12 Understand vent valves normally open on tank 71 and 72', 3);

-- 3. Tank 70 Phases
DECLARE @Tank70Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Tank 70 Phases', 3);
SET @Tank70Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Tank70Id, '3.1.1 Proficient at manually sanitising tank 70 and user loop system', 3),
    (@Tank70Id, '3.1.2 Understand recirc loop function (maintain temp >81° top and bottom)', 3),
    (@Tank70Id, '3.1.3 Understand user loop function (WFI distribution supply) for tank 70', 3),
    (@Tank70Id, '3.1.4 Proficient at operating conductivity unit and opening sample point', 3),
    (@Tank70Id, '3.1.5 Understand the WFI Vessel phase', 3);

-- 4. Tank 71
DECLARE @Tank71Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Tank 71', 3);
SET @Tank71Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Tank71Id, '4.1.1 Understand each operational phase (Rinse, sanitisation, filling)', 3),
    (@Tank71Id, '4.1.2 Understand differences between sanitisation phases and fault finding', 3),
    (@Tank71Id, '4.1.3 Able to identify fault during phase using interlock screen', 3),
    (@Tank71Id, '4.1.4 Proficient at replacing rupture disk without damage (align magnet)', 3);

-- 5. Tank 72
DECLARE @Tank72Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Tank 72', 3);
SET @Tank72Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Tank72Id, '5.1.1 Understand each operational phase (Rinse, sanitisation, filling)', 3),
    (@Tank72Id, '5.1.2 Understand differences between sanitisation phases and fault finding', 3),
    (@Tank72Id, '5.1.3 Able to identify fault during phase using interlock screen', 3),
    (@Tank72Id, '5.1.4 Knowledge of topping up heater jacket and performing manual operations', 3);

-- 6. POU Cooler
DECLARE @POUCoolerId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'POU Cooler', 2);
SET @POUCoolerId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@POUCoolerId, '6.1.1 Understand process air function for micro testing blow down', 3),
    (@POUCoolerId, '6.1.2 Understand implications of overpressurising process air >1 bar (rupture risk)', 3),
    (@POUCoolerId, '6.1.3 Proficient at manually controlling valves and temperature (heating/cooling)', 3);

-- 7. Transfer Skid/Wash Trough
DECLARE @TransferSkidId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Transfer Skid/Wash Trough', 2);
SET @TransferSkidId = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@TransferSkidId, 'Understand different phases (Sanitisation, WFI flush and drain)', 3),
    (@TransferSkidId, 'Able to manually perform functions such as drain when required', 3);

GO

PRINT 'Line 7 Tank Room production area created successfully!';
PRINT 'Machines: 7 | Total Competencies: 34';
GO

-- Verify the insert
SELECT
    pa.name AS ProductionArea,
    m.name AS Machine,
    COUNT(c.id) AS CompetencyCount
FROM production_areas pa
JOIN machines m ON m.production_area_id = pa.id
LEFT JOIN competencies c ON c.machine_id = m.id
WHERE pa.name = 'Line 7 Tank Room'
GROUP BY pa.name, m.name
ORDER BY m.name;
GO
