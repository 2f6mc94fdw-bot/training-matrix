# Aptitude Windows Pilot Checklist

Record pass/fail, tester, date, workstation, duration, and evidence for every item. Any **No-Go** item must be resolved or explicitly accepted by the business and IT owners.

## Release Evidence

- Both jobs in `Aptitude Windows Release Validation` pass for the release commit.
- `BUILD_INFO.txt` identifies the approved commit and says `SourceDirty=False`.
- Package files match `SHA256SUMS.txt` after transfer to the work network.
- `VALIDATE_DATABASE.sql` reports schema version `2026072701` and passes.
- IT confirms the `aptitude_app` login is not `sysadmin`, `db_owner`, `db_ddladmin`, or `db_securityadmin`.
- A full database backup and a documented rollback owner exist before testing.

## Installation And Connection

- Aptitude starts on a clean Windows 10/11 x64 workstation.
- Windows reports no missing DLL, runtime, Qt plugin, or ODBC driver.
- Test Connection succeeds through the approved network route.
- Invalid SQL credentials produce a clear error without freezing the app.
- Connecting to an incomplete test database produces the schema migration message.
- Admin login succeeds and the default Aptitude password is changed.
- **No-Go:** SQL credentials or passwords appear in logs, screenshots, or exported files.

## Manager Workflow

- Dashboard first load completes within 3 seconds on the work network.
- Returning to an already-loaded main page completes within 1.5 seconds.
- Dashboard filters and clickable charts update the displayed detail correctly.
- Production and core-skill summaries reconcile to a manual sample calculation.
- Engineer raw/weighted score toggle updates correctly.
- Production assessment search finds an engineer by full and partial name.
- A manager's direct production score edit persists after restart.
- Core skills are grouped as Mechanical, Electrical, and Software.
- A manager's direct core-skill score edit persists after restart.
- Manager Review loads within 2 seconds.
- Approve and reject each complete within 1.5 seconds.
- A pending row shows the approved score beside the engineer's proposed score.
- Approval updates the official score, the decision, the audit record, and the engineer notification together.
- Rejection requires a reason and leaves the official score unchanged.
- A stale approval attempt is blocked with a refresh/conflict message.
- Engineer/core-skill updates create the expected manager notifications.
- Reports and exports open successfully in approved office software.
- **No-Go:** rejecting a submitted score leaves it represented as manager-approved data.
- **No-Go:** the Audit Log cannot identify who changed a tested score, what changed, and when.

## Engineer Workflow

- Engineer login sees only the intended engineer functions and their own records.
- An engineer cannot access manager pages through navigation or keyboard actions.
- Score buttons show 0, 1, 2, and 3 with the agreed colour, hover, and selected states.
- Submitting a production assessment creates a pending manager review and notification without changing the official score.
- Submitting a core-skill assessment creates a pending manager review and notification without changing the official score.
- My Progress and My Certifications show only the signed-in engineer's data.
- Manager score changes create an engineer notification.
- **No-Go:** an engineer self-assessment silently becomes an approved official score without the agreed review control.

## Data Integrity

- A sample dashboard score reconciles to source assessment records and weighting values.
- Missing assessments are displayed as missing, not silently treated as knowledge.
- Duplicate assessment rows are rejected by SQL Server.
- Scores outside 0-3 and weights outside 0-5 are rejected by SQL Server.
- User accounts are linked to valid engineer records or intentionally have no engineer link.
- Two users can update different records concurrently without lost changes.
- **No-Go:** two managers editing the same assessment can overwrite each other without warning or traceability.

## Certification Workflow

- A PDF certificate uploads and opens on the workstation that uploaded it.
- The pilot team records that certificate files are currently workstation-local.
- **No-Go for multi-PC archive use:** a certificate uploaded on one PC cannot be opened on another and no approved shared/central storage is in place.

## Resilience And Operations

- App restart does not lose committed SQL data.
- Network interruption produces a clear recoverable error.
- SQL Server backup completes successfully.
- IT restores a backup into a non-production database and validates it.
- Logs can be located in the user's Aptitude AppData folder.
- Logs do not grow without an agreed retention/cleanup process.
- The previous client package and database rollback procedure are available.

## Acceptance

- Business owner:
- IT owner:
- Pilot result: Go / Conditional Go / No-Go
- Open issues and owners:
- Approved pilot start date:
- Approved production date:
