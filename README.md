# LALA STORE - E-Commerce Website

A simple, clean e-commerce website for clothing built with C++ (Crow) backend, React frontend, and PostgreSQL database.

## 🏗️ Architecture

```
[ React Frontend (Port 3001) ]
        |
        |  REST API (JSON / HTTP)
        v
[ Crow C++ Backend (Port 8001) ]
        |
        |  SQL
        v
[ PostgreSQL (Local Server) ]
```

## 📁 Project Structure

```
lala-store/
├── backend/              # C++ Backend (Crow framework)
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── config/
│   │   └── db_config.json
│   ├── db/
│   │   ├── connection.h
│   │   └── connection.cpp
│   ├── routes/
│   │   ├── home_routes.cpp/h
│   │   ├── product_routes.cpp/h
│   │   ├── cart_routes.cpp/h
│   │   └── order_routes.cpp/h
│   ├── models/
│   │   ├── User.h
│   │   ├── Product.h
│   │   ├── Category.h
│   │   ├── Cart.h
│   │   └── Order.h
│   └── utils/
│       └── response_helper.h
│
├── frontend/             # React Frontend
│   ├── public/
│   │   └── index.html
│   ├── src/
│   │   ├── api/
│   │   │   └── api.js
│   │   ├── components/
│   │   │   ├── Navbar.jsx/css
│   │   │   ├── Footer.jsx/css
│   │   │   ├── ProductCard.jsx/css
│   │   │   └── CartItem.jsx/css
│   │   ├── pages/
│   │   │   ├── Home.jsx/css
│   │   │   ├── Men.jsx/css
│   │   │   ├── Women.jsx/css
│   │   │   ├── Cart.jsx/css
│   │   │   ├── ProductDetails.jsx/css
│   │   │   └── Checkout.jsx/css
│   │   ├── context/
│   │   │   └── CartContext.jsx
│   │   ├── App.jsx/css
│   │   └── main.jsx
│   ├── package.json
│   └── vite.config.js
│
└── database/
    └── schema.sql        # PostgreSQL schema
```

## 🚀 Setup Instructions

### Prerequisites

- PostgreSQL installed and running locally
- C++ compiler (GCC/Clang) with C++17 support
- CMake (3.15 or higher)
- Node.js and npm/yarn
- Crow C++ framework (header-only, download from GitHub)

### 1. Database Setup

1. **Create the database:**
   ```bash
   psql -U postgres
   CREATE DATABASE "Lala store";
   \q
   ```

2. **Run the schema:**
   ```bash
   psql -U postgres -d "Lala store" -f database/schema.sql
   ```

   Or manually:
   ```bash
   psql -U postgres -d "Lala store"
   ```
   Then paste the contents of `database/schema.sql`

### 2. Backend Setup (C++)

1. **Install Crow framework:**
   ```bash
   cd backend
   git clone https://github.com/CrowCpp/Crow.git crow
   # Or download and extract Crow headers to backend/crow/include/
   ```

2. **Install nlohmann/json (for JSON parsing):**
   ```bash
   # Using vcpkg or download from https://github.com/nlohmann/json
   # Place json.hpp in backend/include/ or adjust include paths
   ```

3. **Build the backend:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Run the backend:**
   ```bash
   ./lala_store
   ```
   The server will start on `http://localhost:8001`

### 3. Frontend Setup (React)

1. **Install dependencies:**
   ```bash
   cd frontend
   npm install
   ```

2. **Start the development server:**
   ```bash
   npm run dev
   ```
   The frontend will start on `http://localhost:3001`

## 📄 API Endpoints

### Home
- `GET /api/home/featured` - Get featured products
- `GET /api/home/categories` - Get all categories

### Products
- `GET /api/products` - Get all products
- `GET /api/products/{gender}` - Get products by gender (men/women)
- `GET /api/products/details/{id}` - Get product by ID

### Cart
- `GET /api/cart/{user_id}` - Get cart items for user
- `POST /api/cart/add` - Add item to cart
  ```json
  {
    "user_id": 1,
    "product_id": 1,
    "quantity": 1
  }
  ```
- `DELETE /api/cart/remove/{cart_item_id}` - Remove item from cart
- `PUT /api/cart/update` - Update cart item quantity
  ```json
  {
    "cart_item_id": 1,
    "quantity": 2
  }
  ```

### Orders
- `POST /api/orders/create` - Create order from cart
  ```json
  {
    "user_id": 1,
    "shipping_address": "123 Main St, City, Country"
  }
  ```
- `GET /api/orders/user/{user_id}` - Get user orders

## 🎨 Pages

- **Home** (`/`) - Featured products and categories
- **Men** (`/men`) - Men's clothing collection
- **Women** (`/women`) - Women's clothing collection
- **Product Details** (`/product/:id`) - Individual product page
- **Cart** (`/cart`) - Shopping cart
- **Checkout** (`/checkout`) - Order placement

## 🔧 Configuration

### Database Configuration

Edit `backend/config/db_config.json`:
```json
{
  "host": "localhost",
  "port": 5432,
  "database": "Lala store",
  "user": "postgres",
  "password": "1234"
}
```

### Frontend API Configuration

The frontend is configured to proxy API requests to `http://localhost:8001` (see `frontend/vite.config.js`).

## 📝 Notes

- The current implementation uses a hardcoded `user_id = 1` for cart operations. In production, implement proper authentication.
- Product images use placeholder URLs. Replace with actual image URLs in the database.
- The database includes sample data for testing.
- CORS is handled by the Crow framework for development.

## 🛠️ Development

### Backend Development
- Backend runs on port 8001
- Hot reload not available - rebuild after changes
- Check logs for database connection issues

### Frontend Development
- Frontend runs on port 3001
- Hot reload enabled via Vite
- API calls are proxied to backend

## 📦 Dependencies

### Backend
- Crow C++ framework
- PostgreSQL (libpq)
- nlohmann/json

### Frontend
- React 18
- React Router DOM 6
- Axios
- Vite

## 🐛 Troubleshooting

1. **Database connection fails:**
   - Verify PostgreSQL is running: `pg_isready -U postgres`
   - Check database name matches exactly: "Lala store" (with space)
   - Verify credentials in `db_config.json`

2. **Backend won't compile:**
   - Ensure Crow headers are accessible
   - Check PostgreSQL development libraries are installed
   - Verify C++17 compiler support

3. **Frontend can't connect to backend:**
   - Ensure backend is running on port 8001
   - Check CORS settings
   - Verify proxy configuration in `vite.config.js`

## 📄 License

This project is for educational purposes.
