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
