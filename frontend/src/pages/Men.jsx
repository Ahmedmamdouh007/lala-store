import { useState, useEffect } from 'react';
import { getProductsByGender, getAllProducts } from '../api/api';
import { fallbackMenProducts } from '../data/fallbackProducts';
import ProductCard from '../components/ProductCard';
import './Men.css';

const Men = () => {
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
        const response = await getProductsByGender('men');
        data = response?.data;
      } catch {
        const fallback = await getAllProducts().catch(() => null);
        if (fallback?.data?.success && Array.isArray(fallback?.data?.data)) {
          data = { success: true, data: fallback.data.data.filter((p) => (p.gender || '').toLowerCase() === 'men') };
        }
      }
      if (data?.success && Array.isArray(data?.data) && data.data.length > 0) {
        setProducts(data.data);
      } else if (data?.data?.length) {
        setProducts(data.data);
      } else {
        setProducts(fallbackMenProducts);
        setUsingFallback(true);
      }
    } catch (err) {
      console.error('Error loading men products:', err);
      setProducts(fallbackMenProducts);
      setUsingFallback(true);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return (
      <div className="men-page">
        <div className="page-header">
          <h1>Men's Collection</h1>
          <p>Discover our latest men's fashion</p>
        </div>
        <div className="loading-skeleton">
          {[...Array(8)].map((_, i) => (
            <div key={i} className="skeleton-card">
              <div className="skeleton-image" />
              <div className="skeleton-text" />
              <div className="skeleton-text short" />
            </div>
          ))}
        </div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="men-page">
        <div className="page-header">
          <h1>Men's Collection</h1>
          <p>Discover our latest men's fashion</p>
        </div>
        <div className="loading">
          <p className="no-products">{error}</p>
          <button className="btn" onClick={loadProducts} style={{ marginTop: '1rem' }}>Retry</button>
        </div>
      </div>
    );
  }

  return (
    <div className="men-page">
      <div className="page-header">
        <h1>Men's Collection</h1>
        <p>Discover our latest men's fashion</p>
      </div>
      {usingFallback && (
        <p className="fallback-notice">Showing demo products (backend offline). Start the backend for live data.</p>
      )}
      {products.length === 0 ? (
        <p className="no-products">No men's products available at the moment.</p>
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

export default Men;
