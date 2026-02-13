import { useState, useEffect } from 'react';
import { useParams, Link } from 'react-router-dom';
import { getProductById } from '../api/api';
import { useCart } from '../context/CartContext';
import { fallbackProducts, fallbackMenProducts, fallbackWomenProducts } from '../data/fallbackProducts';
import './ProductDetails.css';

const ProductDetails = () => {
  const { id } = useParams();
  const [product, setProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  const [quantity, setQuantity] = useState(1);
  const [selectedSize, setSelectedSize] = useState('');
  const [usingFallback, setUsingFallback] = useState(false);
  const { addItemToCart } = useCart();

  useEffect(() => {
    loadProduct();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [id]);

  const loadProduct = async () => {
    try {
      setLoading(true);
      setUsingFallback(false);
      setSelectedSize('');

      const allFallback = [
        ...fallbackProducts,
        ...fallbackMenProducts,
        ...fallbackWomenProducts,
      ];
      const numericId = Number(id);

      // Try backend first
      try {
        const response = await getProductById(id);
        if (response.data?.success && response.data.data) {
          const p = response.data.data;
          setProduct(p);
          if (p.sizes) {
            const firstSize = p.sizes.split(',')[0]?.trim() || '';
            setSelectedSize(firstSize);
          }
          return;
        }
      } catch (error) {
        console.error('Error loading product from backend:', error);
      }

      // Fallback to local demo data (when backend is offline or product missing)
      const local = allFallback.find((p) => p.id === numericId);
      if (local) {
        setProduct(local);
        if (local.sizes) {
          const firstSize = local.sizes.split(',')[0]?.trim() || '';
          setSelectedSize(firstSize);
        }
        setUsingFallback(true);
      } else {
        setProduct(null);
      }
    } finally {
      setLoading(false);
    }
  };

  const handleAddToCart = async () => {
    if (!product?.id) return;
    if (product.sizes && !selectedSize) {
      alert('Please select a size');
      return;
    }
    if ((product.stock_quantity ?? 0) === 0) return;
    await addItemToCart(product.id, quantity, selectedSize);
  };

  const parseSizeChart = (chartStr) => {
    if (!chartStr) return [];
    return chartStr.split(';').map(part => {
      const colon = part.indexOf(':');
      if (colon === -1) return null;
      const label = part.slice(0, colon).trim();
      const values = part.slice(colon + 1).split(',').map(v => v.trim());
      return { label, values };
    }).filter(Boolean);
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
          <p className="product-price">${(product.price ?? 0).toFixed(2)}</p>
          <p className="product-description">{product.description}</p>
          {usingFallback && (
            <p className="fallback-notice">
              Showing demo product (backend offline). Start the backend for live data.
            </p>
          )}
          <div className="product-meta">
            <p><strong>Gender:</strong> {product.gender}</p>
            <p><strong>Stock:</strong> {product.stock_quantity} available</p>
          </div>
          {product.sizes && (
            <div className="size-selector-section">
              <label className="size-label">Select Size:</label>
              <div className="size-buttons">
                {product.sizes.split(',').map(s => {
                  const size = s.trim();
                  return (
                    <button
                      key={size}
                      type="button"
                      className={`size-btn ${selectedSize === size ? 'active' : ''}`}
                      onClick={() => setSelectedSize(size)}
                    >
                      {size}
                    </button>
                  );
                })}
              </div>
            </div>
          )}
          {product.size_chart && (
            <div className="size-chart-section">
              <details className="size-chart-details">
                <summary>Size Chart</summary>
                <table className="size-chart-table">
                  <thead>
                    <tr>
                      <th>Size</th>
                      {product.sizes && product.sizes.split(',').map(s => (
                        <th key={s.trim()}>{s.trim()}</th>
                      ))}
                    </tr>
                  </thead>
                  <tbody>
                    {parseSizeChart(product.size_chart).map((row, i) => (
                      <tr key={i}>
                        <td>{row.label}</td>
                        {row.values.map((v, j) => <td key={j}>{v}</td>)}
                      </tr>
                    ))}
                  </tbody>
                </table>
              </details>
            </div>
          )}
          <div className="product-actions">
            <div className="quantity-selector">
              <label>Quantity:</label>
              <button onClick={() => setQuantity(Math.max(1, quantity - 1))}>-</button>
              <span>{quantity}</span>
              <button onClick={() => setQuantity(Math.min(product.stock_quantity ?? 999, quantity + 1))}>+</button>
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
