#!/usr/bin/env node
/**
 * Ensure LALA STORE SQLite database exists, schema is applied, and verify it.
 * Uses same path as backend: database/lala-store.db (from backend/config/db_config.json or default).
 * Run from project root: node scripts/ensure_sqlite_db.js
 */

const fs = require('fs');
const path = require('path');

const projectRoot = path.resolve(__dirname, '..');
const dbDir = path.join(projectRoot, 'database');
const defaultDbPath = path.join(dbDir, 'lala-store.db');

function getDbPath() {
  const configPath = path.join(projectRoot, 'backend', 'config', 'db_config.json');
  if (fs.existsSync(configPath)) {
    try {
      const config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
      if (config.database_path) {
        const p = path.isAbsolute(config.database_path)
          ? config.database_path
          : path.join(projectRoot, config.database_path);
        return path.normalize(p);
      }
    } catch (e) {
      console.warn('Could not read db_config.json:', e.message);
    }
  }
  return defaultDbPath;
}

async function main() {
  const initSqlJs = require('sql.js');
  const SQL = await initSqlJs();
  const dbPath = getDbPath();

  if (!fs.existsSync(dbDir)) {
    fs.mkdirSync(dbDir, { recursive: true });
    console.log('Created directory:', dbDir);
  }

  const schemaPath = path.join(projectRoot, 'database', 'schema_sqlite.sql');
  if (!fs.existsSync(schemaPath)) {
    console.error('Schema file not found:', schemaPath);
    process.exit(1);
  }

  const schemaSql = fs.readFileSync(schemaPath, 'utf8');
  console.log('Database path:', dbPath);

  let db;
  if (fs.existsSync(dbPath)) {
    const fileBuffer = fs.readFileSync(dbPath);
    db = new SQL.Database(fileBuffer);
    console.log('Loaded existing database.');
  } else {
    db = new SQL.Database();
    console.log('Created new database.');
  }

  try {
    db.run('PRAGMA foreign_keys = ON;');
    db.run(schemaSql);
  } catch (err) {
    console.error('Schema execution error:', err.message);
    db.close();
    process.exit(1);
  }

  const data = db.export();
  const buffer = Buffer.from(data);
  fs.writeFileSync(dbPath, buffer);
  console.log('Schema applied and database saved.');

  // Apply extra seed: men's new products (idempotent)
  const mensSeedPath = path.join(projectRoot, 'database', 'seed_mens_new_products.sql');
  if (fs.existsSync(mensSeedPath)) {
    try {
      const mensSeedSql = fs.readFileSync(mensSeedPath, 'utf8');
      db.exec(mensSeedSql);
      const data2 = db.export();
      fs.writeFileSync(dbPath, Buffer.from(data2));
      console.log('Mens new products seed applied.');
    } catch (e) {
      console.warn('Mens seed warning:', e.message);
    }
  }

  const tables = ['categories', 'users', 'products', 'cart_items', 'orders', 'order_items'];
  for (const table of tables) {
    try {
      db.exec(`SELECT 1 FROM ${table} LIMIT 1`);
    } catch (e) {
      console.error('Missing or invalid table:', table, e.message);
      db.close();
      process.exit(1);
    }
  }

  const productResult = db.exec('SELECT COUNT(*) AS c FROM products');
  const categoryResult = db.exec('SELECT COUNT(*) AS c FROM categories');
  const productCount = productResult.length && productResult[0].values[0][0];
  const categoryCount = categoryResult.length && categoryResult[0].values[0][0];

  db.close();

  console.log('Tables OK:', tables.join(', '));
  console.log('Categories:', categoryCount, '| Products:', productCount);
  console.log('Database is ready.');
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
