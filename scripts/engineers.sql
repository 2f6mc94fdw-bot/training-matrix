-- SQL statements to import engineers and create user accounts
-- Password for all users: 'password' (hashed with bcrypt)

-- Note: The bcrypt hash below is for the password 'password'
-- Generated with: bcrypt.hash('password', 10)
-- Hash: $2a$10$rZc7V8zRg7QHxO5EwZPFIuNfJm1mM8p5xZ5nJQH5xH5xH5xH5xH5xH (example - will generate real ones)

-- Engineers
INSERT INTO engineers (id, name, shift) VALUES
('eng-1', 'John Irish', 'Day Shift'),
('eng-2', 'Mark Castleton', 'Day Shift'),
('eng-import-1761509910877-0', 'Dave Spauls', 'Day Shift'),
('eng-import-1761509910877-1', 'Ben Barnham', 'Day Shift'),
('eng-import-1761509910877-2', 'Mark Quin', 'B Shift'),
('eng-import-1761509910877-3', 'Jon Anderson', 'C Shift'),
('eng-import-1761509910877-4', 'Callum Clark', 'B Shift'),
('eng-import-1761509910877-5', 'Dustin Lawrie', 'Day Shift'),
('eng-import-1761509910877-6', 'Connor Clark', 'B Shift'),
('eng-import-1761509910877-7', 'Matt Challis', 'B Shift'),
('eng-import-1761509910877-8', 'Luke Hancy', 'B Shift'),
('eng-import-1761509910877-9', 'Simon Brooks', 'D Shift'),
('eng-import-1761509910877-10', 'Elliot McMullen', 'C Shift'),
('eng-import-1761509910877-11', 'Richard Smith', 'C Shift'),
('eng-import-1761509910877-12', 'Yauheni Shumanski', 'C Shift'),
('eng-import-1761509910877-13', 'Daniel Wilson', 'D Shift'),
('eng-import-1761509910877-14', 'Oliver Mills', 'D Shift'),
('eng-import-1761509910877-15', 'James Sarosi', 'D Shift'),
('eng-import-1761509910877-16', 'Scott Macfarlane', 'D Shift');

-- User accounts (password: 'password')
-- Bcrypt hash for 'password': $2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy

INSERT INTO users (id, username, password, role, engineer_id) VALUES
('user-eng-1', 'john.irish', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-1'),
('user-eng-2', 'mark.castleton', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-2'),
('user-eng-import-1761509910877-0', 'dave.spauls', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-0'),
('user-eng-import-1761509910877-1', 'ben.barnham', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-1'),
('user-eng-import-1761509910877-2', 'mark.quin', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-2'),
('user-eng-import-1761509910877-3', 'jon.anderson', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-3'),
('user-eng-import-1761509910877-4', 'callum.clark', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-4'),
('user-eng-import-1761509910877-5', 'dustin.lawrie', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-5'),
('user-eng-import-1761509910877-6', 'connor.clark', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-6'),
('user-eng-import-1761509910877-7', 'matt.challis', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-7'),
('user-eng-import-1761509910877-8', 'luke.hancy', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-8'),
('user-eng-import-1761509910877-9', 'simon.brooks', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-9'),
('user-eng-import-1761509910877-10', 'elliot.mcmullen', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-10'),
('user-eng-import-1761509910877-11', 'richard.smith', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-11'),
('user-eng-import-1761509910877-12', 'yauheni.shumanski', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-12'),
('user-eng-import-1761509910877-13', 'daniel.wilson', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-13'),
('user-eng-import-1761509910877-14', 'oliver.mills', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-14'),
('user-eng-import-1761509910877-15', 'james.sarosi', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-15'),
('user-eng-import-1761509910877-16', 'scott.macfarlane', '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy', 'engineer', 'eng-import-1761509910877-16');

-- Verification queries
SELECT COUNT(*) as total_engineers FROM engineers;
SELECT COUNT(*) as total_users FROM users WHERE role = 'engineer';
