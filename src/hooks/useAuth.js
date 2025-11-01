import { useState, useEffect } from 'react';
import { loadData, saveData, logAction, verifyPassword } from '../utils/storage';

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

  const login = (username, password) => {
    const data = loadData();
    const user = data.users.find(u => u.username === username);

    if (user && verifyPassword(password, user.password)) {
      const userSession = {
        id: user.id,
        username: user.username,
        role: user.role,
        engineerId: user.engineerId
      };
      
      setCurrentUser(userSession);
      setIsAuthenticated(true);
      sessionStorage.setItem('current_user', JSON.stringify(userSession));
      
      logAction(username, 'login', 'User logged in');
      
      return { success: true, user: userSession };
    }
    
    return { success: false, message: 'Invalid username or password' };
  };
  
  const logout = () => {
    if (currentUser) {
      logAction(currentUser.username, 'logout', 'User logged out');
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
  
  const getEngineerProfile = () => {
    if (!currentUser?.engineerId) return null;
    
    const data = loadData();
    return data.engineers.find(e => e.id === currentUser.engineerId);
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
