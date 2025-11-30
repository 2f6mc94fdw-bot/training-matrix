# Training Matrix - Data Import Guide

## Quick Import from JSON Backup

You have a JSON backup file with all your production areas, machines, and competencies. Here's how to import it into your SQL Server database.

---

## Step 1: Save Your JSON Data

1. **Copy the JSON data** you have (the one you pasted)
2. **Create a new file** in your training-matrix folder called `production-data.json`
3. **Paste the JSON** into this file and save it

Or if you already have the file saved somewhere, just note the path.

---

## Step 2: Run the Import Script

Open Terminal and run:

```bash
cd /Users/danielabley/Desktop/training-matrix
node import-data.js production-data.json
```

**Or if your file is somewhere else:**
```bash
node import-data.js /path/to/your/backup-file.json
```

---

## Step 3: View Your Data

The script will import everything and show you a summary like:

```
====================================
✅ Import Complete!
====================================

📊 Summary:
   Production Areas: 6
   Machines: 33
   Competencies: 98
```

Then **open your browser** to http://localhost:3000 and you'll see all your production areas with machines and competencies! 🎉

---

## What Gets Imported

From your JSON file:
- ✅ **6 Production Areas**:
  - Viaflo Fill
  - L7 Packing
  - LINE 1
  - LINE 6
  - VIAFLO PACKING
  - Viaflo Racking

- ✅ **All Machines** with their importance ratings
- ✅ **All Competencies** with max scores for each machine

---

## Important Notes

1. **Delete existing data first** if you want a clean import:
   - Go to Admin panel in the app
   - Delete any test production areas you created
   - Or run: `docker exec -i sql-server-training /opt/mssql-tools18/bin/sqlcmd -S localhost -U sa -P "YourStrong@Passw0rd" -C -d training_matrix -Q "DELETE FROM production_areas WHERE id > 2;"`

2. **The import runs in a transaction** - if anything fails, nothing gets imported (all or nothing)

3. **Machines are linked to production areas** - you can't accidentally create orphaned data

4. **Engineers and assessments** are NOT in your JSON, so those won't be imported (you'll add engineers manually or from a separate file)

---

## Troubleshooting

### "File not found"
- Make sure the JSON file is in the training-matrix folder
- Or provide the full path: `node import-data.js /full/path/to/file.json`

### "Cannot find module"
- Make sure you're in the training-matrix directory
- Run `npm install` first

### "Database connection failed"
- Make sure SQL Server is running: `docker ps`
- Start it if needed: `docker start sql-server-training`
- Check the `.env` file has the correct password

### Duplicate data
- If you run the import twice, you'll get duplicate production areas
- Delete them first or start with a fresh database

---

## Next Steps After Import

1. **Add Engineers** - Go to Admin → Manage Engineers
2. **Start Assessments** - Click on a production area and start scoring engineers
3. **View Skills Matrix** - See the complete matrix with all competencies

---

**That's it!** Your entire competency structure will be in the database ready to use! 🚀
