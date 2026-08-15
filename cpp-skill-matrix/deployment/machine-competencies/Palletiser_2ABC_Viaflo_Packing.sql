/*
  Aptitude machine competency deployment
  Production area: Viaflo Packing
  Machine: Palletiser 2ABC
  Source: Tekpak T-PAL-25x2 Operator & Maintenance Manual, document 186597-01_OM V1.0

  This script is idempotent. It creates the machine if required and only inserts
  competencies that do not already exist with the same name.
*/

USE [training_matrix];
GO

SET XACT_ABORT ON;
BEGIN TRANSACTION;

DECLARE @AreaId INT;
DECLARE @MachineId INT;

SELECT @AreaId = [id]
FROM [dbo].[production_areas]
WHERE [name] = N'Viaflo Packing';

IF @AreaId IS NULL
BEGIN
    THROW 50001, 'Production area Viaflo Packing does not exist.', 1;
END;

SELECT @MachineId = [id]
FROM [dbo].[machines]
WHERE [production_area_id] = @AreaId
  AND [name] = N'Palletiser 2ABC';

IF @MachineId IS NULL
BEGIN
    INSERT INTO [dbo].[machines] ([production_area_id], [name], [importance], [created_at], [updated_at])
    VALUES (@AreaId, N'Palletiser 2ABC', 1, GETDATE(), GETDATE());

    SET @MachineId = SCOPE_IDENTITY();
END;

DECLARE @Competencies TABLE ([name] NVARCHAR(200) NOT NULL);

INSERT INTO @Competencies ([name]) VALUES
    (N'Able to safely isolate electrical and pneumatic energy, apply LOTO and verify stored energy is discharged before maintenance'),
    (N'Able to identify the machine safety devices, request guarded access and correctly restore the safety circuit after intervention'),
    (N'Able to complete pre-start checks and perform controlled automatic start, stop and restart procedures'),
    (N'Able to navigate the HMI, interpret module status, beacon indications, active alarms and alarm history'),
    (N'Able to select and verify the correct product recipe and understand which settings require authorised access'),
    (N'Able to use authorised manual mode to safely operate conveyors, pallet dispenser, shuttle and pick-and-place modules'),
    (N'Able to diagnose transfer-area faults using line communication, inverter, Interroll and I/O status screens'),
    (N'Able to diagnose robot status and active robot faults and carry out the approved recovery or escalation procedure'),
    (N'Able to safely clear product or pallet jams, complete line clearance and return the machine to a safe home condition'),
    (N'Able to fault-find the pallet infeed and de-stacker including pallet presence, lift, release gate and low-stack sensing'),
    (N'Able to fault-find the layer-card dispenser and Polycord conveyor including card presence and placement sensing'),
    (N'Able to fault-find pick-and-place or dropped-product issues including air pressure, vacuum components, suction cups and sensors'),
    (N'Able to fault-find the stretch wrapper and labeller including film handling, cutting wire, pallet transfer and reset conditions'),
    (N'Able to complete and record the specified daily and monthly preventive maintenance inspections'),
    (N'Able to inspect conveyor belts, fixings, sensor mountings and motors for wear, damage, heat, noise or lubricant leakage'),
    (N'Able to inspect, lubricate, tension and align roller chains and sprockets and assess chain elongation against the stated limit'),
    (N'Able to inspect and maintain bearings, rotation units and gearboxes using the specified lubricant and maintenance intervals'),
    (N'Able to replace and correctly tension a conveyor belt while preserving conveyor alignment'),
    (N'Able to correctly align photocells and set proximity and magnetic sensors to the specified positions'),
    (N'Able to adjust wrapper carriage and conveyor drive-chain tension and verify safe operation after adjustment');

INSERT INTO [dbo].[competencies] ([machine_id], [name], [max_score], [created_at], [updated_at])
SELECT @MachineId, proposed.[name], 3, GETDATE(), GETDATE()
FROM @Competencies AS proposed
WHERE NOT EXISTS (
    SELECT 1
    FROM [dbo].[competencies] AS existing
    WHERE existing.[machine_id] = @MachineId
      AND existing.[name] = proposed.[name]
);

COMMIT TRANSACTION;

SELECT
    area.[name] AS [production_area],
    machine.[name] AS [machine],
    competency.[id] AS [competency_id],
    competency.[name] AS [competency],
    competency.[max_score]
FROM [dbo].[production_areas] AS area
JOIN [dbo].[machines] AS machine ON machine.[production_area_id] = area.[id]
JOIN [dbo].[competencies] AS competency ON competency.[machine_id] = machine.[id]
WHERE area.[name] = N'Viaflo Packing'
  AND machine.[name] = N'Palletiser 2ABC'
ORDER BY competency.[id];
GO
