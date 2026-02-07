import { useState, useEffect } from 'react';
import { useCart } from '../context/CartContext';
import { createOrder, getStripeConfig } from '../api/api';
import { useNavigate } from 'react-router-dom';
import Toast from '../components/Toast';
import CardPaymentForm from '../components/CardPaymentForm';
import './Checkout.css';

const CARD_PAYMENT_METHODS = ['card', 'visa'];

const Checkout = () => {
  const { cartItems, getCartTotal, userId, loadCart, shippingInfo, setShippingInfo } = useCart();
  const [shippingAddress, setShippingAddress] = useState('');
  const [loading, setLoading] = useState(false);
  const [toast, setToast] = useState(null);
  const [stripePk, setStripePk] = useState('');
  const navigate = useNavigate();

  const isCardPayment = CARD_PAYMENT_METHODS.includes(shippingInfo.paymentMethod);

  useEffect(() => {
    getStripeConfig().then((r) => {
      if (r.data?.publishableKey) setStripePk(r.data.publishableKey);
    }).catch(() => {});
  }, []);

  const placeOrder = async (paymentIntentId = null) => {
    if (!shippingAddress.trim()) {
      setToast({ message: 'Please enter a shipping address', type: 'error' });
      return;
    }
    if (!isCardPayment && !shippingInfo.paymentMethod) {
      setToast({ message: 'Please select a payment method', type: 'error' });
      return;
    }
    if (cartItems.length === 0) {
      setToast({ message: 'Your cart is empty', type: 'error' });
      navigate('/cart');
      return;
    }
    try {
      setLoading(true);
      const payload = {
        user_id: userId,
        shipping_address: shippingAddress.trim(),
        customer_name: shippingInfo.customerName || '',
        phone: shippingInfo.phone || '',
        payment_method: shippingInfo.paymentMethod || '',
      };
      if (paymentIntentId) payload.payment_intent_id = paymentIntentId;
      const response = await createOrder(payload);
      if (response.data.success) {
        setToast({ message: 'Order placed successfully! ✓', type: 'success' });
        await loadCart();
        setTimeout(() => navigate('/'), 1500);
      } else {
        setToast({ message: response.data?.message || 'Failed to place order', type: 'error' });
      }
    } catch (error) {
      console.error('Error placing order:', error);
      const msg = error.response?.data?.message || 'Failed to place order. Please try again.';
      setToast({ message: msg, type: 'error' });
    } finally {
      setLoading(false);
    }
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (isCardPayment) return;
    await placeOrder();
  };

  const handleCardPaymentSuccess = (paymentIntentId) => {
    placeOrder(paymentIntentId);
  };

  if (cartItems.length === 0 && !loading) {
    return (
      <div className="checkout-page">
        <div className="empty-cart">
          <h2>Your cart is empty</h2>
          <button onClick={() => navigate('/cart')} className="back-btn">
            Go to Cart
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="checkout-page">
      <h1>Checkout</h1>
      <div className="checkout-content">
        <div className="checkout-form-section">
          <h2>Shipping Information</h2>
          <form onSubmit={handleSubmit}>
            <div className="form-group">
              <label htmlFor="name">Full Name</label>
              <input
                id="name"
                type="text"
                value={shippingInfo.customerName}
                onChange={(e) => setShippingInfo({ ...shippingInfo, customerName: e.target.value })}
                placeholder="Your name"
              />
            </div>
            <div className="form-group">
              <label htmlFor="phone">Phone Number</label>
              <input
                id="phone"
                type="tel"
                value={shippingInfo.phone}
                onChange={(e) => setShippingInfo({ ...shippingInfo, phone: e.target.value })}
                placeholder="Your phone number"
              />
            </div>
            <div className="form-group">
              <label htmlFor="address">Shipping Address</label>
              <textarea
                id="address"
                value={shippingAddress}
                onChange={(e) => setShippingAddress(e.target.value)}
                placeholder="Enter your full shipping address"
                required
                rows="4"
              />
            </div>
            <div className="form-group">
              <label htmlFor="payment">Payment Method</label>
              <select
                id="payment"
                value={shippingInfo.paymentMethod}
                onChange={(e) => setShippingInfo({ ...shippingInfo, paymentMethod: e.target.value })}
              >
                <option value="">Select payment method</option>
                <option value="card">Credit / Debit Card (Visa, Mastercard)</option>
                <option value="visa">Visa (secure via Stripe)</option>
                <option value="cod">Cash on Delivery</option>
                <option value="paypal">PayPal</option>
                <option value="bank">Bank Transfer</option>
              </select>
            </div>
            {isCardPayment && (
              <CardPaymentForm
                publishableKey={stripePk}
                amountCents={Math.max(50, Math.round(getCartTotal() * 100))}
                onSuccess={handleCardPaymentSuccess}
                onError={(msg) => setToast({ message: msg, type: 'error' })}
                disabled={loading}
              />
            )}
            {!isCardPayment && (
              <button type="submit" className="place-order-btn" disabled={loading}>
                {loading ? 'Placing Order...' : 'Place Order'}
              </button>
            )}
          </form>
        </div>
        <div className="order-summary-section">
          <h2>Order Summary</h2>
          <div className="order-items">
            {(cartItems || []).map((item) => (
              <div key={item.id} className="order-item">
                <span>{item.product_name || 'Product'} x{item.quantity ?? 1}</span>
                <span>${((item.price ?? 0) * (item.quantity ?? 1)).toFixed(2)}</span>
              </div>
            ))}
          </div>
          <div className="order-total">
            <div className="total-row">
              <span>Subtotal:</span>
              <span>${(getCartTotal() || 0).toFixed(2)}</span>
            </div>
            <div className="total-row">
              <span>Shipping:</span>
              <span>Free</span>
            </div>
            <div className="total-row final-total">
              <span>Total:</span>
              <span>${(getCartTotal() || 0).toFixed(2)}</span>
            </div>
          </div>
        </div>
      </div>
      {toast && (
        <Toast message={toast.message} type={toast.type} onClose={() => setToast(null)} />
      )}
    </div>
  );
};

export default Checkout;
