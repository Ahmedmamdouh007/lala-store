import { useState, useEffect } from 'react';
import { getProductsByGender, getAllProducts } from '../api/api';
import ProductCard from '../components/ProductCard';
import './Women.css';

const Women = () => {
  const [products, setProducts] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    loadProducts();
  }, []);

  const loadProducts = async () => {
    try {
      setLoading(true);
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
      if (data?.success && Array.isArray(data?.data)) {
        setProducts(data.data);
      }
    } catch (error) {
      console.error('Error loading women products:', error);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <div className="loading">Loading...</div>;
  }

  return (
    <div className="women-page">
      <div className="page-header">
        <h1>Women's Collection</h1>
        <p>Discover our latest women's fashion</p>
      </div>
      {products.length === 0 ? (
        <p className="no-products">No women's products available at the moment.</p>
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
