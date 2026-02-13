# Bug Reference – Intentionally Vulnerable Code

**Total bugs: 23** (15 SQL injection, 8 buffer overflow)

Use this list to find each vulnerability in the codebase. All are for **educational use only** (e.g. security labs).

---

## Summary

| Type            | Count | Bug IDs      |
|-----------------|-------|--------------|
| SQL injection   | 15    | BUG-001 → BUG-015 |
| Buffer overflow| 8     | BUG-016 → BUG-023 |

---

## SQL injection (BUG-001 – BUG-015)

| ID      | File                    | Line(s)   | Endpoint / context |
|---------|-------------------------|-----------|----------------------------------------|
| **BUG-001** | `backend/routes/product_routes.cpp` | 25, 34–40 | **GET** `/api/products/details/<id>` – product ID concatenated into `WHERE p.id = ...` |
| **BUG-002** | `backend/routes/product_routes.cpp` | 125, 135–143 | **GET** `/api/products/<gender>` – path segment `gender` in `WHERE p.gender = '...'` |
| **BUG-003** | `backend/routes/product_routes.cpp` | 176, 186–190 | **GET** `/api/products/category/<name>` – path segment `categoryName` in `WHERE c.name = '...'` |
| **BUG-004** | `backend/routes/cart_routes.cpp`     | 24, 36–42 | **GET** `/api/cart/<user_id>` – user ID in `WHERE ci.user_id = ...` |
| **BUG-005** | `backend/routes/cart_routes.cpp`     | 99, 100–102 | **POST** `/api/cart/add` – user_id in `SELECT id FROM users WHERE id = ...` |
| **BUG-006** | `backend/routes/cart_routes.cpp`     | 113, 114–116 | **POST** `/api/cart/add` – product_id in `SELECT price FROM products WHERE id = ...` |
| **BUG-007** | `backend/routes/cart_routes.cpp`     | 129, 130–131 | **POST** `/api/cart/add` – user_id, product_id in `SELECT id, quantity FROM cart_items WHERE ...` |
| **BUG-008** | `backend/routes/cart_routes.cpp`     | 142, 143 | **POST** `/api/cart/add` – quantity and id in `UPDATE cart_items SET quantity = ... WHERE id = ...` |
| **BUG-009** | `backend/routes/cart_routes.cpp`     | 157, 160–162 | **POST** `/api/cart/add` – user_id, product_id, quantity, price in `INSERT INTO cart_items ... VALUES (...)` |
| **BUG-010** | `backend/routes/cart_routes.cpp`     | 199, 211–213 | **DELETE** `/api/cart/remove/<id>` – cart item id in `DELETE FROM cart_items WHERE id = ...` |
| **BUG-011** | `backend/routes/cart_routes.cpp`     | 256, 259–260 | **PUT** `/api/cart/update` – quantity and cart_item_id in `UPDATE cart_items SET quantity = ... WHERE id = ...` |
| **BUG-012** | `backend/routes/order_routes.cpp`    | 76, 77–80 | **POST** `/api/orders/create` – user_id in cart query `WHERE ci.user_id = ...` |
| **BUG-013** | `backend/routes/order_routes.cpp`    | 238, 249–252 | **GET** `/api/orders/user/<id>` – user_id in `WHERE user_id = ...` |
| **BUG-014** | `backend/routes/order_routes.cpp`    | 282, 291–294 | **GET** `/api/orders/by-status?status=` – query param `status` in `WHERE status = '...'` |
| **BUG-015** | `backend/routes/home_routes.cpp`     | 116, 127–132 | **GET** `/api/home/search?q=` – query param `q` in `WHERE p.name LIKE '%...%'` |

---

## Buffer overflow (BUG-016 – BUG-023)

| ID      | File                          | Line(s)   | Endpoint / context |
|---------|-------------------------------|-----------|----------------------------------------|
| **BUG-016** | `backend/main.cpp`            | 34, 37, 44 | **POST** `/api/lab/echo` – `strcpy(buf, input)` into `char buf[32]` |
| **BUG-017** | `backend/main.cpp`            | 52, 54, 57 | **GET** `/api/lab/user/<string>` – `strcpy(name_buf, username)` into `char name_buf[16]` |
| **BUG-018** | `backend/main.cpp`            | 63, 66, 70, 72, 74 | **POST** `/api/lab/format` – `sprintf(out, "%s: %s", tag, msg)` into `char out[24]` |
| **BUG-019** | `backend/main.cpp`            | 95, 98, 102 | **POST** `/api/lab/comment-raw` – `strcpy(comment, req.body)` into `char comment[64]` |
| **BUG-020** | `backend/main.cpp`            | 107, 112, 116 | **POST** `/api/lab/copy` – `VulnerableHelper::copyToBuffer(val, buf)` into `char buf[32]` (see BUG-022) |
| **BUG-021** | `backend/main.cpp`            | 124, 128, 131 | **POST** `/api/lab/greet` – `VulnerableHelper::formatGreeting(name, out)` into `char out[40]` (see BUG-023) |
| **BUG-022** | `backend/utils/vulnerable_helper.cpp` | 6–7 | `copyToBuffer()` – `strcpy(out, input.c_str())` with no bounds check (used by BUG-020) |
| **BUG-023** | `backend/utils/vulnerable_helper.cpp` | 10–11 | `formatGreeting()` – `sprintf(out, "Hello, %s!", name)` with no length limit (used by BUG-021) |

---

## File index (where to look)

