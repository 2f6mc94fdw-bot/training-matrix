-- Training Matrix Database Schema
-- PostgreSQL version (can be adapted for MySQL/SQL Server)

-- Create database (run this separately if needed)
-- CREATE DATABASE training_matrix;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id VARCHAR(50) PRIMARY KEY,
    username VARCHAR(100) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    role VARCHAR(20) NOT NULL CHECK (role IN ('admin', 'engineer')),
    engineer_id VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Engineers table
CREATE TABLE IF NOT EXISTS engineers (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    shift VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Production areas table
CREATE TABLE IF NOT EXISTS production_areas (
    id SERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Machines table
CREATE TABLE IF NOT EXISTS machines (
    id SERIAL PRIMARY KEY,
    production_area_id INTEGER NOT NULL REFERENCES production_areas(id) ON DELETE CASCADE,
    name VARCHAR(200) NOT NULL,
    importance INTEGER DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Competencies table
CREATE TABLE IF NOT EXISTS competencies (
    id SERIAL PRIMARY KEY,
    machine_id INTEGER NOT NULL REFERENCES machines(id) ON DELETE CASCADE,
    name VARCHAR(200) NOT NULL,
    max_score INTEGER NOT NULL DEFAULT 3,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Assessments table
CREATE TABLE IF NOT EXISTS assessments (
    id SERIAL PRIMARY KEY,
    engineer_id VARCHAR(50) NOT NULL REFERENCES engineers(id) ON DELETE CASCADE,
    production_area_id INTEGER NOT NULL REFERENCES production_areas(id) ON DELETE CASCADE,
    machine_id INTEGER NOT NULL REFERENCES machines(id) ON DELETE CASCADE,
    competency_id INTEGER NOT NULL REFERENCES competencies(id) ON DELETE CASCADE,
    score INTEGER NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(engineer_id, production_area_id, machine_id, competency_id)
);

-- Core skills categories table
CREATE TABLE IF NOT EXISTS core_skill_categories (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Core skills table
CREATE TABLE IF NOT EXISTS core_skills (
    id VARCHAR(50) PRIMARY KEY,
    category_id VARCHAR(50) NOT NULL REFERENCES core_skill_categories(id) ON DELETE CASCADE,
    name VARCHAR(200) NOT NULL,
    max_score INTEGER NOT NULL DEFAULT 3,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Core skill assessments table
CREATE TABLE IF NOT EXISTS core_skill_assessments (
    id SERIAL PRIMARY KEY,
    engineer_id VARCHAR(50) NOT NULL REFERENCES engineers(id) ON DELETE CASCADE,
    category_id VARCHAR(50) NOT NULL REFERENCES core_skill_categories(id) ON DELETE CASCADE,
    skill_id VARCHAR(50) NOT NULL REFERENCES core_skills(id) ON DELETE CASCADE,
    score INTEGER NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(engineer_id, category_id, skill_id)
);

-- Certifications table
CREATE TABLE IF NOT EXISTS certifications (
    id SERIAL PRIMARY KEY,
    engineer_id VARCHAR(50) NOT NULL REFERENCES engineers(id) ON DELETE CASCADE,
    name VARCHAR(200) NOT NULL,
    date_earned DATE NOT NULL,
    expiry_date DATE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Snapshots table (for progress history)
CREATE TABLE IF NOT EXISTS snapshots (
    id VARCHAR(50) PRIMARY KEY,
    description TEXT,
    timestamp TIMESTAMP NOT NULL,
    data JSONB NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Audit log table
CREATE TABLE IF NOT EXISTS audit_logs (
    id VARCHAR(50) PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    user_id VARCHAR(50),
    action VARCHAR(100) NOT NULL,
    details TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS idx_engineers_shift ON engineers(shift);
CREATE INDEX IF NOT EXISTS idx_machines_area ON machines(production_area_id);
CREATE INDEX IF NOT EXISTS idx_competencies_machine ON competencies(machine_id);
CREATE INDEX IF NOT EXISTS idx_assessments_engineer ON assessments(engineer_id);
CREATE INDEX IF NOT EXISTS idx_assessments_composite ON assessments(engineer_id, production_area_id, machine_id, competency_id);
CREATE INDEX IF NOT EXISTS idx_core_skills_category ON core_skills(category_id);
CREATE INDEX IF NOT EXISTS idx_core_skill_assessments_engineer ON core_skill_assessments(engineer_id);
CREATE INDEX IF NOT EXISTS idx_certifications_engineer ON certifications(engineer_id);
CREATE INDEX IF NOT EXISTS idx_audit_logs_timestamp ON audit_logs(timestamp DESC);

-- Insert default admin user (password: admin123)
-- Note: You should change this password after first login
INSERT INTO users (id, username, password, role, engineer_id)
VALUES ('admin', 'admin', '$2a$10$XQK9X.xjKZv4PqGqxdpN0OYjQz5Z8rCqH9VB0KpXKNH0qUqKjKqKq', 'admin', NULL)
ON CONFLICT (id) DO NOTHING;

-- Insert default core skill categories
INSERT INTO core_skill_categories (id, name) VALUES
    ('mechanical', 'Mechanical Skills'),
    ('electrical', 'Electrical Skills'),
    ('software', 'Software Skills'),
    ('safety', 'Safety Skills'),
    ('leadership', 'Leadership & Soft Skills')
ON CONFLICT (id) DO NOTHING;

-- Insert default core skills
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('troubleshooting', 'mechanical', 'Troubleshooting', 3),
    ('preventive-maintenance', 'mechanical', 'Preventive Maintenance', 3),
    ('repair', 'mechanical', 'Equipment Repair', 3),
    ('installation', 'mechanical', 'Installation', 3),
    ('wiring', 'electrical', 'Wiring & Circuits', 3),
    ('circuit-diagnosis', 'electrical', 'Circuit Diagnosis', 3),
    ('plc', 'electrical', 'PLC Programming', 3),
    ('motor-control', 'electrical', 'Motor Control Systems', 3),
    ('programming', 'software', 'Basic Programming', 3),
    ('hmi', 'software', 'HMI Operation', 3),
    ('data-analysis', 'software', 'Data Analysis', 3),
    ('scada', 'software', 'SCADA Systems', 3),
    ('lockout-tagout', 'safety', 'Lockout/Tagout', 3),
    ('confined-spaces', 'safety', 'Confined Spaces', 3),
    ('hazard-identification', 'safety', 'Hazard Identification', 3),
    ('emergency-response', 'safety', 'Emergency Response', 3),
    ('communication', 'leadership', 'Communication', 3),
    ('teamwork', 'leadership', 'Teamwork & Collaboration', 3),
    ('problem-solving', 'leadership', 'Problem Solving', 3),
    ('training-others', 'leadership', 'Training Others', 3)
ON CONFLICT (id) DO NOTHING;
