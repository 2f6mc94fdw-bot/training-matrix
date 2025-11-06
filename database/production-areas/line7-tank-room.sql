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
