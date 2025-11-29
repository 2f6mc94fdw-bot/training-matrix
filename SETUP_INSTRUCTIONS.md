# Training Matrix - Setup Instructions for Mac

You already have SQL Server running in Docker on port 1433! This makes setup easier.

## Quick Setup (Automated)

From your terminal in the `training-matrix` folder:

```bash
./setup-mac.sh
```

This script will automatically:
1. ✅ Detect your existing SQL Server container
2. ✅ Create the `.env` configuration file
3. ✅ Create the `training_matrix` database
4. ✅ Install all tables and schema
5. ✅ Install npm dependencies

Then just run: `npm start`

---

## Manual Setup (Step by Step)

If you prefer to do it manually or the script has issues:

### Step 1: Create .env file

In your `training-matrix` folder, create a file named `.env` with this content:

```env
# Backend API Server Configuration
PORT=3001
NODE_ENV=development

# Frontend URL (for CORS)
FRONTEND_URL=http://localhost:3000

# Database Configuration (SQL Server)
DB_SERVER=localhost
DB_NAME=training_matrix
DB_USER=sa
DB_PASSWORD=YourStrong@Passw0rd

# Database Connection Options
DB_ENCRYPT=true
DB_TRUST_SERVER_CERTIFICATE=true
DB_CONNECTION_TIMEOUT=30000
DB_REQUEST_TIMEOUT=30000

# Database Pool Settings
DB_POOL_MAX=10
DB_POOL_MIN=0
DB_POOL_IDLE_TIMEOUT=30000
```

### Step 2: Find your SQL Server container

```bash
docker ps -a | grep sql
```

You should see a container running SQL Server.

### Step 3: Create the database

Try this password first (most common default):

```bash
docker exec -i $(docker ps --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}" | head -n 1) \
    /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P "YourStrong@Passw0rd" \
    -Q "CREATE DATABASE training_matrix;"
```

**If you get a password error**, your SQL Server container might be using a different password. Common alternatives:
- `YourStrong@Passw0rd123`
- `Your_password123`
- `SqlServer2022!`

If you can't remember the password, it's easier to create a new container with a known password:

```bash
# Stop the old container
docker stop $(docker ps --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}")

# Remove it
docker rm $(docker ps -a --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}")

# Create new one with known password
docker run -d \
    --name training-matrix-sql \
    -e "ACCEPT_EULA=Y" \
    -e "SA_PASSWORD=YourStrong@Passw0rd" \
    -p 1433:1433 \
    mcr.microsoft.com/mssql/server:2022-latest

# Wait 10 seconds for SQL Server to start
sleep 10
```

### Step 4: Run the database schema

```bash
docker exec -i $(docker ps --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}" | head -n 1) \
    /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P "YourStrong@Passw0rd" -d training_matrix \
    < database/schema.sql
```

### Step 5: Install dependencies

```bash
npm install
```

### Step 6: Start the application

```bash
npm start
```

This will start both the backend (port 3001) and frontend (port 3000).

### Step 7: Open in browser

Go to: **http://localhost:3000**

Login with:
- **Username:** admin
- **Password:** admin123

---

## Troubleshooting

### Problem: "port is already allocated"
- ✅ **You already solved this!** Your SQL Server is already running.
- Just skip the `docker run` step and use the existing container.

### Problem: "Login failed for user 'sa'"
- The `.env` password doesn't match your Docker container password
- Either update `.env` with the correct password, or recreate the container with a known password

### Problem: "Cannot find module"
- Run: `npm install`

### Problem: "Failed to connect to database"
- Check if SQL Server container is running: `docker ps`
- If not running: `docker start [container-name]`
- Check password in `.env` matches your container

### Problem: "CORS error" in browser
- Make sure `.env` has `FRONTEND_URL=http://localhost:3000`
- Restart the backend: `npm run server`

---

## What's Next?

Once you have it running locally:

1. **Test the application** - Add engineers, production areas, etc.
2. **Add your real data** - Use the admin panel to create your actual production areas
3. **Deploy to work** - We'll set up a central SQL Server that all machines can access

---

## For Work Deployment (Later)

When you're ready to use this at work with multiple computers:

1. **Install SQL Server on a work server** (Windows or Linux)
2. **Update `.env`** on each computer to point to that server:
   ```env
   DB_SERVER=work-server-name-or-ip
   DB_NAME=training_matrix
   DB_USER=training_matrix_user
   DB_PASSWORD=YourWorkPassword
   ```
3. **Configure firewall** to allow port 1433
4. **Each computer** runs the same app but connects to the central database

---

**Need help?** Check the other documentation files:
- `README.md` - Application overview
- `REVIEW_SUMMARY.md` - Recent fixes and features
- `SQL_SERVER_SETUP.md` - Detailed SQL Server guide
