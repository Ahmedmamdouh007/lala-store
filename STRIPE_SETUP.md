# Stripe Payment Setup Guide

## Quick Setup

To enable Visa/Mastercard payments, you need to add your Stripe test keys to `backend/config/stripe_config.json`.

### Step 1: Get Your Stripe Test Keys

1. Go to [Stripe Dashboard](https://dashboard.stripe.com/test/apikeys)
2. Sign up or log in (free account)
3. Copy your **Publishable key** (starts with `pk_test_`)
4. Copy your **Secret key** (starts with `sk_test_`)

### Step 2: Update Configuration

Edit `backend/config/stripe_config.json` and replace the placeholder keys:

```json
{
  "secret_key": "sk_test_YOUR_ACTUAL_SECRET_KEY_HERE",
  "publishable_key": "pk_test_YOUR_ACTUAL_PUBLISHABLE_KEY_HERE"
}
```

### Step 3: Restart Backend Server

After updating the config file, restart your backend server for changes to take effect.

## Alternative: Use Environment Variables

You can also set Stripe keys using environment variables:

**Windows PowerShell:**
```powershell
$env:STRIPE_SECRET_KEY="sk_test_your_key_here"
$env:STRIPE_PUBLISHABLE_KEY="pk_test_your_key_here"
```

**Linux/Mac:**
```bash
export STRIPE_SECRET_KEY="sk_test_your_key_here"
export STRIPE_PUBLISHABLE_KEY="pk_test_your_key_here"
```

## Testing

- Use Stripe test card numbers: `4242 4242 4242 4242`
- Use any future expiry date (e.g., 12/34)
- Use any 3-digit CVC
- Use any ZIP code

## Note

- Test keys only work in test mode (no real charges)
- For production, use live keys from [Stripe Dashboard](https://dashboard.stripe.com/apikeys)
- If you don't want to set up Stripe, use "Cash on Delivery" payment method
