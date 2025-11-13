// Input Validation Module
// Provides validation functions for API endpoints

/**
 * Validate that required fields are present in an object
 * @param {Object} data - The data object to validate
 * @param {Array<string>} requiredFields - Array of required field names
 * @returns {Object} { valid: boolean, missing: Array<string> }
 */
function validateRequiredFields(data, requiredFields) {
  const missing = [];

  for (const field of requiredFields) {
    if (data[field] === undefined || data[field] === null || data[field] === '') {
      missing.push(field);
    }
  }

  return {
    valid: missing.length === 0,
    missing
  };
}

/**
 * Validate string length
 * @param {string} value - The string to validate
 * @param {number} min - Minimum length
 * @param {number} max - Maximum length
 * @returns {boolean}
 */
function validateStringLength(value, min = 0, max = 255) {
  if (typeof value !== 'string') return false;
  return value.length >= min && value.length <= max;
}

/**
 * Validate email format (basic)
 * @param {string} email
 * @returns {boolean}
 */
function validateEmail(email) {
  if (typeof email !== 'string') return false;
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return emailRegex.test(email);
}

/**
 * Validate that value is one of allowed values
 * @param {any} value
 * @param {Array} allowedValues
 * @returns {boolean}
 */
function validateEnum(value, allowedValues) {
  return allowedValues.includes(value);
}

/**
 * Validate username format
 * - 3-50 characters
 * - Alphanumeric, underscore, hyphen only
 * @param {string} username
 * @returns {Object} { valid: boolean, message: string }
 */
function validateUsername(username) {
  if (typeof username !== 'string') {
    return { valid: false, message: 'Username must be a string' };
  }

  if (username.length < 3 || username.length > 50) {
    return { valid: false, message: 'Username must be between 3 and 50 characters' };
  }

  if (!/^[a-zA-Z0-9_-]+$/.test(username)) {
    return { valid: false, message: 'Username can only contain letters, numbers, underscores, and hyphens' };
  }

  return { valid: true };
}

/**
 * Validate password strength
 * - At least 8 characters
 * @param {string} password
 * @returns {Object} { valid: boolean, message: string }
 */
function validatePassword(password) {
  if (typeof password !== 'string') {
    return { valid: false, message: 'Password must be a string' };
  }

  if (password.length < 8) {
    return { valid: false, message: 'Password must be at least 8 characters long' };
  }

  if (password.length > 128) {
    return { valid: false, message: 'Password must be less than 128 characters' };
  }

  return { valid: true };
}

/**
 * Validate ID format (alphanumeric with underscores)
 * @param {string} id
 * @returns {boolean}
 */
function validateId(id) {
  if (typeof id !== 'string') return false;
  if (id.length === 0 || id.length > 100) return false;
  return /^[a-zA-Z0-9_-]+$/.test(id);
}

/**
 * Sanitize string input (remove potential XSS)
 * @param {string} input
 * @returns {string}
 */
function sanitizeString(input) {
  if (typeof input !== 'string') return '';
  return input
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#x27;')
    .replace(/\//g, '&#x2F;');
}

/**
 * Validate assessment score (0-3)
 * @param {number} score
 * @returns {boolean}
 */
function validateScore(score) {
  const num = Number(score);
  return !isNaN(num) && num >= 0 && num <= 3 && Number.isInteger(num);
}

/**
 * Validate date format (ISO 8601)
 * @param {string} date
 * @returns {boolean}
 */
function validateDate(date) {
  if (typeof date !== 'string') return false;
  const parsed = Date.parse(date);
  return !isNaN(parsed);
}

/**
 * Create validation middleware for express routes
 * @param {Function} validationFn - Function that takes req.body and returns { valid, errors }
 * @returns {Function} Express middleware
 */
function validateRequest(validationFn) {
  return (req, res, next) => {
    const result = validationFn(req.body, req.params, req.query);

    if (!result.valid) {
      return res.status(400).json({
        error: 'Validation failed',
        details: result.errors || result.message
      });
    }

    next();
  };
}

module.exports = {
  validateRequiredFields,
  validateStringLength,
  validateEmail,
  validateEnum,
  validateUsername,
  validatePassword,
  validateId,
  sanitizeString,
  validateScore,
  validateDate,
  validateRequest
};
