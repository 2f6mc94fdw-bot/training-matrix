import { useState, useEffect } from 'react';
import api from '../utils/api';

export const useAuth = () => {
  const [currentUser, setCurrentUser] = useState(null);
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    // Check if user is already logged in
    const savedUser = sessionStorage.getItem('current_user');
    if (savedUser) {
      try {
        const user = JSON.parse(savedUser);
        setCurrentUser(user);
        setIsAuthenticated(true);
      } catch (error) {
        console.error('Error loading saved user:', error);
      }
    }
    setLoading(false);
  }, []);

  const login = async (username, password) => {
    try {
      const response = await api.login(username, password);

      if (response.user) {
        const userSession = {
          id: response.user.id,
          username: response.user.username,
          role: response.user.role,
          engineerId: response.user.engineer_id
        };

        setCurrentUser(userSession);
        setIsAuthenticated(true);
        sessionStorage.setItem('current_user', JSON.stringify(userSession));

        // Log action to audit log
        await api.logAction(userSession.id, 'login', 'User logged in').catch(console.error);

        return { success: true, user: userSession };
      }

      return { success: false, message: 'Invalid username or password' };
    } catch (error) {
      console.error('Login error:', error);
      return { success: false, message: error.message || 'Login failed' };
    }
  };

  const logout = async () => {
    if (currentUser) {
      // Log action to audit log
      await api.logAction(currentUser.id, 'logout', 'User logged out').catch(console.error);
    }

    setCurrentUser(null);
    setIsAuthenticated(false);
    sessionStorage.removeItem('current_user');
  };

  const isAdmin = () => {
    return currentUser?.role === 'admin';
  };

  const isEngineer = () => {
    return currentUser?.role === 'engineer';
  };

  const getEngineerProfile = async () => {
    if (!currentUser?.engineerId) return null;

    try {
      const engineers = await api.getEngineers();
      return engineers.find(e => e.id === currentUser.engineerId);
    } catch (error) {
      console.error('Error getting engineer profile:', error);
      return null;
    }
  };

  return {
    currentUser,
    isAuthenticated,
    loading,
    isAdmin,
    isEngineer,
    login,
    logout,
    getEngineerProfile
  };
};
