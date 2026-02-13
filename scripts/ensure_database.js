#!/usr/bin/env node
/**
 * Ensure LALA STORE database exists and schema is applied.
 * Uses same config as backend: "Lala store", postgres/1234, localhost:5432.
 * Run from project root: node scripts/ensure_database.js
 */

const { Client } = require('pg');
const fs = require('fs');
const path = require('path');

const DB_NAME = 'Lala store';
const config = {
  host: 'localhost',
  port: 5432,
  user: 'postgres',
  password: '1234',
};

async function main() {
  const projectRoot = path.resolve(__dirname, '..');
  const schemaPath = path.join(projectRoot, 'database', 'schema.sql');

  if (!fs.existsSync(schemaPath)) {
    console.error('Schema file not found:', schemaPath);
    process.exit(1);
  }

  const schemaSql = fs.readFileSync(schemaPath, 'utf8');

  // Connect to default 'postgres' DB to create "Lala store" if needed
  const adminClient = new Client({ ...config, database: 'postgres' });
  try {
    await adminClient.connect();
  } catch (e) {
    console.error('Cannot connect to PostgreSQL:', e.message);
    console.error('Ensure PostgreSQL is running on port 5432 and user postgres / password 1234.');
    process.exit(1);
  }

  const { rows: exists } = await adminClient.query(
    `SELECT 1 FROM pg_database WHERE datname = $1`,
    [DB_NAME]
  );
  if (exists.length === 0) {
    await adminClient.query(`CREATE DATABASE "Lala store"`);
    console.log('Created database "' + DB_NAME + '"');
  }
  await adminClient.end();

  // Connect to "Lala store" and run schema
  const dbClient = new Client({ ...config, database: DB_NAME });
  try {
    await dbClient.connect();
  } catch (e) {
    console.error('Cannot connect to database "' + DB_NAME + '":', e.message);
    process.exit(1);
  }

  try {
    await dbClient.query(schemaSql);
    console.log('Schema applied successfully.');
  } catch (e) {
    console.error('Schema execution error:', e.message);
    process.exit(1);
  }

  const { rows: [{ count: productCount }] } = await dbClient.query('SELECT COUNT(*) AS count FROM products');
  const { rows: [{ count: tableCount }] } = await dbClient.query(
    "SELECT COUNT(*) AS count FROM information_schema.tables WHERE table_schema = 'public'"
  );
  await dbClient.end();

  console.log('Tables:', tableCount);
  console.log('Products:', productCount);
  console.log('Database is ready.');
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
