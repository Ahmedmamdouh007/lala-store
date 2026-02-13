import { createContext, useContext, useState, useEffect } from 'react';
import { getCartItems, addToCart, removeFromCart, updateCartItem } from '../api/api';
import Toast from '../components/Toast';

const CartContext = createContext();

export const useCart = () => {
  const context = useContext(CartContext);
  if (!context) {
    throw new Error('useCart must be used within a CartProvider');
  }
  return context;
};

const defaultShipping = { customerName: '', phone: '', paymentMethod: '' };

export const CartProvider = ({ children }) => {
  const [cartItems, setCartItems] = useState([]);
  const [loading, setLoading] = useState(false);
  const [toast, setToast] = useState(null);
  const [shippingInfo, setShippingInfo] = useState(defaultShipping);
  const [userId] = useState(1); // Temporary: In production, get from auth

  useEffect(() => {
    loadCart();
  }, []);

  const loadCart = async () => {
    try {
      setLoading(true);
      const response = await getCartItems(userId);
      if (response.data.success) {
        setCartItems(response.data.data);
      }
    } catch (error) {
      console.error('Error loading cart:', error);
    } finally {
      setLoading(false);
    }
  };

  const addItemToCart = async (productId, quantity = 1, size = '') => {
    try {
      const response = await addToCart({
        user_id: userId,
        product_id: productId,
        quantity: quantity,
        ...(size && { size }),
      });
      if (response.data.success) {
        await loadCart();
        setToast({ message: 'Added to cart ✓', type: 'success' });
        return true;
      }
      const errorMsg = response.data?.message || 'Could not add to cart';
      console.error('Cart error:', errorMsg);
      setToast({ message: errorMsg, type: 'error' });
      return false;
    } catch (error) {
      console.error('Error adding to cart:', error);
      let errorMessage = 'Could not add to cart. Please try again.';
      
      if (error.response) {
        // Server responded with error status
        const serverError = error.response.data?.message || `Server error: ${error.response.status}`;
        errorMessage = serverError;
        console.error('Server error:', error.response.status, serverError);
      } else if (error.request) {
        // Request was made but no response received
        errorMessage = 'Backend server is not responding. Please ensure the backend is running on port 8001.';
        console.error('No response from server');
      }
      
      setToast({ message: errorMessage, type: 'error' });
      return false;
    }
  };

  const removeItemFromCart = async (cartItemId) => {
    try {
      const response = await removeFromCart(cartItemId);
      if (response.data.success) {
        await loadCart();
        return true;
      }
      setToast({ message: response.data?.message || 'Could not remove item', type: 'error' });
      return false;
    } catch (error) {
      console.error('Error removing from cart:', error);
      setToast({ message: 'Could not remove item. Please try again.', type: 'error' });
      return false;
    }
  };

  const updateItemQuantity = async (cartItemId, quantity) => {
    try {
      const response = await updateCartItem({
        cart_item_id: cartItemId,
        quantity: quantity,
      });
      if (response.data.success) {
        await loadCart();
        return true;
      }
      setToast({ message: response.data?.message || 'Could not update quantity', type: 'error' });
      return false;
    } catch (error) {
      console.error('Error updating cart:', error);
      setToast({ message: 'Could not update quantity. Please try again.', type: 'error' });
      return false;
    }
  };

  const getCartTotal = () => {
    return cartItems.reduce((total, item) => total + (item.price ?? 0) * (item.quantity ?? 1), 0);
  };

  const getCartItemCount = () => {
    return cartItems.reduce((count, item) => count + (item.quantity ?? 1), 0);
  };

  const value = {
    cartItems,
    loading,
    addItemToCart,
    removeItemFromCart,
    updateItemQuantity,
    loadCart,
    getCartTotal,
    getCartItemCount,
    userId,
    shippingInfo,
    setShippingInfo,
  };

  return (
    <CartContext.Provider value={value}>
      {children}
      {toast && (
        <Toast
          message={toast.message}
          type={toast.type}
          onClose={() => setToast(null)}
        />
      )}
    </CartContext.Provider>
  );
};
