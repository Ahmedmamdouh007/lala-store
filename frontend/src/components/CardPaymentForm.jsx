import { useState, useEffect } from 'react';
import { loadStripe } from '@stripe/stripe-js';
import { Elements, CardElement, useStripe, useElements } from '@stripe/react-stripe-js';
import { createPaymentIntent } from '../api/api';
import './CardPaymentForm.css';

const CARD_ELEMENT_OPTIONS = {
  style: {
    base: {
      color: '#e8e4dc',
      fontFamily: 'inherit',
      fontSize: '16px',
      '::placeholder': { color: '#9a9590' },
      iconColor: '#d6c7a1',
    },
    invalid: {
      color: '#e57373',
      iconColor: '#e57373',
    },
  },
};

function CardPaymentFormInner({ amountCents, onSuccess, onError, disabled }) {
  const stripe = useStripe();
  const elements = useElements();
  const [loading, setLoading] = useState(false);

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (!stripe || !elements) return;
    setLoading(true);
    try {
      const { data } = await createPaymentIntent(amountCents, 'usd');
      if (!data.success || !data.client_secret) {
        throw new Error(data.message || 'Could not create payment session');
      }
      const cardElement = elements.getElement(CardElement);
      const { error, paymentIntent } = await stripe.confirmCardPayment(data.client_secret, {
        payment_method: { card: cardElement },
      });
      if (error) {
        onError(error.message || 'Payment failed');
      } else if (paymentIntent?.status === 'succeeded') {
        onSuccess(paymentIntent.id);
      } else {
        onError('Payment was not completed');
      }
    } catch (err) {
      const msg = err?.response?.data?.message || err?.message || 'Payment failed';
      onError(msg);
    } finally {
      setLoading(false);
    }
  };

  return (
    <form onSubmit={handleSubmit} className="card-payment-form">
      <div className="card-element-wrapper">
        <CardElement options={CARD_ELEMENT_OPTIONS} />
      </div>
      <p className="card-payment-note">Secure payment by Stripe. Your card data is never stored.</p>
      <button type="submit" className="pay-with-card-btn" disabled={!stripe || loading || disabled}>
        {loading ? 'Processing...' : 'Pay with Card'}
      </button>
    </form>
  );
}

export default function CardPaymentForm({ publishableKey, amountCents, onSuccess, onError, disabled }) {
  const [stripePromise, setStripePromise] = useState(null);

  useEffect(() => {
    if (publishableKey && publishableKey.startsWith('pk_')) {
      setStripePromise(loadStripe(publishableKey));
    } else {
      setStripePromise(null);
    }
  }, [publishableKey]);

  if (!publishableKey || !publishableKey.startsWith('pk_') || publishableKey.includes('YOUR_')) {
    return (
      <div className="card-payment-placeholder">
        <p>Card payment (Visa/Mastercard) requires Stripe setup.</p>
        <p>Add your keys to <code>backend/config/stripe_config.json</code> or use Cash on Delivery.</p>
      </div>
    );
  }

  return (
    <Elements stripe={stripePromise}>
      <CardPaymentFormInner
        amountCents={amountCents}
        onSuccess={onSuccess}
        onError={onError}
        disabled={disabled}
      />
    </Elements>
  );
}
