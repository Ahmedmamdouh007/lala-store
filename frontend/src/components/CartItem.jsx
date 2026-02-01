import { useCart } from '../context/CartContext';
import './CartItem.css';

const CartItem = ({ item }) => {
  const { updateItemQuantity, removeItemFromCart } = useCart();

  const handleQuantityChange = async (newQuantity) => {
    if (newQuantity <= 0) {
      await removeItemFromCart(item.id);
    } else {
      await updateItemQuantity(item.id, newQuantity);
    }
  };

  const handleRemove = async () => {
    await removeItemFromCart(item.id);
  };

  return (
    <div className="cart-item">
      <div className="cart-item-image">
        <img
          src={item.product_image || '/placeholder-image.jpg'}
          alt={item.product_name}
          onError={(e) => {
            e.target.src = 'https://via.placeholder.com/100x100?text=No+Image';
          }}
        />
      </div>
      <div className="cart-item-details">
        <h3>{item.product_name}</h3>
        <p className="cart-item-price">${item.price.toFixed(2)} each</p>
      </div>
      <div className="cart-item-controls">
        <div className="quantity-controls">
          <button
            onClick={() => handleQuantityChange(item.quantity - 1)}
            className="quantity-btn"
          >
            -
          </button>
          <span className="quantity">{item.quantity}</span>
          <button
            onClick={() => handleQuantityChange(item.quantity + 1)}
            className="quantity-btn"
          >
            +
          </button>
        </div>
        <div className="cart-item-total">
          ${(item.price * item.quantity).toFixed(2)}
        </div>
        <button onClick={handleRemove} className="remove-btn">
          Remove
        </button>
      </div>
    </div>
  );
};

export default CartItem;
