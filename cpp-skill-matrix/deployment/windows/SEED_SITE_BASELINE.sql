/*
  Aptitude site baseline: production areas, machines and assessment questions.
  Source: approved site matrix export. This file contains no users, engineer scores,
  certificates, notifications or other personal/work-record data.

  Safe to rerun: it inserts only missing area/machine/question combinations.
  Legacy machine priorities (5-10) are mapped to the current 0-3 production-impact scale:
  5-6 => 1, 7-8 => 2, 9-10 => 3.
*/

SET NOCOUNT ON;
SET XACT_ABORT ON;

IF DB_NAME() IN (N'master', N'model', N'msdb', N'tempdb')
    THROW 51200, 'Connect to the target Aptitude database before running the site baseline.', 1;

IF OBJECT_ID(N'dbo.production_areas', N'U') IS NULL
   OR OBJECT_ID(N'dbo.machines', N'U') IS NULL
   OR OBJECT_ID(N'dbo.competencies', N'U') IS NULL
    THROW 51201, 'Aptitude production tables are missing. Run schema.sql and PRODUCTION_SCHEMA_MIGRATION.sql first.', 1;

BEGIN TRANSACTION;

DECLARE @Baseline TABLE (
    [production_area] NVARCHAR(200) NOT NULL,
    [machine] NVARCHAR(200) NOT NULL,
    [importance] TINYINT NOT NULL CHECK ([importance] BETWEEN 0 AND 3),
    [competency] NVARCHAR(200) NOT NULL,
    [max_score] TINYINT NOT NULL CHECK ([max_score] BETWEEN 0 AND 3),
    PRIMARY KEY ([production_area], [machine], [competency])
);

