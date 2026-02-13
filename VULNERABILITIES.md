# Intentionally Vulnerable Code (Educational Use Only)

This codebase has been modified to **simulate a beginner student project** with multiple security vulnerabilities. Use **only in an isolated lab** (e.g. localhost). **Never deploy to production or expose to the internet.**

**See [BUG_REFERENCE.md](BUG_REFERENCE.md) for the full list of 23 bugs with IDs (BUG-001–BUG-023), file paths, line numbers, and locations.**

---

## SQL Injection

User- or request-controlled input is concatenated directly into SQL strings instead of using parameterized queries.

| Location | Endpoint / Usage | Vulnerable input |
|----------|------------------|------------------|
| `backend/routes/product_routes.cpp` | `GET /api/products/details/<id>` | Product ID (concatenated into `WHERE p.id = ...`) |
| `backend/routes/product_routes.cpp` | `GET /api/products/<gender>` | URL path segment `gender` |
| `backend/routes/product_routes.cpp` | `GET /api/products/category/<name>` | URL path segment `categoryName` |
| `backend/routes/cart_routes.cpp` | `GET /api/cart/<user_id>` | User ID in `WHERE ci.user_id = ...` |
| `backend/routes/cart_routes.cpp` | `POST /api/cart/add` | user_id, product_id in SELECT/INSERT/UPDATE |
| `backend/routes/cart_routes.cpp` | `DELETE /api/cart/remove/<id>` | Cart item ID in `DELETE ... WHERE id = ...` |
| `backend/routes/cart_routes.cpp` | `PUT /api/cart/update` | quantity, cart_item_id in `UPDATE ... SET quantity = ... WHERE id = ...` |
| `backend/routes/order_routes.cpp` | `POST /api/orders/create` | user_id in cart query `WHERE ci.user_id = ...` |
| `backend/routes/order_routes.cpp` | `GET /api/orders/user/<user_id>` | User ID in `WHERE user_id = ...` |
| `backend/routes/order_routes.cpp` | `GET /api/orders/by-status?status=` | Query param `status` in `WHERE status = '...'` |
| `backend/routes/home_routes.cpp` | `GET /api/home/search?q=` | Query param `q` in `WHERE p.name LIKE '%...%'` |

**Example exploit (SQLi):**  
`GET /api/products/men' OR '1'='1` or `GET /api/home/search?q=x' OR 1=1--`

---

## Buffer Overflows

Fixed-size stack buffers are filled with user-controlled data using unsafe functions (`strcpy`, `sprintf`) with no bounds checking.

| Location | Endpoint / Usage | Buffer size | Unsafe function |
|----------|------------------|-------------|------------------|
| `backend/main.cpp` | `POST /api/lab/echo` | 32 bytes | `strcpy(buf, input.c_str())` |
| `backend/main.cpp` | `GET /api/lab/user/<username>` | 16 bytes | `strcpy(name_buf, username.c_str())` |
| `backend/main.cpp` | `POST /api/lab/format` | 24 bytes | `sprintf(out, "%s: %s", tag, msg)` |
| `backend/main.cpp` | `POST /api/lab/comment-raw` | 64 bytes | `strcpy(comment, req.body.c_str())` |
| `backend/main.cpp` | `POST /api/lab/copy` | 32 bytes | `VulnerableHelper::copyToBuffer(val, buf)` → `strcpy` |
| `backend/main.cpp` | `POST /api/lab/greet` | 40 bytes | `VulnerableHelper::formatGreeting(name, out)` → `sprintf` |
| `backend/utils/vulnerable_helper.cpp` | Used by lab routes | 32 / 40 bytes | `strcpy`, `sprintf` |

**Example exploit (overflow):**  
`POST /api/lab/echo` with body `{"input": "A"... 100 times}` or long username in `/api/lab/user/<long_string>`.

---

## How to Fix (For Learning)

1. **SQL injection:** Use parameterized queries only (e.g. `PQexecParams` with `$1`, `$2`, …). Never build SQL with string concatenation of user input.
2. **Buffer overflows:** Use `std::string`, bounded APIs (`strncpy` with correct null termination, `snprintf`), or safe C++ containers; avoid `strcpy`, `sprintf`, and unbounded copies into fixed-size buffers.

---

*This file is for documentation only. Remove or refactor all vulnerable code before any production or shared deployment.*
