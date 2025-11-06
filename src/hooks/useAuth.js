import { useState, useEffect } from 'react';
import * as api from '../utils/api';

export const useAuth = () => {
  const [currentUser, setCurrentUser] = useState(null);
  const [isAuthenticated, setIsAuthenticated] = useState(false);

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
  }, []);

  const login = async (username, password) => {
    try {
      const user = await api.login(username, password);

      const userSession = {
        id: user.id,
        username: user.username,
        role: user.role,
        engineerId: user.engineer_id || user.engineerId
      };

      setCurrentUser(userSession);
      setIsAuthenticated(true);
      sessionStorage.setItem('current_user', JSON.stringify(userSession));

      // Log the login action
      await api.logAction(username, 'login', 'User logged in');

      return { success: true, user: userSession };
    } catch (error) {
      console.error('Login error:', error);
      return { success: false, message: error.message || 'Invalid username or password' };
    }
  };

  const logout = async () => {
    if (currentUser) {
      try {
        await api.logAction(currentUser.username, 'logout', 'User logged out');
      } catch (error) {
        console.error('Error logging logout:', error);
      }
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
      const data = await api.getAllData();
      return data.engineers.find(e => e.id === currentUser.engineerId);
    } catch (error) {
      console.error('Error getting engineer profile:', error);
      return null;
    }
  };

  return {
    currentUser,
    isAuthenticated,
    isAdmin,
    isEngineer,
    login,
    logout,
    getEngineerProfile
  };
};
