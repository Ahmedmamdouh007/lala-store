import { useState } from 'react';
import { useCart } from '../context/CartContext';
import CartItem from '../components/CartItem';
import { Link } from 'react-router-dom';
import './Cart.css';

const Cart = () => {
  const { cartItems, loading, getCartTotal, shippingInfo, setShippingInfo } = useCart();
  const [showShipping, setShowShipping] = useState(false);

  if (loading) {
    return <div className="loading">Loading cart...</div>;
  }

  return (
    <div className="cart-page">
      <h1>Shopping Cart</h1>
      {cartItems.length === 0 ? (
        <div className="empty-cart">
          <p>Your cart is empty</p>
          <Link to="/" className="continue-shopping-btn">
            Continue Shopping
          </Link>
        </div>
      ) : (
        <div className="cart-content">
          <div className="cart-main">
            <div className="cart-items">
              {(cartItems || []).map((item) => (
                <CartItem key={item.id} item={item} />
              ))}
            </div>
            <div className="shipping-info-section">
              <button
                type="button"
                className="shipping-toggle"
                onClick={() => setShowShipping(!showShipping)}
              >
                {showShipping ? '−' : '+'} Shipping Information
              </button>
              {showShipping && (
                <div className="shipping-form">
                  <div className="form-row">
                    <label>Full Name</label>
                    <input
                      type="text"
                      placeholder="Your name"
                      value={shippingInfo.customerName}
                      onChange={(e) => setShippingInfo({ ...shippingInfo, customerName: e.target.value })}
                    />
                  </div>
                  <div className="form-row">
                    <label>Phone Number</label>
                    <input
                      type="tel"
                      placeholder="Your phone"
                      value={shippingInfo.phone}
                      onChange={(e) => setShippingInfo({ ...shippingInfo, phone: e.target.value })}
                    />
                  </div>
                  <div className="form-row">
                    <label>Payment Method</label>
                    <select
                      value={shippingInfo.paymentMethod}
                      onChange={(e) => setShippingInfo({ ...shippingInfo, paymentMethod: e.target.value })}
                    >
                      <option value="">Select payment</option>
                      <option value="card">Credit / Debit Card (Visa, Mastercard)</option>
                      <option value="visa">Visa (secure)</option>
                      <option value="cod">Cash on Delivery</option>
                      <option value="paypal">PayPal</option>
                      <option value="bank">Bank Transfer</option>
                    </select>
                  </div>
                </div>
              )}
            </div>
          </div>
          <div className="cart-summary">
            <h2>Order Summary</h2>
            <div className="summary-row">
              <span>Subtotal:</span>
              <span>${(getCartTotal() || 0).toFixed(2)}</span>
            </div>
            <div className="summary-row">
              <span>Shipping:</span>
              <span>Free</span>
            </div>
            <div className="summary-row total">
              <span>Total:</span>
              <span>${(getCartTotal() || 0).toFixed(2)}</span>
            </div>
            <Link to="/checkout" className="checkout-btn">
              Proceed to Checkout
            </Link>
          </div>
        </div>
      )}
    </div>
  );
};

export default Cart;
