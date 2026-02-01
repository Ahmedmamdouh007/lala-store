import { useState, useEffect } from 'react';
import { useParams, Link } from 'react-router-dom';
import { getProductById } from '../api/api';
import { useCart } from '../context/CartContext';
import './ProductDetails.css';

const ProductDetails = () => {
  const { id } = useParams();
  const [product, setProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  const [quantity, setQuantity] = useState(1);
  const { addItemToCart } = useCart();

  useEffect(() => {
    loadProduct();
  }, [id]);

  const loadProduct = async () => {
    try {
      setLoading(true);
      const response = await getProductById(id);
      if (response.data.success) {
        setProduct(response.data.data);
      }
    } catch (error) {
      console.error('Error loading product:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleAddToCart = async () => {
    const success = await addItemToCart(product.id, quantity);
    if (success) {
      alert('Product added to cart!');
    } else {
      alert('Failed to add product to cart');
    }
  };

  if (loading) {
    return <div className="loading">Loading product...</div>;
  }

  if (!product) {
    return (
      <div className="product-not-found">
        <h2>Product not found</h2>
        <Link to="/">Go back to home</Link>
      </div>
    );
  }

  return (
    <div className="product-details-page">
      <div className="product-details-content">
        <div className="product-image-section">
          <img
            src={product.image_url || '/placeholder-image.jpg'}
            alt={product.name}
            onError={(e) => {
              e.target.src = 'https://via.placeholder.com/500x600?text=No+Image';
            }}
          />
        </div>
        <div className="product-info-section">
          <h1>{product.name}</h1>
          <p className="product-category">{product.category_name || 'Uncategorized'}</p>
          <p className="product-price">${product.price.toFixed(2)}</p>
          <p className="product-description">{product.description}</p>
          <div className="product-meta">
            <p><strong>Gender:</strong> {product.gender}</p>
            <p><strong>Stock:</strong> {product.stock_quantity} available</p>
          </div>
          <div className="product-actions">
            <div className="quantity-selector">
              <label>Quantity:</label>
              <button onClick={() => setQuantity(Math.max(1, quantity - 1))}>-</button>
              <span>{quantity}</span>
              <button onClick={() => setQuantity(Math.min(product.stock_quantity, quantity + 1))}>+</button>
            </div>
            <button
              className="add-to-cart-btn-large"
              onClick={handleAddToCart}
              disabled={product.stock_quantity === 0}
            >
              {product.stock_quantity === 0 ? 'Out of Stock' : 'Add to Cart'}
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ProductDetails;