| File | Bugs |
|------|------|
| `backend/routes/product_routes.cpp` | BUG-001, BUG-002, BUG-003 |
| `backend/routes/cart_routes.cpp`    | BUG-004, BUG-005, BUG-006, BUG-007, BUG-008, BUG-009, BUG-010, BUG-011 |
| `backend/routes/order_routes.cpp`   | BUG-012, BUG-013, BUG-014 |
| `backend/routes/home_routes.cpp`   | BUG-015 |
| `backend/main.cpp`                 | BUG-016, BUG-017, BUG-018, BUG-019, BUG-020, BUG-021 |
| `backend/utils/vulnerable_helper.cpp` | BUG-022, BUG-023 |

Search for `BUG-` in the repo to jump to each reference in code.

---

## URL for each bug

Base URL: **http://localhost:8001** (backend port 8001)

### SQL injection

| ID | Method | URL | Notes |
|----|--------|-----|--------|
| **BUG-001** | GET | http://localhost:8001/api/products/details/**1** | Replace **1** with product ID (or inject: `1 OR 1=1`) |
| **BUG-002** | GET | http://localhost:8001/api/products/**men** | Replace **men** with gender (or inject: `men' OR '1'='1`) |
| **BUG-003** | GET | http://localhost:8001/api/products/category/**T-Shirts** | Replace with category name (or inject in name) |
| **BUG-004** | GET | http://localhost:8001/api/cart/**1** | Replace **1** with user_id |
| **BUG-005** | POST | http://localhost:8001/api/cart/add | Body: `{"user_id": 1, "product_id": 1}` — inject via `user_id` |
| **BUG-006** | POST | http://localhost:8001/api/cart/add | Body: `{"user_id": 1, "product_id": 1}` — inject via `product_id` |
| **BUG-007** | POST | http://localhost:8001/api/cart/add | Body: `{"user_id": 1, "product_id": 1}` — inject via `user_id` or `product_id` |
| **BUG-008** | POST | http://localhost:8001/api/cart/add | Same as above; triggers on update path (quantity/id in UPDATE) |
| **BUG-009** | POST | http://localhost:8001/api/cart/add | Same as above; inject in user_id, product_id, quantity, or (indirectly) price path |
| **BUG-010** | DELETE | http://localhost:8001/api/cart/remove/**1** | Replace **1** with cart_item id |
| **BUG-011** | PUT | http://localhost:8001/api/cart/update | Body: `{"cart_item_id": 1, "quantity": 2}` |
| **BUG-012** | POST | http://localhost:8001/api/orders/create | Body includes `user_id`; inject via user_id in cart query |
| **BUG-013** | GET | http://localhost:8001/api/orders/user/**1** | Replace **1** with user_id |
| **BUG-014** | GET | http://localhost:8001/api/orders/by-status?**status=pending** | Inject via `status` query param |
| **BUG-015** | GET | http://localhost:8001/api/home/search?**q=shirt** | Inject via `q` query param |

### Buffer overflow

| ID | Method | URL | Notes |
|----|--------|-----|--------|
| **BUG-016** | POST | http://localhost:8001/api/lab/echo | Body: `{"input": "A...long string > 32 chars"}` |
| **BUG-017** | GET | http://localhost:8001/api/lab/user/**username** | Put long string in path (e.g. 20+ chars) |
| **BUG-018** | POST | http://localhost:8001/api/lab/format | Body: `{"tag": "x", "msg": "long..."}` — combined length > 24 |
| **BUG-019** | POST | http://localhost:8001/api/lab/comment-raw | Body: raw string > 64 chars (or JSON string that gets parsed elsewhere; this route uses raw body) |
| **BUG-020** | POST | http://localhost:8001/api/lab/copy | Body: `{"value": "A...long string > 32 chars"}` |
| **BUG-021** | POST | http://localhost:8001/api/lab/greet | Body: `{"name": "A...long string > 33 chars"}` (format is "Hello, %s!") |
| **BUG-022** | — | (no direct URL) | Triggered via **BUG-020**: POST http://localhost:8001/api/lab/copy |
| **BUG-023** | — | (no direct URL) | Triggered via **BUG-021**: POST http://localhost:8001/api/lab/greet |

### One-line URL list (GETs and base URLs for POST/PUT/DELETE)

```
BUG-001  GET  http://localhost:8001/api/products/details/1
BUG-002  GET  http://localhost:8001/api/products/men
BUG-003  GET  http://localhost:8001/api/products/category/T-Shirts
BUG-004  GET  http://localhost:8001/api/cart/1
BUG-005  POST http://localhost:8001/api/cart/add
BUG-006  POST http://localhost:8001/api/cart/add
BUG-007  POST http://localhost:8001/api/cart/add
BUG-008  POST http://localhost:8001/api/cart/add
BUG-009  POST http://localhost:8001/api/cart/add
BUG-010  DELETE http://localhost:8001/api/cart/remove/1
BUG-011  PUT  http://localhost:8001/api/cart/update
BUG-012  POST http://localhost:8001/api/orders/create
BUG-013  GET  http://localhost:8001/api/orders/user/1
BUG-014  GET  http://localhost:8001/api/orders/by-status?status=pending
BUG-015  GET  http://localhost:8001/api/home/search?q=shirt
BUG-016  POST http://localhost:8001/api/lab/echo
BUG-017  GET  http://localhost:8001/api/lab/user/testuser
BUG-018  POST http://localhost:8001/api/lab/format
BUG-019  POST http://localhost:8001/api/lab/comment-raw
BUG-020  POST http://localhost:8001/api/lab/copy
BUG-021  POST http://localhost:8001/api/lab/greet
BUG-022  (via BUG-020) http://localhost:8001/api/lab/copy
BUG-023  (via BUG-021) http://localhost:8001/api/lab/greet
```