INSERT INTO @Baseline ([production_area], [machine], [importance], [competency], [max_score]) VALUES
    (N'Viaflo Fill', N'Filling Machine', 3, N'Proficient in carrying out PM''s and understanding the operation of the machines', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Able to perform manual movements', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Able to diagnose and carry out major repairs (leadscrews, fife units)', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Horizontal press remove/refit', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Horizontal press rebuild', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Vertical press remove/refit', 3),
    (N'Viaflo Fill', N'Filling Machine', 3, N'Proficient in setting up the machine when required (weld improvements)', 3),
    (N'Viaflo Fill', N'Vuormar', 2, N'Proficient in carrying out PM''s and understanding the operation of the machines', 3),
    (N'Viaflo Fill', N'Vuormar', 2, N'Be able to fault find to a high standard', 3),
    (N'Viaflo Fill', N'Vuormar', 2, N'Vacuum box rebuild (rack and pinion etc)', 3),
    (N'Viaflo Fill', N'Vuormar', 2, N'Proficient in setting up the machine when required (Vertical seal, horizontal seal and cut)', 3),
    (N'Viaflo Fill', N'Printer', 1, N'Proficient in carrying out PM''s and understanding the operation of the machines', 3),
    (N'Viaflo Fill', N'Printer', 1, N'Be able to fault find to a high standard and interpret the HMI', 3),
    (N'L7 Packing', N'Bliss 6', 2, N'Able to make adjustments to the set up of the machine eg. timing, encoder start/stop positions to produce a good box', 3),
    (N'L7 Packing', N'Bliss 6', 2, N'Able to adjust the glue pattern on the Nordsson Unit', 3),
    (N'L7 Packing', N'Unloader', 2, N'Navigate throughout menus to identify manual operations and adjust parameters on hmi', 3),
    (N'L7 Packing', N'Unloader', 2, N'Able to calibrate the safety controllers in the event of a collision (SC alarm)', 3),
    (N'L7 Packing', N'Unloader', 2, N'Able to manually move the robot using the pendant', 3),
    (N'L7 Packing', N'TMS', 3, N'Navigate throughout menus to identify manual operations on both loading, unloading, and shuttle systems', 3),
    (N'L7 Packing', N'TMS', 3, N'Proficient in manual movements on HMI and how to interact between conveyor systems', 3),
    (N'L7 Packing', N'TMS', 3, N'Able to perform reset sequences in the event of a fault or manual intervention', 3),
    (N'L7 Packing', N'TMS', 3, N'Identify faults and perform fault finding within the controls systems on the HMI', 3),
    (N'L7 Packing', N'Casepacker', 2, N'Able to make adjustments to pick and place positions of the bags', 3),
    (N'L7 Packing', N'Casepacker', 2, N'Able to size change between 1 litre and 3 litre programme if required', 3),
    (N'L7 Packing', N'Casepacker', 2, N'Able to adjust conveyor speeds if required', 3),
    (N'L7 Packing', N'Leaflet Inserter', 1, N'Able to set up leaflet inserter to run correctly if required', 3),
    (N'L7 Packing', N'Leaflet Inserter', 1, N'Able to strip and refurbish the main components of the leaflet inserter', 3),
    (N'L7 Packing', N'VideoJet labeller', 2, N'Able to move label/print position as required', 3),
    (N'L7 Packing', N'VideoJet labeller', 2, N'Able to change the IP address in the event of swapping a whole labeller or main PCB', 3),
    (N'L7 Packing', N'VideoJet labeller', 2, N'Able to change barcode length as required', 3),
    (N'L7 Packing', N'Palletiser 4', 3, N'Able to move the Fanuc robot', 3),
    (N'L7 Packing', N'Palletiser 4', 3, N'Able to move the Fanuc robot to different positions in the programme such as Home, Pick approach, Pre pick', 3),
    (N'L7 Packing', N'Palletiser 4', 3, N'Able to perform manual movements safely using the HMI or tablet', 3),
    (N'L7 Packing', N'Palletiser 4', 3, N'Able to update the truck tracker on the HMI', 3),
    (N'L7 Packing', N'Loader', 2, N'Able to calibrate the safety controllers in the event of a collision (SC alarm)', 3),
    (N'L7 Packing', N'Loader', 2, N'Navigate throughout menus to identify manual operations and adjust parameters on hmi', 3),
    (N'L7 Packing', N'Loader', 2, N'Able to manually move the robot using the pendant', 3),
    (N'L7 Packing', N'Flex Picker', 2, N'Able to adjust parameters to adjust the pick and placement positions', 3),
    (N'L7 Packing', N'Flex Picker', 2, N'Able to make adjustments to programme to remedy problems such as bags not being picked', 3),
    (N'LINE 1', N'Bliss 5/7', 2, N'Able to perform Bliss 5/7 PM''s', 3),
    (N'LINE 1', N'Bliss 5/7', 2, N'Able to make adjustments to the set up of the machine (compression chamber, timing, encoder start/stop positions etc.)', 3),
    (N'LINE 1', N'Bliss 5/7', 2, N'Able to adjust the glue pattern on the Nordsson Unit', 3),
    (N'LINE 1', N'Leaflet Inserter', 2, N'Able to perform PMs on the leaflet inserter', 3),
    (N'LINE 1', N'Leaflet Inserter', 2, N'Able to strip and refurbish the main components of the leaflet inserter', 3),
    (N'LINE 1', N'Leaflet Inserter', 2, N'Able to adjust cognex parameters during production to improve performance', 3),
    (N'LINE 1', N'Leaflet Inserter', 2, N'Able to locate and identify fault codes on the HMI', 3),
    (N'LINE 1', N'Serialisation Labeller', 2, N'Able to perform all PMs on the serialisation labeller', 3),
    (N'LINE 1', N'Box Labeller', 1, N'Able to perform all PMs on the box labeller', 3),
    (N'LINE 1', N'Little David', 1, N'Able to perform all PMs on the Little David', 3),
    (N'LINE 1', N'Little David', 1, N'Proficient in fault finding/troubleshooting, interpretation of HMI', 3),
    (N'LINE 1', N'Palletiser 4/Fanuc Robot', 3, N'Able to perform all PMs on the Palletiser 4 and Fanuc', 3),
    (N'LINE 1', N'Palletiser 4/Fanuc Robot', 3, N'Able to operate manually and safely, including pendant operations', 3),
    (N'LINE 1', N'Palletiser 4/Fanuc Robot', 3, N'Able to carry out a total reset of palletiser', 3),
    (N'LINE 1', N'Palletiser 4/Fanuc Robot', 3, N'Able to change positions within the programme, understanding each stage and their purpose (pre pick, pre cas, homing etc.)', 3),
    (N'LINE 6', N'Multivac', 3, N'Basic understanding of machine operation', 3),
    (N'LINE 6', N'Multivac', 3, N'HMI able to alter and adjust machine parameters such as temperatures/vacuum pressures', 3),
    (N'LINE 6', N'Multivac', 3, N'Able to perform all Multivac PMs when required', 3),
    (N'LINE 6', N'Emplex', 2, N'Able to refurbish emplex modules', 3),
    (N'LINE 6', N'Emplex', 2, N'Able to perform all Emplex PMs', 3),
    (N'LINE 6', N'Filling Station', 3, N'Understand the operation and components of the filling stations', 3),
    (N'LINE 6', N'Filling Station', 3, N'Able to replace the filling nozzle diaphragms/nozzles when required. Understand when is appropriate to do so', 3),
    (N'LINE 6', N'Multiup', 2, N'Understand machine operation', 3),
    (N'LINE 6', N'Multiup', 2, N'Trox cylinder principles. Understand', 3),
    (N'LINE 6', N'Multiup', 2, N'HMI understand and able to operate manually/ alter parameters', 3),
    (N'LINE 6', N'Single up', 1, N'Understand basic machine operation', 3),
    (N'LINE 6', N'Single up', 1, N'Able to perform all single up printer PMs', 3),
    (N'LINE 6', N'Huckleback Vac pumps', 2, N'Able to service the pump e.g. oil replenish/drain', 3),
    (N'LINE 6', N'Huckleback Vac pumps', 2, N'Understand operation of the pump', 3),
    (N'VIAFLO PACKING', N'Bliss 4/2', 2, N'Able to perform Bliss 4/2 PMs', 3),
    (N'VIAFLO PACKING', N'Bliss 4/2', 2, N'Able to make adjustments to the set up of the machine (compression chamber, timing etc.)', 3),
    (N'VIAFLO PACKING', N'Bliss 4/2', 2, N'Able to adjust the glue pattern on the Nordsson Unit', 3),
    (N'VIAFLO PACKING', N'Unloader', 3, N'Able to perform Unloader PMs', 3),
    (N'VIAFLO PACKING', N'Unloader', 3, N'Able to identify each step in the robot programme (e.g. pre vas, pre sacche etc.)', 3),
    (N'VIAFLO PACKING', N'Unloader', 3, N'Able to modify and adjust robot position to improve robot performance', 3),
    (N'VIAFLO PACKING', N'Unloader', 3, N'Carry out major repairs and set up of robot (Harness / motor replacement and subsequent calibration)', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to perform casepacker PMs', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to adjust programme parameters such as ICS speed, box position', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to adjust pick/place positions in the programme to improve casepacker performance', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to strip and refurbish the lower section of the casepacker including ICS/box chains and conveyors', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to strip and refurbish the upper section of the casepacker including the main upper gearboxes', 3),
    (N'VIAFLO PACKING', N'Casepacker', 3, N'Able to replace and home the main motors as per the HMI', 3),
    (N'VIAFLO PACKING', N'Leaflet Inserter', 2, N'Able to perform PMs on the leaflet inserters', 3),
    (N'VIAFLO PACKING', N'Leaflet Inserter', 2, N'Able to strip and refurbish the main components of the leaflet inserter', 3),
    (N'VIAFLO PACKING', N'Leaflet Inserter', 2, N'Able to adjust Cognex parameters during production to improve performance', 3),
    (N'VIAFLO PACKING', N'Taper', 1, N'Able to perform PMs on the little david and lantech taper', 3),
    (N'VIAFLO PACKING', N'VideoJet Labeller', 1, N'Able to perform PMs on the videoJet labeller', 3),
    (N'VIAFLO PACKING', N'VideoJet Labeller', 1, N'Able to strip and replace all main components of the labeller competently', 3),
    (N'VIAFLO PACKING', N'Palletiser 2', 2, N'Able to perform PMs on Palletiser 2', 3),
    (N'VIAFLO PACKING', N'Palletiser 2', 2, N'Able to carry out a total reset of the palletiser', 3),
    (N'VIAFLO PACKING', N'Palletiser 2', 2, N'Able to safely remove the layer table chains to carry out maintenance', 3),
    (N'Viaflo Racking', N'ABB', 3, N'Proficient in performing the loader PM''s', 3),
    (N'Viaflo Racking', N'ABB', 3, N'Able to identify each step in the robot programme (e.g. pre vas, pre sacche etc.), and manipulate the robot using the pendant', 3),
    (N'Viaflo Racking', N'ABB', 3, N'Able to modify and adjust robot position to improve robot positions', 3),
    (N'Viaflo Racking', N'ABB', 3, N'Carry out major repairs and set up of robot (Harness / motor replacement and subsequent calibration)', 3),
    (N'Viaflo Racking', N'Bag Turner', 2, N'Proficient in performing the bag turner PM''s', 3),
    (N'Viaflo Racking', N'Bag Turner', 2, N'Understand the role of each sensor and how to set up to run correctly', 3),
    (N'Viaflo Racking', N'Bag Turner', 2, N'Be able to make changes to the program on the HMI to optimise performance', 3),
    (N'Viaflo Racking', N'Shingle conveyor', 1, N'Understand knock on effects of shingle placement throughout the packing process, and be able to make adjustments to suit', 3),
    (N'Viaflo Racking', N'Shingle conveyor', 1, N'Replace belts and set tracking', 3),
    (N'Viaflo Racking', N'Vac pump', 1, N'Able to identify the location of the vac pump for each loader', 3);

;WITH [area_source] AS (
    SELECT [production_area] FROM @Baseline GROUP BY [production_area]
)
INSERT INTO [dbo].[production_areas] ([name], [created_at], [updated_at])
SELECT [production_area], GETDATE(), GETDATE()
FROM [area_source] AS source
WHERE NOT EXISTS (
    SELECT 1 FROM [dbo].[production_areas] AS existing
    WHERE UPPER(existing.[name]) = UPPER(source.[production_area])
);

;WITH [machine_source] AS (
    SELECT [production_area], [machine], MAX([importance]) AS [importance]
    FROM @Baseline
    GROUP BY [production_area], [machine]
)
INSERT INTO [dbo].[machines] ([production_area_id], [name], [importance], [created_at], [updated_at])
SELECT area.[id], source.[machine], source.[importance], GETDATE(), GETDATE()
FROM [machine_source] AS source
CROSS APPLY (
    SELECT TOP (1) [id]
    FROM [dbo].[production_areas]
    WHERE UPPER([name]) = UPPER(source.[production_area])
    ORDER BY [id]
) AS area
WHERE NOT EXISTS (
    SELECT 1 FROM [dbo].[machines] AS existing
    WHERE existing.[production_area_id] = area.[id]
      AND UPPER(existing.[name]) = UPPER(source.[machine])
);

INSERT INTO [dbo].[competencies] (
    [machine_id], [name], [max_score],
    [safety_impact], [production_impact], [frequency], [complexity], [future_value],
    [created_at], [updated_at]
)
SELECT machine.[id], source.[competency], source.[max_score],
       3.0, 3.0, 3.0, 3.0, 3.0, GETDATE(), GETDATE()
FROM @Baseline AS source
CROSS APPLY (
    SELECT TOP (1) [id]
    FROM [dbo].[production_areas]
    WHERE UPPER([name]) = UPPER(source.[production_area])
    ORDER BY [id]
) AS area
CROSS APPLY (
    SELECT TOP (1) [id]
    FROM [dbo].[machines]
    WHERE [production_area_id] = area.[id]
      AND UPPER([name]) = UPPER(source.[machine])
    ORDER BY [id]
) AS machine
WHERE NOT EXISTS (
    SELECT 1 FROM [dbo].[competencies] AS existing
    WHERE existing.[machine_id] = machine.[id]
      AND UPPER(existing.[name]) = UPPER(source.[competency])
);

COMMIT TRANSACTION;

SELECT
    COUNT(DISTINCT [production_area]) AS [baseline_production_areas],
    COUNT(DISTINCT CONCAT([production_area], N'|', [machine])) AS [baseline_machines],
    COUNT(*) AS [baseline_competencies]
FROM @Baseline;

PRINT 'Aptitude site baseline applied successfully.';
GO
