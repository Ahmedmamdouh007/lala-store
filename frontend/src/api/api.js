import axios from 'axios';

const API_BASE_URL = 'http://localhost:8001/api';

const api = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Home API
export const getFeaturedProducts = () => api.get('/home/featured');
export const getCategories = () => api.get('/home/categories');

// Products API
export const getAllProducts = () => api.get('/products');
export const getProductsByGender = (gender) => api.get(`/products/${gender}`);
export const getProductById = (id) => api.get(`/products/details/${id}`);

// Cart API
export const getCartItems = (userId) => api.get(`/cart/${userId}`);
export const addToCart = (data) => api.post('/cart/add', data);
export const removeFromCart = (cartItemId) => api.delete(`/cart/remove/${cartItemId}`);
export const updateCartItem = (data) => api.put('/cart/update', data);

// Orders API
export const createOrder = (data) => api.post('/orders/create', data);
export const getUserOrders = (userId) => api.get(`/orders/user/${userId}`);

export default api;
