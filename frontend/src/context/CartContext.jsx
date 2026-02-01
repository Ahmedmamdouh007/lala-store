import { createContext, useContext, useState, useEffect } from 'react';
import { getCartItems, addToCart, removeFromCart, updateCartItem } from '../api/api';

const CartContext = createContext();

export const useCart = () => {
  const context = useContext(CartContext);
  if (!context) {
    throw new Error('useCart must be used within a CartProvider');
  }
  return context;
};

export const CartProvider = ({ children }) => {
  const [cartItems, setCartItems] = useState([]);
  const [loading, setLoading] = useState(false);
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

  const addItemToCart = async (productId, quantity = 1) => {
    try {
      const response = await addToCart({
        user_id: userId,
        product_id: productId,
        quantity: quantity,
      });
      if (response.data.success) {
        await loadCart();
        return true;
      }
      return false;
    } catch (error) {
      console.error('Error adding to cart:', error);
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
      return false;
    } catch (error) {
      console.error('Error removing from cart:', error);
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
      return false;
    } catch (error) {
      console.error('Error updating cart:', error);
      return false;
    }
  };

  const getCartTotal = () => {
    return cartItems.reduce((total, item) => total + item.price * item.quantity, 0);
  };

  const getCartItemCount = () => {
    return cartItems.reduce((count, item) => count + item.quantity, 0);
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
  };

  return <CartContext.Provider value={value}>{children}</CartContext.Provider>;
};
