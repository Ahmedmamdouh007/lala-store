import { useState, useEffect } from 'react';
import { getProductsByGender, getAllProducts } from '../api/api';
import { fallbackWomenProducts } from '../data/fallbackProducts';
import ProductCard from '../components/ProductCard';
import './Women.css';

const Women = () => {
  const [products, setProducts] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [usingFallback, setUsingFallback] = useState(false);

  useEffect(() => {
    loadProducts();
  }, []);

  const loadProducts = async () => {
    try {
      setLoading(true);
      setError(null);
      setUsingFallback(false);
      let data;
      try {
        const response = await getProductsByGender('women');
        data = response?.data;
      } catch {
        const fallback = await getAllProducts().catch(() => null);
        if (fallback?.data?.success && Array.isArray(fallback?.data?.data)) {
          data = { success: true, data: fallback.data.data.filter((p) => (p.gender || '').toLowerCase() === 'women') };
        }
      }
      if (data?.success && Array.isArray(data?.data) && data.data.length > 0) {
        setProducts(data.data);
      } else if (data?.data?.length) {
        setProducts(data.data);
      } else {
        setProducts(fallbackWomenProducts);
        setUsingFallback(true);
      }
    } catch (err) {
      console.error('Error loading women products:', err);
      setProducts(fallbackWomenProducts);
      setUsingFallback(true);
      setError('Unable to load women products from the server.');
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return (
      <div className="women-page">
        <div className="page-header">
          <h1>Women's Collection</h1>
          <p>Discover our latest women's fashion</p>
        </div>
        <div className="loading">Loading...</div>
      </div>
    );
  }

  return (
    <div className="women-page">
      <div className="page-header">
        <h1>Women's Collection</h1>
        <p>Discover our latest women's fashion</p>
      </div>
      {usingFallback && (
        <p className="fallback-notice">Showing demo products (backend offline). Start the backend for live data.</p>
      )}
      {error && products.length === 0 ? (
        <p className="no-products">
          {error}{' '}
          <button type="button" className="retry-btn" onClick={loadProducts}>
            Retry
          </button>
        </p>
      ) : products.length === 0 ? (
        <p className="no-products">
          No women's products available at the moment.{` `}
          <button type="button" className="retry-btn" onClick={loadProducts}>
            Retry
          </button>
        </p>
      ) : (
        <div className="products-grid">
          {products.map((product) => (
            <ProductCard key={product.id} product={product} />
          ))}
        </div>
      )}
    </div>
  );
};

export default Women;
