# Training Matrix - Quick Start Guide

## ✨ Easy Start (Mac)

### Option 1: Double-Click to Start
1. **Find** the file `start-training-matrix.command` in your training-matrix folder
2. **Double-click** it
3. A Terminal window will open and start everything automatically
4. Wait for the message showing the app is ready
5. **Open your browser** to http://localhost:3000
6. **Login** with username: `admin` and password: `admin123`

### Option 2: Command Line
Open Terminal, navigate to the training-matrix folder, and run:
```bash
cd /Users/danielabley/Desktop/training-matrix
npm start
```

## 🛑 How to Stop

Press **Ctrl+C** in the Terminal window where the app is running.

## 🔄 Restarting

Just double-click `start-training-matrix.command` again, or run `npm start`.

## 📊 What Gets Started

When you start the application:
1. ✅ SQL Server Docker container (if not already running)
2. ✅ Backend API server (port 3001)
3. ✅ Frontend web app (port 3000)

## 🌐 Accessing the Application

- **Main App**: http://localhost:3000
- **Login**: admin / admin123

## 💾 Your Data

All your data is stored in the SQL Server database running in Docker:
- **Container name**: sql-server-training
- **Database name**: training_matrix
- **Data persists** even when you stop the container

## 🆘 Troubleshooting

### "Docker is not running"
- Open **Docker Desktop** application
- Wait for Docker to fully start
- Try running the startup script again

### "Port already in use"
- Something else is using port 3000 or 3001
- Close other applications and try again

### "Can't connect to database"
- Make sure Docker is running
- Check that the SQL Server container is running: `docker ps`
- Restart the SQL Server container: `docker restart sql-server-training`

### Clear browser cache
- If the app looks broken, press **Cmd+Shift+R** to hard refresh

## 🚀 Next Steps - Using at Work

Once you've tested locally and are ready to use this at work with multiple computers:

1. **Set up a central SQL Server** on a work server (Windows or Linux)
2. **Update the `.env` file** on each computer to point to that server
3. **Everyone can run the same app** but connect to the shared database

Need help with work deployment? Check `DEPLOYMENT_GUIDE.md`

## 📝 Daily Usage

1. **Start the app**: Double-click `start-training-matrix.command`
2. **Use the app**: Go to http://localhost:3000 in your browser
3. **When finished**: Press Ctrl+C in the Terminal to stop

That's it! 🎉
