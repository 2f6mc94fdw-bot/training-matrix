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
